// SOURCE_ONLY_NO_NATIVE_ID: complete genuine same-SDK new derived operation.
#ifndef EE_WONAPI_WONROUTINGDETECTFIREWALLOP_H
#define EE_WONAPI_WONROUTINGDETECTFIREWALLOP_H
#include "ee/wonapi/WONRouting.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingDetectFirewallOp : public RoutingOp
{
private:
	unsigned short mListenPort;
	unsigned long mMaxConnectWaitTime;
	SmartPtr<BlockingSocket> mListenSocket;
	bool mDoListen;
	bool mWaitingForStatusReply;

	virtual void Virtual0C();
	virtual WONStatus Virtual10(unsigned char theMsgType, ReadBuffer &theMsg);
	virtual void Virtual08();

public:
	RoutingDetectFirewallOp(RoutingConnection *theConnection);

	void SetListenPort(unsigned short thePort) { mListenPort = thePort; }
	void SetMaxConnectWaitTime(unsigned long theNumSeconds) { mMaxConnectWaitTime = theNumSeconds; }
	void SetDoListen(bool doListen) { mDoListen = doListen; }

	unsigned short GetListenPort() const { return mListenPort; }
	unsigned long GetMaxConnectWaitTime() const { return mMaxConnectWaitTime; }
	bool GetDoListen() { return mDoListen; }

	virtual RoutingOpType GetType() const { return (RoutingOpType)17; // SDK RoutingOp_DetectFirewall
 }
};


typedef SmartPtr<RoutingDetectFirewallOp> RoutingDetectFirewallOpPtr;


}; // namespace WONAPI



#endif
