# Movies: Bink

`src/Port/Linux/Bink.cpp`

The plug-in plays its movies through the Bink API (`binkw32.dll`). The layer implements that API on FFmpeg, so the
game's Bink 1 files play, and a replacement movie in a modern format plays in their place.

## Replacement movies

A movie requested as `Movies\Name.bik` is looked for in this order:

1. `Name` with each of `.webm`, `.mkv`, `.mp4`, `.mov`, `.avi`, `.mpg`, in the folder named by `EE_PORT_MOVIES`;
2. the same extensions in the movie's own folder;
3. the `.bik` file itself.

A replacement may have any size: `BinkOpen` reports the file's own size, the plug-in makes its frame buffer that size,
and `BinkDoFrame`/`BinkCopyToBuffer` hand the frame over at that size. The plug-in's `RenderFrame` stretches the frame
onto the back buffer, so an upscaled intro plays full screen.

The 32-bit FFmpeg that the port builds reads MP4, Matroska/WebM, AVI and MPEG, with H.264, HEVC, VP8, VP9 or MPEG
video. It is an LGPL build: no GPL parts and no external libraries.

## Playing

| Bink call | The layer |
| --- | --- |
| `BinkOpen`, `BinkClose` | opens the file, makes the decoder, reads the header |
| `BinkWait`, `BinkDoFrame`, `BinkNextFrame` | decodes the next frame into the layer's own buffer |
| `BinkCopyToBuffer` | copies the frame into the plug-in's surface: 24, 32, 32+alpha, 555 and 565 surfaces |
| `BinkSetSoundSystem`, `BinkOpenDirectSound`, `BinkSetSoundTrack` | accepted and ignored |
| `BinkPause`, `BinkSetVolume`, `BinkGetFrameBuffersInfo` | as on Windows |
| `BinkGoto`, `BinkGetKeyFrame` | frame seek, decoded forward from the nearest key frame |

Sound goes to the movie's own SDL audio device, which the layer opens for the movie and closes with it.

## Evidence

`check_movies` builds a two-second WebM as `Movies\TEST.bik`, plays it through the layer, and compares three decoded
frames with FFmpeg's own decode of the same frames. It then plays the 1596x1080 intro from `media/Movies` (when it is
present) to its last frame and compares a frame in the middle. The timing check wants the two-second movie to play in
1.85 to 2.3 seconds.
