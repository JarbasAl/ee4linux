// Tests of the native build's DirectDraw 7 and Direct3D 7 (src/Port/Linux/DirectDraw.cpp and Direct3D.cpp) through
// the COM interfaces that the DX7 display plug-in calls. The test chooses SDL's offscreen video driver and Mesa's
// software renderer, so the pixels do not depend on a graphics card. Each expected pixel comes from arithmetic
// written out in the check: Direct3D 7's rules for coverage, lighting, fog, blending and the alpha test.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <ddraw.h>
#include <d3d.h>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

static int sFailures;
static int sChecks;

#define CHECK(condition)                                                                 \
    do {                                                                                 \
        ++sChecks;                                                                       \
        if (!(condition)) {                                                              \
            ++sFailures;                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);         \
        }                                                                                \
    } while (0)

// The pixel formats of the plug-in's gPixelFormats table (Common/DX7Screen.cpp)
static DDPIXELFORMAT Pixels(DWORD inBits, DWORD inRed, DWORD inGreen, DWORD inBlue, DWORD inAlpha)
{
    DDPIXELFORMAT theFormat = {};
    theFormat.dwSize = sizeof theFormat;
    theFormat.dwFlags = DDPF_RGB | (inAlpha ? DDPF_ALPHAPIXELS : 0);
    theFormat.dwRGBBitCount = inBits;
    theFormat.dwRBitMask = inRed;
    theFormat.dwGBitMask = inGreen;
    theFormat.dwBBitMask = inBlue;
    theFormat.dwRGBAlphaBitMask = inAlpha;
    return theFormat;
}
static const DDPIXELFORMAT kARGB = Pixels(32, 0xff0000, 0xff00, 0xff, 0xff000000);
static const DDPIXELFORMAT kXRGB = Pixels(32, 0xff0000, 0xff00, 0xff, 0);
static const DDPIXELFORMAT k4444 = Pixels(16, 0x0f00, 0x00f0, 0x000f, 0xf000);
static const DDPIXELFORMAT k565 = Pixels(16, 0xf800, 0x07e0, 0x001f, 0);
static const DDPIXELFORMAT k1555 = Pixels(16, 0x7c00, 0x03e0, 0x001f, 0x8000);

static IDirectDraw7* sDraw;
static IDirect3D7* s3D;

static IDirectDrawSurface7* Make(int inWidth, int inHeight, const DDPIXELFORMAT& inFormat, DWORD inCaps,
                                 DWORD inCaps2 = 0, DWORD inMore = 0)
{
    DDSURFACEDESC2 theDesc = {};
    theDesc.dwSize = sizeof theDesc;
    theDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | inMore;
    theDesc.dwWidth = (DWORD)inWidth;
    theDesc.dwHeight = (DWORD)inHeight;
    theDesc.ddpfPixelFormat = inFormat;
    theDesc.ddsCaps.dwCaps = inCaps;
    theDesc.ddsCaps.dwCaps2 = inCaps2;
    IDirectDrawSurface7* theSurface = nullptr;
    HRESULT theResult = sDraw->CreateSurface(&theDesc, &theSurface, nullptr);
    CHECK(theResult == DD_OK && theSurface);
    return theSurface;
}

static IDirectDrawSurface7* MakeZ(int inWidth, int inHeight, DWORD inBits)
{
    DDPIXELFORMAT theFormat = {};
    theFormat.dwSize = sizeof theFormat;
    theFormat.dwFlags = DDPF_ZBUFFER;
    theFormat.dwZBufferBitDepth = inBits;
    theFormat.dwZBitMask = inBits == 32 ? 0xffffffffu : (1u << inBits) - 1;
    return Make(inWidth, inHeight, theFormat, DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY);
}

// Reads or writes pixels through Lock, as the engine does
struct Locked {
    explicit Locked(IDirectDrawSurface7* inSurface, DWORD inFlags = 0) : mSurface(inSurface)
    {
        mDesc.dwSize = sizeof mDesc;
        mOk = inSurface->Lock(nullptr, &mDesc, inFlags, NULL) == DD_OK;
        CHECK(mOk);
    }
    ~Locked()
    {
        if (mOk)
            CHECK(mSurface->Unlock(nullptr) == DD_OK);
    }
    uint8_t* Row(int inY) { return (uint8_t*)mDesc.lpSurface + (size_t)inY * mDesc.lPitch; }
    DWORD Get(int inX, int inY)
    {
        if (mDesc.ddpfPixelFormat.dwRGBBitCount == 16)
            return ((uint16_t*)Row(inY))[inX];
        return ((DWORD*)Row(inY))[inX];
    }
    void Set(int inX, int inY, DWORD inValue)
    {
        if (mDesc.ddpfPixelFormat.dwRGBBitCount == 16)
            ((uint16_t*)Row(inY))[inX] = (uint16_t)inValue;
        else
            ((DWORD*)Row(inY))[inX] = inValue;
    }
    IDirectDrawSurface7* mSurface;
    DDSURFACEDESC2 mDesc = {};
    bool mOk;
};

static void Fill(IDirectDrawSurface7* inSurface, DWORD inValue, RECT* inRect = nullptr)
{
    DDBLTFX theFX = {};
    theFX.dwSize = sizeof theFX;
    theFX.dwFillColor = inValue;
    CHECK(inSurface->Blt(inRect, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &theFX) == DD_OK);
}

static bool Near(DWORD inGot, DWORD inWanted, int inTolerance)
{
    for (int theShift = 0; theShift < 24; theShift += 8) {
        int theGot = (inGot >> theShift) & 0xff, theWanted = (inWanted >> theShift) & 0xff;
        if (std::abs(theGot - theWanted) > inTolerance)
            return false;
    }
    return true;
}

static DWORD Rgb(double inRed, double inGreen, double inBlue)
{
    auto theByte = [](double inValue) { return (DWORD)std::lround(std::min(1.0, std::max(0.0, inValue)) * 255); };
    return theByte(inRed) << 16 | theByte(inGreen) << 8 | theByte(inBlue);
}

// FVF 0x152, the plug-in's vertex (include/dx7/DX7StdColorVertexArray.h)
struct Vertex {
    float x, y, z, nx, ny, nz;
    DWORD diffuse;
    float u, v;
};
static const DWORD kFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;

static Vertex V(float inX, float inY, float inZ, DWORD inDiffuse, float inU = 0, float inV = 0)
{
    return {inX, inY, inZ, 0, 0, -1, inDiffuse, inU, inV};
}

// The plug-in's 2D projection (Common/DX7Rasterizer.cpp:961-980): pixels to clip space, z and w unchanged
static D3DMATRIX Projection2D(int inWidth, int inHeight)
{
    D3DMATRIX theMatrix = {};
    theMatrix._11 = 2.0f / inWidth;
    theMatrix._22 = -2.0f / inHeight;
    theMatrix._33 = 1;
    theMatrix._41 = -1;
    theMatrix._42 = 1;
    theMatrix._44 = 1;
    return theMatrix;
}

static D3DMATRIX Identity()
{
    D3DMATRIX theMatrix = {};
    theMatrix._11 = theMatrix._22 = theMatrix._33 = theMatrix._44 = 1;
    return theMatrix;
}

struct Target {
    IDirectDrawSurface7* mSurface = nullptr;
    IDirectDrawSurface7* mZ = nullptr;
    IDirect3DDevice7* mDevice = nullptr;
    IDirect3DVertexBuffer7* mBuffer = nullptr;
    int mWidth, mHeight;
    DWORD mPosition = 0;

