# The Win32 layer of the native Linux build

The native build compiles the game's own source against Wine's headers and links it to this layer in place of
Windows. The layer implements the part of the Win32 API that the game, its plug-ins and WON call. It has files and
paths, the registry, threads and synchronization, modules and resources, windows and messages, GDI, Winsock,
DirectDraw and Direct3D, DirectInput, DirectMusic, Bink movies and the Miles Sound System.
SDL2 gives it windows, input and the clipboard.

Build the layer and run its tests:

```
python3 -I tools/build_port.py --target linux win32
python3 -I tools/test_port_linux.py
```

## Running the game

The native game needs an installed Empire Earth and The Art of Conquest, in the default Wine prefix. Build and link
the game, then make a run folder from a copy of the installed Art of Conquest:

```
python3 -I tools/build_port.py --target linux --link
python3 -I tools/port_linux_folder.py
cd build/port-linux/run && ./ee-aoc
```

The layer is `libeeport.so`. The engine and the plug-ins are ELF shared objects that keep their Windows file names,
because the engine loads its plug-ins from the `*.dll` files in its folder and skips the retail ones. The executable
is `ee-aoc`, and the retail `EE-AOC.exe` stays beside it, because the layer reads the game's icons, cursors and
start screen from it. SDL2 and FFmpeg are copied beside the images.

The run folder keeps its settings in `ee-port-registry.reg`. The first start imports them from the Wine prefix,
with the folder of the base game. The tool puts back the retail files that an ee-toolkit mod replaced;
`--keep-mod` keeps the mod.

## Fonts

GDI text uses TrueType faces that are installed on the computer. The layer looks for them in these folders, in this
order:

1. The folders in `EE_PORT_FONTS`, separated by colons.
2. `drive_c/windows/Fonts` in the Wine prefix (`WINEPREFIX`, else `~/.wine`).
3. `~/.local/share/fonts` (or `$XDG_DATA_HOME/fonts`) and `~/.fonts`.
4. `/usr/local/share/fonts`, `/usr/share/fonts` and `/usr/share/wine/fonts`.

The game asks for Arial, Courier New, MS Sans Serif, Tahoma, Times New Roman and Verdana. When the Windows face is
not installed, the layer uses a stand-in. Liberation Sans, Liberation Serif and Liberation Mono have the widths of
Arial, Times New Roman and Courier New. Wine's Tahoma replaces Tahoma and MS Sans Serif, and DejaVu Sans replaces
Verdana. A name that no installed face has falls back to Arial, as on Windows. The first use of each name writes a line to the log, for example
`font "Arial" is drawn with Liberation Sans`.

Liberation Sans and Liberation Serif do not have the tables that set Windows' line heights and hinted widths, so text
can be one pixel taller, and some characters one pixel wider or narrower, than on Windows. For text that measures as on
Windows, install the Windows faces into the Wine prefix:

```
winetricks corefonts tahoma
```

The test `check_gdi_metrics` compares the layer with a Windows 11 capture of 169 font requests and prints the result
for each face. It requires exact line metrics for Tahoma and exact widths for Courier New, because their stand-ins
measure as on Windows.

## Input

DirectInput gives the game its system keyboard and mouse from SDL's events. A key's code is its PC scan code, plus
0x80 for an extended key, as in DirectInput. While the game holds the mouse exclusively, SDL is in relative mouse
mode: the system cursor is hidden, the game draws its own, and the window gets no mouse messages. The game releases
the mouse when it stops being the active application, and in full screen when it shows the multiplayer lobby.

## Display

The DX7 display plug-in draws through `DirectDraw.cpp` and `Direct3D.cpp`, which implement DirectDraw 7 and Direct3D
7 on OpenGL 3.3 core. They implement the interfaces and the fixed-function subset that the plug-in uses: one vertex
format, texture stage 0, up to eight lights, vertex fog, colour keys, mipmaps and GDI text on surfaces.

Each surface keeps its pixels twice: in memory, in the exact layout of its pixel format, and in an OpenGL texture.
Lock, Blt and GetDC work on the memory, and Direct3D draws into the texture. Each copy is brought up to date only when
the other one changed. Direct3D draws are recorded and run at EndScene, or earlier when a surface that they use is
read. One lock serialises all threads, as DDSCL_MULTITHREADED does, so the cursor thread and the loading thread can
use surfaces while the graphics thread draws.

In full screen, the window covers the desktop and the game's mode is scaled to fit it with its aspect kept. The modes
on offer are 640x480, 800x600 and the common sizes up to the desktop's, in 16 and 32 bits. Set `EE_PORT_WINDOWED` to
keep the window at the mode's size.

The test `test_ddraw` draws on Mesa's software renderer through SDL's offscreen driver, so it needs no display. Each
expected pixel comes from Direct3D 7's rules for coverage, lighting, fog, blending and the alpha test, worked out in
the test.

## Sound

The Miles mixer plug-in plays through the layer's Miles Sound System, which opens one SDL audio device per Miles
driver and mixes the playing samples in the device's callback. Samples are the game's PCM WAV files and its MP3 music,
which FFmpeg decodes once and the layer keeps in memory. `SDL_AUDIODRIVER` chooses the SDL audio driver. When no audio
device opens, sound goes to SDL's dummy driver and the game runs without sound.

## Music

The game's music is DirectMusic: style-based segments in the base game's `Data/Music` folder, which the game composes
at run time by groove level. `DirectMusic.cpp` gives the game the DirectMusic COM classes that it creates with
CoCreateInstance: the loader, the performance and the composer. GothicKit's dmusic (`third_party/dmusic/`) reads the
files, composes the music and renders it with TinySoundFont. The performance's output, after the port volume, the
master volume and a reverb in place of the Waves reverb, mixes into the first Miles driver that the game opens.

Bands take some instruments from Windows' General MIDI collection, `gm.dls`, which no one may redistribute. The layer
answers `gm.dls`, `gs.dls` and `xg.dls` with a SoundFont: `EE_GM_BANK` when it is set, else `general-midi.sf2` beside
the executable. `tools/fetch_port_toolchain.py` downloads GeneralUser GS for it. Without a bank those instruments are
silent. `EE_DMUSIC_LOG=warn`, `info` or `debug` shows more of dmusic's log than its errors.

What differs from DirectMusic: the music time that the game reads runs at 120 beats a minute from Init, notifications
are never signalled, secondary segments and motifs do not play, and the synthesizer is not Microsoft's. Its level
against Windows' is known to a few dB only: the loudest passages of Honor reach full scale.
