# Sound: the Miles Sound System

`src/Port/Linux/Miles.cpp`

The game's sound plug-in is the Miles Sound System Mixer, and the layer implements the Miles 6 API that it calls, on
SDL audio. The rules marked *Miles* below are Miles 6.1's own behaviour, measured from the library by ee-macos
(`kit-src/dx/mss32.cpp`, MIT).

## Drivers

A Miles driver is one SDL audio device. `AIL_startup` installs Miles' preference defaults, and the game opens the
driver that the retail mixer asks for. Each driver has its own callback, which mixes the driver's playing samples into
the device's buffer.

`SDL_AUDIODRIVER` chooses the SDL audio driver. When no device opens, the driver falls back to SDL's dummy driver and
the game runs without sound, which is what the test harness uses.

## Samples

A sample plays a RIFF PCM WAV image from the game's memory: 8, 16 or 32 bits, mono or stereo, at the game's rate. The
game's MP3 music is decoded once through FFmpeg and kept in memory.

Miles' behaviour that the layer keeps:

| Rule | Detail |
| --- | --- |
| start | a start always begins at the top of the sample |
| loop count | counts down, and a looping sample ends when it reaches zero |
| balance | the pan law is Miles': a hard pan keeps full volume in one speaker |
| position | a compressed sample's position is counted in its compressed bytes |
| length | `AIL_sample_ms` is the decoded length in milliseconds |
| volume | the sample volume, the driver volume, and the game's master volume multiply |

Implemented calls: `AIL_startup`, `AIL_shutdown`, `AIL_open_digital_driver`, `AIL_close_digital_driver`,
`AIL_set_sample_file`, `AIL_init_sample`, `AIL_start_sample`, `AIL_stop_sample`, `AIL_resume_sample`,
`AIL_end_sample`, `AIL_release_sample_handle`, `AIL_set_sample_volume`, `AIL_set_sample_pan`,
`AIL_set_sample_loop_count`, `AIL_sample_status`, `AIL_sample_ms`, `AIL_set_sample_ms`, `AIL_sample_position`,
`AIL_set_sample_position`, `AIL_set_digital_master_volume`, `AIL_digital_master_volume`, `AIL_quick_*` and the
3D-positioning calls the plug-in uses, plus `AIL_set_file_callbacks` and the memory-file open (`AIL_open_named_file`).

Miles' event system is implemented with the layer's kernel events, so the plug-in's `AIL_register_EOS_callback` and
`AIL_register_timer` work as they do on Windows.

## Mixing for the tests

Two calls exist for the tests, not for the game:

- `MilesWithoutDevice()` makes the drivers opened after it use no device and mix only through `MixMiles`;
- `MixMiles(driver, out, frames)` mixes `frames` stereo frames of a driver's playing samples at the driver's rate.

`check_miles` uses them to compare the decoded PCM with FFmpeg's, sample for sample.

## The music in the mix

DirectMusic's synthesizer mixes into the first Miles driver that the game opens: the layer puts the music behind the
Miles mixer's lock, after the driver volume and the master volume ([music](music-directmusic.md)).
