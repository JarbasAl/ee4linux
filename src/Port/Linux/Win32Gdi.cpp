// The native Linux build's GDI: software drawing into pixel buffers.
//
// A bitmap is a pixel buffer: 32-bit for the bitmaps GDI makes (compatible bitmaps, DIB conversions, resources),
// or the caller's own layout for a DIB section and for a DirectDraw surface's DC (CreateBufferDC). A window DC draws
// into a 32-bit buffer the layer keeps for the window; releasing a DC that drew shows the buffer in the window's
// native SDL window, at once on the video thread or at its next message pump otherwise.
//
// Text is TrueType through stb_truetype, measured as GDI measures it (GdiTrueType.cpp), from the installed faces
// (GdiFontDB.cpp). One lock serialises all GDI calls, as the glyph cache and the font scan are shared.
#include "Win32Internal.h"

#include "GdiTrueType.h"

#include <SDL.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>

namespace {

std::recursive_mutex gGdiLock;

struct GdiObject {
    enum Kind { kPen, kBrush, kFont, kBitmap };
    explicit GdiObject(Kind inKind) : mKind(inKind) {}
    virtual ~GdiObject() {}
    const Kind mKind;
    bool mStock = false;
    int mSelections = 0;            // the DCs that have it selected
    bool mDeletePending = false;    // deleted while selected: freed when the last DC lets it go
};

struct Pen : GdiObject {
    Pen() : GdiObject(kPen) {}
    LOGPEN mLog = {};
};

struct Brush : GdiObject {
    Brush() : GdiObject(kBrush) {}
    LOGBRUSH mLog = {};
};

struct Font : GdiObject {
    Font() : GdiObject(kFont) {}
    LOGFONTA mLog = {};
    bool mRealized = false;
    eeport::gdi::GdiFont mFont;
};

// Pixels: row y starts at mRows + y * mPitch, so a bottom-up DIB has a negative pitch
struct Bitmap : GdiObject {
    Bitmap() : GdiObject(kBitmap) {}
    ~Bitmap() override
    {
        if (mOwned)
            free(mMemory);
    }
    int mWidth = 0, mHeight = 0, mBitCount = 32;
    int mPitch = 0;
    unsigned char* mRows = nullptr;
    unsigned char* mMemory = nullptr;   // the allocation, or the DIB section's bits as the caller sees them
    bool mOwned = true;
    bool mSection = false;
    bool mTopDown = true;
    DWORD mMasks[3] = {0xff0000, 0x00ff00, 0x0000ff};
    std::vector<RGBQUAD> mPalette;      // 8-bit sections
};

struct DC {
    enum Kind { kMemory, kWindow, kScreen, kBuffer };
    Kind mKind = kMemory;
    HWND mWindow = NULL;
    Bitmap* mBitmap = nullptr;
    Pen* mPen = nullptr;
    Brush* mBrush = nullptr;
    Font* mFont = nullptr;
    COLORREF mTextColor = RGB(0, 0, 0);
    COLORREF mBkColor = RGB(255, 255, 255);
    int mBkMode = OPAQUE;
    bool mClipped = false;
    RECT mClip = {};
    bool mDrawn = false;
};

std::set<GdiObject*> gObjects;
std::set<DC*> gDCs;
std::map<HWND, Bitmap*> gWindowPixels;
std::set<HWND> gToPresent;
std::map<int, GdiObject*> gStock;

DC* DCOf(HDC inDC)
{
    return gDCs.count((DC*)inDC) ? (DC*)inDC : nullptr;
}

GdiObject* ObjectOf(HGDIOBJ inObject)
{
    return gObjects.count((GdiObject*)inObject) ? (GdiObject*)inObject : nullptr;
}

template <class T> T* Add(T* inObject)
{
    gObjects.insert(inObject);
    return inObject;
}

void Destroy(GdiObject* inObject)
{
    gObjects.erase(inObject);
    delete inObject;
}

Bitmap* NewBitmap(int inWidth, int inHeight)
{
    Bitmap* theBitmap = new Bitmap;
    theBitmap->mWidth = std::max(inWidth, 1);
    theBitmap->mHeight = std::max(inHeight, 1);
    theBitmap->mPitch = theBitmap->mWidth * 4;
    theBitmap->mMemory = (unsigned char*)calloc((size_t)theBitmap->mPitch, (size_t)theBitmap->mHeight);
    theBitmap->mRows = theBitmap->mMemory;
    return theBitmap;
}

// ---- pixels ----
int ShiftOf(DWORD inMask)
{
    int theShift = 0;
    while (inMask && !(inMask & 1)) {
        inMask >>= 1;
        ++theShift;
    }
    return theShift;
}

// A channel of inBits bits widened to 8 by repeating its top bits, as GDI reads 16-bit pixels
BYTE Widen(DWORD inValue, DWORD inMask)
{
    DWORD theMax = inMask >> ShiftOf(inMask);
    if (!theMax)
        return 0;
    DWORD theValue = (inValue & inMask) >> ShiftOf(inMask);
    return (BYTE)((theValue * 255 + theMax / 2) / theMax);
}

DWORD Narrow(BYTE inValue, DWORD inMask)
{
    DWORD theMax = inMask >> ShiftOf(inMask);
    return ((inValue * theMax + 127) / 255) << ShiftOf(inMask);
}

COLORREF ReadPixel(const Bitmap* inBitmap, int inX, int inY)
{
    const unsigned char* theRow = inBitmap->mRows + (ptrdiff_t)inY * inBitmap->mPitch;
    switch (inBitmap->mBitCount) {
    case 32: {
        DWORD theValue;
        memcpy(&theValue, theRow + inX * 4, 4);
        return RGB((theValue >> 16) & 255, (theValue >> 8) & 255, theValue & 255);
    }
    case 24:
        return RGB(theRow[inX * 3 + 2], theRow[inX * 3 + 1], theRow[inX * 3]);
    case 16: {
        WORD theValue;
        memcpy(&theValue, theRow + inX * 2, 2);
        return RGB(Widen(theValue, inBitmap->mMasks[0]), Widen(theValue, inBitmap->mMasks[1]),
                   Widen(theValue, inBitmap->mMasks[2]));
    }
    case 8: {
        BYTE theIndex = theRow[inX];
        if (theIndex >= inBitmap->mPalette.size())
            return 0;
        const RGBQUAD& theColor = inBitmap->mPalette[theIndex];
        return RGB(theColor.rgbRed, theColor.rgbGreen, theColor.rgbBlue);
    }
    }
    return 0;
}

void WritePixel(Bitmap* inBitmap, int inX, int inY, COLORREF inColor)
{
    unsigned char* theRow = inBitmap->mRows + (ptrdiff_t)inY * inBitmap->mPitch;
    BYTE r = GetRValue(inColor), g = GetGValue(inColor), b = GetBValue(inColor);
    switch (inBitmap->mBitCount) {
    case 32: {
        DWORD theValue = ((DWORD)r << 16) | ((DWORD)g << 8) | b;
        memcpy(theRow + inX * 4, &theValue, 4);
        break;
    }
    case 24:
        theRow[inX * 3] = b;
        theRow[inX * 3 + 1] = g;
        theRow[inX * 3 + 2] = r;
        break;
    case 16: {
        WORD theValue = (WORD)(Narrow(r, inBitmap->mMasks[0]) | Narrow(g, inBitmap->mMasks[1]) |
                               Narrow(b, inBitmap->mMasks[2]));
        memcpy(theRow + inX * 2, &theValue, 2);
        break;
    }
    case 8: {
        int theBest = 0, theBestDistance = INT32_MAX;
        for (size_t i = 0; i < inBitmap->mPalette.size(); ++i) {
            const RGBQUAD& c = inBitmap->mPalette[i];
            int theDistance = (c.rgbRed - r) * (c.rgbRed - r) + (c.rgbGreen - g) * (c.rgbGreen - g) +
                              (c.rgbBlue - b) * (c.rgbBlue - b);
            if (theDistance < theBestDistance) {
                theBest = (int)i;
                theBestDistance = theDistance;
            }
        }
        theRow[inX] = (BYTE)theBest;
        break;
    }
    }
}

// The rectangle a DC may draw in: its pixels, narrowed by its clip rectangle
RECT DrawableOf(const DC* inDC)
{
    RECT theRect = {0, 0, 0, 0};
    if (!inDC->mBitmap || inDC->mKind == DC::kScreen)
        return theRect;
    SetRect(&theRect, 0, 0, inDC->mBitmap->mWidth, inDC->mBitmap->mHeight);
    if (inDC->mClipped)
        IntersectRect(&theRect, &theRect, &inDC->mClip);
    return theRect;
}

// Draws through a DC's clip; every drawing call ends here
struct Painter {
    explicit Painter(DC* inDC) : mDC(inDC), mBounds(DrawableOf(inDC)) {}
    bool Inside(int inX, int inY) const
    {
        return inX >= mBounds.left && inX < mBounds.right && inY >= mBounds.top && inY < mBounds.bottom;
    }
    void Put(int inX, int inY, COLORREF inColor)
    {
        if (!Inside(inX, inY))
            return;
        WritePixel(mDC->mBitmap, inX, inY, inColor);
        mDC->mDrawn = true;
    }
    // inCoverage of 255 is inColor, 0 leaves the pixel
    void Blend(int inX, int inY, COLORREF inColor, int inCoverage)
    {
        if (!Inside(inX, inY) || inCoverage <= 0)
            return;
        if (inCoverage >= 255) {
            Put(inX, inY, inColor);
            return;
        }
        COLORREF theOld = ReadPixel(mDC->mBitmap, inX, inY);
        auto Mix = [inCoverage](int a, int b) { return (BYTE)((a * (255 - inCoverage) + b * inCoverage + 127) / 255); };
        Put(inX, inY, RGB(Mix(GetRValue(theOld), GetRValue(inColor)), Mix(GetGValue(theOld), GetGValue(inColor)),
                          Mix(GetBValue(theOld), GetBValue(inColor))));
    }
    void Fill(RECT inRect, COLORREF inColor)
    {
        IntersectRect(&inRect, &inRect, &mBounds);
        for (int y = inRect.top; y < inRect.bottom; ++y)
            for (int x = inRect.left; x < inRect.right; ++x)
                Put(x, y, inColor);
    }
    DC* mDC;
    RECT mBounds;
};

bool HasPen(const DC* inDC)
{
    return inDC->mPen && inDC->mPen->mLog.lopnStyle != PS_NULL;
}

bool HasBrush(const DC* inDC)
{
    return inDC->mBrush && inDC->mBrush->mLog.lbStyle != BS_NULL;
}

// A line from one point to another, without the last point, as LineTo draws it
void Line(Painter& ioPainter, POINT inFrom, POINT inTo, COLORREF inColor)
{
    int dx = std::abs(inTo.x - inFrom.x), dy = -std::abs(inTo.y - inFrom.y);
    int sx = inFrom.x < inTo.x ? 1 : -1, sy = inFrom.y < inTo.y ? 1 : -1;
    int theError = dx + dy, x = inFrom.x, y = inFrom.y;
    while (x != inTo.x || y != inTo.y) {
        ioPainter.Put(x, y, inColor);
        int theTwice = 2 * theError;
        if (theTwice >= dy) {
            theError += dy;
            x += sx;
        }
        if (theTwice <= dx) {
            theError += dx;
            y += sy;
        }
    }
}

// ---- windows ----
Bitmap* WindowPixels(HWND inWindow)
{
    RECT theClient;
    if (!GetClientRect(inWindow, &theClient))
        return nullptr;
    int theWidth = std::max(1L, theClient.right), theHeight = std::max(1L, theClient.bottom);
    Bitmap*& thePixels = gWindowPixels[inWindow];
    if (thePixels && (thePixels->mWidth != theWidth || thePixels->mHeight != theHeight)) {
        Bitmap* theResized = NewBitmap(theWidth, theHeight);
        for (int y = 0; y < std::min(theHeight, thePixels->mHeight); ++y)
            memcpy(theResized->mRows + y * theResized->mPitch, thePixels->mRows + y * thePixels->mPitch,
                   (size_t)std::min(theWidth, thePixels->mWidth) * 4);
        delete thePixels;
        thePixels = theResized;
    }
    if (!thePixels)
        thePixels = NewBitmap(theWidth, theHeight);
    return thePixels;
}

// Copies a window's pixels into its native window; false when the window cannot show them from this thread
bool Present(HWND inWindow)
{
    auto thePixels = gWindowPixels.find(inWindow);
    if (thePixels == gWindowPixels.end())
        return true;
    SDL_Window* theNative = eeport::NativeWindow(inWindow, false);
    if (!theNative)
        return false;
    SDL_Surface* theTarget = SDL_GetWindowSurface(theNative);
    if (!theTarget)
        return true;
    Bitmap* theBitmap = thePixels->second;
    SDL_Surface* theSource = SDL_CreateRGBSurfaceWithFormatFrom(theBitmap->mRows, theBitmap->mWidth,
                                                                theBitmap->mHeight, 32, theBitmap->mPitch,
                                                                SDL_PIXELFORMAT_XRGB8888);
    if (theSource) {
        SDL_BlitSurface(theSource, NULL, theTarget, NULL);
        SDL_FreeSurface(theSource);
        SDL_UpdateWindowSurface(theNative);
    }
    return true;
}

void Drawn(DC* inDC)
{
    if (inDC->mKind != DC::kWindow || !inDC->mDrawn)
        return;
    inDC->mDrawn = false;
    if (!Present(inDC->mWindow))
        gToPresent.insert(inDC->mWindow);
}

// ---- selection ----
void Select(GdiObject* inObject)
{
    if (inObject)
        ++inObject->mSelections;
}

void Deselect(GdiObject* inObject)
{
    if (inObject && --inObject->mSelections == 0 && inObject->mDeletePending)
        Destroy(inObject);
}

GdiObject* Stock(int inIndex)
{
    if (gStock.empty()) {
        auto MakeBrush = [](UINT inStyle, COLORREF inColor) {
            Brush* theBrush = Add(new Brush);
            theBrush->mLog = {inStyle, inColor, 0};
            return theBrush;
        };
        auto MakePen = [](UINT inStyle, COLORREF inColor) {
            Pen* thePen = Add(new Pen);
            thePen->mLog = {inStyle, {1, 0}, inColor};
            return thePen;
        };
        auto MakeFont = [](LONG inHeight, LONG inWeight, const char* inFace) {
            Font* theFont = Add(new Font);
            theFont->mLog.lfHeight = inHeight;
            theFont->mLog.lfWeight = inWeight;
            theFont->mLog.lfCharSet = ANSI_CHARSET;
            lstrcpynA(theFont->mLog.lfFaceName, inFace, LF_FACESIZE);
            return theFont;
        };
        gStock[WHITE_BRUSH] = MakeBrush(BS_SOLID, RGB(255, 255, 255));
        gStock[LTGRAY_BRUSH] = MakeBrush(BS_SOLID, RGB(192, 192, 192));
        gStock[GRAY_BRUSH] = MakeBrush(BS_SOLID, RGB(128, 128, 128));
        gStock[DKGRAY_BRUSH] = MakeBrush(BS_SOLID, RGB(64, 64, 64));
        gStock[BLACK_BRUSH] = MakeBrush(BS_SOLID, RGB(0, 0, 0));
        gStock[NULL_BRUSH] = MakeBrush(BS_NULL, 0);
        gStock[DC_BRUSH] = MakeBrush(BS_SOLID, RGB(255, 255, 255));
        gStock[WHITE_PEN] = MakePen(PS_SOLID, RGB(255, 255, 255));
        gStock[BLACK_PEN] = MakePen(PS_SOLID, RGB(0, 0, 0));
        gStock[NULL_PEN] = MakePen(PS_NULL, 0);
        gStock[DC_PEN] = MakePen(PS_SOLID, RGB(0, 0, 0));
        gStock[SYSTEM_FONT] = MakeFont(16, FW_BOLD, "System");
        gStock[DEVICE_DEFAULT_FONT] = gStock[SYSTEM_FONT];
        gStock[ANSI_VAR_FONT] = MakeFont(12, FW_NORMAL, "MS Sans Serif");
        gStock[DEFAULT_GUI_FONT] = MakeFont(-11, FW_NORMAL, "MS Shell Dlg");
        gStock[ANSI_FIXED_FONT] = MakeFont(12, FW_NORMAL, "Courier New");
        gStock[OEM_FIXED_FONT] = gStock[ANSI_FIXED_FONT];
        gStock[SYSTEM_FIXED_FONT] = gStock[ANSI_FIXED_FONT];
        gStock[-1] = Add(NewBitmap(1, 1));       // a memory DC's first bitmap
        for (auto& theEntry : gStock)
            theEntry.second->mStock = true;
    }
    auto theEntry = gStock.find(inIndex);
    return theEntry == gStock.end() ? nullptr : theEntry->second;
}

DC* NewDC(DC::Kind inKind)
{
    DC* theDC = new DC;
    theDC->mKind = inKind;
    theDC->mPen = (Pen*)Stock(BLACK_PEN);
    theDC->mBrush = (Brush*)Stock(WHITE_BRUSH);
    theDC->mFont = (Font*)Stock(SYSTEM_FONT);
    Select(theDC->mPen);
    Select(theDC->mBrush);
    Select(theDC->mFont);
    gDCs.insert(theDC);
    return theDC;
}

void FreeDC(DC* inDC)
{
    Deselect(inDC->mPen);
    Deselect(inDC->mBrush);
    Deselect(inDC->mFont);
    if (inDC->mKind == DC::kMemory)
        Deselect(inDC->mBitmap);
    else if (inDC->mKind == DC::kBuffer)
        delete inDC->mBitmap;
    gDCs.erase(inDC);
    delete inDC;
}

// ---- text ----
const eeport::gdi::GdiFont& Realized(Font* inFont)
{
    if (!inFont->mRealized) {
        inFont->mRealized = true;
        const LOGFONTA& theLog = inFont->mLog;
        std::string theFace = theLog.lfFaceName[0] ? theLog.lfFaceName : "Arial";
        bool theSynthBold = false, theSynthItalic = false;
        const eeport::gdi::TrueTypeFace* theTrueType = eeport::gdi::truetype_find(theFace);
        if (!theTrueType)
            theTrueType = eeport::gdi::truetype_system_face(theFace, theLog.lfWeight, theLog.lfItalic != 0,
                                                            &theSynthBold, &theSynthItalic);
        // Windows' font mapper answers a name no installed font has with Arial
        if (!theTrueType)
            theTrueType = eeport::gdi::truetype_system_face("Arial", theLog.lfWeight, theLog.lfItalic != 0,
                                                            &theSynthBold, &theSynthItalic);
        static std::set<std::string> sReported;
        if (sReported.insert(theFace).second)
            eeport::Log("font \"%s\" is drawn with %s", theFace.c_str(),
                        theTrueType ? eeport::gdi::truetype_family_name(theTrueType).c_str() : "no face; text is empty");
        eeport::gdi::GdiLogFont theRequest;
        theRequest.height = theLog.lfHeight;
        theRequest.width = theLog.lfWidth;
        theRequest.weight = theLog.lfWeight;
        theRequest.italic = theLog.lfItalic != 0;
        theRequest.underline = theLog.lfUnderline != 0;
        theRequest.strikeout = theLog.lfStrikeOut != 0;
        theRequest.charset = theLog.lfCharSet;
        theRequest.quality = theLog.lfQuality;
        inFont->mFont = eeport::gdi::truetype_realize(theTrueType, theRequest, theSynthBold, theSynthItalic);
    }
    return inFont->mFont;
}

std::vector<uint32_t> CodePoints(LPCSTR inText, int inLength)
{
    std::vector<uint32_t> thePoints;
    for (int i = 0; i < inLength; ++i) {
        wchar_t theWide = 0;
        MultiByteToWideChar(CP_ACP, 0, inText + i, 1, &theWide, 1);
        thePoints.push_back((uint32_t)theWide);
    }
    return thePoints;
}

int Advance(const eeport::gdi::GdiFont& inFont, uint32_t inCode)
{
    return eeport::gdi::truetype_gdi_advance(inFont, inCode);
}

SIZE Extent(DC* inDC, const std::vector<uint32_t>& inText)
{
    const eeport::gdi::GdiFont& theFont = Realized(inDC->mFont);
    SIZE theSize = {0, theFont.height};
    for (uint32_t theCode : inText)
        theSize.cx += Advance(theFont, theCode);
    return theSize;
}

// Draws text with its cell's top left at inX, inY (TA_TOP | TA_LEFT)
BOOL PaintText(DC* inDC, int inX, int inY, UINT inOptions, const RECT* inRect, const std::vector<uint32_t>& inText,
              const INT* inAdvances)
{
    const eeport::gdi::GdiFont& theFont = Realized(inDC->mFont);
    Painter thePainter(inDC);
    if (inRect && (inOptions & ETO_CLIPPED))
        IntersectRect(&thePainter.mBounds, &thePainter.mBounds, inRect);
    if (inRect && (inOptions & ETO_OPAQUE)) {
        RECT theFill = *inRect;
        Painter theBackground(inDC);
        theBackground.Fill(theFill, inDC->mBkColor);
    }
    int theWidth = 0;
    for (size_t i = 0; i < inText.size(); ++i)
        theWidth += inAdvances ? inAdvances[i] : Advance(theFont, inText[i]);
    if (inDC->mBkMode == OPAQUE) {
        RECT theCell = {inX, inY, inX + theWidth, inY + theFont.height};
        thePainter.Fill(theCell, inDC->mBkColor);
    }
    int theBaseline = inY + theFont.ascent;
    int thePen = inX;
    for (size_t i = 0; i < inText.size(); ++i) {
        const eeport::gdi::TrueTypeGlyph& theGlyph = eeport::gdi::truetype_gdi_glyph(theFont, inText[i]);
        for (int y = 0; y < theGlyph.h; ++y)
            for (int x = 0; x < theGlyph.w; ++x)
                thePainter.Blend(thePen + theGlyph.x + x, theBaseline + theGlyph.y + y, inDC->mTextColor,
                                 theGlyph.coverage[(size_t)y * theGlyph.w + x]);
        thePen += inAdvances ? inAdvances[i] : Advance(theFont, inText[i]);
    }
    if (theFont.underlined) {
        RECT theLine = {inX, theBaseline + theFont.underline_pos, inX + theWidth,
                        theBaseline + theFont.underline_pos + theFont.underline_size};
        thePainter.Fill(theLine, inDC->mTextColor);
    }
    if (theFont.struck) {
        RECT theLine = {inX, theBaseline + theFont.strike_pos, inX + theWidth,
                        theBaseline + theFont.strike_pos + theFont.strike_size};
        thePainter.Fill(theLine, inDC->mTextColor);
    }
    return TRUE;
}

// ---- DIBs ----
int StrideOf(int inWidth, int inBitCount)
{
    return ((inWidth * inBitCount + 31) / 32) * 4;
}

// The colour of one pixel of a DIB row, for the formats bitmaps come in: 1, 4 and 8 bits with a palette, 16 bits
// (5-5-5, or the masks after the header for BI_BITFIELDS), 24 and 32 bits
COLORREF DIBPixel(const BITMAPINFO* inInfo, const unsigned char* inRow, int inX)
{
    const BITMAPINFOHEADER& theHeader = inInfo->bmiHeader;
    const RGBQUAD* thePalette = (const RGBQUAD*)((const unsigned char*)inInfo + theHeader.biSize);
    auto FromPalette = [&](unsigned theIndex) {
        const RGBQUAD& c = thePalette[theIndex];
        return RGB(c.rgbRed, c.rgbGreen, c.rgbBlue);
    };
    switch (theHeader.biBitCount) {
    case 1:
        return FromPalette((inRow[inX / 8] >> (7 - inX % 8)) & 1);
    case 4:
        return FromPalette((inRow[inX / 2] >> (inX % 2 ? 0 : 4)) & 15);
    case 8:
        return FromPalette(inRow[inX]);
    case 16: {
        WORD theValue;
        memcpy(&theValue, inRow + inX * 2, 2);
        DWORD theMasks[3] = {0x7c00, 0x03e0, 0x001f};
        if (theHeader.biCompression == BI_BITFIELDS)
            memcpy(theMasks, thePalette, sizeof theMasks);
        return RGB(Widen(theValue, theMasks[0]), Widen(theValue, theMasks[1]), Widen(theValue, theMasks[2]));
    }
    case 24:
        return RGB(inRow[inX * 3 + 2], inRow[inX * 3 + 1], inRow[inX * 3]);
    case 32:
        return RGB(inRow[inX * 4 + 2], inRow[inX * 4 + 1], inRow[inX * 4]);
    }
    return 0;
}

bool DIBSupported(const BITMAPINFOHEADER& inHeader)
{
    return (inHeader.biCompression == BI_RGB || inHeader.biCompression == BI_BITFIELDS) &&
           (inHeader.biBitCount == 1 || inHeader.biBitCount == 4 || inHeader.biBitCount == 8 ||
            inHeader.biBitCount == 16 || inHeader.biBitCount == 24 || inHeader.biBitCount == 32);
}

// Copies DIB pixels into a 32-bit bitmap of the same size
void FromDIB(Bitmap* outBitmap, const BITMAPINFO* inInfo, const void* inBits)
{
    const BITMAPINFOHEADER& theHeader = inInfo->bmiHeader;
    int theStride = StrideOf(theHeader.biWidth, theHeader.biBitCount);
    bool theTopDown = theHeader.biHeight < 0;
    for (int y = 0; y < outBitmap->mHeight; ++y) {
        const unsigned char* theRow =
            (const unsigned char*)inBits + (size_t)theStride * (theTopDown ? y : outBitmap->mHeight - 1 - y);
        for (int x = 0; x < outBitmap->mWidth; ++x)
            WritePixel(outBitmap, x, y, DIBPixel(inInfo, theRow, x));
    }
}

Bitmap* BitmapOf(HGDIOBJ inObject)
{
    GdiObject* theObject = ObjectOf(inObject);
    return theObject && theObject->mKind == GdiObject::kBitmap ? (Bitmap*)theObject : nullptr;
}

COLORREF BrushColor(HBRUSH inBrush, bool* outHollow)
{
    *outHollow = false;
    if ((UINT_PTR)inBrush <= COLOR_MENUBAR + 1)
        return GetSysColor((int)(UINT_PTR)inBrush - 1);       // (HBRUSH)(COLOR_x + 1)
    GdiObject* theObject = ObjectOf(inBrush);
    if (!theObject || theObject->mKind != GdiObject::kBrush || ((Brush*)theObject)->mLog.lbStyle == BS_NULL) {
        *outHollow = true;
        return 0;
    }
    return ((Brush*)theObject)->mLog.lbColor;
}

// A shape's pixels as a mask over its bounding box, for Ellipse and Arc
struct Shape {
    RECT mBox;
    std::vector<bool> mInside;
    bool At(int inX, int inY) const
    {
        return inX >= mBox.left && inX < mBox.right && inY >= mBox.top && inY < mBox.bottom &&
               mInside[(size_t)(inY - mBox.top) * (mBox.right - mBox.left) + (inX - mBox.left)];
    }
    // A pixel of the shape next to one outside it
    bool Edge(int inX, int inY) const
    {
        return At(inX, inY) && (!At(inX - 1, inY) || !At(inX + 1, inY) || !At(inX, inY - 1) || !At(inX, inY + 1));
    }
};

// The ellipse that fills a bounding box whose right and bottom edges are outside it, as Ellipse draws it: the
// pixels whose centres are inside it
Shape EllipseShape(int inLeft, int inTop, int inRight, int inBottom)
{
    Shape theShape;
    SetRect(&theShape.mBox, std::min(inLeft, inRight), std::min(inTop, inBottom), std::max(inLeft, inRight),
            std::max(inTop, inBottom));
    int theWidth = theShape.mBox.right - theShape.mBox.left, theHeight = theShape.mBox.bottom - theShape.mBox.top;
    theShape.mInside.assign((size_t)std::max(0, theWidth) * std::max(0, theHeight), false);
    double cx = (theWidth - 1) / 2.0, cy = (theHeight - 1) / 2.0;
    double rx = theWidth / 2.0, ry = theHeight / 2.0;
    for (int y = 0; y < theHeight; ++y) {
        double dy = (y - cy) / ry;
        if (std::fabs(dy) > 1.0)
            continue;
        double theHalf = rx * std::sqrt(1.0 - dy * dy);
        int theFirst = (int)std::ceil(cx - theHalf - 1e-9), theLast = (int)std::floor(cx + theHalf + 1e-9);
        for (int x = std::max(0, theFirst); x <= std::min(theWidth - 1, theLast); ++x)
            theShape.mInside[(size_t)y * theWidth + x] = true;
    }
    return theShape;
}

} // namespace

