# Troubleshooting

The failures seen so far, in the order they stopped the game, and what each one means. Every one of them was fixed on
the 32-bit branch; the entries stay here because the same symptom can come back from another cause.

## The game stops at once with SIGTRAP, after the registry line

The game's own check failed in start-up. Read the terminal output: the layer logs nothing here, but the last line
before the trap names the step.

Seen: `DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), …)` in `TSInitialize` returned `FALSE`, because the
main thread had no thread object. The trap is the game's `ESVerify` at `Empire Earth.cpp` line 805. The fix gives
every thread a thread object on first use ([threads](threads-and-sync.md)).

## Music plays, the screen stays empty

The game is running and drawing into a surface that never reaches the screen. Check the log:

- `DirectDraw draws with OpenGL …` missing: the DirectDraw layer has no OpenGL window, so `Present` returns at once.
  Seen: the start screen's thread became the layer's only "video thread", and the game's window, on the main thread,
  was refused a native window. Each window's own thread now makes its window ([windows](windows-and-messages.md)).
- the line present, but the window is behind other windows: the game runs windowed (`EE_PORT_WINDOWED`) or the window
  manager did not raise it. The game's window covers the desktop in full screen.

## SIGSEGV a second after the logo

Seen: the start screen's thread crashed inside SDL's X11 event dispatch because another thread entered SDL without the
lock while it polled. Every call into SDL from the window and GDI units holds `gSdlLock` now
([threads](threads-and-sync.md)).

For any crash, read the core dump; the images keep their symbols:

```
coredumpctl list | tail -3
coredumpctl info <pid> | head -30              # the stack of the faulting thread
coredumpctl dump <pid> --output=/home/miro/tmp/ee-core
gdb -q -batch -ex "bt 12" -ex "info registers eax ebx ecx edx esi edi eip" \
    build/port-linux/run/ee-aoc /home/miro/tmp/ee-core
```

`coredumpctl dump --output` writes the core uncompressed, so `zstd -d` on it fails.

## SIGSEGV when a map or a scenario starts

Seen: `RTSUnit::Link` read the float `1.0` as a pointer. The first unit links into an empty list, where the previous
element is the `RTSDummyUnit` head held in an `RTSUnit*`, so `dynamic_cast<RTSDummyUnit*>` decides the branch. The
Itanium cast returned `NULL` where MSVC's finds the head ([C++ runtime](cxx-runtime.md)).

A core dump shows this as `mov (%eax),%eax` with `eax = 0x3f800000` in `RTSUnit::Link`, called from
`EEWorld::PlaceQueuedUnits`.

## The pointer has a coloured box round it

The game's pointer is a 32x32 TGA whose background is the transparency colour (pure blue) in a format with no alpha
channel. The key must be compared on the bits the format names, not on the whole 32-bit pixel
([DirectDraw](display-directdraw.md)).

A related symptom with the same cause: a sprite or a font texture shows its background where it should show the scene.

## Text is a pixel too tall, or a word a pixel wide

The face resolved to a stand-in that has no Windows metric tables. The log names what was used. Install the Microsoft
faces into the Wine prefix to measure as on Windows:

```
winetricks corefonts tahoma
```

## No sound, or the game runs without sound

A driver that cannot open a device falls back to SDL's dummy driver and the game plays on silently. Check
`SDL_AUDIODRIVER` and that a 32-bit ALSA or PulseAudio client library is installed. `SDL_AUDIODRIVER=dummy` is a
deliberate silent run, which the tests use.

## The music is silent between passages, or some instruments are missing

The bands take instruments from Windows' General MIDI collection, which the layer answers with a SoundFont: `EE_GM_BANK`
or `general-midi.sf2` beside the executable. Without it only the instruments the game carries itself sound
([music](music-directmusic.md)).

## A movie does not play, or plays at the wrong size

A `.bik` needs a 32-bit FFmpeg. A replacement movie may replace it with `.webm`, `.mkv`, `.mp4`, `.mov`, `.avi` or
`.mpg`, at any size ([movies](movies-bink.md)). `EE_PORT_MOVIES` names the folder searched first.

## The picture is wrong, or slow, on the NVIDIA card

Every pixel check so far ran on Mesa. The layer logs the renderer it got; read that line first. On a hybrid laptop the
default is the integrated GPU, so the log names Mesa's Intel driver unless the process is started with the card
selected:

```
__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia ./ee-aoc
```

## Slow menus, or a slow in-game panel

Text drawn over a 3D frame reads the area under the text back from the GPU, so that the text blends with what is under
it ([GDI](gdi-and-fonts.md)). A large read-back on a slow GPU is the cost. Nothing else in the layer reads the
framebuffer.

## `EE_PORT_WINDOWED` does nothing

It is read when a display mode is set. Without a mode change, the window keeps the mode's size anyway. The variable
also keeps SDL out of desktop full screen, so the window stays where the window manager puts it.
