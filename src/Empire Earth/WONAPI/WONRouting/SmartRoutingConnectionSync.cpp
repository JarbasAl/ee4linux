// Native smart routing incremental client/member/group synchronization.
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

			++anOldItr;
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

	bool hasMembers = ((theOp->mFlags&0x80)!=0);

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

			++anOldItr;
			++aNewItr;
		}
	}
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

}

namespace WONAPI {
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

// FUNCTION: WON 0x006c2fc0
void SmartRoutingConnection::SetProductName(const std::string &thePath)
{
	AutoCrit aCrit(mDataCrit);
	mIgnoreFilePath = thePath;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006c2370
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<struct WONAPI::RoutingMemberInfo>,struct std::less<unsigned short>,class std::alloca)
