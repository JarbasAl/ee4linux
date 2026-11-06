// Actual tracker-owning operation and nested completion; source only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// Actual RTTI .?AVTrackerCompletion@AsyncOpWithTracker@WONAPI@@, complete16, two slots.
class AsyncOpWithTracker::TrackerCompletion : public CompletionBase<AsyncOpPtr> {
    SmartPtr<AsyncOpWithTracker> mOp; //08 owningparent
    int mParam; //0c
public:
    TrackerCompletion(AsyncOpWithTracker* op,int param):mOp(op),mParam(param) {}
    // Native2a01a0/101 forwarding callback with by-value owning operation, support0.
    virtual void Complete(AsyncOpPtr op) {mOp->Callback(op.get(),mParam);}
};
// FUNCTION: WON 0x006a00d0
void AsyncOpWithTracker::Callback(AsyncOp* op,int param)
{
    if(GetStatus()==100000 && mTracker.RemoveOp(op)) CallbackHook(op,param);
}
// FUNCTION: WON 0x006a0100
AsyncOp* AsyncOpWithTracker::Track(AsyncOp* op,int param)
{
    op->mCompletion=new TrackerCompletion(this,param);
    return mTracker.AddOp(op,param);
}
// Native2a0290/8 cleanup forwarding, support0.
// FUNCTION: WON 0x006a0290
void AsyncOpWithTracker::Virtual08() {mTracker.KillAll();}
}
namespace WONAPI {

bool AsyncOpWithTracker::CallbackHook(AsyncOp*,int) { return false; }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a0230
// (public: virtual __thiscall WONAPI::AsyncOpWithTracker::TrackerCompletion::~TrackerCompletion(void)    [??1TrackerCompletion@AsyncOpWithTracker@WONAPI@)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a0210
// (public: virtual void * __thiscall WONAPI::AsyncOpWithTracker::TrackerCompletion::`scalar deleting destructor'(unsigned int)    [??_GTrackerCompletion@)
