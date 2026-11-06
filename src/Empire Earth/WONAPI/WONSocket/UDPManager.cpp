// Original-backed UDP manager family; exact original TU grouping provisional.
// Canonical candidate source; full ordinary COFF/EH/data review pending.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {

// FUNCTION: WON 0x006b76b0
UDPManager::UDPManager() : mCleanListItr(NULL), mCleanMapItr(NULL)
{
    mCleanListItr = mOpList.begin();
    mCleanMapItr = mOpMap.begin();
    mMaxRecvBytes = 1024;
    mAutoRecv = false;
}

// FUNCTION: WON 0x006b78e0
void UDPManager::Close()
{
    if (mSocket.get() == NULL)
        return;
    mSocket->Close();
    mSocket = NULL;
    if (mAutoRecvOp.get() != NULL) {
        mAutoRecvOp->Kill(true);
        mAutoRecvOp = NULL;
    }
    for (OpList::iterator aListItr = mOpList.begin(); aListItr != mOpList.end(); ++aListItr)
        (*aListItr)->Kill(true);
    for (OpMap::iterator aMapItr = mOpMap.begin(); aMapItr != mOpMap.end(); ++aMapItr)
        aMapItr->second->Kill(true);
    mOpList.clear();
    mOpMap.clear();
    mCleanListItr = mOpList.end();
    mCleanMapItr = mOpMap.end();
}

// FUNCTION: WON 0x006b7b40
void UDPManager::SetAutoRecv(bool inAutoRecv)
{
    if (inAutoRecv) {
        if (mAutoRecvOp.get() == NULL)
            mAutoRecvOp = new UDPManagerRecvOp;
    }
    else
        mAutoRecvOp = NULL;
}

// FUNCTION: WON 0x006b7c50
void UDPManager::QueueOp(UDPManagerRecvOp* inOp, unsigned long inTimeout)
{
    if (inOp->mCompletion.get() == NULL)
        inOp->mCompletion = mDefaultCompletion;
    if (mSocket.get() == NULL) {
        if (inOp->GetStatus() != 100000)
            inOp->Run(inTimeout);
        inOp->Kill(true);
        return;
    }
    CleanOps();
    if (inOp->mAddress.IsValid())
        mOpMap.insert(OpMap::value_type(inOp->mAddress, inOp));
    else
        mOpList.push_back(inOp);
    if (inOp->GetStatus() != 100000)
        inOp->Run(inTimeout);
}

// FUNCTION: WON 0x006b7ec0
void UDPManager::Send(ByteBuffer* inData, const IPAddr& inAddress)
{
    mSocket->Fn2B4740(new SendBytesToOp(inData, inAddress), -1);
}

// FUNCTION: WON 0x006b8000
void UDPManager::CleanOps()
{
    for (int i = 0; i < 10; ++i) {
        if (mCleanListItr == mOpList.end()) {
            mCleanListItr = mOpList.begin();
            break;
        }
        if ((*mCleanListItr)->GetStatus() != 100000)
            mOpList.erase(mCleanListItr++);
        else
            ++mCleanListItr;
    }
    for (int j = 0; j < 10; ++j) {
        if (mCleanMapItr == mOpMap.end()) {
            mCleanMapItr = mOpMap.begin();
            break;
        }
        if (mCleanMapItr->second->GetStatus() != 100000)
            mOpMap.erase(mCleanMapItr++);
        else
            ++mCleanMapItr;
    }
}
// Round3 additional original operation behavior; source-only.


// FUNCTION: WON 0x006b6bf0
void UDPManagerRecvOp::Fn2B6BF0(MessageVerifier inVerifier, void* inContext)
{
    mVerifier = inVerifier;
    mVerifierContext = inContext;
}
// FUNCTION: WON 0x006b6c10
WONStatus UDPManagerRecvOp::Virtual0C(ReadBuffer& inMessage)
{
    return mVerifier ? mVerifier(inMessage, mVerifierContext) : 0;
}
// FUNCTION: WON 0x006b6d70
void UDPManagerRecvOp::Fn2B6D70(AsyncOpPtr inOp, SmartPtr<RefCount> inParam)
{
    if (inOp->GetStatus() != 0 || inOp->mB39)
        return;
    static_cast<UDPManagerRecvOp*>(inParam.get())->Fn2B6E60(static_cast<RecvBytesFromOp*>(inOp.get()));
}

// FUNCTION: WON 0x006b7250
WONStatus UDPManagerSendRecvOp::Fn2B7250(ReadBuffer& inMessage, void* inParam)
{
    return static_cast<UDPManagerSendRecvOp*>(inParam)->Virtual0C(inMessage);
}
// FUNCTION: WON 0x006b7550
void UDPManagerSendRecvOp::Fn2B7550(AsyncOpPtr inOp, SmartPtr<RefCount> inParam)
{
    static_cast<UDPManagerSendRecvOp*>(inParam.get())->Fn2B75E0(inOp.get());
}

