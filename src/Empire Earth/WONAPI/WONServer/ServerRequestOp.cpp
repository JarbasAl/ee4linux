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
// Native Init2acf10, support0.
// FUNCTION: WON 0x006acf10
void ServerRequestOp::Init()
{
    mLengthFieldSize=4;
    mAuthType=AUTH_TYPE_NONE;
    mMaxRecvTime=mMaxSendTime=mMaxConnectTime=20000;
    mLastServerError=mGetCertStatus=100004;
    mUseAuth2=true;
    mUseLastServerErrorForFinishStatus=false;
    mCloseConnection=true;
}
// FUNCTION: WON 0x006acf60
ServerRequestOp::ServerRequestOp(ServerContext* inContext) : mAddrItr(AddrList::iterator())
{
    Init();
    SetServerContext(inContext);
}
// FUNCTION: WON 0x006ad050
ServerRequestOp::ServerRequestOp(const IPAddr& inAddress) : mAddrItr(AddrList::iterator())
{
    Init();
    SetAddr(inAddress);
}
// FUNCTION: WON 0x006ad110
ServerRequestOp::~ServerRequestOp()
{
}
// Native owning setter2ad2e0, support0.
// FUNCTION: WON 0x006ad2e0
void ServerRequestOp::SetServerContext(ServerContext* inContext)
{
    mServerContext=inContext;
}
// FUNCTION: WON 0x006ad340
void ServerRequestOp::SetAddr(const IPAddr& inAddress)
{
    mServerContext=NULL;
    mAddrList.clear();
    mAddrList.push_back(inAddress);
}
// Native getter2ad410; quota0.
// FUNCTION: WON 0x006ad410
IPAddr ServerRequestOp::GetAddr() const
{
    if(mAddrList.empty()) return IPAddr();
    return mAddrList.front();
}
// FUNCTION: WON 0x006ad5b0
void ServerRequestOp::FinishSaveSession(WONStatus inStatus)
{
    Fn29F220(inStatus);
    if(mAuthType==AUTH_TYPE_SESSION) mAuthContext->PutSession(mCurAddr,mCurSession.get());
    mUsedSession=mCurSession;
    mCurSession=NULL;
}
// FUNCTION: WON 0x006ad660
void ServerRequestOp::SetLastServerError(WONStatus inError)
{
    if(mLastServerError==100004) mUseLastServerErrorForFinishStatus=true;
    else if(inError!=mLastServerError) mUseLastServerErrorForFinishStatus=false;
    mLastServerError=inError;
}
// Native InvalidReplyHeader2ad6a0; support0.
// FUNCTION: WON 0x006ad6a0
WONStatus ServerRequestOp::InvalidReplyHeader()
{
    SetLastServerError(100063);
    return 100057;
}
// FUNCTION: WON 0x006ad6b0
bool ServerRequestOp::TryConnect()
{
    if(mCurSession.get()!=NULL) {
        mSocket->Fn2B0620(new CryptTransform(mCurSession.get()));
        mReusingSession=true;
    }
    if(mRunMode==1) {
        ConnectOp* aConnectOp=new ConnectOp(mCurAddr);
        QueueSocketOp(aConnectOp,ConnectTime());
        Send();
        return true;
    }
    WONStatus aStatus=mSocket->Connect(mCurAddr,ConnectTime());
    if(aStatus==0) return true;
    SetLastServerError(aStatus);
    return false;
}
// FUNCTION: WON 0x006add80
bool ServerRequestOp::TryNextServer()
{
    if(mStatus!=100000) return false;
    for(;;) {
        mTracker.KillAll();
        Virtual18();
        if(mSocket.get()!=NULL) {
            mSocket->Fn2B1AD0();
            mSocket=NULL;
        }
        if(mAddrItr!=mAddrList.begin() && !mSessionExpired && mServerContext.get()!=NULL)
            mServerContext->NotifyFailed(mCurAddr);
        if(mAddrItr==mAddrList.end()) {
            if(mUseLastServerErrorForFinishStatus)
                Fn29F220(mLastServerError);
            else
                Fn29F220(100054);
            return false;
        }
        mSocket=new BlockingSocket(0);
        mSocket->SetLengthFieldSize(mLengthFieldSize);
        mCurAddr=*mAddrItr;
        bool wasReusingSession=mReusingSession;
        mReusingSession=false;
        mSessionExpired=false;
        ++mAddrItr;
        if(!wasReusingSession && mAuthType==AUTH_TYPE_SESSION)
            mCurSession=mAuthContext->GetSession(mCurAddr);
        else mCurSession=NULL;
        if(mCurSession.get()!=NULL || mAuthType==AUTH_TYPE_NONE) {
            if(TryConnect()) return true;
        } else {
            if(TryPeerToPeerAuth()) return true;
            if(GetStatus()!=100000) return false;
        }
        if(TimeLeft()<=0) {
            Fn29F220(100001);
            return false;
        }
    }
}
// FUNCTION: WON 0x006ae050
WONStatus ServerRequestOp::Send()
{
    for(;;) {
        WONStatus aReqStatus=Virtual10();
        if(aReqStatus!=100058 && aReqStatus!=100059) return aReqStatus;
        if(mRunMode==1) {
            SendMsgOp* aSendOp=new SendMsgOp(mRequest.get());
            QueueSocketOp(aSendOp,SendTime());
        } else {
            WONStatus aStatus=mSocket->SendMsg(mRequest.get(),SendTime());
            if(aStatus!=0) {
                SetLastServerError(aStatus);
                return 100057;
            }
        }
        if(aReqStatus==100059) {
            if(mRunMode==1) {
                RecvMsgOp* aRecvOp=new RecvMsgOp;
                QueueSocketOp(aRecvOp,RecvTime());
            }
            return aReqStatus;
        }
    }
}
// FUNCTION: WON 0x006ae200
bool ServerRequestOp::CheckExpiredSession()
{
    if(!mReusingSession || mResponse->length()<10) return false;
    try {
        ReadBuffer aBuf(mResponse->data(),mResponse->length());
        unsigned long aServiceType=aBuf.ReadLong();
        unsigned long aMessageType=aBuf.ReadLong();
        if(aServiceType==1 && aMessageType==15) {
            mSessionExpired=true;
            --mAddrItr;
            return true;
        }
    } catch(ReadBufferException&) {}
    return false;
}
// FUNCTION: WON 0x006ae2d0
WONStatus ServerRequestOp::Recv()
{
    for(;;) {
        WONStatus aStatus=mSocket->RecvMsg(mResponse,RecvTime());
        if(aStatus!=0) {
            SetLastServerError(aStatus);
            return 100057;
        }
        if(CheckExpiredSession()) return 100061;
        try {
            aStatus=Virtual14();
            if(aStatus!=100059) return aStatus;
        } catch(ReadBufferException&) {
            SetLastServerError(100064);
            return 100057;
        }
    }
}
// FUNCTION: WON 0x006ae390
void ServerRequestOp::Virtual04()
{
    mGetCertStatus=mLastServerError=100004;
    mUseLastServerErrorForFinishStatus=false;
    mUsedSession=NULL;
    if(mServerContext.get()!=NULL) mServerContext->CopyAddresses(mAddrList);
    if(mAddrList.empty()) {
        Fn29F220(100055);
        return;
    }
    mAddrItr=mAddrList.begin();
    mReusingSession=false;
    mSessionExpired=false;
    if(mAuthType!=AUTH_TYPE_NONE && mAuthContext.get()==NULL) {
        Fn29F220(100056);
        return;
    }
    if(mRunMode==1) {
        TryNextServer();
        return;
    }
    while(TryNextServer()) {
        for(;;) {
            WONStatus aStatus;
            do { aStatus=Send(); } while(aStatus==100058);
            if(aStatus==100057) break;
            do { aStatus=Recv(); } while(aStatus==100059);
            if(aStatus==100057 || aStatus==100061) break;
            if(aStatus==100058) continue;
            FinishSaveSession(aStatus);
            return;
        }
        if(TimeLeft()==0) {
            Fn29F220(100001);
            return;
        }
    }
}
// FUNCTION: WON 0x006ae4c0
void ServerRequestOp::Virtual08()
{
    AsyncOpWithTracker::Virtual08();
    if(GetStatus()!=0 && (GetStatus()==100001 || mB39) && mServerContext.get()!=NULL)
        mServerContext->NotifyFailed(mCurAddr);
    if(mSocket.get()!=NULL && (mCloseConnection || GetStatus()!=0 || mB39)) {
        mSocket->Fn2B1AD0();
        mSocket=NULL;
    }
    mCurSession=NULL;
}

