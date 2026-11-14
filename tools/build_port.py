"""Port build: compile the game with a modern Clang toolchain (python3 -I tools/build_port.py --help).

  python3 -I tools/build_port.py [-j N] [--target mingw] [module ...]    compile, report per module
  python3 -I tools/build_port.py --link                                  compile everything, then link the images

The mingw target compiles every unit with llvm-mingw (tools/fetch_port_toolchain.py) for i686-w64-mingw32: Windows
images that run under Wine. The unit lists, module flags and per-unit flags are the VC6 build's (tools/ee.py,
tools/verify.py); only the -D options of those flags are used. Every unit gets EE_PORT defined and include/port/compat.h
included first.

A unit compiles again when its object is older than the unit or than any header its dependency file names, or when
its compile command differs from the one stored beside the object. The report
gives, per module, the units in, the objects out and the units that failed with their first error; the full compiler
output is in build/port-<target>/logs/<module>.log.

--sanitize CHECKS builds a linux target with -fsanitize=CHECKS and links the runtime into every image, with recovery
turned off so the first report stops the run and names the site. The output folder is build/port-<target>-<CHECKS>,
so a sanitized build never mixes objects with the normal one.

--link links the seven images (the exe, the engine DLL and the five plug-ins) into build/port-<target>/bin, the engine
DLL first: the others link against its import library. For the linux target it links ELF files with the same names
(LINUX_IMAGES), and the native executable is ee-aoc. An image's other imports come from the original's import list
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
import shutil
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import long32
import verify
from ee import (EEX_INCLUDE_DIRS, LIB_UNITS, LINK_DIR, MODULE_FILES, MODULE_FLAGS, MODULE_SRC, ROOT, TARGET_MODULES,
                TOOLCHAIN, module_sources)

# Library members still built from source: the DCL in the engine, Miles' atexit helper in the mixer. Dinkum's
# operator new[]/delete[] are libc++'s in the port.
PORT_LIB_UNITS = {"lle": ["implode", "explode"], "mixer": ["mssauto"]}
# Units of the port build alone: the names that the VC6 link takes from the original image
# (src/Port/EEPortLinks.cpp and EEPortWonLinks.cpp), the simulator, and Age of AI
PORT_UNITS = {"exe": ["Port/EEPortLinks.cpp", "Port/EEPortWonLinks.cpp", "Port/EEPortLinux.cpp", "Port/EESim.cpp",
                      "Empire Earth/AI Thread/AgeOfAI/AgeOfAI.cpp", "Empire Earth/AI Thread/AgeOfAI/AgeOfAISettings.cpp",
                      "Empire Earth/AI Thread/AgeOfAI/AgeOfAIIntel.cpp", "Empire Earth/AI Thread/AgeOfAI/AgeOfAIIntelScan.cpp",
                      "Empire Earth/AI Thread/AgeOfAI/AgeOfAIRoles.cpp"],
              "lle": ["Port/EEPortSim.cpp"]}
# Modules of the port build alone, each a folder of src: the native build's Win32 layer
PORT_MODULES = {"win32": "Port/Linux"}
# C libraries the Win32 layer carries as source, with their own flags in place of the game's: GothicKit/dmusic
# (src/Port/Linux/third_party/dmusic/README.md), the DirectMusic composer and synthesizer behind DirectMusic.cpp
DMUSIC = ROOT / "src" / "Port" / "Linux" / "third_party" / "dmusic"
LIBRARY_MODULES = {
    "dmusic": {
        "folder": DMUSIC,
        # C11 threads come from the library's own shim over POSIX threads; dm_prefix.h renames the shim and the
        # rest of its global names, which glibc's C11 threads and other copies of TinySoundFont also define
        "flags": ["-fPIC", "-O2", "-msse2", "-mfpmath=sse", "-std=c11", "-D_POSIX_C_SOURCE=200809L",
                  "-DDM_STATIC=1", "-w", f"-I{DMUSIC / 'include'}", f"-I{DMUSIC / 'src'}",
                  f"-I{DMUSIC / 'vendor' / 'TinySoundFont'}", "-include", str(DMUSIC / "recomp" / "dm_prefix.h")],
    },
}
MODULES = list(MODULE_SRC) + list(PORT_MODULES) + list(LIBRARY_MODULES)
# Wine's Windows headers: the wine package's, or a copy on a build machine without Wine (EE_WINE_HEADERS)
WINE_HEADERS = pathlib.Path(os.environ.get("EE_WINE_HEADERS", "/usr/include/wine/windows"))


def find_compiler(name, variable):
    """The Clang driver: the variable (EE_CC, EE_CXX) when it is set, else the first `name` on PATH, so a Clang outside
    /usr/bin (Gentoo's /usr/lib/llvm/<version>/bin, for example) needs no change here."""
    found = os.environ.get(variable) or shutil.which(name)
    return pathlib.Path(found) if found else pathlib.Path("/usr/bin") / name


CLANG = find_compiler("clang", "EE_CC")
CLANGXX = find_compiler("clang++", "EE_CXX")

TARGETS = {
    "mingw": {
        "cc": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-clang",
        "cxx": TOOLCHAIN / "llvm-mingw" / "bin" / "i686-w64-mingw32-clang++",
        "flags": ["-O1", "-march=i686", "-mfpmath=387", "-fasm-blocks", "-fms-extensions", "-fdelayed-template-parsing",
                  "-fno-strict-aliasing", "-w", "-ferror-limit=0",
                  # VC6 never used a member call to infer that a pointer is non-null. The game calls members that
                  # ignore `this` through null pointers and tests the pointer afterwards (EEIsLineToUnitBlocked calls
                  # theList->Scale, then tests theList), so the later null tests must stay
                  "-fno-delete-null-pointer-checks",
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
        "cc": CLANG,
        "cxx": CLANGXX,
        # -fdeclspec, not -fms-extensions: that makes the MSVC intrinsics built-ins, which Wine's headers define
        "flags": ["-m32", "-fPIC", "-O1", "-march=i686", "-mfpmath=387", "-fasm-blocks", "-fdeclspec",
                  "-fdelayed-template-parsing", "-fno-strict-aliasing", "-w", "-ferror-limit=0",
                  "-Wno-address-of-temporary", "-DEE_PORT_LINUX",
                  "-fno-delete-null-pointer-checks",   # see the mingw target
                  # the game creates DirectInput 7 (Input System.cpp); Wine's dinput.h defaults to 8
                  "-DDIRECTINPUT_VERSION=0x0700",
                  # Wine's headers define _WIN32, and SDL would then rename main() to SDL_main
                  "-DSDL_MAIN_HANDLED",
                  # VC6 narrows in braced initializers ({ ..., gTexturePixelDepth, ... })
                  "-Wno-c++11-narrowing",
                  # Wine's headers declare the Win32 functions hidden outside a PE build; the Win32 layer is a shared
                  # object, so its definitions must be visible to the engine, the plug-ins and the executable
                  "-DDECLSPEC_IMPORT="],
        "cxxflags": ["-std=gnu++14"],
        "includes": [ROOT / "include" / "port" / "linux"],
        "system_includes": [WINE_HEADERS, TOOLCHAIN / "sdl2-i686" / "include" / "SDL2",
                            TOOLCHAIN / "ffmpeg-i686" / "include", DMUSIC / "include"],
        # the Win32 layer's libraries (tools/fetch_port_toolchain.py builds SDL2 and FFmpeg), found at run time
        # through the rpath
        "libs": ["-lpthread", "-ldl",
                 *(f"{flag}{TOOLCHAIN / lib / 'lib'}" for lib in ("sdl2-i686", "ffmpeg-i686")
                   for flag in ("-L", "-Wl,-rpath,")),
                 "-lSDL2", "-lavformat", "-lavcodec", "-lswscale", "-lswresample", "-lavutil", "-Wl,--wrap=fopen"],
        "modules": ["win32", "dmusic", "lle", "jpeg", "tcp", "default", "mixer", "dx7hrtnl", "exe", "exeimg", "won"],
    },
    # Native x86-64 Linux. The same layer, the same headers and the system's 64-bit SDL2 and FFmpeg. x86-64 has no
    # x87 mode, so the arithmetic is SSE2 like VC6's floating point on a modern Windows target; the game's own units
    # are not ported yet (docs/ in the wiki audit for the plan).
    "linux64": {
        "cc": CLANG,
        "cxx": CLANGXX,
        # WINE_UNIX_LIB: Wine's headers make __stdcall the Windows x64 convention (ms_abi) otherwise, and a Linux
        # program has one convention of its own; with it the whole port, game and layer, uses the System V one
        "flags": ["-m64", "-fPIC", "-O1", "-g1", "-march=x86-64", "-msse2", "-mfpmath=sse", "-fasm-blocks", "-fdeclspec",
                  "-DWINE_UNIX_LIB",
                  "-fdelayed-template-parsing", "-fno-strict-aliasing", "-w", "-ferror-limit=0",
                  "-Wno-address-of-temporary", "-DEE_PORT_LINUX", "-DEE_PORT_LINUX64", "-DEE_PORT_LONG32",
                  "-fno-delete-null-pointer-checks",   # see the mingw target
                  "-DDIRECTINPUT_VERSION=0x0700",
                  "-DSDL_MAIN_HANDLED",
                  "-Wno-c++11-narrowing",
                  "-DDECLSPEC_IMPORT="],
        "cxxflags": ["-std=gnu++14"],
        "includes": [ROOT / "include" / "port" / "linux"],
        # the system's 64-bit development packages: wine, sdl2, ffmpeg
        "system_includes": [WINE_HEADERS, TOOLCHAIN / "sdl2-x86_64" / "include" / "SDL2",
                            TOOLCHAIN / "ffmpeg-x86_64" / "include", DMUSIC / "include"],
        # the same pinned SDL2 and FFmpeg as the 32-bit build, at 64 bits (tools/fetch_port_toolchain.py)
        "libs": ["-lpthread", "-ldl",
                 *(f"{flag}{TOOLCHAIN / lib / 'lib'}" for lib in ("sdl2-x86_64", "ffmpeg-x86_64")
                   for flag in ("-L", "-Wl,-rpath,")),
                 "-lSDL2", "-lavformat", "-lavcodec", "-lswscale", "-lswresample", "-lavutil", "-Wl,--wrap=fopen"],
        "modules": ["win32", "dmusic", "lle", "jpeg", "tcp", "default", "mixer", "dx7hrtnl", "exe", "exeimg", "won"],
        # the game compiles from a copy in which long is 32 bits wide, as on Windows (tools/long32.py)
        "long32": True,
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
    if mod in LIBRARY_MODULES:
        return [(mod, p) for p in sorted(LIBRARY_MODULES[mod]["folder"].rglob("*.c"))]
    if mod in PORT_MODULES:
        return [(mod, p) for p in sorted((ROOT / "src" / PORT_MODULES[mod]).glob("*.cpp"))]
    paths = module_sources(mod) + [ROOT / "src" / LIB_UNITS[n] for n in PORT_LIB_UNITS.get(mod, [])]
    paths += [ROOT / "src" / rel for rel in PORT_UNITS.get(mod, [])]
    return [(mod, p) for p in paths]


def object_path(out, mod, src):
    return out / "obj" / mod / src.resolve().relative_to(ROOT / "src").with_suffix(".o")


def up_to_date(obj, src, cmd):
    """True when the object was made by this command and is newer than the unit and every header it read."""
    dep, stamp_file = obj.with_suffix(".d"), obj.with_suffix(".cmd")
    if not obj.exists() or not dep.exists() or not stamp_file.exists():
        return False
    if stamp_file.read_text(errors="replace") != shlex.join(cmd):
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
    cfg = TARGETS[target]
    is_c = src.suffix == ".c"
    if mod in LIBRARY_MODULES:
        cmd = [str(cfg["cc"]), cfg["flags"][0], *LIBRARY_MODULES[mod]["flags"], "-MMD", "-MF",
               str(obj.with_suffix(".d")), "-c", str(src), "-o", str(obj)]
    else:
        place = (lambda path: long32.shadow(out / "long32", path)) if cfg.get("long32") else (lambda path: path)
        cmd = [str(cfg["cc"] if is_c else cfg["cxx"]), *cfg["flags"], *([] if is_c else cfg["cxxflags"]),
               *unit_defines(src, mod), "-include", str(COMPAT), *(f"-I{d}" for d in cfg.get("includes", [])),
               *(f"-I{place(d)}" for d in INCLUDES), *(f"-isystem{d}" for d in cfg.get("system_includes", [])),
               "-MMD", "-MF", str(obj.with_suffix(".d")), "-c", str(place(src)), "-o", str(obj)]
    if up_to_date(obj, src, cmd):
        return mod, src, True, ""
    obj.parent.mkdir(parents=True, exist_ok=True)
    run = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    if run.returncode:
        obj.unlink(missing_ok=True)
    else:
        obj.with_suffix(".cmd").write_text(shlex.join(cmd))
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


# The native images, in link order, each with its file name and modules. The Win32 layer is one shared object that every
# other image uses, so all of them share its windows, GDI objects and locks. The engine and the plug-ins keep their
# Windows names: the engine's plug-in scan loads *.dll from the game folder. The executable is not named EE-AOC.exe,
# because the layer reads the main module's resources from the retail EE-AOC.exe beside it.
LINUX_IMAGES = {
    "eeport": ("libeeport.so", ["win32", "dmusic"]),
    "lle": (MODULE_FILES["lle"], TARGET_MODULES["lle"]),
    "tcp": (MODULE_FILES["tcp"], ["tcp"]),
    "default": (MODULE_FILES["default"], ["default"]),
    "mixer": (MODULE_FILES["mixer"], ["mixer"]),
    "dx7hrtnl": (MODULE_FILES["dx7hrtnl"], ["dx7hrtnl"]),
    "exe": ("ee-aoc", TARGET_MODULES["exe"]),
}
# The 64-bit images: the same names and modules as the 32-bit ones, so the engine's plug-in scan finds them
LINUX_IMAGES_64 = {
    "eeport": ("libeeport.so", ["win32", "dmusic"]),
    "lle": (MODULE_FILES["lle"], TARGET_MODULES["lle"]),
    "tcp": (MODULE_FILES["tcp"], ["tcp"]),
    "default": (MODULE_FILES["default"], ["default"]),
    "mixer": (MODULE_FILES["mixer"], ["mixer"]),
    "dx7hrtnl": (MODULE_FILES["dx7hrtnl"], ["dx7hrtnl"]),
    "exe": ("ee-aoc", TARGET_MODULES["exe"]),
}
# The layer's libraries, copied beside the images under the names the images ask for
LINUX_RUNTIME = {"sdl2-i686": ["libSDL2-2.0.so.0"],
                 "ffmpeg-i686": ["libavformat.so.61", "libavcodec.so.61", "libswscale.so.8", "libswresample.so.5",
                                 "libavutil.so.59"],
                 "sdl2-x86_64": ["libSDL2-2.0.so.0"],
                 "ffmpeg-x86_64": ["libavformat.so.61", "libavcodec.so.61", "libswscale.so.8", "libswresample.so.5",
                                   "libavutil.so.59"]}
# The sanitizer runtime the images link against, for --sanitize: clang leaves it out of a shared object's link, and
# every image but the executable is one
SANITIZE_RUNTIME = {"linux": "libclang_rt.ubsan_standalone-i686.a",
                    "linux64": "libclang_rt.ubsan_standalone-x86_64.a"}
SANITIZE_LIBS = []


def link_linux_image(out, image, target="linux"):
    cfg = TARGETS[target]
    images = LINUX_IMAGES if target == "linux" else LINUX_IMAGES_64
    name, modules = images[image]
    bin_dir = out / "bin"
    for folder in (bin_dir, out / "lib", out / "link"):
        folder.mkdir(parents=True, exist_ok=True)
    objects, archives = [], []
    for mod in modules:
        found = sorted(str(o) for o in (out / "obj" / mod).rglob("*.o"))
        objects += found
        if mod in ARCHIVED:
            archive = out / "lib" / f"lib{mod}.a"
            archive.unlink(missing_ok=True)
            mod_rsp = out / "link" / f"{mod}.rsp"
            mod_rsp.write_text("\n".join(f'"{o}"' for o in found) + "\n")
            subprocess.run(["llvm-ar", "rcs", str(archive), f"@{mod_rsp}"], check=True)
            archives.append(str(archive))
    rsp = out / "link" / f"{target}-{image}.rsp"
    rsp.write_text("\n".join(f'"{o}"' for o in objects if not any(f"/obj/{m}/" in o for m in ARCHIVED)) + "\n")
    # each image binds its own names to its own definitions, as a Windows DLL does; the executable is
    # position-independent, so the engine's data stays in the engine
    # the archives need each other (WON's PNG decoder takes libpng from exeimg), and GNU ld reads an archive once
    grouped = ["-Wl,--start-group", *archives, "-Wl,--end-group"] if archives else []
    # every image casts the way MSVC does: src/Port/Linux/CxxAbi.cpp wraps the cast and states the rule
    cmd = [str(cfg["cxx"]), *cfg["flags"], f"@{rsp}", *grouped, "-o", str(bin_dir / name),
           "-Wl,-rpath,$ORIGIN", "-Wl,--wrap=fopen", "-Wl,--wrap=__dynamic_cast", *SANITIZE_LIBS]
    if image == "exe":
        cmd += ["-pie"]
    else:
        cmd += ["-shared", f"-Wl,-soname,{name}", "-Wl,-Bsymbolic", "-Wl,-z,defs"]
    if image == "eeport":
        if target == "linux":
            cmd += [*(f"-L{TOOLCHAIN / lib / 'lib'}" for lib in LINUX_RUNTIME)]
        cmd += [*cfg["libs"], "-lm"]
    else:
        if image != "lle":
            cmd.append(str(bin_dir / images["lle"][0]))
        cmd += [str(bin_dir / images["eeport"][0]), "-lpthread"]
    run = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
    (out / "logs" / f"link-{image}.log").write_text(" ".join(cmd) + "\n" + run.stderr)
    return len(objects), run.returncode == 0, run.stderr


def link_linux(out, target="linux"):
    linked = 0
    images = LINUX_IMAGES if target == "linux" else LINUX_IMAGES_64
    # the libraries go beside the images first: the linker finds the layer's libraries through its rpath
    arch = {"linux": "i686", "linux64": "x86_64"}[target]
    (out / "bin").mkdir(parents=True, exist_ok=True)
    for lib, names in LINUX_RUNTIME.items():
        if lib.endswith(arch):
            for library in names:
                (out / "bin" / library).write_bytes((TOOLCHAIN / lib / "lib" / library).read_bytes())
    for image in images:
        count, ok, log = link_linux_image(out, image, target)
        print(f"link {images[image][0]}: {count} objects in, {'image out' if ok else 'FAILED'}")
        linked += ok
        if not ok:
            errors = [line for line in log.splitlines() if "error" in line or "undefined" in line]
            for line in errors[:8]:
                print(f"  {line}")
            if len(errors) > 8:
                print(f"  ... {len(errors) - 8} more in {out / 'logs' / ('link-' + image + '.log')}")
            if image in ("eeport", "lle"):
                break
    print(f"images: {len(images)} in, {linked} linked")
    return len(images) - linked


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("modules", nargs="*", help=f"modules to compile (default: all of the target's, from {MODULES})")
    ap.add_argument("-j", type=int, default=2, help="parallel compiles")
    ap.add_argument("--target", default="mingw", choices=sorted(TARGETS))
    ap.add_argument("--out", type=pathlib.Path, help="output folder (default build/port-<target>)")
    ap.add_argument("--link", action="store_true", help="link the images after compiling every module")
    ap.add_argument("--sanitize", metavar="CHECKS",
                    help="compile and link the linux targets with -fsanitize=CHECKS (a comma list, e.g. "
                         "pointer-overflow or undefined) and stop at the first report; the default output folder "
                         "gets the first check's name")
    args = ap.parse_args()
    if args.sanitize and args.target == "mingw":
        sys.exit("--sanitize is for the linux targets: the mingw images run under Wine and are the reference")
    if args.sanitize:
        checks = [c.strip() for c in args.sanitize.split(",") if c.strip()]
        # the report names the site and the run stops there, instead of a half-executed program carrying on
        TARGETS[args.target]["flags"] += [f"-fsanitize={','.join(checks)}", f"-fno-sanitize-recover={','.join(checks)}",
                                          "-fno-omit-frame-pointer"]
        TARGETS[args.target]["libs"] += [f"-fsanitize={','.join(checks)}"]
        # clang names the runtime itself for the executable only, so the shared images get it here, and each of them
        # stays self-contained under -z defs
        probe = subprocess.run([str(TARGETS[args.target]["cxx"]),
                                f"-print-file-name={SANITIZE_RUNTIME[args.target]}"],
                               capture_output=True, text=True).stdout.strip()
        runtime = pathlib.Path(probe) if probe else None
        if not runtime or not runtime.is_file():
            sys.exit(f"--sanitize needs {SANITIZE_RUNTIME[args.target]}, which this compiler has not: {probe!r}")
        SANITIZE_LIBS[:] = [str(runtime)]
    args.modules = args.modules or TARGETS[args.target].get("modules", list(MODULE_SRC))
    unknown = set(args.modules) - set(MODULES)
    if unknown:
        sys.exit(f"unknown module(s) {', '.join(sorted(unknown))}; known: {', '.join(MODULES)}")
    if not TARGETS[args.target]["cxx"].exists():
        sys.exit(f"{TARGETS[args.target]['cxx']} is missing: python3 -I tools/fetch_port_toolchain.py")
    suffix = f"-{args.sanitize.split(',')[0]}" if args.sanitize else ""
    out = args.out or ROOT / "build" / f"port-{args.target}{suffix}"
    jobs = [job for mod in args.modules for job in module_jobs(mod)]
    missing = [str(p) for _, p in jobs if not p.exists()]
    if missing:
        sys.exit("units missing:\n  " + "\n  ".join(missing))

    if TARGETS[args.target].get("long32"):
        seen, written = long32.sync(out / "long32")
        print(f"long32: {seen} files in, {written} written")
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
        if args.target in ("linux", "linux64"):
            if set(args.modules) != set(TARGETS[args.target]["modules"]):
                sys.exit("--link needs every module compiled: give no module names")
            return 1 if link_linux(out, args.target) else 0
        if set(args.modules) != set(MODULE_SRC):
            sys.exit("--link needs every module compiled: give no module names")
        return 1 if link_all(args.target, out) else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
