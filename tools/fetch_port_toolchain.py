"""Fetch the pinned compilers of the port build into toolchain/ (gitignored). Refuses on sha256 mismatch.

  python3 -I tools/fetch_port_toolchain.py

llvm-mingw (Clang, LLD and libc++ for i686-w64-mingw32, msvcrt runtime) builds the Windows images that run under
Wine. Source: github.com/mstorsjo/llvm-mingw releases.

The native Linux build needs a 32-bit SDL2, which Arch packages only in multilib. The script builds it from the pinned
source release (github.com/libsdl-org/SDL) with the system clang -m32 into toolchain/sdl2-i686. SDL loads X11, ALSA
and PulseAudio at run time; Wayland, PipeWire, JACK and KMS/DRM are off, because their 32-bit libraries are absent.
"""
import os
import subprocess
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
    "sdl2-src": (
        "https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-2.32.10.tar.gz",
        "5f5993c530f084535c65a6879e9b26ad441169b3e25d789d83287040a9ca5165",
    ),
}
SDL_OPTIONS = ["-DSDL_SHARED=ON", "-DSDL_STATIC=OFF", "-DSDL_TEST=OFF", "-DSDL_WAYLAND=OFF", "-DSDL_PIPEWIRE=OFF",
               "-DSDL_JACK=OFF", "-DSDL_KMSDRM=OFF", "-DSDL_VULKAN=OFF", "-DSDL_X11=ON", "-DSDL_OPENGL=ON",
               "-DSDL_ALSA=ON", "-DSDL_PULSEAUDIO=ON"]


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


def build_sdl():
    """SDL2 for i686 Linux, installed into toolchain/sdl2-i686."""
    prefix = ROOT / "toolchain" / "sdl2-i686"
    if (prefix / "lib" / "libSDL2.so").exists():
        return prefix
    build = ROOT / "toolchain" / "sdl2-build"
    env = dict(os.environ, CC="clang", CFLAGS="-m32", LDFLAGS="-m32", PKG_CONFIG_LIBDIR="/usr/lib32/pkgconfig")
    subprocess.run(["cmake", "-S", ROOT / "toolchain" / "sdl2-src", "-B", build, "-DCMAKE_BUILD_TYPE=Release",
                    f"-DCMAKE_INSTALL_PREFIX={prefix}", *SDL_OPTIONS], env=env, check=True)
    subprocess.run(["cmake", "--build", build, "-j2"], env=env, check=True)
    subprocess.run(["cmake", "--install", build], env=env, check=True)
    return prefix


def main():
    for name, (url, pin) in PACKAGES.items():
        fetch(name, url, pin)
        print(f"{name}: {ROOT / 'toolchain' / name}")
    print(f"sdl2-i686: {build_sdl()}")


if __name__ == "__main__":
    main()
