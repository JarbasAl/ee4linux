// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// FUNCTION: WON 0x7f63e0
CreateGameDialog::CreateGameDialog() : mGameType(0) {}
// FUNCTION: WON 0x7f6450
CreateGameDialog::~CreateGameDialog() {}
// FUNCTION: WON 0x7f6600
void CreateGameDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mNameInput, "GameNameInput", __FILE__, 34);
        SafeGetComponent(aConfig, mSkillCombo, "SkillCombo", __FILE__, 35);
        SafeGetComponent(aConfig, mPasswordInput, "PasswordInput", __FILE__, 36);
        SafeGetComponent(aConfig, mPasswordLabel, "PasswordLabel", __FILE__, 37);
        SafeGetComponent(aConfig, mOpenToAllRadio, "OpenToAllRadio", __FILE__, 38);
        SafeGetComponent(aConfig, mPasswordRadio, "PasswordRadio", __FILE__, 39);
        SafeGetComponent(aConfig, mInviteOnlyRadio, "InviteOnlyRadio", __FILE__, 40);
        SafeGetComponent(aConfig, mAskToJoinRadio, "AskToJoinRadio", __FILE__, 41);
        SafeGetComponent(aConfig, mCreateButton, "CreateButton", __FILE__, 42);
    }
}
// FUNCTION: WON 0x7f6710
void CreateGameDialog::Reset(bool inClear)
{
    mNameInput->Fn2F5890();
    if (inClear) {
        mOpenToAllRadio->SetCheck(true, false);
        mSkillCombo->SetSelItem(0, false);
        mNameInput->Clear(false);
        mPasswordInput->Clear(false);
        UpdatePasswordVisibility();
    }
    CheckInput();
}
// FUNCTION: WON 0x7f6770
void CreateGameDialog::SetGameType(int inType)
{
    mGameType = inType;
    bool aInternet = inType == 2;
    if (mInviteOnlyRadio->IsChecked() || mAskToJoinRadio->IsChecked())
        mOpenToAllRadio->SetCheck(true, false);
    mInviteOnlyRadio->SetVisible(aInternet);
    mAskToJoinRadio->SetVisible(aInternet);
}
// FUNCTION: WON 0x7f67e0
SmartPtr<LobbyGame> CreateGameDialog::GetGame()
{
    SmartPtr<LobbyGame> aGame(LobbyGame::CreateGame(static_cast<LobbyGameType>(mGameType)));
    aGame->SetName(mNameInput->GetText());
    if (mPasswordRadio->IsChecked())
        aGame->SetPassword(mPasswordInput->GetText());
    aGame->SetAskToJoin(mAskToJoinRadio->IsChecked());
    aGame->SetInviteOnly(mInviteOnlyRadio->IsChecked());
    int aSkill = mSkillCombo->GetSelItem();
    if (aSkill < 0 || aSkill >= 5)
        aSkill = 0;
    aGame->SetSkill(aSkill);
    return aGame;
}
// FUNCTION: WON 0x7f6920
void CreateGameDialog::CheckInput()
{
    bool aEnable = true;
    if (mNameInput->GetText().length() == 0)
        aEnable = false;
    else if (mPasswordInput->IsVisible() && mPasswordInput->mText.length() == 0)
        aEnable = false;
    mCreateButton->SetEnabled(aEnable);
}
// FUNCTION: WON 0x7f6980
void CreateGameDialog::UpdatePasswordVisibility()
{
    mPasswordInput->SetVisible(mPasswordRadio->mChecked);
    mPasswordLabel->SetVisible(mPasswordRadio->mChecked);
    CheckInput();
}
// FUNCTION: WON 0x7f69d0
void CreateGameDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 15)
        UpdatePasswordVisibility();
    else if (inEvent->mType == 21) {
        CheckInput();
        return;
    }
    else if (inEvent->mType == 27 && inEvent->mComponent == mSkillCombo.get()) {
        CheckInput();
        return;
    }
    Dialog::HandleComponentEvent(inEvent);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f6430
// (protected: virtual void * __thiscall WONAPI::CreateGameDialog::`scalar deleting destructor'(unsigned int)    [??_GCreateGameDialog@WONAPI@@MAEPAXI@Z])
