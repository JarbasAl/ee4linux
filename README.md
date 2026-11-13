# Empire Earth: The Art of Conquest on Linux - community edition

*Empire Earth: The Art of Conquest* (Stainless Steel Studios / Sierra, 2002) as a native Linux program, 64-bit or
32-bit, with no Wine at run time. The game, its engine and its plug-ins are compiled from source, and a small Win32
layer on SDL2, OpenGL and FFmpeg takes the place of Windows and DirectX.

The source comes from the reconstruction at [Protryon/empire_earth](https://github.com/Protryon/empire_earth): C++
that compiles, with the original Visual C++ 6 toolchain, to the retail game's machine code. This repository compiles
that source for Linux, and its `dev` branch is where the game itself is improved.

This repository contains no game data and no original binaries. You need your own copy of *Empire Earth* with *The
Art of Conquest*.

## Branches

| Branch | What it is | Plays multiplayer with |
| --- | --- | --- |
| **`dev`** (default) | the community edition: the Linux port, plus fixes and improvements to the game | the same `dev` build only |
| `linux-x64` | the faithful 64-bit Linux port. Frozen. | the same 64-bit build. Its float rounding differs from the 32-bit build's, so mixed games are expected to go out of sync. |
| `linux-base-32bit` | the faithful 32-bit Linux port. Frozen. | the retail game, by design (not yet tested on Linux) |

The two frozen branches are the retail game compiled for Linux and nothing else. Every source edit they need keeps the
Visual C++ 6 build's output identical, and they take no new commits. Use them when you want the original game exactly.

`dev` changes the game. It may change the simulation, the AI, the save format and the network traffic, so play
multiplayer only with players of the same `dev` build. A `dev` save can stop loading in the retail game. The faithful
reconstruction itself, with its byte-identical `main` and compatible `fixes` branches, is upstream.

```
git switch linux-x64        # or linux-base-32bit, or dev
```

## What changes for Linux

The game's own code is the same on Linux: the simulation, the AI, the interface and the file formats. What changes is
everything under it. The game calls Windows and DirectX, and `src/Port/Linux/` answers those calls:

| Windows part | On Linux |
| --- | --- |
| DirectDraw 7 and Direct3D 7 | OpenGL 3.3 core |
| GDI and TrueType text | software drawing with stb_truetype, and metric-compatible fonts (Liberation for Arial) |
| Windows and messages | SDL2 |
| DirectInput 7 | SDL keyboard and mouse |
| Miles Sound System | SDL audio, and FFmpeg for MP3 |
| DirectMusic | GothicKit/dmusic with the GeneralUser GS SoundFont |
| Bink movies | FFmpeg |
| Files and paths | POSIX, with Windows paths and case-insensitive names |
| The registry | a `.reg` file in the game folder, imported from Wine on the first start |
| Winsock | BSD sockets |
| The game's `.dll` plug-ins | ELF shared objects with the same names, loaded with `dlopen` |

What you will notice as a player:

- The game runs in a window that covers the desktop and scales the game's resolution to it. It never changes your
  screen's mode. `EE_PORT_WINDOWED=1` gives a window at the game's own size.
- The music comes from a SoundFont, so it sounds a little different from Windows' synthesizer.
- The online services (WON, GameSpy) are gone, as they are on Windows. LAN and Direct Connect are the multiplayer
  paths.
- Your installed game is never changed. The game runs from a copy of its folder.

The 64-bit build needs two more changes, both invisible in play. Windows' `long` is 32 bits and Linux x86-64's is 64,
so the build compiles a copy of the source with `long` as a 32-bit type, and the game's records, saves and packets
keep their shapes. The 64-bit build also computes floats in SSE instead of the x87 unit, so the few places where the game
relied on x87's wider range are computed in double.

[docs/](docs/README.md) documents the port: every replaced part, the tests, and what the port does not do
([limits](docs/limits.md)).

## What works

- Both builds start, show the menus, and play scenarios and random maps.
- Save and load work.
- The tests check the sound, the music and the movies against FFmpeg's own decoding.

Not tested yet: a network game between native builds. Reports are welcome.

## Build and run

You need:

- Linux on x86-64 with X11, or Wayland with XWayland.
- Clang, Python 3, CMake and make.
- The `wine` package, for its Windows headers only (`/usr/include/wine/windows`). The game itself does not run in
  Wine.
- *Empire Earth* with *The Art of Conquest*, installed in a Wine prefix (`~/.wine` by default, or set `WINEPREFIX`).
  The game reads its settings and its base folder from that prefix on the first start.
- For the 32-bit build only: the 32-bit glibc and C++ runtime (on Arch: `lib32-glibc`, `lib32-gcc-libs`,
  `lib32-libstdc++`).

The 64-bit build:

```
python3 -I tools/fetch_port_toolchain.py                     # once: builds SDL2 and FFmpeg into toolchain/
python3 -I tools/build_port.py --target linux64 --link -j 8   # the game, in build/port-linux64/bin
python3 -I tools/port_linux_folder.py --target linux64        # a copy of your installed game with it
cd build/port-linux64/run && ./ee-aoc
```

For the 32-bit build, use `--target linux`. Its files go to `build/port-linux/bin` and `build/port-linux/run`.

The toolchain step checks every download against a pinned sha256. A full build compiles about 1,450 units, so give it
the cores you have with `-j`. [build](docs/build.md) and [run](docs/run.md) give every option and environment
variable, and [troubleshooting](docs/troubleshooting.md) the known failures.

## Tests

```
python3 -I tools/test_port_linux.py --target linux64          # the Win32 layer's tests and checks
python3 -I tools/test_long32.py                               # the 64-bit build's long rewrite
```

[testing](docs/testing.md) describes them, and how to play the whole game headless from a script of timed key presses.

## Credits

- The source reconstruction: [Protryon/empire_earth](https://github.com/Protryon/empire_earth).
- [SDL2](https://www.libsdl.org/), [FFmpeg](https://ffmpeg.org/) (LGPL build),
  [GothicKit/dmusic](https://github.com/GothicKit/dmusic), TinySoundFont, stb_truetype, and the GeneralUser GS
  SoundFont by S. Christian Collins.

## License

No license is granted. Empire Earth is the property of its rights holders. The third-party libraries under
`src/ThirdParty/` and `src/Port/Linux/third_party/` keep their own licenses. The reconstruction this repository builds
on grants no license either.
