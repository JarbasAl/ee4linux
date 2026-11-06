// Routing-server unsolicited wire parsers, original-backed reconstruction.
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

// FUNCTION: WON 0x6c4580
WONStatus RoutingClientFlagsChangedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 6) return (WONStatus)100104;
    mClientId = inMsg.ReadShort();
    mClientFlags = inMsg.ReadLong();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c45c0
WONStatus RoutingClientJoinedGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 7) return (WONStatus)100104;
    unsigned char flags = inMsg.ReadByte();
    mGroupId = inMsg.ReadShort();
    mClientId = inMsg.ReadShort();
    mMemberFlags = inMsg.ReadByte();
    if (flags & 1) inMsg.ReadWString(mClientName, 2);
    if (flags & 2) {
        mClientFlags = inMsg.ReadLong();
        mHasClientFlags = true;
    }
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c4800
WONStatus RoutingClientLeftGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 9) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mClientId = inMsg.ReadShort();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c4840
WONStatus RoutingClientLeftServerOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 10) return (WONStatus)100104;
    mClientId = inMsg.ReadShort();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c4f30
WONStatus RoutingDataObjectDeletedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 15) return (WONStatus)100104;
    mLinkId = inMsg.ReadShort();
    inMsg.ReadString(mDataType, 1);
    inMsg.ReadWString(mName, 1);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6c10
WONStatus RoutingGroupCaptainChangedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 31) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mCaptainId = inMsg.ReadShort();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6c50
WONStatus RoutingGroupCreatedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 32) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    inMsg.ReadWString(mName, 2);
    mMaxClients = inMsg.ReadShort();
    mFlags = inMsg.ReadLong();
    mAsyncFlags = inMsg.ReadLong();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6da0
WONStatus RoutingGroupDeletedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 33) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6dd0
WONStatus RoutingGroupFlagsChangedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 34) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mGroupFlags = inMsg.ReadLong();
    mAsyncFlags = inMsg.ReadLong();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6e10
WONStatus RoutingGroupInvitationOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 35) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mClientId = inMsg.ReadShort();
    mInvited = inMsg.ReadBool();
    inMsg.ReadWString(mComment, 2);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6e60
WONStatus RoutingGroupJoinAttemptOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 36) return (WONStatus)100104;
    mClientId = inMsg.ReadShort();
    mGroupId = inMsg.ReadShort();
    inMsg.ReadWString(mComment, 2);
    mFlags = inMsg.ReadByte();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6ef0
WONStatus RoutingGroupMaxPlayersChangedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 37) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mMaxPlayers = inMsg.ReadShort();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6f30
WONStatus RoutingGroupNameChangedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 38) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    inMsg.ReadWString(mName, 2);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6f70
WONStatus RoutingGroupObserverCountOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 39) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mObserverCount = inMsg.ReadShort();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c7650
WONStatus RoutingMemberFlagsChangedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 44) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mClientId = inMsg.ReadShort();
    mMemberFlags = inMsg.ReadByte();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c7d40
WONStatus RoutingPeerChatOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 47) return (WONStatus)100104;
    mClientId = inMsg.ReadShort();
    mFlags = inMsg.ReadShort();
    inMsg.ReadWString(mText, 2);
    int count = (unsigned short)inMsg.ReadShort();
    for (int i = 0; i < count; ++i) {
        unsigned short recipient = inMsg.ReadShort();
        mRecipients.push_back(recipient);
    }
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c8b80
WONStatus RoutingServerAlertOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 57) return (WONStatus)100104;
    inMsg.ReadWString(mText, 2);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c8bb0
WONStatus RoutingServerShutdownAbortedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 58) return (WONStatus)100104;
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c8bd0
WONStatus RoutingServerShutdownStartedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 59) return (WONStatus)100104;
    inMsg.ReadWString(mText, 2);
    mSeconds = inMsg.ReadLong();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c8eb0
WONStatus RoutingYouWereBannedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 69) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mIsBanned = inMsg.ReadBool();
    inMsg.ReadWString(mComment, 2);
    mDuration = inMsg.ReadLong();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c8f00
WONStatus RoutingYouWereMutedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 70) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mIsMuted = inMsg.ReadBool();
    inMsg.ReadWString(mComment, 2);
    mDuration = inMsg.ReadLong();
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c6eb0
WONStatus RoutingGroupMemberCountOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 71) return (WONStatus)100104;
    mGroupId = inMsg.ReadShort();
    mMemberCount = inMsg.ReadShort();
    return (WONStatus)0;
}
}

namespace WONAPI {
// FUNCTION: WON 0x6c4e40
WONStatus RoutingDataObjectCreatedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 14) return (WONStatus)100104;
    mLinkId = inMsg.ReadShort();
    inMsg.ReadString(mDataType, 1);
    inMsg.ReadWString(mName, 1);
    mData = inMsg.ReadBuf(2);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c4f80
WONStatus RoutingDataObjectModifiedOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 16) return (WONStatus)100104;
    mLinkId = inMsg.ReadShort();
    inMsg.ReadString(mDataType, 1);
    inMsg.ReadWString(mName, 1);
    mOffset = inMsg.ReadShort();
    mIsInsert = inMsg.ReadBool();
    mData = inMsg.ReadBuf(2);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c7e00
WONStatus RoutingPeerDataOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 48) return (WONStatus)100104;
    mClientId = inMsg.ReadShort();
    mData = inMsg.ReadBuf(2);
    return (WONStatus)0;
}

// FUNCTION: WON 0x6c4700
WONStatus RoutingClientJoinedServerOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 8) return (WONStatus)100104;
    mClientInfo = GetNewClientInfo();
    mClientInfo->mClientId = inMsg.ReadShort();
    inMsg.ReadWString(mClientInfo->mName, 2);
    mClientInfo->mFlags = inMsg.ReadLong();
    if (inMsg.mL08 - inMsg.mL00 > 0)
        ParseExClientData(inMsg, mClientInfo.get());
    return (WONStatus)0;
}

// FUNCTION: WON 0x006c4680
void RoutingClientJoinedServerOp::ParseExClientData(ReadBuffer& inMsg, RoutingClientInfo* inClient)
{
    unsigned char count = inMsg.ReadByte();
    for (int i = 0; i < count; ++i) {
        unsigned char kind = inMsg.ReadByte();
        unsigned char length = inMsg.ReadByte();
        switch (kind) {
        case 1: inClient->mUserId = inMsg.ReadLong(); break;
        case 2: inClient->mAddress = inMsg.ReadLong(); break;
        default: inMsg.ReadBytes(length); break;
        }
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x006c6cb0
RoutingGroupInfoPtr RoutingGroupCreatedOp::GetGroupInfo()
{
    RoutingGroupInfoPtr group=GetNewGroupInfo();
    group->mId=mGroupId;
    group->mName=mName;
    group->mMaxClients=mMaxClients;
    group->mFlags=mFlags;
    group->mAsyncFlags=mAsyncFlags;
    return group;
}
}