namespace eeport {

HDC CreateBufferDC(void* inBits, int inWidth, int inHeight, int inPitch, int inBitCount, DWORD inRedMask,
                   DWORD inGreenMask, DWORD inBlueMask)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    Bitmap* theBitmap = new Bitmap;     // the DC's own, not a GDI object: SelectObject cannot reach it
    theBitmap->mWidth = inWidth;
    theBitmap->mHeight = inHeight;
    theBitmap->mPitch = inPitch;
    theBitmap->mBitCount = inBitCount;
    theBitmap->mRows = (unsigned char*)inBits;
    theBitmap->mOwned = false;
    if (inBitCount == 16) {
        theBitmap->mMasks[0] = inRedMask;
        theBitmap->mMasks[1] = inGreenMask;
        theBitmap->mMasks[2] = inBlueMask;
    }
    DC* theDC = NewDC(DC::kBuffer);
    theDC->mBitmap = theBitmap;
    return (HDC)theDC;
}

void PresentWindows()
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    for (auto it = gToPresent.begin(); it != gToPresent.end();)
        it = Present(*it) ? gToPresent.erase(it) : std::next(it);
}

void ForgetWindowSurface(HWND inWindow)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    gToPresent.erase(inWindow);
    auto thePixels = gWindowPixels.find(inWindow);
    if (thePixels == gWindowPixels.end())
        return;
    delete thePixels->second;
    gWindowPixels.erase(thePixels);
    for (DC* theDC : gDCs)
        if (theDC->mKind == DC::kWindow && theDC->mWindow == inWindow)
            theDC->mBitmap = nullptr;       // a DC the game still holds draws nowhere
}

} // namespace eeport