// FUNCTION: WON 0x006b6c30
void UDPManagerRecvOp::Fn2B6C30()
{
    if (mSocket.get() == NULL)
        return;
    SmartPtr<RecvBytesFromOp> anOp = new RecvBytesFromOp(1024, NULL);
    anOp->mCompletion = new ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(Fn2B6D70, this);
    mSocket->Fn2B4740(anOp.get(), -1);
}

// FUNCTION: WON 0x006b6e60
void UDPManagerRecvOp::Fn2B6E60(RecvBytesFromOp* inOp)
{
    if (mStatus != 100000)
        return;
    if (mAddress.IsValid() && inOp->mFromAddress != mAddress) {
        Fn2B6C30();
        return;
    }
    const ByteBuffer* aBuffer = inOp->GetBytes().get();
    try {
        ReadBuffer aMessage(aBuffer->data(), aBuffer->length());
        WONStatus aStatus = Virtual0C(aMessage);
        if (aStatus != 100004) {
            Fn29F220(aStatus);
            return;
        }
    }
    catch (ReadBufferException&) {
    }
    Fn2B6C30();
}

// FUNCTION: WON 0x006b68b0
UDPManagerRecvOp::UDPManagerRecvOp() { mVerifier = NULL; }

// FUNCTION: WON 0x006b6a10
UDPManagerRecvOp::UDPManagerRecvOp(const IPAddr& inAddress)
{
    mAddress = inAddress;
    mVerifier = NULL;
}

// FUNCTION: WON 0x006b6f60
void UDPManagerRecvOp::Virtual08()
{
    if (mSocket.get() != NULL && mOwnSocket) {
        mSocket->Close();
        mSocket = NULL;
    }
}

// FUNCTION: WON 0x006b6fb0
void UDPManagerRecvOp::Virtual04()
{
    if (mRunMode == 2) {
        Fn29F220(100009);
        return;
    }
    if (mManager.get() != NULL) {
        mManager->QueueOp(this, mTimeout);
        return;
    }
    if (mSocket.get() == NULL) {
        mSocket = new QueueSocket(1);
        mSocket->Fn2B0D90(0, 0);
        mOwnSocket = true;
    }
    Fn2B6C30();
}

// FUNCTION: WON 0x006b70b0
UDPManagerSendRecvOp::UDPManagerSendRecvOp() : mRetryLimit(0) { mReceiveTimeout=10000; }

// FUNCTION: WON 0x006b7260
void UDPManagerSendRecvOp::Fn2B7260()
{
    mRecvOp = new UDPManagerRecvOp(mAddress);
    mRecvOp->mCompletion = new ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(Fn2B7550, this);
    mRecvOp->Fn2B6BF0(Fn2B7250, this);
    if (mManager.get() != NULL) {
        mManager->Send(mOutgoing.get(), mAddress);
        mRecvOp->mManager = mManager;
    }
    else {
        mSocket->Fn2B4740(new SendBytesToOp(mOutgoing.get(), mAddress, NULL), -1);
        mRecvOp->mSocket = mSocket;
        mRecvOp->mOwnSocket = false;
    }
    mRecvOp->Run(mReceiveTimeout);
}

// FUNCTION: WON 0x006b7470
void UDPManagerSendRecvOp::Virtual04()
{
    if (mRunMode == 2) {
        Fn29F220(100009);
        return;
    }
    mAttemptCount = 0;
    if (mManager.get() == NULL) {
        mSocket = new QueueSocket(1);
        mSocket->Fn2B0D90(0, 0);
    }
    Fn2B7260();
}

// FUNCTION: WON 0x006b75e0
void UDPManagerSendRecvOp::Fn2B75E0(AsyncOp* inOp)
{
    if (mStatus != 100000 || inOp != mRecvOp.get())
        return;
    WONStatus aStatus = inOp->GetStatus();
    if (aStatus != 100001) {
        Fn29F220(aStatus);
        return;
    }
    if (mAttemptCount >= mRetryLimit) {
        Fn29F220(100015);
        return;
    }
    ++mAttemptCount;
    Fn2B7260();
}

// FUNCTION: WON 0x006b7630
void UDPManagerSendRecvOp::Virtual08()
{
    if (mRecvOp.get() != NULL) {
        mRecvOp->Kill(true);
        mRecvOp = NULL;
    }
    if (mSocket.get() != NULL) {
        mSocket->Close();
        mSocket = NULL;
    }
}
// FUNCTION: WON 0x006b8230
void UDPManager::Fn2B8230(AsyncOpPtr inOp, SmartPtr<RefCount> inParam)
{
    if (inOp->Succeeded())
        static_cast<UDPManager*>(inParam.get())->Fn2B8320(static_cast<RecvBytesFromOp*>(inOp.get()));
}

