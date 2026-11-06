// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Original-backed status dialog; GUI widget implementations remain external.
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
// FUNCTION: WON 0x7f91b0
StatusDialog::StatusDialog() { mStartTick = GetTickCount(); }
// Original ordinary destructor is implicit; generated cleanup has zero body quota.
// FUNCTION: WON 0x7f9370
void StatusDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mTitleLabel, "TitleLabel", __FILE__, 28);
        SafeGetComponent(aConfig, mCloseButton, "CloseButton", __FILE__, 29);
        SafeGetComponent(aConfig, mStatusLabel, "StatusLabel", __FILE__, 30);
        SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 31);
        SafeGetComponent(aConfig, mBack, "Back", __FILE__, 33);
        SafeGetComponent(aConfig, mDialogComponent, "DialogComponent", __FILE__, 34);
    }
}
// FUNCTION: WON 0x7f9430
void StatusDialog::SetStatus(const GUIString& inStatus, bool inCancel)
{
    SetText(mTitleLabel->mWrappedText->mText, inStatus, inCancel);
}
// FUNCTION: WON 0x7f9450
void StatusDialog::SetText(const GUIString& inTitle, const GUIString& inStatus, bool inCancel)
{
    bool anOldB8C = mB8C;
    int anInitialWidth = Width();
    int anInitialHeight = Height();
    int anInitialStatusWidth = mStatusLabel->Width();
    int anInitialStatusHeight = mStatusLabel->Height();
    int anInitialComponentWidth = mDialogComponent->Width();
    int anInitialComponentHeight = mDialogComponent->Height();
    int anInitialButtonY = mCancelButton->Top();
    mTitleLabel->SetText(inTitle, true);
    mTitleLabel->SetDesiredWidth(0);
    mCancelButton->SetText(inCancel ? ::LobbyGlobal_Cancel_String : ::LobbyGlobal_OK_String, true);
    mCancelButton->Fn2F4E90(0, 0);
    mCancelButton->Fn2F4BC0(100, 0);
    mStatusLabel->SetVertPad(0, 0);
    mStatusLabel->SetText(gEmptyGUIString, true);
    mStatusLabel->Fn2F4B90(0, 0);
    Fn2F4B90(0, 0);
    Fn2F4BC0(mCloseButton->Width() + mTitleLabel->Width() +
              mBack->VirtualC8() + mBack->VirtualD0() + 5, 0);
    mDialogComponent->Fn2F4B90(0, 0);
    mDialogComponent->Fn2F4E90(0, 0);
    Fn2F4E90(20, 20);
    int anOldWidth = Width();
    int anOldHeight = Height();
    int anOldStatusHeight = mStatusLabel->Height();
    int aRootWidth = GetLobbyRoot()->Width();
    int aRootHeight = GetLobbyRoot()->Height();
    int aBodyWidth = mStatusLabel->mWrappedText->mFont->GetStringWidth(inStatus, 0, inStatus.length());
    int aMinWidth = mStatusLabel->mWrappedText->mFont->Virtual10('M') * 30;
    if (aBodyWidth < aMinWidth)
        aBodyWidth = aMinWidth;
    if (aBodyWidth > aRootWidth / 2)
        aBodyWidth = aRootWidth / 2;
    int aWidthChange = aBodyWidth - mStatusLabel->Width();
    if (aWidthChange < 0)
        aWidthChange = 0;
    mStatusLabel->Fn2F4B90(aBodyWidth, mStatusLabel->Height());
    mStatusLabel->SetText(inStatus, true);
    int aNumLines = mStatusLabel->Height() / mStatusLabel->mWrappedText->mFont->GetHeight();
    if (aNumLines < 5) {
        int aPad = (5 - aNumLines) * mStatusLabel->mWrappedText->mFont->GetHeight() / 2;
        mStatusLabel->SetVertPad(aPad, aPad);
        mStatusLabel->Fn2F4E90(0, 0);
    }
    int aHeightChange = mStatusLabel->Height() - anOldStatusHeight;
    if (aHeightChange < 0)
        aHeightChange = 0;
    if (anOldHeight + aHeightChange > anOldWidth + aWidthChange ||
        anOldHeight + aHeightChange >= aRootHeight) {
        int anExtraWidth = (anOldWidth + aWidthChange) * 5 / 4 - aWidthChange - anOldWidth;
        aWidthChange += anExtraWidth;
        mStatusLabel->Fn2F4B90(aBodyWidth + anExtraWidth, mStatusLabel->Height());
        aHeightChange = mStatusLabel->Height() - anOldStatusHeight;
    }
    Fn2F4B90(anOldWidth + aWidthChange, anOldHeight + aHeightChange);
    if (GetTickCount() - mStartTick < 50) {
        mStatusLabel->Fn2F4BC0(anInitialStatusWidth, anInitialStatusHeight);
        mDialogComponent->Fn2F4BC0(anInitialComponentWidth, anInitialComponentHeight);
        Fn2F4BC0(anInitialWidth, anInitialHeight);
        if (anInitialButtonY > mCancelButton->Top())
            mCancelButton->Fn2F4B70(mCancelButton->Left(), anInitialButtonY);
    }
    Invalidate();
    mB8C = anOldB8C;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f9200
// (public: virtual void * __thiscall WONAPI::StatusDialog::`scalar deleting destructor'(unsigned int)    [??_GStatusDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f9220
// (public: virtual __thiscall WONAPI::StatusDialog::~StatusDialog(void)    [??1StatusDialog@WONAPI@@UAE@XZ])
