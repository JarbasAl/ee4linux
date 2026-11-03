// C:\EEX\Low-Level Engine\User Interface\UIRadioButton.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1007badf..0x1007c38e: the U2D key init chains, the ctor, then the out-of-line members in alphabetical order. The
// header-inline members (and the implicit copy members, ??_E) are emitted first by UICheckBox.cpp (0x1005fc41..).
// The next unit (0x1007c38e..0x1007c901) is UIScalableButton.cpp (not exported; RTTI .?AVUIScalableButton@@).
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
#include "lle/UIScrollBar.h"
#include "lle/UITextLabel.h"
#include "lle/UIRadioButton.h"
#include "lle/UIFormManager.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"

// FUNCTION: LLE 0x1007bbff
// Creates a radio button with the shared radio bitmaps (checked ones when checked), 19x19 unless scaled, and a label
UIRadioButton::UIRadioButton(unsigned long inId, const UWideString& inText, GEFont* inFont,
                             const GERectangle& inRectangle, UIForm* inForm, unsigned long inIndex,
                             unsigned long inBorderThickness, bool inIsPushButton, bool inIsChecked,
                             bool inScaleBitmap)
    : UIButton(inId, inForm->GetManager()->GetRadioOff(), NULL, inForm->GetManager()->GetRadioDownOff(),
               GERectangle(inRectangle.GetTopLeft(),
                           GE2DPoint(!inScaleBitmap && inRectangle.GetWidth() <= kScrollBarWidth
                                         ? inRectangle.GetLeft() + 19.0f
                                         : inRectangle.GetRight(),
                                     !inScaleBitmap && inRectangle.GetHeight() <= kScrollBarWidth
                                         ? inRectangle.GetTop() + 19.0f
                                         : inRectangle.GetBottom())),
               inForm, inIndex, inBorderThickness)
{
    this->mTextLabel = NULL;
    this->mHasText = false;
    this->mIsPushButton = inIsPushButton;
    this->mIsChecked = inIsChecked;
    this->mScaleBitmap = inScaleBitmap;
    this->mOtherNormalBitmap = this->mForm->GetManager()->GetRadioOn();
    this->mOtherNormalBitmap->Reference();
    this->mOtherPressedBitmap = this->mForm->GetManager()->GetRadioDownOn();
    this->mOtherPressedBitmap->Reference();
    if (this->mIsChecked) {
        GEBitmap* theNormalBitmap = this->mNormalBitmap;
        if (theNormalBitmap)
            theNormalBitmap->Reference();
        GEBitmap* thePressedBitmap = this->mPressedBitmap;
        if (thePressedBitmap)
            thePressedBitmap->Reference();
        this->SetNormalBitmap(this->mOtherNormalBitmap);
        this->SetPressedBitmap(this->mOtherPressedBitmap);
        this->mOtherNormalBitmap->Dereference();
        this->mOtherNormalBitmap = theNormalBitmap;
        this->mOtherNormalBitmap->Reference();
        this->mOtherPressedBitmap->Dereference();
        this->mOtherPressedBitmap = thePressedBitmap;
        this->mOtherPressedBitmap->Reference();
        if (theNormalBitmap)
            theNormalBitmap->Dereference();
        if (thePressedBitmap)
            thePressedBitmap->Dereference();
    }
    if (!this->mScaleBitmap) {
        float theHeight = inRectangle.GetHeight();
        if (theHeight > kScrollBarWidth)
            this->SetBitmapDrawOffset(GE2DPoint(0.0f, (theHeight - 2 * inBorderThickness) * 0.5f - 10.0f));
    }
    this->SetText(inText, inFont);
}

