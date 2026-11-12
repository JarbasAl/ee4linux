# GDI and fonts

`src/Port/Linux/Win32Gdi.cpp`, `GdiFontDB.cpp`, `GdiTrueType.cpp`

## Bitmaps and device contexts

A bitmap is a pixel buffer: 32-bit for the bitmaps GDI makes (compatible bitmaps, DIB conversions, resources), or the
caller's own layout for a DIB section and for a DirectDraw surface's device context.

A device context draws into one of three backings:

| Kind | Backing |
| --- | --- |
| memory DC | the selected bitmap |
| window DC | a 32-bit buffer the layer keeps for the window, shown in the window's native SDL window on release |
| buffer DC | pixels the caller keeps: a DirectDraw surface (`CreateBufferDC`), with the surface's own pitch and format |

A window DC's buffer is shown at once when the thread that owns the window draws, or at that thread's next message
pump otherwise. Showing means `SDL_UpdateWindowSurface` on the window's software native window.

## Drawing

Implemented: `SetPixel`, `GetPixel`, `FillRect`, `Rectangle`, `Ellipse`, `Arc`, `Polyline`, `Polygon`, `BitBlt`,
`AlphaBlend`, the brushes and pens (`CreateSolidBrush`, `CreateBrushIndirect`, `CreatePen`, the stock objects), the clip
rectangle and region (`IntersectClipRect`, `SelectClipRgn`), `CreateCompatibleBitmap`, `CreateDIBSection`,
`CreateDIBitmap`, `LoadBitmapA`, `GetDIBits`, `GetObjectA`, and the icons (`CreateIconIndirect`, `DestroyIcon`).

`GetDeviceCaps` answers the numbers the game reads: the screen size, the desktop's size, the bit depth, the colour
resolution, the raster capabilities, and the font metrics that Windows reports.

## Text

Text is drawn with TrueType, through stb_truetype, from the faces installed on the computer. The measuring is GDI's,
from the font's own Windows tables (VDMX, hdmx and the rest) in `GdiTrueType.cpp`.

`CreateFontA`, `SelectObject`, `GetTextMetricsA`, `GetTextExtentPoint32A`, `GetCharWidthW`, `ExtTextOutA` and the
text colours behave as the game expects: a negative height is a character height, the weight and italic flags select
the face, and the escapement rotates the text. `TextOutW` comes through the `wchar_t` overload in
`include/port/linux/windows.h`. The game draws all its text through these, so `DrawTextA` and `TextOutA` are not
implemented.

`AddFontMemResourceEx` adds a face that the program carries; the game does not use it, the layer supports it.

## Where the faces come from

The game asks for Arial, Courier New, MS Sans Serif, Tahoma, Times New Roman and Verdana. The layer searches these
folders, in this order:

1. the folders in `EE_PORT_FONTS`, colon-separated;
2. `drive_c/windows/Fonts` in the Wine prefix (`WINEPREFIX`, else `~/.wine`), which is where `winetricks corefonts`
   and `winetricks tahoma` install the Microsoft faces;
3. `~/.local/share/fonts` (or `$XDG_DATA_HOME/fonts`) and `~/.fonts`;
4. `/usr/local/share/fonts`, `/usr/share/fonts` and `/usr/share/wine/fonts`.

Wine's `FontSubstitutes` registry values are honoured first, then the metric-compatible stand-ins:

| Asked for | Drawn with |
| --- | --- |
| Arial | Liberation Sans |
| Times New Roman, TimesRoman | Liberation Serif |
| Courier New | Liberation Mono |
| Tahoma, Microsoft Sans Serif | Wine's Tahoma |
| Verdana | DejaVu Sans |
| anything else | Arial's stand-in |

The first use of a name writes the choice to the log: `font "Arial" is drawn with Liberation Sans`.

## What the stand-ins cost

Liberation Sans and Liberation Serif have no VDMX or hdmx tables, so their line heights and some hinted widths differ
from Arial's and Times New Roman's by up to a pixel, and the game's text can be a pixel taller or a word a pixel
wider. The `check_gdi_metrics` test prints the exact agreement per face against a Windows 11 capture; Tahoma and
Courier New must agree exactly, because their stand-ins do.

To measure as on Windows, install the Microsoft faces into the Wine prefix:

```
winetricks corefonts tahoma
```

## GDI on a DirectDraw surface

Text drawn into a surface's DC blends with what Direct3D left there: before the text, the layer runs the recorded
draws and reads the area under the text back from the texture into the surface's memory, which is also the DC's
pixels ([DirectDraw](display-directdraw.md)). Every pixel GDI writes there is what gets compared on release, so only
the text lands on the surface.

## Resources

`LoadStringA` reads the installed `Language.dll`'s string table through the layer's PE resource reader, and
`LoadBitmapA`, `LoadCursorA`, `LoadIconA` read the executable's resources. `check_strings` compares all 3712 strings
of the installed file with an independent PE reader.