    Target(int inWidth, int inHeight, const DDPIXELFORMAT& inFormat, bool inZ, const GUID& inKind = IID_IDirect3DHALDevice)
        : mWidth(inWidth), mHeight(inHeight)
    {
        mSurface = Make(inWidth, inHeight, inFormat, DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY);
        if (inZ) {
            mZ = MakeZ(inWidth, inHeight, 16);
            CHECK(mSurface->AddAttachedSurface(mZ) == DD_OK);
        }
        CHECK(s3D->CreateDevice(inKind, mSurface, &mDevice) == D3D_OK && mDevice);
        D3DVERTEXBUFFERDESC theDesc = {};
        theDesc.dwSize = sizeof theDesc;
        theDesc.dwCaps = D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY;
        theDesc.dwFVF = kFVF;
        theDesc.dwNumVertices = 2400;
        CHECK(s3D->CreateVertexBuffer(&theDesc, &mBuffer, 0) == D3D_OK && mBuffer);
        // the game's 2D setup: world and view identity, the pixel projection, no lighting, colour from the vertex
        D3DMATRIX theIdentity = Identity(), theProjection = Projection2D(inWidth, inHeight);
        mDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &theIdentity);
        mDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &theIdentity);
        mDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &theProjection);
        mDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
        mDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
        mDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
        mDevice->SetTexture(0, nullptr);
        mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
    ~Target()
    {
        CHECK(mBuffer->Release() == 0);
        CHECK(mDevice->Release() == 0);
        mSurface->Release();
        if (mZ)
            mZ->Release();
    }

    void Clear(DWORD inColour)
    {
        CHECK(mDevice->BeginScene() == D3D_OK);
        CHECK(mDevice->Clear(0, nullptr, D3DCLEAR_TARGET | (mZ ? D3DCLEAR_ZBUFFER : 0), inColour, 1.0f, 0) == D3D_OK);
        CHECK(mDevice->EndScene() == D3D_OK);
    }

    // Writes vertices at the buffer's position as LockVB does, and draws them, indexed when given indices
    void Draw(D3DPRIMITIVETYPE inType, const std::vector<Vertex>& inVertices, const std::vector<WORD>& inIndices = {})
    {
        void* theData = nullptr;
        DWORD theFlags = DDLOCK_NOOVERWRITE | DDLOCK_WRITEONLY;
        if (mPosition + inVertices.size() > 2400) {
            mPosition = 0;
            theFlags = DDLOCK_DISCARDCONTENTS | DDLOCK_WRITEONLY;
        }
        CHECK(mBuffer->Lock(theFlags, &theData, nullptr) == D3D_OK && theData);
        memcpy((Vertex*)theData + mPosition, inVertices.data(), inVertices.size() * sizeof(Vertex));
        CHECK(mBuffer->Unlock() == D3D_OK);
        if (inIndices.empty())
            CHECK(mDevice->DrawPrimitiveVB(inType, mBuffer, mPosition, (DWORD)inVertices.size(), 0) == D3D_OK);
        else
            CHECK(mDevice->DrawIndexedPrimitiveVB(inType, mBuffer, mPosition, (DWORD)inVertices.size(),
                                                  (WORD*)inIndices.data(), (DWORD)inIndices.size(), 0) == D3D_OK);
        mPosition += (DWORD)inVertices.size();
    }

    // A quad over [x0, x1) x [y0, y1) in Direct3D's pixel coordinates, as the plug-in's bitmap draw indexes it
    void Quad(float inX0, float inY0, float inX1, float inY1, float inZ, DWORD inColour, float inU0 = 0, float inV0 = 0,
              float inU1 = 1, float inV1 = 1)
    {
        Draw(D3DPT_TRIANGLELIST,
             {V(inX0, inY0, inZ, inColour, inU0, inV0), V(inX1, inY0, inZ, inColour, inU1, inV0),
              V(inX0, inY1, inZ, inColour, inU0, inV1), V(inX1, inY1, inZ, inColour, inU1, inV1)},
             {0, 1, 2, 2, 1, 3});
    }

    // The 24-bit colour of a pixel of an X8R8G8B8 target
    DWORD At(int inX, int inY)
    {
        Locked theLock(mSurface, DDLOCK_READONLY);
        return theLock.Get(inX, inY) & 0xffffff;
    }
};

// Counts the pixels of a 32-bit target with exactly a colour, inside a rectangle and outside it
static void Coverage(Target& inTarget, DWORD inColour, const RECT& inRect, int* outInside, int* outOutside)
{
    Locked theLock(inTarget.mSurface, DDLOCK_READONLY);
    *outInside = *outOutside = 0;
    for (int y = 0; y < inTarget.mHeight; ++y)
        for (int x = 0; x < inTarget.mWidth; ++x) {
            bool theMatch = (theLock.Get(x, y) & 0xffffff) == inColour;
            bool theIn = x >= inRect.left && x < inRect.right && y >= inRect.top && y < inRect.bottom;
            (theIn ? *outInside : *outOutside) += theMatch;
        }
}

static BOOL WINAPI CountDevice(GUID* inGuid, char*, char*, void* inContext, HMONITOR inMonitor)
{
    CHECK(inGuid == nullptr && inMonitor == NULL);
    ++*(int*)inContext;
    return TRUE;
}

struct ModeList {
    bool mHas800x600x16 = false, mHasDesktop = false;
    int mCount = 0;
    DWORD mDesktopWidth, mDesktopHeight;
};

static HRESULT WINAPI ListMode(DDSURFACEDESC2* inDesc, void* inContext)
{
    ModeList* theList = (ModeList*)inContext;
    ++theList->mCount;
    DWORD theBits = inDesc->ddpfPixelFormat.dwRGBBitCount;
    if (inDesc->dwWidth == 800 && inDesc->dwHeight == 600 && theBits == 16 && inDesc->ddpfPixelFormat.dwRBitMask == 0xf800)
        theList->mHas800x600x16 = true;
    if (inDesc->dwWidth == theList->mDesktopWidth && inDesc->dwHeight == theList->mDesktopHeight && theBits == 32)
        theList->mHasDesktop = true;
    CHECK(inDesc->dwFlags & DDSD_REFRESHRATE);
    return DDENUMRET_OK;
}

static HRESULT WINAPI ListDevice(char*, char*, D3DDEVICEDESC7* inDesc, void* inContext)
{
    DWORD* theCaps = (DWORD*)inContext;
    *theCaps |= inDesc->dwDevCaps;
    return D3DENUMRET_OK;
}

static HRESULT WINAPI ListTextureFormat(DDPIXELFORMAT* inFormat, void* inContext)
{
    // the probe's test (Common/DX7Screen.cpp:411-421): a 32-bit format whose masks cover ARGB
    if (inFormat->dwRGBBitCount == 32 && (inFormat->dwRGBAlphaBitMask & 0xff000000) &&
        (inFormat->dwRBitMask & 0xff0000) && (inFormat->dwGBitMask & 0xff00) && (inFormat->dwBBitMask & 0xff))
        *(bool*)inContext = true;
    return D3DENUMRET_OK;
}