// FUNCTION: WON 0x006ad950
bool ServerRequestOp::TryPeerToPeerAuth()
{
    SmartPtr<PeerAuthOp> peerAuth=new PeerAuthOp(mCurAddr,mAuthContext.get(),mAuthType,mSocket.get(),mUseAuth2);
    peerAuth->CopyMaxTimes(this);
    peerAuth->SetClientKeySize(mClientKeySize,mUseClientKey);
    if(mRunMode==1) {
        Track(peerAuth.get(),2);
        peerAuth->Run(-1);
        return true;
    }
    if(peerAuth->RunBlock(TimeLeft())) {
        mCurSession=peerAuth->GetSession();
        return true;
    }
    if(peerAuth->GetStatus()==100053) {
        mGetCertStatus=peerAuth->GetGetCertStatus();
        Fn29F220(100062);
    } else SetLastServerError(peerAuth->GetStatus());
    return false;
}
// FUNCTION: WON 0x006adb10
bool ServerRequestOp::CallbackHook(AsyncOp* op,int param)
{
    if(op->mB39) {
        Kill(true);
        return true;
    }
    if(op->GetStatus()!=0) {
        if(param==2 && op->GetStatus()==100053) {
            mGetCertStatus=((PeerAuthOp*)op)->GetGetCertStatus();
            Fn29F220(100062);
            return true;
        }
        SetLastServerError(op->GetStatus());
        TryNextServer();
        return true;
    }
    switch(param) {
    case 2:
        mCurSession=((PeerAuthOp*)op)->GetSession();
        Send();
        break;
    case 1: {
        RecvMsgOp* receive=dynamic_cast<RecvMsgOp*>(op);
        if(receive==NULL) break;
        mResponse=receive->GetMsg();
        if(CheckExpiredSession()) {
            TryNextServer();
            break;
        }
        try {
            WONStatus status=Virtual14();
            if(status==100057) TryNextServer();
            else if(status==100058) Send();
            else if(status==100059) {
                RecvMsgOp* nextReceive=new RecvMsgOp;
                QueueSocketOp(nextReceive,RecvTime());
            } else if(status==100060) return true;
            else FinishSaveSession(status);
        } catch(ReadBufferException&) {
            SetLastServerError(100064);
            TryNextServer();
        }
        break;
    }
    default: return false;
    }
    return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006adae0
void ServerRequestOp::QueueSocketOp(SocketOp* op,DWORD timeout)
{
    mSocket->Fn2B4740(static_cast<SocketOp*>(Track(op,1)),timeout);
}
}

