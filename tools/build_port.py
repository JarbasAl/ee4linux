"""Port build: compile the game with a modern Clang toolchain (python3 -I tools/build_port.py --help).

  python3 -I tools/build_port.py [-j N] [--target mingw] [module ...]    compile, report per module
  python3 -I tools/build_port.py --link                                  compile everything, then link the images

The mingw target compiles every unit with llvm-mingw (tools/fetch_port_toolchain.py) for i686-w64-mingw32: Windows
images that run under Wine. The unit lists, module flags and per-unit flags are the VC6 build's (tools/ee.py,
tools/verify.py); only the -D options of those flags are used. Every unit gets EE_PORT defined and include/port/compat.h
included first.

A unit compiles again when its object is older than the unit or than any header its dependency file names. The report
gives, per module, the units in, the objects out and the units that failed with their first error; the full compiler
output is in build/port-<target>/logs/<module>.log.

--link links the seven images (the exe, the engine DLL and the five plug-ins) into build/port-<target>/bin, the engine
DLL first: the others link against its import library. An image's other imports come from the original's import list
(link/<module>/imports.txt): a Windows DLL links against the toolchain's import library, and a retail third-party DLL
(Miles, Bink) against one made from that list with the original's decorated import names. A DLL's C entry points are
exported through a .def made from link/<module>/exports.txt; its C++ classes are exported by their declarations.
The images use the toolchain's shared C++ runtime (libc++.dll, libunwind.dll), copied next to them.
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
from ee import (EEX_INCLUDE_DIRS, LIB_UNITS, LINK_DIR, MODULE_FILES, MODULE_FLAGS, MODULE_SRC, ROOT, TARGET_MODULES,
                TOOLCHAIN, module_sources)

# Library members still built from source: the DCL in the engine, Miles' atexit helper in the mixer. Dinkum's
# operator new[]/delete[] are libc++'s in the port.
PORT_LIB_UNITS = {"lle": ["implode", "explode"], "mixer": ["mssauto"]}
# Units of the port build alone: the names that the VC6 link takes from the original image
# (src/Port/EEPortLinks.cpp and EEPortWonLinks.cpp)
PORT_UNITS = {"exe": ["Port/EEPortLinks.cpp", "Port/EEPortWonLinks.cpp"]}
# Modules of the port build alone, each a folder of src: the native build's Win32 layer
PORT_MODULES = {"win32": "Port/Linux"}
MODULES = list(MODULE_SRC) + list(PORT_MODULES)
TARGETS = {
    "mingw": {
        "cc": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-clang",
        "cxx": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-clang++",
        "flags": ["-O1", "-march=i686", "-mfpmath=387", "-fasm-blocks", "-fms-extensions", "-fdelayed-template-parsing",
                  "-fno-strict-aliasing", "-w", "-ferror-limit=0",
                  # VC6 code passes &Temporary(...) as an argument; the temporary lives to the end of the full
                  # expression in both compilers, and no callee keeps the pointer
                  "-Wno-address-of-temporary"],
        "cxxflags": ["-std=gnu++14"],
        "ar": TOOLCHAIN / "llvm-mingw" / "bin" / "llvm-ar",
        "dlltool": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-dlltool",
        "runtime_dir": TOOLCHAIN / "llvm-mingw" / "i686-w64-mingw32" / "bin",
        "runtime": ["libc++.dll", "libunwind.dll"],
    },
    # Native i386 Linux. Wine's headers declare the Win32 and DirectX types and functions for a Unix compiler;
    # include/port/linux comes before them and implements what the game calls. Every unit is position-independent,
    # because the engine and the plug-ins are shared objects.
    "linux": {
        "cc": pathlib.Path("/usr/bin/clang"),
        "cxx": pathlib.Path("/usr/bin/clang++"),
        # -fdeclspec, not -fms-extensions: that makes the MSVC intrinsics built-ins, which Wine's headers define
        "flags": ["-m32", "-fPIC", "-O1", "-march=i686", "-mfpmath=387", "-fasm-blocks", "-fdeclspec",
                  "-fdelayed-template-parsing", "-fno-strict-aliasing", "-w", "-ferror-limit=0",
                  "-Wno-address-of-temporary", "-DEE_PORT_LINUX",
                  # the game creates DirectInput 7 (Input System.cpp); Wine's dinput.h defaults to 8
                  "-DDIRECTINPUT_VERSION=0x0700",
                  # Wine's headers define _WIN32, and SDL would then rename main() to SDL_main
                  "-DSDL_MAIN_HANDLED",
                  # VC6 narrows in braced initializers ({ ..., gTexturePixelDepth, ... })
                  "-Wno-c++11-narrowing"],
        "cxxflags": ["-std=gnu++14"],
        "includes": [ROOT / "include" / "port" / "linux"],
        "system_includes": [pathlib.Path("/usr/include/wine/windows"), TOOLCHAIN / "sdl2-i686" / "include" / "SDL2",
                            TOOLCHAIN / "ffmpeg-i686" / "include"],
        # the Win32 layer's libraries (tools/fetch_port_toolchain.py builds SDL2 and FFmpeg), found at run time
        # through the rpath
        "libs": ["-lpthread", "-ldl",
                 *(f"{flag}{TOOLCHAIN / lib / 'lib'}" for lib in ("sdl2-i686", "ffmpeg-i686")
                   for flag in ("-L", "-Wl,-rpath,")),
                 "-lSDL2", "-lavformat", "-lavcodec", "-lswscale", "-lswresample", "-lavutil", "-Wl,--wrap=fopen"],
        "modules": ["win32", "lle", "jpeg", "tcp", "default", "mixer", "dx7hrtnl", "exe", "exeimg", "won"],
    },
}
# include/port/case holds the Windows SDK headers that the game names in a case the toolchain's headers do not have
INCLUDES = [ROOT / "include", *(ROOT / "src" / d for d in EEX_INCLUDE_DIRS), ROOT / "include" / "winsdk",
            ROOT / "include" / "port" / "case"]
COMPAT = ROOT / "include" / "port" / "compat.h"
DEFINE = re.compile(r"^-D\S+$")
# images in link order (the engine DLL first), each with the modules whose objects it holds
IMAGES = {image: TARGET_MODULES.get(image, [image])
          for image in ("lle", "tcp", "default", "mixer", "dx7hr", "dx7hrtnl", "exe")}
# imports the port does not link: the VC6 runtimes, which the toolchain's C and C++ runtimes replace
RUNTIME_DLLS = {"msvcrt.dll", "sxlrt233.dll"}
SYSTEM_LIBS = ["-ldxguid", "-luuid"]
# modules linked as libraries, as the VC6 link does (tools/exe_fulllink.py): the exe takes only the members it needs
ARCHIVED = {"won", "exeimg"}    # the DirectX and COM GUIDs that the original compiled in from the SDK


def unit_defines(src, mod):
    tokens = shlex.split(verify.DEFINES)
    if mod not in PORT_MODULES:
        tokens += shlex.split(verify.unit_flags(src, MODULE_FLAGS[mod]))
    return [t for t in tokens if DEFINE.match(t)] + ["-DEE_PORT"]


def module_jobs(mod):
    if mod in PORT_MODULES:
        return [(mod, p) for p in sorted((ROOT / "src" / PORT_MODULES[mod]).glob("*.cpp"))]
    paths = module_sources(mod) + [ROOT / "src" / LIB_UNITS[n] for n in PORT_LIB_UNITS.get(mod, [])]
    paths += [ROOT / "src" / rel for rel in PORT_UNITS.get(mod, [])]
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
           *unit_defines(src, mod), "-include", str(COMPAT), *(f"-I{d}" for d in cfg.get("includes", [])),
           *(f"-I{d}" for d in INCLUDES), *(f"-isystem{d}" for d in cfg.get("system_includes", [])),
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


def import_sections(mod):
    """The original's imports: {dll name: [import name, ...]} from link/<mod>/imports.txt."""
    sections, dll = {}, None
    for line in (LINK_DIR / mod / "imports.txt").read_text().splitlines():
        if line.startswith("["):
            dll = line.strip("[]")
            sections[dll] = []
        elif line.strip():
            sections[dll].append(line.split("\t")[0])
    return sections


