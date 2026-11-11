"""Check that source edits leave the VC6 build's code unchanged (python3 -I tools/vc6_equiv.py --help).

  python3 -I tools/vc6_equiv.py [--base REV] [-j N] [module ...]

Port edits must not change what VC6 compiles. This script finds every unit that a changed file reaches (the unit
itself, or a header it includes directly or through other headers), compiles it with the pinned VC6 under Wine twice
- with the base revision's sources and with the working tree's - and compares the two objects: every section except
the debug sections, and the symbol table. Both compiles run in one staging folder (build/vc6-equiv/stage) with the same
relative paths, because VC6's output depends on the length of the paths it reads.

It needs toolchain/msvc6.0 (and msvc6.4 for libjpeg): python -I tools/fetch_toolchain.py 6.0 6.4. Units that need the
DirectX 7 SDK headers are listed as skipped when toolchain/dx7sdk is missing.
"""
import argparse
import concurrent.futures
import os
import pathlib
import re
import shlex
import struct
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import verify
from ee import EEX_INCLUDE_DIRS, MODULE_FLAGS, MODULE_SRC, ROOT, module_sources

WORK = ROOT / "build" / "vc6-equiv"
STAGE = WORK / "stage"
INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.M)
MODULE_COMPILER = {"jpeg": "msvc6.4"}
UNIT_COMPILER = re.compile(r"^// UNIT-COMPILER: (\S+)", re.M)


def include_dirs(src):
    dirs = [ROOT / "include" / "dinkum", ROOT / "include", *(ROOT / "src" / d for d in EEX_INCLUDE_DIRS),
            ROOT / "include" / "winsdk"]
    if "DX7 Display Plug-ins" in str(src) or "dx7sdk" in verify.unit_includes(src):
        dirs.insert(2, ROOT / "toolchain" / "dx7sdk" / "include")
    return dirs


def closure(src, cache):
    """The repo files a unit reaches through quoted includes."""
    seen, todo = set(), [src.resolve()]
    while todo:
        path = todo.pop()
        if path in seen:
            continue
        seen.add(path)
        if path not in cache:
            try:
                names = INCLUDE.findall(path.read_text(errors="replace"))
            except OSError:
                names = []
            found = []
            for name in names:
                for base in [path.parent, *include_dirs(src)]:
                    candidate = (base / name).resolve()
                    if candidate.is_file():
                        found.append(candidate)
                        break
            cache[path] = found
        todo += cache[path]
    return seen


def changed_files(base):
    names = subprocess.run(["git", "diff", "--name-only", base], cwd=ROOT, capture_output=True, text=True,
                           check=True).stdout.split()
    return {(ROOT / n).resolve() for n in names}


def coff_parse(data):
    """The sections (without debug sections) and the symbol records of a COFF object."""
    _machine, count, _stamp, symptr, nsyms, opt, _chars = struct.unpack_from("<HHIIIHH", data)
    strtab = symptr + nsyms * 18

    def text(raw):
        if raw[:4] == b"\0\0\0\0":
            start = strtab + struct.unpack_from("<I", raw, 4)[0]
            return data[start:data.index(b"\0", start)].decode("latin-1")
        return raw[:8].rstrip(b"\0").decode("latin-1")

    def symbol_name(raw):
        # VC6 names an anonymous namespace after the source file and the compile time: drop the time
        # and a named local label ($name$12345) takes a number from the label counter: drop the number
        return NAMED_LABEL.sub(r"\1", ANONYMOUS.sub(r"\1", text(raw)))

    sections = []
    for i in range(count):
        off = 20 + opt + i * 40
        name = data[off:off + 8].rstrip(b"\0").decode("latin-1")
        if name.startswith("/"):
            start = strtab + int(name[1:])
            name = data[start:data.index(b"\0", start)].decode("latin-1")
        size, ptr, relptr, _ln, nrel, _nln, chars = struct.unpack_from("<IIIIHHI", data, off + 16)
        relocs = [struct.unpack_from("<IIH", data, relptr + r * 10) for r in range(nrel)]
        raw = ANONYMOUS_BYTES.sub(rb"\1", data[ptr:ptr + size]) if ptr else b""   # RTTI names hold it too
        sections.append((name, chars, raw, relocs))
    symbols, i = {}, 0
    while i < nsyms:
        entry = data[symptr + i * 18:symptr + i * 18 + 18]
        value, section, kind, storage, naux = struct.unpack_from("<IhHBB", entry, 8)
        symbols[i] = (symbol_name(entry), value, section, kind, storage, data[symptr + (i + 1) * 18:symptr + (i + 1 + naux) * 18])
        i += 1 + naux
    return sections, symbols


