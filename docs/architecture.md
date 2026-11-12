# Architecture

The port has three parts: the game's own source, the headers that let it compile, and the Win32 layer.

```
src/Empire Earth, src/RTS Engine      the game                    -> ee-aoc
src/Low-Level Engine                  the engine                  -> Low-Level Engine.dll
src/Low-Level Engine/.../DX7 ...      the renderer plug-ins       -> DX7HRTnLDisplay.dll
src/Low-Level Engine/.../TCP ...      the network plug-in         -> TCP Protocol.dll
src/Low-Level Engine/.../Miles ...    the sound plug-in           -> Miles Sound System Mixer.dll
src/ThirdParty                        libpng, zlib, libjpeg, a DCL reimplementation
include/port, include/winsdk          the dialect gaps and missing SDK headers
src/Port/Linux                        the Win32 layer             -> libeeport.so
src/Port/*.cpp                        the entry point and link shims
```

## Why the images keep their Windows names

The engine finds its renderer by scanning the game folder for `*.dll` and asking each module for the entry points it
needs (`RPIMGetRasterizer`, and the equivalent in the other plug-ins). A retail image next to the native ones exports
the same names, so the scan would load both.

The layer's module loader reads an ELF shared object as code (`dlopen`), and a Windows PE file as a module of
resources only, with no code. The engine's scan therefore keeps the first image that answers, and the retail DLLs sit
in the folder as data.

For the native build the same scan finds the native plug-ins, which are ELF shared objects with the Windows names.

## The Win32 layer

`src/Port/Linux/` implements the part of Win32 that the game calls. Everything it defines has C linkage and the
Windows calling convention of a 32-bit build. Wine's headers declare those functions, so a unit that calls
`CreateWindowExA` compiles unchanged, and the link takes the layer's definition.

The units, in the order they matter:

| Unit | Subject |
| --- | --- |
| `Win32Sync.cpp` | kernel objects, waits, threads, critical sections, the clock |
| `Win32Window.cpp` | windows, messages, focus, cursors, the clipboard, input method stubs |
| `Win32Gdi.cpp` | device contexts, bitmaps, drawing primitives, text |
| `GdiFontDB.cpp`, `GdiTrueType.cpp` | the installed faces and TrueType rasterizing |
| `DirectDraw.cpp` | DirectDraw 7 on OpenGL |
| `Direct3D.cpp` | Direct3D 7 on OpenGL |
| `DirectInput.cpp` | DirectInput 7 from SDL's input |
| `DirectMusic.cpp` | DirectMusic over dmusic |
| `Miles.cpp` | the Miles Sound System over SDL audio |
| `Bink.cpp` | the Bink API over FFmpeg |
| `Win32File.cpp`, `Win32Path.cpp` | files, folders, finds, mappings, Windows paths |
| `Win32Registry.cpp` | the registry, as a `.reg` file |
| `Win32Module.cpp` | modules and PE resources |
| `Win32System.cpp` | text conversion, version queries, memory, timers, INI files, processes, COM start-up |
| `Win32Socket.cpp` | Winsock over BSD sockets |
| `Win32Keys.cpp` | scan codes and virtual keys |
| `Guids.cpp` | the DirectX and DirectMusic GUIDs |
| `CxxAbi.cpp` | MSVC's `dynamic_cast` rule ([C++ runtime](cxx-runtime.md)) |

`Win32Internal.h` is the header the layer's units share: the `eeport` helpers, the SDL lock, the thread ids, and the
declarations of the internals that more than one unit needs.

## The dialect gap

The game was written for Visual C++ 6 and its Dinkum C++ library. `include/port/compat.h` is included first in every
unit of every target and closes the gap: `__try`/`__except`, `char_traits<unsigned char>`, `min`/`max`, the missing
`typeid` include, and the VC6 idioms a modern compiler rejects.

`include/port/linux/` then adds the Linux-only part:

| Header | What it adds |
| --- | --- |
| `windows.h` | Wine's header, plus `MAKEINTRESOURCE` with VC6's truncation, and `wchar_t` overloads for the text functions (Windows' `wchar_t` is 16 bits, Linux's is 32) |
| `port_msvcrt.h` | the MSVC runtime names glibc lacks: `_mbs*`, `_stricmp`, `_snprintf`, `_hypot`, … in the C locale |
| `winsock2.h`, `winsock.h` | Windows' socket names and `fd_set` layout over glibc's sockets |
| `dinput.h`, `process.h`, `io.h`, `direct.h`, `crtdbg.h`, `mbstring.h` | the parts of the VC6 headers Wine's SDK does not carry |

`include/port/case/` holds the SDK headers whose spelling the game expects (`Mmsystem.h`).

`include/winsdk/` holds the retail SDK headers the reconstruction needs and the modern toolchain does not ship.

## The layers of a frame

A frame crosses the port twice: the game draws through Direct3D into a surface's texture, then DirectDraw shows the
primary in the window.

```
game -> IDirect3DDevice7::DrawPrimitive      recorded, with a copy of the state
     -> EndScene                            one GLSL batch into the surface's RGBA8 texture
     -> flip or blit                        the primary's texture, scaled, to the default framebuffer
     -> SDL_GL_SwapWindow                   the window
```

GDI text on a surface takes a third path: it draws into a buffer device context that holds the same pixels, and the
layer lays what it wrote over the recorded draws in call order ([DirectDraw](display-directdraw.md)).

## Threads

The game creates its own threads through the layer, and several of them draw: the graphics thread, the mouse pointer
thread, the data loading thread, and the start screen's thread. The layer serialises DirectDraw and Direct3D with one
recursive lock, as `DDSCL_MULTITHREADED` asks, and SDL video with another ([threads](threads-and-sync.md)).
