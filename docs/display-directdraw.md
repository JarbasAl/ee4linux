# DirectDraw 7 on OpenGL

`src/Port/Linux/DirectDraw.cpp`, `src/Port/Linux/DirectDrawInternal.h`

The DX7 display plug-in draws through DirectDraw 7 and Direct3D 7. This page covers DirectDraw: the device, the
surfaces, the blits, the colour keys and the presents. [Direct3D](display-direct3d.md) covers the 3D side.

## One device, one display

The layer implements one `IDirectDraw7`, the primary display, at the desktop's size. `SetCooperativeLevel` follows
DirectDraw's rules: exclusive needs full screen and a window, normal must not ask for either, and a mode set without
exclusive mode returns `DDERR_NOEXCLUSIVEMODE`.

A display mode is a size and a depth that the layer pretends to set. In full screen the game's window covers the
desktop and the primary is scaled to fit it with its aspect kept and centred, so the picture keeps the mode's shape on
any desktop. A windowed game's primary is the window's client area, and its rectangles are in screen coordinates, as
the desktop primary's are on Windows.

Modes on offer: 640x480, 800x600 and the common sizes up to the desktop's (1024x768, 1152x864, 1280x720, 1280x800,
1280x960, 1280x1024, 1366x768, 1440x900, 1600x900, 1600x1200, 1680x1050, 1920x1080, 1920x1200, 2560x1440, 2560x1600,
3840x2160), each in 16 and 32 bits at the desktop's refresh rate. `GetAvailableVidMem` answers a fixed 512 MB of
video memory, 448 MB of it free.

## Surfaces keep their pixels twice

A surface keeps its image in memory, in the exact byte layout of its pixel format, and in an RGBA8 OpenGL texture with
the same rows, row 0 first. Either copy can be the newer one:

- `Lock`, `Unlock`, `GetDC`, `ReleaseDC`, colour fills and blits that need the pixels write the memory;
- Direct3D draws write the texture.

A call that needs a copy brings it up to date, and only then: the memory is read back from the texture, or the texture
is uploaded from the memory. That is what makes `Lock` and `Blt` after a 3D frame cheap, and what keeps a surface that
only Direct3D touches out of memory traffic.

The pixel formats support 16 and 32 bits with arbitrary channel masks, plus the Z buffer surfaces (a depth
renderbuffer). Mipmap chains and flip chains are surfaces that share a root: the root owns the texture, the members
own their memory.

## Blits

`Blt` and `BltFast` implement the subset that the plug-in uses:

| Flag or case | Behaviour |
| --- | --- |
| destination rectangle | in screen coordinates when the destination is a windowed primary |
| without a clipper | a rectangle that leaves the surface is `DDERR_INVALIDRECT`; a windowed primary clips to the window instead |
| `DDBLT_COLORFILL` | fills the rectangle with `dwFillColor`, in the target's format |
| `DDBLT_KEYSRC` | the source's colour key leaves the destination as it was |
| `DDBLT_KEYSRCOVERRIDE` | the key comes from `DDBLTFX` |
| stretch | nearest sampling: a destination pixel takes the source pixel under its centre |
| overlapping | a blit from a surface to itself reads the image before it writes |

There are two paths. If the destination's texture is the newer copy, the blit draws into the texture with the layer's
blit shader. Otherwise the layer copies in memory. A keyed blit through the GPU stages the keyed source in a scratch
texture, with alpha 0 for the keyed texels, and the shader discards those.

A colour key names values in the surface's own format, and only the bits that the format's masks define take part: a
32-bit surface without an alpha channel still carries a byte in that position, which the game fills with `0xff`, and
that byte is not part of the pixel.

## The front buffer shows writes

Writes to the primary outside `Flip` (the mouse pointer thread's restore, save and draw, a GDI device context, a
windowed present) do not show one by one. The presenter thread shows them after a 4 ms pause, and at the latest 16 ms
after the first, so the steps of one pointer move reach the screen as one image, as they do on Windows' front buffer,
where each step lasts microseconds. `Flip` shows at once and takes any pending present with it.

`Flip` exchanges the storage of the chain's surfaces, so a back buffer becomes the front without a copy, and then
presents the primary.

## Device contexts on surfaces

`GetDC` gives a device context that draws into the surface's memory ([GDI](gdi-and-fonts.md)). While the game holds
it, the layer keeps a copy of the image as it was when the DC was handed out, to find what GDI wrote. Before GDI draws
text, the layer runs the draws recorded so far and reads back the area under the text, so the text blends with what
Direct3D left there instead of with an old image, and every pixel GDI writes is found when the DC is released.

`ReleaseDC` lays those pixels over the surface: into the texture with a blit when the surface is renderable, or into
the memory otherwise.

## What is not there

`CreateClipper` and `SetClipper` return `DDERR_UNSUPPORTED` (the game creates none), `CreatePalette` likewise (all
modes are 16 or 32 bits), and `BltBatch`, `EnumAttachedSurfaces`, `GetOverlayPosition`, the overlays,
`ProcessVertices`, spot lights and `DDBLT_DEPTHFILL` are unsupported or do nothing. Each writes one log line. No game
code calls them.