static void TestSetup()
{
    int theDevices = 0;
    CHECK(DirectDrawEnumerateExA(CountDevice, &theDevices, DDENUM_ATTACHEDSECONDARYDEVICES |
                                 DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES) == DD_OK);
    CHECK(theDevices == 1);
    CHECK(DirectDrawCreateEx(nullptr, (void**)&sDraw, IID_IDirectDraw7, nullptr) == DD_OK && sDraw);
    if (!sDraw)
        exit(1);
    DDCAPS theCaps = {};
    theCaps.dwSize = sizeof theCaps;
    CHECK(sDraw->GetCaps(&theCaps, nullptr) == DD_OK);
    CHECK(theCaps.dwCaps & DDCAPS_3D);
    CHECK(sDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL | DDSCL_MULTITHREADED) == DD_OK);

    DDSURFACEDESC2 theMode = {};
    theMode.dwSize = sizeof theMode;
    CHECK(sDraw->GetDisplayMode(&theMode) == DD_OK);
    CHECK(theMode.ddpfPixelFormat.dwRGBBitCount == 32 && theMode.dwWidth > 0 && theMode.dwHeight > 0);
    ModeList theList;
    theList.mDesktopWidth = theMode.dwWidth;
    theList.mDesktopHeight = theMode.dwHeight;
    CHECK(sDraw->EnumDisplayModes(DDEDM_REFRESHRATES, nullptr, &theList, ListMode) == DD_OK);
    CHECK(theList.mHas800x600x16 && theList.mHasDesktop && theList.mCount >= 4);

    DWORD theTotal = 0, theFree = 0;
    DDSCAPS2 theVideo = {DDSCAPS_VIDEOMEMORY, 0, 0, {0}};
    CHECK(sDraw->GetAvailableVidMem(&theVideo, &theTotal, &theFree) == DD_OK);
    // the menu lists a mode when the memory holds three of its screens (EEGameUtilities.cpp:169-185)
    CHECK(theTotal >= 4u * 1920 * 1200 * 3 && theFree <= theTotal);
    DDDEVICEIDENTIFIER2 theIdentifier = {};
    CHECK(sDraw->GetDeviceIdentifier(&theIdentifier, 0) == DD_OK);
    CHECK(theIdentifier.szDescription[0] != 0);
    printf("device: %s\n", theIdentifier.szDescription);

    CHECK(sDraw->QueryInterface(IID_IDirect3D7, (void**)&s3D) == DD_OK && s3D);
    DWORD theDevCaps = 0;
    CHECK(s3D->EnumDevices(ListDevice, &theDevCaps) == D3D_OK);
    CHECK((theDevCaps & D3DDEVCAPS_HWRASTERIZATION) && (theDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT));

    // the plug-in's 32-bit probe (Common/DX7Screen.cpp:562-600)
    IDirectDrawSurface7* theProbe = Make(2, 2, k565, DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY);
    IDirect3DDevice7* theDevice = nullptr;
    CHECK(s3D->CreateDevice(IID_IDirect3DHALDevice, theProbe, &theDevice) == D3D_OK && theDevice);
    D3DDEVICEDESC7 theDesc = {};
    CHECK(theDevice->GetCaps(&theDesc) == D3D_OK);
    CHECK(theDesc.dwDeviceRenderBitDepth & DDBD_32);
    bool theARGB = false;
    CHECK(theDevice->EnumTextureFormats(ListTextureFormat, &theARGB) == D3D_OK);
    CHECK(theARGB);
    IDirectDrawSurface7* theTexture = Make(16, 16, kARGB, DDSCAPS_TEXTURE, DDSCAPS2_TEXTUREMANAGE | DDSCAPS2_HINTSTATIC);
    CHECK(theTexture->Release() == 0);
    CHECK(theDevice->Release() == 0);
    CHECK(theProbe->Release() == 0);
}

