// Native async ownership and queue family, source-only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {

// FUNCTION: WON 0x0069f730
AsyncOpContainer::AsyncOpContainer() { mPurging=false; }
// FUNCTION: WON 0x0069f830
AsyncOpContainer::~AsyncOpContainer() { PurgeOps(); }
// FUNCTION: WON 0x0069f900
void AsyncOpContainer::PurgeOps()
{
    mPurging=true;
    while(!mOpQueue.empty()) {
        mCurPumpOp=mOpQueue.front();
        mOpQueue.pop_front();
        if(mCurPumpOp->mStatus==100000) mCurPumpOp->Kill(true);
        mCurPumpOp->Complete();
    }
    mPurging=false;
}
// FUNCTION: WON 0x0069f9b0
void AsyncOpContainer::QueueOp(AsyncOp* op)
{
    AutoCrit lock(mDataCrit);
    if(op==mCurPumpOp.get() && (op->mStatus!=100000 || mPurging)) return;
    mOpQueue.push_back(op);
    if(mOpQueue.size()==1) SetEvent(mSignal.mEvent);
}
// FUNCTION: WON 0x0069fad0
void AsyncOpContainer::Pump(unsigned long waitTime,int maxOps)
{
    AutoCrit lock(mDataCrit);
    unsigned long start=GetTickCount();
    int remaining=maxOps;
    if(remaining<0) remaining=mOpQueue.size();
    if(mOpQueue.empty() && waitTime>0) {
        lock.Leave();
        mSignal.WaitFor(waitTime);
        lock.Enter();
    }
    while(!mOpQueue.empty()) {
        mCurPumpOp=mOpQueue.front();
        mOpQueue.pop_front();
        lock.Leave();
        if(mCurPumpOp->mStatus==100000) mCurPumpOp->StartRunAsync();
        if(mCurPumpOp->mCompletionPending) mCurPumpOp->Complete();
        mCurPumpOp=NULL;
        lock.Enter();
        if(remaining>0 && --remaining==0) break;
        if(waitTime>0 && GetTickCount()-start>=waitTime) break;
    }
    mCurPumpOp=NULL;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0069f7c0
// (public: virtual __thiscall WONAPI::Event::~Event(void)    [??1Event@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0069f7e0
// (public: virtual void * __thiscall WONAPI::Event::`scalar deleting destructor'(unsigned int)    [??_GEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0069f810
// (public: virtual void * __thiscall WONAPI::AsyncOpContainer::`scalar deleting destructor'(unsigned int)    [??_GAsyncOpContainer@WONAPI@@UAEPAXI@Z])