// ---- DCs ----
HDC WINAPI GetDC(HWND inWindow)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    if (!inWindow || inWindow == GetDesktopWindow())
        return (HDC)NewDC(DC::kScreen);
    Bitmap* thePixels = WindowPixels(inWindow);
    if (!thePixels)
        return NULL;
    DC* theDC = NewDC(DC::kWindow);
    theDC->mWindow = inWindow;
    theDC->mBitmap = thePixels;
    return (HDC)theDC;
}

INT WINAPI ReleaseDC(HWND, HDC inDC)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || (theDC->mKind != DC::kWindow && theDC->mKind != DC::kScreen))
        return 0;
    Drawn(theDC);
    FreeDC(theDC);
    return 1;
}

HDC WINAPI BeginPaint(HWND inWindow, LPPAINTSTRUCT outPaint)
{
    bool theErase = false;
    if (!eeport::TakePaint(inWindow, &theErase))
        return NULL;
    HDC theDC = GetDC(inWindow);
    memset(outPaint, 0, sizeof *outPaint);
    outPaint->hdc = theDC;
    GetClientRect(inWindow, &outPaint->rcPaint);
    outPaint->fErase = theErase && !SendMessageA(inWindow, WM_ERASEBKGND, (WPARAM)theDC, 0);
    return theDC;
}

