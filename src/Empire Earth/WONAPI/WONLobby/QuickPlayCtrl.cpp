
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
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
// Semantically empty constructor56 and implicit normalcleanup335 are zero

QuickPlayCtrl::QuickPlayCtrl() {}
// Original3c1690..3c174e,190B; actual typed resource names and lines29..34.
// FUNCTION: WON 0x007c1690
void QuickPlayCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mWelcomeLabel,"WelcomeLabel",__FILE__,29);
        SafeGetComponent(config,mNoviceRadio,"NoviceRadio",__FILE__,30);
        SafeGetComponent(config,mIntermediateRadio,"IntermediateRadio",__FILE__,31);
        SafeGetComponent(config,mAdvancedRadio,"AdvancedRadio",__FILE__,32);
        SafeGetComponent(config,mExpertRadio,"ExpertRadio",__FILE__,33);
        SafeGetComponent(config,mPlayButton,"PlayButton",__FILE__,34);
        Fn2F5360(1);
    }
}
// Original3c1750..3c17fc,172B. Borrowed mutable current user record.
// FUNCTION: WON 0x007c1750
void QuickPlayCtrl::RefreshUserName()
{
    LobbyUserInfo* user=LobbyFn35CDE0();
    if(user!=NULL)
        mWelcomeLabel->SetText(LobbyFormatString(::InternetQuickPlay_UserWelcome_String,user->mUserName),true);
    else
        mWelcomeLabel->SetText(::InternetQuickPlay_Welcome_String,true);
}
// Original missing scan entry3c1800..3c1911,273B. Event24 allocation guard.
// FUNCTION: WON 0x007c1800
void QuickPlayCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 100014:
        RefreshUserName();
        return;
    case 15:
      if(inEvent->mComponent==mPlayButton.get()) {
        int gameType=0;
        if(mNoviceRadio->IsChecked()) gameType=1;
        else if(mIntermediateRadio->IsChecked()) gameType=2;
        else if(mAdvancedRadio->IsChecked()) gameType=3;
        else if(mExpertRadio->IsChecked()) gameType=4;
        Fn2F53F0(new QuickPlayEvent(gameType),false);
      }
      break;
    }
    Dialog::HandleComponentEvent(inEvent);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c1520
// (public: virtual void * __thiscall WONAPI::QuickPlayCtrl::`scalar deleting destructor'(unsigned int)    [??_GQuickPlayCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c1540
// (public: virtual __thiscall WONAPI::QuickPlayCtrl::~QuickPlayCtrl(void)    [??1QuickPlayCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c1920
// (public: virtual void * __thiscall WONAPI::QuickPlayEvent::`scalar deleting destructor'(unsigned int)    [??_GQuickPlayEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c1940
// (public: virtual __thiscall WONAPI::QuickPlayEvent::~QuickPlayEvent(void)    [??1QuickPlayEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c14e0
// (public: __thiscall WONAPI::QuickPlayCtrl::QuickPlayCtrl(void)    [??0QuickPlayCtrl@WONAPI@@QAE@XZ])
