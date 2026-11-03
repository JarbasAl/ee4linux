// C:\EEX\Low-Level Engine\Graphics Engine\GEFont.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x100151b7..0x100152e4: the ctor and the dtor (the accessors are header-inline, emitted by GEBoundingBox.cpp).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/GE2DPoint.h"
#include "lle/GERectangle.h"
#include "lle/TSScheduler.h"
#include "lle/UReferenceCounter.h"
#include "lle/GEDeviceContext.h"
#include "lle/GEFont.h"

// FUNCTION: LLE 0x100151b7
// Creates a Windows font (name, pixel size, bold/italic/underline style, quality) and measures its height
GEFont::GEFont(const UString& inName, unsigned long inSize, unsigned long inStyle, unsigned long inQuality)
    : mName(inName)
{
    unsigned long theQuality = DEFAULT_QUALITY;
    long theHeight = 0;
    HDC theContext;
    this->mSize = inSize;
    this->mStyle = inStyle;
    this->mQuality = inQuality;
    switch (inQuality) {
    case kFontQualityDefault:
        theQuality = DEFAULT_QUALITY;
        break;
    case kFontQualityDraft:
        theQuality = DRAFT_QUALITY;
        break;
    case kFontQualityAntialiased:
        theQuality = ANTIALIASED_QUALITY;
        break;
    }
    this->mFont = CreateFont(-(long)inSize, 0, 0, 0, (inStyle & kFontStyleBold) ? FW_BOLD : FW_NORMAL, (inStyle >> 1) & 1,
                       (inStyle >> 2) & 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, theQuality,
                       DEFAULT_PITCH, this->mName);
    if (this->mFont != NULL) {
        theContext = GEDeviceContext::GetDeviceContext(NULL);
        if (theContext != NULL) {
            HGDIOBJ theOldFont = SelectObject(theContext, this->mFont);
            if (theOldFont != NULL) {
                TEXTMETRIC theMetrics;
                if (GetTextMetrics(theContext, &theMetrics)) {
                    theHeight = theMetrics.tmHeight;
                    SelectObject(theContext, theOldFont);
                } else
                    GetLastError();
            }
        }
        GEDeviceContext::ReleaseDeviceContext(NULL, theContext);
    }
    this->mFontBitmap = NULL;
    this->mHeight = theHeight;
}

// FUNCTION: LLE 0x100152b6
// Destroys the font, deleting the Windows font and releasing its glyph bitmap
GEFont::~GEFont()
{
    DeleteObject(this->mFont);
    if (this->mFontBitmap)
        this->mFontBitmap->Dereference();
}