BOOL WINAPI EndPaint(HWND inWindow, const PAINTSTRUCT* inPaint)
{
    return ReleaseDC(inWindow, inPaint->hdc) != 0;
}

// A memory DC starts with a 1x1 bitmap; it draws into the bitmap selected into it
HDC WINAPI CreateCompatibleDC(HDC)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = NewDC(DC::kMemory);
    theDC->mBitmap = (Bitmap*)Stock(-1);
    Select(theDC->mBitmap);
    return (HDC)theDC;
}

BOOL WINAPI DeleteDC(HDC inDC)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return FALSE;
    Drawn(theDC);
    FreeDC(theDC);
    return TRUE;
}

INT WINAPI GetDeviceCaps(HDC, INT inIndex)
{
    switch (inIndex) {
    case LOGPIXELSX:
    case LOGPIXELSY:
        return 96;
    case BITSPIXEL:
        return 32;
    case PLANES:
        return 1;
    case HORZRES:
        return GetSystemMetrics(SM_CXSCREEN);
    case VERTRES:
        return GetSystemMetrics(SM_CYSCREEN);
    case NUMCOLORS:
        return -1;
    case RASTERCAPS:
        return RC_BITBLT | RC_DI_BITMAP | RC_DIBTODEV | RC_STRETCHBLT | RC_STRETCHDIB;
    case TECHNOLOGY:
        return DT_RASDISPLAY;
    case ASPECTX:
    case ASPECTY:
        return 36;
    case ASPECTXY:
        return 51;
    case VREFRESH:
        return 60;
    }
    return 0;
}

// The mapping mode is always MM_TEXT with no origin, so device and logical points are the same
BOOL WINAPI DPtoLP(HDC inDC, LPPOINT, INT)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    return DCOf(inDC) != nullptr;
}

