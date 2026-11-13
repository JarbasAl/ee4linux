"""Port run: start the port build's images under Wine, from a copy of the installed game (python3 -I tools/port_run.py --help).

  python3 -I tools/port_run.py [--seconds N] [--desktop WxH] [--evidence DIR]

The run never touches the installed game. It copies the Art of Conquest folder to build/port-run/game, puts back the
retail files that an ee-toolkit mod replaced (from the newest ee-toolkit-backups folder that has no .restored mark), and
copies the images of build/port-mingw/bin over the retail ones. The game starts in a Wine virtual desktop window of the
given size, so it cannot change the screen mode. After N seconds the run takes a screenshot of that window, stops the
game and writes the evidence: the exit code, the Wine output, the window title, the screenshot and the list of copied
images with their sha256.

The registry of the default Wine prefix gives the game its base Empire Earth folder, as for the retail exe.
"""
import argparse
import datetime
import hashlib
import json
import os
import pathlib
import shutil
import signal
import subprocess
import sys
import time

ROOT = pathlib.Path(__file__).resolve().parent.parent
PREFIX = pathlib.Path(os.environ.get("WINEPREFIX") or pathlib.Path.home() / ".wine")
DEFAULT_FOLDER = PREFIX / "drive_c" / "Program Files (x86)" / "Empire Earth"


def resolve_case(base, parts):
    """base joined with the parts, each part matched to an existing entry without regard to case, as Windows does"""
    path = base
    for part in parts:
        if not (path / part).exists() and path.is_dir():
            part = next((entry.name for entry in path.iterdir() if entry.name.lower() == part.lower()), part)
        path = path / part
    return path


def installed_folder(key):
    """The folder the Wine prefix's registry gives for a game key ("Installed From Volume" and "Installed From
    Directory" under HKEY_CURRENT_USER), mapped through the prefix's drive links; None when the key is not there."""
    try:
        text = (PREFIX / "user.reg").read_text(errors="replace")
    except OSError:
        return None
    header = "[" + key.replace("\\", "\\\\") + "]"
    start = next((i for i, line in enumerate(text.splitlines()) if line.lower().startswith(header.lower())), None)
    if start is None:
        return None
    values = {}
    for line in text.splitlines()[start + 1:]:
        if line.startswith("["):
            break
        name, _, value = line.partition("=")
        values[name.strip('"')] = value.strip('"').replace("\\\\", "\\")
    volume, directory = values.get("Installed From Volume"), values.get("Installed From Directory")
    if not volume or not directory:
        return None
    drive = PREFIX / "dosdevices" / volume.lower()
    if not drive.exists():
        return None
    return resolve_case(drive.resolve(), [part for part in directory.split("\\") if part])


def game_folder(variable, key, name):
    """A game folder: the environment variable when it is set, else the folder the prefix's registry records, else the
    default install folder in the prefix"""
    if os.environ.get(variable):
        return pathlib.Path(os.environ[variable])
    return installed_folder(key) or DEFAULT_FOLDER / name


# The Art of Conquest folder (EE_GAME_DIR names another) and the base game's (EE_BASE_GAME_DIR)
GAME_KEY = "Software\\Mad Doc Software\\EE-AOC"
BASE_GAME_KEY = "Software\\SSSI\\Empire Earth"
GAME = game_folder("EE_GAME_DIR", GAME_KEY, "Empire Earth - The Art of Conquest")
BASE_GAME = game_folder("EE_BASE_GAME_DIR", BASE_GAME_KEY, "Empire Earth")
IMAGES = ROOT / "build" / "port-mingw" / "bin"
WORK = ROOT / "build" / "port-run"
EVIDENCE = pathlib.Path.home() / "AgentWorkspaces" / "knowledge" / "wiki" / "audits" / "ee-linux"


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def mod_backup(game):
    """The newest ee-toolkit backup that was not restored, or None when the game holds retail files."""
    backups = game / "ee-toolkit-backups"
    if not backups.is_dir():
        return None
    open_ones = sorted(d for d in backups.iterdir() if d.is_dir() and not (d / ".restored").exists())
    return open_ones[-1] if open_ones else None


def prepare(game, work):
    target = work / "game"
    if target.exists():
        shutil.rmtree(target)
    shutil.copytree(game, target, ignore=shutil.ignore_patterns("ee-toolkit-backups"))
    restored = []
    backup = mod_backup(game)
    if backup is not None:
        record = json.loads((backup / "backup.json").read_text())
        for rel, hashes in record["files"].items():
            original = backup / rel
            if sha256(original) != hashes["before"]:
                sys.exit(f"port_run: {original} does not match its recorded sha256")
            shutil.copy2(original, target / rel)
            restored.append(rel)
    images = []
    for image in sorted(IMAGES.iterdir()):
        if image.suffix.lower() in (".exe", ".dll"):
            shutil.copy2(image, target / image.name)
            images.append((image.name, sha256(image)))
    return target, restored, images


def window_of(title_part):
    """The X window id of the Wine desktop, from the root window's client list."""
    out = subprocess.run(["xprop", "-root", "_NET_CLIENT_LIST"], capture_output=True, text=True).stdout
    for wid in out.split("#")[-1].replace(",", " ").split():
        name = subprocess.run(["xprop", "-id", wid, "WM_NAME"], capture_output=True, text=True).stdout
        if title_part in name:
            return wid, name.split("=", 1)[-1].strip()
    return None, None


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--seconds", type=int, default=60, help="how long the game runs before the screenshot and stop")
    ap.add_argument("--desktop", default="1024x768", help="size of the Wine virtual desktop window")
    ap.add_argument("--evidence", type=pathlib.Path, help="evidence folder (default: the ee-linux audits folder)")
    args = ap.parse_args()

    if not (IMAGES / "EE-AOC.exe").exists():
        sys.exit(f"port_run: no {IMAGES / 'EE-AOC.exe'}; run tools/build_port.py --link first")
    stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    evidence = (args.evidence or EVIDENCE) / f"run-{stamp}"
    evidence.mkdir(parents=True)
    target, restored, images = prepare(GAME, WORK)
    print(f"port_run: game copied to {target}; {len(restored)} retail files put back, {len(images)} images copied")

    desktop = f"eeport{stamp}"
    env = dict(os.environ, WINEDEBUG="err+all,fixme-all")
    log = (evidence / "wine.log").open("w")
    exe = subprocess.run(["winepath", "-w", str(target / "EE-AOC.exe")], capture_output=True, text=True,
                         check=True).stdout.strip()   # explorer starts a program only by its full Windows path
    proc = subprocess.Popen(["wine", "explorer", f"/desktop={desktop},{args.desktop}", exe], cwd=target,
                            env=env, stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
    deadline = time.monotonic() + args.seconds
    while time.monotonic() < deadline and proc.poll() is None:
        time.sleep(1)
    exited = proc.poll()
    wid, title = window_of(desktop)
    shot = None
    if wid is not None:
        shot = evidence / "screen.png"
        subprocess.run(["import", "-window", wid, str(shot)], capture_output=True)
    if exited is None:
        os.killpg(proc.pid, signal.SIGTERM)
        try:
            proc.wait(timeout=15)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL)
            proc.wait()
    log.close()

    result = {
        "started": stamp,
        "seconds": args.seconds,
        "exit_code": exited,
        "still_running_at_deadline": exited is None,
        "window": title,
        "screenshot": shot.name if shot is not None and shot.exists() else None,
        "retail_files_put_back": restored,
        "images": dict(images),
    }
    (evidence / "result.json").write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result, indent=2))
    print(f"port_run: evidence in {evidence}")


if __name__ == "__main__":
    main()
