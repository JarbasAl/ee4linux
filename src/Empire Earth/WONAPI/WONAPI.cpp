// Original-backed actual WONAPICore lifecycle; full d0 native/SDK owners, no opaque allocation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAPICore.h"
namespace WONAPI {
WONAPICore* WONAPICore::mInstance;
bool WONAPICore::mHaveWrittenVersion;
// FUNCTION: WON 0x006f2750
void WONAPICore::Init()
{
    mVersionString="wonapi121401";
    mInstance=this;
    mIsStarted=false;
    mDoPumpThread=false;
    mDoSocketAndTimerThreads=true;
    mSocketThread=NULL;
    AsyncSocket::StartWinsock();
}
// SUPPORT: EXE 0x006f28d0 (173B member/default forwarding constructor, quota0)
WONAPICore::WONAPICore(const char*) { Init(); }
// FUNCTION: WON 0x006f2980
WONAPICore::~WONAPICore()
{
    Virtual08();
    delete mSocketThread;
    AsyncSocket::StopWinsock();
    mInstance=NULL;
}
// FUNCTION: WON 0x006f2a80
void WONAPICore::WriteVersion()
{
    if(mHaveWrittenVersion) return;
    mHaveWrittenVersion=true;
    std::string path=GetDefaultFileDirectory()+"_wonapiversion.txt";
    FILE* file=fopen(path.c_str(),"r");
    if(file!=NULL) {
        fclose(file);
        file=fopen(path.c_str(),"w");
        if(file!=NULL) {
            fprintf(file,"%s\n",mVersionString.c_str());
            fclose(file);
        }
    }
}
// FUNCTION: WON 0x006f2b50
void WONAPICore::Virtual04()
{
    if(mIsStarted) return;
    mIsStarted=true;
    WriteVersion();
    if(mSocketThread==NULL) mSocketThread=new SocketThreadSimple;
    mSocketThread->SetSingleThreaded(!mDoSocketAndTimerThreads);
    if(mDoSocketAndTimerThreads) {
        if(mSocketThread->NeedThread()) mSocketThread->Start();
        mTimerThread.Start();
    }
    if(mDoPumpThread) mPumpThread.Start();
}
// FUNCTION: WON 0x006f2c80
void WONAPICore::Virtual08()
{
    if(!mIsStarted) return;
    mIsStarted=false;
    mPumpThread.Stop(false);
    mTimerThread.Stop(false);
    mSocketThread->Stop(false);
    mPumpThread.WaitForStop();
    mTimerThread.WaitForStop();
    mSocketThread->WaitForStop();
    mTimerThread.PurgeOps();
    mSocketThread->PurgeOps();
    mOpContainer.PurgeOps();
}
// FUNCTION: WON 0x006f2d00
bool WONAPICore::IsStarted() { return mInstance?mInstance->mIsStarted:false; }
// FUNCTION: WON 0x006f2d20
void WONAPICore::Pump(unsigned long milliseconds,int maxOps)
{
    DWORD start=GetTickCount();
    DWORD elapsed=0;
    while(true) {
        DWORD waitTime=milliseconds-elapsed;
        if(mDoSocketAndTimerThreads) mOpContainer.Pump(waitTime,maxOps);
        else {
            DWORD timerWait=mTimerThread.GetWaitTime();
            if(timerWait<waitTime) waitTime=timerWait;
            mSocketThread->Pump(waitTime);
            mTimerThread.Pump();
            mOpContainer.Pump(0,maxOps);
        }
        elapsed=GetTickCount()-start;
        if(elapsed>=milliseconds) break;
    }
}
// FUNCTION: WON 0x006f2da0
const std::string& WONAPICore::GetDefaultFileDirectory()
{
    static std::string anEmptyStr;
    return mInstance==NULL?anEmptyStr:mInstance->mFileDirectory;
}
// FUNCTION: WON 0x006f2e60
void WONAPICore::QueueOp(AsyncOp* op)
{
    if(IsStarted()) mInstance->mOpContainer.QueueOp(op);
    else if(mInstance!=NULL) {
        if(op->mStatus!=100000) mInstance->mOpContainer.QueueOp(op);
    }
}
// FUNCTION: WON 0x006f2ea0
void WONAPICore::AddTimerOp(AsyncOp* op,unsigned long timeout)
{
    if(IsStarted()) mInstance->mTimerThread.AddTimerOp(op,timeout);
    else op->Kill(true);
}
// FUNCTION: WON 0x006f2ed0
void WONAPICore::RemoveTimerOp(AsyncOp* op)
{
    if(IsStarted()) mInstance->mTimerThread.RemoveTimerOp(op);
}
// FUNCTION: WON 0x006f2ef0
void WONAPICore::AddToSocketThread(SocketOp* op)
{
    if(IsStarted()) mInstance->mSocketThread->AddSocketOp(op);
    else op->Kill(true);
}
// FUNCTION: WON 0x006f2f20
void WONAPICore::RemoveFromSocketThread(SocketOp* op)
{
    if(IsStarted()) mInstance->mSocketThread->RemoveSocketOp(op);
}
// SUPPORT: EXE 0x006f2f40 (23B delegating constructor, quota0)
WONAPICoreEx::WONAPICoreEx() : WONAPICore("") {}
// SUPPORT: EXE 0x006f2f90 (23B conditional bool setter, quota0)
void WONAPICoreEx::Fn2F2F90(bool value)
{
    if(!mIsStarted) mDoPumpThread=value;
}

void WONAPICore::PumpThread::ThreadFunc()
{
    WONAPICore* api=WONAPICore::GetInstance();
    while(true) {
        api->Pump(20);
        if(mStopped) return;
    }
}
}

namespace WONAPI {
// Genuine same-SDK missing configuration/ownership family; SOURCE_ONLY_NO_NATIVE_ID.
void WONAPICoreEx::SetDefaultFileDirectory(const char* theDirectory)
{
	if(!mIsStarted)
	{
		mFileDirectory = theDirectory;
		if(!mFileDirectory.empty())
		{
			char anEndChar = mFileDirectory[mFileDirectory.length()-1];
			if(anEndChar!='\\' && anEndChar!='/')
				mFileDirectory += '/';
		}
	}
}

void WONAPICoreEx::SetSocketThread(SocketThread *theThread)
{
	if(!mIsStarted)
	{
		if(mSocketThread!=NULL)
			delete mSocketThread;

		mSocketThread = theThread;
	}
	else
		delete theThread;
}

void WONAPICoreEx::SetDoSocketAndTimerThreads(bool doSocketAndTimerThreads)
{
	if(!mIsStarted)
		mDoSocketAndTimerThreads = doSocketAndTimerThreads;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f2f40
// (public: __thiscall WONAPI::WONAPICoreEx::WONAPICoreEx(void))
// TEMPLATE: WON 0x006f2f80
// (public: virtual __thiscall WONAPI::WONAPICoreEx::~WONAPICoreEx(void))
// TEMPLATE: WON 0x006f2f60
// (public: virtual void * __thiscall WONAPI::WONAPICoreEx::`scalar deleting destructor'(unsigned int))
// TEMPLATE: WON 0x006f28a0
// (public: virtual void * __thiscall WONAPI::WONAPICore::`scalar deleting destructor'(unsigned int))
// TEMPLATE: WON 0x006f28d0
// (protected: __thiscall WONAPI::WONAPICore::WONAPICore(char const *))
