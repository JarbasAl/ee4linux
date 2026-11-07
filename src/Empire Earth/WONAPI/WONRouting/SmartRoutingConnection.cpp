// Native routing behavior; source-only reconstruction.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <wctype.h>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x006bfb00
SmartRoutingConnection::SmartRoutingConnection()
    : mClientItr(NULL), mGroupItr(NULL), mMemberItr(NULL)
{
    mDefaultGroupId = 0;
    mClientId = 0;
    mLastInputError = InputError_None;
    mIgnoreFilePath = WONAPICore::GetDefaultFileDirectory() + "_wonignore.bin";
    mDoColonEmote = true;
    mReadClientCDKeyId = false;
    AddDefaultUserCommandStrings();
    AddDefaultModeratorCommandStrings();
    SetAllowPartialNameMatch(true);
    SetServerWideCommands(false);
}
// FUNCTION: WON 0x006bfe60
RoutingClientInfoPtr SmartRoutingConnection::Virtual1C()
{
    return new SmartRoutingClientInfo;
}
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
// FUNCTION: WON 0x6c0570
void SmartRoutingConnection::EraseClient(const RoutingClientMap::iterator &theItr)
{
	if(theItr==mClientItr)
		++mClientItr;

	mClientMap.erase(theItr);
}
// FUNCTION: WON 0x6c0600
void SmartRoutingConnection::EraseClient(unsigned short theId)
{
	RoutingClientMap::iterator anItr = mClientMap.find(theId);
	if(anItr!=mClientMap.end())
		EraseClient(anItr);
}
// FUNCTION: WON 0x6c0670
void SmartRoutingConnection::EraseGroup(const RoutingGroupMap::iterator &theItr)
{
	if(theItr->second.get()==mMemberItrGroup.get())
		mMemberItrGroup = NULL;

	if(theItr==mGroupItr)
		++mGroupItr;

	mGroupMap.erase(theItr);
}
// FUNCTION: WON 0x6c0740
void SmartRoutingConnection::EraseMember(RoutingMemberMap &theMap, const RoutingMemberMap::iterator &theItr)
{
	if(theItr==mMemberItr)
		++mMemberItr;

	theMap.erase(theItr);
}
// FUNCTION: WON 0x6c07d0
void SmartRoutingConnection::SetClientName(RoutingClientInfo *theClient, const std::wstring &theName)
{
	SmartRoutingClientInfo *aClient = (SmartRoutingClientInfo*)theClient;
	aClient->mName = theName;
	aClient->mIgnored = mIgnoreSet.find(theName)!=mIgnoreSet.end();
}
// FUNCTION: WON 0x6c0860
SmartRoutingClientInfo* SmartRoutingConnection::EnsureInClientMap(unsigned short theClientId)
{
	RoutingClientMap::iterator anItr = mClientMap.insert(RoutingClientMap::value_type(theClientId, NULL)).first;
	if(anItr->second.get()==NULL)
	{
		anItr->second = Virtual1C();
		anItr->second->mClientId = theClientId;
	}

	return (SmartRoutingClientInfo*)anItr->second.get();
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
// FUNCTION: WON 0x6c0be0
void SmartRoutingConnection::Virtual2C(RoutingOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	mHandlerClient = NULL;
	mHandlerGroup = NULL;
	mHandlerMember = NULL;

	switch(theOp->GetType())
	{
		case 6:		HandleClientFlagsChanged	((RoutingClientFlagsChangedOp*)	theOp);	break;
		case 7:		HandleClientJoinedGroup		((RoutingClientJoinedGroupOp*)	theOp);	break;
		case 8:		HandleClientJoinedServer	((RoutingClientJoinedServerOp*)	theOp);	break;
		case 9:			HandleClientLeftGroup		((RoutingClientLeftGroupOp*)	theOp);	break;
		case 10:		HandleClientLeftServer		((RoutingClientLeftServerOp*)	theOp);	break;

		case 20:			HandleGetClientList			((RoutingGetClientListOp*)		theOp); break;
		case 21:			HandleGetGroupList			((RoutingGetGroupListOp*)		theOp); break;
		case 22:		HandleGetMembersOfGroup		((RoutingGetMembersOfGroupOp*)	theOp); break;

		case 23:		HandleGroupCaptainChanged	((RoutingGroupCaptainChangedOp*)	theOp); break;
		case 24:			HandleGroupCreated			((RoutingGroupCreatedOp*)			theOp); break;
		case 25:			HandleGroupDeleted			((RoutingGroupDeletedOp*)			theOp); break;
		case 26:		HandleGroupFlagsChanged		((RoutingGroupFlagsChangedOp*)		theOp); break;
		case 29:	HandleGroupMaxPlayersChanged((RoutingGroupMaxPlayersChangedOp*)	theOp); break;
		case 31:		HandleGroupNameChanged		((RoutingGroupNameChangedOp*)		theOp); break;
		case 32:		HandleGroupObserverCount	((RoutingGroupObserverCountOp*)		theOp); break;

		case 34:				HandleJoinGroup				((RoutingJoinGroupOp*)			theOp); break;
		case 35:				HandleLeaveGroup			((RoutingLeaveGroupOp*)			theOp); break;
		case 36:		HandleMemberFlagsChanged	((RoutingMemberFlagsChangedOp*)	theOp); break;
		case 42:			HandleRegisterClient		((RoutingRegisterClientOp*)		theOp); break;
		case 51:			HandleSetClientFlags		((RoutingSetClientFlagsOp*)		theOp); break;

		case 41:			HandleReadDataObject		((RoutingReadDataObjectOp*)			theOp); break;
		case 13:		HandleDataObjectCreated		((RoutingDataObjectCreatedOp*)		theOp); break;
		case 15:		HandleDataObjectModified	((RoutingDataObjectModifiedOp*)		theOp); break;
		case 14:		HandleDataObjectDeleted		((RoutingDataObjectDeletedOp*)		theOp); break;



		// For ignoring
		case 27:			HandleGroupInvitation		((RoutingGroupInvitationOp*)	theOp); break;
		case 28:		HandleGroupJoinAttempt		((RoutingGroupJoinAttemptOp*)	theOp); break;
		case 39:				HandlePeerChat				((RoutingPeerChatOp*)			theOp); break;

	}	
}
// FUNCTION: WON 0x6c0ec0
void SmartRoutingConnection::HandleRegisterClient(RoutingRegisterClientOp *theOp)
{
	if(!(theOp->mStatus==0))
		return;

	LoadIgnoreSet();

	mClientId = theOp->GetClientId();
	mAsyncFlags = theOp->GetAsyncMessageFlags();

	mGroupMap.clear();
	mClientMap = theOp->GetClientMap();
	if(mClientMap.empty()) // didn't get client list (insert myself)
	{
		mMyClient = Virtual1C();
		mMyClient->mName = theOp->GetClientName();
		mMyClient->mClientId = theOp->GetClientId();
		mMyClient->mFlags = theOp->GetClientFlags();
		mClientMap[mMyClient->mClientId] = mMyClient;
	}
	else
		mMyClient = GetClient(mClientId);

	SyncClientMapWithIgnoreList();
}
// FUNCTION: WON 0x6c1100
void SmartRoutingConnection::HandleSetClientFlags(RoutingSetClientFlagsOp *theOp)
{
	if(!(theOp->mStatus==0))
		return;

	unsigned long aMask = theOp->mAsyncMessageFlagMask;
	unsigned long aNewFlags = theOp->mAsyncMessageFlags;

	aNewFlags &= aMask;
	mAsyncFlags &= ~aMask;
	mAsyncFlags |= aNewFlags;
}
// FUNCTION: WON 0x6c1130
void SmartRoutingConnection::HandleGetClientList(RoutingGetClientListOp *theOp)
{
	const RoutingClientMap &aMap = theOp->mClientMap;
	RoutingClientMap::const_iterator aNewItr = aMap.begin();
	RoutingClientMap::iterator anOldItr = mClientMap.begin();

	while(anOldItr!=mClientMap.end() && aNewItr!=aMap.end())
	{
		if(anOldItr->first < aNewItr->first)
			EraseClient(anOldItr++);
		else 
		{
			if(aNewItr->first < anOldItr->first)
				anOldItr = mClientMap.insert(anOldItr, *aNewItr);
			else
			{
				anOldItr->second->mName = aNewItr->second->mName;
				anOldItr->second->mFlags = aNewItr->second->mFlags;
			}
			
			anOldItr++;
			aNewItr++;
		}
	}

	while(anOldItr!=mClientMap.end())
		EraseClient(anOldItr++);

	while(aNewItr!=aMap.end())	
	{
		anOldItr = mClientMap.insert(anOldItr, *aNewItr);
		aNewItr++;
	}


	mMyClient = GetClient(mClientId);
	SyncClientMapWithIgnoreList();
}
// FUNCTION: WON 0x6c1400
void SmartRoutingConnection::HandleReadDataObject(RoutingReadDataObjectOp *theOp)
{
	// return if not parsing client CDKey ID's
	if (theOp->mDataType != "_KeyId")
		return;

	const RoutingReadDataObjectOp* aOp = theOp;
	RoutingDataObjectList::const_iterator anItr = aOp->mDataObjects.begin();
	for (; anItr != aOp->mDataObjects.end(); ++anItr)
		SetClientCDKeyIdFromDataObject((*anItr)->mLinkId, (*anItr)->mData);
}
// FUNCTION: WON 0x6c14e0
void SmartRoutingConnection::HandleDataObjectCreated(RoutingDataObjectCreatedOp *theOp)
{
	if (theOp->mDataType != "_KeyId")
		return;

	SetClientCDKeyIdFromDataObject(theOp->GetLinkId(), theOp->GetData());
}
// FUNCTION: WON 0x6c1560
void SmartRoutingConnection::HandleDataObjectModified(RoutingDataObjectModifiedOp *theOp)
{
	if (theOp->mDataType != "_KeyId")
		return;

	SetClientCDKeyIdFromDataObject(theOp->GetLinkId(), theOp->GetData());
}
// FUNCTION: WON 0x6c15d0
void SmartRoutingConnection::SetClientCDKeyIdFromDataObject(unsigned short theClientID, ByteBufferPtr theBuf)
{
	AutoCrit aCrit(mDataCrit);

	// Set the client key hash
	RoutingClientInfoPtr theClientInfo = GetClient(theClientID);

	if (theClientInfo.get() == NULL)
		return;

	SmartPtr<SmartRoutingClientInfo> theSmartClientInfo = (SmartRoutingClientInfo*)theClientInfo.get();

	unsigned long anId = 0;
	ReadBuffer aReadBuf(theBuf->data(), theBuf->length());
	try
	{
		aReadBuf.ReadLong();	// the community
		anId = aReadBuf.ReadLong();
	}
	catch(ReadBufferException&)
	{
	}

	theSmartClientInfo->mAddress = anId;
}
// FUNCTION: WON 0x6c1770
void SmartRoutingConnection::HandleClientJoinedGroup(RoutingClientJoinedGroupOp *theOp)
{
	int aGroupId = theOp->GetGroupId();
	RoutingGroupInfoPtr aGroup = GetGroup(aGroupId);
	if(aGroup.get()==NULL)
		return;

	RoutingMemberInfoPtr aMember = Virtual20();
	aMember->mClientId = theOp->GetClientId();
	aMember->mFlags = theOp->GetMemberFlags();

	SmartRoutingClientInfo *aClient = EnsureInClientMap(theOp->GetClientId());

	aMember->mClientInfo = aClient;
	if(!theOp->GetClientName().empty())
		SetClientName(aClient, theOp->GetClientName());

	if(theOp->HasClientFlags())
		aClient->mFlags = theOp->GetClientFlags();

	std::pair<RoutingMemberMap::iterator,bool> aRet;
	aRet = aGroup->mMembers.insert(RoutingMemberMap::value_type(aMember->mClientId,aMember));
	if(aRet.second)
		aClient->mNumGroups++;

	InitHandlerVariables(aGroup.get(),aRet.first->second.get());
}
// FUNCTION: WON 0x6c19f0
void SmartRoutingConnection::HandleClientLeftGroup(RoutingClientLeftGroupOp *theOp)
{
	int aGroupId = theOp->GetGroupId();
	RoutingGroupInfoPtr aGroup = GetGroup(aGroupId);
	if(aGroup.get()==NULL)
		return;

	int aClientId = theOp->GetClientId();

	RoutingMemberMap::iterator anItr = aGroup->mMembers.find(aClientId);
	if(anItr!=aGroup->mMembers.end())
	{
		SmartRoutingClientInfo *aClient = (SmartRoutingClientInfo*)anItr->second->mClientInfo.get();
		aClient->mNumGroups--;
		if((mAsyncFlags & 2) && aClient->mClientId!=mClientId && aClient->mNumGroups==0)
			EraseClient(aClient->mClientId);

		InitHandlerVariables(aGroup.get(),anItr->second.get());
		EraseMember(aGroup->mMembers, anItr);
	}
}
// FUNCTION: WON 0x6c1b30
void SmartRoutingConnection::HandleGetMembersOfGroup(RoutingGetMembersOfGroupOp *theOp)
{
	int aGroupId = theOp->GetGroupId();
	RoutingGroupInfoPtr aGroup = GetGroup(aGroupId);
	if(aGroup.get()==NULL)
		return;

	aGroup->mObserverCount = theOp->GetObserverCount();
	SyncMemberMapWithClientMap(aGroup->mMembers, theOp->mMembers);
}
// FUNCTION: WON 0x6c1bd0
void SmartRoutingConnection::SyncMemberMapWithClientMap(RoutingMemberMap &theOldMap, const RoutingMemberMap &theNewMap)
{
	AutoCrit aCrit(mDataCrit);
	RoutingMemberMap::iterator anOldItr = theOldMap.begin();
	RoutingMemberMap::const_iterator aNewItr = theNewMap.begin();

	while(anOldItr!=theOldMap.end() || aNewItr!=theNewMap.end())
	{
		if(aNewItr==theNewMap.end() || (anOldItr!=theOldMap.end() && anOldItr->first<aNewItr->first))
		{
			SmartRoutingClientInfo *anOldClient = (SmartRoutingClientInfo*)anOldItr->second->mClientInfo.get();
			anOldClient->mNumGroups--;
			if((mAsyncFlags & 2) && anOldClient->mClientId!=mClientId && anOldClient->mNumGroups==0)
				EraseClient(anOldClient->mClientId);
			
			EraseMember(theOldMap, anOldItr++);
		}
		else if(anOldItr==theOldMap.end() || aNewItr->first<anOldItr->first)
		{
			RoutingMemberInfo *aNewMember = aNewItr->second.get();

			SmartRoutingClientInfo *anOldClient = EnsureInClientMap(aNewItr->first);
			SmartRoutingClientInfo *aNewClient = (SmartRoutingClientInfo*)aNewMember->mClientInfo.get();
			if(aNewClient!=NULL)
			{
				if(!aNewClient->mName.empty())
					SetClientName(anOldClient, aNewClient->mName);

				if(aNewClient->mFlags!=0)
					anOldClient->mFlags = aNewClient->mFlags;
			}

			aNewMember->mClientInfo = anOldClient;
			anOldClient->mNumGroups++;
			
			theOldMap.insert(*aNewItr);
			++aNewItr;
		}
		else
		{
			anOldItr->second->mFlags = aNewItr->second->mFlags;
			RoutingClientInfo *aNewClient = aNewItr->second->mClientInfo.get();
			if(aNewClient!=NULL)
			{
				RoutingClientInfo *anOldClient = anOldItr->second->mClientInfo.get();
				if(!aNewClient->mName.empty())
					SetClientName(anOldClient, aNewClient->mName);

				if(aNewClient->mFlags!=0)
					anOldClient->mFlags = aNewClient->mFlags;
			}

			anOldItr++;
			++aNewItr;
		}
	}
}
// FUNCTION: WON 0x6c1f20
void SmartRoutingConnection::SyncMemberMapWithClientMap(RoutingMemberMap &theMap)
{
	AutoCrit aCrit(mDataCrit);
	RoutingMemberMap::iterator anItr = theMap.begin();

	while(anItr!=theMap.end())
	{
		RoutingMemberInfo *aNewMember = anItr->second.get();

		SmartRoutingClientInfo *anOldClient = EnsureInClientMap(anItr->first);
		SmartRoutingClientInfo *aNewClient = (SmartRoutingClientInfo*)aNewMember->mClientInfo.get();
		if(aNewClient!=NULL)
		{
			if(!aNewClient->mName.empty())
				SetClientName(anOldClient, aNewClient->mName);

			if(aNewClient->mFlags!=0)
				anOldClient->mFlags = aNewClient->mFlags;
		}

		aNewMember->mClientInfo = anOldClient;
		anOldClient->mNumGroups++;		
		
		++anItr;
	}
}
// FUNCTION: WON 0x6c2070
void SmartRoutingConnection::HandleGetGroupList(RoutingGetGroupListOp *theOp)
{
	const RoutingGroupMap &aNewMap = theOp->GetGroupMap();
	RoutingGroupMap::const_iterator aNewItr = aNewMap.begin();
	RoutingGroupMap::iterator anOldItr = mGroupMap.begin();
	RoutingMemberMap anEmptyMemberMap;
	bool hasMembers = theOp->IncludeMembers();

	while(anOldItr!=mGroupMap.end() || aNewItr!=aNewMap.end())
	{
		if(aNewItr==aNewMap.end() || (anOldItr!=mGroupMap.end() && anOldItr->first<aNewItr->first))
		{
			SyncMemberMapWithClientMap(anOldItr->second->mMembers, anEmptyMemberMap);
			EraseGroup(anOldItr++);
		}
		else if(anOldItr==mGroupMap.end() || aNewItr->first<anOldItr->first)
		{
			if(hasMembers)
				SyncMemberMapWithClientMap(aNewItr->second->mMembers);
			
			mGroupMap.insert(*aNewItr);
			++aNewItr;
		}
		else
		{
			RoutingGroupInfo *anOldGroup = anOldItr->second.get();
			RoutingGroupInfo *aNewGroup = aNewItr->second.get();

			anOldGroup->mName = aNewGroup->mName;
			anOldGroup->mId = aNewGroup->mId;
			anOldGroup->mCaptainId = aNewGroup->mCaptainId;
			anOldGroup->mMaxClients = aNewGroup->mMaxClients;
			anOldGroup->mFlags = aNewGroup->mFlags;
			anOldGroup->mAsyncFlags = aNewGroup->mAsyncFlags;

			if(hasMembers)
			{
				anOldGroup->mObserverCount = aNewGroup->mObserverCount;
				SyncMemberMapWithClientMap(anOldGroup->mMembers, aNewGroup->mMembers);
			}

			anOldItr++;
			++aNewItr;
		}
	}
}
// FUNCTION: WON 0x6c2430
void SmartRoutingConnection::HandleJoinGroup(RoutingJoinGroupOp *theOp)
{
	if(!(theOp->mStatus==0))
		return;

	int aGroupId = theOp->GetGroupId();
	if((theOp->mStatus==0))
		mDefaultGroupId = aGroupId;
	
	RoutingGroupInfoPtr aGroup = GetGroup(aGroupId);
	if(aGroup.get()==NULL)
		return;

	if(theOp->GetJoinFlags()&1)
		SyncMemberMapWithClientMap(aGroup->mMembers, theOp->mMemberMap);

	InitHandlerVariables(aGroup.get());
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
// FUNCTION: WON 0x6c2590
void SmartRoutingConnection::HandleClientJoinedServer(RoutingClientJoinedServerOp *theOp)
{
	RoutingClientInfoPtr aClient = theOp->mClientInfo;
	SetClientName(aClient.get(), theOp->GetClientName());
	mClientMap[aClient->mClientId] = aClient;

	InitHandlerVariables(aClient.get());
}
// FUNCTION: WON 0x6c26c0
void SmartRoutingConnection::HandleClientLeftServer(RoutingClientLeftServerOp *theOp)
{
	int aClientId = theOp->mClientId;

	RoutingClientMap::iterator anItr = mClientMap.find(aClientId);
	if(anItr==mClientMap.end())
		return;

	InitHandlerVariables(anItr->second.get());
	EraseClient(anItr);
}
// FUNCTION: WON 0x6c2740
void SmartRoutingConnection::SyncClientMapWithIgnoreList()
{
	AutoCrit aCrit(mDataCrit);

	RoutingClientMap::iterator aClientItr = mClientMap.begin();
	while(aClientItr!=mClientMap.end())
	{
		((SmartRoutingClientInfo*)aClientItr->second.get())->mIgnored = (mIgnoreSet.find(aClientItr->second->mName)!=mIgnoreSet.end());
		++aClientItr;
	}
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
// FUNCTION: WON 0x6c29a0
void SmartRoutingConnection::HandleGroupCreated(RoutingGroupCreatedOp *theOp)
{
	RoutingGroupInfoPtr aGroup = Virtual24();
	aGroup->mCaptainId = 0xffff;
	aGroup->mFlags = theOp->GetFlags();
	aGroup->mAsyncFlags = theOp->GetAsyncFlags();
	aGroup->mId = theOp->GetGroupId();
	aGroup->mMaxClients = theOp->GetMaxClients();
	aGroup->mName = theOp->GetName();

	mGroupMap[aGroup->mId] = aGroup;
	InitHandlerVariables(aGroup.get());
}
// FUNCTION: WON 0x6c2b30
void SmartRoutingConnection::HandleGroupDeleted(RoutingGroupDeletedOp *theOp)
{
	int aGroupId = theOp->mGroupId;
	RoutingGroupMap::iterator anItr = mGroupMap.find(aGroupId);
	if(anItr==mGroupMap.end())
		return;
		
	InitHandlerVariables(anItr->second.get());
	EraseGroup(anItr);
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

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x6c2ec0
void SmartRoutingConnection::CheckIgnore(RoutingOp *theOp, unsigned short theSenderId)
{
	RoutingClientInfoPtr aClient = GetClient(theSenderId);
	SmartRoutingClientInfo *aSmartClient = (SmartRoutingClientInfo*)aClient.get();
	if(aSmartClient==NULL)
		return;

	if(aSmartClient->mIgnored)
		theOp->mCompletion = NULL; // swallow the completion
	else
		InitHandlerVariables(aSmartClient);	
}
// FUNCTION: WON 0x006c2f80
void SmartRoutingConnection::HandleGroupInvitation(RoutingGroupInvitationOp *theOp)
{
	CheckIgnore(theOp, theOp->mClientId);
}

// (same code as HandlePeerChat; the linker folded the two at 0x006c2fa0)
void SmartRoutingConnection::HandleGroupJoinAttempt(RoutingGroupJoinAttemptOp *theOp)
{
	CheckIgnore(theOp, theOp->mClientId);
}

// SUPPORT-ONLY: EXE 0x006fa500 (true SDK empty method, naturally folded original, zeroquota)
void SmartRoutingConnection::HandleLeaveGroup(RoutingLeaveGroupOp *)
{
	// erase group from map or erase member list?
}

// SUPPORT-ONLY: EXE 0x006fa500 (true SDK empty method, naturally folded original, zeroquota)
void SmartRoutingConnection::HandleDataObjectDeleted(RoutingDataObjectDeletedOp	*theOp)
{
	// client will be removed from the client map, no special processing needed
}


// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x006c2fa0
void SmartRoutingConnection::HandlePeerChat(RoutingPeerChatOp *theOp)
{
	CheckIgnore(theOp, theOp->mClientId);
}
// FUNCTION: WON 0x006c2fc0
void SmartRoutingConnection::SetProductName(const std::string &thePath)
{
	AutoCrit aCrit(mDataCrit);
	mIgnoreFilePath = thePath;
}

// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x6c3110
void SmartRoutingConnection::LoadIgnoreSet()
{
    AutoCrit crit(mDataCrit);
    if(mIgnoreFilePath.empty()) return;
    FILE* file=fopen(mIgnoreFilePath.c_str(),"rb");
    if(file==NULL) return;
    std::wstring name;
    while(!feof(file)) {
        wchar_t ch=0;
        if(fread(&ch,sizeof(wchar_t),1,file)!=1) break;
        if(ch!=0) name+=ch;
        else {
            mIgnoreSet.insert(name);
            name.erase();
        }
    }
    fclose(file);
}
// FUNCTION: WON 0x6c3460
void SmartRoutingConnection::SaveIgnoreSet()
{
    AutoCrit crit(mDataCrit);
    if(mIgnoreFilePath.empty()) return;
    FILE* file=fopen(mIgnoreFilePath.c_str(),"wb");
    if(file==NULL) return;
    IgnoreSet::iterator i=mIgnoreSet.begin();
    while(i!=mIgnoreSet.end()) {
        fwrite(i->c_str(),sizeof(wchar_t),i->length(),file);
        wchar_t end=0;
        fwrite(&end,sizeof(wchar_t),1,file);
        ++i;
    }
    fclose(file);
}
// FUNCTION: WON 0x6c3730
static bool RoutingNamesMatch(const std::wstring& query,const std::wstring& name,bool& partial)
{
    partial=false;
    std::wstring::const_iterator q=query.begin(),n=name.begin();
    while(q!=query.end() && n!=name.end()) {
        if(towupper(*q)!=towupper(*n)) return false;
        ++q;
        ++n;
    }
    if(query.length()<=name.length()) partial=true;
    return query.length()==name.length();
}
// FUNCTION: WON 0x6c35a0
SmartRoutingConnection::MatchResult SmartRoutingConnection::FindClient(
    const std::wstring& inName,RoutingClientInfoPtr& outClient)
{
    AutoCrit crit(mDataCrit);
    MatchResult result=MatchResult_NotFound;
    for(RoutingClientMap::iterator i=mClientMap.begin();i!=mClientMap.end();++i) {
        const std::wstring& aName=i->second->mName;
        bool partial=false;
        if(RoutingNamesMatch(inName,aName,partial)) {
            if(result==MatchResult_Exact) return MatchResult_Ambiguous;
            result=MatchResult_Exact;
            outClient=i->second;
        } else if(partial) {
            if(result==MatchResult_NotFound) {
                result=MatchResult_Partial;
                outClient=i->second;
            } else if(result==MatchResult_Partial) result=MatchResult_Ambiguous;
        }
    }
    return result;
}
// FUNCTION: WON 0x006c37d0
void SmartRoutingConnection::IgnoreClient(RoutingClientInfo *theClient, bool on)
{
	AutoCrit aCrit(mDataCrit);

	((SmartRoutingClientInfo*)theClient)->mIgnored = on;
	if(on)
		mIgnoreSet.insert(theClient->mName);
	else
		mIgnoreSet.erase(theClient->mName);

	SaveIgnoreSet();
}
// FUNCTION: WON 0x006c3970
void SmartRoutingConnection::AddCommandString(const std::wstring& inText, InputCommand inCommand)
{
    AutoCrit crit(mDataCrit);
    mCommandStringMap[WStringToUpperCase(inText)] = inCommand;
}
// FUNCTION: WON 0x6c3a50
void SmartRoutingConnection::AddDefaultUserCommandStrings()
{
    AutoCrit crit(mDataCrit);
    AddCommandString(L"W", InputCommand_Whisper);
    AddCommandString(L"WHISPER", InputCommand_Whisper);
    AddCommandString(L"ME", InputCommand_Emote);
    AddCommandString(L"EMOTE", InputCommand_Emote);
    AddCommandString(L"I", InputCommand_Ignore);
    AddCommandString(L"IGNORE", InputCommand_Ignore);
    AddCommandString(L"AWAY", InputCommand_Away);
    AddCommandString(L"AFK", InputCommand_Away);
}
// FUNCTION: WON 0x6c3d90
void SmartRoutingConnection::AddDefaultModeratorCommandStrings()
{
    AutoCrit crit(mDataCrit);
    AddCommandString(L"MOD", InputCommand_BecomeModerator);
    AddCommandString(L"SMUTE", InputCommand_ServerMute);
    AddCommandString(L"SBAN", InputCommand_ServerBan);
    AddCommandString(L"MUTE", InputCommand_Mute);
    AddCommandString(L"BAN", InputCommand_Ban);
    AddCommandString(L"UNMUTE", InputCommand_Unmute);
    AddCommandString(L"SUNMUTE", InputCommand_ServerUnmute);
}
// FUNCTION: WON 0x006c40a0
void SmartRoutingConnection::SetAllowPartialNameMatch(bool inAllow)
{
    AutoCrit crit(mDataCrit);
    if(inAllow)
        mMatchTolerance = MatchResult_Partial;
    else
        mMatchTolerance = MatchResult_Exact;
}
// FUNCTION: WON 0x006c40f0
void SmartRoutingConnection::SetServerWideCommands(bool inOn)
{
    AutoCrit crit(mDataCrit);
    mServerWideCommands = inOn;
}
// FUNCTION: WON 0x006c4120
void SmartRoutingConnection::Virtual08()
{
    mClientId=0;
    mMyClient=NULL;
    RoutingConnection::Virtual08();
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bfce0
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<struct WONAPI::RoutingClientInfo>,struct std::less<unsigned short>,class std::alloca)
// TEMPLATE: WON 0x006bfda0
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<struct WONAPI::RoutingGroupInfo>,struct std::less<unsigned short>,class std::allocat)
// TEMPLATE: WON 0x006bff00
// (public: virtual void * __thiscall WONAPI::SmartRoutingClientInfo::`scalar deleting destructor'(unsigned int)    [??_GSmartRoutingClientInfo@WONAPI@@UA)
// TEMPLATE: WON 0x006bff20
// (public: virtual __thiscall WONAPI::SmartRoutingClientInfo::~SmartRoutingClientInfo(void)    [??1SmartRoutingClientInfo@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bff90
// (public: virtual void * __thiscall WONAPI::SmartRoutingConnection::`scalar deleting destructor'(unsigned int)    [??_GSmartRoutingConnection@WONAPI@@UA)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bffb0
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,enum)
// TEMPLATE: WON 0x006c0070
// (public: __thiscall std::set<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,stru)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c0130
// (public: virtual __thiscall WONAPI::SmartRoutingConnection::~SmartRoutingConnection(void)    [??1SmartRoutingConnection@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c1550
// (public: unsigned short __thiscall WONAPI::RoutingDataObjectCreatedOp::GetLinkId(void)const     [?GetLinkId@RoutingDataObjectCreatedOp@WONAPI@@QBEGXZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c2370
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<struct WONAPI::RoutingMemberInfo>,struct std::less<unsigned short>,class std::alloca)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c3420
// (_wmemset)
// TEMPLATE: WON 0x006c9040
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingClientInfo> >,struct std::)
// TEMPLATE: WON 0x006c9200
// (public: struct std::pair<class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingGroupInfo)
// TEMPLATE: WON 0x006c92f0
// (public: class WONAPI::SmartPtr<struct WONAPI::RoutingDataObject> const & __thiscall std::list<class WONAPI::SmartPtr<struct WONAPI::RoutingDataObject>)
// TEMPLATE: WON 0x006c9300
// (protected: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingMemberInfo> >,struct st)
// TEMPLATE: WON 0x006c96e0
// (protected: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingGroupInfo> >,struct std)
// TEMPLATE: WON 0x006c99e0
// (public: struct std::pair<class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned)
// TEMPLATE: WON 0x006c9ae0
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006c9b90
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,class s)
// TEMPLATE: WON 0x006c9c40
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,class s)
// TEMPLATE: WON 0x006c9c90
// (protected: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,clas)
// TEMPLATE: WON 0x006ca290
// (protected: struct std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingMemberInfo> >,struct s)
// TEMPLATE: WON 0x006ca750
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006cac30
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006cacb0
// (protected: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,stru)
// TEMPLATE: WON 0x006cafb0
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,class s)
// TEMPLATE: WON 0x006cb490
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,class s)
// TEMPLATE: WON 0x006cb4e0
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006cb550
// (protected: struct std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,cla)
// TEMPLATE: WON 0x006cb5a0
// (public: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)
// TEMPLATE: WON 0x006cbc60
// (void __cdecl std::_Construct(struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingMemberInfo> > *,struct std::pair<unsi)
// TEMPLATE: WON 0x006cbca0
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsi)
// TEMPLATE: WON 0x006cbe00
// (void __cdecl std::_Distance(class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsig)
}
