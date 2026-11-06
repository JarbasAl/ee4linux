// Native DetectFirewall operation behavior. Source-only; no runtime substitution.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006cf5c0
void DetectFirewallOp::Init()
{
    mLengthFieldSize=2;
    mListenPort=0;
    mMaxConnectWaitTime=0;
    mDoListen=true;
    mUseUDP=false;
    mWaitForConnect=true;
    mMaxRecvTime=0xffffffff;
}
// FUNCTION: WON 0x006cf600
DetectFirewallOp::DetectFirewallOp(ServerContext* inContext)
    : ServerRequestOp(inContext)
{
    Init();
}
// FUNCTION: WON 0x006cf760
void DetectFirewallOp::Virtual04()
{
    if (mDoListen || mUseUDP) {
        mListenSocket=new BlockingSocket(mUseUDP?1:0);
        WONStatus status=mListenSocket->Fn2B0D90(mListenPort,0);
        if (status!=0) {
            Fn29F220(status);
            return;
        }
        if (!mUseUDP) {
            status=mListenSocket->Fn2B0FC0(1000);
            if (status!=0) {
                Fn29F220(status);
                return;
            }
        }
        mListenPort=mListenSocket->GetLocalPort();
    }
    ServerRequestOp::Virtual04();
}
// FUNCTION: WON 0x006cf8b0
bool DetectFirewallOp::CallbackHook(AsyncOp* inOp,int inParam)
{
    if (inParam!=1000) return ServerRequestOp::CallbackHook(inOp,inParam);
    if (inOp->mStatus==0 &&
        mRecvBytesFromOp->mFromAddress.Fn2B2FB0()!=GetCurAddr().Fn2B2FB0()) {
        Virtual1C();
        return true;
    }
    Fn29F220(inOp->mStatus);
    return true;
}
// FUNCTION: WON 0x006cf920
void DetectFirewallOp::Virtual08()
{
    ServerRequestOp::Virtual08();
    if (mListenSocket.get()!=NULL) {
        mListenSocket->Fn2B1AD0();
        mListenSocket=NULL;
    }
}
// FUNCTION: WON 0x006cf970
WONStatus DetectFirewallOp::Virtual10()
{
    WriteBuffer request(mLengthFieldSize,true);
    request.AppendByte(5);
    request.AppendShort(3);
    request.AppendShort(mUseUDP?3:1);
    request.AppendShort(mListenPort);
    if (!mUseUDP) {
        request.AppendBool(mWaitForConnect);
        request.AppendLong(mMaxConnectWaitTime);
        request.AppendBool(false);
    }
    mRequest=request.ToByteBuffer(true);
    return 100059;
}
// FUNCTION: WON 0x006cfad0
WONStatus DetectFirewallOp::Virtual14()
{
    ReadBuffer reply(mResponse->data(),mResponse->length());
    unsigned char header=reply.ReadByte();
    unsigned short service=reply.ReadShort();
    unsigned short message=reply.ReadShort();
    if (header!=5 || service!=3 || message!=2) return InvalidReplyHeader();
    short status=reply.ReadShort();
    if (reply.mL08-reply.mL00>=4) {
        long host=reply.ReadLong();
        if (host!=0) mPublicAddr.Fn2B2C20(host,0);
    }
    if (mUseUDP) return Virtual1C();
    return status;
}
// FUNCTION: WON 0x006cfba0
WONStatus DetectFirewallOp::Virtual1C()
{
    if (mRunMode==1) {
        mRecvBytesFromOp=new RecvBytesFromOp(255,mListenSocket.get());
        unsigned long wait=TimeLeft();
        if (mMaxConnectWaitTime>0 && mMaxConnectWaitTime<wait)
            wait=mMaxConnectWaitTime;
        Track(mRecvBytesFromOp.get(),1000);
        if (wait==0) wait=0xffffffff;
        mRecvBytesFromOp->Run(wait);
        return 100060;
    }
    do {
        mRecvBytesFromOp=new RecvBytesFromOp(255,mListenSocket.get());
        unsigned long wait=TimeLeft();
        if (mMaxConnectWaitTime>0 && mMaxConnectWaitTime<wait)
            wait=mMaxConnectWaitTime;
        WONStatus status=mRecvBytesFromOp->Fn29F500(2,wait);
        if (status!=0) return status;
    } while (mRecvBytesFromOp->mFromAddress.Fn2B2FB0()!=GetCurAddr().Fn2B2FB0());
    return 0;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cf690
// (public: virtual __thiscall WONAPI::DetectFirewallOp::~DetectFirewallOp(void)    [??1DetectFirewallOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cf670
// (public: virtual void * __thiscall WONAPI::DetectFirewallOp::`scalar deleting destructor'(unsigned int)    [??_GDetectFirewallOp@WONAPI@@UAEPAXI@Z])
