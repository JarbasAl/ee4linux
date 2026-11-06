// Native-backed lobby event dispatch; singleton remains original-owned.
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
// Actual original cdecl helper3590e0 receives the retained event pointer.
void LobbyFn3590E0(ComponentEvent*);
// FUNCTION: WON 0x00759120
void LobbyFn359120(int type,bool broadcast)
{
    if(gLobbyData.get()==NULL) return;
    SmartPtr<LobbyEvent> event = new LobbyEvent(type);
    gLobbyData->mLobbyContainer->Fn2F52A0(event.get(),broadcast);
}
// FUNCTION: WON 0x00759260
void LobbyFn359260(ComponentEvent* event,Component* target)
{
    SmartPtr<ComponentEvent> eventOwner=event;
    if(gLobbyData.get()!=NULL && target!=NULL) {
        if(event->mComponent==NULL)
            event->SetComponent(gLobbyData->mLobbyContainer.get());
        event->mBroadcast=true;
        target->HandleComponentEvent(event);
    }
}
// FUNCTION: WON 0x00759300
void LobbyFn359300(ComponentEvent* event)
{
    SmartPtr<ComponentEvent> eventOwner=event;
    if(gLobbyData.get()!=NULL) {
        if(event->mComponent==NULL)
            event->SetComponent(gLobbyData->mLobbyContainer.get());
        LobbyFn3590E0(event);
    }
}
// FUNCTION: WON 0x00761020
void LobbyFn361020()
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mLobbyContainer->ShowLoginScreen();
    LobbyEvent* event=new LobbyEvent(100014);
    gLobbyData->mLobbyContainer->mLoginScreen->Fn2F52D0(event);
}
}

namespace WONAPI {
// Genuine missing SDK namespace entry points; retained canonical ABI spellings.

void LobbyFn359100(ComponentEvent* event,bool sendNow)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mLobbyContainer->Fn2F52A0(event,sendNow);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00759100
// (void __cdecl WONAPI::LobbyFn359100(class WONAPI::ComponentEvent *,bool)    [?LobbyFn359100@WONAPI@@YAXPAVComponentEvent@1@_N@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007591e0
// (public: virtual void * __thiscall WONAPI::LobbyEvent::`scalar deleting destructor'(unsigned int)    [??_GLobbyEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00759200
// (public: virtual __thiscall WONAPI::LobbyEvent::~LobbyEvent(void)    [??1LobbyEvent@WONAPI@@UAE@XZ])
