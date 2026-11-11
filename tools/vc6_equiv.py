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


def coff_sections(data):
    """Section name -> raw bytes plus its relocations, and the symbol table, of a COFF object, without debug data."""
    machine, count, _stamp, symptr, nsyms, opt, _chars = struct.unpack_from("<HHIIIHH", data)
    strtab = symptr + nsyms * 18
    sections = {}
    for i in range(count):
        off = 20 + opt + i * 40
        raw_name = data[off:off + 8].rstrip(b"\0")
        name = raw_name.decode("latin-1")
        if name.startswith("/"):
            start = strtab + int(name[1:])
            name = data[start:data.index(b"\0", start)].decode("latin-1")
        size, ptr, relptr, _ln, nrel, _nln, chars = struct.unpack_from("<IIIIHHI", data, off + 16)
        if name.startswith(".debug"):
            continue
        sections[f"{i}:{name}"] = (chars, data[ptr:ptr + size] if ptr else b"", data[relptr:relptr + nrel * 10])
    symbols = data[symptr:strtab]
    strings = data[strtab:]
    return machine, sections, symbols, strings


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
    diff = sorted(set(a[1]) ^ set(b[1])) + [n for n in a[1] if n in b[1] and a[1][n] != b[1][n]]
    if a[2:] != b[2:]:
        diff.append("symbol table")
    return "code differs: " + ", ".join(diff[:12]) + (f" (+{len(diff) - 12} more)" if len(diff) > 12 else "")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("modules", nargs="*", default=list(MODULE_SRC))
    ap.add_argument("--base", default="upstream/fixes", help="revision to compare against (default upstream/fixes)")
    ap.add_argument("-j", type=int, default=2)
    args = ap.parse_args()
    changed = changed_files(args.base)
    cache, units, skipped = {}, [], []
    for mod in args.modules:
        for src in module_sources(mod):
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


if __name__ == "__main__":
    sys.exit(main())
