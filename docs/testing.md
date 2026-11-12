# Testing the native build

`tools/test_port_linux.py`

The harness builds each test against the layer's objects and runs it in a scratch folder. It then compares the layer
with independent programs: the installed game's files, FFmpeg, and Python's own codecs.

```
python3 -I tools/build_port.py --target linux win32 dmusic
python3 -I tools/test_port_linux.py                    # the 32-bit layer
python3 -I tools/test_port_linux.py --target linux64   # the 64-bit layer, against its own objects
```

The last line is the verdict, for example `tests: 18 in, 18 passed`. The count is the eleven test binaries plus the
seven checks below.

## The tests

Each `tests/port/linux/test_*.cpp` is one binary. Every expected value comes from arithmetic, from the documented
behaviour of the Windows function, or from another program; the layer is never compared with itself.

| Test | Subject |
| --- | --- |
| `test_win32` | kernel objects, waits, threads, critical sections, the clock, text conversion, `MulDiv`, `GetCurrentThread`'s pseudo handle |
| `test_window` | window classes, creation messages, rectangles, posted/sent/thread messages, the `PeekMessage` order, timers, focus, the cursor, the clipboard |
| `test_gdi` | pixel formats, DIB layouts, raster operations, shapes, clipping, object select/delete, text drawing, window DCs, `AlphaBlend` |
| `test_ddraw` | DirectDraw 7 and Direct3D 7 through the COM interfaces: surfaces, blits, colour keys, flips, device contexts, the fixed-function pipeline, render targets, colour keying |
| `test_dinput` | the game's own input set-up, keys, buttons, axes, the buffer and its overflow, relative and absolute axes, acquisition as the focus moves |
| `test_miles` | the Miles preferences, files, the mix with Miles' volume and pan rules, loop counts, positions, rates, the DirectSound object |
| `test_dmusic` | the DirectMusic COM classes, the port's properties, the loader on the game's own music, playback through the mixer, volume, reverb, transitions, the MIDI bank |
| `test_socket` | Winsock over loopback: start-up, error codes, `select` on the Windows `fd_set`, `ioctlsocket`, the `WSAEventSelect` rules, `SIGPIPE`, `GetAdaptersInfo` |
| `test_system` | conversion buffer rules, `wsprintf`, INI files, global memory, multimedia timers, the clock, version and volume queries |
| `test_registry` | the import from a Wine prefix, value types and sizes, the file written, key deletion |
| `test_cxxabi` | MSVC's `dynamic_cast` rule: the cross-cast the game makes, a cast that must still fail, null, and both reference forms |

The display tests choose SDL's offscreen video driver and Mesa's software renderer, so no window and no graphics card
are needed, and the expected pixels do not depend on the machine.

## The checks against other programs

| Check | Compares the layer with |
| --- | --- |
| `check_strings` | every string of the installed `Language.dll`, read by `LoadStringA`, against ee-toolkit's PE reader (3712 strings) |
| `check_text` | Windows-1252 and UTF-8 conversion against Python's codecs, and `MulDiv` against exact arithmetic |
| `check_gdi_metrics` | 169 font requests against a Windows 11 capture: Tahoma's line metrics and Courier New's widths and extents must be exact |
| `check_movies` | three frames of a made WebM and one of the intro against FFmpeg's decode of the same frames |
| `check_miles` | the decoded PCM of the base game's music and of AoC's `data.ssa` against FFmpeg's decode, sample for sample |
| `check_dmusic` | the GUIDs against Wine's headers, and which DLS collections each of the game's eight songs asks for against a RIFF walk of the song files |
| `check_linkage` | every definition in the layer: a Windows function must have C linkage, or the game's call would stay undefined |

The font check needs the fonts it measures; the Miles check needs the installed game's music; the movie intro check
needs `media/Movies` and reports that it did not run when the file is absent.

## What the tests do not cover

- A real window, a real GPU and a real sound device. The display tests are software-rendered and headless, and the
  sound tests mix in memory.
- Frame timing and the pointer's flicker, which need a display.
- The game itself. The tests call the layer; the game played through it is the other evidence
  ([troubleshooting](troubleshooting.md) lists what was checked on a real desktop).
- The NVIDIA driver. Every pixel check so far ran on Mesa.

## Adding a test

A new `tests/port/linux/test_*.cpp` is picked up automatically: it is built with the layer's objects and run with
`EE_PORT_TEST_DIR` pointing at a scratch folder. A cross-check that needs another program is a `check_*` function in
`tools/test_port_linux.py`, added to the `checks` list at the end.