// FUNCTION: WON 0x006b79c0
WONStatus UDPManager::Fn2B79C0(int inPort, bool inBroadcast)
{
    Close();
    mSocket = new QueueSocket(1);
    if (inBroadcast)
        mSocket->Fn2B0A50(true);
    WONStatus aStatus = mSocket->Fn2B0D90(inPort, 0);
    if (aStatus != 0)
        return aStatus;
    mSocket->Fn2B3BF0(new ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(Fn2B8230, this));
    mSocket->Fn2B4500(new RecvBytesFromOp(mMaxRecvBytes, NULL));
    return aStatus;
}


// SUPPORT-ONLY: actual iterator-preserving list erase2b8160; helper quota0.
// FUNCTION: WON 0x006b81b0
// FUNCTION: WON 0x006b8160
void UDPManager::Erase(const OpList::iterator& inItr)
{
    if (inItr == mCleanListItr)
        ++mCleanListItr;
    mOpList.erase(inItr);
}

// SUPPORT-ONLY: actual iterator-preserving map erase2b81b0; helper quota0.
void UDPManager::Erase(const OpMap::iterator& inItr)
{
    if (inItr == mCleanMapItr)
        ++mCleanMapItr;
    mOpMap.erase(inItr);
}

// FUNCTION: WON 0x006b8320
void UDPManager::Fn2B8320(RecvBytesFromOp* inOp)
{
    ByteBuffer* aBuffer = inOp->GetBytes().get();
    ReadBuffer aMessage(aBuffer->data(), aBuffer->length());
    if (!mOpMap.empty()) {
        OpMap::iterator aMapItr = mOpMap.lower_bound(inOp->mFromAddress);
        while (aMapItr != mOpMap.end() && aMapItr->first == inOp->mFromAddress) {
            UDPManagerRecvOp* anOp = aMapItr->second.get();
            if (anOp->GetStatus() != 100000) {
                Erase(aMapItr++);
                continue;
            }
            try {
                aMessage.Rewind();
                WONStatus aStatus = anOp->Virtual0C(aMessage);
                if (aStatus == 100059)
                    QueueOp(new UDPManagerRecvOp(anOp->mAddress), anOp->TimeLeft());
                else if (aStatus != 100004) {
                    anOp->mReceivedData = aBuffer;
                    anOp->Fn29F220(aStatus);
                    Erase(aMapItr);
                    return;
                }
            }
            catch (ReadBufferException&) {
            }
            ++aMapItr;
        }
    }
    if (!mOpList.empty()) {
        OpList::iterator aListItr = mOpList.begin();
        while (aListItr != mOpList.end()) {
            UDPManagerRecvOp* anOp = aListItr->get();
            if (anOp->GetStatus() != 100000) {
                Erase(aListItr++);
                continue;
            }
            try {
                aMessage.Rewind();
                WONStatus aStatus = anOp->Virtual0C(aMessage);
                if (aStatus != 100004) {
                    anOp->mAddress = inOp->mFromAddress;
                    anOp->mReceivedData = aBuffer;
                    anOp->Fn29F220(aStatus);
                    Erase(aListItr);
                    return;
                }
            }
            catch (ReadBufferException&) {
            }
            ++aListItr;
        }
    }
    if (mAutoRecvOp.get() != NULL) {
        mAutoRecvOp->mReceivedData = aBuffer;
        mAutoRecvOp->mAddress = inOp->mFromAddress;
        mAutoRecvOp->mCompletion = mDefaultCompletion;
        mAutoRecvOp->ForceFinish(0);
        mAutoRecvOp = new UDPManagerRecvOp;
    }
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006b6930
// (public: virtual __thiscall WONAPI::UDPManagerRecvOp::~UDPManagerRecvOp(void)    [??1UDPManagerRecvOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006b7140
// (public: virtual __thiscall WONAPI::UDPManagerSendRecvOp::~UDPManagerSendRecvOp(void)    [??1UDPManagerSendRecvOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006b7790
// (public: virtual void * __thiscall WONAPI::UDPManager::`scalar deleting destructor'(unsigned int)    [??_GUDPManager@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b7e30
// (public: __thiscall std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::UDPManagerRecvOp> >::~pair<class WONAPI::IPAddr const ,c)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006b6910
// (public: virtual void * __thiscall WONAPI::UDPManagerRecvOp::`scalar deleting destructor'(unsigned int)    [??_GUDPManagerRecvOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b7120
// (public: virtual void * __thiscall WONAPI::UDPManagerSendRecvOp::`scalar deleting destructor'(unsigned int)    [??_GUDPManagerSendRecvOp@WONAPI@@UAEPAX)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006b77b0
// (public: virtual __thiscall WONAPI::UDPManager::~UDPManager(void)    [??1UDPManager@WONAPI@@UAE@XZ])
