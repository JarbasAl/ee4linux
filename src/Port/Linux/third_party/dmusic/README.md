# GothicKit/dmusic, vendored

dmusic is a DirectMusic implementation in C11. It reads DirectMusic Producer files (segments `.sgt`, styles `.sty`,
bands and DLS collections `.dls`), composes the music from styles, chords and grooves, and renders it with a modified
TinySoundFont. `src/Port/Linux/DirectMusic.cpp` puts the DirectMusic COM interfaces that the game calls in front of
it.

| | |
| --- | --- |
| upstream | https://github.com/GothicKit/dmusic |
| commit | `b73b270b94714b8ec8d9d0b2a9f3a5c445da52d1` (`main`) |
| taken from | ee-macos (https://github.com/maxgraf96/ee-macos) `kit-src/third_party/dmusic/` at commit `136d78fdf6608c869f0621942c7dfeba144a74e4`, which carries the patches below |
| licence | MIT (Modern Variant), see `LICENSE.md`. `vendor/TinySoundFont` is GothicKit's modified copy of TinySoundFont (MIT, Bernhard Schelling) under the same terms. |

## What is here

Every file is ee-macos's, byte for byte, except this README:

- `LICENSE.md`, `include/dmusic.h`, every file under `src/`, and `vendor/TinySoundFont/tsf.h` and `tsf.c`: the
  upstream files with the patches below;
- `UPSTREAM-README`: upstream's `README.md`, renamed. Its links point into parts of the upstream tree that are not
  copied here;
- `recomp/dm_prefix.h`: an ee-macos file. It renames each external symbol that the library defines and does not
  export as `Dm*`, chiefly TinySoundFont's `tsf_*`, so that the library can share an executable with another
  TinySoundFont.

## How the build compiles it

`tools/build_port.py` builds the library as the `dmusic` module of the `linux` target (`LIBRARY_MODULES`). It compiles
every `.c` file under this folder as C11 for i386, with `DM_STATIC=1`, `_POSIX_C_SOURCE=200809L`, warnings off, and
`recomp/dm_prefix.h` included first. The objects go to `build/port-linux/obj/dmusic/` and link into the game with the
layer's. `tools/test_port_linux.py` links them into the layer's tests.

## Patches on upstream

ee-macos made these changes, each as its own commit:

| Patch | Why |
| --- | --- |
| Select an instrument by bank MSB, LSB and drum flag | A band patch is `drum<<31 \| MSB<<16 \| LSB<<8 \| program`. Upstream passed the LSB as the bank and ignored the drum flag, so most GM and GS instruments and every drum kit played the collection's first preset. A missing GS variation plays its capital tone (MSB 0), as GS synthesizers do. |
| Master groove level (`DmPerformance_setMasterGroove`) | DirectMusic's `GUID_PerfMasterGrooveLevel`, added to every command's groove level. A game sets the intensity of style-based music with it. |
| Segment repeats setter (`DmSegment_setRepeats`) | `IDirectMusicSegment::SetRepeats`: a game sets how often a segment loops, whatever its file says. |
| Read the playing state under the lock | `playSegment` and `playTransition` read the playing style, band and chord without the performance lock, which raced the render thread. A transition from a segment without a style dereferenced a null style. |
| A segment ends after its last repeat; `DmPerformance_isPlaying` | A DirectMusic segment state ends after its length times (repeats + 1). Upstream let the style choose patterns for ever. `isPlaying` tells the COM layer when the segment has ended. |
| A missing DLS collection leaves its instruments silent | A band whose collection is missing failed the whole download, and the first failure stopped the other bands and the style. Now each missing collection is a warning, and every band and style is still downloaded. |
| Accept a 16-byte `fmt ` chunk | A wave's format may be a `PCMWAVEFORMAT`, without `cbSize`. |
| A SoundFont 2 bank in place of a DLS collection | A resolver may answer `gm.dls` with an SF2 bank. TinySoundFont loads it, and the synthesizer plays its presets. |
| Unnamed DLS instruments and waves | A DLS collection need not name its instruments or waves. Building the presets copied the missing name from a null pointer. |
| Keep the clock sample-exact | Each render call rounded its duration to whole ticks, so the tempo depended on how much a caller rendered at once. The fraction is carried now. |

## The General MIDI bank

Windows supplies `gm.dls`, which no one may redistribute. Bands of six of the base game's eight songs take
instruments from it, and Pawns also from `gs.dls`. The layer answers `gm.dls`, `gs.dls` and `xg.dls` with a SoundFont:
`EE_GM_BANK` when it is set, else `general-midi.sf2` beside the executable. `tools/fetch_port_toolchain.py` downloads
GeneralUser GS (S. Christian Collins, a free licence, see `NOTICE.md`) to `toolchain/soundfonts/`. Without a bank,
those instruments are silent and the rest of the music plays.
