"""Compile helpers: stage a unit under the original build path (C:\\EEX) and compile it with the pinned VC6."""
import json
import os
import pathlib
import re
import struct
import subprocess
import sys
import time
import capstone
import pefile
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from ee import ANALYSIS, BUILD, COMPILER, EEX_INCLUDE_DIRS, ORIG_SRC_ROOT, ROOT, TOOLCHAIN, module_path
DEFINES = "-DWIN32 -DNDEBUG -D_WINDOWS"
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)


def loose(name):
    """Normalise a symbol for comparison: drop __imp_, C underscore/@N decoration; keep C++ names."""
    imp = name.startswith("__imp_")
    if imp:
        name = name[6:]
    if name.startswith("__real@"):
        return ("imp:" if imp else "") + name
    if not name.startswith("?"):
        name = re.sub(r"@\d+$", "", name)
        name = name.lstrip("_")
        if name[:1] == "@":
            name = name[1:]
    return ("imp:" if imp else "") + name


def xref_exclusions(mod="exe"):
    """{field rva: target rva} of analysis/<mod>/xref_noreloc_exclude.txt (`<field rva> <target rva> # reason`):
    committed false positives of tools/xref_noreloc.py (an immediate that looks like an address). A reader drops a
    generated field only when its target matches (a stale line is ignored). exeimg shares exe's image and list."""
    ex = ANALYSIS / ("exe" if mod == "exeimg" else mod) / "xref_noreloc_exclude.txt"
    out = {}
    if ex.exists():
        for ln in open(ex):
            ln = ln.split("#", 1)[0].split()
            if len(ln) >= 2:
                out[int(ln[0], 16)] = int(ln[1], 16)
    return out


