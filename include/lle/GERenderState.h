// lle/GERenderState.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_GERENDERSTATE_H
#define EE_GERENDERSTATE_H

// GETStage::mTextureAddressingMode (GETexture::SetStageTextureAddressingMode): DX7Rasterizer::SetTexture maps each to D3DTADDRESS_*.
enum GEAddressingMode {
    kAddressingClamp = 0,  // DX7Rasterizer::SetTexture: 0 -> D3DTADDRESS_CLAMP
    kAddressingMirror = 1,  // DX7Rasterizer::SetTexture: 1 -> D3DTADDRESS_MIRROR
    kAddressingWrap = 2,  // DX7Rasterizer::SetTexture: 2 -> D3DTADDRESS_WRAP
};

// GERasterizer::SetBlending source / destination factors: DX7Rasterizer::SetBlending 0x1000b4ea maps each to the
// Direct3D 7 D3DBLEND_* value (the SDK names).
enum GEBlendFactor {
    kBlendZero = 0,            // DX7Rasterizer::SetBlending 0x1000b4ea: 0 -> D3DBLEND_ZERO
    kBlendOne = 1,             // DX7Rasterizer::SetBlending 0x1000b4ea: 1 -> D3DBLEND_ONE
    kBlendSrcColor = 2,        // DX7Rasterizer::SetBlending 0x1000b4ea: 2 -> D3DBLEND_SRCCOLOR
    kBlendInvSrcColor = 3,     // DX7Rasterizer::SetBlending 0x1000b4ea: 3 -> D3DBLEND_INVSRCCOLOR
    kBlendSrcAlpha = 4,        // DX7Rasterizer::SetBlending 0x1000b4ea: 4 -> D3DBLEND_SRCALPHA
    kBlendInvSrcAlpha = 5,     // DX7Rasterizer::SetBlending 0x1000b4ea: 5 -> D3DBLEND_INVSRCALPHA
    kBlendDestAlpha = 6,       // DX7Rasterizer::SetBlending 0x1000b4ea: 6 -> D3DBLEND_DESTALPHA
    kBlendInvDestAlpha = 7,    // DX7Rasterizer::SetBlending 0x1000b4ea: 7 -> D3DBLEND_INVDESTALPHA
    kBlendDestColor = 8,       // DX7Rasterizer::SetBlending 0x1000b4ea: 8 -> D3DBLEND_DESTCOLOR
    kBlendInvDestColor = 9,    // DX7Rasterizer::SetBlending 0x1000b4ea: 9 -> D3DBLEND_INVDESTCOLOR
    kBlendSrcAlphaSat = 10,    // DX7Rasterizer::SetBlending 0x1000b4ea: 10 -> D3DBLEND_SRCALPHASAT
    kBlendBothSrcAlpha = 11,   // DX7Rasterizer::SetBlending 0x1000b4ea: 11 -> D3DBLEND_BOTHSRCALPHA
    kBlendBothInvSrcAlpha = 12,  // DX7Rasterizer::SetBlending 0x1000b4ea: 12 -> D3DBLEND_BOTHINVSRCALPHA
};

// GERasterizer::SetZCompare / SetAlphaTestingCompareState functions: DX7Rasterizer::SetZCompare 0x1000c910 and
// SetAlphaTestingCompareState 0x1000b26c map each to D3DCMP_* (the SDK names).
enum GECompareFunction {
    kCompareNever = 0,       // DX7Rasterizer::SetZCompare 0x1000c910: 0 -> D3DCMP_NEVER
    kCompareLess = 1,        // DX7Rasterizer::SetZCompare 0x1000c910: 1 -> D3DCMP_LESS
    kCompareLessEqual = 2,   // DX7Rasterizer::SetZCompare 0x1000c910: 2 -> D3DCMP_LESSEQUAL
    kCompareEqual = 3,       // DX7Rasterizer::SetZCompare 0x1000c910: 3 -> D3DCMP_EQUAL
    kCompareNotEqual = 4,    // DX7Rasterizer::SetZCompare 0x1000c910: 4 -> D3DCMP_NOTEQUAL
    kCompareGreaterEqual = 5,  // DX7Rasterizer::SetZCompare 0x1000c910: 5 -> D3DCMP_GREATEREQUAL
    kCompareGreater = 6,     // DX7Rasterizer::SetZCompare 0x1000c910: 6 -> D3DCMP_GREATER
    kCompareAlways = 7,      // DX7Rasterizer::SetZCompare 0x1000c910: 7 -> D3DCMP_ALWAYS
};

