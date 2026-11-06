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
PasswordDialog::PasswordDialog() {}

// Original ordinary destructor is implicit; review generated cleanup separately, zero body quota.

// FUNCTION: WON 0x7f9050
void PasswordDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mPasswordInput, "PasswordInput", __FILE__, 24);
        SafeGetComponent(aConfig, mOkButton, "OkButton", __FILE__, 25);
        SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 26);
    }
}

// FUNCTION: WON 0x7f90c0
const GUIString& PasswordDialog::GetPassword()
{
    return mPasswordInput->GetText();
}

}

namespace WONAPI {
// SUPPORT-ONLY: genuine SDK explicit PasswordDialog::Reset; shared native physical body, zero new physical/primary credit.
void PasswordDialog::Reset()
{
    mPasswordInput->Clear(false);
    mPasswordInput->Fn2F5890();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f8f50
// (public: virtual void * __thiscall WONAPI::PasswordDialog::`scalar deleting destructor'(unsigned int)    [??_GPasswordDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f8f70
// (public: virtual __thiscall WONAPI::PasswordDialog::~PasswordDialog(void)    [??1PasswordDialog@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f8f20
// (public: __thiscall WONAPI::PasswordDialog::PasswordDialog(void)    [??0PasswordDialog@WONAPI@@QAE@XZ])