INT WINAPI IntersectClipRect(HDC inDC, INT inLeft, INT inTop, INT inRight, INT inBottom)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return ERROR;
    RECT theRect = {inLeft, inTop, inRight, inBottom};
    if (theDC->mClipped)
        IntersectRect(&theDC->mClip, &theDC->mClip, &theRect);
    else
        theDC->mClip = theRect;
    theDC->mClipped = true;
    return IsRectEmpty(&theDC->mClip) ? NULLREGION : SIMPLEREGION;
}

// Only a NULL region, which removes the clip: the game makes no regions
INT WINAPI SelectClipRgn(HDC inDC, HRGN inRegion)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return ERROR;
    if (inRegion) {
        EE_PORT_UNSUPPORTED("SelectClipRgn with a region");
        return ERROR;
    }
    theDC->mClipped = false;
    return SIMPLEREGION;
}

COLORREF WINAPI SetTextColor(HDC inDC, COLORREF inColor)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return CLR_INVALID;
    COLORREF thePrevious = theDC->mTextColor;
    theDC->mTextColor = inColor;
    return thePrevious;
}

COLORREF WINAPI SetBkColor(HDC inDC, COLORREF inColor)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return CLR_INVALID;
    COLORREF thePrevious = theDC->mBkColor;
    theDC->mBkColor = inColor;
    return thePrevious;
}

INT WINAPI SetBkMode(HDC inDC, INT inMode)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || (inMode != OPAQUE && inMode != TRANSPARENT))
        return 0;
    int thePrevious = theDC->mBkMode;
    theDC->mBkMode = inMode;
    return thePrevious;
}

// ---- objects ----
HGDIOBJ WINAPI GetStockObject(INT inIndex)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    return inIndex >= 0 ? (HGDIOBJ)Stock(inIndex) : NULL;
}

HPEN WINAPI CreatePen(INT inStyle, INT inWidth, COLORREF inColor)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    Pen* thePen = Add(new Pen);
    thePen->mLog = {(UINT)inStyle, {std::max(inWidth, 0), 0}, inColor};
    return (HPEN)thePen;
}

HBRUSH WINAPI CreateSolidBrush(COLORREF inColor)
{
    LOGBRUSH theLog = {BS_SOLID, inColor, 0};
    return CreateBrushIndirect(&theLog);
}

// A hatched or pattern brush paints its colour solid
HBRUSH WINAPI CreateBrushIndirect(const LOGBRUSH* inLog)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    Brush* theBrush = Add(new Brush);
    theBrush->mLog = *inLog;
    if (theBrush->mLog.lbStyle != BS_SOLID && theBrush->mLog.lbStyle != BS_NULL) {
        EE_PORT_UNSUPPORTED("a hatched or pattern brush");
        theBrush->mLog.lbStyle = BS_SOLID;
    }
    return (HBRUSH)theBrush;
}

HFONT WINAPI CreateFontA(INT inHeight, INT inWidth, INT inEscapement, INT inOrientation, INT inWeight,
                         DWORD inItalic, DWORD inUnderline, DWORD inStrikeOut, DWORD inCharSet, DWORD inPrecision,
                         DWORD inClipPrecision, DWORD inQuality, DWORD inPitch, LPCSTR inFace)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    Font* theFont = Add(new Font);
    LOGFONTA& theLog = theFont->mLog;
    theLog.lfHeight = inHeight;
    theLog.lfWidth = inWidth;
    theLog.lfEscapement = inEscapement;
    theLog.lfOrientation = inOrientation;
    theLog.lfWeight = inWeight;
    theLog.lfItalic = (BYTE)inItalic;
    theLog.lfUnderline = (BYTE)inUnderline;
    theLog.lfStrikeOut = (BYTE)inStrikeOut;
    theLog.lfCharSet = (BYTE)inCharSet;
    theLog.lfOutPrecision = (BYTE)inPrecision;
    theLog.lfClipPrecision = (BYTE)inClipPrecision;
    theLog.lfQuality = (BYTE)inQuality;
    theLog.lfPitchAndFamily = (BYTE)inPitch;
    lstrcpynA(theLog.lfFaceName, inFace ? inFace : "", LF_FACESIZE);
    return (HFONT)theFont;
}

// A bitmap compatible with any DC is 32-bit: every DC here draws in colour
HBITMAP WINAPI CreateCompatibleBitmap(HDC, INT inWidth, INT inHeight)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    if (inWidth <= 0 || inHeight <= 0)
        return (HBITMAP)Stock(-1);
    return (HBITMAP)Add(NewBitmap(inWidth, inHeight));
}

HBITMAP WINAPI CreateDIBitmap(HDC, const BITMAPINFOHEADER* inHeader, DWORD inInit, LPCVOID inBits,
                              const BITMAPINFO* inInfo, UINT inUsage)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    if (!inHeader || inHeader->biWidth <= 0 || inHeader->biHeight == 0)
        return NULL;
    Bitmap* theBitmap = NewBitmap(inHeader->biWidth, std::abs(inHeader->biHeight));
    if ((inInit & CBM_INIT) && inBits && inInfo) {
        if (inUsage != DIB_RGB_COLORS || !DIBSupported(inInfo->bmiHeader)) {
            EE_PORT_UNSUPPORTED("CreateDIBitmap with palette indices or compression");
            delete theBitmap;
            return NULL;
        }
        FromDIB(theBitmap, inInfo, inBits);
    }
    return (HBITMAP)Add(theBitmap);
}

// The caller writes the pixels, so they keep the layout asked for: 8, 16, 24 or 32 bits, bottom-up for a positive
// height
HBITMAP WINAPI CreateDIBSection(HDC, const BITMAPINFO* inInfo, UINT inUsage, void** outBits, HANDLE inSection,
                                DWORD)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    const BITMAPINFOHEADER& theHeader = inInfo->bmiHeader;
    int theBitCount = theHeader.biBitCount;
    if (inSection || inUsage != DIB_RGB_COLORS || theHeader.biWidth <= 0 || theHeader.biHeight == 0 ||
        (theBitCount != 8 && theBitCount != 16 && theBitCount != 24 && theBitCount != 32) ||
        (theHeader.biCompression != BI_RGB && theHeader.biCompression != BI_BITFIELDS)) {
        EE_PORT_UNSUPPORTED("this CreateDIBSection format");
        return NULL;
    }
    Bitmap* theBitmap = new Bitmap;
    theBitmap->mWidth = theHeader.biWidth;
    theBitmap->mHeight = std::abs(theHeader.biHeight);
    theBitmap->mBitCount = theBitCount;
    theBitmap->mSection = true;
    theBitmap->mTopDown = theHeader.biHeight < 0;
    int theStride = StrideOf(theBitmap->mWidth, theBitCount);
    theBitmap->mMemory = (unsigned char*)calloc((size_t)theStride, (size_t)theBitmap->mHeight);
    theBitmap->mPitch = theBitmap->mTopDown ? theStride : -theStride;
    theBitmap->mRows = theBitmap->mTopDown ? theBitmap->mMemory
                                           : theBitmap->mMemory + (size_t)theStride * (theBitmap->mHeight - 1);
    const RGBQUAD* theColors = (const RGBQUAD*)((const unsigned char*)inInfo + theHeader.biSize);
    if (theBitCount == 16) {
        theBitmap->mMasks[0] = 0x7c00;
        theBitmap->mMasks[1] = 0x03e0;
        theBitmap->mMasks[2] = 0x001f;
        if (theHeader.biCompression == BI_BITFIELDS)
            memcpy(theBitmap->mMasks, theColors, sizeof theBitmap->mMasks);
    }
    if (theBitCount == 8)
        theBitmap->mPalette.assign(theColors, theColors + (theHeader.biClrUsed ? theHeader.biClrUsed : 256));
    if (outBits)
        *outBits = theBitmap->mMemory;
    return (HBITMAP)Add(theBitmap);
}

// A bitmap resource is a DIB without its file header
HBITMAP WINAPI LoadBitmapA(HINSTANCE inInstance, LPCSTR inName)
{
    DWORD theSize = 0;
    const BITMAPINFO* theInfo = (const BITMAPINFO*)eeport::FindResourceData(inInstance, RT_BITMAP, inName, 0, &theSize);
    if (!theInfo || theSize < sizeof(BITMAPINFOHEADER) || !DIBSupported(theInfo->bmiHeader)) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return NULL;
    }
    const BITMAPINFOHEADER& theHeader = theInfo->bmiHeader;
    DWORD theColors = theHeader.biClrUsed ? theHeader.biClrUsed
                    : theHeader.biBitCount <= 8 ? 1u << theHeader.biBitCount
                    : theHeader.biCompression == BI_BITFIELDS ? 3 : 0;
    const unsigned char* theBits = (const unsigned char*)theInfo + theHeader.biSize + theColors * 4;
    size_t theNeeded = (size_t)StrideOf(theHeader.biWidth, theHeader.biBitCount) * std::abs(theHeader.biHeight);
    if (theBits + theNeeded > (const unsigned char*)theInfo + theSize) {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return NULL;
    }
    return CreateDIBitmap(NULL, &theHeader, CBM_INIT, theBits, theInfo, DIB_RGB_COLORS);
}

