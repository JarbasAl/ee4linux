// C:\EEX\Empire Earth\User Interface Thread\EEUIFError.cpp (__FILE__ 0x4cd490). The game's message box form and its two
// button classes: 0x260c27..0x2624a6 (the init chains of the seven colours, EEUIFEBOK, EEUIFEBCancel, EEUIFError;
// EEUIFileComboBox's ctor at 0x2624a6). Unit flags: the exe module set. The colours, constants and button members
// have provisional names.
#include <windows.h>
#include <mbstring.h>
#include <algorithm>
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
#include "lle/GERasterizer.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/SESoundManager.h"
#include "lle/Input System.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/NEMessage.h"
#include "lle/NESynchronousCommunications.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "lle/UResourceFile.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UIScrollBar.h"
#include "lle/UITextLabel.h"
#include "lle/UIEditBox.h"
#include "ee/EmpireEarth.h"
#include "lle/UIBitmapBackground.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIManager.h"
#include "ee/EEUIBGenericBackground.h"
#include "ee/EEUIFError.h"
#include "ee/EEStringIDs.h"
static const float kMessageScrollBarOpacity = 0.3f;             // 0x44a634
static const float kMessageButtonOpacity = 0.4f;                // 0x44a638
static const GEColor kMessageScrollBarColor(1.0f, 1.0f, 1.0f);  // 0x5645f8
static const GEColor kMessageButtonColor(1.0f, 1.0f, 1.0f);     // 0x5645e8
static const GEColor kMessageTextColor(0.0f, 0.0f, 0.0f);       // 0x5645d8
static const GEColor kMessageFocusedColor(0.61f, 0.46f, 0.0f);  // 0x5645c8
static const GEColor kMessageEditBoxColor(1.0f, 1.0f, 1.0f);    // 0x5645b8
static const GEColor kMessageSelectedColor(0.61f, 0.46f, 0.0f); // 0x5645a8
static const GEColor kMessageBackgroundColor(0.91f, 0.85f, 0.73f); // 0x564598

// The OK button (RTTI EEUIFEBOK > UIButton; vtable 0x44a640; sizeof 0x110). The in-class virtuals follow the ctor
// with the ??_G in declaration order.
class EEUIFEBOK : public UIButton {
    friend class EEUIFError;
public:
    EEUIFEBOK(UIForm* inForm, unsigned long inId);
    virtual long DrawPolygons()
    {
        GERhombus theRhombus;
        theRhombus.SetColor(kMessageButtonColor);
        theRhombus.SetRectangle(this->mCopiedDrawRectangle);
        theRhombus.SetOpacity(kMessageButtonOpacity);
        this->GetRasterizer()->Draw(&theRhombus, this->GetScreenIndex());
        return UIButton::DrawPolygons();
    }
    virtual ~EEUIFEBOK() {}
    void SetCallback(EEUIFErrorCallback inCallback, void* inData)
    {
        this->mCallback = inCallback;
        this->mData = inData;
    }
protected:
    virtual void ButtonUp();

    EEUIFErrorCallback mCallback;           // +0x104
    void* mData;                            // +0x108
    bool mPressed;                          // +0x10c
};

// The No and Cancel buttons (RTTI EEUIFEBCancel > UIButton; vtable 0x44a6d8; sizeof 0x114).
class EEUIFEBCancel : public UIButton {
    friend class EEUIFError;
public:
    EEUIFEBCancel(UIForm* inForm, unsigned long inId);
    virtual long DrawPolygons()
    {
        GERhombus theRhombus;
        theRhombus.SetColor(kMessageButtonColor);
        theRhombus.SetRectangle(this->mCopiedDrawRectangle);
        theRhombus.SetOpacity(kMessageButtonOpacity);
        this->GetRasterizer()->Draw(&theRhombus, this->GetScreenIndex());
        return UIButton::DrawPolygons();
    }
    virtual ~EEUIFEBCancel() {}
    void SetCallback(EEUIFErrorCallback inCallback, void* inData)
    {
        this->mCallback = inCallback;
        this->mData = inData;
    }
protected:
    virtual void ButtonUp();

    EEUIFErrorCallback mCallback;           // +0x104
    void* mData;                            // +0x108
    UIForm* mCloseForm;                     // +0x10c (DisplayCancel)
    bool mPressed;                          // +0x110
};

