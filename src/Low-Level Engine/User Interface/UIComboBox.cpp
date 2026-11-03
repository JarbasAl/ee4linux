// C:\EEX\Low-Level Engine\User Interface\UIComboBox.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x100602e4..0x10064ed6. The first includer of UIListBox.h, UIEditBox.h, UILBString.h and UIComboBox.h, so it
// starts with their inline blocks in class-definition order (UWSNoCaseAlphaSort::operator(), UIEditBox's inline
// members + implicit copy members + ??_E, UIListBox<UWideString>'s copy members + ??_E, UILBString's, then
// UIComboBox's), then the U2D key init chains (0x100613ef), the ctor (0x1006150f) and the out-of-line members in
// alphabetical order, the vector<UWideString> copy members, every member of UIListBox<UWideString> in declaration
// order (0x100629a3..), the vector<UWideString> COMDATs, the InsertLine member template (0x1006452c) and the
// lower_bound/sort chains it and Sort() instantiate. UIControl.cpp starts at 0x10064ed6.
#include <windows.h>
#include <mbstring.h>
#include <stdlib.h>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/U2DRectangle.h"
#include "lle/GERasterizer.h"
#include "lle/GEBitmap.h"
#include "lle/GERhombus.h"
#include "lle/GELine.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/TMPoll.h"
#include "lle/Input System.h"
#include "lle/UIControl.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/UITextLabel.h"
#include "lle/UISlider.h"
#include "lle/UIScrollBar.h"
#include "lle/UIListBox.h"
#include "lle/UIEditBox.h"
#include "lle/UILBString.h"
#include "lle/UIComboBox.h"

// FUNCTION: LLE 0x1006150f
// Creates a combo box: a text label (type 1) or edit box, a drop button and a hidden list box showing the first entry
UIComboBox::UIComboBox(unsigned long inId, unsigned long inTextBoxType, std::vector<UWideString> inData,
                       GEFont* inFont, const GERectangle& inRectangle, unsigned long inMaxListBoxHeight,
                       UIForm* inForm, unsigned long inIndex, unsigned long inMaxChars,
                       unsigned long inBorderThickness)
    : UIControl(kControlTypeComboBox, inId, inRectangle, inForm, inIndex, 0)
{
    GERectangle theRectangle;
    UWideString theEmpty("");
    GEColor theColor;
    this->mTextBoxType = inTextBoxType;
    this->mMaxListBoxHeight = inMaxListBoxHeight;
    this->mReleasedOnce = false;
    theRectangle.SetTopLeft(GE2DPoint(this->mRectangle.GetRight() - inRectangle.GetHeight(), inRectangle.GetTop()));
    theRectangle.SetRight(inRectangle.GetRight());
    theRectangle.SetBottom(inRectangle.GetBottom());
    GEBitmap* theNormal = inForm->GetManager()->CreateBitmapForScaledControl(
        inForm->GetManager()->GetSBBitmapDownNormal(), theRectangle);
    GEBitmap* thePressed = inForm->GetManager()->CreateBitmapForScaledControl(
        inForm->GetManager()->GetSBBitmapDownPressed(), theRectangle);
    this->mDropButton = new UIButton(this->GetId(), theNormal, NULL, thePressed, theRectangle, this->GetForm(), this->GetIndex(), 0);
    theNormal->Dereference();
    thePressed->Dereference();
    GE2DPoint theLocation;
    theLocation = inRectangle.GetTopLeft();
    if (inTextBoxType == 1) {
        this->mTextLabel = new UITextLabel(theEmpty, inFont, this->GetId(),
                                     GERectangle(theLocation, this->mRectangle.GetWidth() - this->mDropButton->GetWidth() + 1.0f,
                                                 inRectangle.GetHeight()),
                                     this->GetForm(), 0, this->GetIndex(), inBorderThickness);
        this->mEditBox = NULL;
        this->mTextBox = this->mTextLabel;
    } else {
        this->mEditBox = new UIEditBox(this->GetId(), inFont, inMaxChars, false,
                                 GERectangle(theLocation, this->mRectangle.GetWidth() - this->mDropButton->GetWidth() + 1.0f,
                                             inRectangle.GetHeight()),
                                 this->GetForm(), this->GetIndex(), inBorderThickness, false, false);
        this->mTextLabel = NULL;
        this->mTextBox = this->mEditBox;
    }
    this->mListBox = new UILBString(this->GetId(), inData, inFont, true,
                              GERectangle(this->mRectangle.GetLeft(), this->mTextBox->GetRectangle().GetBottom(),
                                          this->mRectangle.GetRight(),
                                          this->mTextBox->GetRectangle().GetBottom() + inMaxListBoxHeight),
                              this->GetForm(), this->GetIndex(), inBorderThickness);
    this->mListBox->ChangeSelectionIndex(0, true, false);
    this->mIsListBoxOpen = false;
    this->mListBox->SetBgOpacity(1.0f);
    this->mListBox->SetBgColor(GEColor(0.0f, 0.0f, 0.0f));
    this->mListBox->SetScrollBarBgOpacity(1.0f);
    this->mListBox->SetScrollBarBgColor(GEColor(0.0f, 0.0f, 0.0f));
    this->mCurrentSelection = -1;
    this->mCopiedListBoxOpen = false;
    this->mListBoxAlwaysDirty = false;
    if (this->mListBox->GetLineCount() > 0) {
        if (this->mTextLabel)
            this->mTextLabel->SetText(this->mListBox->GetData(0), true);
        else
            this->mEditBox->SetText(this->mListBox->GetData(0));
        this->mCurrentSelection = 0;
    }
    UIControl::SetRectangle(GERectangle(this->mTextBox->GetRectangle().GetTopLeft(),
                                        this->mDropButton->GetRectangle().GetBottomRight()));
    this->mCopiedHasFocus = false;
    this->mListBox->GetSelectedBackground()->GetColorPerVertex(theColor, theColor, theColor, theColor);
    this->mSelectedBackground.SetColor(theColor);
}