// Copies scan lines of a bitmap out as a DIB of 1, 24 or 32 bits; with no buffer, fills in the header. A 1-bit copy
// sets each pixel nearer white than black, with a black and white palette.
INT WINAPI GetDIBits(HDC, HBITMAP inBitmap, UINT inStart, UINT inLines, LPVOID outBits, LPBITMAPINFO ioInfo,
                     UINT inUsage)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    Bitmap* theBitmap = BitmapOf(inBitmap);
    if (!theBitmap || !ioInfo || inUsage != DIB_RGB_COLORS)
        return 0;
    BITMAPINFOHEADER& theHeader = ioInfo->bmiHeader;
    if (!outBits) {
        theHeader.biWidth = theBitmap->mWidth;
        theHeader.biHeight = theBitmap->mHeight;
        theHeader.biPlanes = 1;
        theHeader.biBitCount = 32;
        theHeader.biCompression = BI_RGB;
        theHeader.biSizeImage = (DWORD)StrideOf(theBitmap->mWidth, 32) * theBitmap->mHeight;
        return theBitmap->mHeight;
    }
    int theBitCount = theHeader.biBitCount;
    if (theBitCount != 1 && theBitCount != 24 && theBitCount != 32) {
        EE_PORT_UNSUPPORTED("GetDIBits to this format");
        return 0;
    }
    int theWidth = std::min<int>(theHeader.biWidth, theBitmap->mWidth);
    int theStride = StrideOf(theHeader.biWidth, theBitCount);
    bool theTopDown = theHeader.biHeight < 0;
    int theHeight = std::abs(theHeader.biHeight);
    if (theBitCount == 1) {
        RGBQUAD theBlack = {0, 0, 0, 0}, theWhite = {255, 255, 255, 0};
        ioInfo->bmiColors[0] = theBlack;
        ioInfo->bmiColors[1] = theWhite;
    }
    int theCopied = 0;
    for (UINT i = 0; i < inLines; ++i) {
        int theLine = (int)(inStart + i);                       // DIB scan line, counted from the DIB's first row
        int theImageRow = theTopDown ? theLine : theHeight - 1 - theLine;
        if (theImageRow < 0 || theImageRow >= theBitmap->mHeight || theLine >= theHeight)
            break;
        unsigned char* theRow = (unsigned char*)outBits + (size_t)i * theStride;
        memset(theRow, 0, (size_t)theStride);
        for (int x = 0; x < theWidth; ++x) {
            COLORREF theColor = ReadPixel(theBitmap, x, theImageRow);
            BYTE r = GetRValue(theColor), g = GetGValue(theColor), b = GetBValue(theColor);
            if (theBitCount == 32) {
                theRow[x * 4] = b;
                theRow[x * 4 + 1] = g;
                theRow[x * 4 + 2] = r;
            } else if (theBitCount == 24) {
                theRow[x * 3] = b;
                theRow[x * 3 + 1] = g;
                theRow[x * 3 + 2] = r;
            } else if (r + g + b >= 3 * 128)
                theRow[x / 8] |= 0x80 >> (x % 8);
        }
        ++theCopied;
    }
    return theCopied;
}

INT WINAPI GetObjectA(HGDIOBJ inObject, INT inSize, LPVOID outData)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    GdiObject* theObject = ObjectOf(inObject);
    if (!theObject)
        return 0;
    auto Copy = [&](const void* inSource, int inBytes) {
        if (!outData)
            return inBytes;
        int theBytes = std::min(inBytes, inSize);
        memcpy(outData, inSource, (size_t)std::max(theBytes, 0));
        return std::max(theBytes, 0);
    };
    switch (theObject->mKind) {
    case GdiObject::kPen:
        return Copy(&((Pen*)theObject)->mLog, sizeof(LOGPEN));
    case GdiObject::kBrush:
        return Copy(&((Brush*)theObject)->mLog, sizeof(LOGBRUSH));
    case GdiObject::kFont:
        return Copy(&((Font*)theObject)->mLog, sizeof(LOGFONTA));
    case GdiObject::kBitmap: {
        Bitmap* theBitmap = (Bitmap*)theObject;
        DIBSECTION theSection = {};
        BITMAP& theInfo = theSection.dsBm;
        theInfo.bmWidth = theBitmap->mWidth;
        theInfo.bmHeight = theBitmap->mHeight;
        theInfo.bmWidthBytes = StrideOf(theBitmap->mWidth, theBitmap->mBitCount);
        theInfo.bmPlanes = 1;
        theInfo.bmBitsPixel = (WORD)theBitmap->mBitCount;
        if (!theBitmap->mSection || inSize < (INT)sizeof(DIBSECTION))
            return Copy(&theInfo, sizeof(BITMAP));
        theInfo.bmBits = theBitmap->mMemory;
        theSection.dsBmih.biSize = sizeof(BITMAPINFOHEADER);
        theSection.dsBmih.biWidth = theBitmap->mWidth;
        theSection.dsBmih.biHeight = theBitmap->mTopDown ? -theBitmap->mHeight : theBitmap->mHeight;
        theSection.dsBmih.biPlanes = 1;
        theSection.dsBmih.biBitCount = (WORD)theBitmap->mBitCount;
        theSection.dsBmih.biSizeImage = (DWORD)theInfo.bmWidthBytes * theBitmap->mHeight;
        memcpy(theSection.dsBitfields, theBitmap->mMasks, sizeof theSection.dsBitfields);
        return Copy(&theSection, sizeof(DIBSECTION));
    }
    }
    return 0;
}

HGDIOBJ WINAPI GetCurrentObject(HDC inDC, UINT inType)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return NULL;
    switch (inType) {
    case OBJ_PEN: return (HGDIOBJ)theDC->mPen;
    case OBJ_BRUSH: return (HGDIOBJ)theDC->mBrush;
    case OBJ_FONT: return (HGDIOBJ)theDC->mFont;
    case OBJ_BITMAP: return theDC->mKind == DC::kMemory ? (HGDIOBJ)theDC->mBitmap : NULL;
    }
    return NULL;
}

// Returns the object of the same kind the DC had. A bitmap goes into one memory DC at a time.
HGDIOBJ WINAPI SelectObject(HDC inDC, HGDIOBJ inObject)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    GdiObject* theObject = ObjectOf(inObject);
    if (!theDC || !theObject || theObject->mDeletePending)
        return NULL;
    GdiObject** theSlot = nullptr;
    switch (theObject->mKind) {
    case GdiObject::kPen: theSlot = (GdiObject**)&theDC->mPen; break;
    case GdiObject::kBrush: theSlot = (GdiObject**)&theDC->mBrush; break;
    case GdiObject::kFont: theSlot = (GdiObject**)&theDC->mFont; break;
    case GdiObject::kBitmap:
        if (theDC->mKind != DC::kMemory || (theObject->mSelections > 0 && !theObject->mStock &&
                                            theObject != theDC->mBitmap))
            return NULL;
        theSlot = (GdiObject**)&theDC->mBitmap;
        break;
    }
    GdiObject* thePrevious = *theSlot;
    if (thePrevious == theObject)
        return (HGDIOBJ)thePrevious;
    Select(theObject);
    *theSlot = theObject;
    // The previous object stays alive for the caller, who selects it back or deletes it
    if (thePrevious)
        --thePrevious->mSelections;
    HGDIOBJ theResult = (HGDIOBJ)thePrevious;
    if (thePrevious && thePrevious->mSelections == 0 && thePrevious->mDeletePending) {
        Destroy(thePrevious);
        theResult = NULL;
    }
    return theResult;
}