// FUNCTION: LLE 0x1007be03
// Checks the button: unchecks its checked neighbour, swaps to the checked bitmaps and fires the checked trigger
void UIRadioButton::Check()
{
    if (this->mIsChecked)
        return;
    this->RemoveOtherChecks();
    this->mIsChecked = true;
    GEBitmap* theNormalBitmap = this->mNormalBitmap;
    if (theNormalBitmap)
        theNormalBitmap->Reference();
    GEBitmap* thePressedBitmap = this->mPressedBitmap;
    if (thePressedBitmap)
        thePressedBitmap->Reference();
    this->SetNormalBitmap(this->mOtherNormalBitmap);
    this->SetPressedBitmap(this->mOtherPressedBitmap);
    if (this->mOtherNormalBitmap)
        this->mOtherNormalBitmap->Dereference();
    this->mOtherNormalBitmap = theNormalBitmap;
    if (this->mOtherNormalBitmap)
        this->mOtherNormalBitmap->Reference();
    if (this->mOtherPressedBitmap)
        this->mOtherPressedBitmap->Dereference();
    this->mOtherPressedBitmap = thePressedBitmap;
    if (this->mOtherPressedBitmap)
        this->mOtherPressedBitmap->Reference();
    this->SetDirty(true);
    if (theNormalBitmap)
        theNormalBitmap->Dereference();
    if (thePressedBitmap)
        thePressedBitmap->Dereference();
    this->mBorderType = 0;
    if (this->mIsPushButton)
        this->mState = kButtonStatePressed;
    this->FireTriggerEvent(kControlTriggerChecked);
}

// FUNCTION: LLE 0x1007bed5
// Snapshots the button and its label (deleted once its text is cleared); a checked push button loses its border
long UIRadioButton::CopyData()
{
    if (this->mIsChecked && this->mIsPushButton)
        this->mBorderType = 0;
    long theResult = UIButton::CopyData();
    if (this->mHasText)
        theResult = this->mTextLabel->CopyData();
    else if (this->mTextLabel) {
        delete this->mTextLabel;
        this->mTextLabel = NULL;
    }
    return theResult;
}

// FUNCTION: LLE 0x1007bf2d
// Draws the button (its bitmap scaled to the control height when scaled), then its label
long UIRadioButton::DrawPolygons()
{
    long theResult;
    if (this->mScaleBitmap && this->mCopiedBitmap) {
        float theXScale = this->mCopiedBitmap->GetXScale();
        float theYScale = this->mCopiedBitmap->GetYScale();
        this->mCopiedBitmap->SetXYScale(theXScale * (this->GetCopiedDrawRectangle().GetHeight() * 0.05f),
                                  theYScale * (this->GetCopiedDrawRectangle().GetHeight() * 0.05f));
        theResult = UIButton::DrawPolygons();
        this->mCopiedBitmap->SetXYScale(theXScale, theYScale);
    } else
        theResult = UIButton::DrawPolygons();
    if (this->mTextLabel)
        theResult = this->mTextLabel->DrawPolygons();
    return theResult;
}

// FUNCTION: LLE 0x1007bfbf
// Draws the button's text and its label
long UIRadioButton::DrawTextA()
{
    long theResult = UIButton::DrawTextA();
    if (this->mTextLabel)
        theResult = this->mTextLabel->DrawTextA();
    return theResult;
}

// FUNCTION: LLE 0x1007bfd8
// Presses the button, except a checked push button
long UIRadioButton::MouseLButtonDown()
{
    long theResult = kErrorOK;
    if (!this->mIsPushButton || !this->mIsChecked)
        theResult = UIButton::MouseLButtonDown();
    return theResult;
}

// FUNCTION: LLE 0x1007bff0
// Releases the button (plain button behaviour)
long UIRadioButton::MouseLButtonUp()
{
    return UIButton::MouseLButtonUp();
}

// FUNCTION: LLE 0x1007bff5
// Unchecks the first checked radio button among the adjacent radio controls before, then after, this one
void UIRadioButton::RemoveOtherChecks()
{
    long i;
    UIControl* theControl;
    for (i = this->GetIndex() - 1; i >= 0; i--) {
        theControl = this->mForm->GetControl(i);
        if (!theControl || theControl->GetType() != kControlTypeRadioButton)
            break;
        if (static_cast<UIRadioButton*>(theControl)->mIsChecked) {
            static_cast<UIRadioButton*>(theControl)->UnCheck();
            return;
        }
    }
    for (i = this->GetIndex() + 1; i < (long)this->mForm->GetControlCount(); i++) {
        theControl = this->mForm->GetControl(i);
        if (!theControl || theControl->GetType() != kControlTypeRadioButton)
            break;
        if (static_cast<UIRadioButton*>(theControl)->mIsChecked) {
            static_cast<UIRadioButton*>(theControl)->UnCheck();
            return;
        }
    }
}

