// Native AsyncOp lifecycle, status, timeout and completion family.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x0069ef80
AsyncOp::AsyncOp()
{
    mStartTime=0;
    mTimeout=0xffffffff;
    mRunMode=1;
    mStatus=100004;
    mCompletionPending=false;
    mB39=false;
    mFlag3A=false;
    mTimeoutMap=NULL;
    mContainer=NULL;
}
// FUNCTION: WON 0x0069f000
AsyncOp::~AsyncOp() {}

void AsyncOp::Virtual04() {}
void AsyncOp::Virtual08() {}
// FUNCTION: WON 0x0069f080
void AsyncOp::AddToTimerMap(AsyncOpTimeoutMap& map,unsigned long deadline)
{
    RemoveFromTimerMap(true);
    mTimeoutMap=&map;
    mTimeoutItr=map.insert(std::make_pair(deadline,AsyncOpPtr(this)));
}
// FUNCTION: WON 0x0069f160
void AsyncOp::RemoveFromTimerMap(bool erase)
{
    if(mTimeoutMap!=NULL) {
        if(erase) mTimeoutMap->erase(mTimeoutItr);
        mTimeoutMap=NULL;
    }
}

// FUNCTION: WON 0x0069f1b0
void AsyncOp::Fn29F1B0(unsigned long timeout)
{ if(mStatus!=100000) mTimeout=timeout; }
// FUNCTION: WON 0x0069f1d0
void AsyncOp::SetFlag3A(bool flag) {mFlag3A=flag;}
// FUNCTION: WON 0x0069f1e0
void AsyncOp::Kill(bool finish)
{
    mB39=true;
    if(finish) Fn29F220(100002);
}
// FUNCTION: WON 0x0069f200
void AsyncOp::QueueInOpContainer()
{
    if(mContainer!=NULL) mContainer->QueueOp(this);
    else WONAPICore::QueueOp(this);
}
// FUNCTION: WON 0x0069f220
void AsyncOp::Fn29F220(WONStatus status)
{
    AutoCrit lock(mLock);
    if(mStatus!=100000) return;
    mStatus=status;
    mCompletionPending=true;
    lock.Leave();
    switch(mRunMode) {
    case 1: QueueInOpContainer(); break;
    }
}
// FUNCTION: WON 0x0069f2f0
void AsyncOp::ForceFinish(WONStatus status)
{
    if(mStatus==100000) {Fn29F220(status);return;}
    AutoCrit lock(mLock);
    if(mCompletionPending) return;
    mStatus=100000;
    mB39=false;
    lock.Leave();
    Fn29F220(status);
}
// FUNCTION: WON 0x0069f3a0
void AsyncOp::Complete()
{
    AutoCrit lock(mLock);
    if(!mCompletionPending) return;
    mCompletionPending=false;
    if(mTimeoutMap!=NULL) {
        lock.Leave();
        WONAPICore::RemoveTimerOp(this);
    } else lock.Leave();
    Virtual08();
    SmartPtr<CompletionBase<AsyncOpPtr> > completion=mCompletion;
    if(completion.get()!=NULL) completion->Complete(this);
}
// FUNCTION: WON 0x0069f4c0
void AsyncOp::StartRunAsync()
{
    Virtual04();
    if(mStatus!=100000) return;
    if(mTimeout>0) {
        if(mTimeout!=0xffffffff) WONAPICore::AddTimerOp(this,mTimeout);
    } else Fn29F220(100001);
}
// FUNCTION: WON 0x0069f500
WONStatus AsyncOp::Fn29F500(long mode,unsigned long timeout)
{
    AutoCrit lock(mLock);
    if(mStatus==100000) return 100000;
    if(mCompletionPending) return 100005;
    mStatus=100000;
    mB39=false;
    lock.Leave();
    mStartTime=GetTickCount();
    mTimeout=timeout;
    mRunMode=mode;
    if(mode==1) {
        if(mFlag3A) StartRunAsync();
        else QueueInOpContainer();
    } else {
        Virtual04();
        Complete();
    }
    return mStatus;
}
// FUNCTION: WON 0x0069f610
void AsyncOp::RunAsync(unsigned long timeout)
{
    AutoCrit lock(mLock);
    if(mStatus==100000 || mCompletionPending) return;
    mStatus=100000;
    mB39=false;
    lock.Leave();
    mStartTime=GetTickCount();
    mTimeout=timeout;
    mRunMode=1;
    WONAPICore::AddTimerOp(this,timeout);
}
// Native29f6d0/15 wrapper, zeroquota; existing canonical Run is actual SocketOp timeout dispatch API.
// FUNCTION: WON 0x0069f6d0
void AsyncOp::Run(long timeout) { Fn29F500(1,timeout); }
// Native29f6e0/20 bool blocking wrapper, support0. Main identity remains29f500.
// FUNCTION: WON 0x0069f6e0
bool AsyncOp::RunBlock(unsigned long timeout) {return Fn29F500(2,timeout)==0;}
// FUNCTION: WON 0x0069f700
unsigned long AsyncOp::TimeLeft()
{
    if(mRunMode==1) return 0;
    unsigned long elapsed=GetTickCount()-mStartTime;
    if(elapsed>=mTimeout) return 0;
    return mTimeout-elapsed;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing session/profile/operation methods.
namespace WONAPI {
DWORD AsyncOp::RealTimeLeft()
{
	int deltaTime = GetTickCount() - mStartTime;
	if(deltaTime >= mTimeout)
		return 0;
	else
		return mTimeout - deltaTime;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0069efe0
// (public: virtual void * __thiscall WONAPI::AsyncOp::`scalar deleting destructor'(unsigned int)    [??_GAsyncOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0069f070
// (public: __thiscall WONAPI::CriticalSection::~CriticalSection(void)    [??1CriticalSection@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x0069f2c0
// (public: __thiscall WONAPI::AutoCrit::~AutoCrit(void)    [??1AutoCrit@WONAPI@@QAE@XZ])
