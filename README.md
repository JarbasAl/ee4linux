# Empire Earth: The Art of Conquest - source reconstruction

A matching decompilation of *Empire Earth: The Art of Conquest* (Stainless Steel Studios / Sierra, 2002): C++ source
that compiles, with the original toolchain, to the same machine code as the retail release. It covers the game
executable and its first-party DLLs.

Toolchain of the original build:

- Microsoft Visual C++ 6.0 RTM (C/C++ compiler 12.00.8168), LINK 6.00
- Dinkum C++ Library 2.33 (the game's STL, shipped as `sxlrt233.dll`); its headers are reconstructed in
  `include/dinkum/`
- DirectX 7.0a SDK headers (renderer plug-ins), DirectX 8.1 SDK and Platform SDK (June 2001) libraries
- third-party code linked into the images: IJG libjpeg 6b, libpng 1.0.12, zlib 1.1.3 (`src/ThirdParty/`)

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
| - | `Language.dll` (resources only) | `src/Empire Earth/Language/Language.rc` |

## Status

<!-- status:begin -->
| Module | Functions byte-identical | Code bytes byte-identical |
|---|---:|---:|
| EE-AOC.exe (game code) | 15,214 / 16,883 (90.1%) | 1,677,842 / 2,736,250 (61.3%) |
| TCP Protocol.dll | 124 / 125 (99.2%) | 13,417 / 13,649 (98.3%) |
| Miles Sound System Mixer.dll | 106 / 106 (100.0%) | 5,021 / 5,021 (100.0%) |
| Default.dll | 795 / 845 (94.1%) | 52,833 / 97,011 (54.5%) |
| Low-Level Engine.dll | 5,876 / 6,030 (97.4%) | 442,615 / 538,055 (82.3%) |
| libjpeg in the engine | 295 / 295 (100.0%) | 64,136 / 64,136 (100.0%) |
| DX7HRDisplay.dll | 553 / 584 (94.7%) | 28,576 / 60,561 (47.2%) |
| DX7HRTnLDisplay.dll | 594 / 626 (94.9%) | 29,549 / 62,738 (47.1%) |
| libpng/zlib/libjpeg in the exe | 318 / 318 (100.0%) | 98,476 / 98,476 (100.0%) |
| Language.dll | resources only | rebuilt byte-identical (apart from the link timestamp) |

Measured on 2026-11-03 at source revision d94ed06014f4: a function counts when its compiled code is identical to the original's, in the context of its own unit, with every reference resolving to the same symbol. Counts exclude CRT, SDK and import-thunk code; the exe row is the game code range (the WON networking library linked into the exe is not counted).
<!-- status:end -->

Testing (automated, on the full-source build of the game and engine):

- The executable linked entirely from this source plays every tested game identically to the original, compared
  tick by tick (simulation, AI and render state): short games with 2 to 6 AI players on several map sizes and types
  and both difficulty levels, long Deathmatch games to a decisive end, a scripted human player, and save/load in
  every direction (saves are compatible with the original's).
- Multiplayer (TCP/IP, host and client mixing original and rebuilt engines): simulation and AI identical to the end
  of the game, no out-of-sync.
- The original executable running on the rebuilt `Low-Level Engine.dll` plays identically to the retail engine.
- The settings screens and the menu tour pass on both the original and the rebuilt images.
- Not covered yet: audio output, physical input devices.

## Building

Requirements: Windows (the 32-bit VC6 tools run natively on 64-bit Windows 10/11), Python 3.12, 7-Zip, about 1 GB of
free disk space, network access to github.com and archive.org for the toolchain downloads.

1. Fetch the compilers (VC6 RTM and SP4 from the decomp.me compiler packages; sha256-pinned) and the DirectX 7.0a
   SDK headers (archive.org; sha256-pinned):

   ```
   python -I tools/fetch_toolchain.py 6.0 6.4
   python -I tools/fetch_libs.py
   ```

   Everything goes to `toolchain/` (not tracked). `python -I tools/fetch_libs.py --libs` also fetches the link
   libraries (VC6 RTM `VC98\LIB`, DirectX 8.1 SDK, Platform SDK June 2001; several hundred MB of downloads).

2. Compile:

   ```
   python -I build.py --compile-only            # every module
   python -I build.py --compile-only -j 4 lle   # one module, 4 parallel compiles
   ```

   Sources are staged under `C:\EEX\` (the original build tree, so that `__FILE__` strings match) before each
   compile; set `EE_ORIG_SRC_ROOT` to `X:\EEX` to use another drive. VC6's code generation for a few functions
   depends on the lengths of the paths it reads (its own directory, the include directories), so unless the checkout
   path is 23 characters long the build first copies `src/`, `include/` and the compiler packages it uses to
   `C:\EEX-root____________` (same drive as the staging root, padded to that length) and compiles from there.
   Objects go to `build/obj/<module>/`.

Linking the DLLs and the executable from this repository is not available yet (pending).

CI (GitHub Actions, `windows-latest`) fetches the pinned toolchain and compiles every unit of every module.

## License

No license is granted. Empire Earth is the property of its rights holders. The third-party libraries under
`src/ThirdParty/` keep their own licenses.
