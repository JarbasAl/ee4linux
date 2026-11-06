// Genuine SDK thread-safe snapshot copies and routing-map traversal.
// SOURCE_ONLY_NO_NATIVE_ID; no Native/primary/physical credit.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
RoutingMemberInfoPtr SmartRoutingConnection::GetMemberRef(unsigned short theClientId, int theGroupId)
{
	AutoCrit aCrit(mDataCrit);
	if(theGroupId==-1)
		theGroupId = mDefaultGroupId;

	if(mHandlerMember.get()!=NULL && mHandlerMember->mClientId==theClientId && mHandlerGroup->mId==theGroupId)
		return mHandlerMember;

	RoutingGroupInfoPtr aGroup = GetGroup(theGroupId);
	if(aGroup.get()==NULL)
		return NULL;

	RoutingMemberMap::iterator anItr = aGroup->mMembers.find(theClientId);
	if(anItr==aGroup->mMembers.end())
		return NULL;
	else
		return anItr->second;
}

static bool CopyClient(SmartRoutingConnection::ClientInfo &theClientCopy, RoutingClientInfo *theClient)
{
	if(theClient==NULL)
		return false;

	theClientCopy.mFlags = theClient->mFlags;
	theClientCopy.mId = theClient->mClientId;
	theClientCopy.mName = theClient->mName;

	SmartRoutingClientInfo* aSmartClient = (SmartRoutingClientInfo*)theClient;
	theClientCopy.mCDKeyId = aSmartClient->mAddress;
	return true;
}

static bool CopyGroup(SmartRoutingConnection::GroupInfo &theGroupCopy, RoutingGroupInfo *theGroup)
{
	if(theGroup==NULL)
		return false;

	theGroupCopy.mCaptainId = theGroup->mCaptainId;
	theGroupCopy.mFlags = theGroup->mFlags;
	theGroupCopy.mAsyncFlags = theGroup->mAsyncFlags;
	theGroupCopy.mId = theGroup->mId;
	theGroupCopy.mMaxPlayers = theGroup->mMaxClients;
	theGroupCopy.mName = theGroup->mName;
	theGroupCopy.mObserverCount = theGroup->mObserverCount;
	return true;
}

static bool CopyMember(SmartRoutingConnection::MemberInfo &theMemberCopy, RoutingMemberInfo *theMember)
{
	if(theMember==NULL)
		return false;
	
	RoutingClientInfo *aClient = theMember->mClientInfo;
	if(aClient!=NULL)
	{
		theMemberCopy.mName = aClient->mName;
		theMemberCopy.mClientFlags = aClient->mFlags;
	}

	theMemberCopy.mClientId = theMember->mClientId;
	theMemberCopy.mMemberFlags = theMember->mFlags;
	return true;
}

bool SmartRoutingConnection::GetClient(unsigned short theClientId, ClientInfo &theClient)
{
	AutoCrit aCrit(mDataCrit);
	RoutingClientInfoPtr aClient = GetClient(theClientId);
	return CopyClient(theClient,aClient);
}

bool SmartRoutingConnection::GetGroup(unsigned short theGroupId, GroupInfo &theGroup)
{
	AutoCrit aCrit(mDataCrit);
	RoutingGroupInfoPtr aGroup = GetGroup(theGroupId);
	return CopyGroup(theGroup,aGroup);
}

bool SmartRoutingConnection::GetMember(unsigned short theGroupId, unsigned short theClientId, MemberInfo &theMember)
{
	AutoCrit aCrit(mDataCrit);
	RoutingMemberInfoPtr aMember = GetMemberRef(theClientId,theGroupId);
	return CopyMember(theMember,aMember);
}

void SmartRoutingConnection::StartClientItr()
{
	AutoCrit aCrit(mDataCrit);
	mClientItr = mClientMap.begin();
}

bool SmartRoutingConnection::GetNextClient(ClientInfo &theInfo)
{
	AutoCrit aCrit(mDataCrit);
	if(mClientItr==mClientMap.end())
		return false;

	CopyClient(theInfo,mClientItr->second);
	++mClientItr;
	return true;
}

void SmartRoutingConnection::StartGroupItr()
{
	AutoCrit aCrit(mDataCrit);
	mGroupItr = mGroupMap.begin();
}

bool SmartRoutingConnection::GetNextGroup(GroupInfo &theInfo)
{
	AutoCrit aCrit(mDataCrit);
	if(mGroupItr==mGroupMap.end())
		return false;

	CopyGroup(theInfo,mGroupItr->second);
	++mGroupItr;
	return true;
}

bool SmartRoutingConnection::StartMemberItr(unsigned short theGroupId)
{
	AutoCrit aCrit(mDataCrit);
	mMemberItrGroup = GetGroup(theGroupId);

	if(mMemberItrGroup.get()==NULL)
		return false;

	mMemberItr = mMemberItrGroup->mMembers.begin();
	return true;
}

bool SmartRoutingConnection::GetNextMember(MemberInfo &theMember)
{
	AutoCrit aCrit(mDataCrit);
	if(mMemberItrGroup.get()==NULL)
		return false;

	if(mMemberItr==mMemberItrGroup->mMembers.end())
		return false;

	CopyMember(theMember,mMemberItr->second);
	++mMemberItr;
	return true;
}

bool SmartRoutingConnection::GetLastTargetMember(MemberInfo &theMember)
{
	AutoCrit aCrit(mDataCrit);
	return CopyMember(theMember,mLastTargetMember);
}

bool SmartRoutingConnection::GetLastTargetClient(ClientInfo &theClient)
{
	AutoCrit aCrit(mDataCrit);
	return CopyClient(theClient, mLastTargetClient);
}
}