// FUNCTION: LLE 0x10061978
// Destroys the combo box and its text box, list box and drop button
UIComboBox::~UIComboBox()
{
    delete this->mTextBox;
    delete this->mListBox;
    delete this->mDropButton;
}

// FUNCTION: LLE 0x100619c5
// Selects the edit box's text in the list (OK), or adds it at the top and selects it (not found); fails on empty text
long UIComboBox::AddString()
{
    long theResult = kErrorNotFound;
    UWideString theString(this->mEditBox->GetText(0));
    long i = this->mListBox->GetLineCount();
    if (theString.GetLength() > 0) {
        bool theFound = false;
        for (i--; i >= 0; i--)
            if (this->mListBox->GetData(i) == theString) {
                theFound = true;
                break;
            }
        if (theFound) {
            theResult = kErrorOK;
            this->mListBox->ChangeSelectionIndex(i, true, false);
        } else {
            this->InsertLine(theString, 0);
            this->mListBox->ChangeSelectionIndex(0, true, false);
        }
        this->SetDirty(true);
    } else
        theResult = kErrorFailed;
    return theResult;
}

// FUNCTION: LLE 0x10061a86
// Appends a line to the list, reopening an open list so it resizes
void UIComboBox::AppendLine(UWideString& inLine)
{
    this->SetDirty(true);
    this->mListBox->AppendLine(inLine);
    if (this->mIsListBoxOpen) {
        this->CloseListBox();
        this->OpenListBox();
    }
}

// FUNCTION: LLE 0x10061ac3
// Appends several lines to the list, reopening an open list so it resizes
void UIComboBox::AppendStringArray(std::vector<UWideString>& inLines)
{
    this->SetDirty(true);
    this->mListBox->AppendStringArray(inLines);
    if (this->mIsListBoxOpen) {
        this->CloseListBox();
        this->OpenListBox();
    }
}

// FUNCTION: LLE 0x10061b00
// Shows the selected entry in the text box and fires the selection-changed trigger
void UIComboBox::ChangeSelection()
{
    if (this->mTextLabel)
        this->mTextLabel->SetText(this->mListBox->GetData(this->mCurrentSelection), true);
    else
        this->mEditBox->SetText(this->mListBox->GetData(this->mCurrentSelection));
    this->FireTriggerEvent(kControlTriggerSelectionChanged);
}

// FUNCTION: LLE 0x10061b5b
// Resizes the text box, the list and the control to a new width
void UIComboBox::ChangeWidth(unsigned long inWidth)
{
    GERectangle theRectangle;
    theRectangle = this->mTextBox->GetRectangle();
    float theWidth = inWidth;
    theRectangle.SetRight(theRectangle.GetLeft() + theWidth - kScrollBarWidth);
    this->mTextBox->SetRectangle(theRectangle);
    this->mListBox->ChangeWidth(inWidth);
    theRectangle.SetRight(theRectangle.GetLeft() + theWidth);
    UIControl::SetRectangle(theRectangle);
}

