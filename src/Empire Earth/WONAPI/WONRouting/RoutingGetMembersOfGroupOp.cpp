// Native group/member list request and response behavior.
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
// FUNCTION: WON 0x6c6320
void RoutingGetMembersOfGroupOp::Virtual0C()
{
    mMembers.clear();
    InitSendMsg(30);
    mSendMsg.AppendShort(mGroupId);
    SendMsg();
    AddOp();
}

// FUNCTION: WON 0x6c63f0
WONStatus RoutingGetMembersOfGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 29) return (WONStatus)100104;
    WONStatus status = inMsg.ReadShort();
    unsigned short groupId = inMsg.ReadShort();
    if (groupId != mGroupId) return (WONStatus)100104;
    if (status == 0) {

        unsigned char flags = inMsg.ReadByte();
        mHasClientNames = (flags & 1) != 0;
        mHasClientFlags = (flags & 2) != 0;
        mObserverCount = inMsg.ReadShort();
        int count = (unsigned short)inMsg.ReadShort();
        for (int i = 0; i < count; ++i) {
            unsigned short length = inMsg.ReadShort();
            long start = inMsg.pos();
            RoutingMemberInfoPtr member = GetNewMemberInfo();
            member->mClientId = inMsg.ReadShort();
            member->mFlags = inMsg.ReadByte(); // actual BYTE+a field gate, not WORD store
            mMembers[member->mClientId] = member;
            if (flags != 0) member->mClientInfo = GetNewClientInfo();
            if (mHasClientNames) inMsg.ReadWString(member->mClientInfo->mName, 2);
            if (mHasClientFlags) member->mClientInfo->mFlags = inMsg.ReadLong();
            inMsg.ReadBytes(length - inMsg.pos() + start);
        }
    }
    return status;
}
}