static void TestSurfaces()
{
    // each format's memory has the exact layout of its masks, as GEPackColor writes it
    for (const DDPIXELFORMAT* theFormat : {&kARGB, &kXRGB, &k4444, &k565, &k1555}) {
        IDirectDrawSurface7* theSurface = Make(7, 3, *theFormat, DDSCAPS_TEXTURE);
        DDSURFACEDESC2 theDesc = {};
        theDesc.dwSize = sizeof theDesc;
        CHECK(theSurface->Lock(nullptr, &theDesc, 0, NULL) == DD_OK);
        CHECK(theDesc.lPitch >= 7 * (LONG)theFormat->dwRGBBitCount / 8 && theDesc.lPitch % 2 == 0);
        CHECK(theDesc.ddpfPixelFormat.dwRBitMask == theFormat->dwRBitMask &&
              theDesc.ddpfPixelFormat.dwRGBAlphaBitMask == theFormat->dwRGBAlphaBitMask);
        CHECK(theSurface->Unlock(nullptr) == DD_OK);
        CHECK(theSurface->Unlock(nullptr) == DDERR_NOTLOCKED);
        DDPIXELFORMAT theRead = {};
        theRead.dwSize = sizeof theRead;
        CHECK(theSurface->GetPixelFormat(&theRead) == DD_OK && theRead.dwGBitMask == theFormat->dwGBitMask);
        CHECK(theSurface->Release() == 0);
    }

    // colour fill and copies on the CPU, with stretching and the source colour key
    IDirectDrawSurface7* theA = Make(8, 8, k565, DDSCAPS_OFFSCREENPLAIN);
    IDirectDrawSurface7* theB = Make(16, 16, k565, DDSCAPS_OFFSCREENPLAIN);
    Fill(theB, 0x1234);
    RECT theCorner = {2, 3, 5, 7};
    Fill(theA, 0x0001);
    Fill(theA, 0xf800, &theCorner);
    {
        Locked theLock(theA);
        int theRed = 0;
        for (int y = 0; y < 8; ++y)
            for (int x = 0; x < 8; ++x)
                theRed += theLock.Get(x, y) == 0xf800;
        CHECK(theRed == 12 && theLock.Get(2, 3) == 0xf800 && theLock.Get(5, 7) == 0x0001);
        for (int x = 0; x < 8; ++x)
            theLock.Set(x, 0, (DWORD)(x * 0x0841));        // row 0 holds eight distinct values
    }
    RECT theWhole = {0, 0, 16, 16};
    CHECK(theB->Blt(&theWhole, theA, nullptr, DDBLT_WAIT, nullptr) == DD_OK);
    {
        // a 2x stretch takes each source pixel for two by two target pixels
        Locked theLock(theB);
        bool theStretched = true;
        for (int x = 0; x < 16; ++x)
            theStretched &= theLock.Get(x, 0) == (DWORD)((x / 2) * 0x0841) && theLock.Get(x, 1) == theLock.Get(x, 0);
        CHECK(theStretched);
        CHECK(theLock.Get(4, 6) == 0xf800 && theLock.Get(9, 13) == 0xf800 && theLock.Get(10, 14) == 0x0001 &&
              theLock.Get(3, 6) == 0x0001);
    }
    DDCOLORKEY theKey = {0x0001, 0x0001};
    CHECK(theA->SetColorKey(DDCKEY_SRCBLT, &theKey) == DD_OK);
    Fill(theB, 0x1234);
    RECT theTarget = {4, 4, 12, 12};
    CHECK(theB->Blt(&theTarget, theA, nullptr, DDBLT_KEYSRC | DDBLT_DONOTWAIT, nullptr) == DD_OK);
    {
        // the key's pixels leave the target as it was; the rest arrive
        Locked theLock(theB);
        CHECK(theLock.Get(4 + 3, 4 + 5) == 0xf800 && theLock.Get(4 + 7, 4 + 7) == 0x1234 && theLock.Get(4 + 1, 4) == 0x0841);
    }
    DDBLTFX theFX = {};
    theFX.dwSize = sizeof theFX;
    theFX.ddckSrcColorkey = {0xf800, 0xf800};
    Fill(theB, 0x1234);
    CHECK(theB->Blt(&theTarget, theA, nullptr, DDBLT_KEYSRCOVERRIDE, &theFX) == DD_OK);
    {
        Locked theLock(theB);
        CHECK(theLock.Get(4 + 3, 4 + 5) == 0x1234 && theLock.Get(4 + 7, 4 + 7) == 0x0001);
    }
    // with no clipper, a rectangle that leaves the target fails and draws nothing, as on Wine's ddraw
    Fill(theB, 0x1234);
    RECT theOutside = {6, 6, 20, 20};
    CHECK(theB->Blt(&theOutside, theA, &theCorner, 0, nullptr) == DDERR_INVALIDRECT);
    DDBLTFX theFill = {};
    theFill.dwSize = sizeof theFill;
    theFill.dwFillColor = 0xffff;
    CHECK(theB->Blt(&theOutside, nullptr, nullptr, DDBLT_COLORFILL, &theFill) == DDERR_INVALIDRECT);
    {
        Locked theLock(theB);
        CHECK(theLock.Get(10, 10) == 0x1234 && theLock.Get(15, 15) == 0x1234);
    }
    RECT theBad = {0, 0, 9, 9};
    CHECK(theB->Blt(nullptr, theA, &theBad, 0, nullptr) == DDERR_INVALIDRECT);
    CHECK(theA->Release() == 0);
    CHECK(theB->Release() == 0);

    // GDI on a glyph page: GEFontBitmap draws white text and keeps the green nibble as alpha
    IDirectDrawSurface7* thePage = Make(64, 32, k4444, DDSCAPS_TEXTURE);
    Fill(thePage, 0);
    HDC theDC = NULL;
    CHECK(thePage->GetDC(&theDC) == DD_OK && theDC);
    HDC theSecond = NULL;
    CHECK(thePage->GetDC(&theSecond) == DDERR_DCALREADYCREATED);
    SetBkMode(theDC, TRANSPARENT);
    SetTextColor(theDC, RGB(255, 255, 255));
    CHECK(ExtTextOutA(theDC, 2, 2, 0, nullptr, "Wg", 2, nullptr));
    CHECK(thePage->ReleaseDC(theDC) == DD_OK);
    {
        Locked theLock(thePage);
        int theInked = 0;
        for (int y = 0; y < 32; ++y)
            for (int x = 0; x < 64; ++x)
                theInked += (theLock.Get(x, y) & 0x00f0) != 0;
        printf("glyph page: %d pixels inked\n", theInked);
        CHECK(theInked > 20);
    }
    CHECK(thePage->Release() == 0);

    // a mip chain: levels halve, each found through the one before
    IDirectDrawSurface7* theMip = Make(8, 4, k565, DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
    IDirectDrawSurface7* theLevel = theMip;
    int theSizes[4][2] = {};
    int theLevels = 0;
    while (theLevel && theLevels < 4) {
        DDSURFACEDESC2 theDesc = {};
        theDesc.dwSize = sizeof theDesc;
        theLevel->GetSurfaceDesc(&theDesc);
        theSizes[theLevels][0] = (int)theDesc.dwWidth;
        theSizes[theLevels][1] = (int)theDesc.dwHeight;
        ++theLevels;
        DDSCAPS2 theCaps = {DDSCAPS_TEXTURE | DDSCAPS_MIPMAP, 0, 0, {0}};
        IDirectDrawSurface7* theNext = nullptr;
        theLevel->GetAttachedSurface(&theCaps, &theNext);
        if (theLevel != theMip)
            theLevel->Release();
        theLevel = theNext;
    }
    CHECK(theLevels == 4 && theSizes[1][0] == 4 && theSizes[1][1] == 2 && theSizes[3][0] == 1 && theSizes[3][1] == 1);
    CHECK(theMip->Release() == 0);
}

static HWND Window(const char* inName, DWORD inStyle, int inX, int inY, int inWidth, int inHeight)
{
    static bool sRegistered;
    if (!sRegistered) {
        WNDCLASSA theClass = {};
        theClass.lpfnWndProc = DefWindowProcA;
        theClass.lpszClassName = "DirectDraw Test";
        CHECK(RegisterClassA(&theClass) != 0);
        sRegistered = true;
    }
    HWND theWindow = CreateWindowExA(0, "DirectDraw Test", inName, inStyle, inX, inY, inWidth, inHeight, NULL, NULL,
                                     NULL, NULL);
    CHECK(theWindow != NULL);
    return theWindow;
}

static void TestFlip()
{
    // a flipping primary with two back buffers: each flip shows the first back buffer and moves the images round
    IDirectDraw7* theDraw = nullptr;
    CHECK(DirectDrawCreateEx(nullptr, (void**)&theDraw, IID_IDirectDraw7, nullptr) == DD_OK);
    HWND theWindow = Window("flip", WS_POPUP | WS_VISIBLE, 0, 0, 64, 48);
    CHECK(theDraw->SetCooperativeLevel(theWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_MULTITHREADED) == DD_OK);
    CHECK(theDraw->SetDisplayMode(640, 480, 16, 0, 0) == DD_OK);
    CHECK(theDraw->SetDisplayMode(641, 480, 16, 0, 0) == DDERR_INVALIDMODE);
    DDSURFACEDESC2 theDesc = {};
    theDesc.dwSize = sizeof theDesc;
    theDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    theDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    theDesc.dwBackBufferCount = 2;
    IDirectDrawSurface7* thePrimary = nullptr;
    CHECK(theDraw->CreateSurface(&theDesc, &thePrimary, nullptr) == DD_OK && thePrimary);
    DDSCAPS2 theFlip = {DDSCAPS_FLIP, 0, 0, {0}};
    IDirectDrawSurface7 *theBack1 = nullptr, *theBack2 = nullptr;
    CHECK(thePrimary->GetAttachedSurface(&theFlip, &theBack1) == DD_OK && theBack1);
    CHECK(theBack1->GetAttachedSurface(&theFlip, &theBack2) == DD_OK && theBack2 && theBack2 != thePrimary);
    DDSURFACEDESC2 theRead = {};
    theRead.dwSize = sizeof theRead;
    thePrimary->GetSurfaceDesc(&theRead);
    CHECK(theRead.dwWidth == 640 && theRead.dwHeight == 480 && theRead.ddpfPixelFormat.dwRGBBitCount == 16);
    Fill(thePrimary, 0x1111);
    Fill(theBack1, 0x2222);
    Fill(theBack2, 0x3333);
    CHECK(thePrimary->Flip(nullptr, DDFLIP_DONOTWAIT | DDFLIP_NOVSYNC) == DD_OK);
    {
        Locked a(thePrimary), b(theBack1), c(theBack2);
        CHECK(a.Get(5, 5) == 0x2222 && b.Get(5, 5) == 0x3333 && c.Get(5, 5) == 0x1111);
    }
    // a render into the back buffer shows after the next flip, and the cursor's blit to the primary shows at once
    IDirect3D7* the3D = nullptr;
    CHECK(theDraw->QueryInterface(IID_IDirect3D7, (void**)&the3D) == DD_OK);
    IDirect3DDevice7* theDevice = nullptr;
    CHECK(the3D->CreateDevice(IID_IDirect3DTnLHalDevice, theBack1, &theDevice) == D3D_OK);
    CHECK(theDevice->BeginScene() == D3D_OK);
    CHECK(theDevice->Clear(0, nullptr, D3DCLEAR_TARGET, 0xff00ff00, 1, 0) == D3D_OK);
    CHECK(theDevice->EndScene() == D3D_OK);
    CHECK(thePrimary->Flip(nullptr, DDFLIP_DONOTWAIT) == DD_OK);
    {
        Locked a(thePrimary);
        CHECK(a.Get(100, 100) == 0x07e0);
    }
    IDirectDrawSurface7* theCursor = Make(4, 4, k565, DDSCAPS_OFFSCREENPLAIN);
    Fill(theCursor, 0x001f);
    RECT theSpot = {10, 10, 14, 14};
    CHECK(thePrimary->Blt(&theSpot, theCursor, nullptr, DDBLT_DONOTWAIT, nullptr) == DD_OK);
    {
        Locked a(thePrimary);
        CHECK(a.Get(10, 10) == 0x001f && a.Get(14, 14) == 0x07e0);
    }
    CHECK(theDevice->Release() == 0);
    the3D->Release();
    CHECK(theCursor->Release() == 0);
    // the primary frees its flip chain, though the game never releases the back buffers it was given
    CHECK(thePrimary->Release() == 0);
    CHECK(theDraw->RestoreDisplayMode() == DD_OK);
    CHECK(theDraw->Release() == 0);
    DestroyWindow(theWindow);
}

static void TestDraws()
{
    Target t(64, 64, kXRGB, true);
    t.Clear(0xff102030);
    CHECK(t.At(0, 0) == 0x102030 && t.At(63, 63) == 0x102030);

    // Clear stays inside the viewport
    D3DVIEWPORT7 theViewport = {10, 12, 20, 16, 0, 1};
    CHECK(t.mDevice->SetViewport(&theViewport) == D3D_OK);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    D3DRECT theAll = {0, 0, 64, 64};
    CHECK(t.mDevice->Clear(1, &theAll, D3DCLEAR_TARGET, 0xffff0000, 1, 0) == D3D_OK);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    int theInside, theOutside;
    Coverage(t, 0xff0000, {10, 12, 30, 28}, &theInside, &theOutside);
    CHECK(theInside == 20 * 16 && theOutside == 0);
    D3DVIEWPORT7 theFull = {0, 0, 64, 64, 0, 1};
    CHECK(t.mDevice->SetViewport(&theFull) == D3D_OK);

    // coverage: a quad from 8 to 24 covers the pixels whose centres, at whole coordinates, lie inside: 8 to 23;
    // one from 8.5 to 24.5 covers 9 to 24
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(8, 8, 24, 24, 0.5f, 0xffffffff);
    t.Quad(32.5f, 8.5f, 48.5f, 24.5f, 0.5f, 0xff00ff00);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    Coverage(t, 0xffffff, {8, 8, 24, 24}, &theInside, &theOutside);
    CHECK(theInside == 256 && theOutside == 0);
    Coverage(t, 0x00ff00, {33, 9, 49, 25}, &theInside, &theOutside);
    printf("half-pixel quad: %d inside, %d outside\n", theInside, theOutside);
    CHECK(theInside == 256 && theOutside == 0);

    // culling: the quad's first triangle (0,1,2) turns clockwise on the screen
    for (DWORD theCull : {D3DCULL_NONE, D3DCULL_CW, D3DCULL_CCW}) {
        t.Clear(0xff000000);
        CHECK(t.mDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, theCull) == D3D_OK);
        CHECK(t.mDevice->BeginScene() == D3D_OK);
        t.Draw(D3DPT_TRIANGLELIST, {V(8, 8, 0.5f, 0xffffffff), V(40, 8, 0.5f, 0xffffffff), V(8, 40, 0.5f, 0xffffffff)});
        t.Draw(D3DPT_TRIANGLELIST, {V(48, 48, 0.5f, 0xffff0000), V(48, 60, 0.5f, 0xffff0000), V(60, 48, 0.5f, 0xffff0000)});
        CHECK(t.mDevice->EndScene() == D3D_OK);
        bool theClockwise = t.At(12, 12) == 0xffffff, theCounter = t.At(50, 50) == 0xff0000;
        CHECK(theClockwise == (theCull != D3DCULL_CW));
        CHECK(theCounter == (theCull != D3DCULL_CCW));
    }
    t.mDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

    // depth: z runs from 0 to 1; a nearer quad drawn first keeps its pixels; z below 0 is clipped
    t.Clear(0xff000000);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 32, 32, 0.25f, 0xffff0000);
    t.Quad(16, 16, 48, 48, 0.75f, 0xff00ff00);
    t.Quad(40, 0, 64, 16, -0.1f, 0xff0000ff);
    t.Quad(0, 48, 16, 64, 0.999f, 0xffffff00);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(t.At(20, 20) == 0xff0000 && t.At(40, 40) == 0x00ff00 && t.At(50, 8) == 0 && t.At(8, 56) == 0xffff00);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);

    // alpha test GREATER 0x80 and blending SRCALPHA, INVSRCALPHA
    t.Clear(0xff0000ff);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0x80);
    t.mDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    t.mDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 32, 32, 0.5f, 0x80ff0000);
    t.Quad(32, 0, 64, 32, 0xc0 / 255.0f * 0 + 0.5f, 0xc0ff0000);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    double theAlpha = 0xc0 / 255.0;
    CHECK(t.At(10, 10) == 0x0000ff);
    CHECK(Near(t.At(40, 10), Rgb(theAlpha, 0, 1 - theAlpha), 1));
    printf("blend: %06lx against %06lx\n", (unsigned long)t.At(40, 10), (unsigned long)Rgb(theAlpha, 0, 1 - theAlpha));
    // BOTHSRCALPHA ignores the destination blend
    t.mDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_BOTHSRCALPHA);
    t.mDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
    t.Clear(0xff0000ff);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 32, 32, 0.5f, 0x40ff0000);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(Near(t.At(10, 10), Rgb(0x40 / 255.0, 0, 1 - 0x40 / 255.0), 1));
    t.mDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

    // flat shading takes the first vertex's colour
    t.Clear(0xff000000);
    t.mDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Draw(D3DPT_TRIANGLELIST, {V(0, 0, 0.5f, 0xff123456), V(64, 0, 0.5f, 0xffffffff), V(0, 64, 0.5f, 0xff000000)});
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(t.At(20, 20) == 0x123456 && t.At(2, 50) == 0x123456);
    t.mDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);

    // an indexed draw at a later buffer position: indices count from the start vertex
    t.Clear(0xff000000);
    t.mPosition = 1000;
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(4, 4, 12, 12, 0.5f, 0xff00ffff);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    Coverage(t, 0x00ffff, {4, 4, 12, 12}, &theInside, &theOutside);
    CHECK(theInside == 64 && theOutside == 0);

    // lines: a horizontal line list from (4.001, 30.001) to (60.001, 30.001), as Draw2D moves them
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Draw(D3DPT_LINELIST, {V(4.001f, 30.001f, 0.5f, 0xffffffff), V(60.001f, 30.001f, 0.5f, 0xffffffff)});
    t.Draw(D3DPT_POINTLIST, {V(10.001f, 50.001f, 0.5f, 0xffff0000)});
    CHECK(t.mDevice->EndScene() == D3D_OK);
    Coverage(t, 0xffffff, {4, 30, 60, 31}, &theInside, &theOutside);
    printf("line: %d of 56 pixels on row 30, %d elsewhere\n", theInside, theOutside);
    CHECK(theInside >= 55 && theOutside == 0);
    CHECK(t.At(10, 50) == 0xff0000);
}