// FUNCTION: LLE 0x10061bdf
// Closes the drop-down list, shrinks the control back to the text box and fires the list-closed trigger
void UIComboBox::CloseListBox()
{
    GERectangle theRectangle;
    if (this->mEditBox == NULL)
        this->ReleaseCapturedKeyboard();
    this->mIsListBoxOpen = false;
    this->SetDirty(true);
    this->mForm->SetPopup(NULL);
    theRectangle.SetTopLeft(this->mTextBox->GetRectangle().GetTopLeft());
    theRectangle.SetRight(this->mDropButton->GetRectangle().GetRight());
    theRectangle.SetBottom(this->mTextBox->GetRectangle().GetBottom());
    this->SetRectangle(theRectangle);
    if (this->mListBoxAlwaysDirty)
        this->SetAlwaysDirty(false);
    this->FireTriggerEvent(kControlTriggerListClosed);
}

// FUNCTION: LLE 0x10061c92
// Snapshots the parts for drawing: the open list in the text box's colours, or the focus highlight when tabbed to
long UIComboBox::CopyData()
{
    GERectangle theRectangle;
    this->mCopiedListBoxOpen = false;
    this->mTextBox->CopyData();
    this->mDropButton->CopyData();
    if (this->mIsListBoxOpen) {
        this->mCopiedListBoxOpen = true;
        GEText* theText;
        if (this->mTextLabel)
            theText = this->mTextLabel->GetGEText();
        else
            theText = this->mEditBox->GetGEText();
        GEText* theListText = this->mListBox->GetGEText();
        theListText->SetFgColor(theText->GetFgColor());
        theListText->SetBgColor(theText->GetBgColor());
        theListText->SetIsTransparent(theText->GetIsTransparent());
        this->mListBox->CopyData();
    } else if (this->mForm->GetTabbingOn() && this->mForm->GetTabStop() == this->mIndex) {
        theRectangle.SetLeft(this->mTextBox->GetRectangle().GetLeft() + this->mTextBox->GetBorderThickness());
        theRectangle.SetTop(this->mTextBox->GetRectangle().GetTop() + this->mTextBox->GetBorderThickness());
        theRectangle.SetRight(this->mTextBox->GetRectangle().GetRight() - this->mTextBox->GetBorderThickness());
        theRectangle.SetBottom(this->mTextBox->GetRectangle().GetBottom() - this->mTextBox->GetBorderThickness());
        this->mSelectedBackground.SetRectangle(theRectangle);
        this->mCopiedHasFocus = true;
    } else
        this->mCopiedHasFocus = false;
    return UIControl::CopyData();
}

// FUNCTION: LLE 0x10061dd0
// Empties the list and the text box and clears the selection
void UIComboBox::DeleteAllEntries()
{
    this->mListBox->DeleteAllEntries();
    if (this->mTextLabel)
        this->mTextLabel->SetText("", true);
    else
        this->mEditBox->SetText("");
    this->SetDirty(true);
    if (this->mIsListBoxOpen) {
        this->CloseListBox();
        this->OpenListBox();
    }
    this->mCurrentSelection = -1;
}

// FUNCTION: LLE 0x10061e62
// Removes a list line and shows the entry now selected, or clears the text and the selection when none is
void UIComboBox::DeleteLine(unsigned long inLine)
{
    this->mListBox->DeleteLine(inLine);
    if (this->mListBox->GetCurrentSelection(inLine) == kErrorOK) {
        if (this->mTextLabel)
            this->mTextLabel->SetText(this->mListBox->GetData(inLine), true);
        else
            this->mEditBox->SetText(this->mListBox->GetData(inLine));
    } else {
        this->mCurrentSelection = -1;
        if (this->mTextLabel)
            this->mTextLabel->SetText("", true);
        else
            this->mEditBox->SetText("");
    }
    this->SetDirty(true);
    if (this->mIsListBoxOpen) {
        this->CloseListBox();
        this->OpenListBox();
    }
}

// FUNCTION: LLE 0x10061f56
// Draws the text box, the focus highlight, the drop button and the open list
long UIComboBox::DrawPolygons()
{
    long theResult;
    UIControl::DrawPolygons();
    this->mTextBox->DrawPolygons();
    if (this->mCopiedHasFocus)
        this->GetRasterizer()->Draw(&this->mSelectedBackground, this->GetScreenIndex());
    theResult = this->mDropButton->DrawPolygons();
    if (this->mCopiedListBoxOpen)
        theResult = this->mListBox->DrawPolygons();
    return theResult;
}

