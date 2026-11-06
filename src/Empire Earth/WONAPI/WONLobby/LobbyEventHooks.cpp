// Native lobby event registration and bounded teardown.
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
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
void LobbyFn3590E0(ComponentEvent*);

// FUNCTION: WON 0x00759020
void LobbyTimerCallback(int)
{
    if(gLobbyData.get()!=NULL)
        gLobbyData->mLogicMgr.DispatchTimerEvent();
}

// FUNCTION: WON 0x00758f70
void LobbyFn358F70()
{
    if(gLobbyData.get()==NULL)
        return;
    LobbyFn359080(gLobbyData->mLobbyContainer.get());
    gLobbyData->mTimerEvent=new UserEvent(LobbyTimerCallback,0);
    WONAPI::WindowManager::GetDefaultWindowManager()->AddTimerEvent(gLobbyData->mTimerEvent.get());
}

// FUNCTION: WON 0x00759040
void LobbyFn359040()
{
    if(gLobbyData.get()==NULL)
        return;
    gLobbyData->mLobbyContainer->AddListener(NULL);
    WindowManager* manager=WONAPI::WindowManager::GetDefaultWindowManager();
    if(manager!=NULL) {
        manager->RemoveTimerEvent(gLobbyData->mTimerEvent.get());
        manager->ClearSubscriptions();
    }
}

// FUNCTION: WON 0x00759080
void LobbyFn359080(Component* component)
{
    if(component!=NULL)
        component->AddListener(new ComponentListenerFunc(LobbyFn3590E0));
}

// FUNCTION: WON 0x007590e0
void LobbyFn3590E0(ComponentEvent* event)
{
    if(gLobbyData.get()!=NULL)
        gLobbyData->mLogicMgr.HandleComponentEvent(event);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007590d0
// (public: virtual void __thiscall WONAPI::ComponentListenerFunc::Virtual04(class WONAPI::ComponentEvent *)    [?Virtual04@ComponentListenerFunc@WONAPI@@)
