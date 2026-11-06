// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Original-backed port dialog; external GUI widget lifetimes remain retained.
#include <windows.h>
#include <stdio.h>
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
LanPortDialog::LanPortDialog() {}
// FUNCTION: WON 0x7f85d0
LanPortDialog::~LanPortDialog() {}
// FUNCTION: WON 0x7f86c0
void LanPortDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* aConfig = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig, mPortInput, "PortInput", __FILE__, 32);
        mPortInput->mInputFilter = InputBox::NumericFilter;
        SafeGetComponent(aConfig, mGamePortLabel, "GamePortLabel", __FILE__, 35);
        SafeGetComponent(aConfig, mOkButton, "OKButton", __FILE__, 36);
    }
}
// FUNCTION: WON 0x7f8730
void LanPortDialog::Reset()
{
    mPortInput->Fn2F5890();
    CheckInput();
}
// FUNCTION: WON 0x7f8750
void LanPortDialog::SetPort(unsigned short inPort)
{
    char aBuf[10];
    sprintf(aBuf, "%d", inPort);
    mPortInput->SetText(GUIString(new GUIStringData(aBuf)), true);
    mPortInput->SelectRange(0, -1);
}
// FUNCTION: WON 0x7f8840
void LanPortDialog::SetGamePort(unsigned short inPort)
{
    mGamePort = inPort;
    char aBuf[10];
    sprintf(aBuf, "%d", inPort);
    mGamePortLabel->SetText(GUIString(new GUIStringData(aBuf)), true);
}
// FUNCTION: WON 0x7f8920
unsigned short LanPortDialog::GetPort()
{
    return mPortInput->GetText().mData->ToInt(0);
}
// FUNCTION: WON 0x7f8940
void LanPortDialog::CheckInput()
{
    int aPort = mPortInput->GetText().mData->ToInt(0);
    if (aPort != mGamePort && aPort > 4999 && aPort < 65536) {
        if (mOkButton->IsDisabled())
            mOkButton->SetEnabled(true);
    }
    else if (!mOkButton->IsDisabled())
        mOkButton->SetEnabled(false);
}
// FUNCTION: WON 0x7f89b0
void LanPortDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 21 && inEvent->mComponent == mPortInput.get())
        CheckInput();
    else
        Dialog::HandleComponentEvent(inEvent);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f8580
// (public: __thiscall WONAPI::LanPortDialog::LanPortDialog(void)    [??0LanPortDialog@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007f85b0
// (public: virtual void * __thiscall WONAPI::LanPortDialog::`scalar deleting destructor'(unsigned int)    [??_GLanPortDialog@WONAPI@@UAEPAXI@Z])
