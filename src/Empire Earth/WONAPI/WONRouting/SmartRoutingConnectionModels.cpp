// Native smart routing map access and notification model updates.
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
// FUNCTION: WON 0x006c0330
RoutingClientInfoPtr SmartRoutingConnection::GetClient(unsigned short id)
{
    AutoCrit crit(mDataCrit);
    if(mHandlerClient.get()!=NULL && mHandlerClient->mClientId==id)
        return mHandlerClient;
    RoutingClientMap::iterator i=mClientMap.find(id);
    if(i==mClientMap.end()) return NULL;
    return i->second;
}
// FUNCTION: WON 0x006c0450
RoutingGroupInfoPtr SmartRoutingConnection::GetGroup(unsigned short id)
{
    AutoCrit crit(mDataCrit);
    if(mHandlerGroup.get()!=NULL && mHandlerGroup->mId==id)
        return mHandlerGroup;
    RoutingGroupMap::iterator i=mGroupMap.find(id);
    if(i==mGroupMap.end()) return NULL;
    return i->second;
}
// FUNCTION: WON 0x006c0a10
void SmartRoutingConnection::InitHandlerVariables(RoutingClientInfo* client)
{
    mHandlerClient=client;
    mHandlerGroup=NULL;
    mHandlerMember=NULL;
}
// FUNCTION: WON 0x006c0ac0
void SmartRoutingConnection::InitHandlerVariables(RoutingGroupInfo* group,RoutingMemberInfo* member)
{
    mHandlerGroup=group;
    mHandlerMember=member;
    if(member!=NULL) mHandlerClient=member->mClientInfo;
    else mHandlerClient=NULL;
}
// FUNCTION: WON 0x006c24f0
void SmartRoutingConnection::HandleClientFlagsChanged(RoutingClientFlagsChangedOp* op)
{
    int anId=op->GetClientId();
    RoutingClientInfoPtr client=GetClient(anId);
    if(client.get()==NULL) return;
    client->mFlags=op->GetClientFlags();
    InitHandlerVariables(client.get());
}
// FUNCTION: WON 0x6c2860
void SmartRoutingConnection::HandleGroupCaptainChanged(RoutingGroupCaptainChangedOp* op)
{
    int anId=op->GetGroupId();
    RoutingGroupInfoPtr group=GetGroup(anId);
    if(group.get()==NULL) return;
    group->mCaptainId=op->GetCaptainId();
    InitHandlerVariables(group.get());
}
// FUNCTION: WON 0x6c2900
void SmartRoutingConnection::HandleGroupMaxPlayersChanged(RoutingGroupMaxPlayersChangedOp* op)
{
    int anId=op->GetGroupId();
    RoutingGroupInfoPtr group=GetGroup(anId);
    if(group.get()==NULL) return;
    group->mMaxClients=op->GetMaxPlayers();
    InitHandlerVariables(group.get());
}
// FUNCTION: WON 0x6c2bb0
void SmartRoutingConnection::HandleGroupFlagsChanged(RoutingGroupFlagsChangedOp* op)
{
    int anId=op->GetGroupId();
    RoutingGroupInfoPtr group=GetGroup(anId);
    if(group.get()==NULL) return;
    group->mFlags=op->GetGroupFlags();
    InitHandlerVariables(group.get());
}
// FUNCTION: WON 0x6c2c50
void SmartRoutingConnection::HandleGroupNameChanged(RoutingGroupNameChangedOp* op)
{
    int anId=op->GetGroupId();
    RoutingGroupInfoPtr group=GetGroup(anId);
    if(group.get()==NULL) return;
    group->mName=op->mName;
    InitHandlerVariables(group.get());
}
// FUNCTION: WON 0x6c2d00
void SmartRoutingConnection::HandleGroupObserverCount(RoutingGroupObserverCountOp* op)
{
    int anId=op->GetGroupId();
    RoutingGroupInfoPtr group=GetGroup(anId);
    if(group.get()==NULL) return;
    group->mObserverCount=op->GetObserverCount();
    InitHandlerVariables(group.get());
}
// FUNCTION: WON 0x006c2da0
void SmartRoutingConnection::HandleMemberFlagsChanged(RoutingMemberFlagsChangedOp* op)
{
    int anId=op->GetGroupId();
    RoutingGroupInfoPtr group=GetGroup(anId);
    if(group.get()==NULL) return;
    int aClientId=op->GetClientId();
    RoutingMemberMap::iterator i=group->mMembers.find(aClientId);
    if(i==group->mMembers.end()) return;
    i->second->mFlags=op->GetMemberFlags();
    InitHandlerVariables(group.get(),i->second.get());
}
}
