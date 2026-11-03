"""Build driver: compile every unit of the selected modules with the pinned VC6 toolchain.

  python -I tools/build.py [--compile-only] [-j N] [--out DIR] [module ...]

Modules (default: all): tcp mixer default lle jpeg dx7hr dx7hrtnl exeimg exe. Units come from
analysis/<module>/units.txt (in link order), or every .cpp of the module's source folder when the module has no unit
list. Each unit is compiled with the module's flags (tools/ee.py MODULE_FLAGS), or its own `// UNIT-FLAGS:` line,
with the compiler package its `// UNIT-COMPILER:` line names (default VC6 RTM, toolchain/msvc6.0).

Sources are staged under C:\\EEX (env EE_ORIG_SRC_ROOT overrides it; it must sit at the root of a drive), the
original build tree, so that __FILE__ strings match. Objects go to <out>/<module>/<unit path>.obj (default
build/obj). Exit status 1 if any unit fails to compile.

Linking (the DLLs and EE-AOC.exe) is not available in this driver yet; --compile-only is the only mode.
"""
import argparse
import concurrent.futures
import os
import pathlib
import re
import shutil
import sys
import tempfile
import time

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import verify
from ee import COMPILER, MODULE_FLAGS, ORIG_SRC_ROOT, ROOT, TOOLCHAIN, module_units

MODULES = ["tcp", "mixer", "default", "lle", "jpeg", "dx7hr", "dx7hrtnl", "exeimg", "exe"]
MODULE_COMPILER = {"jpeg": "msvc6.4"}
EXTRA_UNITS = {
    **{m: ["Low-Level Engine/Utilities/UFtol.cpp"] for m in ("tcp", "default", "dx7hr", "dx7hrtnl")},
    "lib": ["ThirdParty/Dinkum/newaop.cpp", "ThirdParty/Dinkum/delaop.cpp"],
}
EXTRA_FLAGS = {"lib": "-O1"}
UNIT_COMPILER = re.compile(r"^// UNIT-COMPILER: (\S+)", re.M)
ROOT_PATH_LEN = 23


def head(path, n=30):
    return "".join(pathlib.Path(path).read_text(errors="replace").splitlines(keepends=True)[:n])


def jobs_for(mod):
    """[(module, source path, flags, compiler package)] for one module."""
    flags = EXTRA_FLAGS.get(mod) or MODULE_FLAGS[mod]
    paths = [] if mod == "lib" else [p for _, _, p in module_units(mod)]
    paths += [ROOT / "src" / rel for rel in EXTRA_UNITS.get(mod, [])]
    out = []
    for p in paths:
        if not p.exists():
            sys.exit(f"{mod}: unit {p} does not exist")
        m = UNIT_COMPILER.search(head(p))
        out.append((mod, p, flags, m.group(1) if m else MODULE_COMPILER.get(mod, COMPILER)))
    return out


def obj_path(out, mod, src):
    rel = src.resolve().relative_to(verify.ROOT / "src")
    return out / mod / rel.with_suffix(".obj")


def compile_one(job, out, workroot):
    mod, src, flags, _ = job
    wd = tempfile.mkdtemp(dir=workroot)
    try:
        obj, log = verify.compile_candidate(src.resolve(), flags, wd)
        if obj:
            dst = obj_path(out, mod, src)
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(obj, dst)
        return job, obj is not None, log
    finally:
        shutil.rmtree(wd, ignore_errors=True)


def sync_tree(src, dst):
    """Make dst a copy of src (files compared by size and modification time)."""
    want = {p.relative_to(src): p for p in src.rglob("*") if p.is_file()} if src.exists() else {}
    have = {p.relative_to(dst): p for p in dst.rglob("*") if p.is_file()} if dst.exists() else {}
    for rel in have.keys() - want.keys():
        have[rel].unlink()
    for rel, f in want.items():
        g = dst / rel
        st = f.stat()
        if rel in have and g.stat().st_size == st.st_size and g.stat().st_mtime_ns == st.st_mtime_ns:
            continue
        g.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(f, g)