// A pen, brush or font still selected goes when its last DC lets it go; a selected bitmap is not deleted, as on
// Windows. Stock objects stay.
BOOL WINAPI DeleteObject(HGDIOBJ inObject)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    GdiObject* theObject = ObjectOf(inObject);
    if (!theObject)
        return FALSE;
    if (theObject->mStock)
        return TRUE;
    if (theObject->mSelections > 0) {
        if (theObject->mKind == GdiObject::kBitmap)
            return FALSE;
        theObject->mDeletePending = true;
        return TRUE;
    }
    Destroy(theObject);
    return TRUE;
}

// ---- drawing ----
COLORREF WINAPI SetPixel(HDC inDC, INT inX, INT inY, COLORREF inColor)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return CLR_INVALID;
    Painter thePainter(theDC);
    if (!thePainter.Inside(inX, inY))
        return CLR_INVALID;
    thePainter.Put(inX, inY, inColor & 0xffffff);
    return ReadPixel(theDC->mBitmap, inX, inY);
}

COLORREF WINAPI GetPixel(HDC inDC, INT inX, INT inY)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || !Painter(theDC).Inside(inX, inY))
        return CLR_INVALID;
    return ReadPixel(theDC->mBitmap, inX, inY);
}

INT WINAPI FillRect(HDC inDC, const RECT* inRect, HBRUSH inBrush)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    bool theHollow;
    COLORREF theColor = BrushColor(inBrush, &theHollow);
    if (!theDC || !inRect)
        return FALSE;
    if (!theHollow)
        Painter(theDC).Fill(*inRect, theColor);
    return TRUE;
}

// The outline in the pen inside the rectangle, its right and bottom edges outside; the inside in the brush. Without a
// pen the filled area is one pixel smaller on the right and bottom, as on Windows.
BOOL WINAPI Rectangle(HDC inDC, INT inLeft, INT inTop, INT inRight, INT inBottom)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return FALSE;
    RECT theRect = {std::min(inLeft, inRight), std::min(inTop, inBottom), std::max(inLeft, inRight),
                    std::max(inTop, inBottom)};
    Painter thePainter(theDC);
    bool thePen = HasPen(theDC);
    if (HasBrush(theDC)) {
        RECT theInside = theRect;
        if (thePen)
            InflateRect(&theInside, -1, -1);
        else {
            --theInside.right;
            --theInside.bottom;
        }
        thePainter.Fill(theInside, theDC->mBrush->mLog.lbColor);
    }
    if (thePen && theRect.right > theRect.left && theRect.bottom > theRect.top) {
        COLORREF theColor = theDC->mPen->mLog.lopnColor;
        RECT theTop = {theRect.left, theRect.top, theRect.right, theRect.top + 1};
        RECT theBottom = {theRect.left, theRect.bottom - 1, theRect.right, theRect.bottom};
        RECT theLeft = {theRect.left, theRect.top, theRect.left + 1, theRect.bottom};
        RECT theRight = {theRect.right - 1, theRect.top, theRect.right, theRect.bottom};
        thePainter.Fill(theTop, theColor);
        thePainter.Fill(theBottom, theColor);
        thePainter.Fill(theLeft, theColor);
        thePainter.Fill(theRight, theColor);
    }
    return TRUE;
}

BOOL WINAPI Ellipse(HDC inDC, INT inLeft, INT inTop, INT inRight, INT inBottom)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return FALSE;
    Shape theShape = EllipseShape(inLeft, inTop, inRight, inBottom);
    Painter thePainter(theDC);
    bool thePen = HasPen(theDC), theBrush = HasBrush(theDC);
    for (int y = theShape.mBox.top; y < theShape.mBox.bottom; ++y)
        for (int x = theShape.mBox.left; x < theShape.mBox.right; ++x) {
            if (thePen && theShape.Edge(x, y))
                thePainter.Put(x, y, theDC->mPen->mLog.lopnColor);
            else if (theBrush && theShape.At(x, y))
                thePainter.Put(x, y, theDC->mBrush->mLog.lbColor);
        }
    return TRUE;
}

// The ellipse's outline counter-clockwise from the ray through the start point to the ray through the end point
BOOL WINAPI Arc(HDC inDC, INT inLeft, INT inTop, INT inRight, INT inBottom, INT inStartX, INT inStartY, INT inEndX,
                INT inEndY)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return FALSE;
    if (!HasPen(theDC))
        return TRUE;
    Shape theShape = EllipseShape(inLeft, inTop, inRight, inBottom);
    double cx = (theShape.mBox.left + theShape.mBox.right - 1) / 2.0;
    double cy = (theShape.mBox.top + theShape.mBox.bottom - 1) / 2.0;
    const double kTurn = 2 * M_PI;
    auto AngleOf = [&](double x, double y) {
        double theAngle = std::atan2(cy - y, x - cx);       // y grows down on the screen
        return theAngle < 0 ? theAngle + kTurn : theAngle;
    };
    double theStart = AngleOf(inStartX, inStartY), theEnd = AngleOf(inEndX, inEndY);
    double theSweep = std::fmod(theEnd - theStart + kTurn, kTurn);
    if (theSweep == 0)
        theSweep = kTurn;
    Painter thePainter(theDC);
    for (int y = theShape.mBox.top; y < theShape.mBox.bottom; ++y)
        for (int x = theShape.mBox.left; x < theShape.mBox.right; ++x)
            if (theShape.Edge(x, y) && std::fmod(AngleOf(x, y) - theStart + kTurn, kTurn) <= theSweep + 1e-9)
                thePainter.Put(x, y, theDC->mPen->mLog.lopnColor);
    return TRUE;
}

// Each segment without its last point, as LineTo draws them
BOOL WINAPI Polyline(HDC inDC, const POINT* inPoints, INT inCount)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || inCount < 2)
        return FALSE;
    if (!HasPen(theDC))
        return TRUE;
    Painter thePainter(theDC);
    for (int i = 0; i + 1 < inCount; ++i)
        Line(thePainter, inPoints[i], inPoints[i + 1], theDC->mPen->mLog.lopnColor);
    return TRUE;
}

// The inside by the alternate (even-odd) rule, pixel centres tested; the outline closed in the pen
BOOL WINAPI Polygon(HDC inDC, const POINT* inPoints, INT inCount)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || inCount < 2)
        return FALSE;
    Painter thePainter(theDC);
    if (HasBrush(theDC)) {
        int theTop = inPoints[0].y, theBottom = inPoints[0].y;
        for (int i = 1; i < inCount; ++i) {
            theTop = std::min<int>(theTop, inPoints[i].y);
            theBottom = std::max<int>(theBottom, inPoints[i].y);
        }
        std::vector<double> theCrossings;
        for (int y = theTop; y < theBottom; ++y) {
            double theCentre = y + 0.5;
            theCrossings.clear();
            for (int i = 0; i < inCount; ++i) {
                POINT a = inPoints[i], b = inPoints[(i + 1) % inCount];
                if ((a.y <= theCentre) == (b.y <= theCentre))
                    continue;
                theCrossings.push_back(a.x + (theCentre - a.y) * (b.x - a.x) / (double)(b.y - a.y));
            }
            std::sort(theCrossings.begin(), theCrossings.end());
            for (size_t i = 0; i + 1 < theCrossings.size(); i += 2)
                for (int x = (int)std::ceil(theCrossings[i] - 0.5); x + 0.5 < theCrossings[i + 1]; ++x)
                    thePainter.Put(x, y, theDC->mBrush->mLog.lbColor);
        }
    }
    if (HasPen(theDC))
        for (int i = 0; i < inCount; ++i)
            Line(thePainter, inPoints[i], inPoints[(i + 1) % inCount], theDC->mPen->mLog.lopnColor);
    return TRUE;
}