// FUNCTION: EXE 0x660d85
// Creates the message box's OK/Yes button (50x20) with its callback unset
EEUIFEBOK::EEUIFEBOK(UIForm* inForm, unsigned long inId)
    : UIButton(inId, NULL, NULL, NULL, GERectangle(GE2DPoint(0.0f, 0.0f), 50.0f, 20.0f), inForm, inId, 1),
      mCallback(NULL), mData(NULL)
{
    this->SetFocusedTextColor(kMessageFocusedColor);
    this->mPressed = false;
}

// FUNCTION: EXE 0x660e01
// Draws the button over a light translucent panel
// (EEUIFEBOK::DrawPolygons, in the class)

// SYNTHETIC: EXE 0x660e7b
// (the scalar deleting destructor, emitted with the vtable; slot EEUIFEBOK+0x0)

// FUNCTION: EXE 0x660e9e
// Marks the OK/Yes button pressed and closes the message box
void EEUIFEBOK::ButtonUp()
{
    this->mPressed = true;
    UIButton::ButtonUp();
    gUserInterface->CloseForm(NULL);
}

// FUNCTION: EXE 0x660eb9
// Creates a No or Cancel button (50x20) with its callback and close form unset
EEUIFEBCancel::EEUIFEBCancel(UIForm* inForm, unsigned long inId)
    : UIButton(inId, NULL, NULL, NULL, GERectangle(GE2DPoint(0.0f, 0.0f), 50.0f, 20.0f), inForm, inId, 1),
      mCallback(NULL), mData(NULL), mCloseForm(NULL)
{
    this->SetFocusedTextColor(kMessageFocusedColor);
    this->mPressed = false;
}

// FUNCTION: EXE 0x660f3b
// Draws the button over a light translucent panel
// (EEUIFEBCancel::DrawPolygons, in the class)

// SYNTHETIC: EXE 0x660fb5
// (the scalar deleting destructor, emitted with the vtable; slot EEUIFEBCancel+0x0)

// FUNCTION: EXE 0x660fd8
// Marks the No/Cancel button pressed and closes the message box (or its close form)
void EEUIFEBCancel::ButtonUp()
{
    this->mPressed = true;
    UIButton::ButtonUp();
    gUserInterface->CloseForm(this->mCloseForm);
}

// FUNCTION: EXE 0x660ffb
// Creates the game's message box form: message label, OK, No and Cancel buttons, an edit box and the gold
// framed background, laid out for a 1024x768 design
EEUIFError::EEUIFError(UIManager* inManager)
    : UIForm(7, 1, inManager, -1, true)
{
    GERectangle theScreen(0.0f, 0.0f, (float)gGameWindow->GetWidth(), (float)gGameWindow->GetHeight());
    UILayoutUtility theLayout;
    GETextureDescription theDescription;
    theDescription.mIs3DRenderable = false;
    theDescription.mIsTransparencyEnabled = false;
    theDescription.mPixelDepth = 16;
    theDescription.mNumMipLevelsToDrop = 0;
    theDescription.mSurfaceVolatility = 1;
    theDescription.mFrameFormat = 0;
    theDescription.mHasAlpha = true;
    theDescription.mCreateMIPMaps = true;
    GEFont* theFont = UIFontTable::CreateClosestFont(0x1c, UMFloatToLong((gFontTahoma16->GetHeight() * (1.0f / 768.0f)) *
                                                                         theScreen.GetHeight()));
    this->mError = new UITextLabel(UWideString(""), theFont, 0, GERectangle(GE2DPoint(0.0f, 0.0f), 280.0f, 135.0f), this, 1,
                             0, 0);
    ESVERIFY_AT(this->mError, 220);
    this->mError->SetScrollBarBgColor(kMessageScrollBarColor);
    this->mError->SetScrollBarBgOpacity(kMessageScrollBarOpacity);
    this->mError->SetAlignment(kTextAlignCenter);
    GEText* theGEText = this->mError->GetGEText();
    theGEText->SetIsTransparent(true);
    theGEText->SetFgColor(kMessageTextColor);
    this->AddControl(this->mError, 0);

    this->mOk = new EEUIFEBOK(this, 1);
    ESVERIFY_AT(this->mOk, 236);
    this->AddControl(this->mOk, 0);

    this->mNo = new EEUIFEBCancel(this, 2);
    ESVERIFY_AT(this->mNo, 244);
    this->AddControl(this->mNo, 0);

    this->mCancel = new EEUIFEBCancel(this, 3);
    ESVERIFY_AT(this->mCancel, 252);
    this->AddControl(this->mCancel, 0);

    this->mEditBox = new UIEditBox(4, theFont, 0x32, false, GERectangle(GE2DPoint(0.0f, 0.0f), 220.0f, 24.0f), this, 4, 2,
                             false, false);
    ESVERIFY_AT(this->mEditBox, 260);
    this->mEditBox->GetGEText()->SetIsTransparent(true);
    this->mEditBox->GetGEText()->SetFgColor(kMessageTextColor);
    this->mEditBox->SetSelectedBgColor(kMessageSelectedColor);
    this->mEditBox->SetBgColor(kMessageEditBoxColor);
    this->mEditBox->SetCursorColor(kMessageTextColor);
    this->AddControl(this->mEditBox, 0);
    this->GetEntityGroup(0)->Enable();
    this->mMode = 0;
    theLayout.SetLeft(4, 295.0f / 1024.0f);
    theLayout.SetRight(4, 730.0f / 1024.0f);
    theLayout.SetTop(4, 262.0f / 768.0f);
    theLayout.SetBottom(4, 506.0f / 768.0f);
    this->SetLayout(theLayout);
    theDescription.mPixelDepth = this->GetManager()->GetUITextureBitDepth();
    theDescription.mHasAlpha = true;
    this->mBackground = new EEUIBGenericBackground(0, "textures\\EEGenericBack_Gold", theDescription, theLayout, this, 0,
                                             kMessageBackgroundColor, true, 1.0f, 1.0f, 1.0f, 3);
    theFont->Dereference();
}

