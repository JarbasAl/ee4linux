// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Original-backed dialog geometry; GUI widget implementations remain external.
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// Natural empty constructor: automatic vptr/defaultmember initialization, zero substantive body credit.
OkCancelDialog::OkCancelDialog() {}

// FUNCTION: WON 0x7f8bc0
void OkCancelDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mTitleLabel, "TitleLabel", __FILE__, 26);
        SafeGetComponent(aConfig, mCloseButton, "CloseButton", __FILE__, 27);
        SafeGetComponent(aConfig, mBodyLabel, "BodyLabel", __FILE__, 28);
        SafeGetComponent(aConfig, mOkButton, "OKButton", __FILE__, 30);
        SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 31);
        SafeGetComponent(aConfig, mBack, "Back", __FILE__, 33);
        SafeGetComponent(aConfig, mDialogComponent, "DialogComponent", __FILE__, 34);
    }
}
// FUNCTION: WON 0x7f8c90
void OkCancelDialog::SetText(const GUIString& inTitle, const GUIString& inBody,
                            const GUIString& inOkText, const GUIString& inCancelText)
{
    mTitleLabel->SetText(inTitle, true);
    mTitleLabel->SetDesiredWidth(0);
    mOkButton->SetText(inOkText, true);
    mOkButton->Fn2F4E90(0, 0);
    mOkButton->Fn2F4BC0(100, 0);
    mCancelButton->SetText(inCancelText, true);
    mCancelButton->Fn2F4E90(0, 0);
    mCancelButton->Fn2F4BC0(100, 0);
    mBodyLabel->SetVertPad(0, 0);
    mBodyLabel->SetText(gEmptyGUIString, true);
    mBodyLabel->Fn2F4B90(0, 0);
    Fn2F4B90(0, 0);
    Fn2F4BC0(mTitleLabel->Width() + mCloseButton->Width() +
              mBack->VirtualC8() + mBack->VirtualD0() + 5, 0);
    mDialogComponent->Fn2F4B90(0, 0);
    mDialogComponent->Fn2F4E90(0, 0);
    Fn2F4E90(20, 20);
    int anOldHeight = Height();
    int anOldWidth = Width();
    int anOldBodyHeight = mBodyLabel->Height();
    int aTextWidth = mBodyLabel->mWrappedText->mFont->GetStringWidth(inBody, 0, inBody.length());
    int aBodyWidth;
    if (aTextWidth < 250)
        aBodyWidth = aTextWidth;
    else if (aTextWidth < 800)
        aBodyWidth = 250;
    else
        aBodyWidth = 400;
    int aWidthChange = aBodyWidth - mBodyLabel->Width();
    if (aWidthChange < 0)
        aWidthChange = 0;
    mBodyLabel->Fn2F4B90(aBodyWidth, mBodyLabel->Height());
    mBodyLabel->SetText(inBody, true);
    int aNumLines = mBodyLabel->Height() / mBodyLabel->mWrappedText->mFont->GetHeight();
    if (aNumLines < 5) {
        int aPad = (5 - aNumLines) * mBodyLabel->mWrappedText->mFont->GetHeight() / 2;
        mBodyLabel->SetVertPad(aPad, aPad);
        mBodyLabel->Fn2F4E90(0, 0);
    }
    int aHeightChange = mBodyLabel->Height() - anOldBodyHeight;
    if (aHeightChange < 0)
        aHeightChange = 0;
    // The original calls the root accessor twice; its first result is discarded.
    int aRootWidth = GetLobbyRoot()->Width();
    int aRootHeight = GetLobbyRoot()->Height();
    if (anOldHeight + aHeightChange > anOldWidth + aWidthChange ||
        anOldHeight + aHeightChange >= aRootHeight) {
        int anExtraWidth = (anOldWidth + aWidthChange) * 5 / 4 - aWidthChange - anOldWidth;
        aWidthChange += anExtraWidth;
        mBodyLabel->Fn2F4B90(aBodyWidth + anExtraWidth, mBodyLabel->Height());
        aHeightChange = mBodyLabel->Height() - anOldBodyHeight;
    }
    Fn2F4B90(anOldWidth + aWidthChange, anOldHeight + aHeightChange);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f8a40
// (public: virtual __thiscall WONAPI::OkCancelDialog::~OkCancelDialog(void)    [??1OkCancelDialog@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007fa620

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f8a20
// (public: virtual void * __thiscall WONAPI::OkCancelDialog::`scalar deleting destructor'(unsigned int)    [??_GOkCancelDialog@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f89e0
// (public: __thiscall WONAPI::OkCancelDialog::OkCancelDialog(void)    [??0OkCancelDialog@WONAPI@@QAE@XZ])
