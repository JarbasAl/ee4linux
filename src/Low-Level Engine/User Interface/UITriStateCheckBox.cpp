// C:\EEX\Low-Level Engine\User Interface\UITriStateCheckBox.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x100805c4..0x10080c07: the header-inline members (dtor, GetType, ButtonUp), the implicit copy ctor/operator= and
// ??_E (this unit is the first includer of UITriStateCheckBox.h), the U2D key init chains (0x100806d4), then the ctor
// and the out-of-line members (alphabetical, the protected CycleCheck last).
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
#include "lle/UIControl.h"
#include "lle/UIButton.h"
#include "lle/UIScrollBar.h"
#include "lle/UITextLabel.h"
#include "lle/UITriStateCheckBox.h"
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
#include "lle/UIFormManager.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"

// FUNCTION: LLE 0x100807f4
// Creates a three-state check box (off, on, not) at most 19 pixels square with an optional text label beside it
UITriStateCheckBox::UITriStateCheckBox(unsigned long inId, const UWideString& inText, GEFont* inFont,
                                       const GERectangle& inRectangle, UIForm* inForm, unsigned long inIndex,
                                       unsigned long inBorderThickness)
    : UIButton(inId, inForm->GetManager()->GetCheckOff(), NULL, inForm->GetManager()->GetCheckDownOff(),
               GERectangle(inRectangle.GetTopLeft(),
                           GE2DPoint(inRectangle.GetWidth() > kScrollBarWidth ? inRectangle.GetRight()
                                                                               : inRectangle.GetLeft() + 19.0f,
                                     inRectangle.GetHeight() > kScrollBarWidth ? inRectangle.GetBottom()
                                                                                : inRectangle.GetTop() + 19.0f)),
               inForm, inIndex, inBorderThickness)
{
    this->mCheckState = 0;
    this->mTextLabel = NULL;
    this->mHasText = false;
    float theHeight = inRectangle.GetHeight();
    if (theHeight > kScrollBarWidth)
        this->SetBitmapDrawOffset(GE2DPoint(0.0f, (theHeight - 2.0f * inBorderThickness) * 0.5f - 10.0f));
    this->SetText(inText, inFont);
}

// FUNCTION: LLE 0x10080916
// Sets the check box width, shrinking the text label to fit beside the box
void UITriStateCheckBox::ChangeWidth(unsigned long inWidth)
{
    GERectangle theRectangle;
    if (this->mTextLabel) {
        theRectangle = this->mTextLabel->GetRectangle();
        theRectangle.SetRight(theRectangle.GetLeft() + inWidth - kScrollBarWidth);
        this->mTextLabel->SetRectangle(theRectangle);
    }
    theRectangle = this->mRectangle;
    theRectangle.SetRight(theRectangle.GetLeft() + inWidth - 1.0f);
    UIControl::SetRectangle(theRectangle);
}

// FUNCTION: LLE 0x100809ab
// Copies the draw state of the box and its label; deletes a label whose text was cleared
long UITriStateCheckBox::CopyData()
{
    long theResult = UIButton::CopyData();
    if (this->mHasText)
        theResult = this->mTextLabel->CopyData();
    else if (this->mTextLabel) {
        delete this->mTextLabel;
        this->mTextLabel = NULL;
    }
    return theResult;
}

// FUNCTION: LLE 0x100809ec
// Draws the box's polygons, then the label's
long UITriStateCheckBox::DrawPolygons()
{
    long theResult = UIButton::DrawPolygons();
    if (this->mTextLabel)
        theResult = this->mTextLabel->DrawPolygons();
    return theResult;
}

// FUNCTION: LLE 0x10080a05
// Draws the control text, then the label's text
long UITriStateCheckBox::DrawTextA()
{
    long theResult = UIControl::DrawTextA();
    if (this->mTextLabel)
        theResult = this->mTextLabel->DrawTextA();
    return theResult;
}

