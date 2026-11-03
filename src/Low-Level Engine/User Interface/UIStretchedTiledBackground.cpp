// C:\EEX\Low-Level Engine\User Interface\UIStretchedTiledBackground.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1007e312..0x1007e47c: ctor, dtor, DrawBackground. No key chains or COMDATs.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/UIBitmapBackground.h"
#include "lle/UITiledBackground.h"
#include "lle/UIStretchedTiledBackground.h"

// FUNCTION: LLE 0x1007e312
// Creates a background tiled a given number of times across and down, stretching the tiles to fit
UIStretchedTiledBackground::UIStretchedTiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                                                       const GETextureDescription& inDescription,
                                                       const GERectangle& inTextureRectangle, float inHorizontalCount,
                                                       float inVerticalCount, const UILayoutUtility& inLayout,
                                                       UIForm* inForm, unsigned long inDrawPriority, bool inVisible)
    : UITiledBackground(inBackgroundID, inTextureFileName, inDescription, inTextureRectangle, inLayout, inForm,
                        inDrawPriority, inVisible)
{
    this->mVerticalCount = inVerticalCount;
    this->mHorizontalCount = inHorizontalCount;
}

// FUNCTION: LLE 0x1007e351
// Destroys the background (members only)
UIStretchedTiledBackground::~UIStretchedTiledBackground()
{
}

// FUNCTION: LLE 0x1007e35c
// Draws the stretched tiles that overlap the given rectangle; returns the draw result (0 when outside)
long UIStretchedTiledBackground::DrawBackground(const GERectangle& inRectangle)
{
    GERectangle theClip;
    long theResult = 0;
    if (inRectangle.And(this->mRectangle, theClip)) {
        float theWidth = this->mRectangle.GetWidth() / this->mHorizontalCount;
        float theHeight = this->mRectangle.GetHeight() / this->mVerticalCount;
        long theFirstRow = UMFloor((theClip.GetTop() - this->mRectangle.GetTop()) / (theHeight + 1.0f));
        long theLastRow = UMFloor((theClip.GetBottom() - this->mRectangle.GetTop()) / (theHeight + 1.0f));
        long theFirstColumn = UMFloor((theClip.GetLeft() - this->mRectangle.GetLeft()) / (theWidth + 1.0f));
        long theLastColumn = UMFloor((theClip.GetRight() - this->mRectangle.GetLeft()) / (theWidth + 1.0f));
        theResult = this->DrawTiledBackground(theClip, theFirstRow, theLastRow, theFirstColumn, theLastColumn, theWidth,
                                        theHeight);
    }
    return theResult;
}