// SYNTHETIC: EXE 0x66149b
// (the scalar deleting destructor, emitted with the vtable; slot EEUIFError+0x0)

// FUNCTION: EXE 0x6614b7
// Destroys the message box and its controls
EEUIFError::~EEUIFError()
{
    delete this->mError;
    delete this->mOk;
    delete this->mNo;
    delete this->mCancel;
    delete this->mEditBox;
}

// FUNCTION: EXE 0x66151a
// Shows the message box asking for a text or password, which OK passes to a callback
void EEUIFError::DisplayPassword(const UWideString& inMessage, unsigned long inAlignment,
                                 EEUIFPasswordCallback inCallback, bool inPassword)
{
    UWideString theText;
    GEText* theGEText;
    gResource->GetString(IDS_LOBBY_OK, theText);
    this->mOk->SetText(theText, gFontTahoma12);
    this->mOk->SetCallback((EEUIFErrorCallback)inCallback, (void*)this->mEditBox->GetText(0));
    this->mOk->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mOk->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    this->mError->SetAlignment(inAlignment);
    this->mError->SetText(inMessage);
    this->mError->SetRectangle(GERectangle(GE2DPoint(0.0f, 0.0f), 280.0f, 50.0f));
    this->mNo->SetDisplayMode(kDisplayModeHidden);
    this->mCancel->SetDisplayMode(kDisplayModeHidden);
    this->mEditBox->SetDisplayMode(kDisplayModeShown);
    this->SetTabbing(true);
    this->SetTabStop(this->mEditBox->GetIndex());
    this->mEditBox->SetIsPasswordBox(inPassword);
    this->mMode = 3;
    gUserInterface->SwitchToForm(this);
}

// FUNCTION: EXE 0x66167f
// Shows a message with only a Cancel button, with its callback and the form Cancel closes
void EEUIFError::DisplayCancel(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                               EEUIFErrorCallback inCallback, UIForm* inForm)
{
    UWideString theText;
    GEText* theGEText;
    gResource->GetString(IDS_LOBBY_CANCEL, theText);
    this->mCancel->SetText(theText, gFontTahoma12);
    this->mCancel->SetCallback(inCallback, inData);
    this->mCancel->SetDisplayMode(kDisplayModeShown);
    this->mCancel->mCloseForm = inForm;
    theGEText = this->mCancel->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    this->mError->SetAlignment(inAlignment);
    this->mError->SetText(inMessage);
    this->mNo->SetDisplayMode(kDisplayModeHidden);
    this->mOk->SetDisplayMode(kDisplayModeHidden);
    this->mEditBox->SetDisplayMode(kDisplayModeHidden);
    this->mMode = 4;
    gUserInterface->SwitchToForm(this);
}

