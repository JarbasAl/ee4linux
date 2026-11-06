// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// Natural empty constructor: automatic vptr/defaultmember initialization, zero substantive body credit.
CreateRoomDialog::CreateRoomDialog() {}
// Original ordinary destructor is implicit; generated cleanup has zero body quota.
// FUNCTION: WON 0x7f6bc0
void CreateRoomDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mNameInput, "RoomNameInput", __FILE__, 25);
        SafeGetComponent(aConfig, mPasswordInput, "PasswordInput", __FILE__, 26);
        SafeGetComponent(aConfig, mPasswordLabel, "PasswordLabel", __FILE__, 27);
        SafeGetComponent(aConfig, mPasswordCheck, "PasswordCheck", __FILE__, 28);
        SafeGetComponent(aConfig, mCreateButton, "CreateButton", __FILE__, 29);
        CheckInput();
    }
}
// FUNCTION: WON 0x7f6c70
void CreateRoomDialog::Reset(bool inClear)
{
    mNameInput->Fn2F5890();
    if (inClear) {
        mNameInput->Clear(false);
        mPasswordInput->Clear(false);
        mPasswordCheck->SetCheck(false, false);
        UpdatePasswordVisibility();
    }
    CheckInput();
}
// FUNCTION: WON 0x7f6cd0
void CreateRoomDialog::UpdatePasswordVisibility()
{
    mPasswordInput->SetVisible(mPasswordCheck->mChecked);
    mPasswordLabel->SetVisible(mPasswordCheck->mChecked);
    CheckInput();
}
// FUNCTION: WON 0x7f6d20
void CreateRoomDialog::CheckInput()
{
    bool aEnable = true;
    if (mNameInput->mText.length() == 0 ||
        (mPasswordInput->IsVisible() && mPasswordInput->mText.length() == 0))
        aEnable = false;
    mCreateButton->SetEnabled(aEnable);
}
// FUNCTION: WON 0x7f6d70
const GUIString& CreateRoomDialog::GetPassword()
{
    if (mPasswordCheck->IsChecked())
        return mPasswordInput->GetText();
    return gEmptyGUIString;
}
// FUNCTION: WON 0x7f6da0
void CreateRoomDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 15 && inEvent->mComponent == mPasswordCheck.get())
        UpdatePasswordVisibility();
    else if (inEvent->mType == 21)
        CheckInput();
    else
        Dialog::HandleComponentEvent(inEvent);
}
// Natural ordinary shared getter; no duplicate physical body credit or marker. Root alias gate pending.
const GUIString& CreateRoomDialog::GetRoomName()
{
    return mNameInput->GetText();
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f6a90
// (public: virtual __thiscall WONAPI::CreateRoomDialog::~CreateRoomDialog(void)    [??1CreateRoomDialog@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f6a70
// (public: virtual void * __thiscall WONAPI::CreateRoomDialog::`scalar deleting destructor'(unsigned int)    [??_GCreateRoomDialog@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f6a30
// (public: __thiscall WONAPI::CreateRoomDialog::CreateRoomDialog(void)    [??0CreateRoomDialog@WONAPI@@QAE@XZ])