// FUNCTION: LLE 0x10080a1d
// Sets the check state, shows its bitmaps and fires the checked trigger (an unknown state only stores the value)
void UITriStateCheckBox::SetCheckState(unsigned long inState)
{
    this->mCheckState = inState;
    switch (inState) {
    case kCheckStateOff:
        this->SetNormalBitmap(this->mForm->GetManager()->GetCheckOff());
        this->SetPressedBitmap(this->mForm->GetManager()->GetCheckDownOff());
        break;
    case kCheckStateOn:
        this->SetNormalBitmap(this->mForm->GetManager()->GetCheckOn());
        this->SetPressedBitmap(this->mForm->GetManager()->GetCheckDownOn());
        break;
    case kCheckStateNot:
        this->SetNormalBitmap(this->mForm->GetManager()->GetCheckNot());
        this->SetPressedBitmap(this->mForm->GetManager()->GetCheckDownNot());
        break;
    default:
        return;
    }
    this->FireTriggerEvent(kControlTriggerChecked);
}

// FUNCTION: LLE 0x10080ac1
// Moves the check box and its text label
void UITriStateCheckBox::SetLocation(const GE2DPoint& inLocation)
{
    GE2DPoint theLocation;
    UIControl::SetLocation(inLocation);
    if (this->mTextLabel == NULL)
        return;
    theLocation.SetX(this->mRectangle.GetLeft() + this->mBorderThickness + kScrollBarWidth);
    theLocation.SetY(this->mRectangle.GetTop() + this->mBorderThickness);
    this->mTextLabel->SetLocation(theLocation);
}

// FUNCTION: LLE 0x10080b1c
// Sets the label text, creating the label on first use; empty text hides the label
void UITriStateCheckBox::SetText(const UWideString& inText, GEFont* inFont)
{
    if (inText.GetLength() > 0) {
        if (this->mTextLabel) {
            this->mTextLabel->SetText(inText, true);
            this->SetDirty(true);
        } else
            this->mTextLabel = new UITextLabel(inText, inFont, this->GetId(),
                                         GERectangle(this->mRectangle.GetLeft() + this->mBorderThickness + kScrollBarWidth,
                                                     this->mRectangle.GetTop() + this->mBorderThickness,
                                                     this->mRectangle.GetRight() - this->mBorderThickness,
                                                     this->mRectangle.GetBottom() - this->mBorderThickness),
                                         this->GetForm(), 0, this->GetIndex(), 0);
        this->mHasText = true;
        this->SetDirty(true);
    } else
        this->mHasText = false;
}

// FUNCTION: LLE 0x10080be6
// Advances the check state off -> on -> not -> off
void UITriStateCheckBox::CycleCheck()
{
    this->mCheckState = (this->mCheckState + 1) % 3;
    this->SetCheckState(this->mCheckState);
}

// Header-inline members first emitted here (bodies in lle/UITriStateCheckBox.h)
// FUNCTION: LLE 0x100805c4
// FUNCTION: LLE 0x100805e6
// FUNCTION: LLE 0x100805ea

// Compiler-generated: the implicit copy ctor/operator=, ??_E and the U2D key init chains (names in
// analysis/lle/names.txt)
// SYNTHETIC: LLE 0x100805fb
// SYNTHETIC: LLE 0x1008063a
// SYNTHETIC: LLE 0x10080673
// SYNTHETIC: LLE 0x100806d4
// SYNTHETIC: LLE 0x100806de
// SYNTHETIC: LLE 0x100806ee
// SYNTHETIC: LLE 0x100806fa
// SYNTHETIC: LLE 0x10080704
// SYNTHETIC: LLE 0x1008070e
// SYNTHETIC: LLE 0x1008071e
// SYNTHETIC: LLE 0x1008072a
// SYNTHETIC: LLE 0x10080734
// SYNTHETIC: LLE 0x1008073e
// SYNTHETIC: LLE 0x1008074e
// SYNTHETIC: LLE 0x1008075a
// SYNTHETIC: LLE 0x10080764
// SYNTHETIC: LLE 0x1008076e
// SYNTHETIC: LLE 0x1008077e
// SYNTHETIC: LLE 0x1008078a
// SYNTHETIC: LLE 0x10080794
// SYNTHETIC: LLE 0x1008079e
// SYNTHETIC: LLE 0x100807ae
// SYNTHETIC: LLE 0x100807ba
// SYNTHETIC: LLE 0x100807c4
// SYNTHETIC: LLE 0x100807ce
// SYNTHETIC: LLE 0x100807de
// SYNTHETIC: LLE 0x100807ea
