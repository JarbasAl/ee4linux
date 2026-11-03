// C:\EEX\Low-Level Engine\User Interface\UIScalableButton.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1007c38e..0x1007c901: the U2D key init chains, the ctor (its ??_G follows: the class is not exported, so no ??_E
// and no implicit copy members), then CopyData, DrawPolygons and SetLocation. UIScrollBar.cpp's key chains start at
// 0x1007c901.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/GEBitmap.h"
#include "lle/GERhombus.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/U2DRectangle.h"
#include "lle/GERasterizer.h"
#include "lle/TSCriticalSection.h"
#include "lle/UILayoutUtility.h"
#include "lle/UIEntity.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIControl.h"
#include "lle/UIButton.h"
#include "lle/UIScalableButton.h"

// FUNCTION: LLE 0x1007c4ae
// Creates a scalable button from source rectangles for its normal, focused, pressed and text parts
UIScalableButton::UIScalableButton(unsigned long inId, GEBitmap* inNormalBitmap, GEBitmap* inFocusedBitmap,
                                   GEBitmap* inPressedBitmap, const GERectangle& inNormalRectangle,
                                   const GERectangle& inFocusedRectangle, const GERectangle& inPressedRectangle,
                                   const GERectangle& inTextRectangle, const GERectangle& inRectangle,
                                   UIForm* inForm, unsigned long inIndex)
    : UIButton(inId, inNormalBitmap, inFocusedBitmap, inPressedBitmap, inRectangle, inForm, inIndex, 0)
{
    this->mNormalRectangle = inNormalRectangle;
    this->mFocusedRectangle = inFocusedRectangle;
    this->mPressedRectangle = inPressedRectangle;
    this->mTextRectangle = inTextRectangle;
}

// FUNCTION: LLE 0x1007c609
// Picks the source rectangle for the button state (focused falls back to normal without a focus bitmap), then snapshots
long UIScalableButton::CopyData()
{
    switch (this->mState) {
    case kButtonStateNormal:
        this->mCopiedRectangle = this->mNormalRectangle;
        break;
    case kButtonStateFocused:
        if (this->mFocusedBitmap)
            this->mCopiedRectangle = this->mFocusedRectangle;
        else
            this->mCopiedRectangle = this->mNormalRectangle;
        break;
    case kButtonStatePressed:
        this->mCopiedRectangle = this->mPressedRectangle;
        break;
    }
    return UIButton::CopyData();
}

// FUNCTION: LLE 0x1007c65b
// Draws the button as a left cap, a right cap and a stretched middle cut from its bitmap; returns the last draw result
long UIScalableButton::DrawPolygons()
{
    GERectangle theSource;
    GERectangle theDestination;
    long theResult = kErrorOK;
    if (this->mCopiedBitmap) {
        // left cap
        theSource.SetLeft(0.0f);
        theSource.SetTop(this->mCopiedRectangle.GetTop());
        theSource.SetRight(this->mCopiedRectangle.GetLeft() + 1.0f);
        theSource.SetBottom(this->mCopiedRectangle.GetBottom());
        theDestination.SetLeft(this->mCopiedDrawRectangle.GetLeft());
        theDestination.SetRight(theDestination.GetLeft() + theSource.GetWidth() - 1.0f);
        theDestination.SetTop(this->mCopiedDrawRectangle.GetTop());
        theDestination.SetBottom(this->mCopiedDrawRectangle.GetBottom());
        this->mCopiedBitmap->ComputeStretchedCoordsAndScale(theDestination, theDestination, theSource, this->GetScreenIndex());
        this->mCopiedBitmap->SetLocation(this->mCopiedDrawRectangle.GetTopLeft());
        theResult = this->GetRasterizer()->Draw(this->mCopiedBitmap, this->GetScreenIndex());

        // right cap
        theSource.SetLeft(this->mCopiedRectangle.GetRight() + 1.0f);
        theSource.SetTop(this->mCopiedRectangle.GetTop());
        theSource.SetRight((float)this->mCopiedBitmap->GetTexture(this->GetScreenIndex())->GetWidth());
        theSource.SetBottom(this->mCopiedRectangle.GetBottom());
        theDestination.SetLeft(this->mCopiedDrawRectangle.GetRight() - theSource.GetWidth() + 1.0f);
        theDestination.SetRight(this->mCopiedDrawRectangle.GetRight());
        this->mCopiedBitmap->ComputeStretchedCoordsAndScale(theDestination, theDestination, theSource, this->GetScreenIndex());
        this->mCopiedBitmap->SetLocation(theDestination.GetTopLeft());
        theResult = this->GetRasterizer()->Draw(this->mCopiedBitmap, this->GetScreenIndex());

        // stretched middle
        theSource = this->mCopiedRectangle;
        theDestination.SetLeft(this->mCopiedDrawRectangle.GetLeft() + theSource.GetLeft() + 1.0f);
        theDestination.SetRight(this->mCopiedDrawRectangle.GetRight()
                                - (this->mCopiedBitmap->GetTexture(this->GetScreenIndex())->GetWidth() - theSource.GetRight())
                                + 1.0f);
        this->mCopiedBitmap->ComputeStretchedCoordsAndScale(theDestination, theDestination, theSource, this->GetScreenIndex());
        this->mCopiedBitmap->SetLocation(theDestination.GetTopLeft());
        theResult = this->GetRasterizer()->Draw(this->mCopiedBitmap, this->GetScreenIndex());
    }
    return theResult;
}

// FUNCTION: LLE 0x1007c8f5
// Moves the button (plain button behaviour)
void UIScalableButton::SetLocation(const GE2DPoint& inLocation)
{
    UIButton::SetLocation(inLocation);
}

// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x1007c5e7
// SYNTHETIC: LLE 0x1007c38e
// SYNTHETIC: LLE 0x1007c398
// SYNTHETIC: LLE 0x1007c3a8
// SYNTHETIC: LLE 0x1007c3b4
// SYNTHETIC: LLE 0x1007c3be
// SYNTHETIC: LLE 0x1007c3c8
// SYNTHETIC: LLE 0x1007c3d8
// SYNTHETIC: LLE 0x1007c3e4
// SYNTHETIC: LLE 0x1007c3ee
// SYNTHETIC: LLE 0x1007c3f8
// SYNTHETIC: LLE 0x1007c408
// SYNTHETIC: LLE 0x1007c414
// SYNTHETIC: LLE 0x1007c41e
// SYNTHETIC: LLE 0x1007c428
// SYNTHETIC: LLE 0x1007c438
// SYNTHETIC: LLE 0x1007c444
// SYNTHETIC: LLE 0x1007c44e
// SYNTHETIC: LLE 0x1007c458
// SYNTHETIC: LLE 0x1007c468
// SYNTHETIC: LLE 0x1007c474
// SYNTHETIC: LLE 0x1007c47e
// SYNTHETIC: LLE 0x1007c488
// SYNTHETIC: LLE 0x1007c498
// SYNTHETIC: LLE 0x1007c4a4
