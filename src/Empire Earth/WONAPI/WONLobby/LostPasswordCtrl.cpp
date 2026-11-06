// Reviewed original-backed controls; source-only, near code/support remain carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SCRATCH ONLY; canonical events/persistence GUI D gate and independent flags pending.
#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
//32B empty source-kind candidate; zero primary credit pending normal COFF form comparison.
LostPasswordCtrl::LostPasswordCtrl() {}
//147B empty source-kind candidate; zero primary credit pending normal COFF form comparison.
// Implicit destructor support is generated from the actual class/vtable.
//3c0400..3c045a,90B. No base initialization call.
// FUNCTION: WON 0x007c0400
void LostPasswordCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mUserNameCombo,"UserNameCombo",__FILE__,27);
        SafeGetComponent(config,mRetrieveButton,"RetrieveButton",__FILE__,28);
        UpdateRetrieveButton();
        Fn2F5360(1);
    }
}
//3c0460..3c048f,47B.
// FUNCTION: WON 0x007c0460
void LostPasswordCtrl::UpdateRetrieveButton()
{
    mRetrieveButton->SetEnabled(mUserNameCombo->mEdit->GetText().length()>=2);
}
//3c0490..3c0512,130B.
// FUNCTION: WON 0x007c0490
void LostPasswordCtrl::FillUserNames()
{
    mUserNameCombo->Clear(true);
    const LobbyUserInfoMap& users=LobbyFn35CE60();
    for(LobbyUserInfoMap::const_iterator i=users.begin();i!=users.end();++i)
        mUserNameCombo->AddString(i->first,-1);
}
//3c0520..3c069e,382B. Five event-construction cleanup states; null new skips GetText.
// FUNCTION: WON 0x007c0520
void LostPasswordCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==21)
        UpdateRetrieveButton();
    else if(inEvent->mType==15 && inEvent->mComponent==mRetrieveButton.get())
        Fn2F53F0(new GetLostPasswordEvent(mUserNameCombo->mEdit->GetText()),false);
    else if(inEvent->mType==100052)
        FillUserNames();
    else
        Component::HandleComponentEvent(inEvent);
}
}
/* Generated zero-primary support only: GetLostPasswordEvent implicit normal dtor3c06c0,
144B, no own453a38 vptr store; deleting3c06a0. Canonical event declaration belongs core.
*/

/* Rejected empty explicit destructor candidates; zero handwritten credit.
LostPasswordCtrl::~LostPasswordCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c06c0
// (public: virtual __thiscall WONAPI::GetLostPasswordEvent::~GetLostPasswordEvent(void)    [??1GetLostPasswordEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c0340
// (public: virtual void * __thiscall WONAPI::LostPasswordCtrl::`scalar deleting destructor'(unsigned int)    [??_GLostPasswordCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c0360
// (public: virtual __thiscall WONAPI::LostPasswordCtrl::~LostPasswordCtrl(void)    [??1LostPasswordCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c06a0
// (public: virtual void * __thiscall WONAPI::GetLostPasswordEvent::`scalar deleting destructor'(unsigned int)    [??_GGetLostPasswordEvent@WONAPI@@UAEPAX)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c0320
// (public: __thiscall WONAPI::LostPasswordCtrl::LostPasswordCtrl(void)    [??0LostPasswordCtrl@WONAPI@@QAE@XZ])
