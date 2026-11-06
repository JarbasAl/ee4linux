// Complete native ThreadBase14/Thread1c core and Win32 execution hooks.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <process.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006a6c10
ThreadBase::ThreadBase()
{
    mStopped=true;
    mRunning=false;
    mAutoDelete=false;
    mPriority=ThreadPriority_Normal;
}
// Native2a6c50 setter/default hook, support0.
// FUNCTION: WON 0x006a6c50
void ThreadBase::StartHook() {mRunning=false;}
void ThreadBase::WaitForStopHook() {}
bool ThreadBase::InThisThreadHook() {return false;}
void ThreadBase::SetPriorityHook() {}
// Native2a6c80 default body and natural Event cleanup, support0.
ThreadBase::~ThreadBase() {}
// Native2a6ca0 lifecycle forwarding, support0.
// FUNCTION: WON 0x006a6ca0
void ThreadBase::Start()
{
    if(!mRunning) {mStopped=false;mRunning=true;StartHook();}
}
// FUNCTION: WON 0x006a6cc0
void ThreadBase::Stop(bool waitForStop)
{
    if(!mStopped) {
        mStopped=true;
        Signal();
        if(waitForStop) WaitForStop();
    }
}
// Native2a6cf0 signal forwarding, support0.
// FUNCTION: WON 0x006a6cf0
void ThreadBase::Signal() {mSignalEvent.Set();}
// FUNCTION: WON 0x006a6d00
bool ThreadBase::InThisThread()
{
    if(!mRunning) return false;
    return InThisThreadHook();
}
// FUNCTION: WON 0x006a6d10
void ThreadBase::WaitForStop()
{
    if(mRunning && !InThisThread()) WaitForStopHook();
}
// FUNCTION: WON 0x006a85e0
unsigned __stdcall Thread::StaticThreadFunc(void* data)
{
    __try {
        Thread* thread=(Thread*)data;
        thread->mThreadId=GetCurrentThreadId();
        thread->ThreadFunc();
        thread->mRunning=false;
        if(thread->mAutoDelete) delete thread;
    } __except(WONDebug::DumpStackToFile(GetExceptionInformation())) {
    }
    return 0;
}
// FUNCTION: WON 0x006a8670
void Thread::StartHook()
{
    unsigned threadAddress;
    int flags=mPriority==ThreadPriority_Normal ? 0 : CREATE_SUSPENDED;
    mThreadHandle=(HANDLE)_beginthreadex(NULL,0,StaticThreadFunc,this,flags,&threadAddress);
    if(flags) {
        SetPriorityHook();
        ResumeThread(mThreadHandle);
    }
}
// Native2a86c0 forwarding, support0; native does not close thread handle here.
// FUNCTION: WON 0x006a86c0
void Thread::WaitForStopHook() {WaitForSingleObject(mThreadHandle,INFINITE);}
// Native2a86d0 thread identity query, support0.
// FUNCTION: WON 0x006a86d0
bool Thread::InThisThreadHook() {return GetCurrentThreadId()==mThreadId;}
// FUNCTION: WON 0x006a86f0
void Thread::SetPriorityHook()
{
    int priority=THREAD_PRIORITY_NORMAL;
    switch(mPriority) {
    case ThreadPriority_Highest:priority=THREAD_PRIORITY_HIGHEST;break;
    case ThreadPriority_High:priority=THREAD_PRIORITY_ABOVE_NORMAL;break;
    case ThreadPriority_Normal:priority=THREAD_PRIORITY_NORMAL;break;
    case ThreadPriority_Low:priority=THREAD_PRIORITY_BELOW_NORMAL;break;
    case ThreadPriority_Lowest:priority=THREAD_PRIORITY_LOWEST;break;
    }
    SetThreadPriority(mThreadHandle,priority);
}
}

// Genuine same-library SDK implementation. Native entry remains unidentified.
namespace WONAPI {
void ThreadBase::SetPriority(ThreadPriority thePriority)
{
    if(thePriority == mPriority)
        return;

    mPriority = thePriority;
    if(mRunning)
        SetPriorityHook();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a6c60
// (public: virtual void * __thiscall WONAPI::ThreadBase::`scalar deleting destructor'(unsigned int)    [??_GThreadBase@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a6c80
// (public: virtual __thiscall WONAPI::ThreadBase::~ThreadBase(void)    [??1ThreadBase@WONAPI@@UAE@XZ])