static void TestTextures()
{
    Target t(32, 32, kXRGB, false);
    // a 4x4 texture with a distinct colour per texel; row 0 is the top of the image
    IDirectDrawSurface7* theTexture = Make(4, 4, kARGB, DDSCAPS_TEXTURE, DDSCAPS2_TEXTUREMANAGE);
    {
        Locked theLock(theTexture);
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                theLock.Set(x, y, 0xff000000 | (DWORD)(x * 60) << 16 | (DWORD)(y * 60) << 8 | 0x10);
    }
    t.mDevice->SetTexture(0, theTexture);
    t.mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    t.mDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    t.mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    t.mDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    t.mDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
    t.mDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
    t.mDevice->SetTextureStageState(0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 16, 16, 0.5f, 0xffffffff);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    bool theMapped = true;
    for (int y = 0; y < 16; ++y)
        for (int x = 0; x < 16; ++x)
            theMapped &= t.At(x, y) == ((DWORD)(x / 4 * 60) << 16 | (DWORD)(y / 4 * 60) << 8 | 0x10);
    CHECK(theMapped);

    // MODULATE: texture times diffuse; DISABLE: the diffuse colour alone
    t.mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    t.mDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 16, 16, 0.5f, 0xff808080);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(Near(t.At(13, 13), Rgb(180 / 255.0 * 128 / 255, 180 / 255.0 * 128 / 255, 16 / 255.0 * 128 / 255), 1));
    t.mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    t.mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 16, 16, 0.5f, 0xff336699);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(t.At(13, 13) == 0x336699);

    // a 4444 texture changed through Lock after a draw that sampled it draws the old image in that draw only
    t.mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    t.mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 8, 8, 0.5f, 0xffffffff);
    {
        Locked theLock(theTexture);
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                theLock.Set(x, y, 0xff00ff00);
    }
    t.Quad(16, 16, 24, 24, 0.5f, 0xffffffff);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(t.At(1, 1) == 0x000010 && t.At(20, 20) == 0x00ff00);
    t.mDevice->SetTexture(0, nullptr);
    CHECK(theTexture->Release() == 0);

    // colour keying: a key-0 R5G6B5 texture drops its black texels while COLORKEYENABLE is on
    IDirectDrawSurface7* theKeyed = Make(2, 1, k565, DDSCAPS_TEXTURE, 0, DDSD_CKSRCBLT);
    {
        Locked theLock(theKeyed);
        theLock.Set(0, 0, 0x0000);
        theLock.Set(1, 0, 0xf800);
    }
    t.mDevice->SetTexture(0, theKeyed);
    for (BOOL theOn : {TRUE, FALSE}) {
        t.mDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, theOn);
        t.Clear(0xff0000ff);
        CHECK(t.mDevice->BeginScene() == D3D_OK);
        t.Quad(0, 0, 16, 8, 0.5f, 0xffffffff);
        CHECK(t.mDevice->EndScene() == D3D_OK);
        CHECK(t.At(3, 3) == (theOn ? 0x0000ffu : 0x000000u) && t.At(12, 3) == 0xff0000);
    }
    t.mDevice->SetTexture(0, nullptr);
    CHECK(theKeyed->Release() == 0);

    // mipmaps: a quad a quarter of the texture's size samples level 2 with a mip filter, level 0 without
    IDirectDrawSurface7* theMip = Make(8, 8, kXRGB, DDSCAPS_TEXTURE | DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
    IDirectDrawSurface7* theLevel = theMip;
    theLevel->AddRef();
    const DWORD kLevelColours[4] = {0xff0000, 0x00ff00, 0x0000ff, 0xffffff};
    for (int i = 0; theLevel; ++i) {
        DDSURFACEDESC2 theDesc = {};
        theDesc.dwSize = sizeof theDesc;
        theLevel->GetSurfaceDesc(&theDesc);
        Fill(theLevel, kLevelColours[i]);
        DDSCAPS2 theCaps = {DDSCAPS_TEXTURE | DDSCAPS_MIPMAP, 0, 0, {0}};
        IDirectDrawSurface7* theNext = nullptr;
        theLevel->GetAttachedSurface(&theCaps, &theNext);
        theLevel->Release();
        theLevel = theNext;
    }
    t.mDevice->SetTexture(0, theMip);
    for (DWORD theFilter : {D3DTFP_POINT, D3DTFP_NONE}) {
        t.mDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, theFilter);
        t.Clear(0xff000000);
        CHECK(t.mDevice->BeginScene() == D3D_OK);
        t.Quad(0, 0, 2, 2, 0.5f, 0xffffffff);
        CHECK(t.mDevice->EndScene() == D3D_OK);
        CHECK(t.At(0, 0) == (theFilter == D3DTFP_POINT ? 0x0000ffu : 0xff0000u));
    }
    t.mDevice->SetTexture(0, nullptr);
    CHECK(theMip->Release() == 0);
}