// FUNCTION: LLE 0x10061fb6
// Draws the text of the text box, the drop button and the open list
long UIComboBox::DrawTextA()
{
    long theResult;
    this->mDrawCount--;
    this->mTextBox->DrawTextA();
    theResult = this->mDropButton->DrawTextA();
    if (this->mCopiedListBoxOpen)
        theResult = this->mListBox->DrawTextA();
    return theResult;
}

// FUNCTION: LLE 0x10061feb
// Closes an open list and releases mouse and keyboard capture when the control is hidden
void UIComboBox::HideControl()
{
    if (!this->mIsListBoxOpen)
        return;
    this->CloseListBox();
    this->ReleaseCapturedMouse();
    this->ReleaseCapturedKeyboard();
}

// FUNCTION: LLE 0x1006200f
// Inserts a line into the list at an index, reopening an open list so it resizes
void UIComboBox::InsertLine(const UWideString& inLine, unsigned long inIndex)
{
    this->SetDirty(true);
    this->mListBox->InsertLine(inLine, inIndex);
    if (this->mIsListBoxOpen) {
        this->CloseListBox();
        this->OpenListBox();
    }
}

// FUNCTION: LLE 0x10062053
// Handles a click: opens the list (or starts editing the edit box), or picks in / closes the open list
long UIComboBox::MouseLButtonDown()
{
    long theResult = kErrorOK;
    if (!this->mIsListBoxOpen && !this->GetHasMouseCapture()) {
        if (this->mTextLabel) {
            this->mDropButton->MouseLButtonDown();
            theResult = this->mTextLabel->MouseLButtonDown();
            this->mListBox->GetCurrentSelection(this->mCurrentSelection);
            this->OpenListBox();
        } else if (this->mTextBox->GetRectangle().Contains(this->GetMouseLocation())) {
            theResult = this->mEditBox->MouseLButtonDown();
            this->CaptureMouse();
        } else {
            theResult = this->mDropButton->MouseLButtonDown();
            this->mListBox->GetCurrentSelection(this->mCurrentSelection);
            this->OpenListBox();
        }
    } else {
        if (this->mListBox->GetRectangle().Contains(this->GetMouseLocation())) {
            theResult = this->mListBox->MouseLButtonDown();
            if (!this->mListBox->GetScrollBarCaptured())
                this->mListBox->GetCurrentSelection(this->mCurrentSelection);
        } else
            this->CloseListBox();
        this->CaptureMouse();
    }
    return theResult;
}

// FUNCTION: LLE 0x10062144
// Handles a release: commits the entry under the mouse and closes the list, else keeps it open for a second click
long UIComboBox::MouseLButtonUp()
{
    GERectangle theRectangle = this->mListBox->GetRectangle();
    if (this->mIsListBoxOpen) {
        if (!this->mListBox->GetScrollBarCaptured()) {
            if (this->mListBox->GetScrollBar())
                theRectangle.SetRight(this->mListBox->GetScrollBar()->GetRectangle().GetLeft());
            if (theRectangle.Contains(this->GetMouseLocation())) {
                if (this->mListBox->GetCurrentSelection(this->mCurrentSelection) == kErrorOK)
                    this->ChangeSelection();
                this->CloseListBox();
                this->ReleaseCapturedMouse();
            } else if (this->mReleasedOnce && this->mCurrentSelection < this->mListBox->GetLineCount()) {
                if (this->mListBox->GetCurrentSelection(this->mCurrentSelection) == kErrorOK)
                    this->ChangeSelection();
                this->CloseListBox();
                this->ReleaseCapturedMouse();
            } else
                this->mReleasedOnce = true;
        }
        this->mListBox->MouseLButtonUp();
    } else {
        if (this->mCurrentSelection < this->mListBox->GetLineCount())
            this->mListBox->ChangeSelectionIndex(this->mCurrentSelection, false, false);
        this->ReleaseCapturedMouse();
    }
    this->mTextBox->MouseLButtonUp();
    long theResult = this->mDropButton->MouseLButtonUp();
    this->SetDirty(true);
    return theResult;
}

// FUNCTION: LLE 0x10062252
// Passes mouse movement to the text box, the drop button and the open list
long UIComboBox::MouseMove(const GE2DPoint& inLocation)
{
    long theResult;
    this->mTextBox->MouseMove(inLocation);
    theResult = this->mDropButton->MouseMove(inLocation);
    if (this->mIsListBoxOpen)
        theResult = this->mListBox->MouseMove(inLocation);
    return theResult;
}