// GERasterizer::SetCulling / mCulling: DX7Rasterizer::SetCulling 0x1000b7e7 maps 1 -> D3DCULL_CW, 2 -> D3DCULL_CCW, anything else -> D3DCULL_NONE.
enum GECullMode {
    kCullNone = 0,  // DX7Rasterizer::SetCulling 0x1000b7e7: default -> D3DCULL_NONE
    kCullCW = 1,  // DX7Rasterizer::SetCulling 0x1000b7e7: 1 -> D3DCULL_CW
    kCullCCW = 2,  // DX7Rasterizer::SetCulling 0x1000b7e7: 2 -> D3DCULL_CCW
};

// GERasterizer::SetDrawStyle: DX7Rasterizer::SetDrawStyle 0x1000b916 maps each to D3DFILL_*.
enum GEDrawStyle {
    kDrawStyleSolid = 0,    // DX7Rasterizer::SetDrawStyle 0x1000b916: 0 -> D3DFILL_SOLID
    kDrawStyleWireframe = 1,  // DX7Rasterizer::SetDrawStyle 0x1000b916: 1 -> D3DFILL_WIREFRAME
    kDrawStylePoint = 2,    // DX7Rasterizer::SetDrawStyle 0x1000b916: 2 -> D3DFILL_POINT
};

// GETStage::mMagFilterMode: DX7Rasterizer::SetTexture maps each to D3DTFG_*.
enum GEMagFilter {
    kMagFilterPoint = 0,    // DX7Rasterizer::SetTexture: 0 -> D3DTFG_POINT
    kMagFilterLinear = 1,   // DX7Rasterizer::SetTexture: 1 -> D3DTFG_LINEAR
    kMagFilterFlatCubic = 2,  // DX7Rasterizer::SetTexture: 2 -> D3DTFG_FLATCUBIC
};

// GETStage::mMipFilterMode (GETexture::SetStageMipFilterMode, GETextureManager::UpdateMIPFilterMode): DX7Rasterizer::SetTexture maps each to D3DTFP_*.
enum GEMipMapFilter {
    kMipFilterNone = 0,  // DX7Rasterizer::SetTexture: 0 -> D3DTFP_NONE
    kMipFilterPoint = 1,  // DX7Rasterizer::SetTexture: 1 -> D3DTFP_POINT
    kMipFilterLinear = 2,  // DX7Rasterizer::SetTexture: 2 -> D3DTFP_LINEAR
};

enum GEPixelFormatID {
    kPixelFormatA8R8G8B8 = 0,  // DX7Screen gPixelFormats[0] RGB|ALPHAPIXELS 32: 00ff0000/0000ff00/000000ff/ff000000 (provisional name)
    kPixelFormatX8R8G8B8 = 1,  // DX7Screen gPixelFormats[1] RGB 32: 00ff0000/0000ff00/000000ff (provisional name)
    kPixelFormatA4R4G4B4 = 2,  // DX7Screen gPixelFormats[2] RGB|ALPHAPIXELS 16: 0f00/00f0/000f/f000 (provisional name)
    kPixelFormatA1R5G5B5 = 3,  // DX7Screen gPixelFormats[3] RGB|ALPHAPIXELS 16: 7c00/03e0/001f/8000 (provisional name)
    kPixelFormatR5G5B5A1 = 4,  // DX7Screen gPixelFormats[4] RGB|ALPHAPIXELS 16: f800/07c0/003e/0001 (provisional name)
    kPixelFormatX1R5G5B5 = 5,  // DX7Screen gPixelFormats[5] RGB 16: 7c00/03e0/001f (provisional name)
    kPixelFormatR5G5B5X1 = 6,  // DX7Screen gPixelFormats[6] RGB 16: f800/07c0/003e (provisional name)
    kPixelFormatR5G6B5 = 7,    // DX7Screen gPixelFormats[7] RGB 16: f800/07e0/001f (provisional name)
    kPixelFormatDXT1 = 8,      // DX7Screen gPixelFormats[8] FOURCC DXT1 (provisional name)
    kPixelFormatZ32 = 9,       // DX7Screen gPixelFormats[9] ZBUFFER 32 (provisional name)
    kPixelFormatZ24 = 10,      // DX7Screen gPixelFormats[10] ZBUFFER 24 (provisional name)
    kPixelFormatZ16 = 11,      // DX7Screen gPixelFormats[11] ZBUFFER 16 (provisional name)
    kPixelFormatZPrefer16 = 12,  // CreateSurface type 3: tries 0xb (16), then 0xa (24), then 9 (32) (provisional name)
    kPixelFormatZPrefer32 = 13,  // CreateSurface type 3: tries 9 (32), then 0xa (24), then 0xb (16) (provisional name)
    kPixelFormatUnknown = 14,  // EnumVideoModeCallback: default when no gPixelFormats entry matches the mode (provisional name)
};

