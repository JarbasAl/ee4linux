"""Fetch the pinned compilers of the port build into toolchain/ (gitignored). Refuses on sha256 mismatch.

  python3 -I tools/fetch_port_toolchain.py [--target all|mingw|linux|linux64]

llvm-mingw (Clang, LLD and libc++ for i686-w64-mingw32, msvcrt runtime) builds the Windows images that run under
Wine. Source: github.com/mstorsjo/llvm-mingw releases.

The native Linux build needs a 32-bit SDL2, which Arch packages only in multilib. The script builds it from the pinned
source release (github.com/libsdl-org/SDL) with the system clang -m32 into toolchain/sdl2-i686. SDL loads X11, ALSA
and PulseAudio at run time; Wayland, PipeWire, JACK and KMS/DRM are off, because their 32-bit libraries are absent.

The 64-bit target (`--target linux64`) gets its own SDL2 and FFmpeg from the same pins, into `toolchain/sdl2-x86_64`
and `toolchain/ffmpeg-x86_64`: Arch's own SDL2 package is sdl2-compat, a shim over SDL3, and the layer wants the same
SDL2 on both targets.

Movies play through a 32-bit FFmpeg, built from the pinned release (ffmpeg.org) into toolchain/ffmpeg-i686. It reads
the game's Bink 1 files and replacement movies in MP4, Matroska/WebM, AVI or MPEG with H.264, HEVC, VP8, VP9 or MPEG
video. The build is LGPL only (no --enable-gpl, no external libraries). Its x86 assembly needs NASM, built from the
pinned release (nasm.us) into toolchain/nasm. The NASM pin is the sha256 of the archive whose sha512 Arch's nasm
package records.

DirectMusic plays the instruments that the game's bands take from Windows' General MIDI collection (gm.dls, which no
one may redistribute) from GeneralUser GS 2.0.3 by S. Christian Collins, a General MIDI SoundFont whose licence allows
use and redistribution in software. The script puts it and its licence in toolchain/soundfonts, from the author's
repository (github.com/mrbumpy409/GeneralUser-GS) at a pinned commit.
"""
import argparse
import os
import subprocess
import hashlib
import pathlib
import sys
import tarfile
import urllib.request

ROOT = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from build_port import CLANG
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
    "nasm-src": (
        "https://www.nasm.us/pub/nasm/releasebuilds/3.02/nasm-3.02.tar.xz",
        "87336eba53b4acfe917424ab5d500d2b0054d9f5148d35c2273ccf2cfb712f0d",
    ),
    "ffmpeg-src": (
        "https://ffmpeg.org/releases/ffmpeg-7.1.1.tar.xz",
        "733984395e0dbbe5c046abda2dc49a5544e7e0e1e2366bba849222ae9e3a03b1",
    ),
}
GENERALUSER = ("https://raw.githubusercontent.com/mrbumpy409/GeneralUser-GS/"
               "684543d5e5efaef08d02be50dcda8d552478fa60/")