// FUNCTION: LLE 0x1006228c
// Scrolls the open list with the mouse wheel; fails while the list is closed
long UIComboBox::MouseWheelTurned(long inDelta)
{
    long theResult = kErrorFailed;
    if (this->mIsListBoxOpen)
        theResult = this->mListBox->MouseWheelTurned(inDelta);
    return theResult;
}

// FUNCTION: LLE 0x100622aa
// Opens the drop-down list below the text box, or above it when there is more room, and captures input
void UIComboBox::OpenListBox()
{
    GERectangle theListRectangle;
    GERectangle theRectangle;
    GEWindow* theWindow = this->GetRasterizer()->GetWindow(this->GetScreenIndex());
    long theHeight = this->mListBox->GetLineCount() * this->mListBox->mLineHeight + this->mListBox->GetBorderThickness() * 2 +
                     1;
    if (theHeight > (long)this->mMaxListBoxHeight)
        theHeight = this->mMaxListBoxHeight;
    theListRectangle.SetLeft(this->mTextBox->GetRectangle().GetLeft());
    theListRectangle.SetRight(this->mDropButton->GetRectangle().GetRight());
    theRectangle.SetLeft(theListRectangle.GetLeft());
    theRectangle.SetRight(theListRectangle.GetRight());
    long theBelow = theWindow->GetHeight() - UMFloor(this->mTextBox->GetRectangle().GetBottom()) - 1;
    long theAbove = UMFloor(this->mTextBox->GetRectangle().GetTop()) - 1;
    if (theHeight > theBelow) {
        if (theBelow < theAbove) {
            if (theAbove < theHeight)
                theHeight = theAbove;
            theListRectangle.SetBottom(this->mTextBox->GetRectangle().GetTop() - 1.0f);
            theListRectangle.SetTop(theListRectangle.GetBottom() - theHeight);
            theRectangle.SetTop(theListRectangle.GetTop());
            theRectangle.SetBottom(this->mTextBox->GetRectangle().GetBottom());
        } else {
            if (theBelow < theHeight)
                theHeight = theBelow;
            theListRectangle.SetTop(this->mTextBox->GetRectangle().GetBottom() + 1.0f);
            theListRectangle.SetBottom(theListRectangle.GetTop() + theHeight);
            theRectangle.SetTop(this->mTextBox->GetRectangle().GetTop());
            theRectangle.SetBottom(theListRectangle.GetBottom());
        }
    } else {
        theListRectangle.SetTop(this->mTextBox->GetRectangle().GetBottom() + 1.0f);
        theListRectangle.SetBottom(theListRectangle.GetTop() + theHeight);
        theRectangle.SetTop(this->mTextBox->GetRectangle().GetTop());
        theRectangle.SetBottom(theListRectangle.GetBottom());
    }
    this->mReleasedOnce = false;
    this->mIsListBoxOpen = true;
    this->mListBox->SetRectangle(theListRectangle);
    this->SetRectangle(theRectangle);
    this->mForm->SetPopup(this);
    this->CaptureMouse();
    this->CaptureKeyboard();
    this->SetDirty(true);
    if (this->mListBoxAlwaysDirty)
        this->SetAlwaysDirty(true);
    this->FireTriggerEvent(kControlTriggerListOpened);
}

// FUNCTION: LLE 0x10062462
// Shows the list's current selection in the text box after a Down key, or clears the text
void UIComboBox::ProcessDown()
{
    if (this->mListBox->GetCurrentSelection(this->mCurrentSelection) == kErrorOK) {
        if (this->mTextLabel)
            this->mTextLabel->SetText(this->mListBox->GetData(this->mCurrentSelection), true);
        else
            this->mEditBox->SetText(this->mListBox->GetData(this->mCurrentSelection));
    } else if (this->mTextLabel)
        this->mTextLabel->DeleteLine(0);
    else
        this->mEditBox->DeleteText();
}

// Shows the list's current selection in the text box after an Up key, or clears the text
// (ICF-folded with ProcessDown 0x10062462)
void UIComboBox::ProcessUp()
{
    if (this->mListBox->GetCurrentSelection(this->mCurrentSelection) == kErrorOK) {
        if (this->mTextLabel)
            this->mTextLabel->SetText(this->mListBox->GetData(this->mCurrentSelection), true);
        else
            this->mEditBox->SetText(this->mListBox->GetData(this->mCurrentSelection));
    } else if (this->mTextLabel)
        this->mTextLabel->DeleteLine(0);
    else
        this->mEditBox->DeleteText();
}

