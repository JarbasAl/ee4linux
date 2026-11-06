// Reviewed original-backed controls; source-only, near code/support remain carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SCRATCH ONLY; canonical events/email-validator GUI D gate and independent flags pending.
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
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
//32B empty source-kind candidate; zero primary credit pending normal COFF form comparison.
LostUserNameCtrl::LostUserNameCtrl() {}
//147B empty source-kind candidate; zero primary credit pending normal COFF form comparison.
// Implicit destructor support is generated from the actual class/vtable.
//3c0830..3c0881,81B. Unlike LostPasswordCtrl this original has no Fn2F5360 call.
// FUNCTION: WON 0x007c0830
void LostUserNameCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mEmailInput,"EmailInput",__FILE__,27);
        SafeGetComponent(config,mRetrieveButton,"RetrieveButton",__FILE__,28);
        UpdateRetrieveButton();
    }
}
//3c0890..3c0930,160B. Actual std::string temporary spans validator and SetEnabled.
// FUNCTION: WON 0x007c0890
void LostUserNameCtrl::UpdateRetrieveButton()
{
    const std::string& email=mEmailInput->GetText(); //single lifetime-extended conversion temporary
    Button* button=mRetrieveButton.get();
    button->SetEnabled(LobbyFn3BA6A0(email));
}
//3c0930..3c0a83,339B. Five event-construction cleanup states; null new skips GetText.
// FUNCTION: WON 0x007c0930
void LostUserNameCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==21)
        UpdateRetrieveButton();
    else if(inEvent->mType==15 && inEvent->mComponent==mRetrieveButton.get())
        Fn2F53F0(new GetLostUserNameEvent(mEmailInput->GetText()),false);
    else
        Component::HandleComponentEvent(inEvent);
}
}
/* Generated zero-primary support only: GetLostUserNameEvent implicit normal dtor3c0ab0,
143B, no own453b14 vptr store; deleting3c0a90. Canonical event declaration belongs core.
*/

/* Rejected empty explicit destructor candidates; zero handwritten credit.
LostUserNameCtrl::~LostUserNameCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c0ab0
// (public: virtual __thiscall WONAPI::GetLostUserNameEvent::~GetLostUserNameEvent(void)    [??1GetLostUserNameEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c0770
// (public: virtual void * __thiscall WONAPI::LostUserNameCtrl::`scalar deleting destructor'(unsigned int)    [??_GLostUserNameCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c0790
// (public: virtual __thiscall WONAPI::LostUserNameCtrl::~LostUserNameCtrl(void)    [??1LostUserNameCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c0a90
// (public: virtual void * __thiscall WONAPI::GetLostUserNameEvent::`scalar deleting destructor'(unsigned int)    [??_GGetLostUserNameEvent@WONAPI@@UAEPAX)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c0750
// (public: __thiscall WONAPI::LostUserNameCtrl::LostUserNameCtrl(void)    [??0LostUserNameCtrl@WONAPI@@QAE@XZ])
