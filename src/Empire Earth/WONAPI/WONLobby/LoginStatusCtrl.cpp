// Login status display; constructor/destructor support excluded from primary quota.
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
// Original3c0200,28B. Real Container ctor, null SmartPtr<Label>.
LoginStatusCtrl::LoginStatusCtrl() {}
// Original3c0240,106B. Release label, unwind guard retains Container cleanup.
// Implicit destructor support is generated from the actual class/vtable.
// Original3c02b0,56B. No base Init call in original.
// FUNCTION: WON 0x007c02b0
void LoginStatusCtrl::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* config = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config, mStatusLabel, "StatusLabel", __FILE__, 26);
        Fn2F5360(1);
    }
}
// Original3c02f0,42B. Checked original derived event; base fallback exact receiver.
// FUNCTION: WON 0x007c02f0
void LoginStatusCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 100036)
        mStatusLabel->SetText(static_cast<LoginInitStatusEvent*>(inEvent)->mStatus, true);
    else
        Component::HandleComponentEvent(inEvent);
}
}

/* Rejected empty explicit destructor candidates; zero handwritten credit.
LoginStatusCtrl::~LoginStatusCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

/* R1 rejected draft provenance; zero credit. The shared base/event blockers below
were closed by root C/D, but explicit empty derived destructor remains rejected
pending source-kind COFF evidence. Earlier comment-only text is retained verbatim.
// DEFERRED ORIGINAL UNIT: WONAPI/WONLobby/LoginStatusCtrl.cpp.
// Comment-only preservation; no executable definitions, markers, registration,
// compilation, byte-match claim, or written/exact coverage credit.
// Original __FILE__: C:\EEX\Empire Earth\WONAPI\WONLobby\LoginStatusCtrl.cpp
// (literal RVA 0x4dfff0). Namespace WONAPI and class names have RTTI evidence;
// method/member descriptive spellings and unobserved original declarations
// remain provisional. The exact surrounding source/header arrangement is unknown.
//
// ADMISSION BLOCKERS
// Canonical Container currently omits its inherited virtual overrides. The real
// Container-derived LoginStatusCtrl vtable has 51 slots (0x00..0xc8), with only
// deleting destructor 0x00, InitComponent 0x1c, and event handler 0x7c changed.
// All other targets must equal original Container vtable RVA 0x44d7cc; guessed
// virtual signatures or a shadow GUI base are not an acceptable implementation.
// Canonical ComponentEvent currently includes provisional long mParam at +0x20
// and calls original 0x32b530 the LobbyEvent constructor. Original RTTI instead
// identifies that constructor/vtable 0x451174 as ComponentEvent; its actual
// derived payload starts at +0x20. Actual LobbyEvent vtable is 0x451ae4 and adds
// no payload. The existing canonical event classes cannot implement this draft
// correctly until all affected game adapters and event producers are reconciled.
// Do not use this commented contract as an executable replacement or shadow.
//
// ORIGINAL CONTRACT REQUEST (all source below remains comments)
// namespace WONAPI {
// // Actual single inheritance: LoginStatusCtrl > Container > Component > RefCount.
// // RTTI vtable RVA 0x453898; complete allocation 0x88 in factory RVA 0x363720.
// class LoginStatusCtrl : public Container {
// public:
//     LoginStatusCtrl();
//     virtual ~LoginStatusCtrl();
//     virtual void InitComponent(ComponentInit& inInit);          // slot 0x1c
//     virtual void HandleComponentEvent(ComponentEvent* inEvent); // slot 0x7c
//     SmartPtr<Label> mStatusLabel;                              // +0x84
// };
//
// // Actual event chain: LoginInitStatusEvent > LobbyEvent > ComponentEvent >
// // WindowEventBase > RefCount. Both ComponentEvent and LobbyEvent end at +0x20.
// // ComponentEvent fields already proven: owning pointer +0x10, type +0x14,
// // flag/state storage +0x18, Component* +0x1c. Complete declaration/signatures
// // of that shared base require canonical repair; no guessed gap is supplied.
// // LoginInitStatusEvent allocated 0x30: producer RVA 0x3dd3b0 calls ComponentEvent
// // ctor 0x32b530, installs LobbyEvent vtable 0x451ae4, constructs strings +20/+24,
// // stores bool +28 and 32-bit kind +2c, then installs final vtable 0x4558f8.
// class LoginInitStatusEvent : public LobbyEvent {
// public:
//     GUIString mStatus;       // +0x20; descriptive spelling provisional
//     GUIString mDescription;  // +0x24; descriptive spelling provisional
//     bool mAllowIgnore;       // +0x28; descriptive spelling provisional
//     int mKind;               // +0x2c; descriptive spelling provisional
// };
//
// DEFERRED ORIGINAL VA 0x007c0200, 28 bytes: complete constructor draft.
// LoginStatusCtrl::LoginStatusCtrl()
// {
// }
// // Implicit Container construction calls RVA 0x2f5b70, default SmartPtr<Label>
// // zeroes +0x84, then the compiler installs LoginStatusCtrl vtable 0x453898.
//
// DEFERRED ORIGINAL VA 0x007c0220, 30 bytes: compiler-generated deleting destructor.
// // This is synthesized from the real virtual destructor below, not a source
// // helper to hand-implement. Complete original generated behavior:
// //   preserve the original object pointer;
// //   call LoginStatusCtrl::~LoginStatusCtrl() at RVA 0x3c0240;
// //   if ((deletionFlags & 1) != 0), call operator delete(originalObject);
// //   return originalObject and pop the one 32-bit deletionFlags argument.
// // Both flag branches and the original receiver are retained by this account.
//
// DEFERRED ORIGINAL VA 0x007c0240, 106 bytes: complete ordinary destructor draft.
// LoginStatusCtrl::~LoginStatusCtrl()
// {
// }
// // Member destruction releases mStatusLabel when nonnull: decrement pointee+4
// // atomically, delete through pointee slot 0 with flag 1 when result <= 0.
// // Then destroy Container via RVA 0x2f5e80. Original /GX unwind state 0 ensures
// // Container cleanup if member destruction unwinds; no cleanup is omitted.
//
// DEFERRED ORIGINAL VA 0x007c02b0, 56 bytes: inventory-missed complete Init body.
// void LoginStatusCtrl::InitComponent(ComponentInit& inInit)
// {
//     if (inInit.mType == 2) {
//         ComponentConfig* config = static_cast<ComponentConfigInit&>(inInit).mConfig;
//         SafeGetComponent(config, mStatusLabel, "StatusLabel", __FILE__, 26);
//         Fn2F5360(1);
//     }
// }
// // Existing Label specialization RVA 0x294be5 receives exactly five arguments;
// // config comes from ComponentConfigInit +4. No base InitComponent call occurs.
// // Preserve logical __FILE__/line through staging, never source path remapping.
//
// DEFERRED ORIGINAL VA 0x007c02f0, 42 bytes: inventory-missed complete event body.
// void LoginStatusCtrl::HandleComponentEvent(ComponentEvent* inEvent)
// {
//     if (inEvent->mType == 100036) {
//         LoginInitStatusEvent* status = static_cast<LoginInitStatusEvent*>(inEvent);
//         mStatusLabel->SetText(status->mStatus, true);
//     } else
//         Component::HandleComponentEvent(inEvent);
// }
// // The checked event type identifies the original derived payload at +0x20.
// // SetText receiver is the label at this+0x84; fallback receiver is this itself
// // through Component::HandleComponentEvent RVA 0x2f5550, not a GUI substitute.
// } // namespace WONAPI
//
// Original body total: 28 + 30 + 106 + 56 + 42 = 262 bytes, zero admitted credit.
// Supporting original-only evidence: build/wonlobby_r1/controls/phase0_packet.txt,
// loginstatus_identity.txt, event_identity.txt, and commented rejected scratch.
// Dialogs lane independently reviewed all five extents, Init arguments/branches,
// event sender layout and fallback receiver. No normal COFF exists for this draft.

*/

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c0220
// (public: virtual void * __thiscall WONAPI::LoginStatusCtrl::`scalar deleting destructor'(unsigned int)    [??_GLoginStatusCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c0240
// (public: virtual __thiscall WONAPI::LoginStatusCtrl::~LoginStatusCtrl(void)    [??1LoginStatusCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c0200
// (public: __thiscall WONAPI::LoginStatusCtrl::LoginStatusCtrl(void)    [??0LoginStatusCtrl@WONAPI@@QAE@XZ])
