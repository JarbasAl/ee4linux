"""Shared paths, module table and compiler flags of the build."""
import os
import pathlib
ROOT = pathlib.Path(__file__).resolve().parent.parent
LINK_DIR = ROOT / "link"
TOOLCHAIN = pathlib.Path(os.environ.get("EE_TOOLCHAIN", ROOT / "toolchain"))
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
    "lle": "-O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS -Zi",
    "dx7hr": "-O1 -G6 -MD -GR -DLLE_CURRENT_HEADERS -Zi",
    "dx7hrtnl": "-O1 -G6 -MD -GR -DLLE_CURRENT_HEADERS -Zi",
    "exe": "-O1 -G6 -MD -GR -DLLE_CURRENT_HEADERS -DLLE_EXE_HEADERS -Zi -Zm200",
    "jpeg": "-O1 -G6 -MD",
    "exeimg": "-O2 -MD",
}
MODULE_FILES = {"exe": "EE-AOC.exe", "lle": "Low-Level Engine.dll", "default": "Default.dll", "tcp": "TCP Protocol.dll",
                "dx7hr": "DX7HRDisplay.dll", "dx7hrtnl": "DX7HRTnLDisplay.dll",
                "mixer": "Miles Sound System Mixer.dll", "jpeg": "Low-Level Engine.dll", "exeimg": "EE-AOC.exe"}


def module_filename(mod):
    if mod not in MODULE_FILES:
        raise SystemExit(f"unknown module {mod!r}; known: {', '.join(MODULE_FILES)}")
    return MODULE_FILES[mod]


def module_units(mod):
    uf = LINK_DIR / mod / "units.txt"
    if not uf.exists():
        return [(None, None, p) for p in sorted(module_src(mod).glob("*.cpp"))]
    units = []
    for line in open(uf):
        line = line.split("#", 1)[0].strip()
        if line:
            lo, hi, name = line.split(None, 2)
            units.append((int(lo, 16), int(hi, 16), module_src(mod) / name.strip()))
    return units
