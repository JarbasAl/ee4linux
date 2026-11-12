# Direct3D 7 on OpenGL

`src/Port/Linux/Direct3D.cpp`

The plug-in uses Direct3D 7's HAL device and its T&L HAL device, one vertex format, and the fixed-function pipeline as
it was in 2000. The layer implements all of it with one GLSL program.

## Devices

`IDirect3D7::CreateDevice` makes a device for a render target: the primary, a back buffer, or an offscreen surface
created with `DDSCAPS_3DDEVICE`. A Z buffer is attached with `AddAttachedSurface`. Both the HAL and the T&L HAL device
are accepted, because the game always lets Direct3D transform and light.

## Draws are recorded

A draw (`DrawPrimitive`, `DrawIndexedPrimitive` and the rest) copies its vertices and indices and a snapshot of the
state it draws with. The copy runs as one batch:

- at `EndScene`;
- earlier, when DirectDraw needs a surface that a recorded draw writes or samples (`SubmitDraws`). A `Lock`, `Blt`,
  `GetDC`, `Flip` or read-back of such a surface runs the draws first.

That lets the game change its vertex buffer after a draw, as `DDLOCK_DISCARDCONTENTS` and `DDLOCK_NOOVERWRITE` permit,
and keeps the surfaces consistent for `Lock` and GDI.

## The single vertex format

The game uses one flexible vertex format, `D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1` (0x152), 36
bytes per vertex. Every recorded vertex is converted to it, so the shader has one input layout.

## The pipeline

| Direct3D | The layer |
| --- | --- |
| transform | world, view and projection in the shader |
| lighting | emissive and ambient, and up to 8 directional and point lights with range and attenuation. Material colour sources come from the vertex colour. |
| texture stage 0 | modulate, add, subtract, blend and the select modes, with the stage's colour and alpha operations |
| alpha test, blending | in the shader, with the alpha test on `round(a × 255)` |
| colour keying | `D3DRENDERSTATE_COLORKEYENABLE`: the texture is uploaded with alpha 0 at the keyed texels |
| fog | linear and exponential vertex fog |
| culling | `D3DCULL_CW` culls triangles that turn clockwise on the screen |
| viewport, scissor | as set, in surface rows from the top |
| `Clear` | colour, Z and stencil, over the rectangles that intersect the viewport |

## Conventions

Direct3D's conventions are kept in the shader and in the state, so the reconstructed code's numbers mean what they
mean on Windows:

- clip-space z runs from 0 to `w` (Direct3D's `z' = 2z - w`), where OpenGL runs from `-w` to `w`;
- a pixel centre is at whole coordinates, so the vertex shader moves geometry by 63/128 of a pixel, as Wine does;
- a texture's row 0 is its top;
- a render target's window row equals its surface row, so the shader negates y and viewports, scissors and read-backs
  need no flip.

## Textures and mipmaps

A texture is a surface with `DDSCAPS_TEXTURE`. Its mip levels are a chain, and the upload writes every level.
`SetLOD`, `GetLOD`, priorities and private data are kept on the surface.

## What is not there

`IDirect3DDevice7::ProcessVertices` is unsupported, and so are the retained-mode execute buffers, the texture
management interfaces and the Direct3D 3 interfaces. The game calls none of them. Spot lights are not implemented
(the game uses none); point and directional lights are.
