// Reviewed original-backed controls, source-only; generated support has zeroquota.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Scratch complete control methods; zero primary/admission/runtime credit.
// Actual ServerListCtrl reference-only contract requires root header gate.
#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyControls.h"
// Scratch: actualVT452f18, Container-derived90,51slots; no admission authority.
namespace WONAPI {
// Reference-only contract: actual RTTI ServerListCtrl>Container>Component>RefCount,
// VT453fec exactly51slots. No sizeof/new/ctor/copy/byvalue allowed for this view.
// It overrides only1c/44/7c plus deleting destructor00. Full field ownership
// remains future ServerListCtrl family; direct external getter needs no fake gaps.
// Use the sole canonical ServerListCtrl from WONLobbyRoomServer.h.
// DirectConnectCtrl declaration hoisted to WONLobbyControlConfig.h; existing bodies unchanged.
}

namespace WONAPI {
DirectConnectCtrl::DirectConnectCtrl() {} //3bada0,38B,zeroemptyctorquota
// Original3badf0 releasesRemove8c,Add88,Connection84,thenContainer; ownVT3bae0f.
DirectConnectCtrl::~DirectConnectCtrl() {} //explicitemptycandidatezeroquota
// Original3baeb0..3baf1c,108B. Resource assertions lines34..36.
// FUNCTION: WON 0x007baeb0
void DirectConnectCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mConnectionList,"ConnectionList",__FILE__,34);
        SafeGetComponent(config,mAddButton,"AddButton",__FILE__,35);
        SafeGetComponent(config,mRemoveButton,"RemoveButton",__FILE__,36);
        Fn2F5360(1);
    }
}
// Original3baf20..3baffe,222B. No callback between borrowed selectedgame and retain.
// FUNCTION: WON 0x007baf20
void DirectConnectCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 15:
        if(inEvent->mComponent==mAddButton.get()) {
            Fn2F53F0(new ChangeDirectConnectEvent(0,NULL),false);
            return;
        }
        if(inEvent->mComponent==mRemoveButton.get()) {
            LobbyGame* game=mConnectionList->GetSelGame();
            if(game!=NULL)
                Fn2F53F0(new ChangeDirectConnectEvent(1,game),false);
            return;
        }
        break;
    }
    Component::HandleComponentEvent(inEvent);
}
}
// Specific blockers: canonical source gate for reference-only ServerListCtrl and
// Component::Fn2F53F0(ComponentEvent*,bool) call-only method; normal fullTU COFF,
// both new/delete state guards0/1 and inherited table/static payload review.
// No full ServerList allocation is reconstructed. Empty destructor has zeroquota.
// TEMPLATE: WON 0x007cdba0

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007badd0
// (public: virtual void * __thiscall WONAPI::DirectConnectCtrl::`scalar deleting destructor'(unsigned int)    [??_GDirectConnectCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007badf0
// (public: virtual __thiscall WONAPI::DirectConnectCtrl::~DirectConnectCtrl(void)    [??1DirectConnectCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007bada0
// (public: __thiscall WONAPI::DirectConnectCtrl::DirectConnectCtrl(void)    [??0DirectConnectCtrl@WONAPI@@QAE@XZ])