static void TestLighting()
{
    Target t(16, 16, kXRGB, false);
    // one material, one directional light pointing along +z, normals facing -z: N.L is 1
    D3DMATERIAL7 theMaterial = {};
    theMaterial.diffuse = {0.8f, 0.4f, 0.2f, 1.0f};
    theMaterial.ambient = {1.0f, 1.0f, 1.0f, 1.0f};
    CHECK(t.mDevice->SetMaterial(&theMaterial) == D3D_OK);
    D3DLIGHT7 theLight = {};
    theLight.dltType = D3DLIGHT_DIRECTIONAL;
    theLight.dcvDiffuse = {1, 1, 1, 0};
    theLight.dvDirection = {0, 0, 1};
    CHECK(t.mDevice->SetLight(0, &theLight) == D3D_OK);
    CHECK(t.mDevice->LightEnable(0, TRUE) == D3D_OK);
    D3DLIGHT7 theRead = {};
    CHECK(t.mDevice->GetLight(0, &theRead) == D3D_OK && theRead.dvDirection.z == 1);
    t.mDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, FALSE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    t.mDevice->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    t.mDevice->SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0x00333333);      // 0.2
    auto theLit = [&](float inNx, float inNy, float inNz) {
        t.Clear(0xff000000);
        CHECK(t.mDevice->BeginScene() == D3D_OK);
        std::vector<Vertex> theQuad = {V(0, 0, 0.5f, 0), V(16, 0, 0.5f, 0), V(0, 16, 0.5f, 0), V(16, 16, 0.5f, 0)};
        for (Vertex& theVertex : theQuad) {
            theVertex.nx = inNx;
            theVertex.ny = inNy;
            theVertex.nz = inNz;
        }
        t.Draw(D3DPT_TRIANGLELIST, theQuad, {0, 1, 2, 2, 1, 3});
        CHECK(t.mDevice->EndScene() == D3D_OK);
        return t.At(8, 8);
    };
    double theAmbient = 0x33 / 255.0;
    CHECK(Near(theLit(0, 0, -1), Rgb(0.8 + theAmbient, 0.4 + theAmbient, 0.2 + theAmbient), 1));
    // a normal at 60 degrees to the light, not of unit length: NORMALIZENORMALS makes N.L one half
    DWORD theHalf = theLit(0, 2 * std::sqrt(3.0f), -2);
    printf("lighting at 60 degrees: %06lx against %06lx\n", (unsigned long)theHalf,
           (unsigned long)Rgb(0.4 + theAmbient, 0.2 + theAmbient, 0.1 + theAmbient));
    CHECK(Near(theHalf, Rgb(0.4 + theAmbient, 0.2 + theAmbient, 0.1 + theAmbient), 1));
    // facing away: ambient alone
    CHECK(Near(theLit(0, 0, 1), Rgb(theAmbient, theAmbient, theAmbient), 1));
    // the vertex colour as the diffuse source (COLORVERTEX, D3DMCS_COLOR1)
    t.mDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    t.mDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0);
    {
        t.Clear(0xff000000);
        CHECK(t.mDevice->BeginScene() == D3D_OK);
        t.Quad(0, 0, 16, 16, 0.5f, 0xff4080c0);
        CHECK(t.mDevice->EndScene() == D3D_OK);
        CHECK(Near(t.At(8, 8), 0x4080c0, 1));
    }
    // a point light two units in front of pixel (8, 8), attenuation 1/d^2. Direct3D 7 lights each vertex: the corners
    // of a 2 by 2 quad round the pixel lie sqrt(1 + 1 + 4) from the light, at N.L = 2 / sqrt(6), so each corner and
    // every pixel between them gets 2 / sqrt(6) / 6 of the material's diffuse colour
    t.mDevice->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    t.mDevice->SetRenderState(D3DRENDERSTATE_COLORVERTEX, FALSE);
    D3DLIGHT7 thePoint = {};
    thePoint.dltType = D3DLIGHT_POINT;
    thePoint.dcvDiffuse = {1, 1, 1, 0};
    thePoint.dvRange = 100;
    thePoint.dvAttenuation2 = 1;
    // world and view are identity, so the camera-space position of a vertex is its pixel position
    thePoint.dvPosition = {8, 8, 0.5f - 2};
    CHECK(t.mDevice->SetLight(0, &thePoint) == D3D_OK);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(7, 7, 9, 9, 0.5f, 0);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    double thePart = 2 / std::sqrt(6.0) / 6;
    DWORD thePointLit = t.At(8, 8), thePointWanted = Rgb(0.8 * thePart, 0.4 * thePart, 0.2 * thePart);
    printf("point light: %06lx against %06lx\n", (unsigned long)thePointLit, (unsigned long)thePointWanted);
    CHECK(Near(thePointLit, thePointWanted, 1));
    t.mDevice->LightEnable(0, FALSE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);

    // linear vertex fog on camera-space z: start 0, end 2, z 0.5 gives a factor of 0.75
    t.mDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, TRUE);
    t.mDevice->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_LINEAR);
    t.mDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, 0x00ffffff);
    float theStart = 0, theEnd = 2;
    DWORD theBits;
    memcpy(&theBits, &theStart, 4);
    t.mDevice->SetRenderState(D3DRENDERSTATE_FOGSTART, theBits);
    memcpy(&theBits, &theEnd, 4);
    t.mDevice->SetRenderState(D3DRENDERSTATE_FOGEND, theBits);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 16, 16, 0.5f, 0xff000000);
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(Near(t.At(8, 8), Rgb(0.25, 0.25, 0.25), 1));
    t.mDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
}