FILES = {
    "soundfonts/GeneralUser-GS.sf2": (
        GENERALUSER + "GeneralUser-GS.sf2", "9575028c7a1f589f5770fccc8cff2734566af40cd26ed836944e9a5152688cfe"),
    "soundfonts/GeneralUser-GS-LICENSE.txt": (
        GENERALUSER + "documentation/LICENSE.txt", "7b32efefdf95ce38a043799f0659853ddc00fbaa14d8c50f0aca16b9b8b405be"),
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


def fetch_file(name, url, pin):
    dest = ROOT / "toolchain" / name
    if not dest.exists():
        dest.parent.mkdir(parents=True, exist_ok=True)
        part = dest.with_name(dest.name + ".part")
        urllib.request.urlretrieve(url, part)
        part.rename(dest)
    digest = hashlib.sha256(dest.read_bytes()).hexdigest()
    if digest != pin:
        sys.exit(f"{name}: sha256 {digest} != pinned {pin}")


def pkgconfig_32():
    """The folder of the 32-bit libraries' pkg-config files: /usr/lib32 (Arch), /usr/lib/i386-linux-gnu (Debian,
    Ubuntu), or /usr/lib where the 64-bit libraries are in /usr/lib64 (Gentoo, Fedora). EE_PKG_CONFIG_32 names another."""
    if os.environ.get("EE_PKG_CONFIG_32"):
        return pathlib.Path(os.environ["EE_PKG_CONFIG_32"])
    candidates = [pathlib.Path("/usr/lib32/pkgconfig"), pathlib.Path("/usr/lib/i386-linux-gnu/pkgconfig")]
    if pathlib.Path("/usr/lib64").is_dir() and not pathlib.Path("/usr/lib64").is_symlink():
        candidates.append(pathlib.Path("/usr/lib/pkgconfig"))
    for folder in candidates:
        if folder.is_dir():
            return folder
    sys.exit("fetch_port_toolchain: no 32-bit pkg-config folder found; set EE_PKG_CONFIG_32, or use --target linux64")


def build_sdl(arch="i686"):
    """SDL2 for Linux, installed into toolchain/sdl2-<arch>: i686 for the 32-bit build, x86_64 for the 64-bit one."""
    prefix = ROOT / "toolchain" / f"sdl2-{arch}"
    if (prefix / "lib" / "libSDL2.so").exists():
        return prefix
    build = ROOT / "toolchain" / f"sdl2-build-{arch}"
    if arch == "i686":
        env = dict(os.environ, CC=str(CLANG), CFLAGS="-m32", LDFLAGS="-m32", PKG_CONFIG_LIBDIR=str(pkgconfig_32()))
    else:
        env = dict(os.environ, CC=str(CLANG))
    subprocess.run(["cmake", "-S", ROOT / "toolchain" / "sdl2-src", "-B", build, "-DCMAKE_BUILD_TYPE=Release",
                    f"-DCMAKE_INSTALL_PREFIX={prefix}", *SDL_OPTIONS], env=env, check=True)
    subprocess.run(["cmake", "--build", build, "-j2"], env=env, check=True)
    subprocess.run(["cmake", "--install", build], env=env, check=True)
    return prefix


FFMPEG_OPTIONS = [
    "--enable-shared", "--disable-static", "--enable-pic", "--disable-programs", "--disable-doc", "--disable-network",
    "--disable-autodetect", "--disable-everything", "--disable-avdevice", "--disable-avfilter",
    "--enable-swscale", "--enable-swresample", "--enable-protocol=file",
    "--enable-demuxer=bink,mov,matroska,avi,mpegps,mpegvideo,ogg",
    "--enable-decoder=bink,binkaudio_dct,binkaudio_rdft,h264,hevc,vp8,vp9,mpeg1video,mpeg2video,mpeg4,"
    "aac,mp2,mp3,opus,vorbis,flac,pcm_s16le,pcm_u8",
    "--enable-parser=h264,hevc,vp8,vp9,mpegvideo,mpeg4video,aac,mpegaudio,opus,vorbis,flac",
]


def build_nasm():
    """NASM for the host, installed into toolchain/nasm."""
    prefix = ROOT / "toolchain" / "nasm"
    if (prefix / "bin" / "nasm").exists():
        return prefix
    source = ROOT / "toolchain" / "nasm-src"
    subprocess.run(["./configure", f"--prefix={prefix}"], cwd=source, check=True)
    subprocess.run(["make", "-j2", "nasm", "ndisasm"], cwd=source, check=True)
    (prefix / "bin").mkdir(parents=True, exist_ok=True)
    for tool in ("nasm", "ndisasm"):
        (prefix / "bin" / tool).write_bytes((source / tool).read_bytes())
        (prefix / "bin" / tool).chmod(0o755)
    return prefix


def build_ffmpeg(nasm, arch="i686"):
    """FFmpeg's libraries for Linux, installed into toolchain/ffmpeg-<arch>."""
    prefix = ROOT / "toolchain" / f"ffmpeg-{arch}"
    if (prefix / "lib" / "libavcodec.so").exists():
        return prefix
    source = ROOT / "toolchain" / "ffmpeg-src"
    target = ["--arch=x86_32", "--target-os=linux", f"--cc={CLANG}", "--extra-cflags=-m32", "--extra-ldflags=-m32"]
    if arch == "x86_64":
        # a separate copy of the source tree, without the 32-bit build's objects and configuration
        source = ROOT / "toolchain" / "ffmpeg-src-x86_64"
        if not (source / "libavutil").exists():
            import shutil
            shutil.copytree(ROOT / "toolchain" / "ffmpeg-src", source, symlinks=True)
            for pattern in ("*.o", "*.d", "*.a", "*.so*"):
                for stale in source.rglob(pattern):
                    if stale.is_file():
                        stale.unlink()
            for stale in ("config.h", "config.asm", "config.mak", "ffbuild/config.mak", "ffbuild/config.fate",
                          "ffbuild/config.log", "ffbuild/config.sh"):
                (source / stale).unlink(missing_ok=True)
        target = ["--arch=x86_64", "--target-os=linux", f"--cc={CLANG}"]
    subprocess.run(["./configure", f"--prefix={prefix}", f"--x86asmexe={nasm / 'bin' / 'nasm'}", *target,
                    *FFMPEG_OPTIONS], cwd=source, check=True)
    subprocess.run(["make", "-j2"], cwd=source, check=True)
    subprocess.run(["make", "install"], cwd=source, check=True)
    return prefix


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--target", choices=["all", "mingw", "linux", "linux64"], default="all",
                    help="fetch only what one target needs (default all): linux64 needs no 32-bit libraries")
    args = ap.parse_args()
    wanted = {"all": {"mingw", "linux", "linux64"}}.get(args.target, {args.target})
    for name, (url, pin) in PACKAGES.items():
        if name == "llvm-mingw" and "mingw" not in wanted:
            continue
        fetch(name, url, pin)
        print(f"{name}: {ROOT / 'toolchain' / name}")
    for name, (url, pin) in FILES.items():
        fetch_file(name, url, pin)
        print(f"{name}: {ROOT / 'toolchain' / name}")
    arches = [arch for arch, target in (("i686", "linux"), ("x86_64", "linux64")) if target in wanted]
    if arches:
        nasm = build_nasm()
    for arch in arches:
        print(f"sdl2-{arch}: {build_sdl(arch)}")
        print(f"ffmpeg-{arch}: {build_ffmpeg(nasm, arch)}")


if __name__ == "__main__":
    main()
