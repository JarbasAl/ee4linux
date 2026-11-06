// Native routing behavior; source-only reconstruction.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x6c56d0
void RoutingGetClientListOp::Virtual0C()
{
    mClientMap.clear();
    InitSendMsg(24);
    if (mIsEx) mSendMsg.AppendBool(true);
    SendMsg();
    AddOp();
}

// FUNCTION: WON 0x6c57a0
WONStatus RoutingGetClientListOp::Virtual10(unsigned char inMsgType,ReadBuffer& inMsg)
{
    if (inMsgType!=23 && inMsgType!=74) return (WONStatus)100104;
    mIsEx=inMsgType==74;
    WONStatus status=inMsg.ReadShort();
    if (status!=0) return status;
    int count=(unsigned short)inMsg.ReadShort();
    for (int i=0;i<count;++i) {
        RoutingClientInfoPtr client=GetNewClientInfo();
        client->mClientId=inMsg.ReadShort();
        inMsg.ReadWString(client->mName,2);
        client->mFlags=inMsg.ReadLong();
        if (mIsEx) RoutingClientJoinedServerOp::ParseExClientData(inMsg,client.get());
        mClientMap[client->mClientId]=client;
    }
    return status;
}
}
