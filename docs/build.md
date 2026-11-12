# Building the native images

`tools/build_port.py` compiles the game's units for a target and links the images. The native target is `linux`; the
`mingw` target builds Windows images that run under Wine.

```
python3 -I tools/build_port.py --help
python3 -I tools/build_port.py --target linux            # compile every native module
python3 -I tools/build_port.py --target linux --link     # compile, then link the seven images
python3 -I tools/build_port.py --target linux -j 8       # more compilers at once (default 2)
```

`--link` needs every module of the target compiled, so it takes no module names.

## The toolchain

`tools/fetch_port_toolchain.py` puts the toolchain in `toolchain/`, which git ignores. It refuses an archive whose
sha256 does not match the pin. It fetches:

| Part | What it is for |
| --- | --- |
| `llvm-mingw` | Clang, LLD and libc++ for `i686-w64-mingw32`; the `mingw` target |
| `sdl2-i686` | a 32-bit SDL2 built from the pinned source release; Arch packages it only in multilib |
| `ffmpeg-i686` | a 32-bit FFmpeg, LGPL only, for Bink movies and MP3 |
| `nasm` | the assembler FFmpeg's x86 code needs |
| `soundfonts` | GeneralUser GS 2.0.3, whose licence allows redistribution in software |

The native target uses the system `/usr/bin/clang++` for `-m32`, so a 32-bit compiler runtime and glibc are needed:
`lib32-glibc`, `lib32-gcc-libs` and `lib32-libstdc++` on Arch.

The 32-bit SDL2 is built without Wayland, PipeWire, JACK and KMS/DRM, because their 32-bit libraries are absent. SDL
loads X11, ALSA and PulseAudio at run time.

## Modules

A module is a group of units with one set of flags, as in the VC6 build (`tools/ee.py`, `tools/verify.py`). The unit
lists, the module flags and the per-unit flags come from that build; only the `-D` options of the flags are used.

| Module | Units | Goes into |
| --- | --- | --- |
| `exe` | the game (`src/Empire Earth/`, `src/RTS Engine/`) | `ee-aoc` |
| `won` | the WON library inside the game's executable | `ee-aoc` |
| `exeimg` | libpng, zlib, libjpeg inside the executable | `ee-aoc` |
| `lle` | the engine (`src/Low-Level Engine/`) | `Low-Level Engine.dll` |
| `dx7hrtnl` | the T&L renderer plug-in | `DX7HRTnLDisplay.dll` |
| `tcp` | the network plug-in | `TCP Protocol.dll` |
| `default` | the random map generators | `Default.dll` |
| `mixer` | the Miles mixer plug-in | `Miles Sound System Mixer.dll` |
| `win32` | the port's Win32 layer (`src/Port/Linux/`) | `libeeport.so` |
| `dmusic` | GothicKit/dmusic and TinySoundFont | `libeeport.so` |

Three units exist only in the port build:

- `src/Port/EEPortLinks.cpp` and `src/Port/EEPortWonLinks.cpp` give the link the names that the VC6 link took from
  the original image.
- `src/Port/EEPortLinux.cpp` is the entry point (`main`, which calls `WinMain`) and the GameSpy stubs.
- `src/Port/Linux/*.cpp` is the Win32 layer.

## The images

The native link produces ELF files that keep the Windows file names, because the engine loads its plug-ins from the
`*.dll` files in its folder:

| File | What it is |
| --- | --- |
| `ee-aoc` | the game, position independent, with `main` instead of the startup code |
| `Low-Level Engine.dll` | the engine, a shared object |
| `DX7HRTnLDisplay.dll` | the T&L renderer plug-in, a shared object |
| `TCP Protocol.dll` | the network plug-in, a shared object |
| `Default.dll` | the random map generators, a shared object |
| `Miles Sound System Mixer.dll` | the sound plug-in, a shared object |
| `libeeport.so` | the Win32 layer, linked by every image |
| `libSDL2-2.0.so.0`, `libav*.so.*` | the libraries the layer needs, copied beside the images |

Each image binds its own names to its own definitions (`-Wl,-Bsymbolic`), as a Windows DLL does, so the engine's data
stays in the engine. The executable is position independent (`-pie`). The plug-ins and the engine are linked with
`-Wl,-z,defs`, so a missing definition fails the link instead of the game.

## How a unit compiles

A unit is one source file. It compiles with the target's flags and the defines of its module, plus:

- `-DEE_PORT` on every unit of every target, and `-DEE_PORT_LINUX` on the native target;
- `-include include/port/compat.h` first: the gap between the VC6 dialect and a modern compiler
  ([C++ runtime](cxx-runtime.md));
- `-isystem` for Wine's headers, SDL2, FFmpeg and dmusic; `-I` for `include/`, the game's own include folders,
  `include/winsdk`, `include/port/case` and `include/port/linux`.

The unit flags for the native target are `-m32 -fPIC -O1 -march=i686 -mfpmath=387 -fasm-blocks -fdeclspec
-fdelayed-template-parsing -fno-strict-aliasing -std=gnu++14`, with the compiler's warnings off: the source is a
reconstruction of VC6 output, not new code.

Two consequences of the source:

- VC6 narrows in braced initializers, so `-Wno-c++11-narrowing`.
- VC6 accepts `&Temporary(...)` as an argument, so `-Wno-address-of-temporary`.

The `dmusic` module is C, compiled separately with its own flags and a C11-threads shim over POSIX threads.

## Incremental builds

An object compiles again when it is older than its unit or than any header in its dependency file, or when its
compile command differs from the one stored beside it. The build writes `build/port-linux/logs/<module>.log` with the
full compiler output, and prints the first error of a failed unit.

Output layout:

| Path | Contents |
| --- | --- |
| `build/port-linux/obj/<module>/…` | one object per unit, mirroring `src/` |
| `build/port-linux/bin/` | the images and the runtime libraries |
| `build/port-linux/lib/` | import libraries for the DLLs and the archived modules |
| `build/port-linux/link/` | the response files and link logs |
| `build/port-linux/logs/` | the compiler and linker output |
| `build/port-linux/run/` | the run folder ([run](run.md)) |

## Loading the retail resources

Three files of the retail installation are read, not rebuilt:

- `EE-AOC.exe`: its resources hold the start screen's bitmap and the game's cursors and icons. The native executable
  stays in the run folder for this reason.
- `Language.dll`: its string table ([GDI](gdi-and-fonts.md) and [architecture](architecture.md)).
- A retail plug-in beside the native ones: the engine's plug-in scan loads every `*.dll` in the folder and keeps the
  ones that export its entry points, so a retail image without them is skipped.

`EE_PORT_EXE_RESOURCES` names another executable whose resources the layer should use.