// Any of the 256 ternary raster operations: each result bit is the operation's bit for the pattern, source and
// destination bits at that place
BOOL WINAPI BitBlt(HDC inDestination, INT inX, INT inY, INT inWidth, INT inHeight, HDC inSource, INT inSourceX,
                   INT inSourceY, DWORD inOperation)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDestination = DCOf(inDestination);
    if (!theDestination)
        return FALSE;
    BYTE theTable = (BYTE)(inOperation >> 16);
    bool theUsesSource = ((theTable >> 2) & 0x33) != (theTable & 0x33);
    bool theUsesPattern = ((theTable >> 4) & 0x0f) != (theTable & 0x0f);
    DC* theSource = theUsesSource ? DCOf(inSource) : nullptr;
    if (theUsesSource && (!theSource || !theSource->mBitmap))
        return FALSE;
    DWORD thePattern = 0;
    if (theUsesPattern && HasBrush(theDestination))
        thePattern = theDestination->mBrush->mLog.lbColor & 0xffffff;
    Painter thePainter(theDestination);
    for (int y = 0; y < inHeight; ++y)
        for (int x = 0; x < inWidth; ++x) {
            int theX = inX + x, theY = inY + y;
            if (!thePainter.Inside(theX, theY))
                continue;
            DWORD theSourceColor = 0;
            if (theSource) {
                int sx = inSourceX + x, sy = inSourceY + y;
                if (sx < 0 || sy < 0 || sx >= theSource->mBitmap->mWidth || sy >= theSource->mBitmap->mHeight)
                    continue;
                theSourceColor = ReadPixel(theSource->mBitmap, sx, sy);
            }
            DWORD theDestinationColor = ReadPixel(theDestination->mBitmap, theX, theY);
            DWORD theResult = 0;
            for (int k = 0; k < 8; ++k)
                if (theTable & (1 << k))
                    theResult |= ((k & 4) ? thePattern : ~thePattern) & ((k & 2) ? theSourceColor : ~theSourceColor) &
                                 ((k & 1) ? theDestinationColor : ~theDestinationColor);
            thePainter.Put(theX, theY, theResult & 0xffffff);
        }
    return TRUE;
}

// The source scaled to the destination by nearest pixels, blended by the constant alpha and, with AC_SRC_ALPHA, by
// the source's own premultiplied alpha
BOOL WINAPI AlphaBlend(HDC inDestination, int inX, int inY, int inWidth, int inHeight, HDC inSource, int inSourceX,
                       int inSourceY, int inSourceWidth, int inSourceHeight, BLENDFUNCTION inBlend)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDestination = DCOf(inDestination);
    DC* theSource = DCOf(inSource);
    if (!theDestination || !theSource || !theSource->mBitmap || inWidth <= 0 || inHeight <= 0 ||
        inSourceWidth <= 0 || inSourceHeight <= 0 || inBlend.BlendOp != AC_SRC_OVER)
        return FALSE;
    bool thePerPixel = (inBlend.AlphaFormat & AC_SRC_ALPHA) && theSource->mBitmap->mBitCount == 32;
    int theConstant = inBlend.SourceConstantAlpha;
    Painter thePainter(theDestination);
    for (int y = 0; y < inHeight; ++y)
        for (int x = 0; x < inWidth; ++x) {
            int theX = inX + x, theY = inY + y;
            int sx = inSourceX + x * inSourceWidth / inWidth, sy = inSourceY + y * inSourceHeight / inHeight;
            if (!thePainter.Inside(theX, theY) || sx < 0 || sy < 0 || sx >= theSource->mBitmap->mWidth ||
                sy >= theSource->mBitmap->mHeight)
                continue;
            COLORREF s = ReadPixel(theSource->mBitmap, sx, sy), d = ReadPixel(theDestination->mBitmap, theX, theY);
            int theAlpha = 255;
            if (thePerPixel)
                theAlpha = theSource->mBitmap->mRows[(ptrdiff_t)sy * theSource->mBitmap->mPitch + sx * 4 + 3];
            int theCover = theAlpha * theConstant / 255;     // the share of the destination that is replaced
            auto Mix = [&](int sc, int dc) {
                return (BYTE)std::min(255, (sc * theConstant + dc * (255 - theCover) + 127) / 255);
            };
            thePainter.Put(theX, theY, RGB(Mix(GetRValue(s), GetRValue(d)), Mix(GetGValue(s), GetGValue(d)),
                                           Mix(GetBValue(s), GetBValue(d))));
        }
    return TRUE;
}

// ---- text ----
BOOL WINAPI GetTextMetricsA(HDC inDC, LPTEXTMETRICA outMetrics)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || !outMetrics)
        return FALSE;
    const eeport::gdi::GdiFont& f = Realized(theDC->mFont);
    memset(outMetrics, 0, sizeof *outMetrics);
    outMetrics->tmHeight = f.height;
    outMetrics->tmAscent = f.ascent;
    outMetrics->tmDescent = f.descent;
    outMetrics->tmInternalLeading = f.internal;
    outMetrics->tmExternalLeading = f.external;
    outMetrics->tmAveCharWidth = f.ave_width;
    outMetrics->tmMaxCharWidth = f.max_width;
    outMetrics->tmWeight = f.weight;
    outMetrics->tmOverhang = f.overhang;
    outMetrics->tmDigitizedAspectX = 96;
    outMetrics->tmDigitizedAspectY = 96;
    // The byte record narrows the range as GDI does: from the default character less one, to at most 0xff
    outMetrics->tmFirstChar = (BYTE)(f.def - 1);
    outMetrics->tmLastChar = (BYTE)std::min<uint16_t>(f.last, 0xff);
    outMetrics->tmDefaultChar = (BYTE)f.def;
    outMetrics->tmBreakChar = (BYTE)f.brk;
    outMetrics->tmItalic = f.italic;
    outMetrics->tmUnderlined = f.underlined;
    outMetrics->tmStruckOut = f.struck;
    outMetrics->tmPitchAndFamily = f.pitch_family;
    outMetrics->tmCharSet = f.charset;
    return TRUE;
}

BOOL WINAPI GetTextExtentPoint32A(HDC inDC, LPCSTR inText, INT inLength, LPSIZE outSize)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || !outSize || inLength < 0)
        return FALSE;
    *outSize = Extent(theDC, CodePoints(inText, inLength));
    return TRUE;
}

BOOL ee_port_GetTextExtentPoint32W(HDC inDC, const wchar_t* inText, int inLength, LPSIZE outSize)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || !outSize || inLength < 0)
        return FALSE;
    *outSize = Extent(theDC, std::vector<uint32_t>(inText, inText + inLength));
    return TRUE;
}

BOOL WINAPI GetCharWidthW(HDC inDC, UINT inFirst, UINT inLast, LPINT outWidths)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || !outWidths || inLast < inFirst)
        return FALSE;
    const eeport::gdi::GdiFont& theFont = Realized(theDC->mFont);
    for (UINT theCode = inFirst; theCode <= inLast; ++theCode)
        outWidths[theCode - inFirst] = Advance(theFont, theCode);
    return TRUE;
}

BOOL WINAPI ExtTextOutA(HDC inDC, INT inX, INT inY, UINT inOptions, const RECT* inRect, LPCSTR inText, UINT inLength,
                        const INT* inAdvances)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC)
        return FALSE;
    return PaintText(theDC, inX, inY, inOptions, inRect, CodePoints(inText ? inText : "", inText ? inLength : 0),
                    inAdvances);
}

BOOL ee_port_TextOutW(HDC inDC, int inX, int inY, const wchar_t* inText, int inLength)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    DC* theDC = DCOf(inDC);
    if (!theDC || inLength < 0)
        return FALSE;
    return PaintText(theDC, inX, inY, 0, NULL, std::vector<uint32_t>(inText, inText + inLength), NULL);
}

// ---- icons: a colour bitmap with a mask (white is transparent), or a 32-bit bitmap alone, opaque ----
HICON WINAPI CreateIconIndirect(PICONINFO inInfo)
{
    std::lock_guard<std::recursive_mutex> theLock(gGdiLock);
    Bitmap* theColor = inInfo ? BitmapOf(inInfo->hbmColor) : nullptr;
    if (!theColor)
        return NULL;
    Bitmap* theMask = BitmapOf(inInfo->hbmMask);
    SDL_Surface* theImage =
        SDL_CreateRGBSurfaceWithFormat(0, theColor->mWidth, theColor->mHeight, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!theImage)
        return NULL;
    for (int y = 0; y < theColor->mHeight; ++y)
        for (int x = 0; x < theColor->mWidth; ++x) {
            COLORREF c = ReadPixel(theColor, x, y);
            bool theClear = theMask && x < theMask->mWidth && y < theMask->mHeight &&
                            (ReadPixel(theMask, x, y) & 0xffffff) == 0xffffff;
            Uint32 thePixel = theClear ? 0 : 0xff000000u | ((Uint32)GetRValue(c) << 16) | ((Uint32)GetGValue(c) << 8) |
                                                 GetBValue(c);
            ((Uint32*)((Uint8*)theImage->pixels + y * theImage->pitch))[x] = thePixel;
        }
    return (HICON)eeport::MakeCursor(theImage, inInfo->fIcon ? theColor->mWidth / 2 : (int)inInfo->xHotspot,
                                     inInfo->fIcon ? theColor->mHeight / 2 : (int)inInfo->yHotspot);
}

BOOL WINAPI DestroyIcon(HICON inIcon)
{
    return DestroyCursor((HCURSOR)inIcon);
}
