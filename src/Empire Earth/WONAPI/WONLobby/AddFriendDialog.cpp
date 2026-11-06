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
AddFriendDialog::AddFriendDialog() {}



// FUNCTION: WON 0x7f4980
void AddFriendDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mNameInput, "NameInput", __FILE__, 25);
        SafeGetComponent(aConfig, mOkButton, "OkButton", __FILE__, 26);
        SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 27);
    }
}

// FUNCTION: WON 0x7f49f0
void AddFriendDialog::CheckInput()
{
    mOkButton->SetEnabled(mNameInput->mText.length() != 0);
}

// FUNCTION: WON 0x7f4a20
void AddFriendDialog::Reset()
{
    mNameInput->Clear(true);
    mNameInput->Fn2F5890();
}

// FUNCTION: WON 0x7f4a40
void AddFriendDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 21)
        CheckInput();
    else
        Dialog::HandleComponentEvent(inEvent);
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f4880
// (public: virtual void * __thiscall WONAPI::AddFriendDialog::`scalar deleting destructor'(unsigned int)    [??_GAddFriendDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f48a0
// (public: virtual __thiscall WONAPI::AddFriendDialog::~AddFriendDialog(void)    [??1AddFriendDialog@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f4850
// (public: __thiscall WONAPI::AddFriendDialog::AddFriendDialog(void)    [??0AddFriendDialog@WONAPI@@QAE@XZ])
