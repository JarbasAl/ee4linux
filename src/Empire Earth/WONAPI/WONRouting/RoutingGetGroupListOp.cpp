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
// FUNCTION: WON 0x6c5980
void RoutingGetGroupListOp::Virtual0C()
{
    mGroupMap.clear();
    InitSendMsg(26);
    mSendMsg.AppendShort(mFlags);
    mSendMsg.AppendShort(mParentGroupId);
    SendMsg();
    AddOp();
}

// FUNCTION: WON 0x6c5bd0
WONStatus RoutingGetGroupListOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 25) return (WONStatus)100104;
    WONStatus status = inMsg.ReadShort();
    if (status == 0) ParseReplyExceptForStatus(inMsg);
    return status;
}

// FUNCTION: WON 0x6c5c10
void RoutingGetGroupListOp::ParseReplyExceptForStatus(ReadBuffer& inMsg)
{
    mGroupMap.clear();
    mFlags = inMsg.ReadShort();
    int count = (unsigned short)inMsg.ReadShort();
    for (int i = 0; i < count; ++i) {
        unsigned short length = inMsg.ReadShort();
        long start = inMsg.pos();
        RoutingGroupInfoPtr group = GetNewGroupInfo();
        group->mId = inMsg.ReadShort();
        if (IncludeName()) inMsg.ReadWString(group->mName, 2);
        if (IncludeCaptain()) group->mCaptainId = inMsg.ReadShort();
        if (IncludeMaxPlayers()) group->mMaxClients = inMsg.ReadShort();
        if (IncludeFlags()) group->mFlags = inMsg.ReadLong();
        if (IncludeAsyncFlags()) group->mAsyncFlags = inMsg.ReadLong();
        if (IncludeObserverCount()) group->mObserverCount = inMsg.ReadShort();
        if (IncludeMemberCount()) group->mMaxPlayers = inMsg.ReadShort(); // canonical2a spelling, actual membercount
        if (IncludeMembers()) {
            for (int j = 0; j < group->mMaxPlayers; ++j) {
                RoutingMemberInfoPtr member = GetNewMemberInfo();
                member->mClientId = inMsg.ReadShort();
                member->mFlags = inMsg.ReadByte(); // BYTE+a type gate
                group->mMembers[member->mClientId] = member;
            }
        }
        mGroupMap[group->mId] = group;
        inMsg.ReadBytes(length - inMsg.pos() + start);
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x006c5a60
RoutingMemberInfoPtr RoutingGetGroupListOp::GetNewMemberInfo()
{
    if(mConnection.get()!=NULL) return RoutingOp::GetNewMemberInfo();
    return new RoutingMemberInfo;
}
// FUNCTION: WON 0x006c5ad0
RoutingGroupInfoPtr RoutingGetGroupListOp::GetNewGroupInfo()
{
    if(mConnection.get()!=NULL) return RoutingOp::GetNewGroupInfo();
    return new RoutingGroupInfo;
}
// FUNCTION: WON 0x006c5f70
bool RoutingGetGroupListOp::ParseGroupListObj(const void* data,unsigned long size)
{
    try {
        ReadBuffer msg(data,size);
        ParseReplyExceptForStatus(msg);
        return true;
    } catch(ReadBufferException&) {}
    return false;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c8f50
// (public: struct std::pair<class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingMemberInf)
