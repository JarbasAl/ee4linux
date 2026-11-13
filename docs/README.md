# The native Linux build

This folder documents the native Linux build of *Empire Earth: The Art of Conquest*: the game's reconstructed source,
compiled for i386 Linux — and, with `--target linux64`, for x86-64 — drawing and playing through a Win32 layer
instead of Windows.

The Windows images that the reconstruction produces are documented in the reconstruction's own README, upstream at
[Protryon/empire_earth](https://github.com/Protryon/empire_earth). This folder covers the port only.

## What it is

The game and its plug-ins are source code that compiles to the retail machine code with Visual C++ 6. The port
compiles the same source with Clang for i386 Linux. Wine's headers declare the Win32 types and functions, and
`src/Port/Linux/` implements the part of Win32 that the game, its plug-ins and its libraries call:

| Part of Windows | The port's answer | Page |
| --- | --- | --- |
| Threads, events, mutexes, critical sections, waits | POSIX threads and a condition variable | [threads](threads-and-sync.md) |
| Windows, messages, focus, cursors, the clipboard | SDL2 | [windows](windows-and-messages.md) |
| DirectDraw 7 | OpenGL 3.3 core, one texture per surface | [DirectDraw](display-directdraw.md) |
| Direct3D 7 | OpenGL 3.3 core, one GLSL program | [Direct3D](display-direct3d.md) |
| GDI, TrueType text | software drawing, stb_truetype | [GDI](gdi-and-fonts.md) |
| DirectInput 7 | SDL keyboard and mouse | [input](input.md) |
| Miles Sound System 6 | SDL audio, FFmpeg for MP3 | [sound](sound-miles.md) |
| DirectMusic | GothicKit/dmusic and a SoundFont | [music](music-directmusic.md) |
| Bink | FFmpeg | [movies](movies-bink.md) |
| Files, paths, finds, the registry | POSIX, and a `.reg` file in the game folder | [files](files-paths-and-registry.md) |
| Winsock 2, WON | BSD sockets | [network](network.md) |
| The C++ runtime | libstdc++, with MSVC's cast rule | [C++ runtime](cxx-runtime.md) |
| Modules and resources | `dlopen`, and a PE resource reader | [architecture](architecture.md) |

## Quick start

The native build needs an installed *Empire Earth* and *The Art of Conquest*, in the default Wine prefix
(`~/.wine`) or in folders that `EE_GAME_DIR` and `EE_BASE_GAME_DIR` name ([run](run.md)). The game data is never
modified.

```
python3 -I tools/fetch_port_toolchain.py          # once: Clang target files, 32-bit SDL2, FFmpeg, NASM, a SoundFont
python3 -I tools/build_port.py --target linux --link
python3 -I tools/port_linux_folder.py
cd build/port-linux/run && ./ee-aoc
```

`EE_PORT_WINDOWED=1 ./ee-aoc` keeps the window at the game's own size instead of covering the desktop.

## Pages

| Page | Subject |
| --- | --- |
| [build](build.md) | the toolchain, the modules, the images, and how a unit compiles |
| [run](run.md) | the run folder, the settings file, and every environment variable |
| [architecture](architecture.md) | how the images and the layer fit together, and where the code lives |
| [threads](threads-and-sync.md) | kernel objects, waits, threads, message queues, and the SDL lock |
| [windows](windows-and-messages.md) | windows, messages, full screen, cursors, the clipboard |
| [DirectDraw](display-directdraw.md) | surfaces, blits, colour keys, presents, display modes |
| [Direct3D](display-direct3d.md) | devices, recorded draws, the fixed-function pipeline |
| [GDI](gdi-and-fonts.md) | bitmaps, device contexts, text, and the font folders |
| [input](input.md) | DirectInput, key codes, buffered devices, relative mouse |
| [sound](sound-miles.md) | the Miles mixer, samples, MP3, drivers |
| [music](music-directmusic.md) | DirectMusic, the composer, the SoundFont |
| [movies](movies-bink.md) | Bink playback and replacement movies |
| [files](files-paths-and-registry.md) | Windows paths, files, and the settings file |
| [network](network.md) | Winsock, adapters, and the multiplayer libraries |
| [C++ runtime](cxx-runtime.md) | the dialect gaps, casts, callbacks, and the C library |
| [testing](testing.md) | the test harness, every test, and the checks against other programs |
| [simulator](simulator.md) | `ee-aoc --sim`: seeded computer games without a display, and their comparison |
| [Age of AI](ageofai.md) | the port of Grille's AI mod, and the index of its hook sites in this tree |
| [troubleshooting](troubleshooting.md) | the failures seen so far, and what each one means |
| [limits](limits.md) | what the port does not do |