static void TestCoherence()
{
    // GDI text drawn into the render target while Direct3D draws to it in the same scene, as the graphics thread
    // holds the back buffer's DC from TextLock until after EndScene (RTSGraphics.cpp). Each reaches the image in the
    // order of the calls: a draw after the text covers it, and GDI after a draw covers the draw, as on Wine's ddraw
    Target t(64, 32, kXRGB, false);
    t.Clear(0xff000000);
    CHECK(t.mDevice->BeginScene() == D3D_OK);
    t.Quad(0, 0, 8, 8, 0.5f, 0xff0000ff);
    HDC theDC = NULL;
    CHECK(t.mSurface->GetDC(&theDC) == DD_OK);
    SetBkMode(theDC, OPAQUE);
    SetBkColor(theDC, RGB(255, 255, 0));
    SetTextColor(theDC, RGB(255, 0, 0));
    RECT theBox = {20, 4, 40, 20};
    CHECK(ExtTextOutA(theDC, 20, 4, ETO_OPAQUE | ETO_CLIPPED, &theBox, "Hi", 2, nullptr));
    t.Quad(0, 16, 64, 32, 0.5f, 0xff00ff00);
    t.Quad(30, 0, 34, 32, 0.5f, 0xffffffff);
    SetBkColor(theDC, RGB(255, 0, 255));
    RECT theLater = {28, 22, 36, 30};
    CHECK(ExtTextOutA(theDC, 28, 22, ETO_OPAQUE, &theLater, "", 0, nullptr));
    CHECK(t.mDevice->EndScene() == D3D_OK);
    CHECK(t.mSurface->ReleaseDC(theDC) == DD_OK);
    int theYellow = 0, theWhiteInBox = 0;
    {
        Locked theLock(t.mSurface, DDLOCK_READONLY);
        for (int y = 4; y < 16; ++y)
            for (int x = 20; x < 40; ++x) {
                DWORD theColour = theLock.Get(x, y) & 0xffffff;
                theYellow += theColour == 0xffff00;
                theWhiteInBox += theColour == 0xffffff;
            }
        CHECK((theLock.Get(2, 2) & 0xffffff) == 0x0000ff && (theLock.Get(10, 24) & 0xffffff) == 0x00ff00);
        CHECK((theLock.Get(32, 2) & 0xffffff) == 0xffffff && (theLock.Get(38, 6) & 0xffffff) == 0xffff00);
        CHECK((theLock.Get(38, 18) & 0xffffff) == 0x00ff00);
        CHECK((theLock.Get(29, 25) & 0xffffff) == 0xff00ff && (theLock.Get(31, 25) & 0xffffff) == 0xff00ff);
        CHECK((theLock.Get(31, 31) & 0xffffff) == 0xffffff && (theLock.Get(27, 25) & 0xffffff) == 0x00ff00);
    }
    printf("text and draws: %d background pixels in the box, %d white\n", theYellow, theWhiteInBox);
    CHECK(theYellow > 100 && theWhiteInBox == 4 * 12);

    // GDI over what Direct3D drew after GetDC blends with that image: a red glyph's edges over a white panel keep
    // full red, and an opaque black box over the panel stays black, though the memory held black when GDI got the DC
    {
        Target u(64, 32, kXRGB, false);
        u.Clear(0xff000000);
        CHECK(u.mDevice->BeginScene() == D3D_OK);
        HDC theTextDC = NULL;
        CHECK(u.mSurface->GetDC(&theTextDC) == DD_OK);
        u.Quad(0, 0, 64, 32, 0.5f, 0xffffffff);
        SetBkMode(theTextDC, TRANSPARENT);
        SetTextColor(theTextDC, RGB(255, 0, 0));
        CHECK(ExtTextOutA(theTextDC, 2, 2, 0, nullptr, "Wg", 2, nullptr));
        SetBkColor(theTextDC, RGB(0, 0, 0));
        RECT theBlack = {40, 4, 60, 28};
        CHECK(ExtTextOutA(theTextDC, 40, 4, ETO_OPAQUE, &theBlack, "", 0, nullptr));
        CHECK(u.mDevice->EndScene() == D3D_OK);
        CHECK(u.mSurface->ReleaseDC(theTextDC) == DD_OK);
        Locked theLock(u.mSurface, DDLOCK_READONLY);
        int theInked = 0, theDark = 0, theBlackPixels = 0;
        for (int y = 0; y < 32; ++y)
            for (int x = 0; x < 64; ++x) {
                DWORD c = theLock.Get(x, y) & 0xffffff;
                if (x < 36) {
                    theInked += (c & 0xff00) < 0xfa00;
                    theDark += (c >> 16) < 0xfa;
                } else
                    theBlackPixels += c == 0;
            }
        printf("text over a panel: %d pixels inked, %d with red below 250, %d black of %d\n", theInked, theDark,
               theBlackPixels, 20 * 24);
        CHECK(theInked > 20 && theDark == 0 && theBlackPixels == 20 * 24);
    }

    // the cursor's keyed blit onto a back buffer that Direct3D drew, then the save-under read back from it
    IDirectDrawSurface7* thePointer = Make(4, 4, kXRGB, DDSCAPS_OFFSCREENPLAIN, 0, DDSD_CKSRCBLT);
    DDCOLORKEY theBlue = {0x0000ff, 0x0000ff};
    CHECK(thePointer->SetColorKey(DDCKEY_SRCBLT, &theBlue) == DD_OK);
    Fill(thePointer, 0x0000ff);
    RECT theDot = {1, 1, 3, 3};
    Fill(thePointer, 0xff00ff, &theDot);
    IDirectDrawSurface7* theUnder = Make(4, 4, kXRGB, DDSCAPS_OFFSCREENPLAIN);
    t.Clear(0xff202020);
    RECT theSpot = {40, 20, 44, 24};
    CHECK(theUnder->Blt(nullptr, t.mSurface, &theSpot, DDBLT_DONOTWAIT, nullptr) == DD_OK);
    CHECK(t.mSurface->Blt(&theSpot, thePointer, nullptr, DDBLT_DONOTWAIT | DDBLT_KEYSRC, nullptr) == DD_OK);
    CHECK(t.At(40, 20) == 0x202020 && t.At(41, 21) == 0xff00ff);
    {
        Locked theLock(theUnder, DDLOCK_READONLY);
        CHECK((theLock.Get(1, 1) & 0xffffff) == 0x202020);
    }
    CHECK(t.mSurface->Blt(&theSpot, theUnder, nullptr, DDBLT_DONOTWAIT, nullptr) == DD_OK);
    CHECK(t.At(41, 21) == 0x202020);
    CHECK(thePointer->Release() == 0);
    CHECK(theUnder->Release() == 0);

    // the same keyed blit of a surface whose unused high byte is set, as the game's pointer textures have it: the key
    // names the format's own bits, so the blue background still drops
    IDirectDrawSurface7* theFilledPointer = Make(4, 4, kXRGB, DDSCAPS_OFFSCREENPLAIN, 0, DDSD_CKSRCBLT);
    CHECK(theFilledPointer->SetColorKey(DDCKEY_SRCBLT, &theBlue) == DD_OK);
    Fill(theFilledPointer, 0xff0000ff);
    Fill(theFilledPointer, 0xffff00ff, &theDot);
    t.Clear(0xff202020);
    CHECK(t.mSurface->Blt(&theSpot, theFilledPointer, nullptr, DDBLT_DONOTWAIT | DDBLT_KEYSRC, nullptr) == DD_OK);
    CHECK(t.At(40, 20) == 0x202020 && t.At(41, 21) == 0xff00ff);
    CHECK(t.At(43, 23) == 0x202020);
    CHECK(theFilledPointer->Release() == 0);

    // 16-bit render target: every R5G6B5 value survives the trip into OpenGL and back
    Target theSixteen(256, 256, k565, false);
    {
        Locked theLock(theSixteen.mSurface);
        for (int y = 0; y < 256; ++y)
            for (int x = 0; x < 256; ++x)
                theLock.Set(x, y, (DWORD)(y * 256 + x));
    }
    CHECK(theSixteen.mDevice->BeginScene() == D3D_OK);
    D3DRECT theCorner = {0, 0, 1, 1};
    CHECK(theSixteen.mDevice->Clear(1, &theCorner, D3DCLEAR_TARGET, 0xff000000, 1, 0) == D3D_OK);
    CHECK(theSixteen.mDevice->EndScene() == D3D_OK);
    {
        Locked theLock(theSixteen.mSurface, DDLOCK_READONLY);
        int theWrong = 0;
        for (int y = 0; y < 256; ++y)
            for (int x = 0; x < 256; ++x)
                theWrong += theLock.Get(x, y) != (x == 0 && y == 0 ? 0u : (DWORD)(y * 256 + x));
        CHECK(theWrong == 0);
        printf("R5G6B5 through OpenGL: %d of 65536 values changed\n", theWrong);
    }
}

