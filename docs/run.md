# Running the native build

The native images run from a copy of the installed game. `tools/port_linux_folder.py` makes that copy; the installed
game is never touched.

```
python3 -I tools/port_linux_folder.py [--keep-mod] [--target linux|linux64] [--out DIR]
```

What the tool does:

1. Copies the Art of Conquest folder to `build/port-linux/run` (`cp -a --reflink=auto`, so a file system that
   supports reflinks copies nothing).
2. Puts back the retail files that an ee-toolkit mod replaced, unless `--keep-mod` is given. The mod's backups carry
   the sha256 of each file before the change; a file that does not match its record stops the tool.
3. Copies the native images and their libraries from `build/port-linux/bin/` over the retail files of the same names.
   The retail `EE-AOC.exe` stays, because the layer reads its resources.
4. Copies the General MIDI SoundFont in as `general-midi.sf2`.
5. Prints the command that starts the game. It does not start the game.

```
cd build/port-linux/run && ./ee-aoc
EE_PORT_WINDOWED=1 ./ee-aoc        # the game's own window size instead of the desktop
```

`--target linux64` builds the same folder from the 64-bit images, in `build/port-linux64/run` ([build](build.md)).

## The first start

The first registry call imports the game's settings from the Wine prefix: `HKEY_CURRENT_USER` from `user.reg`,
`HKEY_LOCAL_MACHINE` from `system.reg`, where a 32-bit program's keys sit under `Software\Wow6432Node`. The layer also
takes the base game's folder from there, which is how the game finds its data.

The settings then live in `ee-port-registry.reg` in the run folder, so they travel with it. Every change writes the
whole file again. A second copy of the game therefore does not share the settings with the first, as two copies of the
retail game in one registry would.

## Environment variables

| Variable | Effect |
| --- | --- |
| `EE_PORT_WINDOWED` | any value: the window keeps the game's mode size instead of covering the desktop. SDL is not put in full-screen mode. |
| `EE_PORT_REGISTRY` | the settings file to use instead of `ee-port-registry.reg` in the starting folder |
| `EE_PORT_EXE_RESOURCES` | the executable whose resources the layer reads instead of `EE-AOC.exe` in the starting folder |
| `EE_PORT_FONTS` | colon-separated folders to search for TrueType faces before the default ones |
| `EE_PORT_MOVIES` | a folder searched first for a movie's replacement file |
| `EE_GM_BANK` | the SoundFont for the General MIDI instruments instead of `general-midi.sf2` beside the executable |
| `EE_DMUSIC_LOG` | `warn`, `info` or `debug`: how much of dmusic's log to print |
| `EE_PORT_NO_MESSAGE_BOXES` | any value: a message box answers OK at once, and only its text goes to the log |
| `EE_PORT_INPUT_SCRIPT` | a file of timed key and mouse events to feed the game, for a run without a person at the keyboard ([testing](testing.md)) |
| `EE_PORT_FPE_LOG` | x86-64 build, any value: the log names each instruction that overflows single precision, once ([testing](testing.md)) |
| `SDL_VIDEODRIVER` | the SDL video driver. `offscreen` runs the game with no window. |
| `SDL_AUDIODRIVER` | the SDL audio driver. `dummy` runs the game without sound. |
| `WINEPREFIX` | the prefix the settings are imported from. Default `~/.wine`. |
| `XDG_DATA_HOME` | where the font search looks for `fonts/` before `~/.fonts` |

## What the game writes

| File | Who writes it |
| --- | --- |
| `ee-port-registry.reg` | the layer, on every settings change |
| `0_Error.log` | the game's own error system |
| `Users/…` | the game's profiles, saves and settings, as on Windows |
| `Data\…` | the game's own caches |

## The log

The layer writes one line per notable event to standard error, prefixed `ee-port:`. The lines worth knowing:

| Line | Meaning |
| --- | --- |
| `registry: N keys imported from …` | the first start imported the settings. Later starts say `N keys from …`. |
| `DirectDraw draws with OpenGL … on …` | the renderer and GPU in use. This is the line to read when the picture misbehaves. |
| `font "Arial" is drawn with Liberation Sans` | a face resolved to a stand-in ([GDI](gdi-and-fonts.md)) |
| `CreateMutexA("…"): named mutexes are not shared between processes here` | expected: the layer has no cross-process objects, so the game's single-instance check cannot stop a second copy |
| `… is not supported; the call does nothing` | the game called something outside the layer's scope; printed once per call site ([limits](limits.md)) |
| `input script: N events from …` | the script read at the first pump; `(some lines were skipped)` follows a line the parser refused, and each refusal is logged with its line number |

The game's own lines (`::EESetVideoMode: …`, `CMUCreateTrees: …`) come from the reconstruction's debug output.