// FUNCTION: LLE 0x1007c066
// Sets the pressed bitmap shown while checked
void UIRadioButton::SetDownCheckedBitmap(GEBitmap* inBitmap)
{
    if (this->mIsChecked) {
        if (this->mPressedBitmap)
            this->mPressedBitmap->Dereference();
        this->mPressedBitmap = inBitmap;
        if (this->mPressedBitmap)
            this->mPressedBitmap->Reference();
    } else {
        if (this->mOtherPressedBitmap)
            this->mOtherPressedBitmap->Dereference();
        this->mOtherPressedBitmap = inBitmap;
        if (this->mOtherPressedBitmap)
            this->mOtherPressedBitmap->Reference();
    }
}

// FUNCTION: LLE 0x1007c098
// Sets the pressed bitmap shown while unchecked
void UIRadioButton::SetDownUncheckedBitmap(GEBitmap* inBitmap)
{
    if (this->mIsChecked) {
        if (this->mOtherPressedBitmap)
            this->mOtherPressedBitmap->Dereference();
        this->mOtherPressedBitmap = inBitmap;
        if (this->mOtherPressedBitmap)
            this->mOtherPressedBitmap->Reference();
    } else {
        if (this->mPressedBitmap)
            this->mPressedBitmap->Dereference();
        this->mPressedBitmap = inBitmap;
        if (this->mPressedBitmap)
            this->mPressedBitmap->Reference();
    }
}

// FUNCTION: LLE 0x1007c0ca
// Moves the button, keeping its label to the right of the bitmap
void UIRadioButton::SetLocation(const GE2DPoint& inLocation)
{
    GE2DPoint theLocation;
    UIControl::SetLocation(inLocation);
    if (this->mTextLabel == NULL)
        return;
    if (this->mScaleBitmap)
        theLocation.SetX(this->mRectangle.GetHeight() + this->mRectangle.GetLeft() + this->GetBorderThickness());
    else
        theLocation.SetX(this->mRectangle.GetLeft() + this->GetBorderThickness() + kScrollBarWidth);
    theLocation.SetY(this->mRectangle.GetTop() + this->GetBorderThickness());
    this->mTextLabel->SetLocation(theLocation);
}

// FUNCTION: LLE 0x1007c146
// Sets the label text, creating the label right of the bitmap on first use; empty text hides the label
void UIRadioButton::SetText(const UWideString& inText, GEFont* inFont)
{
    if (inText.GetLength() > 0) {
        if (this->mTextLabel) {
            this->mTextLabel->SetText(inText, true);
            this->SetDirty(true);
        } else {
            float theLeft;
            if (this->mScaleBitmap == true)
                theLeft = this->mRectangle.GetHeight() + this->mRectangle.GetLeft() + this->GetBorderThickness();
            else
                theLeft = this->mRectangle.GetLeft() + this->GetBorderThickness() + kScrollBarWidth;
            this->mTextLabel = new UITextLabel(inText, inFont, this->GetId(),
                                         GERectangle(theLeft, this->mRectangle.GetTop() + this->GetBorderThickness(),
                                                     this->mRectangle.GetWidth() + this->mRectangle.GetLeft() -
                                                         this->GetBorderThickness(),
                                                     this->mRectangle.GetHeight() + this->mRectangle.GetTop() -
                                                         this->GetBorderThickness()),
                                         this->GetForm(), 0, this->GetIndex(), 0);
        }
        this->mHasText = true;
        this->SetDirty(true);
    } else
        this->mHasText = false;
}

