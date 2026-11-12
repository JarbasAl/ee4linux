# Music: DirectMusic

`src/Port/Linux/DirectMusic.cpp`, `src/Port/Linux/third_party/dmusic/`

The game's music is DirectMusic: the conductor creates a loader, a composer and a performance with
`CoCreateInstance`, takes the software synthesizer's port from the performance, loads style-based segments, plays
them, changes the groove level for intensity, and composes transitions between grooves.

Reading the segment, style, band and DLS files, choosing patterns by groove level, the transitions and the synthesizer
are GothicKit's dmusic, carried as source. The layer is the COM surface over it, the file lookup through the layer's
paths, and the output stage that DirectMusic puts behind its port.

## The port's output stage

Behind the port, the layer applies:

- the port's volume and the performance's master volume;
- a reverb in place of the Waves reverb (dmusic's own);
- the mix into the first Miles driver the game opens, under the mixer's lock.

## Instruments

Some bands take instruments from Windows' General MIDI collection, `gm.dls`, which no one may redistribute. The layer
answers `gm.dls`, `gs.dls` and `xg.dls` with a SoundFont:

- `EE_GM_BANK` when it is set;
- else `general-midi.sf2` beside the executable.

`tools/fetch_port_toolchain.py` downloads GeneralUser GS 2.0.3 for it, and `tools/port_linux_folder.py` copies it into
the run folder. Without a bank, those instruments are silent and the rest of the music plays.

## Logging

`EE_DMUSIC_LOG=warn`, `info` or `debug` shows more of dmusic's log than its errors. `check_dmusic` reads its lines to
see which collections each of the game's eight songs asks for.

## What differs from DirectMusic

- The music time that the game reads runs at 120 beats a minute from `Init`.
- Notifications are never signalled: the plug-in registers for them and the layer's calls succeed and do nothing.
- Secondary segments and motifs do not play.
- The synthesizer is not Microsoft's, so the level against Windows' is known to a few dB only. The loudest passages of
  *Honor* reach full scale.
- The waves reverb parameters are accepted and mapped onto dmusic's reverb.

Everything else follows the game's calls: `IDirectMusicLoader`, `IDirectMusicPerformance`,
`IDirectMusicComposer`, `IDirectMusicSegment`, `IDirectMusicSegmentState`, `IDirectMusicStyle`,
`IDirectMusicPort`, `IDirectMusicBuffer`, `IDirectMusicCollection`, and the download and tool calls the plug-in makes.