class Original:
    def __init__(self, mod):
        self.mod = mod
        self.pe = pefile.PE(str(module_path(mod)))
        oh = self.pe.OPTIONAL_HEADER
        self.base, self.size = oh.ImageBase, oh.SizeOfImage
        self.names = {}
        self.funcs = {}
        self.has_reloc = bool(oh.DATA_DIRECTORY[5].Size)
        self.relocs = set()
        if self.has_reloc:
            for blk in self.pe.DIRECTORY_ENTRY_BASERELOC:
                for e in blk.entries:
                    if e.type == 3:
                        self.relocs.add(e.rva)
        else:
            xf = BUILD / mod / "xref_noreloc.txt"
            if xf.exists():
                self.relocs = {int(ln.split()[0], 16) for ln in open(xf) if not ln.startswith("#")}
                self.has_reloc = True
                excl = xref_exclusions(mod)
                if excl:
                    for ln in open(xf):
                        if not ln.startswith("#"):
                            f, t = (int(x, 16) for x in ln.split()[:2])
                            if excl.get(f) == t:
                                self.relocs.discard(f)
            else:
                print(f"verify: {mod} has no relocations and no {xf.relative_to(ROOT)}; run "
                      f"tools/xref_noreloc.py {mod}", file=sys.stderr)
        self.import_slots = {}
        for e in getattr(self.pe, "DIRECTORY_ENTRY_IMPORT", []):
            for i in e.imports:
                nm = i.name.decode() if i.name else f"{e.dll.decode()}#{i.ordinal}"
                self.names[i.address - self.base] = "__imp_" + nm
                self.import_slots[nm] = i.address - self.base
        self.export_aliases = {}
        if hasattr(self.pe, "DIRECTORY_ENTRY_EXPORT"):
            for s in self.pe.DIRECTORY_ENTRY_EXPORT.symbols:
                if s.name:
                    self.names[s.address] = s.name.decode()
                    self.export_aliases[loose(s.name.decode())] = s.address
        inv = ANALYSIS / mod / "functions.jsonl"
        if inv.exists():
            for line in open(inv):
                f = json.loads(line)
                self.funcs[f["rva"]] = f
        bf = ANALYSIS / mod / "boundaries.txt"
        if bf.exists():
            for line in open(bf):
                line = line.split("#", 1)[0].strip()
                if line:
                    rva, size = line.split()[:2]
                    rva, size = int(rva, 16), int(size, 0)
                    if size == 0:
                        self.funcs.pop(rva, None)
                        continue
                    self.funcs[rva] = {"rva": rva, "name": f"FUN_{rva:05x}", "size": size,
                                       "ranges": [[rva, rva + size]]}
        self.aliases = {}
        self.all_rvas = {}
        nf = pathlib.Path(os.environ["EE_NAMES"]) if os.environ.get("EE_NAMES") else ANALYSIS / mod / "names.txt"
        if nf.exists():
            for line in open(nf):
                line = line.split("#", 1)[0].strip()
                if line:
                    rva, sym = line.split(None, 1)
                    rva, sym = int(rva, 16), sym.strip()
                    self.names.setdefault(rva, sym)
                    self.aliases[loose(sym)] = rva
                    self.all_rvas.setdefault(loose(sym), set()).add(rva)

    def rva_of(self, name):
        if name.startswith("__imp__"):
            for n in (name[7:], re.sub(r"@\d+$", "", name[7:])):
                if n in self.import_slots:
                    return self.import_slots[n]
        if not hasattr(self, "_by_name"):
            self._by_name = {loose(n): r for r, n in self.names.items()}
            self._by_name.update(self.export_aliases)
            self._by_name.update(self.aliases)
        return self._by_name.get(loose(name))

    def rvas_of(self, name):
        """Every address names.txt gives this name (several for per-unit statics in multi-unit modules)."""
        found = set(self.all_rvas.get(loose(name), ()))
        r = self.rva_of(name)
        if r is not None:
            found.add(r)
        return found

    def name_of(self, rva):
        if rva in self.names:
            return self.names[rva]
        return f"FUN_{rva:05x}" if rva in self.funcs else f"DAT_{rva:05x}"

    def bytes_at(self, rva, n):
        return self.pe.get_data(rva, n)

    def extent(self, rva, size=None):
        if size is None:
            f = self.funcs.get(rva)
            if not f:
                raise SystemExit(f"no function at {rva:#x} in analysis/{self.mod}/functions.jsonl (pass --size)")
            size = next((e - s for s, e in f["ranges"] if s == rva), f["size"])
        end = rva + size
        while self.has_reloc and end in self.relocs:
            tgt = struct.unpack("<I", self.bytes_at(end, 4))[0] - self.base
            if not rva <= tgt < end:
                break
            end += 4
        return rva, end

    def instructions(self, rva, end):
        """Yield (offset, raw bytes, masked bytes, refs) with refs = tuple of symbolic targets."""
        code = self.bytes_at(rva, end - rva)
        abs_fields = {r - rva for r in self.relocs if rva <= r < end} if self.has_reloc else None
        tail = []
        if abs_fields is not None:
            self_refs = {f: ("self", struct.unpack_from("<I", code, f)[0] - self.base - rva) for f in abs_fields
                         if f + 4 <= len(code)}
            self_refs = {f: r for f, r in self_refs.items() if 0 <= r[1] < len(code)}
            data_from = min((r[1] for f, r in self_refs.items() if f < r[1]), default=len(code))
            tail = list(data_items(code, data_from, self_refs))
            code = code[:data_from]
        for ins in disasm(code):
            off, raw = ins[0], bytearray(ins[1])
            refs = []
            fields = []
            if abs_fields is not None:
                fields = [f - off for f in sorted(abs_fields) if off <= f and f + 4 <= off + len(raw)]
            else:
                for k in range(len(raw) - 3):
                    v = struct.unpack_from("<I", raw, k)[0]
                    if self.base <= v < self.base + self.size and k >= 1:
                        fields.append(k)
            for k in fields:
                tgt = struct.unpack_from("<I", raw, k)[0] - self.base
                refs.append(self.ref(tgt, rva, end))
                raw[k:k + 4] = b"\0\0\0\0"
            rel = rel32_field(ins[1])
            if rel is not None:
                tgt = rva + off + len(raw) + struct.unpack_from("<i", raw, rel)[0]
                if not rva <= tgt < end or (ins[1][0] == 0xE8 and tgt == rva):
                    refs.append(("sym", self.name_of(tgt), tgt) if tgt == rva else self.ref(tgt, rva, end))
                    raw[rel:rel + 4] = b"\0\0\0\0"
            yield off, ins[1], bytes(raw), tuple(refs)
        yield from tail

    def ref(self, tgt, rva, end):
        if rva <= tgt < end:
            return ("self", tgt - rva)
        return ("sym", self.name_of(tgt), tgt)


def rel32_field(b):
    if b[0] in (0xE8, 0xE9) and len(b) == 5:
        return 1
    if len(b) == 6 and b[0] == 0x0F and 0x80 <= b[1] <= 0x8F:
        return 2
    return None


def data_items(code, data_from, self_refs):
    """The switch tables VC6 puts after a function's code, as data items instead of decoded instructions: a dword
    per address field (self_refs: offset -> ("self", target offset)), a byte for anything else (index tables)."""
    off = data_from
    while off < len(code):
        if off in self_refs and off + 4 <= len(code):
            yield off, code[off:off + 4], b"\0\0\0\0", (self_refs[off],)
            off += 4
        else:
            yield off, code[off:off + 1], code[off:off + 1], ()
            off += 1


def disasm(code):
    off = 0
    while off < len(code):
        ins = next(MD.disasm(code[off:off + 16], off), None)
        n = ins.size if ins else 1
        yield off, code[off:off + n]
        off += n