namespace WONAPI {
// Genuine SDK explicitly authored default virtual methods. Native contexts are

// FUNCTION: WON 0x006ad020
WONStatus ServerRequestOp::Virtual10() { return 100059; }
WONStatus ServerRequestOp::Virtual14() { return 0; }
void ServerRequestOp::Virtual18() {}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ad030
// (protected: virtual void * __thiscall WONAPI::ServerRequestOp::`scalar deleting destructor'(unsigned int)    [??_GServerRequestOp@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006ad8e0
// (public: virtual __thiscall WONAPI::CryptTransform::~CryptTransform(void)    [??1CryptTransform@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ad940
// (public: virtual __thiscall WONAPI::MsgTransform::~MsgTransform(void)    [??1MsgTransform@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ae940
// (public: __thiscall std::multimap<unsigned long,struct WONAPI::AsyncOpTrackerEntry *,struct std::less<unsigned long>,class std::allocator<struct std::p)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ad8c0
// (public: virtual void * __thiscall WONAPI::CryptTransform::`scalar deleting destructor'(unsigned int)    [??_GCryptTransform@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ad8a0
// (public: virtual long __thiscall WONAPI::CryptTransform::Recv(class WONAPI::SmartPtr<class WONAPI::ByteBuffer> &)    [?Recv@CryptTransform@WONAPI@@UAEJ)
// TEMPLATE: WON 0x006ad8b0
// (public: virtual long __thiscall WONAPI::CryptTransform::Send(class WONAPI::SmartPtr<class WONAPI::ByteBuffer> &)    [?Send@CryptTransform@WONAPI@@UAEJ)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ad870
// (public: virtual long __thiscall WONAPI::MsgTransform::Send(class WONAPI::SmartPtr<class WONAPI::ByteBuffer> &)    [?Send@MsgTransform@WONAPI@@UAEJAAV?)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ad880
// (public: virtual void * __thiscall WONAPI::MsgTransform::`scalar deleting destructor'(unsigned int))

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f2670
// (public: class std::list<class WONAPI::IPAddr,class std::allocator<class WONAPI::IPAddr> >::iterator __thiscall std::list<class WONAPI::IPAddr,class st)
