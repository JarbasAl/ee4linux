// Taken from ee-macos (https://github.com/maxgraf96/ee-macos, MIT licence, Copyright (c) 2026 Max Graf),
// kit-src/runtime/gdi32_fontdb.cpp at 136d78f. Adapted for the native Linux build: POSIX file calls, the Linux and
// Wine font folders, and metric-compatible substitutes for Windows faces a Linux system lacks. The licence text is
// in third_party/NOTICE.md.
//
// The installed fonts, as GDI's font mapper would see them. A game names a face ("Arial", "Tahoma") and Windows
// answers from its own font folder. Here the folders are searched in this order: EE_PORT_FONTS (a colon-separated
// list), the Wine prefix's Windows font folder (where winetricks installs the Microsoft core fonts, with the tables
// GDI measures with), the user's and the system's font folders, and Wine's own fonts (its Tahoma).
//
// The folders are scanned once, lazily, reading only each file's table directory and its name, OS/2 and head
// tables; a face is read whole only when it is first chosen. Matching follows GDI: the family name (name ID 1,
// Windows English record) first, then the typographic family (ID 16), then a face whose full name (ID 4) is the
// requested name ("Arial Bold"); among a family's faces, the requested slant and the nearest weight, with bold and
// italic simulated when the family has no such face. A name that matches nothing goes through Windows' own
// FontSubstitutes, then to a face with the same metrics: Liberation Sans for Arial, Liberation Serif for Times New
// Roman, Liberation Mono for Courier New, and DejaVu when Liberation is missing.
#include "GdiTrueType.h"
#include "Win32Internal.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace eeport {
namespace gdi {
namespace {

struct Entry {
    std::string path;
    int index = 0;
    std::string family, typo, full; // lower case
    int weight = 400;
    bool italic = false;
};

std::string g_dirs_override;
bool g_scanned = false;
std::vector<Entry> &entries() {
    static std::vector<Entry> list;
    return list;
}
std::map<std::pair<std::string, int>, const TrueTypeFace *> &loaded() {
    static std::map<std::pair<std::string, int>, const TrueTypeFace *> m;
    return m;
}

std::string lower_ascii(std::string s) {
    for (char &ch : s)
        if (ch >= 'A' && ch <= 'Z')
            ch = char(ch - 'A' + 'a');
    return s;
}
bool ends_with(const std::string &s, const char *tail) {
    const size_t n = strlen(tail);
    return s.size() >= n && lower_ascii(s.substr(s.size() - n)) == tail;
}

// Reads exactly `n` bytes at `offset`, or fails.
bool read_at(int fd, uint64_t offset, void *buf, size_t n) {
    uint8_t *p = static_cast<uint8_t *>(buf);
    while (n) {
        const ssize_t got = pread(fd, p, n, off_t(offset));
        if (got <= 0)
            return false;
        p += got;
        offset += uint64_t(got);
        n -= size_t(got);
    }
    return true;
}
uint16_t be16(const uint8_t *p) {
    return uint16_t((p[0] << 8) | p[1]);
}
uint32_t be32(const uint8_t *p) {
    return (uint32_t(be16(p)) << 16) | be16(p + 2);
}

// One name from a name table: the Windows Unicode English record when there
// is one (what GDI matches), else any Windows Unicode record, else Mac Roman.
std::string name_of(const std::vector<uint8_t> &t, uint16_t id) {
    if (t.size() < 6)
        return {};
    const uint16_t count = be16(&t[2]), strings = be16(&t[4]);
    int best = -1, best_rank = 0;
    for (uint16_t i = 0; i < count; ++i) {
        const size_t r = 6 + 12 * size_t(i);
        if (r + 12 > t.size())
            break;
        const uint16_t platform = be16(&t[r]), encoding = be16(&t[r + 2]),
                       language = be16(&t[r + 4]), name = be16(&t[r + 6]);
        if (name != id)
            continue;
        int rank = 0;
        if (platform == 3 && (encoding == 1 || encoding == 0))
            rank = language == 0x409 ? 3 : 2;
        else if (platform == 1 && encoding == 0 && language == 0)
            rank = 1;
        if (rank > best_rank) {
            best_rank = rank;
            best = i;
        }
    }
    if (best < 0)
        return {};
    const size_t r = 6 + 12 * size_t(best);
    const uint16_t platform = be16(&t[r]), length = be16(&t[r + 8]), offset = be16(&t[r + 10]);
    const size_t at = size_t(strings) + offset;
    if (at + length > t.size())
        return {};
    std::string out;
    if (platform == 3) {
        for (size_t k = 0; k + 1 < length; k += 2) {
            const uint16_t unit = be16(&t[at + k]);
            out.push_back(unit < 128 ? char(unit) : '?');
        }
    } else
        out.assign(reinterpret_cast<const char *>(&t[at]), length);
    return lower_ascii(out);
}

// The faces in one file: a TrueType or OpenType font, or a collection.
void scan_file(const std::string &path) {
    const int fd = open(path.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return;
    uint8_t head[12];
    std::vector<uint32_t> offsets;
    if (read_at(fd, 0, head, 12)) {
        if (be32(head) == 0x74746366 /* 'ttcf' */) {
            const uint32_t n = std::min<uint32_t>(be32(head + 8), 64);
            std::vector<uint8_t> list(4 * size_t(n));
            if (n && read_at(fd, 12, list.data(), list.size()))
                for (uint32_t i = 0; i < n; ++i)
                    offsets.push_back(be32(&list[4 * i]));
        } else
            offsets.push_back(0);
    }
    for (size_t index = 0; index < offsets.size(); ++index) {
        uint8_t sfnt[12];
        if (!read_at(fd, offsets[index], sfnt, 12))
            continue;
        const uint32_t version = be32(sfnt);
        if (version != 0x00010000 && version != 0x74727565 /* 'true' */ &&
            version != 0x4f54544f /* 'OTTO' */)
            continue;
        const uint16_t tables = std::min<uint16_t>(be16(sfnt + 4), 256);
        std::vector<uint8_t> dir(16 * size_t(tables));
        if (!read_at(fd, offsets[index] + 12, dir.data(), dir.size()))
            continue;
        uint32_t name_at = 0, name_size = 0, os2_at = 0, os2_size = 0, head_at = 0;
        for (uint16_t t = 0; t < tables; ++t) {
            const uint8_t *e = &dir[16 * size_t(t)];
            const uint32_t at = be32(e + 8), size = be32(e + 12);
            if (!memcmp(e, "name", 4))
                name_at = at, name_size = size;
            else if (!memcmp(e, "OS/2", 4))
                os2_at = at, os2_size = size;
            else if (!memcmp(e, "head", 4) && size >= 54)
                head_at = at;
        }
        if (!name_at || !name_size || name_size > (1u << 20))
            continue;
        std::vector<uint8_t> names(name_size);
        if (!read_at(fd, name_at, names.data(), names.size()))
            continue;
        Entry e;
        e.path = path;
        e.index = int(index);
        e.family = name_of(names, 1);
        e.typo = name_of(names, 16);
        e.full = name_of(names, 4);
        if (e.family.empty())
            continue;
        uint8_t os2[64];
        if (os2_at && os2_size >= 64 && read_at(fd, os2_at, os2, 64)) {
            e.weight = be16(os2 + 4) ? be16(os2 + 4) : 400;
            e.italic = (be16(os2 + 62) & 1) != 0;
        }
        uint8_t hd[54];
        if (head_at && read_at(fd, head_at, hd, 54) && (be16(hd + 44) & 2))
            e.italic = true;
        entries().push_back(std::move(e));
    }
    close(fd);
}

void scan_dir(const std::string &dir, int depth) {
    std::vector<std::string> names;
    if (DIR *d = opendir(dir.c_str())) {
        while (const dirent *entry = readdir(d))
            names.push_back(entry->d_name);
        closedir(d);
    }
    std::sort(names.begin(), names.end()); // the same faces in the same order on every run
    for (const std::string &name : names) {
        if (name.empty() || name[0] == '.')
            continue;
        const std::string path = dir + "/" + name;
        struct stat st;
        if (stat(path.c_str(), &st) != 0)
            continue;
        if (S_ISDIR(st.st_mode)) {
            if (depth < 4)
                scan_dir(path, depth + 1);
        } else if (S_ISREG(st.st_mode) &&
                   (ends_with(name, ".ttf") || ends_with(name, ".ttc") || ends_with(name, ".otf")))
            scan_file(path);
    }
}

// The font folders, one per line, in the order the header gives
std::string font_dirs() {
    std::string dirs;
    auto add = [&](const std::string &dir) { dirs += dir + "\n"; };
    if (const char *own = getenv("EE_PORT_FONTS")) {
        std::string list = own;
        for (size_t start = 0, end; start <= list.size(); start = end + 1) {
            end = std::min(list.find(':', start), list.size());
            if (end > start)
                add(list.substr(start, end - start));
        }
    }
    const char *home = getenv("HOME");
    const char *prefix = getenv("WINEPREFIX");
    if (prefix && *prefix)
        add(std::string(prefix) + "/drive_c/windows/Fonts");
    else if (home)
        add(std::string(home) + "/.wine/drive_c/windows/Fonts");
    const char *data = getenv("XDG_DATA_HOME");
    if (data && *data)
        add(std::string(data) + "/fonts");
    else if (home)
        add(std::string(home) + "/.local/share/fonts");
    if (home)
        add(std::string(home) + "/.fonts");
    add("/usr/local/share/fonts");
    add("/usr/share/fonts");
    add("/usr/share/wine/fonts");
    return dirs;
}

void scan() {
    if (g_scanned)
        return;
    g_scanned = true;
    std::string dirs = g_dirs_override;
    if (dirs.empty())
        dirs = font_dirs();
    size_t start = 0;
    while (start <= dirs.size()) {
        size_t end = dirs.find('\n', start);
        if (end == std::string::npos)
            end = dirs.size();
        if (end > start)
            scan_dir(dirs.substr(start, end - start), 0);
        start = end + 1;
    }
}

// The face of an entry, read whole on first use.
const TrueTypeFace *load(const Entry &e) {
    auto key = std::make_pair(e.path, e.index);
    auto it = loaded().find(key);
    if (it != loaded().end())
        return it->second;
    const TrueTypeFace *face = nullptr;
    struct stat st;
    if (stat(e.path.c_str(), &st) == 0 && st.st_size > 0 && st.st_size <= (64 << 20)) {
        std::vector<uint8_t> bytes(size_t(st.st_size));
        const int fd = open(e.path.c_str(), O_RDONLY | O_CLOEXEC);
        if (fd >= 0) {
            if (read_at(fd, 0, bytes.data(), bytes.size()))
                face = truetype_adopt_file(std::move(bytes), e.index);
            close(fd);
        }
    }
    if (!face)
        Log("the font file %s cannot be read", e.path.c_str());
    loaded()[key] = face;
    return face;
}

// Windows' FontSubstitutes and the font mapper's aliases for names no
// installation has as a family, then the faces with the same metrics that a
// Linux system has in place of the Windows ones.
const char *substitute(const std::string &name) {
    static const struct {
        const char *from, *to;
    } kAliases[] = {
        {"ms shell dlg", "microsoft sans serif"},
        {"ms shell dlg 2", "tahoma"},
        {"ms sans serif", "microsoft sans serif"},
        {"helv", "microsoft sans serif"},
        {"helvetica", "arial"},
        {"arial cyr", "arial"},
        {"tms rmn", "times new roman"},
        {"times", "times new roman"},
        {"ms serif", "times new roman"},
        {"courier", "courier new"},
        {"arial", "liberation sans"},
        {"times new roman", "liberation serif"},
        {"courier new", "liberation mono"},
        {"microsoft sans serif", "tahoma"},
        {"tahoma", "dejavu sans"},
        {"verdana", "dejavu sans"},
        {"liberation sans", "dejavu sans"},
        {"liberation serif", "dejavu serif"},
        {"liberation mono", "dejavu sans mono"},
    };
    for (const auto &a : kAliases)
        if (name == a.from)
            return a.to;
    return nullptr;
}

} // namespace

void truetype_set_font_dirs_for_test(const std::string &dirs) {
    g_dirs_override = dirs;
    g_scanned = false;
    entries().clear();
    loaded().clear();
}

const TrueTypeFace *truetype_system_face(const std::string &name, int32_t weight, bool italic,
                                         bool *synth_bold, bool *synth_italic) {
    if (synth_bold)
        *synth_bold = false;
    if (synth_italic)
        *synth_italic = false;
    scan();
    std::string key = lower_ascii(name);
    const int want = weight > 0 ? weight : 400;
    for (int round = 0; round < 5; ++round) {
        // The family (ID 1); failing that, the typographic family (ID 16).
        std::vector<const Entry *> family;
        for (const Entry &e : entries())
            if (e.family == key)
                family.push_back(&e);
        if (family.empty())
            for (const Entry &e : entries())
                if (e.typo == key)
                    family.push_back(&e);
        if (!family.empty()) {
            const Entry *best = nullptr;
            int best_cost = 0;
            for (const Entry *e : family) {
                const int cost = (e->italic != italic ? 10000 : 0) + std::abs(e->weight - want);
                if (!best || cost < best_cost) {
                    best = e;
                    best_cost = cost;
                }
            }
            if (synth_bold)
                *synth_bold = want >= 550 && best->weight < 550;
            if (synth_italic)
                *synth_italic = italic && !best->italic;
            return load(*best);
        }
        // A face's full name names exactly that face.
        for (const Entry &e : entries())
            if (e.full == key) {
                if (synth_italic)
                    *synth_italic = italic && !e.italic;
                return load(e);
            }
        const char *alias = substitute(key);
        if (!alias)
            break;
        key = alias;
    }
    return nullptr;
}
} // namespace gdi
} // namespace eeport