// FUNCTION: LLE 0x100624db
// Selects a list entry by index and shows it; out-of-range indices are ignored
void UIComboBox::SetCurrentSelection(unsigned long inIndex)
{
    unsigned long theSelection;
    this->mListBox->GetCurrentSelection(theSelection);
    if (!(inIndex < this->mListBox->GetLineCount()))
        return;
    if (inIndex != theSelection)
        this->mListBox->ChangeSelectionIndex(inIndex, true, false);
    this->mCurrentSelection = inIndex;
    UIComboBox::ChangeSelection();
}

// FUNCTION: LLE 0x1006252f
// Moves the combo box, keeping a list that opened upwards above the text box
void UIComboBox::SetLocation(const GE2DPoint& inLocation)
{
    if (this->mIsListBoxOpen && this->mListBox->GetRectangle().GetTop() < this->mTextBox->GetRectangle().GetTop()) {
        this->mListBox->SetLocation(inLocation);
        this->mTextBox->SetLocation(GE2DPoint(inLocation.GetX(), this->mListBox->GetRectangle().GetBottom() + 1.0f));
        this->mDropButton->SetLocation(GE2DPoint(this->mTextBox->GetRectangle().GetRight() + 1.0f,
                                           this->mTextBox->GetRectangle().GetTop()));
    } else {
        this->mTextBox->SetLocation(inLocation);
        this->mDropButton->SetLocation(GE2DPoint(this->mTextBox->GetRectangle().GetRight() + 1.0f, inLocation.GetY()));
        this->mListBox->SetLocation(GE2DPoint(this->mTextBox->GetRectangle().GetLeft(),
                                        this->mTextBox->GetRectangle().GetBottom() + 1.0f));
    }
    UIControl::SetLocation(inLocation);
}

// FUNCTION: LLE 0x10062619
// Takes keyboard focus when tabbed to (the edit box too, if any)
void UIComboBox::TabbedTo()
{
    this->SetDirty(true);
    if (this->mEditBox) {
        this->mEditBox->TabbedTo();
        this->CaptureKeyboard();
    } else if (this->mForm->GetTabbingOn() == true)
        this->CaptureKeyboard();
}

// FUNCTION: LLE 0x10062649
// Gives up focus when tabbed away: closes the list and releases mouse and keyboard
void UIComboBox::TabbedFrom()
{
    if (this->mIsListBoxOpen)
        this->CloseListBox();
    else
        this->SetDirty(true);
    this->ReleaseCapturedMouse();
    if (this->mTextLabel == NULL)
        this->mEditBox->TabbedFrom();
    this->ReleaseCapturedKeyboard();
}

// FUNCTION: LLE 0x1006268a
// Handles keys: Down opens the list; in the open list Enter commits and Up/Down move; else the edit box gets them
long UIComboBox::UpdateKeyboard()
{
    long theResult = kErrorOK;
    if (!this->mIsListBoxOpen) {
        if (UIControl::DownKeyPressed())
            this->OpenListBox();
        else if (this->mEditBox) {
            if (UIControl::sEnterKey->GetValue())
                this->AddString();
            theResult = this->mEditBox->UpdateKeyboard();
        }
    } else {
        theResult = this->mListBox->UpdateKeyboard();
        if (UIControl::sEnterKey->GetValue()) {
            if (this->mEditBox)
                this->AddString();
            else {
                if (this->mListBox->GetCurrentSelection(this->mCurrentSelection) == kErrorOK)
                    this->ChangeSelection();
                this->CloseListBox();
                this->ReleaseCapturedMouse();
            }
        }
        if (UIControl::DownKeyPressed())
            this->ProcessDown();
        if (UIControl::UpKeyPressed())
            this->ProcessUp();
        if (this->mEditBox && !UIControl::DownKeyPressed() && !UIControl::UpKeyPressed() && !UIControl::HomeKeyPressed() && !UIControl::EndKeyPressed())
            theResult = this->mEditBox->UpdateKeyboard();
    }
    return theResult;
}

// FUNCTION: LLE 0x100627a7
// Passes a typed character to the open list, else to the text box
long UIComboBox::UpdateTranslatedKey(const char* inKey, short inRepeat)
{
    UIControl* theControl;
    if (this->mIsListBoxOpen)
        theControl = this->mListBox;
    else
        theControl = this->mTextBox;
    return theControl->UpdateTranslatedKey(inKey, inRepeat);
}

