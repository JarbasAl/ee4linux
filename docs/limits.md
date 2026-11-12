# What the port does not do

The layer implements what the game calls. Everything else is missing on purpose: a function the game never calls costs
nothing to leave out, and a function that cannot mean what it means on Windows is better absent than wrong.

## Display

| Missing | Why |
| --- | --- |
| Wayland | the 32-bit SDL2 is built with X11 only, because the 32-bit Wayland libraries are absent. SDL's X11 driver runs under XWayland on a Wayland desktop. |
| Exclusive full-screen mode changes | the desktop's mode is never changed. The window covers the desktop and the game's mode is scaled to fit it, so a mode change cannot leave the desktop in a strange state. |
| Palettes (`CreatePalette`, `SetPalette`) | all modes are 16 or 32 bits; the game creates no palette. |
| Clippers (`CreateClipper`, `SetClipper`) | the game uses none. A blit that leaves a surface is clipped to the window on a windowed primary, and refused otherwise. |
| Overlays, `BltBatch`, `EnumAttachedSurfaces` | not used by the plug-in |
| Direct3D: `ProcessVertices`, retained mode, texture management, spot lights | not used by the plug-in. Directional and point lights are implemented. |
| Hardware cursors | the game draws its own pointer, and SDL's cursor is used only for the window manager's pointer. |
| Colour keys other than `DDCKEY_SRCBLT` | not used. |
| Copying a surface to itself through the GPU | done in memory instead. |

## Windows and messages

| Missing | Why |
| --- | --- |
| `TerminateThread`, `SuspendThread` | not used by the game; a suspended POSIX thread cannot be suspended safely |
| named kernel objects shared between processes | the layer has no cross-process objects, so the game's single-instance check cannot see a second copy. Two copies can run at once. |
| window enumeration, hooks, DDE, OLE drag and drop | not used |
| the input method (IME) | the game's IME code runs only on IME messages, which the layer never sends |
| `WM_SETTINGCHANGE` and the desktop's own messages | the game does not read them |

## Input

| Missing | Why |
| --- | --- |
| joysticks and gamepads | the game's system devices are keyboard and mouse only |
| force feedback | not used |
| DirectInput's action mapping | not used |

## Sound

| Missing | Why |
| --- | --- |
| 3D audio providers other than the plug-in's calls | the plug-in's 3D calls are answered, but the mix is stereo. |
| EAX and the Waves reverb | replaced by dmusic's reverb ([music](music-directmusic.md)) |
| MCI CD audio, waveOut, the mixer and aux devices | the game asks about them; the layer reports none present so that the game takes the Miles path |

## Music

| Missing | Why |
| --- | --- |
| DirectMusic notifications | the handle the game gets is never signalled |
| secondary segments and motifs | not composed by dmusic |
| Microsoft's synthesizer | dmusic's synthesizer and a SoundFont instead, so the level is known to a few dB only |

## Network

| Missing | Why |
| --- | --- |
| GameSpy query reporting | `qr_init` fails as a closed socket does; the services no longer exist |
| WON's online services | the library is rebuilt, the servers are gone. LAN and Direct Connect are the testable paths. |
| IPv6 addresses in `GetAdaptersInfo` | only the IPv4 adapters are listed, which is what the game reads |

## The build

| Missing | Why |
| --- | --- |
| a 64-bit native target | the reconstruction is a 32-bit game: the source carries the retail's pointer-sized structures, offsets, inline assembly and x87 arithmetic. |
| a Windows build of the layer | the Windows images use the real Windows and the retail DirectX |
| the original's image checksums | only the Windows images carry the compensation block that matches the retail files' checksums |

## Things that look missing but are not

- **The retail DLLs in the run folder.** The engine scans the folder for plug-ins; the native images answer for the
  retail ones, which stay because the run folder is a copy of the installation
  ([run](run.md)).
- **`EE-AOC.exe` in the run folder.** The native executable reads its resources: the start screen's bitmap, the
  cursors and the icons ([build](build.md)).
- **A `.dll` suffix on the engine and the plug-ins.** The images are ELF shared objects with the Windows names,
  because that is what the engine's scan looks for.
