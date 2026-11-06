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
// FUNCTION: WON 0x006aab10
ServerConnection::ServerConnection() : BlockingSocket(0)
{
    mAuthType=AUTH_TYPE_NONE;
    mKeepAlivePeriod=300;
}
// FUNCTION: WON 0x006aaf10
void ServerConnection::StartKeepAlive()
{
    AutoCrit aCrit(mDataCrit);
    if(mKeepAliveOp.get()!=NULL) {
        mKeepAliveOp->Kill(true);
        mKeepAliveOp=NULL;
    }
    if(mKeepAlivePeriod!=0) {
        mKeepAliveOp=new AsyncOp;
        mKeepAliveOp->mCompletion=new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(
            StaticKeepAliveCallback,SmartPtr<RefCount>(this));
        mKeepAliveOp->Run(mKeepAlivePeriod*1000);
    }
}
// FUNCTION: WON 0x006ab0a0
void ServerConnection::Fn2AB0A0(AuthContext* inContext,AuthType inType)
{
    AutoCrit aCrit(mDataCrit);
    mAuthContext=inContext;
    mAuthType=inType;
}
// Native static callback2ab160: generated-looking lifetime expansion, support0.
// FUNCTION: WON 0x006ab160
void ServerConnection::StaticConnectCallback(AsyncOpPtr inOp,SmartPtr<RefCount> inParam)
{
    ((ServerConnection*)inParam.get())->Virtual14((PeerAuthOp*)inOp.get());
}
// FUNCTION: WON 0x006ab1f0
void ServerConnection::Virtual14(PeerAuthOp* inOp)
{
    AutoCrit aCrit(mDataCrit);
    if(inOp!=mPeerAuthOp.get()) return;
    OpCompletionBasePtr aCompletion=mConnectCompletion;
    mPeerAuthOp=NULL;
    aCrit.Leave();
    if(aCompletion.get()!=NULL) aCompletion->Complete(AsyncOpPtr(inOp));
    if(inOp->GetStatus()==0) {
        Fn2B3BF0(new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(
            StaticMsgCallback,SmartPtr<RefCount>(this)));
        Fn2B4500(new RecvMsgOp);
        StartKeepAlive();
    }
}
// FUNCTION: WON 0x006ab6a0
void ServerConnection::StaticMsgCallback(AsyncOpPtr inOp,SmartPtr<RefCount> inParam)
{
    RecvMsgOp* anOp=(RecvMsgOp*)inOp.get();
    ServerConnection* aConnection=(ServerConnection*)inParam.get();
    if(anOp->GetStatus()==0) aConnection->Virtual18(anOp->GetMsg().get());
}
// Native static callback2ab7b0 owning dispatch, support0.
// FUNCTION: WON 0x006ab7b0
void ServerConnection::StaticKeepAliveCallback(AsyncOpPtr inOp,SmartPtr<RefCount> inParam)
{
    ((ServerConnection*)inParam.get())->KeepAliveCallback(inOp.get());
}
// FUNCTION: WON 0x006ab840
void ServerConnection::KeepAliveCallback(AsyncOp* inOp)
{
    AutoCrit aCrit(mDataCrit);
    if(inOp==mKeepAliveOp.get()) {
        mKeepAliveOp=NULL;
        if(inOp->mB39) return;
        SendNoOp();
        StartKeepAlive();
    }
}
// FUNCTION: WON 0x006ab910
void ServerConnection::SendNoOp()
{
    WriteBuffer aBuf(mLengthFieldSize,true);
    aBuf.AppendByte(3);
    aBuf.AppendByte(1);
    aBuf.AppendByte(7);
    Fn2B4740(new SendMsgOp(aBuf.ToByteBuffer(true).get()),-1);
}
// FUNCTION: WON 0x006abdc0
void ServerConnection::SetConnectCompletion(CompletionBase<AsyncOpPtr>* inCompletion)
{
    AutoCrit aCrit(mDataCrit);
    mConnectCompletion=inCompletion;
}
// FUNCTION: WON 0x006abe70
void ServerConnection::Virtual08()
{
    if(mKeepAliveOp.get()!=NULL) {
        mKeepAliveOp->Kill(true);
        mKeepAliveOp=NULL;
    }
    if(mPeerAuthOp.get()!=NULL) {
        mPeerAuthOp->mCompletion=mConnectCompletion;
        if(mPeerAuthOp->GetStatus()==100000) mPeerAuthOp->Kill(true);
        mPeerAuthOp=NULL;
    }
    Fn2B3BF0(NULL);
    QueueSocket::Virtual08();
}
// FUNCTION: WON 0x006abf50
void ServerConnection::Virtual0C()
{
    QueueSocket::Virtual0C();
    mConnectCompletion=NULL;
    mAuthContext=NULL;
}
// FUNCTION: WON 0x006abfc0
ServerConnectionConnectOp::ServerConnectionConnectOp(ServerConnection* inConnection,const IPAddr& inAddr)
    : mConnection(inConnection),mAddr(inAddr)
{
}
// Native owning callback2ac240, support0.
// FUNCTION: WON 0x006ac240
void ServerConnectionConnectOp::StaticConnectCompletion(AsyncOpPtr inOp,SmartPtr<RefCount> inParam)
{
    ((ServerConnectionConnectOp*)inParam.get())->ConnectCompletion(inOp.get());
}
// FUNCTION: WON 0x006ac2d0
void ServerConnectionConnectOp::ConnectCompletion(AsyncOp* inOp)
{
    if(GetStatus()!=100000) return;
    Fn29F220(inOp->GetStatus());
}
// FUNCTION: WON 0x006ac2f0
void ServerConnectionConnectOp::Virtual04()
{
    mConnection->SetConnectCompletion(new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(
        StaticConnectCompletion,SmartPtr<RefCount>(this)));
    mConnection->ConnectAsync(mAddr,mTimeout);
}
// Native CleanupHook2ac380 delegates completion clearing, support0.
// FUNCTION: WON 0x006ac380
void ServerConnectionConnectOp::Virtual08()
{
    mConnection->SetConnectCompletion(NULL);
}