// Header-inline members first emitted here, in class-definition order (names in analysis/lle/names.txt)
// SYNTHETIC: LLE 0x100602e4
// SYNTHETIC: LLE 0x10060308
// SYNTHETIC: LLE 0x10060362
// SYNTHETIC: LLE 0x1006037a
// SYNTHETIC: LLE 0x10060381
// SYNTHETIC: LLE 0x10060388
// SYNTHETIC: LLE 0x1006038f
// SYNTHETIC: LLE 0x10060396
// SYNTHETIC: LLE 0x1006039d
// SYNTHETIC: LLE 0x100603b9
// SYNTHETIC: LLE 0x100603d4
// SYNTHETIC: LLE 0x100603db
// SYNTHETIC: LLE 0x100603f1
// SYNTHETIC: LLE 0x1006040f
// SYNTHETIC: LLE 0x1006042c
// SYNTHETIC: LLE 0x1006044b
// SYNTHETIC: LLE 0x10060468
// SYNTHETIC: LLE 0x100604a7
// SYNTHETIC: LLE 0x100604af
// SYNTHETIC: LLE 0x10060799
// SYNTHETIC: LLE 0x10060b30
// SYNTHETIC: LLE 0x10060b91
// SYNTHETIC: LLE 0x10060cf9
// SYNTHETIC: LLE 0x10060f03
// SYNTHETIC: LLE 0x10060f64
// SYNTHETIC: LLE 0x10060fd7
// SYNTHETIC: LLE 0x1006100b
// SYNTHETIC: LLE 0x10061028
// SYNTHETIC: LLE 0x1006105b
// SYNTHETIC: LLE 0x10061088
// SYNTHETIC: LLE 0x100610e9
// SYNTHETIC: LLE 0x1006110b
// SYNTHETIC: LLE 0x10061112
// SYNTHETIC: LLE 0x10061130
// SYNTHETIC: LLE 0x10061137
// SYNTHETIC: LLE 0x1006113e
// SYNTHETIC: LLE 0x1006114b
// SYNTHETIC: LLE 0x10061152
// SYNTHETIC: LLE 0x10061159
// SYNTHETIC: LLE 0x10061166
// SYNTHETIC: LLE 0x10061178
// SYNTHETIC: LLE 0x1006119a
// SYNTHETIC: LLE 0x100611c5
// SYNTHETIC: LLE 0x100611d7
// SYNTHETIC: LLE 0x100611e9
// SYNTHETIC: LLE 0x100611ff
// SYNTHETIC: LLE 0x100612cb
// SYNTHETIC: LLE 0x1006138e

// Compiler-generated: the U2D key init chains (names in analysis/lle/names.txt)
// SYNTHETIC: LLE 0x100613ef
// SYNTHETIC: LLE 0x100613f9
// SYNTHETIC: LLE 0x10061409
// SYNTHETIC: LLE 0x10061415
// SYNTHETIC: LLE 0x1006141f
// SYNTHETIC: LLE 0x10061429
// SYNTHETIC: LLE 0x10061439
// SYNTHETIC: LLE 0x10061445
// SYNTHETIC: LLE 0x1006144f
// SYNTHETIC: LLE 0x10061459
// SYNTHETIC: LLE 0x10061469
// SYNTHETIC: LLE 0x10061475
// SYNTHETIC: LLE 0x1006147f
// SYNTHETIC: LLE 0x10061489
// SYNTHETIC: LLE 0x10061499
// SYNTHETIC: LLE 0x100614a5
// SYNTHETIC: LLE 0x100614af
// SYNTHETIC: LLE 0x100614b9
// SYNTHETIC: LLE 0x100614c9
// SYNTHETIC: LLE 0x100614d5
// SYNTHETIC: LLE 0x100614df
// SYNTHETIC: LLE 0x100614e9
// SYNTHETIC: LLE 0x100614f9
// SYNTHETIC: LLE 0x10061505

