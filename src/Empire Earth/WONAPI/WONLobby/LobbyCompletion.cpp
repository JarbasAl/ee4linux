// PROVISIONAL original family357d70..357f24. Filename/object ownership remains unproved.
// All unmatched code/EH/data remains original-carved until full normal-COFF closure is reviewed.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
class LobbyCompletionEvent : public WindowEventBase {
public:
    LobbyCompletionEvent(LobbyCompletionBase* inCompletion, AsyncOp* inOp)
        : mCompletion(inCompletion), mOp(inOp) {}
    virtual void Deliver();
    SmartPtr<LobbyCompletionBase> mCompletion; //10
    AsyncOpPtr mOp; //14
};
// FUNCTION: WON 0x00757d70
void LobbyCompletionBase::Complete(AsyncOpPtr theParam)
{
    DWORD aThreadId = WONAPI::WindowManager::GetThreadId();
    if (GetCurrentThreadId() == aThreadId)
        Run(theParam.get());
    else {
        WindowManager* aManager = WONAPI::WindowManager::GetDefaultWindowManager();
        if (aManager != NULL)
            aManager->PostEvent(new LobbyCompletionEvent(this, theParam.get()), true);
    }
}
// FUNCTION: WON 0x00757e70
void LobbyCompletionEvent::Deliver()
{
    mCompletion->Run(mOp.get());
}
// Original ordinary destructor357ea0 is naturally implicit; emitted support is reviewed separately.
// It must release mOp then mCompletion before WindowEventBase/RefCount. No handwritten vptr store.
}

// HISTORICAL ROUND1 COMMENT-ONLY DRAFT BELOW; SUPERSEDED, ZERO CODE/QUOTA CREDIT.
// Active natural source and independently-reviewed normal COFF above are the current implementation.
// DEFERRED ONLY: all lines in this file are comments; no executable definitions or matching credit.
// Filename LobbyCompletion.cpp is PROVISIONAL; no original source-path literal was found.
// Original family 0x357d70..0x357f30 has grade-B adjacency/RTTI/EH ownership evidence only.
// It may be header COMDAT content in the neighboring Chat or Config original unit. Do not
// register a separate TU until original object ownership is resolved. Config factory begins
// 0x357f30 and config static wrapper at 0x357f90: neither belongs in this deferred family.
// Complete family 424 original bytes: Complete247,Deliver15,dtor132,compiler deleting dtor30.
// No production header contracts, names bindings, compilation or verification admitted.
//
// BLOCKERS: WindowEventBase's actual slot04 is pure Deliver; canonical declaration omits it.
// ComponentEvent's actual inherited Deliver is 0x32b610 and must be declared when that base
// contract is corrected. No event field-layout changes are required for this candidate.
// Canonical PostEvent0x32e850 and GetLobbyWindow0x361250 currently name Window, but receiver
// 0x566270 is actually WindowManager: ctor0x32b836 installs RTTI WindowManager vtable0x4511c0,
// then stores this at0x32b8d7. PostEvent uses queue+0x110 and critical-section+8. Correct the
// real canonical receiver and bindings before admitting; never cast Window* to WindowManager*.
// Global0x566274 receives GetCurrentThreadId at0x32b842. Global spellings below are descriptive
// provisional identities, not new admitted bindings. WindowManager remains canonical in
// WONLobbyGUI.h; add its true PostEvent declaration there only after full review.
// Evidence: build/wonlobby_r1/dialogs/completion_phase0.md and completion_identity.txt.
//
// #include <windows.h>
// #include <string>
// #include <list>
// #include <vector>
// #include "lle/LowLevelEngine.h"
// #include "ee/EEWonNet.h"
// #include "ee/wonapi/WONRouting.h"
// #include "ee/wonapi/WONLobbyResource.h"
// #include "ee/wonapi/WONLobbyGUI.h"
//
// namespace WONAPI {
//
// // Deferred canonical declaration request; no local base/WindowManager replacements.
// // WindowEventBase must declare virtual void Deliver()=0 at slot04.
// // WindowManager must declare void PostEvent(WindowEventBase*, bool), actual receiver0x566270.
// extern WindowManager* gDefaultWindowManager; // original RVA0x566270; actual manager pointer.
// extern DWORD gWindowManagerThreadId;        // original RVA0x566274.
//
// // RTTI0x451aa4: LobbyCompletionEvent -> WindowEventBase -> RefCount; PMD0, allocation0x18.
// class LobbyCompletionEvent : public WindowEventBase {
// public:
//     // Original constructor is fully inlined in Complete; raw-pointer parameter spellings
//     // follow the observed retains, but no independently emitted constructor symbol exists.
//     LobbyCompletionEvent(LobbyCompletionBase* inCompletion, AsyncOp* inOp)
//         : mCompletion(inCompletion), mOp(inOp) {}
//     virtual ~LobbyCompletionEvent();
//     virtual void Deliver();
//     SmartPtr<LobbyCompletionBase> mCompletion; // +0x10
//     AsyncOpPtr mOp;                           // +0x14
// };
//
// // DEFERRED ORIGINAL VA0x757d70;247bytes. Owns its by-value AsyncOpPtr parameter.
// // LobbyCompletionBase canonical hierarchy already has slots00,04 Complete,08 Run.
// void LobbyCompletionBase::Complete(AsyncOpPtr theParam)
// {
//     DWORD aThreadId = gWindowManagerThreadId;
//     if (GetCurrentThreadId() == aThreadId)
//         Run(theParam.get());
//     else {
//         WindowManager* aManager = gDefaultWindowManager;
//         if (aManager != NULL)
//             aManager->PostEvent(new LobbyCompletionEvent(this, theParam.get()), true);
//     }
// }
//
// // DEFERRED ORIGINAL VA0x757e70;15bytes. Missing from old inventory; actual event slot04.
// // No null guard: dispatch raw op through completion's Run(slot08), not Complete(slot04).
// void LobbyCompletionEvent::Deliver()
// {
//     mCompletion->Run(mOp.get());
// }
//
// // DEFERRED ORIGINAL VA0x757ea0;132bytes. Reverse member release then WindowEventBase/RefCount.
// // EH handler0x427033 -> FuncInfo0x48ff08 preserves both cleanup states.
// // Compiler scalar deleting destructor at original VA0x757e80 is30bytes, not handwritten.
// // Generated behavior: preserve original this; call the ordinary destructor; if flags&1,
// // operator delete(original this); return original this and pop the 4-byte flags argument.
// LobbyCompletionEvent::~LobbyCompletionEvent()
// {
// }
//
// } // namespace WONAPI

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00757ea0
// (public: virtual __thiscall WONAPI::LobbyCompletionEvent::~LobbyCompletionEvent(void)    [??1LobbyCompletionEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00757e80
// (public: virtual void * __thiscall WONAPI::LobbyCompletionEvent::`scalar deleting destructor'(unsigned int)    [??_GLobbyCompletionEvent@WONAPI@@UAEPAX)
