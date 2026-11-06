// Native smart routing completion dispatch and state/ignore event handlers.
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

	mClientId = theOp->mClientId;
	mAsyncFlags = theOp->mAsyncMessageFlags;

	mGroupMap.clear();
	mClientMap = theOp->mClientMap;
	if(mClientMap.empty()) // didn't get client list (insert myself)
	{
		mMyClient = Virtual1C();
		mMyClient->mName = theOp->mClientName;
		mMyClient->mClientId = theOp->mClientId;
		mMyClient->mFlags = theOp->mClientFlags;
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

// FUNCTION: WON 0x6c1400
void SmartRoutingConnection::HandleReadDataObject(RoutingReadDataObjectOp *theOp)
{
	// return if not parsing client CDKey ID's 
	if (theOp->mDataType != "_KeyId")
		return;

	RoutingDataObjectList::const_iterator anItr = theOp->mDataObjects.begin();
	for (; anItr != theOp->mDataObjects.end(); ++anItr)
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
	aMember->mClientId = theOp->mClientId;
	aMember->mFlags = theOp->mMemberFlags;

	SmartRoutingClientInfo *aClient = EnsureInClientMap(theOp->mClientId);

	aMember->mClientInfo = aClient;
	if(!theOp->mClientName.empty())
		SetClientName(aClient, theOp->mClientName);

	if(theOp->mHasClientFlags)
		aClient->mFlags = theOp->mClientFlags;

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

// FUNCTION: WON 0x6c2590
void SmartRoutingConnection::HandleClientJoinedServer(RoutingClientJoinedServerOp *theOp)
{
	RoutingClientInfoPtr aClient = theOp->mClientInfo; //Virtual1C();
//	aClient->mFlags = theOp->GetClientFlags();
//	aClient->mClientId = theOp->GetClientId();
	SetClientName(aClient.get(), theOp->mClientInfo->mName);
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

// FUNCTION: WON 0x6c29a0
void SmartRoutingConnection::HandleGroupCreated(RoutingGroupCreatedOp *theOp)
{
	RoutingGroupInfoPtr aGroup = Virtual24();
	aGroup->mCaptainId = 0xffff;
	aGroup->mFlags = theOp->mFlags;
	aGroup->mAsyncFlags = theOp->mAsyncFlags;
	aGroup->mId = theOp->mGroupId;
	aGroup->mMaxClients = theOp->mMaxClients;
	aGroup->mName = theOp->mName;

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

// FUNCTION: WON 0x006c2fa0
void SmartRoutingConnection::HandlePeerChat(RoutingPeerChatOp *theOp)
{
	CheckIgnore(theOp, theOp->mClientId);
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

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006c1550
// (public: unsigned short __thiscall WONAPI::RoutingDataObjectCreatedOp::GetLinkId(void)const     [?GetLinkId@RoutingDataObjectCreatedOp@WONAPI@@QBEGXZ])
