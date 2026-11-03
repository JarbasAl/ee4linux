// C:\EEX\Low-Level Engine\User Interface\UITiledBackground.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1007fb46..0x1007fd7d: ctor, dtor, DrawBackground, DrawTiledBackground. No key chains or COMDATs.
// UITLScrolling.cpp's ctor starts at 0x1007fd7d.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/UIBitmapBackground.h"
#include "lle/UITiledBackground.h"

// FUNCTION: LLE 0x1007fb46
// Creates a tiled bitmap background from a texture file and layout
UITiledBackground::UITiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                                     const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                                     const UILayoutUtility& inLayout, UIForm* inForm, unsigned long inDrawPriority,
                                     bool inVisible)
    : UIBitmapBackground(inBackgroundID, inTextureFileName, inDescription, inTextureRectangle, inLayout, inForm,
                         inDrawPriority, inVisible)
{
}

// FUNCTION: LLE 0x1007fb76
// Destroys the tiled background (base class only)
UITiledBackground::~UITiledBackground()
{
}

// FUNCTION: LLE 0x1007fb81
// Draws the tiles that cover the part of the given rectangle inside the background; returns 0 when none overlap
long UITiledBackground::DrawBackground(const GERectangle& inRectangle)
{
    GERectangle theClip;
    long theResult = 0;
    if (inRectangle.And(this->mRectangle, theClip)) {
        float theWidth = this->mTextureRectangle.GetWidth() - 1.0f;
        float theHeight = this->mTextureRectangle.GetHeight() - 1.0f;
        long theFirstRow = UMFloor((theClip.GetTop() - this->mRectangle.GetTop()) / theHeight);
        long theLastRow = UMFloor((theClip.GetBottom() - this->mRectangle.GetTop()) / theHeight);
        long theFirstColumn = UMFloor((theClip.GetLeft() - this->mRectangle.GetLeft()) / theWidth);
        long theLastColumn = UMFloor((theClip.GetRight() - this->mRectangle.GetLeft()) / theWidth);
        theResult = this->DrawTiledBackground(theClip, theFirstRow, theLastRow, theFirstColumn, theLastColumn, theWidth,
                                        theHeight);
    }
    return theResult;
}

// FUNCTION: LLE 0x1007fc8d
// Draws each tile of the given row and column range stretched and clipped to the clip rectangle
long UITiledBackground::DrawTiledBackground(const GERectangle& inClip, unsigned long inFirstRow,
                                            unsigned long inLastRow, unsigned long inFirstColumn,
                                            unsigned long inLastColumn, float inWidth, float inHeight)
{
    long theResult = 0;
    GERectangle theTile;
    GERectangle theClippedTile;
    for (unsigned long theRow = inFirstRow; theRow <= inLastRow; theRow++) {
        for (unsigned long theColumn = inFirstColumn; theColumn <= inLastColumn; theColumn++) {
            GE2DPoint thePosition(theColumn * (inWidth + 1.0f) + this->mRectangle.GetLeft(),
                                  this->mRectangle.GetTop() + (inHeight + 1.0f) * theRow);
            theTile.SetTopLeft(thePosition);
            theTile.SetRight(thePosition.GetX() + inWidth);
            theTile.SetBottom(thePosition.GetY() + inHeight);
            theTile.RoundToNearest();
            if (theTile.And(inClip, theClippedTile)) {
                theClippedTile.RoundToNearest();
                theResult = this->DrawStrectchedBackground(theTile, theClippedTile);
            }
        }
    }
    return theResult;
}
