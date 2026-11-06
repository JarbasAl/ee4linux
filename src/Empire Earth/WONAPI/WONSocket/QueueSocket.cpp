// Original-backed QueueSocket family. File identity corroborated by native
// anonymous-scope RTTI naming C:\EEX\Empire Earth\WONAPI\WONSocket\QueueSocket.cpp.
// Object boundaries still provisional; source-only, no runtime authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
using namespace WONAPI;
namespace {
// Complete genuine SDK QueueOpFastCompletion owner: callback8/ownedParamc/originalCompletion10.
// SOURCE_ONLY_NO_NATIVE_ID constructor unlocks genuine fast-queue allocation; implicit lifetime.
class QueueOpFastCompletion : public ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> > {
protected:
    typedef void (*Callback)(AsyncOpPtr, SmartPtr<RefCount>);
public:
    OpCompletionBasePtr mOriginalCompletion;
    QueueOpFastCompletion(Callback callback, RefCount* param, CompletionBase<AsyncOpPtr>* original)
        : ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(callback,param), mOriginalCompletion(original) {}
};
// Full5c from actual allocation2b4ac3 and complete six-slot RTTI44caac.
// Native ordinary2b4b90 is implicit, no own-vptr store. All support credit0.
class QueueCloseOp : public SocketOp {
public:
    QueueCloseOp() : SocketOp(NULL) {}
    virtual WONStatus Virtual10() { return 0; }
};
}
namespace WONAPI {
// FUNCTION: WON 0x006b3930
QueueSocket::QueueSocket(int inType) : AsyncSocket(inType)
{
    mB9C = false;
}

// FUNCTION: WON 0x006b39d0
AsyncSocket* QueueSocket::Duplicate()
{
    return new QueueSocket(mType);
}

// FUNCTION: WON 0x006b3bf0
void QueueSocket::Fn2B3BF0(CompletionBase<AsyncOpPtr>* inCompletion)
{
    AutoCrit aCrit(mDataCrit);
    mRepeatCompletion = inCompletion;
}

// FUNCTION: WON 0x006b3ca0
void QueueSocket::SetCloseCompletion(CompletionBase<AsyncOpPtr>* inCompletion)
{
    AutoCrit aCrit(mDataCrit);
    mCloseCompletion = inCompletion;
}

// FUNCTION: WON 0x006b3d50
void QueueSocket::Fn2B3D50(long inTimeout)
{
    AutoCrit aCrit(mDataCrit);
    Fn2B3ED0();
    if (inTimeout != 0) {
        mCloseTimer = new AsyncOp;
        mCloseTimer->mCompletion =
            new ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(Fn2B3F80, this);
        mCloseTimer->Run(inTimeout);
    }
}

// FUNCTION: WON 0x006b3ed0
void QueueSocket::Fn2B3ED0()
{
    if (mCloseTimer.get() != NULL) {
        AsyncOpPtr aTimer = mCloseTimer;
        mCloseTimer = NULL;
        aTimer->Kill(true);
    }
}

// FUNCTION: WON 0x006b3f80
void QueueSocket::Fn2B3F80(AsyncOpPtr inOp, SmartPtr<RefCount> inParam)
{
    static_cast<QueueSocket*>(inParam.get())->Fn2B4010(inOp.get());
}

// FUNCTION: WON 0x006b4010
void QueueSocket::Fn2B4010(AsyncOp* inTimer)
{
    AutoCrit aCrit(mDataCrit);
    if (inTimer == mCloseTimer.get() && inTimer->GetStatus() == 100001) {
        mCloseTimer = NULL;
        OpCompletionBasePtr aCloseCompletion = mCloseCompletion;
        aCrit.Leave();
        Fn2B1AD0();
        if (aCloseCompletion.get() != NULL) {
            SmartPtr<CloseOp> aCloseOp = new CloseOp(this);
            aCloseOp->mCompletion = aCloseCompletion;
            aCloseOp->ForceFinish(100002);
        }
    }
}

// FUNCTION: WON 0x006b4190
void QueueSocket::Fn2B4190(AsyncOpPtr inOp, SmartPtr<RefCount> inParam)
{
    static_cast<QueueSocket*>(inParam.get())->Fn2B4220(static_cast<SocketOp*>(inOp.get()));
}

// FUNCTION: WON 0x006b4220
void QueueSocket::Fn2B4220(SocketOp* inOp)
{
    AutoCrit aCrit(mDataCrit);
    bool aClose = (inOp->GetStatus() != 0 && mType != 1) || !Fn2B08C0();
    OpCompletionBasePtr aCompletion;
    if (inOp == mQueueOp.get()) {
        aCompletion = mCurQueueCompletion;
        mCurQueueCompletion = NULL;
        mQueueOp = NULL;
        if (!aClose) {
            if (mB9C && mOpQueue.empty())
                aClose = true;
            else
                Fn2B48B0();
        }
    }
    else if (inOp == mRepeatOp.get()) {
        aCompletion = mRepeatCompletion;
        if (inOp->mB39)
            aClose = true;
        if (!aClose) {
            // Slot14 factory consumers/producers are the real SocketOp family:
            // Accept60/RecvBytesac/RecvMsgb4/RecvFrom90 or NULL, all zero base offset.
            mRepeatOp = static_cast<SocketOp*>(mRepeatOp->Duplicate());
            Fn2B45B0();
        }
        else {
            if (mB9C && inOp->mB39) {
                aClose = false;
                aCompletion = NULL;
            }
            mRepeatOp = NULL;
        }
    }
    else {
        QueueOpFastCompletion* aFast = dynamic_cast<QueueOpFastCompletion*>(inOp->mCompletion.get());
        if (aFast != NULL)
            aCompletion = aFast->mOriginalCompletion;
    }
    OpCompletionBasePtr aCloseCompletion;
    if (aClose)
        aCloseCompletion = mCloseCompletion;
    aCrit.Leave();
    if (aCompletion.get() != NULL)
        aCompletion->Complete(inOp);
    if (aClose) {
        Fn2B1AD0();
        if (aCloseCompletion.get() != NULL) {
            SmartPtr<CloseOp> aCloseOp = new CloseOp(this);
            aCloseOp->mCompletion = aCloseCompletion;
            aCloseOp->ForceFinish(inOp->GetStatus());
        }
    }
}

// FUNCTION: WON 0x006b4500
void QueueSocket::Fn2B4500(SocketOp* inOp)
{
    AutoCrit aCrit(mDataCrit);
    mRepeatOp = inOp;
    Fn2B45B0();
}

// FUNCTION: WON 0x006b45b0
void QueueSocket::Fn2B45B0()
{
    AutoCrit aCrit(mDataCrit);
    if (mRepeatOp.get() == NULL || mRepeatOp->GetStatus() == 100000)
        return;
    mRepeatOp->mCompletion =
        new ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(Fn2B4190, this);
    mRepeatOp->mSocket = this;
    mRepeatOp->Run(-1);
}

// FUNCTION: WON 0x006b4740
void QueueSocket::Fn2B4740(SocketOp* inOp, long inTimeout)
{
    AutoCrit aCrit(mDataCrit);
    inOp->mSocket = this;
    inOp->Fn29F1B0(inTimeout);
    if (inOp->mCompletion.get() == NULL)
        inOp->mCompletion = mDefaultQueueCompletion;
    mOpQueue.push_back(inOp);
    Fn2B48B0();
}

// FUNCTION: WON 0x006b48b0
void QueueSocket::Fn2B48B0()
{
    AutoCrit aCrit(mDataCrit);
    if (mQueueOp.get() != NULL || mOpQueue.empty())
        return;
    mQueueOp = mOpQueue.front();
    mOpQueue.pop_front();
    mCurQueueCompletion = mQueueOp->mCompletion;
    mQueueOp->mCompletion =
        new ParamCompletion<AsyncOpPtr, SmartPtr<RefCount> >(Fn2B4190, this);
    mQueueOp->mSocket = this;
    mQueueOp->Run(mQueueOp->mTimeout);
}

// FUNCTION: WON 0x006b4a70
void QueueSocket::QueueClose(long inTimeout)
{
    AutoCrit aCrit(mDataCrit);
    mB9C = true;
    if (mRepeatOp.get() != NULL)
        mRepeatOp->Kill(true);
    SmartPtr<QueueCloseOp> aCloseOp = new QueueCloseOp;
    aCloseOp->SetFlag3A(false);
    Fn2B4740(aCloseOp.get(), -1);
    if (inTimeout != -1)
        Fn2B3D50(inTimeout);
}

// FUNCTION: WON 0x006b4c00
void QueueSocket::Virtual04()
{
    mB9C = false;
}

// FUNCTION: WON 0x006b4c10
void QueueSocket::Virtual08()
{
    mB9C = false;
    if (mRepeatOp.get() != NULL) {
        mRepeatOp->mCompletion = mRepeatCompletion;
        mRepeatOp->Kill(true);
        mRepeatOp = NULL;
    }
    if (mQueueOp.get() != NULL) {
        mQueueOp->mCompletion = mCurQueueCompletion;
        mQueueOp->Kill(true);
        mQueueOp = NULL;
        mCurQueueCompletion = NULL;
    }
    std::list<SmartPtr<SocketOp> >::iterator anItr;
    anItr=mOpQueue.begin();
    while(true) {
        if(!(anItr != mOpQueue.end())) break;
        (*anItr)->Kill(true);
        ++anItr;
    }
    if (mCloseTimer.get() != NULL) {
        mCloseTimer->Kill(true);
        mCloseTimer = NULL;
    }
    mOpQueue.clear();
}

// FUNCTION: WON 0x006b4d40
void QueueSocket::Virtual0C()
{
    mDefaultQueueCompletion = NULL;
    mCurQueueCompletion = NULL;
    mRepeatCompletion = NULL;
    mCloseCompletion = NULL;
}
}
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing executable methods.
namespace WONAPI {
void QueueSocket::SetQueueCompletion(CompletionBase<AsyncOpPtr> *theCompletion)
{
	AutoCrit aCrit(mDataCrit);
	mDefaultQueueCompletion = theCompletion;
}

void QueueSocket::QueueOpFast(SocketOp *theOp, DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	if(mQueueOp.get()!=NULL)
	{
		Fn2B4740(theOp,theTimeout);
		return;
	}

	theOp->SetSocket(this);
	OpCompletionBasePtr anOriginalCompletion  = theOp->mCompletion;
	if(anOriginalCompletion.get()==NULL)
		anOriginalCompletion =  mDefaultQueueCompletion;

	theOp->mCompletion = new QueueOpFastCompletion(Fn2B4190,this,anOriginalCompletion);
	theOp->Run(theTimeout);
	if(theOp->mStatus==100000)
	{
		mQueueOp = theOp;
		mCurQueueCompletion = anOriginalCompletion;
	}
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b3a40
// (public: virtual void * __thiscall WONAPI::QueueSocket::`scalar deleting destructor'(unsigned int)    [??_GQueueSocket@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b4b90
// (public: virtual __thiscall WONAPI::SocketOp::~SocketOp(void)    [??1SocketOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b3a60
// (public: virtual __thiscall WONAPI::QueueSocket::~QueueSocket(void)    [??1QueueSocket@WONAPI@@UAE@XZ])