// FUNCTION: LLE 0x1007c25d
// Sets the normal bitmap shown while checked
void UIRadioButton::SetUpCheckedBitmap(GEBitmap* inBitmap)
{
    if (this->mIsChecked) {
        if (this->mNormalBitmap)
            this->mNormalBitmap->Dereference();
        this->mNormalBitmap = inBitmap;
        if (this->mNormalBitmap)
            this->mNormalBitmap->Reference();
    } else {
        if (this->mOtherNormalBitmap)
            this->mOtherNormalBitmap->Dereference();
        this->mOtherNormalBitmap = inBitmap;
        if (this->mOtherNormalBitmap)
            this->mOtherNormalBitmap->Reference();
    }
}

// FUNCTION: LLE 0x1007c28f
// Sets the normal bitmap shown while unchecked
void UIRadioButton::SetUpUncheckedBitmap(GEBitmap* inBitmap)
{
    if (this->mIsChecked) {
        if (this->mOtherNormalBitmap)
            this->mOtherNormalBitmap->Dereference();
        this->mOtherNormalBitmap = inBitmap;
        if (this->mOtherNormalBitmap)
            this->mOtherNormalBitmap->Reference();
    } else {
        if (this->mNormalBitmap)
            this->mNormalBitmap->Dereference();
        this->mNormalBitmap = inBitmap;
        if (this->mNormalBitmap)
            this->mNormalBitmap->Reference();
    }
}

// FUNCTION: LLE 0x1007c2c1
// Unchecks the button: swaps to the unchecked bitmaps, restores its border and fires the unchecked trigger
void UIRadioButton::UnCheck()
{
    if (!this->mIsChecked)
        return;
    this->mIsChecked = false;
    GEBitmap* theNormalBitmap = this->mNormalBitmap;
    if (theNormalBitmap)
        theNormalBitmap->Reference();
    GEBitmap* thePressedBitmap = this->mPressedBitmap;
    if (thePressedBitmap)
        thePressedBitmap->Reference();
    this->SetNormalBitmap(this->mOtherNormalBitmap);
    this->SetPressedBitmap(this->mOtherPressedBitmap);
    if (this->mOtherNormalBitmap)
        this->mOtherNormalBitmap->Dereference();
    this->mOtherNormalBitmap = theNormalBitmap;
    if (this->mOtherNormalBitmap)
        this->mOtherNormalBitmap->Reference();
    if (this->mOtherPressedBitmap)
        this->mOtherPressedBitmap->Dereference();
    this->mOtherPressedBitmap = thePressedBitmap;
    if (this->mOtherPressedBitmap)
        this->mOtherPressedBitmap->Reference();
    this->SetDirty(true);
    if (theNormalBitmap)
        theNormalBitmap->Dereference();
    if (thePressedBitmap)
        thePressedBitmap->Dereference();
    if (this->mIsPushButton)
        this->mState = kButtonStateNormal;
    this->mBorderType = 1;
    this->FireTriggerEvent(kControlTriggerUnchecked);
}

// Compiler-generated: the U2D key init chains (names in analysis/lle/names.txt)
// SYNTHETIC: LLE 0x1007badf
// SYNTHETIC: LLE 0x1007bae9
// SYNTHETIC: LLE 0x1007baf9
// SYNTHETIC: LLE 0x1007bb05
// SYNTHETIC: LLE 0x1007bb0f
// SYNTHETIC: LLE 0x1007bb19
// SYNTHETIC: LLE 0x1007bb29
// SYNTHETIC: LLE 0x1007bb35
// SYNTHETIC: LLE 0x1007bb3f
// SYNTHETIC: LLE 0x1007bb49
// SYNTHETIC: LLE 0x1007bb59
// SYNTHETIC: LLE 0x1007bb65
// SYNTHETIC: LLE 0x1007bb6f
// SYNTHETIC: LLE 0x1007bb79
// SYNTHETIC: LLE 0x1007bb89
// SYNTHETIC: LLE 0x1007bb95
// SYNTHETIC: LLE 0x1007bb9f
// SYNTHETIC: LLE 0x1007bba9
// SYNTHETIC: LLE 0x1007bbb9
// SYNTHETIC: LLE 0x1007bbc5
// SYNTHETIC: LLE 0x1007bbcf
// SYNTHETIC: LLE 0x1007bbd9
// SYNTHETIC: LLE 0x1007bbe9
// SYNTHETIC: LLE 0x1007bbf5
