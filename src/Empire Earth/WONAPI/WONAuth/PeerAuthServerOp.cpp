// Original peer authentication server operation on its complete actual owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAuthPeerData.h"

namespace WONAPI {
// FUNCTION: WON 0x006e9050
void PeerAuthServerOp::Virtual04()
{
    mLengthFieldSize=mSocket->GetLengthFieldSize();
    mPeerAuthServer.Start(mAuthContext->GetPeerData(),mLengthFieldSize);
    if(mRunMode==1)
    {
        AsyncRun();
        return;
    }

    ByteBufferPtr recvMsg;
    WONStatus status=mSocket->RecvMsg(recvMsg,RecvTime());
    if(CheckStatus(status))
        return;

    ByteBufferPtr sendMsg;
    status=mPeerAuthServer.HandleRecvMsg(recvMsg->data(),recvMsg->length(),sendMsg);
    CheckStatus(status);
    if(sendMsg.get()==NULL)
        return;

    status=mSocket->SendMsg(sendMsg.get(),SendTime());
    CheckStatus(status);
    if(mStatus!=100000) // WS_Pending
        return;

    status=mSocket->RecvMsg(recvMsg,RecvTime());
    if(CheckStatus(status))
        return;

    status=mPeerAuthServer.HandleRecvMsg(recvMsg->data(),recvMsg->length(),sendMsg);
    CheckStatus(status);
    if(sendMsg.get()==NULL)
        return;

    status=mSocket->SendMsg(sendMsg.get(),SendTime());
    if(status==0)
        Success();
    else
        Fn29F220(status);
}

// FUNCTION: WON 0x006e8950
PeerAuthServerOp::PeerAuthServerOp(AuthContext* context,BlockingSocket* socket)
{
    mAuthContext=context;
    mSocket=socket;
}

// FUNCTION: WON 0x006e8bc0
PeerAuthServerOp::~PeerAuthServerOp()
{
}

// FUNCTION: WON 0x006e8d70
bool PeerAuthServerOp::CallbackHook(AsyncOp* op,int type)
{
    if(op->mStatus!=0)
    {
        Fn29F220(op->mStatus);
        return true;
    }
    if(type==1) // PeerAuth_Track_Recv
    {
        RecvMsgOp* recvOp=dynamic_cast<RecvMsgOp*>(op);
        if(recvOp==NULL)
            return true;

        ByteBufferPtr sendMsg;
        ByteBufferPtr recvMsg=recvOp->GetMsg();
        WONStatus status=mPeerAuthServer.HandleRecvMsg(recvMsg->data(),recvMsg->length(),sendMsg);
        int sendType=0;
        if(status==0 && mPeerAuthServer.GetState()==PeerAuthServer::STATE_NOT_STARTED)
            sendType=2; // PeerAuth_Track_LastSend
        if(sendMsg.get()!=NULL)
            mSocket->Fn2B4740(static_cast<SocketOp*>(Track(new SendMsgOp(sendMsg.get()),sendType)),SendTime());
        if(status!=0)
            Fn29F220(status);
        else if(sendType!=2)
            mSocket->Fn2B4740(static_cast<SocketOp*>(Track(new RecvMsgOp,1)),RecvTime());
    }
    else if(type==2)
        Success();
    return true;
}

// FUNCTION: WON 0x006e8fa0
bool PeerAuthServerOp::CheckStatus(WONStatus status)
{
    if(status==0)
        return false;
    Fn29F220(status);
    return true;
}

// FUNCTION: WON 0x006e8fc0
void PeerAuthServerOp::AsyncRun()
{
    mSocket->Fn2B4740(static_cast<SocketOp*>(Track(new RecvMsgOp,1)),RecvTime());
}

// FUNCTION: WON 0x006e9370
void PeerAuthServerOp::Success()
{
    AuthSession* session=mPeerAuthServer.GetSession();
    AuthType type=session->GetAuthType();
    if(type==AUTH_TYPE_SESSION || type==AUTH_TYPE_PERSISTENT)
        mSocket->Fn2B0620(new CryptTransform(session));
    Fn29F220(0);
}

// SUPPORT: EXE 0x006e7cb0 (5B genuine shared base cleanup forwarder, zero primary quota)
void PeerAuthServerOp::Virtual08()
{
    AsyncOpWithTracker::Virtual08();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006e8a40
// (protected: virtual void * __thiscall WONAPI::PeerAuthServerOp::`scalar deleting destructor'(unsigned int)    [??_GPeerAuthServerOp@WONAPI@@MAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006e8a60
// (public: __thiscall WONAPI::PeerAuthServer::~PeerAuthServer(void)    [??1PeerAuthServer@WONAPI@@QAE@XZ])
