// Original-backed LobbyLogic family; source filename has no embedded target anchor.

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyLogic.h"

namespace WONAPI {
// Original namespace runner; exact return is void in both callers.
void LobbyFn35B5B0(AsyncOp* inOp, unsigned long inTimeout);

// Original initializes complete tracker lock/maps/flags with true,false.
// Canonical Maps constructor remains an original callee; its out-of-line shape
// is retained honestly until ordinary COFF calibration proves any refinement.
// FUNCTION: WON 0x007e5930
LobbyLogic::LobbyLogic() { mKilled = false; }

// FUNCTION: WON 0x007e5a40
LobbyLogic::~LobbyLogic() {}


void LobbyLogic::KillHook() {}
void LobbyLogic::InitComponent(ComponentConfig*) {}
void LobbyLogic::Reset() {}
void LobbyLogic::HandleControlEvent(int) {}
void LobbyLogic::HandleComponentEvent(ComponentEvent*) {}

// FUNCTION: WON 0x007e5b00
void LobbyLogic::Kill()
{
    mKilled = true;
    mTracker.KillAll();
    KillHook();
}

// FUNCTION: WON 0x007e5b20
void LobbyLogic::RunTrackedOp(AsyncOp* inOp, LogicCompletionBase* inCompletion,
                             unsigned long inTimeout)
{
    inOp->mCompletion = inCompletion;
    if (inCompletion != NULL)
        inCompletion->mCheckTracker = true;
    mTracker.AddOp(inOp, 0);
    LobbyFn35B5B0(inOp, inTimeout);
}

// FUNCTION: WON 0x007e5ba0
void LobbyLogic::RunOp(AsyncOp* inOp, CompletionBase<AsyncOpPtr>* inCompletion,
                      unsigned long inTimeout)
{
    inOp->mCompletion = inCompletion;
    LobbyFn35B5B0(inOp, inTimeout);
}

// FUNCTION: WON 0x007e5c00
bool LobbyLogic::Fn3E5C00(AsyncOp* inOp)
{
    return mTracker.RemoveOp(inOp);
}

// NetLogic's original five-byte destructor3dfe00 has no own-vptr store and is
// likely an implicit generated destructor. Leave it as original-carved support
// until ordinary full LanLogic emission establishes the natural source shape.
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e5a20
// (public: virtual void * __thiscall WONAPI::LobbyLogic::`scalar deleting destructor'(unsigned int)    [??_GLobbyLogic@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0076c4f0
// (protected: void __thiscall std::_Tree<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::AsyncOp> con)
// TEMPLATE: WON 0x007672b0
// (public: __thiscall std::_Tree<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::AsyncOp> const ,clas)
