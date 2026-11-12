// Tests of the native build's GDI (src/Port/Linux/Win32Gdi.cpp): pixel formats, DIB layouts, raster operations,
// shapes, clipping, object selection and deletion, text drawing, window DCs and the msimg32 AlphaBlend that
// LoadLibrary finds. Expected pixels come from hand arithmetic or from the named raster operations' formulas.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
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

// A memory DC with a new compatible bitmap selected, filled white
struct Canvas {
    Canvas(int inWidth, int inHeight) : mWidth(inWidth), mHeight(inHeight)
    {
        mDC = CreateCompatibleDC(NULL);
        mBitmap = CreateCompatibleBitmap(mDC, inWidth, inHeight);
        mOld = SelectObject(mDC, mBitmap);
        RECT theAll = {0, 0, inWidth, inHeight};
        FillRect(mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    }
    ~Canvas()
    {
        SelectObject(mDC, mOld);
        DeleteObject(mBitmap);
        DeleteDC(mDC);
    }
    int Count(COLORREF inColor) const
    {
        int theCount = 0;
        for (int y = 0; y < mHeight; ++y)
            for (int x = 0; x < mWidth; ++x)
                theCount += GetPixel(mDC, x, y) == inColor;
        return theCount;
    }
    int mWidth, mHeight;
    HDC mDC;
    HBITMAP mBitmap;
    HGDIOBJ mOld;
};

static const COLORREF kRed = RGB(255, 0, 0), kBlue = RGB(0, 0, 255), kWhite = RGB(255, 255, 255);

static void TestPixelsAndFormats()
{
    Canvas theCanvas(4, 3);
    CHECK(SetPixel(theCanvas.mDC, 1, 2, RGB(10, 20, 30)) == RGB(10, 20, 30));
    CHECK(GetPixel(theCanvas.mDC, 1, 2) == RGB(10, 20, 30));
    CHECK(GetPixel(theCanvas.mDC, 4, 0) == CLR_INVALID && SetPixel(theCanvas.mDC, -1, 0, kRed) == CLR_INVALID);

    // A DirectDraw-style 16-bit buffer: 5-6-5 and 5-5-5 layouts
    WORD the565[2 * 2] = {};
    HDC theDC = eeport::CreateBufferDC(the565, 2, 2, 4, 16, 0xf800, 0x07e0, 0x001f);
    SetPixel(theDC, 0, 0, kRed);
    SetPixel(theDC, 1, 0, RGB(0, 255, 0));
    SetPixel(theDC, 0, 1, kBlue);
    SetPixel(theDC, 1, 1, kWhite);
    CHECK(the565[0] == 0xf800 && the565[1] == 0x07e0 && the565[2] == 0x001f && the565[3] == 0xffff);
    CHECK(GetPixel(theDC, 0, 0) == kRed && GetPixel(theDC, 1, 1) == kWhite);
    the565[0] = 0x8410;                         // half of each channel: 16/31, 32/63, 16/31
    CHECK(GetPixel(theDC, 0, 0) == RGB(132, 130, 132));
    SetPixel(theDC, 0, 0, RGB(13, 0, 0));       // 13 * 31 / 255 = 1.58: the nearest level is 2, not 1
    CHECK(the565[0] == 2 << 11);
    DeleteDC(theDC);
    WORD the555[1] = {};
    theDC = eeport::CreateBufferDC(the555, 1, 1, 2, 16, 0x7c00, 0x03e0, 0x001f);
    SetPixel(theDC, 0, 0, kRed);
    CHECK(the555[0] == 0x7c00);
    DeleteDC(theDC);
    // A 32-bit buffer with a pitch wider than its row
    DWORD the32[3 * 2] = {};
    theDC = eeport::CreateBufferDC(the32, 2, 2, 12, 32, 0, 0, 0);
    SetPixel(theDC, 1, 1, RGB(1, 2, 3));
    CHECK(the32[4] == 0x010203 && the32[2] == 0);
    DeleteDC(theDC);
}

static void TestDIBs()
{
    // A bottom-up section: row 0 is the last in memory, rows are 4-byte aligned
    BITMAPINFO theInfo = {};
    theInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    theInfo.bmiHeader.biWidth = 3;
    theInfo.bmiHeader.biHeight = 2;
    theInfo.bmiHeader.biPlanes = 1;
    theInfo.bmiHeader.biBitCount = 24;
    theInfo.bmiHeader.biCompression = BI_RGB;
    unsigned char* theBits = nullptr;
    HBITMAP theSection = CreateDIBSection(NULL, &theInfo, DIB_RGB_COLORS, (void**)&theBits, NULL, 0);
    CHECK(theSection && theBits);
    HDC theDC = CreateCompatibleDC(NULL);
    HGDIOBJ theOld = SelectObject(theDC, theSection);
    SetPixel(theDC, 0, 0, RGB(1, 2, 3));
    SetPixel(theDC, 2, 1, RGB(4, 5, 6));
    CHECK(theBits[12] == 3 && theBits[13] == 2 && theBits[14] == 1);    // row 0 at the second 12-byte row
    CHECK(theBits[6] == 6 && theBits[7] == 5 && theBits[8] == 4);       // row 1 first
    DIBSECTION theDescription;
    CHECK(GetObjectA(theSection, sizeof theDescription, &theDescription) == sizeof theDescription);
    CHECK(theDescription.dsBm.bmBits == theBits && theDescription.dsBm.bmWidthBytes == 12 &&
          theDescription.dsBmih.biHeight == 2);
    BITMAP thePlain;
    CHECK(GetObjectA(theSection, sizeof thePlain, &thePlain) == sizeof thePlain && thePlain.bmWidth == 3 &&
          thePlain.bmBitsPixel == 24);
    SelectObject(theDC, theOld);
    DeleteObject(theSection);

    // A top-down 32-bit section
    theInfo.bmiHeader.biHeight = -2;
    theInfo.bmiHeader.biBitCount = 32;
    DWORD* theWords = nullptr;
    theSection = CreateDIBSection(NULL, &theInfo, DIB_RGB_COLORS, (void**)&theWords, NULL, 0);
    theOld = SelectObject(theDC, theSection);
    SetPixel(theDC, 1, 1, RGB(7, 8, 9));
    CHECK(theWords[3 + 1] == 0x070809);
    SelectObject(theDC, theOld);
    DeleteObject(theSection);

    // An 8-bit DIB with a palette, bottom-up, into a device bitmap; then read back as 32-bit top-down and 1-bit
    struct {
        BITMAPINFOHEADER mHeader;
        RGBQUAD mColors[2];
    } thePaletted = {};
    thePaletted.mHeader = {sizeof(BITMAPINFOHEADER), 2, 2, 1, 8, BI_RGB, 0, 0, 0, 2, 0};
    thePaletted.mColors[0] = {0, 0, 0, 0};
    thePaletted.mColors[1] = {255, 255, 255, 0};
    const unsigned char kIndices[8] = {1, 0, 0, 0, 0, 1, 0, 0};          // bottom row: white, black
    HBITMAP theBitmap = CreateDIBitmap(NULL, &thePaletted.mHeader, CBM_INIT, kIndices, (BITMAPINFO*)&thePaletted,
                                       DIB_RGB_COLORS);
    theOld = SelectObject(theDC, theBitmap);
    CHECK(GetPixel(theDC, 0, 1) == kWhite && GetPixel(theDC, 1, 1) == 0 && GetPixel(theDC, 1, 0) == kWhite);
    BITMAPINFO theOut = {};
    theOut.bmiHeader = {sizeof(BITMAPINFOHEADER), 2, -2, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
    DWORD theCopy[4] = {};
    CHECK(GetDIBits(theDC, theBitmap, 0, 2, theCopy, &theOut, DIB_RGB_COLORS) == 2);
    CHECK(theCopy[0] == 0 && theCopy[1] == 0xffffff && theCopy[2] == 0xffffff && theCopy[3] == 0);
    struct {
        BITMAPINFOHEADER mHeader;
        RGBQUAD mColors[2];
    } theMask = {};
    theMask.mHeader = {sizeof(BITMAPINFOHEADER), 2, -2, 1, 1, BI_RGB, 0, 0, 0, 0, 0};
    DWORD theMaskBits[2] = {};
    CHECK(GetDIBits(theDC, theBitmap, 0, 2, theMaskBits, (BITMAPINFO*)&theMask, DIB_RGB_COLORS) == 2);
    CHECK((theMaskBits[0] & 0xff) == 0x40 && (theMaskBits[1] & 0xff) == 0x80 && theMask.mColors[1].rgbRed == 255);
    theOut.bmiHeader.biHeight = 2;                                        // bottom-up: the bottom row first
    CHECK(GetDIBits(theDC, theBitmap, 0, 2, theCopy, &theOut, DIB_RGB_COLORS) == 2);
    CHECK(theCopy[0] == 0xffffff && theCopy[1] == 0 && theCopy[2] == 0 && theCopy[3] == 0xffffff);
    BITMAPINFO theAsk = {};
    theAsk.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    CHECK(GetDIBits(theDC, theBitmap, 0, 0, NULL, &theAsk, DIB_RGB_COLORS) == 2 && theAsk.bmiHeader.biWidth == 2);
    SelectObject(theDC, theOld);
    DeleteObject(theBitmap);

    // A 1-bit DIB, as WON's masks are: the leftmost pixel is the high bit
    theMask.mHeader.biWidth = 8;
    theMask.mHeader.biHeight = 1;
    const DWORD kOneBit = 0xa0;                                          // pixels 0 and 2 white
    theBitmap = CreateDIBitmap(NULL, &theMask.mHeader, CBM_INIT, &kOneBit, (BITMAPINFO*)&theMask, DIB_RGB_COLORS);
    theOld = SelectObject(theDC, theBitmap);
    CHECK(GetPixel(theDC, 0, 0) == kWhite && GetPixel(theDC, 1, 0) == 0 && GetPixel(theDC, 2, 0) == kWhite &&
          GetPixel(theDC, 7, 0) == 0);
    SelectObject(theDC, theOld);
    DeleteObject(theBitmap);
    DeleteDC(theDC);
}

// Each named operation against its formula: P pattern (brush), S source, D destination
static void TestRasterOperations()
{
    const COLORREF kS = RGB(0x5a, 0xc3, 0x0f), kD = RGB(0x33, 0x99, 0xf0), kP = RGB(0x81, 0x18, 0x7e);
    struct {
        const char* mName;
        DWORD mOperation;
        DWORD mExpected;
    } kOperations[] = {
        {"SRCCOPY", SRCCOPY, kS},
        {"SRCAND", SRCAND, kS & kD},
        {"SRCPAINT", SRCPAINT, kS | kD},
        {"SRCINVERT", SRCINVERT, kS ^ kD},
        {"SRCERASE", SRCERASE, kS & ~kD},
        {"NOTSRCCOPY", NOTSRCCOPY, ~kS},
        {"NOTSRCERASE", NOTSRCERASE, ~(kS | kD)},
        {"MERGECOPY", MERGECOPY, kP & kS},
        {"MERGEPAINT", MERGEPAINT, ~kS | kD},
        {"PATCOPY", PATCOPY, kP},
        {"PATINVERT", PATINVERT, kP ^ kD},
        {"DSTINVERT", DSTINVERT, ~kD},
        {"BLACKNESS", BLACKNESS, 0},
        {"WHITENESS", WHITENESS, 0xffffff},
    };
    Canvas theSource(2, 2), theTarget(4, 4);
    SetPixel(theSource.mDC, 1, 1, kS);
    HBRUSH theBrush = CreateSolidBrush(kP);
    HGDIOBJ theOldBrush = SelectObject(theTarget.mDC, theBrush);
    for (const auto& theCase : kOperations) {
        SetPixel(theTarget.mDC, 2, 3, kD);
        SetPixel(theTarget.mDC, 3, 3, kD);
        CHECK(BitBlt(theTarget.mDC, 2, 3, 1, 1, theSource.mDC, 1, 1, theCase.mOperation));
        COLORREF theResult = GetPixel(theTarget.mDC, 2, 3);
        if (theResult != (theCase.mExpected & 0xffffff))
            fprintf(stderr, "  %s: %06lx, expected %06lx\n", theCase.mName, (unsigned long)theResult,
                    (unsigned long)(theCase.mExpected & 0xffffff));
        CHECK(theResult == (theCase.mExpected & 0xffffff));
        CHECK(GetPixel(theTarget.mDC, 3, 3) == kD);         // the pixel beside it stays
    }
    SelectObject(theTarget.mDC, theOldBrush);
    DeleteObject(theBrush);

    // WON's masked draw: AND with a white-is-transparent mask, then OR the image whose transparent pixels are black
    Canvas theMask(2, 1), theImage(2, 1), theScreen(2, 1);
    SetPixel(theMask.mDC, 0, 0, 0);                         // opaque
    SetPixel(theImage.mDC, 0, 0, kRed);
    SetPixel(theImage.mDC, 1, 0, 0);
    SetPixel(theScreen.mDC, 0, 0, kBlue);
    SetPixel(theScreen.mDC, 1, 0, kBlue);
    BitBlt(theScreen.mDC, 0, 0, 2, 1, theMask.mDC, 0, 0, SRCAND);
    BitBlt(theScreen.mDC, 0, 0, 2, 1, theImage.mDC, 0, 0, SRCPAINT);
    CHECK(GetPixel(theScreen.mDC, 0, 0) == kRed && GetPixel(theScreen.mDC, 1, 0) == kBlue);
}

static void TestShapes()
{
    Canvas theCanvas(12, 12);
    HPEN thePen = CreatePen(PS_SOLID, 0, kRed);
    HBRUSH theBrush = CreateSolidBrush(kBlue);
    HGDIOBJ theOldPen = SelectObject(theCanvas.mDC, thePen);
    HGDIOBJ theOldBrush = SelectObject(theCanvas.mDC, theBrush);
    // A 4x3 rectangle: 10 outline pixels and 2 inside; the right and bottom edges are outside
    CHECK(Rectangle(theCanvas.mDC, 2, 2, 6, 5));
    CHECK(theCanvas.Count(kRed) == 10 && theCanvas.Count(kBlue) == 2);
    CHECK(GetPixel(theCanvas.mDC, 5, 4) == kRed && GetPixel(theCanvas.mDC, 6, 4) == kWhite &&
          GetPixel(theCanvas.mDC, 3, 3) == kBlue);
    // Without a pen the brush fills one pixel less on the right and bottom: 3x2
    RECT theAll = {0, 0, 12, 12};
    FillRect(theCanvas.mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    SelectObject(theCanvas.mDC, GetStockObject(NULL_PEN));
    Rectangle(theCanvas.mDC, 2, 2, 6, 5);
    CHECK(theCanvas.Count(kBlue) == 6 && GetPixel(theCanvas.mDC, 5, 2) == kWhite);

    // A polygon with a square hole: the joining edges cancel, and the even-odd rule leaves the inner 2x2 empty
    FillRect(theCanvas.mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    const POINT kHoled[] = {{0, 0}, {10, 0}, {10, 10}, {0, 10}, {0, 0}, {4, 4}, {6, 4}, {6, 6}, {4, 6}, {4, 4}};
    CHECK(Polygon(theCanvas.mDC, kHoled, 10));
    CHECK(theCanvas.Count(kBlue) == 96 && GetPixel(theCanvas.mDC, 5, 5) == kWhite &&
          GetPixel(theCanvas.mDC, 3, 5) == kBlue && GetPixel(theCanvas.mDC, 10, 5) == kWhite);
    // A right triangle: the pixel centres with x + y + 1 < 10, so 9 + 8 + ... + 1 = 45 pixels
    FillRect(theCanvas.mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    const POINT kTriangle[] = {{0, 0}, {10, 0}, {0, 10}};
    Polygon(theCanvas.mDC, kTriangle, 3);
    CHECK(theCanvas.Count(kBlue) == 45);

    // Polyline leaves out each segment's last point; WON adds one more point to draw it
    FillRect(theCanvas.mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    SelectObject(theCanvas.mDC, thePen);
    const POINT kLine[] = {{1, 1}, {5, 1}};
    CHECK(Polyline(theCanvas.mDC, kLine, 2));
    CHECK(theCanvas.Count(kRed) == 4 && GetPixel(theCanvas.mDC, 5, 1) == kWhite);
    const POINT kWonLine[] = {{1, 3}, {5, 3}, {6, 3}};
    Polyline(theCanvas.mDC, kWonLine, 3);
    CHECK(GetPixel(theCanvas.mDC, 5, 3) == kRed && theCanvas.Count(kRed) == 9);

    // An ellipse in a 9x7 box: symmetric, inside the box, its centre filled, its outline in the pen
    FillRect(theCanvas.mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    CHECK(Ellipse(theCanvas.mDC, 1, 2, 10, 9));
    bool theSymmetric = true, theInside = true;
    for (int y = 0; y < 12; ++y)
        for (int x = 0; x < 12; ++x) {
            COLORREF c = GetPixel(theCanvas.mDC, x, y);
            if (c != kWhite && (x < 1 || x > 9 || y < 2 || y > 8))
                theInside = false;
            if (x >= 1 && x <= 9 && y >= 2 && y <= 8 && c != GetPixel(theCanvas.mDC, 10 - x, y))
                theSymmetric = false;
            if (x >= 1 && x <= 9 && y >= 2 && y <= 8 && c != GetPixel(theCanvas.mDC, x, 10 - y))
                theSymmetric = false;
        }
    CHECK(theSymmetric && theInside);
    CHECK(GetPixel(theCanvas.mDC, 5, 5) == kBlue && GetPixel(theCanvas.mDC, 1, 5) == kRed &&
          GetPixel(theCanvas.mDC, 5, 2) == kRed && GetPixel(theCanvas.mDC, 1, 2) == kWhite);
    // Its area is close to pi * 4.5 * 3.5 = 49.5
    int theArea = theCanvas.Count(kRed) + theCanvas.Count(kBlue);
    CHECK(theArea >= 45 && theArea <= 55);
    // An arc from the right counter-clockwise to the top draws the upper right quarter of the outline only
    FillRect(theCanvas.mDC, &theAll, (HBRUSH)GetStockObject(WHITE_BRUSH));
    CHECK(Arc(theCanvas.mDC, 1, 2, 10, 9, 20, 5, 5, -20));
    CHECK(GetPixel(theCanvas.mDC, 9, 5) == kRed && GetPixel(theCanvas.mDC, 5, 2) == kRed &&
          GetPixel(theCanvas.mDC, 1, 5) == kWhite && GetPixel(theCanvas.mDC, 5, 8) == kWhite &&
          theCanvas.Count(kBlue) == 0);

    SelectObject(theCanvas.mDC, theOldPen);
    SelectObject(theCanvas.mDC, theOldBrush);
    DeleteObject(thePen);
    DeleteObject(theBrush);
}

static void TestClipAndObjects()
{
    Canvas theCanvas(6, 6);
    HBRUSH theBrush = CreateSolidBrush(kRed);
    CHECK(IntersectClipRect(theCanvas.mDC, 1, 1, 4, 4) == SIMPLEREGION);
    CHECK(IntersectClipRect(theCanvas.mDC, 2, 2, 8, 8) == SIMPLEREGION);   // narrows to 2..4
    RECT theAll = {0, 0, 6, 6};
    FillRect(theCanvas.mDC, &theAll, theBrush);
    CHECK(theCanvas.Count(kRed) == 4 && GetPixel(theCanvas.mDC, 2, 2) == kRed);
    CHECK(SelectClipRgn(theCanvas.mDC, NULL) == SIMPLEREGION);
    FillRect(theCanvas.mDC, &theAll, theBrush);
    CHECK(theCanvas.Count(kRed) == 36);
    DeleteObject(theBrush);

    // A bitmap goes into one DC at a time, and a selected bitmap is not deleted
    HDC theOther = CreateCompatibleDC(NULL);
    CHECK(SelectObject(theOther, theCanvas.mBitmap) == NULL);
    CHECK(!DeleteObject(theCanvas.mBitmap));
    BITMAP theInfo;
    CHECK(GetObjectA(theCanvas.mBitmap, sizeof theInfo, &theInfo) == sizeof theInfo && theInfo.bmWidth == 6);
    // A pen deleted while selected lives until the DC lets it go
    HPEN thePen = CreatePen(PS_SOLID, 0, kBlue);
    HGDIOBJ theStockPen = SelectObject(theOther, thePen);
    CHECK(theStockPen == GetStockObject(BLACK_PEN) && GetCurrentObject(theOther, OBJ_PEN) == thePen);
    CHECK(DeleteObject(thePen));
    LOGPEN theLog;
    CHECK(GetObjectA(thePen, sizeof theLog, &theLog) == sizeof theLog && theLog.lopnColor == kBlue);
    SelectObject(theOther, theStockPen);
    CHECK(GetObjectA(thePen, sizeof theLog, &theLog) == 0);
    // Stock objects survive DeleteObject
    CHECK(DeleteObject(GetStockObject(BLACK_BRUSH)) && GetObjectA(GetStockObject(BLACK_BRUSH), 0, NULL) > 0);
    CHECK(GetDeviceCaps(theOther, LOGPIXELSY) == 96);
    POINT thePoint = {7, 9};
    CHECK(DPtoLP(theOther, &thePoint, 1) && thePoint.x == 7 && thePoint.y == 9);
    DeleteDC(theOther);
}

static void TestText()
{
    Canvas theCanvas(200, 40);
    HFONT theFont = CreateFontA(-16, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 0, 0, DEFAULT_QUALITY, 0, "Arial");
    SelectObject(theCanvas.mDC, theFont);
    TEXTMETRICA theMetrics;
    CHECK(GetTextMetricsA(theCanvas.mDC, &theMetrics));
    CHECK(theMetrics.tmHeight == theMetrics.tmAscent + theMetrics.tmDescent && theMetrics.tmHeight >= 16);
    CHECK(theMetrics.tmFirstChar == 0x1e && theMetrics.tmLastChar == 0xff && theMetrics.tmDefaultChar == 0x1f &&
          theMetrics.tmBreakChar == 0x20);
    // Extents add the advances, in the same widths GetCharWidthW gives; code page 1252 maps 0x80 to the euro sign
    INT theI, theW, theEuro;
    GetCharWidthW(theCanvas.mDC, 'i', 'i', &theI);
    GetCharWidthW(theCanvas.mDC, 'W', 'W', &theW);
    GetCharWidthW(theCanvas.mDC, 0x20ac, 0x20ac, &theEuro);
    SIZE theSize;
    CHECK(GetTextExtentPoint32A(theCanvas.mDC, "iWi\x80", 4, &theSize));
    CHECK(theSize.cx == 2 * theI + theW + theEuro && theSize.cy == theMetrics.tmHeight && theW > theI);
    const wchar_t kWide[] = L"iW";
    CHECK(GetTextExtentPoint32W(theCanvas.mDC, kWide, 2, &theSize) && theSize.cx == theI + theW);

    // Drawn text: black ink inside the cell; the transparent mode leaves the background, the opaque mode fills it
    SetBkMode(theCanvas.mDC, TRANSPARENT);
    CHECK(ExtTextOutA(theCanvas.mDC, 10, 5, 0, NULL, "Hi", 2, NULL));
    GetTextExtentPoint32A(theCanvas.mDC, "Hi", 2, &theSize);
    int theInk = 0, theOutside = 0;
    for (int y = 0; y < 40; ++y)
        for (int x = 0; x < 200; ++x)
            if (GetPixel(theCanvas.mDC, x, y) != kWhite) {
                ++theInk;
                if (x < 10 || x >= 10 + theSize.cx || y < 5 || y >= 5 + theSize.cy)
                    ++theOutside;
            }
    CHECK(theInk > 20 && theOutside == 0);
    CHECK(GetPixel(theCanvas.mDC, 10 + theSize.cx - 1, 5) == kWhite);       // above the 'i' dot is background
    SetBkMode(theCanvas.mDC, OPAQUE);
    SetBkColor(theCanvas.mDC, kBlue);
    SetTextColor(theCanvas.mDC, kRed);
    ExtTextOutA(theCanvas.mDC, 100, 5, 0, NULL, "Hi", 2, NULL);
    CHECK(GetPixel(theCanvas.mDC, 100 + theSize.cx - 1, 5) == kBlue && GetPixel(theCanvas.mDC, 100 + theSize.cx, 5) == kWhite);
    // ETO_CLIPPED keeps the text inside its rectangle
    RECT theClip = {150, 5, 152, 30};
    SetBkMode(theCanvas.mDC, TRANSPARENT);
    ExtTextOutA(theCanvas.mDC, 148, 5, ETO_CLIPPED, &theClip, "WWW", 3, NULL);
    int theClipped = 0;
    for (int y = 0; y < 40; ++y)
        for (int x = 140; x < 200; ++x)
            if (GetPixel(theCanvas.mDC, x, y) != kWhite && (x < 150 || x >= 152))
                ++theClipped;
    CHECK(theClipped == 0);
    SelectObject(theCanvas.mDC, GetStockObject(SYSTEM_FONT));
    DeleteObject(theFont);
}

static LRESULT CALLBACK Painted(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    if (inMessage == WM_PAINT) {
        PAINTSTRUCT thePaint;
        HDC theDC = BeginPaint(inWindow, &thePaint);
        HBRUSH theBrush = CreateSolidBrush(kRed);
        RECT theRect = {0, 0, 8, 8};
        FillRect(theDC, &theRect, theBrush);
        DeleteObject(theBrush);
        EndPaint(inWindow, &thePaint);
        return 0;
    }
    return DefWindowProcA(inWindow, inMessage, inWParam, inLParam);
}

static void TestWindows()
{
    // A window DC draws into the window's pixels; releasing it shows them in its native window
    WNDCLASSA theClass = {};
    theClass.lpfnWndProc = Painted;
    theClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    theClass.lpszClassName = "Painted";
    RegisterClassA(&theClass);
    HWND theWindow = CreateWindowExA(0, "Painted", "", WS_POPUP | WS_VISIBLE, 0, 0, 16, 16, NULL, NULL, NULL, NULL);
    MSG theMessage;
    int thePaints = 0;
    while (PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE)) {
        thePaints += theMessage.message == WM_PAINT;
        DispatchMessageA(&theMessage);
    }
    CHECK(thePaints == 1);
    HDC theDC = GetDC(theWindow);
    CHECK(GetPixel(theDC, 0, 0) == kRed && GetPixel(theDC, 12, 12) == kWhite);   // the erase used COLOR_WINDOW
    ReleaseDC(theWindow, theDC);
    SDL_Window* theNative = eeport::NativeWindow(theWindow, false);
    SDL_Surface* theSurface = theNative ? SDL_GetWindowSurface(theNative) : nullptr;
    CHECK(theSurface != nullptr);
    if (theSurface) {
        Uint8 r, g, b;
        SDL_GetRGB(((Uint32*)theSurface->pixels)[0], theSurface->format, &r, &g, &b);
        CHECK(r == 255 && g == 0 && b == 0);
    }
    // An OpenGL window belongs to the DirectDraw layer: no software window over it
    SDL_Window* theGL = eeport::NativeWindow(theWindow, true);
    CHECK(eeport::NativeWindow(theWindow, false) == NULL || theGL == NULL);
    DestroyWindow(theWindow);
}

static void TestAlphaBlend()
{
    HMODULE theModule = LoadLibraryA("MSIMG32.DLL");
    CHECK(theModule != NULL);
    typedef BOOL(WINAPI * Blend)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
    Blend theBlend = (Blend)(void*)GetProcAddress(theModule, "AlphaBlend");
    CHECK(theBlend != NULL && GetProcAddress(theModule, "GradientFill") == NULL);
    if (!theBlend)
        return;
    Canvas theSource(1, 1), theTarget(2, 1);
    SetPixel(theSource.mDC, 0, 0, RGB(200, 100, 0));
    SetPixel(theTarget.mDC, 0, 0, RGB(0, 100, 200));
    BLENDFUNCTION theFunction = {AC_SRC_OVER, 0, 64, 0};
    // A quarter of the source: (200 * 64 + 0 * 191 + 127) / 255 = 50, 100, (0 + 200 * 191 + 127) / 255 = 150
    CHECK(theBlend(theTarget.mDC, 0, 0, 1, 1, theSource.mDC, 0, 0, 1, 1, theFunction));
    CHECK(GetPixel(theTarget.mDC, 0, 0) == RGB(50, 100, 150) && GetPixel(theTarget.mDC, 1, 0) == kWhite);
    FreeLibrary(theModule);
}

int main()
{
    setenv("SDL_VIDEODRIVER", "offscreen", 1);
    CHECK(eeport::StartVideo());
    TestPixelsAndFormats();
    TestDIBs();
    TestRasterOperations();
    TestShapes();
    TestClipAndObjects();
    TestText();
    TestWindows();
    TestAlphaBlend();
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
