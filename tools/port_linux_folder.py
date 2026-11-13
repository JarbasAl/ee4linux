"""Native run folder: make a copy of the installed game that runs the native Linux build (python3 -I tools/port_linux_folder.py --help).

  python3 -I tools/port_linux_folder.py [--keep-mod] [--out DIR] [--target linux|linux64]

The folder never touches the installed game. The tool copies the Art of Conquest folder to build/port-<target>/run (a
reflink copy where the file system allows it), puts back the retail files that an ee-toolkit mod replaced unless
--keep-mod is given, copies the native images and their libraries from build/port-<target>/bin over the retail DLLs of
the same names, and adds the General MIDI SoundFont as general-midi.sf2. The retail EE-AOC.exe stays: the native
build reads its resources. The tool prints the command that starts the game; it does not start it.

The tool adds config/ageofai.ini, the settings of the Age of AI engine (docs/ageofai.md), unless the game folder has
an ageofai.ini of its own. The file selects the classic AI.

Settings go to ee-port-registry.reg in the folder. The first start imports them, and the base Empire Earth folder,
from the Wine prefix (WINEPREFIX, else ~/.wine). When that prefix does not record the base game, or EE_BASE_GAME_DIR
names it, the tool writes ee-port-registry.reg itself with the two install folders, and the game starts with default
settings. EE_GAME_DIR and EE_BASE_GAME_DIR name game folders outside a Wine prefix (tools/port_run.py).
"""
import argparse
import json
import os
import pathlib
import shutil
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from port_run import BASE_GAME, BASE_GAME_KEY, GAME, GAME_KEY, installed_folder, mod_backup, sha256

ROOT = pathlib.Path(__file__).resolve().parent.parent
IMAGES = ROOT / "build" / "port-linux" / "bin"
SOUNDFONT = ROOT / "toolchain" / "soundfonts" / "GeneralUser-GS.sf2"
AGEOFAI_INI = ROOT / "config" / "ageofai.ini"

REGISTRY_HEADER = "WINE REGISTRY Version 2\n;; The Empire Earth native port's registry (ee-port-registry.reg)\n"


def escaped(text):
    return text.replace("\\", "\\\\").replace('"', '\\"')


def install_keys(folders):
    """ee-port-registry.reg text that gives each key its install folder as a Z: path (Z: is the root, as in Wine)"""
    text = REGISTRY_HEADER
    for key, folder in folders.items():
        name = escaped("HKEY_CURRENT_USER\\" + key)
        directory = escaped(str(folder.resolve()).replace("/", "\\").rstrip("\\") + "\\")
        text += f'\n[{name}]\n"Installed From Directory"="{directory}"\n"Installed From Volume"="Z:"\n'
    return text


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--keep-mod", action="store_true", help="keep the installed ee-toolkit mod's files")
    ap.add_argument("--target", default="linux", choices=["linux", "linux64"], help="which build's images to use")
    ap.add_argument("--out", type=pathlib.Path, help="the run folder (default build/port-<target>/run)")
    args = ap.parse_args()

    global IMAGES
    IMAGES = ROOT / "build" / f"port-{args.target}" / "bin" if args.target != "linux" else IMAGES
    args.out = args.out or ROOT / "build" / f"port-{args.target}" / "run"
    if not (IMAGES / "ee-aoc").exists():
        sys.exit(f"port_linux_folder: no {IMAGES / 'ee-aoc'}; run tools/build_port.py --target {args.target} --link first")
    if not SOUNDFONT.exists():
        sys.exit(f"port_linux_folder: no {SOUNDFONT}; run tools/fetch_port_toolchain.py first")
    if args.out.exists():
        shutil.rmtree(args.out)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    copied = subprocess.run(["cp", "-a", "--reflink=auto", str(GAME), str(args.out)], capture_output=True, text=True)
    if copied.returncode:
        sys.exit(f"port_linux_folder: the copy failed: {copied.stderr.strip()}")
    shutil.rmtree(args.out / "ee-toolkit-backups", ignore_errors=True)

    restored = []
    backup = mod_backup(GAME)
    if backup is not None and not args.keep_mod:
        record = json.loads((backup / "backup.json").read_text())
        for rel, hashes in record["files"].items():
            original = backup / rel
            if sha256(original) != hashes["before"]:
                sys.exit(f"port_linux_folder: {original} does not match its recorded sha256")
            shutil.copy2(original, args.out / rel)
            restored.append(rel)

    images = sorted(p for p in IMAGES.iterdir() if p.is_file())
    for image in images:
        shutil.copy2(image, args.out / image.name)
    shutil.copy2(SOUNDFONT, args.out / "general-midi.sf2")
    ageofai = "the game folder's own"
    if not (args.out / "ageofai.ini").exists():
        shutil.copy2(AGEOFAI_INI, args.out / "ageofai.ini")
        ageofai = f"{AGEOFAI_INI.relative_to(ROOT)} (the classic AI)"

    registry = "imported from the Wine prefix at the first start"
    if os.environ.get("EE_BASE_GAME_DIR") or installed_folder(BASE_GAME_KEY) is None:
        if not BASE_GAME.is_dir():
            sys.exit(f"port_linux_folder: no base Empire Earth folder {BASE_GAME}; set EE_BASE_GAME_DIR")
        (args.out / "ee-port-registry.reg").write_text(
            install_keys({GAME_KEY: args.out, BASE_GAME_KEY: BASE_GAME}))
        registry = f"written with the base game at {BASE_GAME}"

    mod = "kept" if backup is not None and args.keep_mod else f"{len(restored)} retail files put back"
    print(f"port_linux_folder: {GAME} copied to {args.out}; mod: {mod}; {len(images)} native files copied")
    print(f"settings: {registry}")
    print(f"ageofai.ini: {ageofai}")
    for image in images:
        print(f"  {image.name}  {sha256(image)[:16]}")
    print("Start the game with:")
    print(f'  cd "{args.out}" && ./ee-aoc')


if __name__ == "__main__":
    main()
