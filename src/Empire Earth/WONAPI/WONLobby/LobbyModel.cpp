#include <stdio.h>
// C:\EEX\Empire Earth\WONAPI\WONLobby\LobbyModel.cpp (reconstruction filename PROVISIONAL).
// One root-adjudicated original model object hypothesis, grounded by EH/order and deferred templates.
// Original file spelling unproved. User families3a51a0..3aa9e0; deferred template tail through3ae8d0.
// Complete contributed bodies only; other original functions remain carved, with zero replacement credit.
// Scoped SXL233 repairs frozen0/library-exception ownership; residual grow/trim helpers remain qualified near.
// PROVISIONAL: canonical hinted insertion ignores hints; invalid/stale-hint behavior and complexity are unproved.
// Near bodies have no runtime substitution authority. Primary code matches do not certify complete EH/data.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233

#include <windows.h>
#include <string.h>
#include <time.h>
#include <set>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "LostDebug.h"
namespace WONAPI {
LobbyClientMap& GetEmptyClientMap();

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a51a0
LobbyClient::LobbyClient() : mName(GetEmptyGUIStringData())
{
    mClientId = 0xffff;
    mGroupId1E = 0xffff;
    mGroupId20 = 0xffff;
    mU10 = 0;
    mU14 = 0;
    mU18 = 0;
    mFlags1C = 0;
    mFlags1D = 0;
    mB22 = false;
    mB23 = false;
    mB24 = false;
    mB25 = false;
    mB26 = false;
    mB27 = false;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5260
LobbyClient::~LobbyClient() {}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a52f0
void LobbyClient::Fn3A52F0(unsigned short inId, bool inMode)
{
    if (inMode) {
        if (inId == mGroupId20) {
            mGroupId20 = 0xffff; mFlags1D = 0; mB25 = false;
        }
    } else if (inId == mGroupId1E) {
        mGroupId1E = 0xffff; mFlags1C = 0; mB26 = false;
    }
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5330
bool LobbyClient::Fn3A5330() { return mGroupId1E != 0xffff; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5340
bool LobbyClient::Fn3A5340() { return mGroupId20 != 0xffff; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5350
void LobbyClient::Fn3A5350(RoutingClientInfo* inInfo)
{
    mName = GUIString(new GUIStringData(inInfo->mName));
    mU10 = inInfo->mFlags;
    mClientId = inInfo->mClientId;
    mU14 = inInfo->mUserId;
    mU18 = inInfo->mAddress;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5450
void LobbyClient::Fn3A5450()
{
    mB22 = LobbyFn35D010(mName);
    mB23 = LobbyFn35D090(mU18);
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5470
bool LobbyClient::Fn3A5470() { return (mU10 & 0x40) ? true : false; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5480
bool LobbyClient::Fn3A5480(bool inMode)
{
    if (mU10 & 2) return true;
    if (inMode) return (mFlags1D & 2) ? true : false;
    return (mFlags1C & 2) ? true : false;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a54b0
bool LobbyClient::Fn3A54B0() { return (mU10 & 8) ? true : false; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a54c0
bool LobbyClient::Fn3A54C0() { return (mU10 & 0x20) ? true : false; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a54d0
bool LobbyClient::Fn3A54D0() { return (mU10 & 1) ? true : false; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a54e0
bool LobbyClient::Fn3A54E0(bool inFlag) { return inFlag ? mB25 : mB26; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5500
void LobbyClient::Fn3A5500(bool inValue, bool inMode)
{
    if (inMode) mB25 = inValue;
    else mB26 = inValue;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5520
void LobbyClient::Fn3A5520(LobbyPlayer* inPlayer) { mPlayer = inPlayer; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5580
bool LobbyClient::Fn3A5580() { return mPlayer.get() != NULL; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5590
bool LobbyClient::Fn3A5590() { return mPlayer.get() != NULL ? mPlayer->mBA : false; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a55a0
unsigned short LobbyClient::Fn3A55A0()
{
    if (mPlayer.get() != NULL) return mPlayer->mPlayerId;
    return 0xfffe;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a55c0
bool LobbyClient::Fn3A55C0(LobbyClient* inOther)
{
    if (inOther == NULL || mPlayer.get() == NULL || inOther->mPlayer.get() == NULL) return false;
    return mPlayer->IsSameTeam(inOther->mPlayer.get());
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a55f0
LobbyClientList::LobbyClientList() {}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a56a0
LobbyClientList::~LobbyClientList() {}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5770
void LobbyClientList::Fn3A5770() { mClients.clear(); }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5820
void LobbyClientList::Fn3A5820(const RoutingClientMap& inMap)
{
    LobbyClientMap::iterator aClient = mClients.begin();
    LobbyClientMap::iterator anInsert = mClients.begin();
    RoutingClientMap::const_iterator anInfo = inMap.begin();
    while (aClient != mClients.end() && anInfo != inMap.end()) {
        if (anInfo->first > aClient->first) mClients.erase(aClient++);
        else if (anInfo->first < aClient->first) {
            SmartPtr<LobbyClient> aNewClient = new LobbyClient;
            aNewClient->Fn3A5350(anInfo->second.get());
            anInsert = mClients.insert(anInsert, LobbyClientMap::value_type(aNewClient->mClientId, aNewClient));
            ++anInfo;
        } else {
            SmartPtr<LobbyClient> aCopy = aClient->second;
            aCopy->Fn3A5350(anInfo->second.get());
            ++aClient; ++anInfo;
        }
    }
    mClients.erase(aClient, mClients.end());
    while (anInfo != inMap.end()) {
        SmartPtr<LobbyClient> aNewClient = new LobbyClient;
        aNewClient->Fn3A5350(anInfo->second.get());
        anInsert = mClients.insert(anInsert, LobbyClientMap::value_type(aNewClient->GetClientId(), aNewClient));
        ++anInfo;
    }
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5c50
void LobbyClientList::Fn3A5C50()
{
    for (LobbyClientMap::iterator it = mClients.begin(); it != mClients.end(); ++it)
        it->second->Fn3A5450();
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5cc0
void LobbyClientList::Fn3A5CC0(LobbyClient* inClient) {
    unsigned short clientId = inClient->mClientId;
    mClients[clientId] = inClient;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5d30
LobbyClient* LobbyClientList::FindClient(unsigned short inId)
{
    LobbyClientMap::iterator anItr = mClients.find(inId);
    return anItr != mClients.end() ? anItr->second.get() : NULL;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5d90
SmartPtr<LobbyClient> LobbyClientList::Fn3A5D90(unsigned short inId)
{
    LobbyClientMap::iterator anItr = mClients.find(inId);
    if (anItr != mClients.end()) {
        SmartPtr<LobbyClient> aClient = anItr->second;
        mClients.erase(anItr);
        return aClient;
    }
    return NULL;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5ea0
LobbyClient* LobbyClientList::Fn3A5EA0(unsigned long inId)
{
    for (LobbyClientMap::iterator anItr = mClients.begin(); anItr != mClients.end(); ++anItr)
    {
        LobbyClient* aClient = anItr->second;
        if (aClient->GetU14() == inId) return aClient;
    }
    return NULL;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5f10
void LobbyClientList::Fn3A5F10()
{
    for (LobbyClientMap::iterator anItr = mClients.begin(); anItr != mClients.end(); ++anItr)
        anItr->second->SetB27(false);
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5f70
LobbyClientIterator::LobbyClientIterator(LobbyClientList* inList)
    : mClients(inList != NULL ? &inList->mClients : &GetEmptyClientMap())
{
    Rewind();
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a5fa0
LobbyClientMap& GetEmptyClientMap() { static LobbyClientMap aMap; return aMap; }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a60b0
bool LobbyClientIterator::HasMore() { return mItr != mClients->end(); }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a60d0
LobbyClient* LobbyClientIterator::Next()
{
    if (mItr != mClients->end()) {
        LobbyClient* aClient = mItr->second.get();
        ++mItr;
        return aClient;
    }
    return NULL;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a6140
void LobbyClientIterator::Rewind() { mItr = mClients->begin(); }

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a6150
LobbyClientSetIterator::LobbyClientSetIterator(LobbyClientList* inList)
    : mClients(inList != NULL ? &inList->mClients : &GetEmptyClientMap())
{
    Rewind();
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a6180
bool LobbyClientSetIterator::HasMore()
{
    if (mReady) return true;
    while (mItr != mClients->end()) {
        if (mItr->second->mPlayer.get() != NULL) {
            mReady = true;
            return true;
        }
        ++mItr;
    }
    return false;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a6210
LobbyPlayer* LobbyClientSetIterator::Next(LobbyClient** outClient)
{
    if (HasMore()) {
        LobbyClient* aClient = mItr->second.get();
        if (outClient != NULL) *outClient = aClient;
        LobbyPlayer* aPlayer = aClient->mPlayer.get();
        mReady = false;
        ++mItr;
        return aPlayer;
    }
    return NULL;
}

// CONTRIBUTOR: client/LobbyClient.cpp.draft
// FUNCTION: WON 0x7a62a0
void LobbyClientSetIterator::Rewind() { mItr = mClients->begin(); mReady = false; }

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a62b0
LobbyFriend::LobbyFriend(const GUIString& inName, unsigned long inUserId)
    : mName(inName), mLocation(GetEmptyGUIStringData())
{
    mUserId = inUserId;
    mFlags = 0;
    mLastSearch = 0;
    mLastLocate = 0;
    mSearchStart = 0;
    mAnonymous = false;
}

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// TODO: EXE 0x7a6380 Original state=1 store at 0x3a63d0 before IPAddr host release is missing in candidate; state remains too high across delete and may repeat IPAddr destruction on throw. Original remains carved.

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a6440
void LobbyFriend::Fn3A6440(const IPAddr& inAddress, const GUIString& inLocation,
                          int inFlags)
{
    mAddress = inAddress;
    mLocation = inLocation;
    mFlags = inFlags;
}

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a65b0
void LobbyFriend::Fn3A65B0()
{
    mAddress = IPAddr();
    mLocation = GetEmptyGUIStringData();
    mFlags = 0;
    mLastLocate = 0;
    mAnonymous = false;
}

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a66b0
void LobbyFriend::Fn3A66B0(LobbyClient* inClient)
{
    int flags = 0;
    if (inClient->Fn3A5330()) flags = 1;
    if (inClient->Fn3A5340()) flags |= 2;
    mFlags = flags;
    mAnonymous = inClient->Fn3A5470();
}

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a66f0
void LobbyFriend::Fn3A66F0(bool inAnonymous) { mAnonymous = inAnonymous; }

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a6700
void LobbyFriend::Fn3A6700(unsigned long inTime)
{
    mFlags = 4;
    mSearchStart = inTime;
}

// CONTRIBUTOR: social/LobbyFriend.cpp.draft
// FUNCTION: WON 0x7a6710
void LobbyFriend::Fn3A6710(unsigned long inTime)
{
    mFlags &= ~4;
    mLastLocate = inTime;
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6790
LobbyGame::LobbyGame() : mName(GetEmptyGUIStringData()), mPassword(GetEmptyGUIStringData())
{
    mL60 = mL62 = 0xffff;
    mL2C = 0;
    mB11 = false;
    mB12 = false;
    mB13 = false;
    mB10 = false;
    mB14 = false;
    mIsHost = false;
    mB16 = false;
    mB17 = false;
    mB18 = false;
    mL30 = 0;
    mL24 = 0;
    mNumOpenSlots = 0;
    mL64 = 0xfffe;
    mL1C = 0;
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a69d0
void LobbyGame::CopyFrom(LobbyGame* inGame)
{
    mName = inGame->mName;
    mPassword = inGame->mPassword;
    mB10 = inGame->mB10;
    mB11 = inGame->mB11;
    mB12 = inGame->mB12;
    mB13 = inGame->mB13;
    mB14 = inGame->mB14;
    mIsHost = inGame->mIsHost;
    mB16 = inGame->mB16;
    mB17 = inGame->mB17;
    mB18 = inGame->mB18;
    mL1C = inGame->mL1C;
    mL24 = inGame->mL24;
    mNumOpenSlots = inGame->mNumOpenSlots;
    mL2C = inGame->mL2C;
    mL30 = inGame->mL30;
    mClients = inGame->mClients.get();
    mAddress = inGame->mAddress;
    mL60 = inGame->mL60;
    mL62 = inGame->mL62;
    mL64 = inGame->mL64;
    Virtual40(inGame);
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6c10
void LobbyGame::SetClients(LobbyClientList* inClients) { mClients = inClients; }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6c70
LobbyClient* LobbyGame::FindClient(unsigned short inId)
{
    if (mClients.get() != NULL) return mClients->FindClient(inId);
    return NULL;
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6d40
bool LobbyGame::Fn3A6D40(long inFlags)
{
    if (inFlags & 4) {
        if (mL2C == 2) return true;
    }
    else if (inFlags & 8) {
        if (mL2C == 1 || mL2C == 3) return true;
    }
    return false;
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6d70
bool LobbyGame::Fn3A6D70() { return mL64 == 0xfffd || mL64 == 0xfffc; }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6d90
bool LobbyGame::Fn3A6D90() { return mL64 == 0xfffc; }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6da0
bool LobbyGame::Fn3A6DA0() { return Virtual04(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6db0
bool LobbyGame::Fn3A6DB0() { return Virtual08(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6dc0
bool LobbyGame::Fn3A6DC0() { return Virtual0C(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6dd0
bool LobbyGame::Fn3A6DD0() { return Virtual10(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6de0
void LobbyGame::Fn3A6DE0() { Virtual14(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6df0
void LobbyGame::Fn3A6DF0() { Virtual18(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a6ed0
void LobbyGame::WriteSummary(WriteBuffer& outBuffer)
{
    if (mL2C == 2)
        outBuffer.AppendBytes(mAddress.GetSixByte(), 6);
    else
        outBuffer.AppendShort(GetLobbyProductId());
    outBuffer.AppendBool(mB13);
    if (mL2C != 2)
        outBuffer.AppendWString(mName, 2);
    outBuffer.AppendByte(mL30);
    if (mL2C != 2) {
        unsigned char aFlags = 0;
        if (mPassword.length() != 0) aFlags = 1;
        if (mB11) aFlags |= 2;
        if (mB12) aFlags |= 4;
        outBuffer.AppendByte(aFlags);
    }
    outBuffer.AppendShort(mL24);
    outBuffer.AppendShort(mNumOpenSlots);
    Virtual50(outBuffer);
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a7000
bool LobbyGame::ReadSummary(ReadBuffer& inBuffer)
{
    try {
        if (mL2C == 2)
            mAddress.SetSixByte(inBuffer.ReadBytes(6));
        else {
            short aProductId = inBuffer.ReadShort();
            if (aProductId != GetLobbyProductId()) return false;
        }
        mB13 = inBuffer.ReadBool();
        if (mL2C != 2) {
            std::wstring aName;
            inBuffer.ReadWString(aName, 2);
            mName = GUIString(new GUIStringData(aName));
        }
        mL30 = (char)inBuffer.ReadByte();
        if (mL30 < 0 || mL30 >= 5) mL30 = 0;
        if (mL2C != 2) {
            unsigned char aFlags = inBuffer.ReadByte();
            mB10 = (aFlags & 1) != 0;
            mB11 = ((aFlags >> 1) & 1) != 0;
            mB12 = ((aFlags >> 2) & 1) != 0;
        }
        mL24 = inBuffer.ReadShort();
        mNumOpenSlots = inBuffer.ReadShort();
        return Virtual58(inBuffer);
    }
    catch (ReadBufferException&) { return false; }
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a7240
void LobbyGame::Fn3A7240(WriteBuffer& outBuffer) { Virtual54(outBuffer); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a7250
bool LobbyGame::ReadDetails(ReadBuffer& inBuffer)
{
    try {
        if (Virtual5C(inBuffer)) {
            mB17 = true;
            return true;
        }
        return false;
    }
    catch (ReadBufferException&) { return false; }
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a74b0
bool LobbyGame::Fn3A74B0() { return Virtual28(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a74c0
bool LobbyGame::Fn3A74C0() { return Virtual20(); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a74d0
void LobbyGame::Fn3A74D0(LobbyClient* inClient) { Virtual4C(inClient); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a74e0
void LobbyGame::RemoveClient(LobbyClient* inClient)
{
    SmartPtr<LobbyPlayer> aPlayer(inClient->mPlayer);
    if (aPlayer.get() == NULL) return;
    --mL24;
    inClient->Fn3A5520(NULL);
    Virtual48(inClient, aPlayer.get());
}

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a79e0
void LobbyGame::Fn3A79E0(unsigned char inType, ReadBuffer& inBuffer, LobbyClient* inFrom)
{ Virtual60(inType, inBuffer, inFrom); }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a7a00
bool LobbyGame::IsFull() { return mNumOpenSlots > 0 && mL24 >= mNumOpenSlots; }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a7a20
bool LobbyGame::Fn3A7A20() { return GetJoinStatus() == 0; }

// CONTRIBUTOR: player/LobbyGame.cpp
// FUNCTION: WON 0x7a7a30
short LobbyGame::GetJoinStatus()
{
    if (IsFull()) return 2;
    if (mB11 && !mB14 && !mIsHost) return 5;
    if (mB13) return 3;
    return Virtual2C();
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8a90
LobbyGroup::LobbyGroup() : mName(GetEmptyGUIStringData()), mClients(NULL)
{
    mNumClients = 0; mMaxClients = 0; mFlags = 0;
    mId = 0xffff; mCaptainId = 0xffff;
    mClients = new LobbyClientList;
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8ba0
LobbyGroup::~LobbyGroup() {}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8c30
void LobbyGroup::Fn3A8C30(RoutingGroupInfo* inInfo, LobbyClientList* inClients)
{
    mId = inInfo->mId;
    mName = GUIString(new GUIStringData(inInfo->mName));
    mNumClients = inInfo->mMaxPlayers; // canonical legacy name: original +0x2a is current member count
    mFlags = inInfo->mFlags;
    mCaptainId = inInfo->mCaptainId;
    mMaxClients = inInfo->mMaxClients;
    if (inClients != NULL) Fn3A8D50(inInfo->mMembers, inClients);
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8d50
void LobbyGroup::Fn3A8D50(const RoutingMemberMap& inMembers, LobbyClientList* inClients)
{
    mClients->Fn3A5770();
    RoutingMemberMap::const_iterator it = inMembers.begin();
    while (it != inMembers.end()) {
        LobbyClient* client = inClients->FindClient(it->first);
        if (client != NULL) {
            RoutingMemberInfo* aMember = it->second;
            client->SetMemberFlags(aMember->mFlags, Fn3A9060());
            client->Fn3A5500(client->GetClientId() == mCaptainId, Fn3A9060());
            client->SetGroupId(mId, Fn3A9060());
            mClients->Fn3A5CC0(client);
        }
        ++it;
    }
    mNumClients = mClients->mClients.size();
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8e40
void LobbyGroup::Fn3A8E40(LobbyClient* inClient, unsigned char inFlags)
{
    mClients->Fn3A5CC0(inClient);
    inClient->SetGroupId(mId, Fn3A9060());
    inClient->SetMemberFlags(inFlags, Fn3A9060());
    inClient->Fn3A5500(inClient->GetClientId() == mCaptainId, Fn3A9060());
    mNumClients = mClients->mClients.size();
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8eb0
SmartPtr<LobbyClient> LobbyGroup::Fn3A8EB0(unsigned short inId)
{
    SmartPtr<LobbyClient> client = mClients->Fn3A5D90(inId);
    mNumClients = mClients->mClients.size();
    if (client.get() != NULL) client->Fn3A52F0(mId, Fn3A9060());
    return client;
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8f80
LobbyClient* LobbyGroup::Fn3A8F80(unsigned short inId, LobbyClient** outOld)
{
    LobbyClient* oldClient = Fn3A8FF0();
    if (oldClient != NULL) oldClient->Fn3A5500(false, Fn3A9060());
    LobbyClient* newClient = NULL;
    if (inId != 0xffff) {
        newClient = Fn3A9010(inId);
        if (newClient != NULL) newClient->Fn3A5500(true, Fn3A9060());
    }
    if (outOld != NULL) *outOld = oldClient;
    mCaptainId = inId;
    return newClient;
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a8ff0
LobbyClient* LobbyGroup::Fn3A8FF0()
{
    if (mCaptainId != 0xffff) return Fn3A9010(mCaptainId);
    return NULL;
}

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9010
LobbyClient* LobbyGroup::Fn3A9010(unsigned short inId) { return mClients->FindClient(inId); }

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9020
bool LobbyGroup::Fn3A9020() { return (mFlags >> 2) & 1; }

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9030
bool LobbyGroup::Fn3A9030() { return (mFlags >> 8) & 1; }

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9040
bool LobbyGroup::Fn3A9040() { return (mFlags >> 7) & 1; }

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9050
bool LobbyGroup::Fn3A9050() { return ((mFlags >> 11) & 1) ? true : false; }

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9060
bool LobbyGroup::Fn3A9060() { return ((mFlags >> 11) & 1) ? false : true; }

// CONTRIBUTOR: social/LobbyGroup.cpp.draft
// FUNCTION: WON 0x7a9070
bool LobbyGroup::Fn3A9070()
{
    return Fn3A9050() && !Fn3A9020() && !Fn3A9040() && !Fn3A9030();

}

// CONTRIBUTOR: player/LobbyPlayer.cpp
// FUNCTION: WON 0x7a90b0
LobbyPlayer::LobbyPlayer() { mPlayerId=0xfffe; mBA=false; }

// CONTRIBUTOR: player/LobbyPlayer.cpp
// FUNCTION: WON 0x7a90d0
bool LobbyPlayer::IsTeammate(LobbyPlayer* inOther) { return false; }

// CONTRIBUTOR: player/LobbyPlayer.cpp
// FUNCTION: WON 0x7a90f0
bool LobbyPlayer::ReadFromBuffer(ReadBuffer& inBuffer)
{
    try {
        mPlayerId = inBuffer.ReadShort();
        mBA = inBuffer.ReadBool();
        return ReadData(inBuffer);
    }
    catch (ReadBufferException&) {
        return false;
    }
}

// CONTRIBUTOR: player/LobbyPlayer.cpp
// FUNCTION: WON 0x7a9170
void LobbyPlayer::WriteToBuffer(WriteBuffer& outBuffer)
{
    outBuffer.AppendShort(mPlayerId);
    outBuffer.AppendBool(mBA);
    WriteData(outBuffer);
}

// CONTRIBUTOR: player/LobbyPlayer.cpp
// FUNCTION: WON 0x7a91a0
bool LobbyPlayer::IsSameTeam(LobbyPlayer* inOther)
{
    if (this == inOther) return true;
    if (inOther == NULL) return false;
    return IsTeammate(inOther);
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a91c0
LobbyServer::LobbyServer() : mName(GetEmptyGUIStringData()), mClients(NULL), mConnection(NULL)
{
    mNumChatClients = 0; mNumClients = 0; mLastPing = 0; mB4C = false; mMyClientId = 0xffff;
    mClients = new LobbyClientList;
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a9490
bool LobbyServer::Fn3A9490(const std::string& inName)
{
    return mAddresses.find(inName) != mAddresses.end();
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// TODO: EXE 0x7a9500 Original state=2 store at 0x3a95a9 before IPAddr host release is missing in candidate; state remains too high across delete and may repeat IPAddr destruction on throw. Original remains carved.

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a9640
void LobbyServer::Fn3A9640(RoutingConnection* inConnection) { mConnection = inConnection; }

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a9ad0
void LobbyServer::Fn3A9AD0(const RoutingClientMap& inMap)
{
    mClients->Fn3A5820(inMap);
    mClients->Fn3A5C50();
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a9af0
void LobbyServer::Fn3A9AF0()
{
    LobbyClientMap& aClients = mClients->mClients;
    for (LobbyClientMap::iterator it = aClients.begin(); it != aClients.end(); ++it) {
        LobbyClient* client = it->second;
        LobbyFriend* aFriend = LobbyFn35D3F0(client->GetU14());
        if (aFriend != NULL) {
            client->SetIsFriend(true);
            aFriend->Fn3A6440(mAddress, mName, 0);
            aFriend->Fn3A66B0(client);
        } else client->SetIsFriend(false);
    }
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a9b60
void LobbyServer::Fn3A9B60()
{
    time_t now = time(NULL);
    LobbyClientMap& aClients = mClients->mClients;
    for (LobbyClientMap::iterator it = aClients.begin(); it != aClients.end(); ++it) {
        LobbyClient* client = it->second;
        if (client->IsFriend()) {
            LobbyFriend* aFriend = LobbyFn35D3F0(client->GetU14());
            if (aFriend != NULL) {
                if (client->Fn3A5470()) aFriend->Fn3A65B0();
                else aFriend->SetLastLocate(now);
            }
        }
    }
}

// CONTRIBUTOR: social/ServerMerge.cpp.draft
// FUNCTION: WON 0x7a9be0
void LobbyServer::Fn3A9BE0(const RoutingGroupMap& inGroups, bool inUpdateClients)
{
    mNumChatClients = 0;
    mNumClients = 0;
    LobbyClientList* clients = inUpdateClients ? mClients : SmartPtr<LobbyClientList>(NULL);
    LobbyGroupMap::iterator groupIt = mGroups.begin();
    LobbyGroupMap::iterator insertIt = mGroups.begin();
    RoutingGroupMap::const_iterator infoIt = inGroups.begin();
    while (groupIt != mGroups.end() && infoIt != inGroups.end()) {
        if (infoIt->first > groupIt->first) mGroups.erase(groupIt++);
        else if (infoIt->first < groupIt->first) {
            SmartPtr<LobbyGroup> group = new LobbyGroup;
            group->Fn3A8C30(infoIt->second.get(), clients);
            insertIt = mGroups.insert(insertIt, LobbyGroupMap::value_type(group->mId, group));
            if (group->Fn3A9050()) mNumChatClients += group->mNumClients;
            mNumClients += group->mNumClients;
            ++infoIt;
        } else {
            SmartPtr<LobbyGroup> group = groupIt->second;
            group->Fn3A8C30(infoIt->second.get(), clients);
            if (group->Fn3A9050()) mNumChatClients += group->mNumClients;
            mNumClients += group->mNumClients;
            ++groupIt;
            ++infoIt;
        }
    }
    while (groupIt != mGroups.end()) mGroups.erase(groupIt++);
    while (infoIt != inGroups.end()) {
        SmartPtr<LobbyGroup> group = new LobbyGroup;
        group->Fn3A8C30(infoIt->second.get(), clients);
        insertIt = mGroups.insert(insertIt, LobbyGroupMap::value_type(group->GetId(), group));
        if (group->Fn3A9050()) mNumChatClients += group->mNumClients;
        mNumClients += group->mNumClients;
        ++infoIt;
    }
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7a9f50
LobbyClient* LobbyServer::Fn3A9F50(RoutingClientInfo* inInfo)
{
    SmartPtr<LobbyClient> client = new LobbyClient;
    client->Fn3A5350(inInfo);
    client->Fn3A5450();
    mClients->Fn3A5CC0(client.get());
    return client.get();
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa000
void LobbyServer::Fn3AA000(LobbyClient* inClient, unsigned char inFlags, LobbyGroup* inGroup)
{
    inGroup->Fn3A8E40(inClient, inFlags);
    if (inGroup->Fn3A9050()) ++mNumChatClients;
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa030
LobbyGroup* LobbyServer::Fn3AA030(RoutingGroupInfo* inInfo)
{
    SmartPtr<LobbyGroup> group = new LobbyGroup;
    group->Fn3A8C30(inInfo, mClients.get());
    mGroups[group->GetId()] = group;
    return group.get();
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa170
SmartPtr<LobbyGroup> LobbyServer::Fn3AA170(unsigned short inId)
{
    LobbyGroupMap::iterator it = mGroups.find(inId);
    if (it == mGroups.end()) return NULL;
    SmartPtr<LobbyGroup> group = it->second;
    mGroups.erase(it);
    return group;
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa280
SmartPtr<LobbyClient> LobbyServer::Fn3AA280(unsigned short inId)
{
    return mClients->Fn3A5D90(inId);
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa2b0
SmartPtr<LobbyClient> LobbyServer::Fn3AA2B0(unsigned short inId, LobbyGroup* inGroup)
{
    SmartPtr<LobbyClient> client = inGroup->Fn3A8EB0(inId);
    if (client.get() != NULL && inGroup->Fn3A9050()) {
        --mNumChatClients;
        if (mNumChatClients < 0) mNumChatClients = 0;
    }
    return client;
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa370
LobbyClient* LobbyServer::GetClient(int inId) { return mClients->FindClient(inId); }

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa380
LobbyClient* LobbyServer::Fn3AA380(unsigned long inId) { return mClients->Fn3A5EA0(inId); }

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa390
LobbyClient* LobbyServer::Fn3AA390()
{
    if (mMyClientId == 0xffff) return NULL;
    return GetClient(mMyClientId);
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa3b0
LobbyGroup* LobbyServer::Fn3AA3B0(int inId)
{
    LobbyGroupMap::iterator it = mGroups.find(inId);
    return it != mGroups.end() ? it->second.get() : NULL;
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa410
int LobbyServer::Fn3AA410()
{
    if (Fn3AA430()) return mClients->mClients.size();
    return mNumClients;
}

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa430
bool LobbyServer::Fn3AA430() { return mConnection.get() != NULL; }

// CONTRIBUTOR: social/LobbyServer.windowB.cpp.draft
// FUNCTION: WON 0x7aa440
void LobbyServer::Fn3AA440() { mGroups.clear(); }

// CONTRIBUTOR: social/LobbyServerList.cpp.draft
// FUNCTION: WON 0x7aa4f0
LobbyServerList::LobbyServerList() {}

// CONTRIBUTOR: social/LobbyServerList.cpp.draft
// FUNCTION: WON 0x7aa5a0
LobbyServerList::~LobbyServerList() {}

// CONTRIBUTOR: social/LobbyServerList.cpp.draft
// FUNCTION: WON 0x7aa670
void LobbyServerList::Fn3AA670(const DirEntityList& inEntities)
{
    for (DirEntityList::const_iterator it = inEntities.begin(); it != inEntities.end(); ++it) {
        DirEntity* entity = it->get();
        LobbyServerMap::iterator serverIt = mServers.insert(LobbyServerMap::value_type(entity->GetNetAddrAsIP(), NULL)).first;
        if (serverIt->second.get() == NULL) serverIt->second = new LobbyServer;
        LobbyServer* aServer = serverIt->second.get();
        if (!aServer->Fn3AA430()) aServer->Fn3A9700(entity);
    }
}

// CONTRIBUTOR: social/LobbyServerList.cpp.draft
// FUNCTION: WON 0x7aa980
LobbyServer* LobbyServerList::Fn3AA980(const IPAddr& inAddress)
{
    LobbyServerMap::iterator it = mServers.find(inAddress);
    return it == mServers.end() ? NULL : it->second.get();
}

// OPEN: 0x3a96a0 inherited socket Close receiver remains unresolved; original carved, no substitute.
// OPEN: 0x3a9700 DirEntity metadata decode remains unresolved; original carved, no substitute.

}

// Deferred source: not compiled, no source-body credit.
// BLOCKER: EH debt: candidate3 states vs original4; missing clients cleanup guard during IPAddr destruction. Keep original carved.
// // DEFERRED FUNCTION: EXE 0x7a68f0
// LobbyGame::~LobbyGame() {}




// DEFERRED: Original state=1 store at 0x3a63d0 before IPAddr host release is missing in candidate; state remains too high across delete and may repeat IPAddr destruction on throw. Original remains carved.
// // DEFERRED FUNCTION: EXE 0x7a6380
// LobbyFriend::~LobbyFriend() {}

// DEFERRED: Original state=2 store at 0x3a95a9 before IPAddr host release is missing in candidate; state remains too high across delete and may repeat IPAddr destruction on throw. Original remains carved.
// // DEFERRED FUNCTION: EXE 0x7a9500
// LobbyServer::~LobbyServer() {}


namespace WONAPI {
// Original data4dec84/4dec88 are function pointers, initialized to3a6730/3a5140.
// FUNCTION: WON 0x7a6c90
void LobbyGame::Fn3A6C90()
{
    LobbyClientList* clients = mClients.get();
    if (clients == NULL) return;
    for (LobbyClientMap::iterator it=clients->mClients.begin(); it!=clients->mClients.end(); ++it)
        it->second->Fn3A5520(NULL);
}
// FUNCTION: WON 0x7a6d00
bool LobbyGame::Fn3A6D00(LobbyGame* inOther)
{
    if (inOther->mL2C != mL2C) return false;
    if (mL2C == 2) return inOther->mL62 == mL62;
    return inOther->mAddress == mAddress;
}
// FUNCTION: WON 0x7a6e00
void LobbyGame::Fn3A6E00(LobbyGroup* inGroup)
{
    mName = inGroup->mName;
    mB10 = inGroup->Fn3A9020();
    mB11 = inGroup->Fn3A9040();
    mB12 = inGroup->Fn3A9030();
    mL62 = inGroup->mId;
    mL60 = inGroup->mCaptainId;
    mClients = inGroup->mClients;
}
// FUNCTION: WON 0x7a7200
bool LobbyGame::Fn3A7200(const ByteBuffer* inData)
{
    if (inData == NULL) return false;
    ReadBuffer buffer(inData->data(),inData->length());
    return ReadSummary(buffer);
}
// FUNCTION: WON 0x7a72c0
ConstSmartPtr<ByteBuffer> LobbyGame::Fn3A72C0()
{
    WriteBuffer buffer(0,true);
    buffer.AppendByte(1);
    buffer.AppendShort(mL64);
    Virtual34(buffer);
    return buffer.ToByteBuffer(true);
}
// FUNCTION: WON 0x7a7360
void LobbyGame::Fn3A7360(LobbyClient* inClient)
{
    SmartPtr<LobbyPlayer> player = LobbyPlayer::CreatePlayer();
    inClient->Fn3A5520(player.get());
    Virtual30(inClient);
    mL24 = 1;
}
// FUNCTION: WON 0x7a7400
bool LobbyGame::Fn3A7400()
{
    LobbyClientMap& clients = mClients->mClients;
    for (LobbyClientMap::iterator it=clients.begin(); it!=clients.end(); ++it) {
        LobbyClient* client=it->second.get();
        if (client->Fn3A5580() && !client->Fn3A54E0(1) && !client->Fn3A5590()) return false;
    }
    return true;
}
// FUNCTION: WON 0x7a7490
bool LobbyGame::Fn3A7490()
{
    if (!Fn3A7400()) return false;
    return Virtual24();
}
// FUNCTION: WON 0x7a7570
short LobbyGame::Fn3A7570(ReadBuffer& inBuffer,LobbyClient* inClient,WriteBuffer& outBuffer)
{
    SmartPtr<LobbyPlayer> player;
    if (IsFull()) return 2;
    if (mB13) return 3;
    LobbyConfig* config=GetLobbyConfig();
    if (config!=NULL && !config->mAllowDuplicateNames) {
        for (LobbyClientMap::iterator it=mClients->mClients.begin(); it!=mClients->mClients.end(); ++it)
            if (it->second->GetName()==inClient->GetName() && it->second.get()!=inClient) return 8;
    }
    // LOST-DEBUG: none: exact only (one more free site refuses the second early return ~SmartPtr)
    EE_LOST_TRACE(inClient->GetName());
    player=LobbyPlayer::CreatePlayer();
    inClient->Fn3A5520(player.get());
    try {
        short anId=inBuffer.ReadShort();
        player->mPlayerId=anId;
        short result=Virtual38(inBuffer,inClient);
        if (result!=0) { inClient->Fn3A5520(NULL); return result; }
    } catch (ReadBufferException&) {
        inClient->Fn3A5520(NULL);
        return 4;
    }
    outBuffer.AppendShort(++mL24);
    LobbyClientSetIterator it(mClients.get());
    while (it.HasMore()) {
        LobbyClient* client;
        LobbyPlayer* otherPlayer=it.Next(&client);
        if (otherPlayer!=NULL) {
            outBuffer.AppendShort(client->GetClientId());
            otherPlayer->WriteToBuffer(outBuffer);
        }
    }
    Virtual3C(inClient,outBuffer);
    return 0;
}
// FUNCTION: WON 0x7a7830
ConstSmartPtr<ByteBuffer> LobbyGame::Fn3A7830(ReadBuffer& inBuffer,LobbyClient* inClient)
{
    WriteBuffer buffer(0,true);
    buffer.AppendByte(2);
    buffer.AppendShort(0);
    buffer.SetShort(1,Fn3A7570(inBuffer,inClient,buffer));
    return buffer.ToByteBuffer(true);
}
// FUNCTION: WON 0x7a78e0
short LobbyGame::Fn3A78E0(ReadBuffer& inBuffer)
{
    short result;
    try {
        result=inBuffer.ReadShort();
        if (result!=0) return result;
        unsigned short count=inBuffer.ReadShort();
        for (int i=0; i<count; ++i) {
            LobbyClient* client=FindClient(inBuffer.ReadShort());
            if (client!=NULL) {
                SmartPtr<LobbyPlayer> player=LobbyPlayer::CreatePlayer();
                if (player->ReadFromBuffer(inBuffer)) client->Fn3A5520(player.get());
            }
        }
        Virtual44(inBuffer);
    } catch (ReadBufferException&) { result=4; }
    return result;
}

// FUNCTION: WON 0x7a7c50
LobbyGameList::LobbyGameList(int inType) : mIdItr(NULL),mAddrItr(NULL) { mType=inType; }
// FUNCTION: WON 0x7a7e00
LobbyGameList::~LobbyGameList() {}
// FUNCTION: WON 0x7a7f10
void LobbyGameList::Clear() { mIdGames.clear(); mAddrGames.clear(); }
// FUNCTION: WON 0x7a8030
void LobbyGameList::AddGame(LobbyGame* inGame)
{
    inGame->mL2C=mType;
    if (mType==2) {
        unsigned short id = inGame->mL62;
        mIdGames[id]=inGame;
    }
    else mAddrGames[inGame->mAddress]=inGame;
}
// FUNCTION: WON 0x7a80f0
LobbyGame* LobbyGameList::GetOrCreateGame(unsigned short inId,bool* outExisted)
{
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(5);
    LobbyIdGameMap::iterator it=mIdGames.insert(LobbyIdGameMap::value_type(inId,NULL)).first;
    SmartPtr<LobbyGame>& game=it->second;
    if (game.get()==NULL) {
        game=LobbyGame::CreateGame(static_cast<LobbyGameType>(mType));
        game->mL62=inId;
        if (outExisted!=NULL) *outExisted=false;
    } else if (outExisted!=NULL) *outExisted=true;
    return game.get();
}
// FUNCTION: WON 0x7a8290
LobbyGame* LobbyGameList::GetOrCreateGame(const IPAddr& inAddress,bool* outExisted)
{
    LobbyAddrGameMap::iterator it=mAddrGames.insert(LobbyAddrGameMap::value_type(inAddress,NULL)).first;
    SmartPtr<LobbyGame>& game=it->second;
    if (game.get()==NULL) {
        game=LobbyGame::CreateGame(static_cast<LobbyGameType>(mType));
        game->mAddress=inAddress;
        if (outExisted!=NULL) *outExisted=false;
    } else if (outExisted!=NULL) *outExisted=true;
    return game.get();
}
// FUNCTION: WON 0x7a8560
LobbyGame* LobbyGameList::GetGame(unsigned short inId)
{
    LobbyIdGameMap::iterator it=mIdGames.find(inId);
    return it!=mIdGames.end()?it->second.get():NULL;
}
// FUNCTION: WON 0x7a85c0
LobbyGame* LobbyGameList::GetGame(const IPAddr& inAddress)
{
    LobbyAddrGameMap::iterator it=mAddrGames.find(inAddress);
    return it!=mAddrGames.end()?it->second.get():NULL;
}
// FUNCTION: WON 0x7a8620
SmartPtr<LobbyGame> LobbyGameList::RemoveGame(unsigned short inId)
{
    LobbyIdGameMap::iterator it=mIdGames.find(inId);
    if (it==mIdGames.end()) return NULL;
    SmartPtr<LobbyGame> game=it->second;
    game->mL64=0xfffe;
    mIdGames.erase(it);
    return game;
}
// FUNCTION: WON 0x7a8730
SmartPtr<LobbyGame> LobbyGameList::RemoveGame(const IPAddr& inAddress)
{
    LobbyAddrGameMap::iterator it=mAddrGames.find(inAddress);
    if (it==mAddrGames.end()) return NULL;
    SmartPtr<LobbyGame> game=it->second;
    game->mL64=0xfffe;
    mAddrGames.erase(it);
    return game;
}
// FUNCTION: WON 0x7a8850
void LobbyGameList::Rewind()
{
    if (mType==2) mIdItr=mIdGames.begin();
    else mAddrItr=mAddrGames.begin();
}
// FUNCTION: WON 0x7a8870
bool LobbyGameList::HasMore()
{
    if (mType==2) return mIdItr!=mIdGames.end();
    return mAddrItr!=mAddrGames.end();
}
// FUNCTION: WON 0x7a88a0
LobbyGame* LobbyGameList::Next()
{
    if (mType==2) return (mIdItr++)->second.get();
    return (mAddrItr++)->second.get();
}
// FUNCTION: WON 0x7a8960
bool LobbyGameList::IsEmpty()
{
    if (mType==2) return mIdGames.empty();
    return mAddrGames.empty();
}
// FUNCTION: WON 0x7a8980
bool LobbyGameList::RemoveStaleGames(long inMaxAge)
{
    if (mType==2) return false;
    time_t now=time(NULL);
    unsigned long oldSize=mAddrGames.size();
    LobbyAddrGameMap::iterator it=mAddrGames.begin();
    while (it!=mAddrGames.end()) {
        if (now-it->second->mL1C>inMaxAge) {
            it->second->mL64=0xfffe;
            mAddrGames.erase(it++);
        } else ++it;
    }
    return mAddrGames.size()<oldSize;
}
}
namespace WONAPI {
// FUNCTION: WON 0x7a68f0
LobbyGame::~LobbyGame() {}
// FUNCTION: WON 0x7a9500
LobbyServer::~LobbyServer() {}

}

namespace WONAPI {
// The stored count is little-endian, as is the host.
static unsigned short ShortFromLittleEndian(unsigned short theShort);
// FUNCTION: WON 0x007a9700
void LobbyServer::Fn3A9700(DirEntity* inEntity)
{
    mName = GUIString(inEntity->mDisplayName);
    mAddress = inEntity->GetNetAddrAsIP();
    const DirDataObjectList& aList = inEntity->mDataObjects;
    if (!aList.empty())
    for (DirDataObjectList::const_iterator anItr = aList.begin(); anItr != aList.end(); ++anItr) {
        const DirDataObject& anObject = *anItr;
        if (anObject.mDataType == "_C" && anObject.mData.get() != NULL &&
            anObject.mData->length() == 2) {
            unsigned short aCount;
            memcpy(&aCount,anObject.mData->data(),2);
            mNumClients = ShortFromLittleEndian(aCount);
        } else if (anObject.mDataType == "_L") {
            mAddresses.clear();
            const ByteBuffer* aData = anObject.mData.get();
            std::string aText(anObject.mData->data(),anObject.mData->length());
            StringParser aParser(aText.c_str());
            std::list<std::string> aNames;
            aParser.ReadValue(aNames);
            for (std::list<std::string>::iterator aName = aNames.begin();
                 aName != aNames.end(); ++aName)
                mAddresses.insert(*aName);
        }
    }
}

// FUNCTION: WON 0x7a9ac0
static unsigned short ShortFromLittleEndian(unsigned short theShort)
{
    return theShort;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007a96a0
void LobbyServer::Fn3A96A0()
{
    if (mConnection.get() != NULL) {
        Fn3A9B60();
        mNumClients = mClients->mClients.size();
        mConnection->QueueClose(-1);
        mConnection = NULL;
    }
    mMyClientId = 0xffff;
    mClients->Fn3A5770();
}
}

namespace WONAPI {
// FUNCTION: WON 0x007a7a90
GUIString LobbyGame::Fn3A7A90(short status)
{
    switch(status) {
    case 0: return ::LobbyGameStatus_Success_String;
    case 2: return ::LobbyGameStatus_GameFull_String;
    case 3: return ::LobbyGameStatus_GameInProgress_String;
    case 4: return ::LobbyGameStatus_UnpackFailure_String;
    case 5: return ::LobbyGameStatus_NotInvited_String;
    case 6: return ::LobbyGameStatus_CaptainRejectedYou_String;
    case 7: return ::LobbyGameStatus_InvalidPassword_String;
    case 8: return ::LobbyGameStatus_DuplicateName_String;
    }
    GUIString custom=Virtual64(status);
    if(custom.length()!=0) return custom;
    char buffer[50];
    sprintf(buffer,"%d",static_cast<int>(status));
    return GUIString(new GUIStringData(buffer));
}
}

namespace WONAPI {
// SUPPORT-ONLY: genuine SDK explicit LobbyClient::IsFriend; shared native physical body, zero new physical/primary credit.
bool LobbyClient::IsFriend()
{
    return mB24;
}
}

namespace WONAPI {
// SUPPORT-ONLY: genuine SDK explicit empty LobbyPlayer lifetime; already shared native7B, zero physical/primary novelty.
LobbyPlayer::~LobbyPlayer() {}
}

namespace WONAPI {
// SUPPORT-ONLY: genuine SDK CPP implementation LobbyGame::Virtual2C, positive native 4B; no primary credit.
// FUNCTION: WON 0x007a7a80
short LobbyGame::Virtual2C()
{
    return 0;
}
}

namespace WONAPI {
// SUPPORT-ONLY: actual SDK public creator3a6c00, existing canonical cdecl API; no new allocation or primary credit.
// FUNCTION: WON 0x007a6c00
LobbyGame* LobbyGame::CreateGame(LobbyGameType inType)
{
    LobbyGame* game=mGameFactory();
    game->mL2C=inType;
    return game;
}
// SUPPORT-ONLY: actual SDK public creator3a90e0, existing canonical cdecl API.
// FUNCTION: WON 0x007a90e0
LobbyPlayer* LobbyPlayer::CreatePlayer()
{
    return mPlayerFactory();
}
}

namespace WONAPI {
// Genuine SDK class-static storage: one mutable pointer per model, no CRT initializer.
LobbyGame::LobbyGameFactory LobbyGame::mGameFactory = LobbyGame::DefaultGameFactory;
LobbyPlayer::LobbyPlayerFactory LobbyPlayer::mPlayerFactory = LobbyPlayer::DefaultPlayerFactory;
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a5240
// (public: virtual void * __thiscall WONAPI::LobbyClient::`scalar deleting destructor'(unsigned int)    [??_GLobbyClient@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007a5680
// (public: virtual void * __thiscall WONAPI::LobbyClientList::`scalar deleting destructor'(unsigned int)    [??_GLobbyClientList@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007a6380
// (public: virtual __thiscall WONAPI::LobbyFriend::~LobbyFriend(void)    [??1LobbyFriend@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007a6730
// (protected: static class WONAPI::LobbyGame * __cdecl WONAPI::LobbyGame::DefaultGameFactory(void)    [?DefaultGameFactory@LobbyGame@WONAPI@@KAPAV12@XZ])
// TEMPLATE: WON 0x007a68d0
// (public: virtual void * __thiscall WONAPI::LobbyGame::`scalar deleting destructor'(unsigned int)    [??_GLobbyGame@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007a7d20
// (public: virtual void * __thiscall WONAPI::LobbyGameList::`scalar deleting destructor'(unsigned int)    [??_GLobbyGameList@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007a8b80
// (public: virtual void * __thiscall WONAPI::LobbyGroup::`scalar deleting destructor'(unsigned int)    [??_GLobbyGroup@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007a92f0
// (public: virtual void * __thiscall WONAPI::LobbyServer::`scalar deleting destructor'(unsigned int)    [??_GLobbyServer@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007a93d0
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<class WONAPI::LobbyGroup>,struct std::less<unsigned short>,class std::allocator<stru)
// TEMPLATE: WON 0x007aa580
// (public: virtual void * __thiscall WONAPI::LobbyServerList::`scalar deleting destructor'(unsigned int)    [??_GLobbyServerList@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a6870
// (public: virtual short __thiscall WONAPI::LobbyGame::Virtual38(class WONAPI::ReadBuffer &,class WONAPI::LobbyClient *)    [?Virtual38@LobbyGame@WONAPI@)
// TEMPLATE: WON 0x007a6890
// (public: virtual class WONAPI::GUIString __thiscall WONAPI::LobbyGame::Virtual64(short)    [?Virtual64@LobbyGame@WONAPI@@UAE?AVGUIString@2@F@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a6360
// (public: virtual void * __thiscall WONAPI::LobbyFriend::`scalar deleting destructor'(unsigned int)    [??_GLobbyFriend@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a5c20
// (public: __thiscall std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >::~pair<unsigned short const ,class WONAPI::Smar)
// TEMPLATE: WON 0x007a9310
// (public: __thiscall std::set<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct WONAPI::StringLessNoCase,cl)
// TEMPLATE: WON 0x007aa8f0
// (public: __thiscall std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyServer> >::~pair<class WONAPI::IPAddr const ,class )

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a7d40
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<class WONAPI::LobbyGame>,struct std::less<unsigned short>,class std::allocator<struc)
// TEMPLATE: WON 0x007a84d0
// (public: __thiscall std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >::~pair<class WONAPI::IPAddr const ,class WO)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007aace0
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >,struct std::_Mapk<u)
// TEMPLATE: WON 0x007ab310
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struct std::_Mapk<uns)
// TEMPLATE: WON 0x007ab990
// (public: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struct st)
// TEMPLATE: WON 0x007ac070
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct std::_Mapk<un)
// TEMPLATE: WON 0x007ac560
// (public: __thiscall std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >::pair<unsigned short const ,class WONAPI::SmartP)
// TEMPLATE: WON 0x007ac5a0
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >,struct std::_Mapk<u)
// TEMPLATE: WON 0x007ac700
// (protected: void __thiscall std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >,struct)
// TEMPLATE: WON 0x007aca40
// (protected: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struct std::_Mapk<)
// TEMPLATE: WON 0x007acd40
// (public: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struct st)
// TEMPLATE: WON 0x007acdf0
// (protected: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struct)
// TEMPLATE: WON 0x007ad0f0
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struct s)
// TEMPLATE: WON 0x007ad1a0
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struc)
// TEMPLATE: WON 0x007ad940
// (protected: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyServer> >,stru)
// TEMPLATE: WON 0x007adc40
// (protected: void __thiscall std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struct s)
// TEMPLATE: WON 0x007adc80
// (protected: void __thiscall std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame)
// TEMPLATE: WON 0x007adcc0
// (protected: struct std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >,struc)
// TEMPLATE: WON 0x007ae260
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<)
// TEMPLATE: WON 0x007ae2a0
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,stru)
// TEMPLATE: WON 0x007ae330
// (protected: void __thiscall std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct )
// TEMPLATE: WON 0x007ae370
// (public: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyServer> >,struct )
// TEMPLATE: WON 0x007ae890
// (protected: void __thiscall std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyServ)
// TEMPLATE: WON 0x007ae8d0
// (public: void __thiscall std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> >)
// TEMPLATE: WON 0x007aa9e0
// (public: class WONAPI::SmartPtr<class WONAPI::LobbyClient> & __thiscall std::map<unsigned short,class WONAPI::SmartPtr<class WONAPI::LobbyClient>,struc)
// TEMPLATE: WON 0x007ab1d0
// (public: class WONAPI::SmartPtr<class WONAPI::LobbyGame> & __thiscall std::map<unsigned short,class WONAPI::SmartPtr<class WONAPI::LobbyGame>,struct st)
// TEMPLATE: WON 0x007ab800
// (public: class WONAPI::SmartPtr<class WONAPI::LobbyGame> & __thiscall std::map<class WONAPI::IPAddr,class WONAPI::SmartPtr<class WONAPI::LobbyGame>,str)
// TEMPLATE: WON 0x007ac610
// (public: struct std::pair<class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >,st)
// TEMPLATE: WON 0x007ac740
// (protected: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >,struct std::_Map)
// TEMPLATE: WON 0x007ad4a0
// (public: struct std::pair<class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,str)
// TEMPLATE: WON 0x007ad590
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct std::_Mapk<un)
// TEMPLATE: WON 0x007ad640
// (protected: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct std::_Mapk)
// TEMPLATE: WON 0x007add10
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struct s)
// TEMPLATE: WON 0x007ae1f0
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<)
// TEMPLATE: WON 0x007ae2f0
// (protected: void __thiscall std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct )
// TEMPLATE: WON 0x007ae930
// (void __cdecl std::_Construct(struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyGame> > *,struct std::pair<class W)
// TEMPLATE: WON 0x007ae9e0
// (void __cdecl std::_Construct(struct std::pair<class WONAPI::IPAddr const ,class WONAPI::SmartPtr<class WONAPI::LobbyServer> > *,struct std::pair<class)
// TEMPLATE: WON 0x007a6880
// (public: virtual void __thiscall WONAPI::LobbyGame::Virtual60(unsigned char,class WONAPI::ReadBuffer &,class WONAPI::LobbyClient *))
// TEMPLATE: WON 0x0076cff0
// (protected: void __thiscall std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct )
// TEMPLATE: WON 0x007aab20
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyClient> >,struct std::_Mapk<u)
// TEMPLATE: WON 0x007abeb0
// (public: class std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<class WONAPI::LobbyGroup> >,struct std::_Mapk<un)
// TEMPLATE: WON 0x007a5140
// (protected: static class WONAPI::LobbyPlayer * __cdecl WONAPI::LobbyPlayer::DefaultPlayerFactory(void))
