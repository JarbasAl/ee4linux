// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK routing firewall request/reply lifecycle.
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONRoutingDetectFirewallOp.h"
namespace WONAPI {
RoutingDetectFirewallOp::RoutingDetectFirewallOp(RoutingConnection *theConnection) :
	RoutingOp(theConnection),
	mListenPort(0),
	mMaxConnectWaitTime(0),
	mDoListen(true),
	mWaitingForStatusReply(false)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingDetectFirewallOp::Virtual08()
{
	RoutingOp::Virtual08();
	if(mListenSocket.get()!=NULL)
	{
		mListenSocket->Fn2B1AD0();
		mListenSocket = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingDetectFirewallOp::Virtual0C()
{
	InitSendMsg(18 /* SDK RoutingDetectFirewallRequest */);
	if(mDoListen)
	{
		mListenSocket = new BlockingSocket(AsyncSocket::TCP);
		WONStatus aStatus = mListenSocket->Fn2B0D90(mListenPort,0);
		if(aStatus!=0)
		{
			Fn29F220(aStatus);
			return;
		}

		aStatus = mListenSocket->Fn2B0FC0(1000);
		if(aStatus!=0)
		{
			Fn29F220(aStatus);
			return;
		}

		mListenPort = mListenSocket->GetLocalPort();
	}


	mWaitingForStatusReply = true;
	mSendMsg.AppendShort(mListenPort);
	mSendMsg.AppendLong(mMaxConnectWaitTime);
	SendMsg();
	AddOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingDetectFirewallOp::Virtual10(unsigned char theMsgType, ReadBuffer &theMsg)
{
	WONStatus aStatus;
	if(mWaitingForStatusReply && theMsgType==66 /* SDK RoutingStatusReply */)
	{
		mWaitingForStatusReply = false;
		aStatus = (WONStatus)theMsg.ReadShort();
		if(aStatus!=0)
			return aStatus;
		else
			return 100105 /* SDK WS_RoutingOp_NeedMoreReplies */;
	}
	else if(theMsgType==19 /* SDK RoutingDetectFirewallResult */)
	{
		aStatus = (WONStatus)theMsg.ReadShort();
		mListenPort = theMsg.ReadShort();
		return aStatus;
	}
	else
		return 100104 /* SDK WS_RoutingOp_DontWantReply */;
}

}
