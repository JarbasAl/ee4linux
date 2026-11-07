// Native TimerThread timeout map owner and wraparound behavior.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONTimerThread.h"
namespace WONAPI {
// FUNCTION: WON 0x006a02a0
TimerThread::TimerThread() { mLastTick=0; }
// FUNCTION: WON 0x006a0480
TimerThread::~TimerThread() {PurgeOps();}
// FUNCTION: WON 0x006a0540
void TimerThread::PurgeOps()
{
    AutoCrit lock(mDataCrit);
    AsyncOpTimeoutMap::iterator i=mTimerMap.begin();
    while(i!=mTimerMap.end()) {
        AsyncOp* op=i->second;
        op->RemoveFromTimerMap(false);
        op->Kill(true);
        ++i;
    }
    i=mWrapAroundMap.begin();
    while(i!=mWrapAroundMap.end()) {
        AsyncOp* op=i->second;
        op->RemoveFromTimerMap(false);
        op->Kill(true);
        ++i;
    }
    mTimerMap.clear();
    mWrapAroundMap.clear();
    mLastTick=0;
}
// FUNCTION: WON 0x006a0700
void TimerThread::AddTimerOp(AsyncOp* op,unsigned long timeout)
{
    AutoCrit lock(mDataCrit);
    unsigned long now=GetTickCount();
    unsigned long untilWrap=0xffffffff-now;
    if(timeout>untilWrap) op->AddToTimerMap(mWrapAroundMap,timeout-untilWrap);
    else op->AddToTimerMap(mTimerMap,now+timeout);
    Signal();
}
// FUNCTION: WON 0x006a0790
void TimerThread::RemoveTimerOp(AsyncOp* op)
{
    AutoCrit lock(mDataCrit);
    op->RemoveFromTimerMap(true);
}
// FUNCTION: WON 0x006a0800
void TimerThread::Pump()
{
    AutoCrit lock(mDataCrit);
    unsigned long now=GetTickCount();
    AsyncOpTimeoutMap::iterator i;
    if(now<mLastTick) {
        for(i=mTimerMap.begin();i!=mTimerMap.end();++i) {
            AsyncOp* op=i->second;
            op->RemoveFromTimerMap(false);
            op->Fn29F220(100001);
        }
        mTimerMap=mWrapAroundMap;
        mWrapAroundMap.clear();
    }
    mLastTick=now;
    i=mTimerMap.begin();
    while(i!=mTimerMap.end() && i->first<=mLastTick) {
        AsyncOp* op=i->second;
        op->RemoveFromTimerMap(false);
        op->Fn29F220(100001);
        mTimerMap.erase(i++);
    }
}
// FUNCTION: WON 0x006a0970
unsigned long TimerThread::GetWaitTime()
{
    AutoCrit lock(mDataCrit);
    unsigned long waitTime=9-mLastTick;
    if(!mTimerMap.empty()) {
        unsigned long deadline=mTimerMap.begin()->first;
        waitTime=deadline<=mLastTick ? 0 : deadline-mLastTick;
    }
    return waitTime;
}
// FUNCTION: WON 0x006a09c0
void TimerThread::ThreadFunc()
{
    mLastTick=GetTickCount();
    for(;;) {
        mSignalEvent.WaitFor(GetWaitTime());
        if(mStopped) break;
        Pump();
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a0380
// (public: virtual void * __thiscall WONAPI::TimerThread::`scalar deleting destructor'(unsigned int)    [??_GTimerThread@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a03a0
// (public: __thiscall std::multimap<unsigned long,class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::less<unsigned long>,class std::allocator<stru)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006dccc0
// (public: void * __thiscall std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::AsyncOp> >::`scalar deleting destructor'(unsigned int))
// TEMPLATE: WON 0x006a8900
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::AsyncOp> >,struct std::_Mmapk<unsign)
// TEMPLATE: WON 0x006a8f90
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::AsyncOp> >,struct std::_Mmapk<unsign)
// TEMPLATE: WON 0x006a93c0
// (protected: struct std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::AsyncOp> >,struct std::_Mmapk<un)
// TEMPLATE: WON 0x006a9450
// (protected: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::AsyncOp> >,struct std::)
// TEMPLATE: WON 0x006a8780
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::AsyncOp> >,struct std::_Mmapk<unsign)
// TEMPLATE: WON 0x006a0460
// (public: virtual void * __thiscall WONAPI::Thread::`scalar deleting destructor'(unsigned int))
// TEMPLATE: WON 0x006f28c0
// (public: virtual __thiscall WONAPI::Thread::~Thread(void))
