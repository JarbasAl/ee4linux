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
DirectConnectDialog::DirectConnectDialog() {}
// Original ordinary destructor is implicit; generated cleanup has zero body quota.
// FUNCTION: WON 0x7f7aa0
void DirectConnectDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mNameInput, "NameInput", __FILE__, 25);
        SafeGetComponent(aConfig, mAddressInput, "AddressInput", __FILE__, 26);
        SafeGetComponent(aConfig, mAddButton, "AddButton", __FILE__, 27);
        SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 28);
    }
}
// FUNCTION: WON 0x7f7b20
void DirectConnectDialog::Reset()
{
    mNameInput->Fn2F5890();
    mNameInput->Clear(false);
    mAddressInput->Clear(false);
}
// FUNCTION: WON 0x7f7b50
const GUIString& DirectConnectDialog::GetAddress()
{
    return mAddressInput->GetText();
}
// Natural ordinary shared getter; no duplicate physical body credit or marker. Root alias gate pending.
const GUIString& DirectConnectDialog::GetName()
{
    return mNameInput->GetText();
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f7970
// (public: virtual void * __thiscall WONAPI::DirectConnectDialog::`scalar deleting destructor'(unsigned int)    [??_GDirectConnectDialog@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x007f7990
// (public: virtual __thiscall WONAPI::DirectConnectDialog::~DirectConnectDialog(void)    [??1DirectConnectDialog@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f7940
// (public: __thiscall WONAPI::DirectConnectDialog::DirectConnectDialog(void)    [??0DirectConnectDialog@WONAPI@@QAE@XZ])
