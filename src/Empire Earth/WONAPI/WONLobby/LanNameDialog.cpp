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
LanNameDialog::LanNameDialog() {}

// FUNCTION: WON 0x7f8400
LanNameDialog::~LanNameDialog() {}

// FUNCTION: WON 0x7f84f0
void LanNameDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mNameInput, "LanNameInput", __FILE__, 31);
        SafeGetComponent(aConfig, mOkButton, "OkButton", __FILE__, 32);
        SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 33);
    }
}

// FUNCTION: WON 0x7f8560
void LanNameDialog::Reset()
{
    mNameInput->Clear(false);
    mNameInput->Fn2F5890();
}

}

namespace WONAPI {
// SUPPORT-ONLY: genuine SDK explicit LanNameDialog::GetName; shared native physical body, zero new physical/primary credit.
const GUIString& LanNameDialog::GetName()
{
    return mNameInput->GetText();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f83b0
// (public: __thiscall WONAPI::LanNameDialog::LanNameDialog(void)    [??0LanNameDialog@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007f83e0
// (public: virtual void * __thiscall WONAPI::LanNameDialog::`scalar deleting destructor'(unsigned int)    [??_GLanNameDialog@WONAPI@@UAEPAXI@Z])
