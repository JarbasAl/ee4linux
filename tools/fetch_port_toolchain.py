"""Fetch the pinned compilers of the port build into toolchain/ (gitignored). Refuses on sha256 mismatch.

  python3 -I tools/fetch_port_toolchain.py

llvm-mingw (Clang, LLD and libc++ for i686-w64-mingw32, msvcrt runtime) builds the Windows images that run under
Wine. Source: github.com/mstorsjo/llvm-mingw releases.
"""
import hashlib
import pathlib
import sys
import tarfile
import urllib.request

ROOT = pathlib.Path(__file__).resolve().parent.parent
PACKAGES = {
    "llvm-mingw": (
        "https://github.com/mstorsjo/llvm-mingw/releases/download/20261020/"
        "llvm-mingw-20261020-msvcrt-ubuntu-22.04-x86_64.tar.xz",
        "ceb8c5acc6f76e143f9c714772d2fa677027802561ce68c396e8dc990a80f0b6",
    ),
}


def fetch(name, url, pin):
    dest = ROOT / "toolchain" / name
    archive = ROOT / "toolchain" / "dl" / url.rsplit("/", 1)[1]
    if not archive.exists():
        archive.parent.mkdir(parents=True, exist_ok=True)
        part = archive.with_suffix(".part")
        urllib.request.urlretrieve(url, part)
        part.rename(archive)
    digest = hashlib.sha256(archive.read_bytes()).hexdigest()
    if digest != pin:
        sys.exit(f"{name}: sha256 {digest} != pinned {pin}")
    if dest.exists():
        return
    staging = dest.with_name(dest.name + ".extracting")
    with tarfile.open(archive) as tar:
        tar.extractall(staging, filter="data")
    (top,) = staging.iterdir()
    top.rename(dest)
    staging.rmdir()


def main():
    for name, (url, pin) in PACKAGES.items():
        fetch(name, url, pin)
        print(f"{name}: {ROOT / 'toolchain' / name}")


if __name__ == "__main__":
    main()
