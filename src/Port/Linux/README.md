# The Win32 layer of the native Linux build

The native build compiles the game's own source against Wine's headers and links it to this layer in place of
Windows. The layer implements the part of the Win32 API that the game, its plug-ins and WON call. It has files and
paths, the registry, threads and synchronization, modules and resources, windows and messages, GDI, Winsock,
DirectInput, DirectMusic, Bink movies and the Miles Sound System.
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
