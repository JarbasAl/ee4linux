// Original-backed blocking operation family; source only, no runtime authority.
// SendBytes/SendMsg const-ownership candidates are excluded entirely.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006b60d0
SocketThread::SocketThread() { mSingleThreaded = false; }
// FUNCTION: WON 0x006b6120
SocketThread::~SocketThread() {}
// FUNCTION: WON 0x006b6140
void SocketThread::ThreadFunc()
{
    while(!mStopped) Pump(1000);
}
// FUNCTION: WON 0x006b6160
bool SocketThread::DoOp(SocketOp* inOp)
{
    if(inOp->GetStatus()==100000) {
        WONStatus aStatus=inOp->Virtual10();
        if(aStatus!=100001) {
            inOp->mB44=false;
            inOp->Fn29F220(aStatus);
        }
    }
    if(inOp->GetStatus()!=100000) {
        inOp->mB44=false;
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006b61a0
SocketThreadSimple::~SocketThreadSimple()
{
    PurgeOps();
}
// FUNCTION: WON 0x006b6380
void SocketThreadSimple::PurgeOps()
{
    OpSet::iterator anItr=mOpSet.begin();
    while(anItr!=mOpSet.end()) {
        SmartPtr<SocketOp> anOp=*anItr;
        anOp->Kill(true);
        ++anItr;
    }
    mOpSet.clear();
}
// FUNCTION: WON 0x006b64f0
void SocketThreadSimple::AddSocketOp(SocketOp* inOp)
{
    AutoCrit aCrit(mDataCrit);
    mOpSet.insert(SmartPtr<SocketOp>(inOp));
    inOp->mB44=true;
}
// FUNCTION: WON 0x006b6660
void SocketThreadSimple::RemoveSocketOp(SocketOp* inOp)
{
    AutoCrit aCrit(mDataCrit);
    mOpSet.erase(SmartPtr<SocketOp>(inOp));
    inOp->mB44=false;
}
// FUNCTION: WON 0x006b6760
void SocketThreadSimple::Pump(unsigned long inWaitTime)
{
    AutoCrit aCrit(mDataCrit);
    OpSet::iterator anItr=mOpSet.begin();
    while(anItr!=mOpSet.end()) {
        SocketOp* anOp=anItr->get();
        if(DoOp(anOp)) mOpSet.erase(anItr++);
        else ++anItr;
    }
    aCrit.Leave();
    if(inWaitTime>=20) inWaitTime=20;
    if(inWaitTime>0) mSignalEvent.WaitFor(inWaitTime);
}
// FUNCTION: WON 0x006b6890
void SocketThreadSimple::ThreadFunc()
{
    while(!mStopped) Pump(20);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b6100
// (public: virtual void * __thiscall WONAPI::SocketThread::`scalar deleting destructor'(unsigned int)    [??_GSocketThread@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b62a0
// (public: virtual void * __thiscall WONAPI::SocketThreadSimple::`scalar deleting destructor'(unsigned int)    [??_GSocketThreadSimple@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b62c0
// (public: __thiscall std::set<class WONAPI::SmartPtr<class WONAPI::SocketOp>,struct WONAPI::SmartPtr<class WONAPI::SocketOp>::Comp,class std::allocator<)
