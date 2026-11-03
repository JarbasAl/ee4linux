"""Fetch the pinned VC6 packages into toolchain/msvc6.x (gitignored). Refuses on sha256 mismatch.

  python -I tools/fetch_toolchain.py [6.0 6.3 ...]   (default: all)

The build uses 6.0 (compiler, LINK, LIB), 6.3 (LIB for the by-name sxlrt233.lib), 6.4 (libjpeg in the engine, Dinkum
new[]/delete[]), 6.5pp (MSS_auto_cleanup) and 7.0 (RC).

Source: the decomp.me win32 compiler packages (github.com/decompme/compilers -> OmniBlade/decomp.me releases).
"""
import hashlib
import pathlib
import sys
import tarfile
import urllib.request

ROOT = pathlib.Path(__file__).resolve().parent.parent
URL = "https://github.com/OmniBlade/decomp.me/releases/download/msvcwin9x/msvc{v}.tar.gz"
PINS = {
    "6.0": "5c81e9c2ab0ac5545022c1418a23392cb514db950cf6dcb1f48327270403fcd3",
    "6.3": "84f73e718b3671bfd5de3b7764622b07633b572ee826ca3b77602d224c128608",
    "6.4": "6d4ef930390ca7481ae5b63ea3bf00d62e0993cd95f1d6f42ba06bb30f993c57",
    "6.5": "16f639b7961ca74b938532d912d2bb655d5ea08b01dc2f50e4a180774d138169",
    "6.6": "0a4cd7628a79067ee62dd90d40d2c027221cc46f530e40d92245ec46d550dd4d",
    "6.5pp": "0de4c5dc848d59b8af842eb157ffe8b1d24fba6621b18be67ea9932e219ce14e",
    "7.0": "49eba7f9f72644f6f872a4176baeac544e8b5dfb2be1f47bb4d439b1136ffeac",
}


def main():
    for v in sys.argv[1:] or PINS:
        dest = ROOT / "toolchain" / f"msvc{v}"
        tgz = ROOT / "toolchain" / "dl" / f"msvc{v}.tar.gz"
        if not tgz.exists():
            tgz.parent.mkdir(parents=True, exist_ok=True)
            urllib.request.urlretrieve(URL.format(v=v), tgz)
        h = hashlib.sha256(tgz.read_bytes()).hexdigest()
        if h != PINS[v]:
            sys.exit(f"msvc{v}: sha256 {h} != pinned {PINS[v]}")
        if not dest.exists():
            dest.mkdir(parents=True)
            with tarfile.open(tgz) as t:
                t.extractall(dest, filter="data")
        print(f"msvc{v}: ok ({dest.relative_to(ROOT)})")


if __name__ == "__main__":
    main()
