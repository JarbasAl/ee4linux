// C:\EEX\Low-Level Engine\User Interface\UILRTiledBackground.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10078a70..0x10078c81: UITiledBackground's implicit copy members and ??_E (first includer of UITiledBackground.h),
// then its own implicit copy ctor and ??_E, the ctor, dtor and DrawBackground. No key chains. UIManager.cpp's key
// chains start at 0x10078c81.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/UIBitmapBackground.h"
#include "lle/UITiledBackground.h"
#include "lle/UILRTiledBackground.h"

// FUNCTION: LLE 0x10078b83
// Creates a left-to-right tiled background (the texture repeats across its layout rectangle)
UILRTiledBackground::UILRTiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                                         const GETextureDescription& inDescription,
                                         const GERectangle& inTextureRectangle, const UILayoutUtility& inLayout,
                                         UIForm* inForm, unsigned long inDrawPriority, bool inVisible)
    : UITiledBackground(inBackgroundID, inTextureFileName, inDescription, inTextureRectangle, inLayout, inForm,
                        inDrawPriority, inVisible)
{
}

// FUNCTION: LLE 0x10078bb3
// Destroys the background (the base class does the work)
UILRTiledBackground::~UILRTiledBackground()
{
}

// FUNCTION: LLE 0x10078bbe
// Redraws the tiles of its single row that overlap the rectangle
long UILRTiledBackground::DrawBackground(const GERectangle& inRectangle)
{
    GERectangle theClip;
    long theResult = 0;
    if (inRectangle.And(this->mRectangle, theClip)) {
        float theWidth = this->mTextureRectangle.GetWidth() - 1.0f;
        float theHeight = this->mRectangle.GetHeight();
        long theFirstColumn = UMFloor((theClip.GetLeft() - this->mRectangle.GetLeft()) / theWidth);
        long theLastColumn = UMFloor((theClip.GetRight() - this->mRectangle.GetLeft()) / theWidth);
        theResult = this->DrawTiledBackground(theClip, 0, 0, theFirstColumn, theLastColumn, theWidth, theHeight);
    }
    return theResult;
}

// Implicit copy members and ??_E of UITiledBackground and UILRTiledBackground
// FUNCTION: LLE 0x10078a70
// FUNCTION: LLE 0x10078a91
// SYNTHETIC: LLE 0x10078aac
// FUNCTION: LLE 0x10078b07
// SYNTHETIC: LLE 0x10078b28