NUMBERED = re.compile(r"^\$[LT]\d+$")
ANONYMOUS = re.compile(r"(\?%[^@]*?\.cp?p?)\d+")
NAMED_LABEL = re.compile(r"^(\$\w+\$)\d+$")
ANONYMOUS_BYTES = re.compile(rb"(\?%[^@\0]*?\.cp?p?)\d+")


def numbered(name):
    return bool(NUMBERED.match(name))


def coff_sections(data):
    """The object's code and data, independent of the order VC6 emits sections in: each section is keyed by the
    COMDAT symbol it defines (or by its name and rank), and each relocation names its target instead of its index.
    Local labels ($L...) and compiler temporaries ($T..., the exception-handling tables) are compared by position
    only: their numbers come from counters that any edit can shift."""
    sections, symbols = coff_parse(data)
    owner = {}
    for name, value, section, kind, storage, _aux in symbols.values():
        if section > 0 and storage in (2, 3) and not numbered(name) and not name.startswith(".") \
                and section not in owner:
            owner[section] = name
    rank, keys = {}, {}
    for number, (name, *_rest) in enumerate(sections, 1):
        if number in owner:
            keys[number] = f"{name}:{owner[number]}"
        else:
            rank[name] = rank.get(name, 0) + 1
            keys[number] = f"{name}#{rank[name]}"

    def target(index):
        name, value, section, _kind, storage, _aux = symbols[index]
        if numbered(name):
            return ("numbered", name[:2], keys.get(section), value)
        if storage == 3 and name.startswith(".") and section > 0:
            return ("section", keys[section], value)
        return ("symbol", name)

    content = {}
    for number, (name, chars, raw, relocs) in enumerate(sections, 1):
        if name.startswith(".debug"):
            continue
        content[keys[number]] = (chars, raw, sorted((vaddr, target(index), kind) for vaddr, index, kind in relocs))
    externals = sorted((name, str(keys.get(section, section)), value, kind, storage)
                       for name, value, section, kind, storage, _aux in symbols.values()
                       if storage == 2 and not numbered(name))
    return content, externals


def compile_vc6(tree, rel, mod, out):
    src = tree / rel
    flags = verify.unit_flags(src, MODULE_FLAGS[mod])
    flags = [f for f in shlex.split(verify.DEFINES) + shlex.split(flags) if f not in ("-Zi", "/Zi")]
    head = "".join(src.read_text(errors="replace").splitlines(keepends=True)[:30])
    match = UNIT_COMPILER.search(head)
    compiler = match.group(1) if match else MODULE_COMPILER.get(mod, "msvc6.0")
    win = lambda p: str(p).replace("/", "\\")
    incs = [f"/I{win(d.relative_to(ROOT))}" for d in include_dirs(src)]
    incs.insert(2 + ("dx7sdk" in " ".join(incs)), f"/I{win(pathlib.Path('toolchain') / compiler / 'Include')}")
    out.parent.mkdir(parents=True, exist_ok=True)
    cmd = ["wine", str(ROOT / "toolchain" / compiler / "Bin" / "CL.EXE"), "/nologo", "/c", *flags, *incs,
           f"/Fo{win(os.path.relpath(out, tree))}", win(rel)]
    env = dict(os.environ, WINEDEBUG="-all")
    run = subprocess.run(cmd, cwd=tree, capture_output=True, text=True, errors="replace", env=env)
    return run.returncode == 0 and out.exists(), run.stdout + run.stderr


