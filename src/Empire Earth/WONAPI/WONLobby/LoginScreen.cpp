// Independent R3 source intent: no image-global, HTML or operation allocation gate.
// ScreenContainer88/ShowScreen external contract closed in GUI D; next own header lease pending.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
// Constructor/default destruction support is zero primary quota.
LoginScreen::LoginScreen() {}
// Original3c0060..3c0123,195B; genericComponent5 +actualScreenContainer lookup last.
// FUNCTION: WON 0x007c0060
void LoginScreen::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(config,mMOTDCtrl,"MOTDCtrl",__FILE__,26);
    SafeGetComponent(config,mCreateAccountCtrl,"CreateAccountCtrl",__FILE__,27);
    SafeGetComponent(config,mLostUserNameCtrl,"LostUserNameCtrl",__FILE__,28);
    SafeGetComponent(config,mLostPasswordCtrl,"LostPasswordCtrl",__FILE__,29);
    SafeGetComponent(config,mLoginHelpCtrl,"LoginHelpCtrl",__FILE__,30);
    SafeGetComponent(config,mRightPanel,"RightPanel",__FILE__,31);
    mRightPanel->ShowScreen(mMOTDCtrl.get(),false);
}
// Original3c0130..3c0174,68B. Source sender ID, not this screen ID.
// FUNCTION: WON 0x007c0130
void LoginScreen::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==15) {
        switch(inEvent->mComponent->mControlId) {
        case 200007: mRightPanel->ShowScreen(mLoginHelpCtrl.get(),true); return;
        case 200004: ShowMOTD(); return;
        }
    }
    Component::HandleComponentEvent(inEvent);
}
// Thin screen transition wrappers21B each: support0, preserve ordinary implementations.
// FUNCTION: WON 0x007c0180
void LoginScreen::ShowMOTD() { mRightPanel->ShowScreen(mMOTDCtrl.get(),true); }
// FUNCTION: WON 0x007c01a0
void LoginScreen::ShowCreateAccountCtrl() { mRightPanel->ShowScreen(mCreateAccountCtrl.get(),true); }
// FUNCTION: WON 0x007c01c0
void LoginScreen::ShowLostPasswordCtrl() { mRightPanel->ShowScreen(mLostPasswordCtrl.get(),true); }
// FUNCTION: WON 0x007c01e0
void LoginScreen::ShowLostUsernameCtrl() { mRightPanel->ShowScreen(mLostUserNameCtrl.get(),true); }
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007bff30
// (public: virtual __thiscall WONAPI::LoginScreen::~LoginScreen(void)    [??1LoginScreen@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007ce1d0

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007bff10
// (public: virtual void * __thiscall WONAPI::LoginScreen::`scalar deleting destructor'(unsigned int)    [??_GLoginScreen@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007bfed0
// (public: __thiscall WONAPI::LoginScreen::LoginScreen(void)    [??0LoginScreen@WONAPI@@QAE@XZ])