static void TestThreads()
{
    // the graphics thread draws while a loading thread fills textures and a cursor thread blits; none of them owns
    // the OpenGL context
    Target t(64, 64, kXRGB, true);
    IDirectDrawSurface7* theTexture = Make(8, 8, kARGB, DDSCAPS_TEXTURE);
    Fill(theTexture, 0xff00ff00);
    IDirectDrawSurface7* theUnder = Make(4, 4, kXRGB, DDSCAPS_OFFSCREENPLAIN);
    std::atomic<bool> theStop{false};
    std::atomic<int> theLoads{0}, theBlits{0};
    std::thread theLoader([&] {
        while (!theStop) {
            IDirectDrawSurface7* theFresh = Make(16, 16, k4444, DDSCAPS_TEXTURE);
            {
                Locked theLock(theFresh);
                theLock.Set(3, 3, 0xffff);
            }
            theFresh->Release();
            ++theLoads;
        }
    });
    std::thread theCursor([&] {
        while (!theStop) {
            RECT theSpot = {60, 60, 64, 64};
            CHECK(theUnder->Blt(nullptr, t.mSurface, &theSpot, DDBLT_DONOTWAIT, nullptr) == DD_OK);
            ++theBlits;
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    });
    int theFrames = 0;
    std::thread theGraphics([&] {
        t.mDevice->SetTexture(0, theTexture);
        t.mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        t.mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        // at least 60 frames, and as many more as it takes the other threads to run five times each
        for (; theFrames < 60 || ((theLoads < 5 || theBlits < 5) && theFrames < 100000); ++theFrames) {
            CHECK(t.mDevice->BeginScene() == D3D_OK);
            CHECK(t.mDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1, 0) == D3D_OK);
            t.Quad(0, 0, 32, 32, 0.5f, 0xffffffff);
            CHECK(t.mDevice->EndScene() == D3D_OK);
        }
        t.mDevice->SetTexture(0, nullptr);
    });
    theGraphics.join();
    theStop = true;
    theLoader.join();
    theCursor.join();
    printf("threads: %d frames, %d texture loads, %d cursor blits\n", theFrames, theLoads.load(), theBlits.load());
    CHECK(theFrames >= 60 && theLoads >= 5 && theBlits >= 5);
    CHECK(t.At(10, 10) == 0x00ff00 && t.At(40, 40) == 0);
    CHECK(theTexture->Release() == 0);
    CHECK(theUnder->Release() == 0);
}

static void TestWindowed()
{
    // a windowed game: the primary is the window's client area, addressed in screen coordinates
    HWND theWindow = Window("windowed", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 50, 80, 60);
    CHECK(sDraw->SetCooperativeLevel(theWindow, DDSCL_NORMAL | DDSCL_MULTITHREADED) == DD_OK);
    DDSURFACEDESC2 theDesc = {};
    theDesc.dwSize = sizeof theDesc;
    theDesc.dwFlags = DDSD_CAPS;
    theDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    IDirectDrawSurface7* thePrimary = nullptr;
    CHECK(sDraw->CreateSurface(&theDesc, &thePrimary, nullptr) == DD_OK && thePrimary);
    IDirectDrawSurface7* theSecond = nullptr;
    CHECK(sDraw->CreateSurface(&theDesc, &theSecond, nullptr) == DDERR_PRIMARYSURFACEALREADYEXISTS);
    RECT theClient;
    GetClientRect(theWindow, &theClient);
    Target theBack(theClient.right, theClient.bottom, kXRGB, false);
    theBack.Clear(0xff445566);
    POINT theOrigin = {0, 0};
    ClientToScreen(theWindow, &theOrigin);
    RECT theScreen = {theOrigin.x, theOrigin.y, theOrigin.x + theClient.right, theOrigin.y + theClient.bottom};
    CHECK(thePrimary->Blt(&theScreen, theBack.mSurface, nullptr, 0, nullptr) == DD_OK);
    {
        Locked theLock(thePrimary, DDLOCK_READONLY);
        CHECK(theLock.mDesc.dwWidth == (DWORD)theClient.right);
        CHECK((theLock.Get(0, 0) & 0xffffff) == 0x445566 && (theLock.Get(theClient.right - 1, theClient.bottom - 1) &
                                                             0xffffff) == 0x445566);
    }
    // the cursor's save-under reads the primary in screen coordinates too (RTSMouse::SaveUnder)
    IDirectDrawSurface7* theSave = Make(4, 4, kXRGB, DDSCAPS_OFFSCREENPLAIN);
    Fill(theSave, 0);
    RECT theUnder = {theOrigin.x + 2, theOrigin.y + 3, theOrigin.x + 6, theOrigin.y + 7};
    CHECK(theSave->Blt(nullptr, thePrimary, &theUnder, DDBLT_DONOTWAIT, nullptr) == DD_OK);
    {
        Locked theLock(theSave, DDLOCK_READONLY);
        CHECK((theLock.Get(0, 0) & 0xffffff) == 0x445566 && (theLock.Get(3, 3) & 0xffffff) == 0x445566);
    }
    CHECK(theSave->Release() == 0);
    CHECK(thePrimary->Release() == 0);
    CHECK(sDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL) == DD_OK);
    DestroyWindow(theWindow);
}

int main()
{
    // SDL's offscreen driver makes its OpenGL context through EGL; Mesa's software renderer draws the same pixels on
    // every machine
    setenv("SDL_VIDEODRIVER", "offscreen", 1);
    setenv("__EGL_VENDOR_LIBRARY_FILENAMES", "/usr/share/glvnd/egl_vendor.d/50_mesa.json", 1);
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
    setvbuf(stdout, nullptr, _IOLBF, 0);
    TestSetup();
    TestSurfaces();
    TestDraws();
    TestTextures();
    TestLighting();
    TestCoherence();
    TestThreads();
    TestFlip();
    TestWindowed();
    s3D->Release();
    CHECK(sDraw->Release() == 0);
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
