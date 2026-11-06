// Native lobby modal loop, suspension and end-state control.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
extern SmartPtr<Dialog> gLobbyFailureDialog5662F4; //existing original-owned data
extern bool gLobbyInitFailed56633C; //existing original-owned data
void LobbyFn361D90(); //actual noarg NetLogic Virtual44 dispatch provider
void LobbyFn361DB0(); //actual noarg NetLogic Virtual48 dispatch provider
// FUNCTION: WON 0x00762760
int LobbyFn362760()
{
    if(gLobbyInitFailed56633C && gLobbyFailureDialog5662F4.get()!=NULL) {
        Window* window=gLobbyFailureDialog5662F4->Fn2F5940();
        if(window!=NULL && window->Virtual4C())
            window->RunDialog(gLobbyFailureDialog5662F4.get());
        return 0;
    }
    if(gLobbyData.get()==NULL) return 0;
    gLobbyData->mLobbyStatus=static_cast<LobbyStatus>(0);
    gLobbyData->mRunMode=static_cast<LobbyRunMode>(1);
    gLobbyData->mSuspended=false;
    WindowManager* manager=GetLobbyWindow();
    if(manager==NULL) return 0;
    manager->MessageLoop();
    if(gLobbyData.get()==NULL) return 0;
    return gLobbyData->mLobbyStatus;
}
// FUNCTION: WON 0x007627f0
void LobbyFn3627F0(int status)
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mLobbyStatus=static_cast<LobbyStatus>(status);
    LobbyRunMode mode=gLobbyData->mRunMode;
    LobbyFn359120(100013,true);
    if(mode!=0) {
        WindowManager* manager=GetLobbyWindow();
        if(manager!=NULL) manager->ExitMessageLoop();
    }
}
// FUNCTION: WON 0x00762830
void LobbyFn362830()
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mSuspended=true;
    ::LobbyGlobal_LobbyMusic_Sound->Stop();
    ::LobbyGlobal_SierraLogon_Sound->Stop();
    if(LobbyFn35B940()>0) {
        LobbyFn35B960(1);
        LobbyFn359120(100059,true);
    }
    LobbyFn361D90();
}
// FUNCTION: WON 0x00762880
void LobbyFn362880()
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mSuspended=false;
    if(LobbyFn35E350()) ::LobbyGlobal_LobbyMusic_Sound->Resume();
    LobbyFn361DB0();
}
}

#include "ee/wonapi/WONLobbyAccountLogic.h"

namespace WONAPI {
// Genuine missing SDK namespace entry points; retained canonical ABI spellings.
// FUNCTION: WON 0x00762740
void LobbyFn362740()
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mSuspended=false;
    gLobbyData->mInitLogic->Init();
}
}