// FUNCTION: EXE 0x661788
// Shows a message with an optional OK button and its callback
void EEUIFError::DisplayMessage(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                                EEUIFErrorCallback inCallback, bool inShowOk)
{
    UWideString theText;
    GEText* theGEText;
    gResource->GetString(IDS_LOBBY_OK, theText);
    this->mOk->SetText(theText, gFontTahoma12);
    this->mOk->SetCallback(inCallback, inData);
    this->mOk->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mOk->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    this->mError->SetAlignment(inAlignment);
    this->mError->SetText(inMessage);
    if (inShowOk)
        this->mOk->SetDisplayMode(kDisplayModeShown);
    else
        this->mOk->SetDisplayMode(kDisplayModeHidden);
    this->mNo->SetDisplayMode(kDisplayModeHidden);
    this->mCancel->SetDisplayMode(kDisplayModeHidden);
    this->mEditBox->SetDisplayMode(kDisplayModeHidden);
    this->mMode = 0;
    gUserInterface->SwitchToForm(this);
}

// FUNCTION: EXE 0x661894
// Shows a Yes/No question with a callback for each answer
void EEUIFError::DisplayQuestion(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                                 EEUIFErrorCallback inYes, EEUIFErrorCallback inNo)
{
    UWideString theText;
    GEText* theGEText;
    gResource->GetString(IDS_YES, theText);
    this->mOk->SetText(theText, gFontTahoma12);
    this->mOk->SetCallback(inYes, inData);
    this->mOk->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mOk->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    gResource->GetString(IDS_NO, theText);
    this->mNo->SetText(theText, gFontTahoma12);
    this->mNo->SetCallback(inNo, inData);
    this->mNo->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mNo->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    this->mError->SetAlignment(inAlignment);
    this->mError->SetText(inMessage);
    this->mCancel->SetDisplayMode(kDisplayModeHidden);
    this->mEditBox->SetDisplayMode(kDisplayModeHidden);
    this->mMode = 1;
    gUserInterface->SwitchToForm(this);
}

// FUNCTION: EXE 0x6619f9
// Shows a Yes/No/Cancel question with Yes and No callbacks (Cancel has none)
void EEUIFError::DisplayYesNoCancel(const UWideString& inMessage, unsigned long inAlignment, void* inData,
                                    EEUIFErrorCallback inYes, EEUIFErrorCallback inNo)
{
    UWideString theText;
    GEText* theGEText;
    gResource->GetString(IDS_YES, theText);
    this->mOk->SetText(theText, gFontTahoma12);
    this->mOk->SetCallback(inYes, inData);
    this->mOk->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mOk->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    gResource->GetString(IDS_NO, theText);
    this->mNo->SetText(theText, gFontTahoma12);
    this->mNo->SetCallback(inNo, inData);
    this->mNo->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mNo->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    gResource->GetString(IDS_LOBBY_CANCEL, theText);
    this->mCancel->SetText(theText, gFontTahoma12);
    this->mCancel->SetDisplayMode(kDisplayModeShown);
    theGEText = this->mCancel->GetGEText();
    theGEText->SetFgColor(kMessageTextColor);
    theGEText->SetIsTransparent(true);
    this->mEditBox->SetDisplayMode(kDisplayModeHidden);
    this->mError->SetAlignment(inAlignment);
    this->mError->SetText(inMessage);
    this->mMode = 2;
    gUserInterface->SwitchToForm(this);
}

// FUNCTION: EXE 0x661baf
// Draws the message box background
long EEUIFError::DrawBackground(const GERectangle& inRectangle)
{
    return UIForm::DrawBackground(inRectangle);
}

// FUNCTION: EXE 0x661bbc
// Opens the message box, scales its background to the window and lays out the buttons for its mode
void EEUIFError::Open()
{
    UIForm::Open();
    this->mBackground->SetScale((float)gGameWindow->GetWidth() * (1.0f / 1024.0f),
                          (float)gGameWindow->GetHeight() * (1.0f / 768.0f));
    switch (this->mMode) {
    case 4:
        this->LayoutCancel();
        break;
    case 3:
        this->LayoutPassword();
        break;
    case 2:
        this->LayoutYesNoCancel();
        break;
    case 1:
        this->LayoutQuestion();
        break;
    case 0:
        this->LayoutMessage();
        break;
    }
}

// FUNCTION: EXE 0x661c4c
// Closes the message box and posts the pressed button's callback and data (OK first, then No, then Cancel)
void EEUIFError::Close()
{
    EEUIFErrorCallbackData* theData = new EEUIFErrorCallbackData;
    theData->mCallback = NULL;
    theData->mData = NULL;
    if (this->mOk != NULL && this->mOk->mPressed) {
        theData->mCallback = this->mOk->mCallback;
        theData->mData = this->mOk->mData;
    } else if (this->mNo != NULL && this->mNo->mPressed) {
        theData->mCallback = this->mNo->mCallback;
        theData->mData = this->mNo->mData;
    } else if (this->mCancel != NULL && this->mCancel->mPressed) {
        theData->mCallback = this->mCancel->mCallback;
        theData->mData = this->mCancel->mData;
    }
    gUserInterface->PostErrorClosed(this, theData);
    UIForm::Close();
}