def import_library(target, out, dll, names):
    """An import library for a retail DLL that the toolchain has none for, with the original's import names."""
    stem = dll.rsplit(".", 1)[0].lower()
    lib_dir = out / "lib"
    def_file = lib_dir / f"{stem}.def"
    # _Name@N is a stdcall import: the symbol is Name@N to the compiler, the import name is the decorated original
    lines = [f"{n[1:]} == {n}" if n.startswith("_") else n for n in names]
    def_file.write_text(f"LIBRARY {dll}\nEXPORTS\n" + "\n".join(lines) + "\n")
    lib = lib_dir / f"lib{stem}.a"
    subprocess.run([str(TARGETS[target]["dlltool"]), "-d", str(def_file), "-l", str(lib)], check=True)
    return str(lib)


def link_image(target, out, image):
    cfg = TARGETS[target]
    bin_dir = out / "bin"
    for folder in (bin_dir, out / "lib", out / "link"):
        folder.mkdir(parents=True, exist_ok=True)
    objects, archives = [], []
    for mod in IMAGES[image]:
        found = sorted(str(o) for o in (out / "obj" / mod).rglob("*.o"))
        objects += found
        if mod in ARCHIVED:
            archive = out / "lib" / f"lib{mod}.a"
            archive.unlink(missing_ok=True)
            mod_rsp = out / "link" / f"{mod}.rsp"
            mod_rsp.write_text("\n".join(f'"{o}"' for o in found) + "\n")
            subprocess.run([str(cfg["ar"]), "rcs", str(archive), f"@{mod_rsp}"], check=True)
            archives.append(str(archive))
    rsp = out / "link" / f"{image}.rsp"
    rsp.write_text("\n".join(f'"{o}"' for o in objects if not any(f"/obj/{m}/" in o for m in ARCHIVED)) + "\n")
    cmd = [str(cfg["cxx"]), f"@{rsp}", *archives, "-o", str(bin_dir / MODULE_FILES[image]), "-Wl,--error-limit=0"]
    if image == "exe":
        cmd.append("-mwindows")
    else:
        cmd += ["-shared", f"-Wl,--out-implib,{out / 'lib' / ('lib' + image + '.a')}"]
        exports = [line.split()[0] for line in (LINK_DIR / image / "exports.txt").read_text().splitlines()
                   if line.strip() and not line.startswith("?")]
        if exports:
            def_file = out / "link" / f"{image}.def"
            def_file.write_text("EXPORTS\n" + "\n".join(exports) + "\n")
            cmd.append(str(def_file))
    toolchain_libs = TOOLCHAIN / "llvm-mingw" / "i686-w64-mingw32" / "lib"
    for dll, names in import_sections(image).items():
        stem = dll.rsplit(".", 1)[0].lower()
        if dll.lower() in RUNTIME_DLLS:
            continue
        if dll == MODULE_FILES["lle"]:
            cmd.append(str(out / "lib" / "liblle.a"))
        elif (toolchain_libs / f"lib{stem}.a").exists():
            cmd.append(f"-l{stem}")
        else:
            cmd.append(import_library(target, out, dll, names))
    cmd += SYSTEM_LIBS
    run = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    (out / "logs" / f"link-{image}.log").write_text(" ".join(cmd) + "\n" + run.stderr)
    return len(objects), run.returncode == 0, run.stderr


