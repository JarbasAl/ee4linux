// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK routing group administration.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONRoutingGroupAdmin.h"
namespace WONAPI {
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingSetGroupMaxPlayersOp::Virtual0C()
{
	InitSendMsg(62 /* SDK RoutingSetGroupMaxPlayersRequest */);
	mSendMsg.AppendShort(mGroupId);
	mSendMsg.AppendShort(mMaxPlayers);
	SendMsg();
	AddOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingSetGroupMaxPlayersOp::Virtual10(unsigned char theMsgType, ReadBuffer &theMsg)
{
	if(theMsgType!=66 /* SDK RoutingStatusReply */)
		return 100104 /* SDK WS_RoutingOp_DontWantReply */;

	WONStatus aStatus = (WONStatus)theMsg.ReadShort();
	return aStatus;
}


}
