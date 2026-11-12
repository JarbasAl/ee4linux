"""Native run folder: make a copy of the installed game that runs the native Linux build (python3 -I tools/port_linux_folder.py --help).

  python3 -I tools/port_linux_folder.py [--keep-mod] [--out DIR]

The folder never touches the installed game. The tool copies the Art of Conquest folder to build/port-linux/run (a
reflink copy where the file system allows it), puts back the retail files that an ee-toolkit mod replaced unless
--keep-mod is given, copies the native images and their libraries from build/port-linux/bin over the retail DLLs of
the same names, and adds the General MIDI SoundFont as general-midi.sf2. The retail EE-AOC.exe stays: the native
build reads its resources. The tool prints the command that starts the game; it does not start it.

Settings go to ee-port-registry.reg in the folder. The first start imports them, and the base Empire Earth folder,
from the default Wine prefix.
"""
import argparse
import json
import pathlib
import shutil
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from port_run import GAME, mod_backup, sha256

ROOT = pathlib.Path(__file__).resolve().parent.parent
IMAGES = ROOT / "build" / "port-linux" / "bin"
SOUNDFONT = ROOT / "toolchain" / "soundfonts" / "GeneralUser-GS.sf2"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--keep-mod", action="store_true", help="keep the installed ee-toolkit mod's files")
    ap.add_argument("--out", type=pathlib.Path, default=ROOT / "build" / "port-linux" / "run",
                    help="the run folder (default build/port-linux/run)")
    args = ap.parse_args()

    if not (IMAGES / "ee-aoc").exists():
        sys.exit(f"port_linux_folder: no {IMAGES / 'ee-aoc'}; run tools/build_port.py --target linux --link first")
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

    mod = "kept" if backup is not None and args.keep_mod else f"{len(restored)} retail files put back"
    print(f"port_linux_folder: {GAME.name} copied to {args.out}; mod: {mod}; {len(images)} native files copied")
    for image in images:
        print(f"  {image.name}  {sha256(image)[:16]}")
    print("Start the game with:")
    print(f'  cd "{args.out}" && ./ee-aoc')


if __name__ == "__main__":
    main()