def stage(tree):
    """Copy a tree's sources to the one staging folder: VC6's output depends on the length of the paths it reads."""
    STAGE.mkdir(parents=True, exist_ok=True)
    for part in ("src", "include"):
        subprocess.run(["rsync", "-a", "--delete", f"{tree / part}/", f"{STAGE / part}/"], check=True)
    link = STAGE / "toolchain"
    if not link.exists():
        link.symlink_to(ROOT / "toolchain")


def compile_all(label, units, jobs):
    def one(job):
        mod, src = job
        rel = src.resolve().relative_to(ROOT)
        out = WORK / "obj" / label / mod / rel.with_suffix(".obj")
        out.unlink(missing_ok=True)
        ok, log = compile_vc6(STAGE, rel, mod, out)
        return (mod, rel), (out.read_bytes() if ok else None, log)
    with concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        return dict(pool.map(one, units))


def compare(base, head):
    if base is None or head is None:
        return "does not compile"
    a, b = coff_sections(base), coff_sections(head)
    if a == b:
        return None
    diff = sorted(set(a[0]) ^ set(b[0])) + [n for n in a[0] if n in b[0] and a[0][n] != b[0][n]]
    if a[1] != b[1]:
        diff.append("external symbols")
    return "code differs: " + ", ".join(diff[:8]) + (f" (+{len(diff) - 8} more)" if len(diff) > 8 else "")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("modules", nargs="*", default=list(MODULE_SRC))
    ap.add_argument("--base", default="upstream/fixes", help="revision to compare against (default upstream/fixes)")
    ap.add_argument("-j", type=int, default=2)
    ap.add_argument("--only", help="check only units whose path contains this text")
    ap.add_argument("--recompare", action="store_true", help="compare the objects of the last run again, no compiles")
    args = ap.parse_args()
    changed = changed_files(args.base)
    cache, units, skipped = {}, [], []
    for mod in args.modules:
        for src in module_sources(mod):
            if args.only and args.only not in str(src):
                continue
            if closure(src, cache) & changed:
                if any("dx7sdk" in str(d) for d in include_dirs(src)) and not (ROOT / "toolchain" / "dx7sdk").exists():
                    skipped.append(src.relative_to(ROOT))
                else:
                    units.append((mod, src))
    print(f"{len(changed)} changed files reach {len(units)} units ({len(skipped)} skipped: no DirectX 7 SDK)")
    for rel in skipped:
        print(f"  skipped {rel}")
    if not units:
        return 0

    if args.recompare:
        objects = {}
        for label in ("base", "head"):
            objects[label] = {}
            for mod, src in units:
                out = WORK / "obj" / label / mod / src.resolve().relative_to(ROOT).with_suffix(".obj")
                objects[label][(mod, src.resolve().relative_to(ROOT))] = (out.read_bytes() if out.exists() else None, "")
        base, head = objects["base"], objects["head"]
    else:
        base, head = run_compiles(args, units)
    failures = 0
    for key in base:
        problem = compare(base[key][0], head[key][0])
        if problem:
            failures += 1
            mod, rel = key
            log = (base[key][1] if base[key][0] is None else head[key][1]).strip()
            print(f"DIFFERENT {mod} {rel}: {problem}" + (f"\n{log}" if problem == "does not compile" else ""))
    print(f"{len(units)} units checked, {len(units) - failures} identical, {failures} different")
    return 1 if failures else 0


def run_compiles(args, units):
    base_tree = WORK / "base-tree"
    rev = subprocess.run(["git", "rev-parse", args.base], cwd=ROOT, capture_output=True, text=True,
                         check=True).stdout.strip()
    if base_tree.exists():
        subprocess.run(["git", "checkout", "-q", "--detach", rev], cwd=base_tree, check=True)
    else:
        subprocess.run(["git", "worktree", "add", "-q", "--detach", str(base_tree), rev], cwd=ROOT, check=True)

    stage(base_tree)
    base = compile_all("base", units, args.j)
    stage(ROOT)
    head = compile_all("head", units, args.j)
    return base, head


if __name__ == "__main__":
    sys.exit(main())
