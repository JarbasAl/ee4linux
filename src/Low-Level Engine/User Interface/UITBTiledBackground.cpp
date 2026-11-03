// C:\EEX\Low-Level Engine\User Interface\UITBTiledBackground.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1007e47c..0x1007e579: ctor, dtor, DrawBackground. No key chains or COMDATs. UITextLabel.cpp starts at 0x1007e579.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/UIBitmapBackground.h"
#include "lle/UITiledBackground.h"
#include "lle/UITBTiledBackground.h"

// FUNCTION: LLE 0x1007e47c
// Creates a background tiled from top to bottom
UITBTiledBackground::UITBTiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                                         const GETextureDescription& inDescription,
                                         const GERectangle& inTextureRectangle, const UILayoutUtility& inLayout,
                                         UIForm* inForm, unsigned long inDrawPriority, bool inVisible)
    : UITiledBackground(inBackgroundID, inTextureFileName, inDescription, inTextureRectangle, inLayout, inForm,
                        inDrawPriority, inVisible)
{
}

// FUNCTION: LLE 0x1007e4ac
// Destroys the background (members only)
UITBTiledBackground::~UITBTiledBackground()
{
}

// FUNCTION: LLE 0x1007e4b7
// Draws the full-width tile rows that overlap the given rectangle; returns the draw result (0 when outside)
long UITBTiledBackground::DrawBackground(const GERectangle& inRectangle)
{
    GERectangle theClip;
    long theResult = 0;
    if (inRectangle.And(this->mRectangle, theClip)) {
        float theWidth = this->mRectangle.GetWidth();
        float theHeight = this->mTextureRectangle.GetHeight() - 1.0f;
        long theFirstRow = UMFloor((theClip.GetTop() - this->mRectangle.GetTop()) / theHeight);
        long theLastRow = UMFloor((theClip.GetBottom() - this->mRectangle.GetTop()) / theHeight);
        theResult = this->DrawTiledBackground(theClip, theFirstRow, theLastRow, 0, 0, theWidth, theHeight);
    }
    return theResult;
}
