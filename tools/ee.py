"""Shared paths, module table and compiler flags of the build."""
import os
import pathlib

ROOT = pathlib.Path(__file__).resolve().parent.parent
ORIG = ROOT / "orig-bin" / "aoc"
ANALYSIS = ROOT / "analysis"
BUILD = ROOT / "build"
TOOLCHAIN = pathlib.Path(os.environ.get("EE_TOOLCHAIN", ROOT / "toolchain"))
GHIDRA = pathlib.Path(os.environ.get("EE_GHIDRA", r"C:\tools\ghidra\ghidra_12.1.4_PUBLIC"))

MODULES = {
    "exe": ORIG / "EE-AOC.unwrapped.exe",
    "lle": ORIG / "retail" / "Low-Level Engine.dll",
    "default": ORIG / "retail" / "Default.dll",
    "tcp": ORIG / "retail" / "TCP Protocol.dll",
    "dx7hr": ORIG / "retail" / "DX7HRDisplay.dll",
    "dx7hrtnl": ORIG / "retail" / "DX7HRTnLDisplay.dll",
    "mixer": ORIG / "retail" / "Miles Sound System Mixer.dll",
    "jpeg": ORIG / "retail" / "Low-Level Engine.dll",
    "exeimg": ORIG / "EE-AOC.unwrapped.exe",
}

COMPILER = "msvc6.0"

ORIG_SRC_ROOT = pathlib.Path(os.environ.get("EE_ORIG_SRC_ROOT", r"C:\EEX"))

MODULE_SRC = {
    "tcp": "Low-Level Engine/Network Engine/TCP Protocol",
    "mixer": "Low-Level Engine/Sound Engine/Miles Sound System Mixer",
    "default": "Empire Earth/Random Map/Random Map Generators/Default",
    "lle": "Low-Level Engine",
    "dx7hr": "Low-Level Engine/Graphics Engine/DX7 Display Plug-ins",
    "dx7hrtnl": "Low-Level Engine/Graphics Engine/DX7 Display Plug-ins",
    "exe": "",
    "jpeg": "ThirdParty/libjpeg",
    "exeimg": "ThirdParty",
}


EEX_INCLUDE_DIRS = ["Low-Level Engine/Thread System", "Low-Level Engine/Graphics Engine",
                    "Empire Earth/Database", "Empire Earth/AI Thread"]


def module_src(mod):
    return ROOT / "src" / MODULE_SRC.get(mod, mod)

MODULE_FLAGS = {
    "tcp": "-O1 -G6 -MD -GR",
    "mixer": "-O1 -G6 -MD -GR",
    "default": "-O1 -G6 -MD -GR",
    "lle": "-O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS",
    "dx7hr": "-O1 -G6 -MD -GR -DLLE_CURRENT_HEADERS",
    "dx7hrtnl": "-O1 -G6 -MD -GR -DLLE_CURRENT_HEADERS",
    "exe": "-O1 -G6 -MD -GR -DLLE_CURRENT_HEADERS -DLLE_EXE_HEADERS",
    "jpeg": "-O1 -G6 -MD",
    "exeimg": "-O2 -MD",
}
DEFAULT_FLAGS = "-O2 -MD -GX"


def no_originals():
    """EE_NO_ORIGINALS=1: the build must not read any original game file (orig-bin/, the install, the ISO, or files
    derived from them under build/, such as build/exe/xref_noreloc.txt); module_path and verify.Original refuse."""
    return os.environ.get("EE_NO_ORIGINALS", "") not in ("", "0")


def refuse_original(what):
    if no_originals():
        raise SystemExit(f"EE_NO_ORIGINALS: refusing to read {what} (the build must not read original game files)")


def module_path(mod):
    if mod not in MODULES:
        raise SystemExit(f"unknown module {mod!r}; known: {', '.join(MODULES)}")
    refuse_original(f"the original image of {mod!r} ({MODULES[mod]})")
    return MODULES[mod]


def module_filename(mod):
    """The module's file name (`TCP Protocol.dll`), without touching the original."""
    if mod not in MODULES:
        raise SystemExit(f"unknown module {mod!r}; known: {', '.join(MODULES)}")
    return {"exe": "EE-AOC.exe"}.get(mod, MODULES[mod].name)


def module_units(mod):
    """Units in link order as (first rva, end rva, path). From analysis/<mod>/units.txt when it exists (multi-unit
    modules); otherwise every .cpp under src/<module>/ in name order, with no ranges."""
    uf = ANALYSIS / mod / "units.txt"
    if not uf.exists():
        return [(None, None, p) for p in sorted(module_src(mod).glob("*.cpp"))]
    units = []
    for line in open(uf):
        line = line.split("#", 1)[0].strip()
        if line:
            lo, hi, name = line.split(None, 2)
            units.append((int(lo, 16), int(hi, 16), module_src(mod) / name.strip()))
    return units
