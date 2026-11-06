# Empire Earth: The Art of Conquest - source reconstruction

[![build](https://github.com/Protryon/empire_earth/actions/workflows/ci.yml/badge.svg)](https://github.com/Protryon/empire_earth/actions/workflows/ci.yml)

A matching decompilation of *Empire Earth: The Art of Conquest* (Stainless Steel Studios / Sierra, 2002): C++ source
that compiles, with the original toolchain, to the same machine code as the retail release. It covers the game
executable and its first-party DLLs. The source is not complete yet (see Status); every image links from it.

Toolchain of the original build:

- Microsoft Visual C++ 6.0 RTM (C/C++ compiler 12.00.8168), LINK 6.00
- Dinkum C++ Library 2.33 (the game's STL, shipped as `sxlrt233.dll`); its headers are reconstructed in
  `include/dinkum/`
- DirectX 7.0a SDK headers (renderer plug-ins), DirectX 8.1 SDK and Platform SDK (June 2001) libraries
- third-party code linked into the images: IJG libjpeg 6b, libpng 1.0.12, zlib 1.1.3 (`src/ThirdParty/`)

The engine's PKWARE Data Compression Library code (`implode`/`explode`, `src/ThirdParty/pkdcl/`) is not a
reconstruction of the original library: it is a compatible reimplementation of the DCL format written for this
project. It reads and writes the game's compressed data like the original (the game's data files decode identically,
and data compressed by either one decompresses with the other), but its machine code differs from the retail engine's.

This repository contains no game data and no original binaries. You need a legal copy of the game to play anything
you build.

## Modules

| Module | Image | Source |
|---|---|---|
| `exe` | `EE-AOC.exe` (game) | `src/Empire Earth/`, `src/RTS Engine/` |
| `lle` | `Low-Level Engine.dll` (engine core) | `src/Low-Level Engine/` |
| `jpeg` | libjpeg 6b inside `Low-Level Engine.dll` | `src/ThirdParty/libjpeg/` |
| `exeimg` | libpng / zlib / libjpeg inside `EE-AOC.exe` | `src/ThirdParty/` |
| `default` | `Default.dll` (random map generators) | `src/Empire Earth/Random Map/Random Map Generators/Default/` |
| `dx7hr` | `DX7HRDisplay.dll` (renderer) | `src/Low-Level Engine/Graphics Engine/DX7 Display Plug-ins/` |
| `dx7hrtnl` | `DX7HRTnLDisplay.dll` (T&L renderer) | same folder |
| `tcp` | `TCP Protocol.dll` (network) | `src/Low-Level Engine/Network Engine/TCP Protocol/` |
| `mixer` | `Miles Sound System Mixer.dll` (sound) | `src/Low-Level Engine/Sound Engine/Miles Sound System Mixer/` |
| `language` | `Language.dll` (resources only) | `src/Empire Earth/Language/Language.rc` |

## Status

<!-- status:begin -->
| Module | Functions byte-identical | Code bytes byte-identical |
|---|---:|---:|
| EE-AOC.exe (game code) | 15,234 / 16,884 (90.2%) | 1,678,616 / 2,736,250 (61.3%) |
| EE-AOC.exe (WON library) | 5,698 / 8,380 (68.0%) | 347,590 / 1,386,594 (25.1%) |
| TCP Protocol.dll | 124 / 125 (99.2%) | 13,417 / 13,649 (98.3%) |
| Miles Sound System Mixer.dll | 106 / 106 (100.0%) | 5,021 / 5,021 (100.0%) |
| Default.dll | 795 / 845 (94.1%) | 52,833 / 97,011 (54.5%) |
| Low-Level Engine.dll | 5,880 / 6,030 (97.5%) | 443,832 / 538,055 (82.5%) |
| libjpeg in the engine | 295 / 295 (100.0%) | 64,136 / 64,136 (100.0%) |
| DX7HRDisplay.dll | 553 / 584 (94.7%) | 28,576 / 60,561 (47.2%) |
| DX7HRTnLDisplay.dll | 594 / 626 (94.9%) | 29,549 / 62,738 (47.1%) |
| libpng/zlib/libjpeg in the exe | 318 / 318 (100.0%) | 98,476 / 98,476 (100.0%) |
| Language.dll | resources only | identical to the original apart from the link time stamps and the debug record (PDB path) |

Measured on 2026-11-06 at source revision c70d53ef010a: a function counts when its compiled code is identical to the original's, in the context of its own unit, with every reference resolving to the same symbol. Counts exclude CRT, SDK and import-thunk code.
<!-- status:end -->

The images link from this source alone. Where a function is not byte-identical yet, the image carries this source's
version of it, so the images are not byte-identical to the retail files. `EE-AOC.exe` includes the WON library from
source. The WON online services themselves were not tested: the servers no longer exist.

Testing (automated, on the full-source build of the game and engine):

- The executable linked entirely from this source plays every tested game identically to the original, compared
  tick by tick (simulation, AI and render state): short games with 2 to 6 AI players on several map sizes and types
  and both difficulty levels, long Deathmatch games to a decisive end, a scripted human player, and save/load in
  every direction (saves are compatible with the original's).
- Multiplayer (TCP/IP, host and client mixing original and rebuilt engines): simulation and AI identical to the end
  of the game, no out-of-sync.
- The original executable running on the rebuilt `Low-Level Engine.dll` plays identically to the retail engine.
- The settings screens and the menu tour pass on both the original and the rebuilt images.
- The multiplayer lobby opens and its LAN and Direct Connect screens behave as on the original. Direct Connect games
  start in every direction between original and rebuilt images. Peer sync during play was not shown there.
- Not covered yet: audio output, physical input devices.

## Building

Requirements: Windows (the 32-bit VC6 tools run natively on 64-bit Windows 10/11), Python 3.12 (standard library
only), 7-Zip (`7z` on PATH, `C:\Program Files\7-Zip\7z.exe`, MSYS2's p7zip, or env `EE_7Z`), about 3 GB of free disk
space while the libraries are extracted (about 1.7 GB afterwards, 1.1 GB of it the downloads cached in
`toolchain/dl/`), network access to github.com and archive.org.

1. Fetch the toolchain. Everything goes to `toolchain/` (not tracked); every download and every extracted file is
   sha256-pinned and checked.

   ```
   python -I tools/fetch_toolchain.py 6.0 6.3 6.4 6.5pp 7.0
   python -I tools/fetch_libs.py --libs
   ```

   - `fetch_toolchain.py`: the decomp.me compiler packages (github.com; about 60 MB): VC6 RTM (the compiler, LINK,
     LIB), SP3 (LIB for one import library), SP4 (libjpeg in the engine, Dinkum's `operator new[]`/`delete[]`),
     SP5 + Processor Pack (Miles' `MSS_auto_cleanup`) and Visual C++ .NET 2002 (only its RC, for the resource
     scripts).
   - `fetch_libs.py`: the DirectX 7.0a SDK headers, and with `--libs` the link libraries: the VC6 RTM import
     libraries and `msvcrt.lib` (from the Visual C++ 6.0 Professional CD), DirectX 8.1 SDK `ddraw.lib`,
     `dinput.lib`, `dxguid.lib` and the Platform SDK (June 2001) `IPHlpApi.Lib` (archive.org; about 1.1 GB of
     downloads, cached in `toolchain/dl/`; `--dl DIR` reads downloads you already have).

   `python -I tools/fetch_libs.py` without `--libs` fetches only the headers, enough for `--compile-only`.

2. Build:

   ```
   python -I build.py                  # every image
   python -I build.py -j 4 lle         # one image, 4 parallel compiles
   python -I build.py --compile-only   # compile every unit, link nothing
   ```

   Outputs (each with its `.map` and `.pdb`):

   | Target | Output |
   |---|---|
   | `tcp` | `build/tcp/TCP Protocol.dll` |
   | `mixer` | `build/mixer/Miles Sound System Mixer.dll` |
   | `default` | `build/default/Default.dll` |
   | `lle` | `build/lle/Low-Level Engine.dll` (with libjpeg and the DCL) |
   | `dx7hr` | `build/dx7hr/DX7HRDisplay.dll` |
   | `dx7hrtnl` | `build/dx7hrtnl/DX7HRTnLDisplay.dll` |
   | `language` | `build/language/Language.dll` |
   | `exe` | `build/exe/EE-AOC.exe` (with the image libraries) |

   Objects go to `build/obj/<module>/`; `--out DIR` puts everything under `DIR` instead of `build/`. A full build
   compiles 845 units and links 8 images (about 3 minutes with `-j 2` on a current desktop).

   The engine and TCP DLLs carry a compensation block (appended after the image, never loaded) so their checksum
   matches the retail files, which the game's multiplayer version check compares (`--no-retail-checksum` leaves them
   as linked).

   Paths: sources are staged under `C:\EEX\` (the original build tree, so that `__FILE__` strings match) before each
   compile; set `EE_ORIG_SRC_ROOT` to `X:\EEX` to use another drive. VC6's code generation depends on the lengths of
   some paths it reads, so the build normalises them: unless the checkout path is 23 characters long it copies
   `src/`, `include/` and the compiler packages it uses to `C:\EEX-root____________` (same drive as the staging
   root, padded to that length) and compiles from there, and it runs every tool with `TMP`/`TEMP` set to
   `C:\EEX-tmp_____________________` (31 characters).

   Two builds of the same source give the same images apart from link time stamps, PDB paths and signatures, and
   one more field: VC6 names an anonymous namespace after the source file and the time of the compile (in seconds),
   so the RTTI type name of the one class in an anonymous namespace (`EELobbyConnectDialog.cpp`, in `EE-AOC.exe`)
   carries a number that changes with every compile.

CI (GitHub Actions, `windows-latest`) fetches the pinned toolchain and builds every image.

## Branches

- **`main`**: the reconstruction. Its code is byte-identical to the retail release wherever it is matched, and the
  game it builds behaves like the original in every tested game. It keeps the original's bugs on purpose, so it plays
  multiplayer with the original game and shares saves with it.
- **`fixes`**: `main` plus the fixes that keep compatibility with the original: crashes, resource leaks, memory-safety
  guards, local display, interface and audio fixes, and reads of uninitialised data made to read zero. Simulation, AI,
  network traffic and save files stay exactly the original's, so a `fixes` build plays multiplayer with original
  players and loads their saves, and they load its saves. `FIXES.md` on the branch describes every fix.
- **`fixes-all`**: `fixes` plus every other bug fix, including those that change the simulation or the AI. It
  behaves as the game was meant to rather than as it shipped. Play multiplayer only with other `fixes-all` players
  (the simulation differs from the original's, so mixed games go out of sync); saves load where the save format
  allows. Its `FIXES.md` adds every fix beyond `fixes` and the behaviour changes a player will notice.

Every branch builds the same way (`python -I build.py`, see Building). To switch, check the branch out and rebuild:

```
git switch <branch>
python -I build.py
```

The fix branches are patch sets on top of `main`: whenever `main` is updated, their commits are replayed onto the new
`main` (the branches are rewritten, so update a local copy with `git fetch` and `git reset --hard origin/<branch>`
rather than a merge). Each fix is one commit that names the bug it fixes (`git log --oneline main..fixes`).
`FIXES.md` on each fix branch, added by the branch's first commit, describes every fix: the function and its address
in the original game, what the original does wrong and what the fix does, grouped by area.

## License

No license is granted. Empire Earth is the property of its rights holders. The third-party libraries under
`src/ThirdParty/` keep their own licenses.