// The vector<UWideString> copy members, UIListBox<UWideString>'s members in declaration order, the vector
// COMDATs, the InsertLine member template and the lower_bound/sort chains
// TEMPLATE: LLE 0x100627d1
// TEMPLATE: LLE 0x1006281b
// TEMPLATE: LLE 0x1006298d
// TEMPLATE: LLE 0x100629a3
// TEMPLATE: LLE 0x10062c18
// TEMPLATE: LLE 0x10062ca9
// TEMPLATE: LLE 0x10062ccf
// TEMPLATE: LLE 0x10062d24
// TEMPLATE: LLE 0x10062dda
// TEMPLATE: LLE 0x10062e18
// TEMPLATE: LLE 0x10062e20
// TEMPLATE: LLE 0x10063079
// TEMPLATE: LLE 0x100630aa
// TEMPLATE: LLE 0x10063138
// TEMPLATE: LLE 0x10063348
// TEMPLATE: LLE 0x1006335a
// TEMPLATE: LLE 0x10063397
// TEMPLATE: LLE 0x100633b1
// TEMPLATE: LLE 0x100633b8
// TEMPLATE: LLE 0x100633d4
// TEMPLATE: LLE 0x100633db
// TEMPLATE: LLE 0x100633e2
// TEMPLATE: LLE 0x100633e9
// TEMPLATE: LLE 0x100633fc
// TEMPLATE: LLE 0x10063403
// TEMPLATE: LLE 0x1006340a
// TEMPLATE: LLE 0x10063464
// TEMPLATE: LLE 0x1006347a
// TEMPLATE: LLE 0x10063489
// TEMPLATE: LLE 0x10063498
// TEMPLATE: LLE 0x100634b5
// TEMPLATE: LLE 0x100634de
// TEMPLATE: LLE 0x100634e9
// TEMPLATE: LLE 0x100634f6
// TEMPLATE: LLE 0x10063513
// TEMPLATE: LLE 0x10063645
// TEMPLATE: LLE 0x1006366d
// TEMPLATE: LLE 0x100636a0
// TEMPLATE: LLE 0x100636e2
// TEMPLATE: LLE 0x100637d9
// TEMPLATE: LLE 0x1006388e
// TEMPLATE: LLE 0x10063899
// TEMPLATE: LLE 0x100638af
// TEMPLATE: LLE 0x100638b6
// TEMPLATE: LLE 0x100638bd
// TEMPLATE: LLE 0x10063951
// TEMPLATE: LLE 0x10063a48
// TEMPLATE: LLE 0x10063b0b
// TEMPLATE: LLE 0x10063bdf
// TEMPLATE: LLE 0x10063c48
// TEMPLATE: LLE 0x10063c9d
// TEMPLATE: LLE 0x10063cdb
// TEMPLATE: LLE 0x10063d38
// TEMPLATE: LLE 0x10063d60
// TEMPLATE: LLE 0x10063f1e
// TEMPLATE: LLE 0x10063f3a
// TEMPLATE: LLE 0x10063f66
// TEMPLATE: LLE 0x10063fda
// TEMPLATE: LLE 0x10064015
// TEMPLATE: LLE 0x1006404e
// TEMPLATE: LLE 0x10064063
// TEMPLATE: LLE 0x1006411f
// TEMPLATE: LLE 0x10064179
// TEMPLATE: LLE 0x100641e4
// TEMPLATE: LLE 0x100644be
// TEMPLATE: LLE 0x100644e9
// TEMPLATE: LLE 0x10064500
// TEMPLATE: LLE 0x1006452c
// TEMPLATE: LLE 0x100645fb
// TEMPLATE: LLE 0x100646b6
// TEMPLATE: LLE 0x100646df
// TEMPLATE: LLE 0x10064703
// TEMPLATE: LLE 0x10064720
// TEMPLATE: LLE 0x1006474c
// TEMPLATE: LLE 0x1006475e
// TEMPLATE: LLE 0x10064796
// TEMPLATE: LLE 0x100647bf
// TEMPLATE: LLE 0x10064898
// TEMPLATE: LLE 0x100648d7
// TEMPLATE: LLE 0x1006491b
// TEMPLATE: LLE 0x100649e2
// TEMPLATE: LLE 0x100649e8
// TEMPLATE: LLE 0x10064a25
// TEMPLATE: LLE 0x10064a64
// TEMPLATE: LLE 0x10064b93
// TEMPLATE: LLE 0x10064be0
// TEMPLATE: LLE 0x10064c0b
// TEMPLATE: LLE 0x10064c68
// TEMPLATE: LLE 0x10064c91
// TEMPLATE: LLE 0x10064cb6
// TEMPLATE: LLE 0x10064ccb
// TEMPLATE: LLE 0x10064d69
// TEMPLATE: LLE 0x10064dd7
// TEMPLATE: LLE 0x10064e09
// TEMPLATE: LLE 0x10064e79
// TEMPLATE: LLE 0x10064ec5