// FUNCTION: WON 0x006abc40
PeerAuthOp* ServerConnection::ConnectAsync(const IPAddr& address,unsigned long timeout)
{
    AutoCrit aCrit(mDataCrit);
    Fn2B1AD0();
    mPeerAuthOp=new PeerAuthOp(address,mAuthContext.get(),mAuthType,this,true);
    mPeerAuthOp->mCompletion=new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(
        StaticConnectCallback,SmartPtr<RefCount>(this));
    mPeerAuthOp->Run(timeout);
    return mPeerAuthOp.get();
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing executable methods.
namespace WONAPI {
void ServerConnection::SetKeepAlivePeriod(DWORD theSeconds)
{
	AutoCrit aCrit(mDataCrit);
	mKeepAlivePeriod = theSeconds;
}

WONStatus ServerConnection::ConnectBlock(const IPAddr &theAddr, DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	Fn2B1AD0();
	mPeerAuthOp = new PeerAuthOp(theAddr, mAuthContext, mAuthType, this,true);
	SmartPtr<PeerAuthOp> aPeerAuthOp = mPeerAuthOp;
	aCrit.Leave();

	aPeerAuthOp->RunBlock(theTimeout);
	Virtual14(aPeerAuthOp);
	return aPeerAuthOp->GetStatus();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ac1a0
// (public: virtual __thiscall WONAPI::ServerConnectionConnectOp::~ServerConnectionConnectOp(void)    [??1ServerConnectionConnectOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006aeab0
// (public: virtual void * __thiscall WONAPI::ParamCompletion<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,class WONAPI::SmartPtr<class WONAPI::RefCount>)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006aabc0
// (public: virtual void * __thiscall WONAPI::ServerConnection::`scalar deleting destructor'(unsigned int)    [??_GServerConnection@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006aabe0
// (public: virtual __thiscall WONAPI::ServerConnection::~ServerConnection(void)    [??1ServerConnection@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006abab0
// (public: virtual void * __thiscall WONAPI::SendBytesOp::`scalar deleting destructor'(unsigned int)    [??_GSendBytesOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006abb60
// (public: virtual void * __thiscall WONAPI::SendMsgOp::`scalar deleting destructor'(unsigned int)    [??_GSendMsgOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ac180
// (public: virtual void * __thiscall WONAPI::ServerConnectionConnectOp::`scalar deleting destructor'(unsigned int)    [??_GServerConnectionConnectOp@WONA)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ae7e0
// (public: __thiscall WONAPI::ParamCompletion<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,class WONAPI::SmartPtr<class WONAPI::RefCount> >::ParamComple)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ae840
// (public: virtual void __thiscall WONAPI::ParamCompletion<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,class WONAPI::SmartPtr<class WONAPI::RefCount> >)
// TEMPLATE: WON 0x006aead0
// (public: virtual __thiscall WONAPI::ParamCompletion<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,class WONAPI::SmartPtr<class WONAPI::RefCount> >::~Pa)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006aad90
// (public: virtual __thiscall WONAPI::BlockingSocket::~BlockingSocket(void)    [??1BlockingSocket@WONAPI@@UAE@XZ])
