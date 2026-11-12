# The Win32 layer of the native Linux build

The native build compiles the game's own source against Wine's headers and links it to this layer in place of
Windows. The layer implements the part of the Win32 API that the game, its plug-ins and WON call. It has files and
paths, the registry, threads and synchronization, modules and resources, windows and messages, GDI, Winsock,
DirectInput, Bink movies and the Miles Sound System.
SDL2 gives it windows, input and the clipboard.

Build the layer and run its tests:

```
python3 -I tools/build_port.py --target linux win32
python3 -I tools/test_port_linux.py
```

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

## Sound

The Miles mixer plug-in plays through the layer's Miles Sound System, which opens one SDL audio device per Miles
driver and mixes the playing samples in the device's callback. Samples are the game's PCM WAV files and its MP3 music,
which FFmpeg decodes once and the layer keeps in memory. `SDL_AUDIODRIVER` chooses the SDL audio driver. When no audio
device opens, sound goes to SDL's dummy driver and the game runs without sound.