// GETStage::mColorMode / mAlphaMode (GETexture::SetStageColorMode / SetStageAlphaMode): DX7Rasterizer::SetTexture
// 0x1000bec0 sets the stage-0 D3DTSS_COLOROP/ALPHAOP and arguments per mode. Names are provisional from the D3D setup.
enum GEStageOperation {
    kStageOpDiffuse = 0,        // DX7Rasterizer::SetTexture: 0 -> D3DTOP_SELECTARG2, arg2 D3DTA_DIFFUSE (provisional name)
    kStageOpTexture = 1,        // DX7Rasterizer::SetTexture: 1 -> D3DTOP_SELECTARG1, arg1 D3DTA_TEXTURE (provisional name)
    kStageOpModulate = 2,       // DX7Rasterizer::SetTexture: 2 -> D3DTOP_MODULATE texture x D3DTA_DIFFUSE (provisional name)
    kStageOpModulateCurrent = 3,  // DX7Rasterizer::SetTexture: 3 -> D3DTOP_MODULATE texture x D3DTA_CURRENT (provisional name)
};

enum GESurfaceType {
    kSurfacePrimary = 0,         // DX7Screen::CreateSurface 0x1000d7e4: 0 -> DDSCAPS_PRIMARYSURFACE (provisional name)
    kSurfaceOffscreen = 1,       // DX7Screen::CreateSurface 0x1000d7e4: 1 -> DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY (provisional name)
    kSurfaceFlippingPrimary = 2,  // DX7Screen::CreateSurface 0x1000d7e4: 2 -> DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX, back buffer count (provisional name)
    kSurfaceZBuffer = 3,         // DX7Screen::CreateSurface 0x1000d7e4: 3 -> DDSCAPS_ZBUFFER (provisional name)
    kSurfaceSystemMemory = 4,    // DX7Screen::CreateSurface 0x1000d7e4: 4 -> DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY (provisional name)
    kSurfaceTexture = 5,         // DX7Screen::CreateSurface 0x1000d7e4: 5 -> DDSCAPS_TEXTURE (provisional name)
    kSurfaceMipMappedTexture = 6,  // DX7Screen::CreateSurface 0x1000d7e4: 6 -> DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP (provisional name)
    kSurfaceBackBuffer = 7,      // DX7Screen::CreateSurface 0x1000d7e4: 7 -> the attached flip surfaces of a type-2 surface get type 7 (provisional name)
};

// GETStage::mMinFilterMode: DX7Rasterizer::SetTexture maps each to D3DTFN_*.
enum GETextureMinFilter {
    kMinFilterPoint = 0,      // DX7Rasterizer::SetTexture: 0 -> D3DTFN_POINT
    kMinFilterLinear = 1,     // DX7Rasterizer::SetTexture: 1 -> D3DTFN_LINEAR
    kMinFilterAnisotropic = 2,  // DX7Rasterizer::SetTexture: 2 -> D3DTFN_ANISOTROPIC
};

// GERasterizer::SetZBuffer state bits: DX7Rasterizer::SetZBuffer 0x1000c84a writes D3DRENDERSTATE_ZWRITEENABLE = bit 0,
// D3DRENDERSTATE_ZENABLE = bit 1 or bit 0. Names are provisional.
enum GEZBufferMode {
    kZBufferDisabled = 0,    // DX7Rasterizer::SetZBuffer 0: ZENABLE 0, ZWRITEENABLE 0 (provisional name)
    kZBufferWriteOnly = 1,   // DX7Rasterizer::SetZBuffer bit 0: ZWRITEENABLE (and ZENABLE) (provisional name)
    kZBufferTestOnly = 2,    // DX7Rasterizer::SetZBuffer bit 1: ZENABLE without ZWRITEENABLE (provisional name)
    kZBufferTestAndWrite = 3,  // DX7Rasterizer::SetZBuffer bits 0+1 (provisional name)
};

#endif // EE_GERENDERSTATE_H