def stage_source(src):
    """Units under src/ are compiled from a copy at the original build path (C:\\EEX\\...), because __FILE__
    (used by ESVERIFY and friends) is the command-line path verbatim. Returns the path to hand to cl."""
    src = pathlib.Path(src).resolve()
    try:
        rel = src.relative_to(ROOT / "src")
    except ValueError:
        return src
    staged = ORIG_SRC_ROOT / rel
    staged.parent.mkdir(parents=True, exist_ok=True)
    for f in [src, *sorted(src.parent.glob("*.h"))]:
        _stage_write(staged.parent / f.name, f.read_bytes())
    return staged


def _stage_write(dst, data):
    for i in range(120):
        try:
            if not dst.exists() or dst.read_bytes() != data:
                dst.write_bytes(data)
            return
        except PermissionError:
            time.sleep(0.25)
    if not dst.exists() or dst.read_bytes() != data:
        dst.write_bytes(data)


def include_paths(src=None):
    """(/I flags, INCLUDE env) for every compile. Stages the headers of EEX_INCLUDE_DIRS under ORIG_SRC_ROOT first;
    the /I paths are drive-less like the original's (the compile's cwd must be on the same drive)."""
    flags = []
    for d in EEX_INCLUDE_DIRS:
        dst = ORIG_SRC_ROOT / d
        dst.mkdir(parents=True, exist_ok=True)
        for f in sorted((ROOT / "src" / d).glob("*.h")):
            _stage_write(dst / f.name, f.read_bytes())
        flags.append("/I\\eex\\" + d.replace("/", "\\"))
    pkg = TOOLCHAIN / COMPILER
    incdir = pathlib.Path(os.environ.get("EE_INCLUDE", str(ROOT / "include")))
    dx7 = pathlib.Path(os.environ.get("EE_DX7SDK", str(TOOLCHAIN / "dx7sdk" / "include")))
    use_dx7 = src is not None and ("DX7 Display Plug-ins" in str(src) or "dx7sdk" in unit_includes(src)) \
        and dx7.exists()
    inc = os.pathsep.join([os.environ.get("EE_DINKUM", str(incdir / "dinkum")), str(incdir),
                           *([str(dx7)] if use_dx7 else []),
                           str(pkg / "Include"), str(ROOT / "include" / "winsdk")])
    return flags, inc


UNIT_FLAGS = re.compile(r"^// UNIT-FLAGS: (.+?)\s*$", re.M)
UNIT_INCLUDE = re.compile(r"^// UNIT-INCLUDE: (.+?)\s*$", re.M)
UNIT_INCLUDE_KNOWN = {"dx7sdk"}


def unit_includes(src):
    """Extra include sets a unit opts into in its first 30 lines: `// UNIT-INCLUDE: dx7sdk` (space-separated names
    from UNIT_INCLUDE_KNOWN; an unknown name is an error, not silently ignored)."""
    try:
        head = "".join(pathlib.Path(src).read_text(errors="replace").splitlines(keepends=True)[:30])
    except OSError:
        return set()
    names = {n for m in UNIT_INCLUDE.finditer(head) for n in m.group(1).split()}
    unknown = names - UNIT_INCLUDE_KNOWN
    if unknown:
        raise SystemExit(f"{src}: unknown // UNIT-INCLUDE: {' '.join(sorted(unknown))}")
    return names


def unit_flags(src, flags):
    """A unit compiled with its own flag set (a per-file setting in the original .dsp) says so in its first lines:
    `// UNIT-FLAGS: -Os -Oy -Ob1 -Gs -Gf -Gy -G6 -MD`. It replaces the module's flags for that unit only."""
    try:
        head = "".join(pathlib.Path(src).read_text(errors="replace").splitlines(keepends=True)[:30])
    except OSError:
        return flags
    m = UNIT_FLAGS.search(head)
    return m.group(1) if m else flags


def compile_candidate(src, flags, workdir):
    flags = unit_flags(src, flags)
    src = stage_source(src)
    pkg = TOOLCHAIN / COMPILER
    iflags, inc = include_paths(src)
    env = dict(os.environ, PATH=str(pkg / "Bin") + os.pathsep + os.environ["PATH"], INCLUDE=inc)
    obj = pathlib.Path(workdir) / "cand.obj"
    cmd = [str(pkg / "Bin" / "CL.EXE"), "/nologo", "/c", *flags.split(), *DEFINES.split(), *iflags, f"/Fo{obj}",
           str(src)]
    r = subprocess.run(cmd, cwd=workdir, env=env, capture_output=True, text=True,
                       creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
    out = (r.stdout + r.stderr).strip()
    return (obj if r.returncode == 0 and obj.exists() else None), out
