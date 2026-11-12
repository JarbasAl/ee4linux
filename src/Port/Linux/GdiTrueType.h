// Taken from ee-macos (https://github.com/maxgraf96/ee-macos, MIT licence, Copyright (c) 2026 Max Graf),
// kit-src/runtime/gdi32_truetype.h at 136d78f. Adapted for the native Linux build: the namespace is eeport::gdi,
// there is no bundled stand-in face, and the font database reads Linux font folders. The licence text is in
// third_party/NOTICE.md.
//
// TrueType faces for GDI text, measured and rasterized with stb_truetype. A face comes from a font the program
// registered from memory (AddFontMemResourceEx), else from the installed faces (GdiFontDB.cpp): the Windows face
// itself when a Windows font folder has it, else a face with the same metrics (Liberation for Arial, Times New
// Roman and Courier New; Wine's Tahoma).
//
// How a face is MEASURED follows GDI rather than the font's own vertical
// metrics: truetype_realize turns a LOGFONT into the whole-pixel em (ppem)
// GDI hints at, takes the ascent and descent from the VDMX table when the font
// has a record for that size (else the OS/2 Windows ascent and descent), and
// the advances from the hdmx device widths when the font has a record for
// that size (else the linear advance, rounded). That is what makes text
// widths, line heights and glyph-atlas layouts come out as they do on Windows.
// What it cannot do is run a font's hinting program, so at a size the hdmx
// table leaves out, a glyph whose hinting moves its advance (below the LTSH
// threshold) may differ by a pixel.
#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace eeport {
namespace gdi {
struct TrueTypeFace;
struct TrueTypeGlyph {
    int32_t w = 0, h = 0, x = 0, y = 0; // offset from the pen on the baseline
    std::vector<uint8_t> coverage;
};
// Registers every font in `data` and returns how many could be read.
uint32_t truetype_add_memory(const uint8_t *data, size_t size);
// The registered face with this family name, compared without case, or null.
const TrueTypeFace *truetype_find(const std::string &family);

// ---------------------------------------------------------------------------
// The platform's installed fonts (gdi32_fontdb.cpp).
// ---------------------------------------------------------------------------
// The installed face GDI's mapper would pick for this face name, weight and
// italic flag: the family (name ID 1, or the typographic family, ID 16), else
// a face whose full name (ID 4) is the name; then among the family's faces the
// nearest weight and the requested slant. `synth_bold`/`synth_italic` say
// when GDI would simulate what the family lacks. Null when nothing installed
// matches. The directories are scanned once, reading only table directories
// and the name, OS/2 and head tables; a chosen file is then read whole.
const TrueTypeFace *truetype_system_face(const std::string &name, int32_t weight, bool italic,
                                         bool *synth_bold, bool *synth_italic);
// For tests: search these directories ('\n'-separated) instead of the
// platform's, and forget what was scanned. An empty string restores them.
void truetype_set_font_dirs_for_test(const std::string &dirs);
// Builds a face from file bytes handed over (the font database's files).
// Null when stb_truetype cannot read the face at `index`.
const TrueTypeFace *truetype_adopt_file(std::vector<uint8_t> &&bytes, int index);

// ---------------------------------------------------------------------------
// GDI realization (gdi32_truetype.cpp).
// ---------------------------------------------------------------------------
// The parts of a LOGFONT realization depends on.
struct GdiLogFont {
    int32_t height = 0, width = 0, weight = 0;
    bool italic = false, underline = false, strikeout = false;
    uint8_t charset = 0, quality = 0;
};
// A face realized at a size, as GDI holds it in a DC, with its TEXTMETRICW.
struct GdiFont {
    const TrueTypeFace *face = nullptr;
    int32_t ppem = 0;           // the em in whole pixels, the size GDI hints at
    double sx = 0, sy = 0;      // font units to pixels
    bool device_widths = false; // hdmx has a record for this ppem
    bool bold_sim = false, italic_sim = false, bilevel = false;
    // TEXTMETRICW, in pixels.
    int32_t height = 0, ascent = 0, descent = 0, internal = 0, external = 0;
    int32_t ave_width = 0, max_width = 0, weight = 400, overhang = 0;
    uint16_t first = 0, last = 0, def = 0, brk = 0;
    uint8_t italic = 0, underlined = 0, struck = 0, pitch_family = 0, charset = 0;
    // Distance below the baseline (positive down) and thickness, in pixels.
    int32_t underline_pos = 0, underline_size = 1, strike_pos = 0, strike_size = 1;
};
GdiFont truetype_realize(const TrueTypeFace *face, const GdiLogFont &lf, bool synth_bold,
                         bool synth_italic);
// The integer advance GDI gives this character (GetCharWidth, text extents).
int32_t truetype_gdi_advance(const GdiFont &font, uint32_t codepoint);
// The glyph's coverage as drawn at this realization; cached, valid until the
// next call.
const TrueTypeGlyph &truetype_gdi_glyph(const GdiFont &font, uint32_t codepoint);
// The face's family name as the font spells it (GetTextFace).
const std::string &truetype_family_name(const TrueTypeFace *face);
} // namespace gdi
} // namespace eeport
