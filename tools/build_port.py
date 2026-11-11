"""Port build: compile the game with a modern Clang toolchain (python3 -I tools/build_port.py --help).

  python3 -I tools/build_port.py [-j N] [--target mingw] [module ...]    compile, report per module

The mingw target compiles every unit with llvm-mingw (tools/fetch_port_toolchain.py) for i686-w64-mingw32: Windows
images that run under Wine. The unit lists, module flags and per-unit flags are the VC6 build's (tools/ee.py,
tools/verify.py); only the -D options of those flags are used. Every unit gets EE_PORT defined and include/port/compat.h
included first.

A unit compiles again when its object is older than the unit or than any header its dependency file names. The report
gives, per module, the units in, the objects out and the units that failed with their first error; the full compiler
output is in build/port-<target>/logs/<module>.log.
"""
import argparse
import concurrent.futures
import os
import pathlib
import re
import shlex
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import verify
from ee import EEX_INCLUDE_DIRS, LIB_UNITS, MODULE_FLAGS, MODULE_SRC, ROOT, TOOLCHAIN, module_sources

MODULES = list(MODULE_SRC)
# Library members still built from source: the DCL in the engine, Miles' atexit helper in the mixer. Dinkum's
# operator new[]/delete[] are libc++'s in the port.
PORT_LIB_UNITS = {"lle": ["implode", "explode"], "mixer": ["mssauto"]}
TARGETS = {
    "mingw": {
        "cc": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-clang",
        "cxx": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-clang++",
        "flags": ["-O1", "-march=i686", "-mfpmath=387", "-fasm-blocks", "-fms-extensions", "-fdelayed-template-parsing",
                  "-fno-strict-aliasing", "-w"],
        "cxxflags": ["-std=gnu++14"],
    },
}
INCLUDES = [ROOT / "include", *(ROOT / "src" / d for d in EEX_INCLUDE_DIRS), ROOT / "include" / "winsdk"]
COMPAT = ROOT / "include" / "port" / "compat.h"
DEFINE = re.compile(r"^-D\S+$")


def unit_defines(src, mod):
    tokens = shlex.split(verify.DEFINES) + shlex.split(verify.unit_flags(src, MODULE_FLAGS[mod]))
    return [t for t in tokens if DEFINE.match(t)] + ["-DEE_PORT"]


def module_jobs(mod):
    paths = module_sources(mod) + [ROOT / "src" / LIB_UNITS[n] for n in PORT_LIB_UNITS.get(mod, [])]
    return [(mod, p) for p in paths]


def object_path(out, mod, src):
    return out / "obj" / mod / src.resolve().relative_to(ROOT / "src").with_suffix(".o")


def up_to_date(obj, src):
    dep = obj.with_suffix(".d")
    if not obj.exists() or not dep.exists():
        return False
    stamp = obj.stat().st_mtime
    text = dep.read_text(errors="replace").replace("\\\n", " ")
    headers = re.findall(r"(?:[^\s\\]|\\ )+", text.split(":", 1)[1]) if ":" in text else []
    for name in [str(src)] + [h.replace("\\ ", " ") for h in headers]:
        try:
            if pathlib.Path(name).stat().st_mtime > stamp:
                return False
        except OSError:
            return False
    return True


def compile_unit(target, out, mod, src):
    obj = object_path(out, mod, src)
    if up_to_date(obj, src):
        return mod, src, True, ""
    obj.parent.mkdir(parents=True, exist_ok=True)
    cfg = TARGETS[target]
    is_c = src.suffix == ".c"
    cmd = [str(cfg["cc"] if is_c else cfg["cxx"]), *cfg["flags"], *([] if is_c else cfg["cxxflags"]),
           *unit_defines(src, mod), "-include", str(COMPAT), *(f"-I{d}" for d in INCLUDES),
           "-MMD", "-MF", str(obj.with_suffix(".d")), "-c", str(src), "-o", str(obj)]
    run = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    if run.returncode:
        obj.unlink(missing_ok=True)
    return mod, src, run.returncode == 0, run.stderr


def first_error(log):
    for line in log.splitlines():
        if "error:" in line:
            return line.split(str(ROOT) + "/", 1)[-1]
    return log.strip().splitlines()[-1] if log.strip() else "(no output)"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("modules", nargs="*", default=MODULES, help=f"modules to compile (default: all of {MODULES})")
    ap.add_argument("-j", type=int, default=2, help="parallel compiles")
    ap.add_argument("--target", default="mingw", choices=sorted(TARGETS))
    ap.add_argument("--out", type=pathlib.Path, help="output folder (default build/port-<target>)")
    args = ap.parse_args()
    unknown = set(args.modules) - set(MODULES)
    if unknown:
        sys.exit(f"unknown module(s) {', '.join(sorted(unknown))}; known: {', '.join(MODULES)}")
    if not TARGETS[args.target]["cxx"].exists():
        sys.exit(f"{TARGETS[args.target]['cxx']} is missing: python3 -I tools/fetch_port_toolchain.py")
    out = args.out or ROOT / "build" / f"port-{args.target}"
    jobs = [job for mod in args.modules for job in module_jobs(mod)]
    missing = [str(p) for _, p in jobs if not p.exists()]
    if missing:
        sys.exit("units missing:\n  " + "\n  ".join(missing))

    results = {mod: [] for mod in args.modules}
    with concurrent.futures.ThreadPoolExecutor(args.j) as pool:
        for mod, src, ok, log in pool.map(lambda job: compile_unit(args.target, out, *job), jobs):
            results[mod].append((src, ok, log))

    (out / "logs").mkdir(parents=True, exist_ok=True)
    total_in = total_out = 0
    for mod, rows in results.items():
        failed = [(src, log) for src, ok, log in rows if not ok]
        objects = sum(object_path(out, mod, src).exists() for src, _, _ in rows)
        total_in += len(rows)
        total_out += objects
        with open(out / "logs" / f"{mod}.log", "w") as log_file:
            for src, ok, log in rows:
                if log:
                    log_file.write(f"=== {src.relative_to(ROOT)}\n{log}\n")
        print(f"{mod}: {len(rows)} units in, {objects} objects out, {len(failed)} failed")
        for src, log in failed[:5]:
            print(f"  {src.relative_to(ROOT / 'src')}: {first_error(log)}")
        if len(failed) > 5:
            print(f"  ... {len(failed) - 5} more in {out / 'logs' / (mod + '.log')}")
    print(f"total: {total_in} units in, {total_out} objects out")
    return 0 if total_in == total_out else 1


if __name__ == "__main__":
    sys.exit(main())