// FUNCTION: EXE 0x661cda
// Lays out the Cancel-only box: the button and the message area
void EEUIFError::LayoutCancel()
{
    UILayoutUtility theLayout;
    GERectangle theScreen(0.0f, 0.0f, (float)gGameWindow->GetWidth(), (float)gGameWindow->GetHeight());
    GERectangle theRectangle;
    theLayout.SetLeft(4, 451.0f / 1024.0f);
    theLayout.SetRight(4, 573.0f / 1024.0f);
    theLayout.SetTop(4, 465.0f / 768.0f);
    theLayout.SetBottom(4, 491.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mCancel->SetRectangle(theRectangle);
    GEFont* theFont = UIFontTable::CreateClosestFont(0x1c, UMFloatToLong((gFontTahoma16->GetHeight() * (1.0f / 768.0f)) *
                                                                         theScreen.GetHeight()));
    this->mCancel->GetGEText()->SetFont(theFont);
    theFont->Dereference();
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.SetTop(4, 300.0f / 768.0f);
    theLayout.SetBottom(4, 460.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mError->SetRectangle(theRectangle);
}

// FUNCTION: EXE 0x661e18
// Lays out the text-entry box: OK, the message and the edit box
void EEUIFError::LayoutPassword()
{
    UILayoutUtility theLayout;
    GERectangle theScreen(0.0f, 0.0f, (float)gGameWindow->GetWidth(), (float)gGameWindow->GetHeight());
    GERectangle theRectangle;
    theLayout.SetLeft(4, 451.0f / 1024.0f);
    theLayout.SetRight(4, 573.0f / 1024.0f);
    theLayout.SetTop(4, 465.0f / 768.0f);
    theLayout.SetBottom(4, 491.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mOk->SetRectangle(theRectangle);
    GEFont* theFont = UIFontTable::CreateClosestFont(0x1c, UMFloatToLong((gFontTahoma16->GetHeight() * (1.0f / 768.0f)) *
                                                                         theScreen.GetHeight()));
    this->mOk->GetGEText()->SetFont(theFont);
    theFont->Dereference();
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.SetTop(4, 300.0f / 768.0f);
    theLayout.SetBottom(4, 420.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mError->SetRectangle(theRectangle);
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.SetTop(4, 430.0f / 768.0f);
    theLayout.SetBottom(4, 460.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mEditBox->SetRectangle(theRectangle);
}

// FUNCTION: EXE 0x661f9c
// Lays out the question box: Yes, No and the message area
void EEUIFError::LayoutQuestion()
{
    UILayoutUtility theLayout;
    GERectangle theScreen(0.0f, 0.0f, (float)gGameWindow->GetWidth(), (float)gGameWindow->GetHeight());
    GERectangle theRectangle;
    theLayout.SetLeft(4, 388.0f / 1024.0f);
    theLayout.SetRight(4, 510.0f / 1024.0f);
    theLayout.SetTop(4, 465.0f / 768.0f);
    theLayout.SetBottom(4, 491.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mOk->SetRectangle(theRectangle);
    GEFont* theFont = UIFontTable::CreateClosestFont(0x1c, UMFloatToLong((gFontTahoma16->GetHeight() * (1.0f / 768.0f)) *
                                                                         theScreen.GetHeight()));
    this->mOk->GetGEText()->SetFont(theFont);
    theLayout.SetLeft(4, 514.0f / 1024.0f);
    theLayout.SetRight(4, 636.0f / 1024.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mNo->SetRectangle(theRectangle);
    this->mNo->GetGEText()->SetFont(theFont);
    theFont->Dereference();
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.SetTop(4, 300.0f / 768.0f);
    theLayout.SetBottom(4, 460.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mError->SetRectangle(theRectangle);
}

// FUNCTION: EXE 0x662121
// Lays out the Yes/No/Cancel box: the three buttons and the message area
void EEUIFError::LayoutYesNoCancel()
{
    UILayoutUtility theLayout;
    GERectangle theScreen(0.0f, 0.0f, (float)gGameWindow->GetWidth(), (float)gGameWindow->GetHeight());
    GERectangle theRectangle;
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 447.0f / 1024.0f);
    theLayout.SetTop(4, 465.0f / 768.0f);
    theLayout.SetBottom(4, 491.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mOk->SetRectangle(theRectangle);
    GEFont* theFont = UIFontTable::CreateClosestFont(0x1c, UMFloatToLong((gFontTahoma16->GetHeight() * (1.0f / 768.0f)) *
                                                                         theScreen.GetHeight()));
    this->mOk->GetGEText()->SetFont(theFont);
    theLayout.SetLeft(4, 451.0f / 1024.0f);
    theLayout.SetRight(4, 573.0f / 1024.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mNo->SetRectangle(theRectangle);
    this->mNo->GetGEText()->SetFont(theFont);
    theLayout.SetLeft(4, 577.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mCancel->SetRectangle(theRectangle);
    this->mCancel->GetGEText()->SetFont(theFont);
    theFont->Dereference();
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.SetTop(4, 300.0f / 768.0f);
    theLayout.SetBottom(4, 460.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mError->SetRectangle(theRectangle);
}

// FUNCTION: EXE 0x6622ed
// Lays out the message box: OK, and the message area centred vertically for short messages
void EEUIFError::LayoutMessage()
{
    UILayoutUtility theLayout;
    GERectangle theScreen(0.0f, 0.0f, (float)gGameWindow->GetWidth(), (float)gGameWindow->GetHeight());
    GERectangle theRectangle;
    theLayout.SetLeft(4, 451.0f / 1024.0f);
    theLayout.SetRight(4, 573.0f / 1024.0f);
    theLayout.SetTop(4, 465.0f / 768.0f);
    theLayout.SetBottom(4, 491.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    this->mOk->SetRectangle(theRectangle);
    GEFont* theFont = UIFontTable::CreateClosestFont(0x1c, UMFloatToLong((gFontTahoma16->GetHeight() * (1.0f / 768.0f)) *
                                                                         theScreen.GetHeight()));
    this->mOk->GetGEText()->SetFont(theFont);
    theFont->Dereference();
    theLayout.SetLeft(4, 325.0f / 1024.0f);
    theLayout.SetRight(4, 699.0f / 1024.0f);
    theLayout.SetTop(4, 300.0f / 768.0f);
    theLayout.SetBottom(4, 460.0f / 768.0f);
    theLayout.CalculateRectangle(theScreen, theRectangle, true);
    unsigned long theLines = UMFloor(theRectangle.GetHeight() / gFontTahoma12->GetHeight()) - 2;
    float theOffset;
    if (theLines > this->mError->GetLineCount())
        theOffset = (float)(theLines - this->mError->GetLineCount()) * gFontTahoma12->GetHeight() * 0.5f;
    else
        theOffset = 0.0f;
    theRectangle.SetTop(theRectangle.GetTop() + theOffset);
    theRectangle.SetBottom(theRectangle.GetBottom() - theOffset);
    this->mError->SetRectangle(theRectangle);
}

// SYNTHETIC: EXE 0x660c27
// SYNTHETIC: EXE 0x660c31
// SYNTHETIC: EXE 0x660c4a
// SYNTHETIC: EXE 0x660c56
// SYNTHETIC: EXE 0x660c57
// SYNTHETIC: EXE 0x660c61
// SYNTHETIC: EXE 0x660c7a
// SYNTHETIC: EXE 0x660c86
// SYNTHETIC: EXE 0x660c87
// SYNTHETIC: EXE 0x660c91
// SYNTHETIC: EXE 0x660caa
// SYNTHETIC: EXE 0x660cb6
// SYNTHETIC: EXE 0x660cb7
// SYNTHETIC: EXE 0x660cc1
// SYNTHETIC: EXE 0x660cde
// SYNTHETIC: EXE 0x660cea
// SYNTHETIC: EXE 0x660ceb
// SYNTHETIC: EXE 0x660cf5
// SYNTHETIC: EXE 0x660d0e
// SYNTHETIC: EXE 0x660d1a
// SYNTHETIC: EXE 0x660d1b
// SYNTHETIC: EXE 0x660d25
// SYNTHETIC: EXE 0x660d42
// SYNTHETIC: EXE 0x660d4e
// SYNTHETIC: EXE 0x660d4f
// SYNTHETIC: EXE 0x660d59
// SYNTHETIC: EXE 0x660d78
// SYNTHETIC: EXE 0x660d84
