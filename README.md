# Empire Earth: The Art of Conquest - community edition

The community edition of *Empire Earth: The Art of Conquest* (Stainless Steel Studios / Sierra, 2002) runs natively
on Linux, as a 64-bit (x86-64) or a 32-bit (i386) program. It is built from the source reconstruction of
[Protryon/empire_earth](https://github.com/Protryon/empire_earth), which compiles with the original toolchain to the
retail machine code. A Win32 layer on SDL2, OpenGL, FFmpeg and a SoundFont replaces Windows and DirectX.

`dev` is the branch where the game changes. It takes improvements, engine changes and fixes that a faithful
reconstruction must refuse. It may therefore change the simulation, the AI, the save format and the network traffic.
Play multiplayer only with players of the same `dev` build. A `dev` save can stop loading in the retail game.

This repository contains no game data and no original binaries. You need a legal copy of the game, with *The Art of
Conquest* installed, to play anything you build.

## Branches

| Branch | What it is | Changes |
| --- | --- | --- |
| `dev` | the community edition, and the default branch | all new work |
| `linux-x64` | the native x86-64 port of the reconstruction | frozen |
| `linux-base-32bit` | the native i386 port of the reconstruction | frozen |

The two frozen branches keep the reconstruction's rule: a source edit for the port leaves the Visual C++ 6 build's
output unchanged, so the port stays the retail game compiled for Linux. `dev` does not keep that rule.

The reconstruction itself, with the byte-identical `main` and the compatible bug fixes of `fixes`, is upstream.

## Build and run

The build runs on Linux with Clang and Python 3 (standard library only). Wine's headers declare the Win32 types and
functions; the build does not run Wine.

```
python3 -I tools/fetch_port_toolchain.py                     # SDL2, FFmpeg and a SoundFont, into toolchain/
python3 -I tools/build_port.py --target linux64 --link        # the seven images, in build/port-linux64/bin
python3 -I tools/port_linux_folder.py --target linux64        # a copy of the installed game with the images
cd build/port-linux64/run && ./ee-aoc
```

For the 32-bit build, use `--target linux`. Its images go to `build/port-linux/bin` and its run folder is
`build/port-linux/run`. The run folder is a copy, so the installed game does not change.

[docs/](docs/README.md) documents the port: the toolchain, every part of Windows the layer replaces, the tests, and
what the port does not do ([limits](docs/limits.md)).

## Tests

```
python3 -I tools/test_port_linux.py --target linux64          # the layer's tests and checks
python3 -I tools/test_long32.py                               # the x86-64 build's 32-bit long rewrite
```

[testing](docs/testing.md) describes them, and how to play the whole game headless from a script of timed events.

## License

No license is granted. Empire Earth is the property of its rights holders. The third-party libraries under
`src/ThirdParty/` and `src/Port/Linux/third_party/` keep their own licenses. The repository is private, because the
reconstruction it is built from grants no license.