def canonical_root(pkgs):
    """VC6's code generation for some functions depends on the lengths of the paths it reads (its own directory,
    the include directories). Every build therefore compiles from a tree whose root path is ROOT_PATH_LEN characters
    long, the length of the reference build's root. Elsewhere src/, include/ and the toolchain packages in use are
    copied to <drive>:/EEX-root___... (padded to that length) and compiled from there. The drive is the staging
    root's, or the drive behind it when that is a subst drive (paths are resolved, which would leave the copy)."""
    if len(str(ROOT)) == ROOT_PATH_LEN and TOOLCHAIN == ROOT / "toolchain":
        return ROOT, TOOLCHAIN
    drive = pathlib.Path(ORIG_SRC_ROOT.drive + "\\").resolve().drive + "\\"
    dst = pathlib.Path(drive + "EEX-root".ljust(ROOT_PATH_LEN - len(drive), "_"))
    sync_tree(ROOT / "src", dst / "src")
    sync_tree(ROOT / "include", dst / "include")
    for name in [*pkgs, "dx7sdk"]:
        sync_tree(TOOLCHAIN / name, dst / "toolchain" / name)
    return dst, dst / "toolchain"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("modules", nargs="*", help=f"default: all ({' '.join(MODULES)})")
    ap.add_argument("--compile-only", action="store_true", help="compile every unit, do not link")
    ap.add_argument("-j", type=int, default=2, help="parallel compiles (default 2)")
    ap.add_argument("--out", default=str(ROOT / "build" / "obj"))
    a = ap.parse_args()
    mods = a.modules or MODULES
    for m in mods:
        if m not in MODULES:
            sys.exit(f"unknown module {m!r}; known: {' '.join(MODULES)}")
    if not a.compile_only:
        sys.exit("linking from source is not available yet: run with --compile-only")

    jobs = [j for m in mods for j in jobs_for(m)] + jobs_for("lib")
    for pkg in sorted({j[3] for j in jobs}):
        if not (TOOLCHAIN / pkg / "Bin" / "CL.EXE").exists():
            sys.exit(f"{TOOLCHAIN / pkg} is missing: python -I tools/fetch_toolchain.py {pkg[4:]}")
    if any("DX7 Display Plug-ins" in str(j[1]) or "dx7sdk" in verify.unit_includes(j[1]) for j in jobs) \
            and not (TOOLCHAIN / "dx7sdk" / "include" / "ddraw.h").exists():
        sys.exit(f"{TOOLCHAIN / 'dx7sdk'} is missing: python -I tools/fetch_libs.py")

    root, verify.TOOLCHAIN = canonical_root(sorted({j[3] for j in jobs}))
    verify.ROOT = root
    jobs = [(m, root / "src" / p.resolve().relative_to(ROOT / "src"), f, c) for m, p, f, c in jobs]
    out = pathlib.Path(a.out).resolve()
    workroot = ORIG_SRC_ROOT.with_name(ORIG_SRC_ROOT.name + ".tmp")
    workroot.mkdir(parents=True, exist_ok=True)
    for _, src, _, _ in jobs:
        verify.include_paths(verify.stage_source(src))

    t0 = time.time()
    failed = []
    done = {}
    for pkg in sorted({j[3] for j in jobs}):
        verify.COMPILER = pkg
        batch = [j for j in jobs if j[3] == pkg]
        with concurrent.futures.ThreadPoolExecutor(max(1, a.j)) as ex:
            for job, ok, log in ex.map(lambda j: compile_one(j, out, workroot), batch):
                mod = job[0]
                done.setdefault(mod, [0, 0])
                done[mod][0 if ok else 1] += 1
                if not ok:
                    failed.append(job)
                    print(f"FAILED {mod} {job[1].relative_to(verify.ROOT / 'src')}\n{log}\n", flush=True)
    verify.COMPILER = COMPILER
    for m in [*mods, "lib"]:
        ok, bad = done.get(m, [0, 0])
        print(f"{m:9} {ok:4} compiled  {bad:3} failed")
    print(f"{len(jobs) - len(failed)}/{len(jobs)} units compiled in {time.time() - t0:.0f} s -> {out}")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