def copy_runtime(target, out):
    for dll in TARGETS[target]["runtime"]:
        data = (TARGETS[target]["runtime_dir"] / dll).read_bytes()
        (out / "bin" / dll).write_bytes(data)


def link_all(target, out):
    linked = 0
    for image in IMAGES:
        count, ok, log = link_image(target, out, image)
        print(f"link {MODULE_FILES[image]}: {count} objects in, {'image out' if ok else 'FAILED'}")
        linked += ok
        if not ok:
            errors = [line for line in log.splitlines() if "error:" in line]
            for line in errors[:8]:
                print(f"  {line}")
            if len(errors) > 8:
                print(f"  ... {len(errors) - 8} more in {out / 'logs' / ('link-' + image + '.log')}")
            if image == "lle":
                break
    copy_runtime(target, out)
    print(f"images: {len(IMAGES)} in, {linked} linked")
    return len(IMAGES) - linked


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("modules", nargs="*", help=f"modules to compile (default: all of the target's, from {MODULES})")
    ap.add_argument("-j", type=int, default=2, help="parallel compiles")
    ap.add_argument("--target", default="mingw", choices=sorted(TARGETS))
    ap.add_argument("--out", type=pathlib.Path, help="output folder (default build/port-<target>)")
    ap.add_argument("--link", action="store_true", help="link the images after compiling every module")
    args = ap.parse_args()
    args.modules = args.modules or TARGETS[args.target].get("modules", list(MODULE_SRC))
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
    if total_in != total_out:
        return 1
    if args.link:
        if args.target != "mingw":
            sys.exit("--link supports only the mingw target")
        if set(args.modules) != set(MODULE_SRC):
            sys.exit("--link needs every module compiled: give no module names")
        return 1 if link_all(args.target, out) else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
