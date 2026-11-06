
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <string>
#include <list>
#include <map>
#include <vector>
#include <windows.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
// Original3bdaa0,38B. All three owned pointers default construct to null.
InitFailedCtrl::InitFailedCtrl() {}
// Original3bdaf0,184B. Reverse member destruction, then Container.
// Implicit destructor support is generated from the actual class/vtable.
// Original3bdbb0,120B; actual source assertion line26/27/28.
// FUNCTION: WON 0x007bdbb0
void InitFailedCtrl::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* config = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config, mRetryButton, "RetryButton", __FILE__, 26);
        SafeGetComponent(config, mIgnoreButton, "IgnoreButton", __FILE__, 27);
        SafeGetComponent(config, mDescLabel, "DescLabel", __FILE__, 28);
        SetVisible(false);
        Fn2F5360(1);
    }
}
// Original3bdc30,113B; methods2f6120/2f6290 requested to canonical GUI owner.
// FUNCTION: WON 0x007bdc30
void InitFailedCtrl::HandleLoginInitStatus(LoginInitStatusEvent* inEvent)
{
    if (inEvent->mKind == 1)
        SetVisible(false);
    else if (inEvent->mKind == 3) {
        mDescLabel->SetText(inEvent->mDescription, true);
        mIgnoreButton->SetVisible(inEvent->mAllowIgnore);
        Fn2F6120(1, inEvent->mAllowIgnore != false);
        SetVisible(true);
        Fn2F6290();
    }
}
// Original3bdcb0,94B. Both button branches notify and return; unhandled fallback.
// FUNCTION: WON 0x007bdcb0
void InitFailedCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 100036) {
        HandleLoginInitStatus(static_cast<LoginInitStatusEvent*>(inEvent));
        return;
    }
    if (inEvent->mType == 15) {
        Component* component = inEvent->mComponent;
        if (component == mRetryButton.get()) {
            Fn2F5380(100045, 0);
            return;
        }
        if (component == mIgnoreButton.get()) {
            Fn2F5380(100025, 0);
            return;
        }
    }
    Component::HandleComponentEvent(inEvent);
}
}

/* Rejected empty explicit destructor candidates; zero handwritten credit.
InitFailedCtrl::~InitFailedCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bdad0
// (public: virtual void * __thiscall WONAPI::InitFailedCtrl::`scalar deleting destructor'(unsigned int)    [??_GInitFailedCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bdaf0
// (public: virtual __thiscall WONAPI::InitFailedCtrl::~InitFailedCtrl(void)    [??1InitFailedCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bdaa0
// (public: __thiscall WONAPI::InitFailedCtrl::InitFailedCtrl(void)    [??0InitFailedCtrl@WONAPI@@QAE@XZ])
