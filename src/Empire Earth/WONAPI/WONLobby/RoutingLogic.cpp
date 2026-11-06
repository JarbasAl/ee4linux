// Original-backed reconstruction proposal; normal COFF validation pending.
// Original class/TU identity: phase0.md. Root released the full source cohort.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyDialogs.h"
#include "ee/wonapi/WONLobbyRoutingLogic.h"
extern unsigned short gEmptyWideString[]; // 0x8e60bc, an empty wide string (zero .bss; defined in EEWonNet.cpp)
// Actual RTTI is global RoutingOpCompletion, not WONAPI namespace.
// Generated ctor/support remain zero new-body credit; full metadata required.
class RoutingOpCompletion : public WONAPI::LogicCompletionBase {
public:
    typedef void (WONAPI::RoutingLogic::*Callback)(WONAPI::RoutingOp*, WONAPI::LobbyServer*);
    RoutingOpCompletion(Callback inCallback, WONAPI::RoutingLogic* inLogic)
        : mCallback(inCallback), mLogic(inLogic) {}
    // SUPPORT: EXE 0x007ee8f0 (implicit lifetime support, zero primary credit)
    // Destructor remains implicit:3ee8f0 releases10, no own-vptr store.
    // SUPPORT: EXE 0x007ee8a0 (completion dispatch wrapper, zero primary credit)
    virtual void Run(WONAPI::AsyncOp* inOp)
    {
        if (mLogic->mKilled || inOp->mB39) return;
        WONAPI::RoutingOp* anOp = static_cast<WONAPI::RoutingOp*>(inOp);
        WONAPI::LobbyServer* aServer = mLogic->GetServer(anOp);
        if (aServer != NULL) (mLogic.get()->*mCallback)(anOp, aServer);
    }
    Callback mCallback; //0c
    WONAPI::SmartPtr<WONAPI::RoutingLogic> mLogic; //10
};

namespace WONAPI {












































































// FUNCTION: WON 0x007e7200
void RoutingLogic::InitComponent(ComponentConfig* inConfig)
{
    SafeGetComponent(inConfig, mAddFriendDialog, "AddFriendDialog", __FILE__, 125);
    SafeGetComponent(inConfig, mAdminActionDialog, "AdminActionDialog", __FILE__, 126);
    SafeGetComponent(inConfig, mBadUserDialog, "BadUserDialog", __FILE__, 127);
    SafeGetComponent(inConfig, mCreateRoomDialog, "CreateRoomDialog", __FILE__, 128);
    SafeGetComponent(inConfig, mPasswordDialog, "PasswordDialog", __FILE__, 129);
    SafeGetComponent(inConfig, mRoomDialog, "RoomDialog", __FILE__, 130);
}


// FUNCTION: WON 0x007e6e80
RoutingLogic::RoutingLogic()
{
    mServerList = new LobbyServerList;
    mGameList = new LobbyGameList(2);
    mGameDataType = "G";
    mSuppressIdle = false;
    mIdle = false;
    mHaveGameList = false;
    mWantToBeModerator = false;
    mLastServerRefresh = 0;
}

// FUNCTION: WON 0x007e7070
RoutingLogic::~RoutingLogic() {}

// Actual one-pointer local cleanup, observed hot writes + unwind action.
// SUPPORT 0x007eae90, zero new-primary-body credit. Original identifier provisional.
class PendingServerCleanup {
public:
    PendingServerCleanup(RoutingLogic* inLogic) : mLogic(inLogic) {}
    ~PendingServerCleanup() { mLogic->ClearPendingServer(); }
    RoutingLogic* mLogic;
};

// FUNCTION: WON 0x007eaea0
bool RoutingLogic::Virtual18(LobbyGame* inGame)
{
    if (IsConnecting() || mGameGroup.get() != NULL) return false;
    LobbyServer* aServer = mCurrentServer.get();
    if (aServer == NULL) aServer = GetDefaultServer();
    if (aServer == NULL) return false;
    PendingServerCleanup aCleanup(this);
    if (!ConnectToServer(aServer, false)) return false;
    unsigned long aFlags = 0;
    if (inGame->GetB12()) aFlags = 0x100;
    if (inGame->mB11) aFlags |= 0x80;
    unsigned short aGroupId;
    if (!CreateGroup(mPendingServer.get(), inGame->mName, inGame->mPassword,
                      aFlags, aGroupId)) return false;
    JoinedGroup(aGroupId, true);
    if (GetConnection() == NULL) return false;
    inGame->mL62 = aGroupId;
    aServer->mClients->Fn3A5F10();
    if (mGameGroup.get() != NULL) inGame->SetClients(mGameGroup->mClients.get());
    return true;
}

// FUNCTION: WON 0x007eafd0
bool RoutingLogic::Virtual1C(LobbyGame* inGame)
{
    if (IsConnecting() || mGameGroup.get() != NULL || mCurrentServer.get() == NULL ||
        inGame->mL2C != 2) return false;
    PendingServerCleanup aCleanup(this);
    if (!ConnectToServer(mCurrentServer.get(), false)) return false;
    if (!JoinGroup(mPendingServer.get(), inGame->GetL62())) return false;
    JoinedGroup(inGame->mL62, true);
    if (mGameGroup.get() != NULL) inGame->SetClients(mGameGroup->mClients.get());
    return true;
}

// FUNCTION: WON 0x007eeaa0
void RoutingLogic::ClearPendingServer()
{
    if (mPendingServer.get() != NULL && mPendingServer.get() != mCurrentServer.get())
        mPendingServer->Fn3A96A0();
    mPendingServer = NULL;
}

// FUNCTION: WON 0x007e72b0
int RoutingLogic::GetRoomSpec(unsigned short inGroupId)
{
    if (mChatGroup.get() != NULL && mChatGroup->mId == inGroupId) return 5;
    if (mGameGroup.get() != NULL && mGameGroup->mId == inGroupId) return 6;
    return 0;
}

// FUNCTION: WON 0x007e72f0
int RoutingLogic::GetRoomSpec(LobbyGroup* inGroup, LobbyServer* inServer)
{
    if (inServer == mCurrentServer.get() && inGroup != NULL)
        return GetRoomSpec(inGroup->GetId());
    return 0;
}

// FUNCTION: WON 0x007e9aa0
LobbyClientList* RoutingLogic::GetClientList()
{
    if (mCurrentServer.get() != NULL) return mCurrentServer->mClients.get();
    return NULL;
}

// FUNCTION: WON 0x007ea290
bool RoutingLogic::IsConnected() { return mCurrentServer.get() != NULL; }

// FUNCTION: WON 0x007ea2a0
RoutingConnection* RoutingLogic::GetConnection()
{
    if (mCurrentServer.get() == NULL) return NULL;
    return mCurrentServer->mConnection.get();
}

// FUNCTION: WON 0x007ea2b0
LobbyGroup* RoutingLogic::GetGroup(int inRoomSpec)
{
    if (inRoomSpec & 1) return mChatGroup.get();
    if (inRoomSpec & 2) return mGameGroup.get();
    return NULL;
}

// FUNCTION: WON 0x007ea2d0
LobbyServer* RoutingLogic::GetServer(const IPAddr& inAddress)
{
    return mServerList->Fn3AA980(inAddress);
}

// FUNCTION: WON 0x007ea2e0
LobbyServer* RoutingLogic::GetServer(RoutingOp* inOp)
{
    if (mCurrentServer.get() != NULL &&
        inOp->mConnection.get() == mCurrentServer->mConnection.get())
        return mCurrentServer.get();
    if (mPendingServer.get() != NULL &&
        inOp->mConnection.get() == mPendingServer->mConnection.get())
        return mPendingServer.get();
    return NULL;
}

// FUNCTION: WON 0x007ed1c0
LobbyServer* RoutingLogic::GetDefaultServer()
{
    LobbyConfig* aConfig = GetLobbyConfig();
    if (aConfig == NULL) return NULL;
    LobbyServer* aBest = NULL;
    LobbyServer* aPreferred = NULL;
    const LobbyServerMap& aServers = mServerList->mServers;
    LobbyServerMap::const_iterator anItr = aServers.begin();
    while(anItr != aServers.end()) {
        LobbyServer* aServer = anItr->second.get();
        if (aServer->mName == LobbyFn35DC40()) return aServer;
        if (aBest == NULL || aServer->Fn3AA410() > aBest->Fn3AA410()) aBest = aServer;
        if (aServer->Fn3A9490(aConfig->mSB8)) {
            if (aPreferred == NULL || aServer->Fn3AA410() > aPreferred->Fn3AA410())
                aPreferred = aServer;
        }
        ++anItr;
    }
    return aPreferred != NULL ? aPreferred : aBest;
}

// FUNCTION: WON 0x007ed660
bool RoutingLogic::IsConnecting() { return mPendingServer.get() != NULL; }

// FUNCTION: WON 0x007eeae0
LobbyClient* RoutingLogic::Virtual20()
{
    return mCurrentServer.get() == NULL ? NULL : mCurrentServer->Fn3AA390();
}

// FUNCTION: WON 0x007eeaf0
void RoutingLogic::Virtual24(ByteBuffer* inData)
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection != NULL && mGameGroup.get() != NULL)
        aConnection->SendData(inData, 0, mGameGroup->mCaptainId, NULL);
}

// FUNCTION: WON 0x007eeb20
void RoutingLogic::Virtual28(ByteBuffer* inData)
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection != NULL && mGameGroup.get() != NULL)
        aConnection->SendData(inData, 0, mGameGroup->mId, NULL);
}

// FUNCTION: WON 0x007eeb50
void RoutingLogic::Virtual2C(LobbyClient* inClient, ByteBuffer* inData)
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection != NULL && mGameGroup.get() != NULL)
        aConnection->SendData(inData, 0, inClient->mClientId, NULL);
}

// FUNCTION: WON 0x007eeb80
void RoutingLogic::Virtual30(LobbyGame* inGame)
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection == NULL || mGameGroup.get() == NULL) return;
    WriteBuffer aBuffer(0, true);
    inGame->WriteSummary(aBuffer);
    aConnection->CreateDataObject(true, mGameGroup->mId, mGameDataType,
                                  gEmptyWideString, aBuffer.ToByteBuffer(true).get(), NULL);
}

// FUNCTION: WON 0x007eecc0
void RoutingLogic::Virtual34(LobbyGame*)
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection == NULL || mGameGroup.get() == NULL) return;
    aConnection->ReplaceDataObject(mGameGroup->GetId(), mGameDataType, gEmptyWideString, NULL);
}

// FUNCTION: WON 0x007eb0c0
void RoutingLogic::Virtual38(LobbyClient* inClient, bool inBan)
{
    if (mGameGroup.get() == NULL) return;
    RoutingConnection* aConnection = GetConnection();
    if (aConnection == NULL) return;
    aConnection->BanClient(mGameGroup->GetId(), true, !inBan,
                               gEmptyWideString, inClient->GetClientId(), NULL);
}

// FUNCTION: WON 0x007eb1b0
void RoutingLogic::Virtual3C(bool inClose)
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection == NULL || mGameGroup.get() == NULL) return;
    aConnection->SetGroupFlags(mGameGroup->mId, 2, inClose ? 2 : 0, 0, 0, NULL);
}

// Callback draft requires true shared RoutingOp prefix and actual subclass;
// no allocation/sizeof/RTTI emission of a partial operation is used here.
// FUNCTION: WON 0x007e7b40
void RoutingLogic::MemberFlagsChanged(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingMemberFlagsChangedOp* inOp = static_cast<RoutingMemberFlagsChangedOp*>(inBaseOp);
    int aGroupId = inOp->GetGroupId();
    LobbyGroup* aGroup = inServer->Fn3AA3B0(aGroupId);
    if (aGroup == NULL) return;
    LobbyClient* aClient = aGroup->Fn3A9010(inOp->mClientId);
    if (aClient == NULL) return;
    unsigned char aFlags = inOp->mMemberFlags;
    if (aGroup->Fn3A9060()) aClient->mFlags1D = aFlags;
    else aClient->mFlags1C = aFlags;
    if (inServer != mCurrentServer.get()) return;
    unsigned int aRoomSpec = GetRoomSpec(aGroup, inServer);
    if (aRoomSpec != 0) LobbyFn359100(new ClientChangedEvent(aClient, aRoomSpec, 2), true);
}

// FUNCTION: WON 0x007e7a30
void RoutingLogic::ClientFlagsChanged(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingClientFlagsChangedOp* inOp = static_cast<RoutingClientFlagsChangedOp*>(inBaseOp);
    LobbyClient* aClient = inServer->GetClient(inOp->GetClientId());
    if (aClient == NULL) return;
    aClient->mU10 = inOp->mClientFlags;
    unsigned int aRoomSpec = 4;
    LobbyFn359100(new ClientChangedEvent(aClient, aRoomSpec, 2), true);
    if (!aClient->IsFriend()) return;
    LobbyFriend* aFriend = LobbyFn35D3F0(aClient->GetU14());
    if (aFriend == NULL || aFriend->GetAnonymous() == aClient->Fn3A5470()) return;
    aFriend->Fn3A66F0(aClient->Fn3A5470());
    LobbyFn359100(new FriendChangedEvent(aFriend, 2), true);
}

// FUNCTION: WON 0x007ebc90
bool RoutingLogic::ShouldLocateFriend(LobbyFriend* inFriend, unsigned long inNow)
{
    if (inFriend->mFlags & 4) return false;
    const LobbyServerMap& aServers = mServerList->mServers;
    if (mCurrentServer.get() != NULL && inFriend->mAddress == mCurrentServer->mAddress)
        return false;
    if (aServers.empty()) return false;
    if (aServers.size() == 1 && mCurrentServer.get() != NULL) return false;
    return static_cast<long>(inNow - inFriend->mLastLocate) >= 30 ? true : false;
}

// FUNCTION: WON 0x007ebd10
bool RoutingLogic::AnnounceClientLocation(LobbyClient* inClient, bool inForce,
                                        int inRoomSpec, bool inFindByUserId)
{
    if (mCurrentServer.get() == NULL || inClient == NULL) return false;
    GUIString aChatRoom;
    GUIString aGameRoom;
    if (inFindByUserId && inClient->mU14 != 0) {
        LobbyClient* aClient = mCurrentServer->Fn3AA380(inClient->mU14);
        if (aClient != NULL) inClient = aClient;
        else goto announce;
    }
    if (inClient->Fn3A5330()) {
        int aKey = inClient->GetGroupId1E();
        LobbyGroup* aGroup = mCurrentServer->Fn3AA3B0(aKey);
        if (aGroup != NULL) aChatRoom = aGroup->mName;
    }
    if (inClient->Fn3A5340()) {
        int aKey = inClient->GetGroupId20();
        LobbyGroup* aGroup = mCurrentServer->Fn3AA3B0(aKey);
        if (aGroup != NULL) aGameRoom = aGroup->mName;
    }
announce:
    if (aChatRoom.length() == 0 && aGameRoom.length() == 0 && !inForce) return false;
    LobbyFn359100(new AnnounceClientLocationEvent(inClient, aChatRoom,
                                                       aGameRoom, inRoomSpec | 4), true);
    return true;
}

// FUNCTION: WON 0x007e7940
void RoutingLogic::PeerData(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingPeerDataOp* inOp = static_cast<RoutingPeerDataOp*>(inBaseOp);
    if (inServer != mCurrentServer.get() || mGameGroup.get() == NULL) return;
    LobbyClient* aClient = mGameGroup->Fn3A9010(inOp->GetClientId());
    if (aClient == NULL || aClient->GetClientId() == mCurrentServer->GetMyClientId()) return;
    LobbyFn361E10(inOp->GetData().get(), aClient);
}

// FUNCTION: WON 0x007e8030
void RoutingLogic::ClientJoinedGroup(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingClientJoinedGroupOp* inOp = static_cast<RoutingClientJoinedGroupOp*>(inBaseOp);
    int aKey = inOp->GetGroupId();
    LobbyGroup* aGroup = inServer->Fn3AA3B0(aKey);
    if (aGroup == NULL) return;
    LobbyClient* aClient = inServer->GetClient(inOp->GetClientId());
    if (aClient == NULL) return;
    inServer->Fn3AA000(aClient, inOp->GetMemberFlags(), aGroup);
    unsigned int aRoomSpec = GetRoomSpec(aGroup, inServer);
    if (aRoomSpec != 0)
        LobbyFn359100(new ClientChangedEvent(aClient, aRoomSpec, 0), true);
    LobbyFn359100(new GroupChangedEvent(aGroup, inServer, 2), true);
    UpdateFriend(aClient);
}

// FUNCTION: WON 0x007e8160
void RoutingLogic::ClientLeftGroup(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingClientLeftGroupOp* inOp = static_cast<RoutingClientLeftGroupOp*>(inBaseOp);
    LobbyGroup* aGroup = inServer->Fn3AA3B0(inOp->mGroupId);
    if (aGroup == NULL) return;
    SmartPtr<LobbyClient> aClient = inServer->Fn3AA2B0(inOp->mClientId, aGroup);
    if (aClient.get() == NULL) return;
    unsigned int aRoomSpec = GetRoomSpec(aGroup, inServer);
    if (aRoomSpec != 0) {
        LobbyFn359100(new ClientChangedEvent(aClient.get(), aRoomSpec, 1), true);
        if (aRoomSpec & 2) LobbyFn361E70(aClient.get());
    }
    LobbyFn359100(new GroupChangedEvent(aGroup, inServer, 2), true);
    UpdateFriend(aClient.get());
}
// FUNCTION: WON 0x007e8f80
void RoutingLogic::GroupCreated(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingGroupCreatedOp* inOp = static_cast<RoutingGroupCreatedOp*>(inBaseOp);
    LobbyGroup* aGroup = inServer->Fn3AA030(inOp->GetGroupInfo().get());
    if (aGroup != NULL)
        LobbyFn359100(new GroupChangedEvent(aGroup, inServer, 0), true);
}

// FUNCTION: WON 0x007e9040
void RoutingLogic::GroupDeleted(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingGroupDeletedOp* inOp = static_cast<RoutingGroupDeletedOp*>(inBaseOp);
    SmartPtr<LobbyGroup> aGroup = inServer->Fn3AA170(inOp->GetGroupId());
    if (aGroup.get() != NULL)
        LobbyFn359100(new GroupChangedEvent(aGroup.get(), inServer, 1), true);
}

// FUNCTION: WON 0x007ef0d0
void RoutingLogic::UpdateFriend(LobbyClient* inClient)
{
    if (!inClient->IsFriend()) return;
    LobbyFriend* aFriend = LobbyFn35D3F0(inClient->mU14);
    if (aFriend == NULL) return;
    aFriend->Fn3A66B0(inClient);
    LobbyFn359100(new FriendChangedEvent(aFriend, 2), true);
}

// FUNCTION: WON 0x007ebf20
void RoutingLogic::AnnounceFriendLocation(LobbyFriend* inFriend, int inRoomSpec)
{
    if (inFriend == NULL) return;
    if (mCurrentServer.get() != NULL && inFriend->mAddress == mCurrentServer->mAddress) {
        LobbyClient* aClient = mCurrentServer->Fn3AA380(inFriend->mUserId);
        if (aClient != NULL && AnnounceClientLocation(aClient, false, inRoomSpec, false)) return;
    }
    LobbyFn359100(new AnnounceFriendEvent(inFriend, inRoomSpec, 2), true);
}

// FUNCTION: WON 0x007e7d60
void RoutingLogic::ClientJoinedServer(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingClientJoinedServerOp* inOp = static_cast<RoutingClientJoinedServerOp*>(inBaseOp);
    LobbyClient* aClient = inServer->Fn3A9F50(inOp->mClientInfo.get());
    LobbyFriend* aFriend = LobbyFn35D3F0(aClient->GetU14());
    if (aFriend == NULL) return;
    aClient->mB24 = true;
    aFriend->Fn3A6440(inServer->mAddress, inServer->mName, 0);
    aFriend->Fn3A66F0(aClient->Fn3A5470());
    LobbyFn359100(new FriendChangedEvent(aFriend, 2), true);
    if (inServer != mCurrentServer.get() || aClient->Fn3A5470()) return;
    if (LobbyFn35E330()) LobbyFn35B9A0(::LobbyGlobal_FriendJoined_Sound.get());
    LobbyFn359100(new AnnounceFriendEvent(aFriend, 4, 0), true);
}

// FUNCTION: WON 0x007e7e80
void RoutingLogic::ClientLeftServer(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingClientLeftServerOp* inOp = static_cast<RoutingClientLeftServerOp*>(inBaseOp);
    SmartPtr<LobbyClient> aClient = inServer->Fn3AA280(inOp->GetClientId());
    if (aClient.get() == NULL || !aClient->IsFriend()) return;
    LobbyFriend* aFriend = LobbyFn35D3F0(aClient->mU14);
    if (aFriend == NULL) return;
    aFriend->Fn3A65B0();
    LobbyFn359100(new FriendChangedEvent(aFriend, 2), true);
    if (inServer != mCurrentServer.get() || aClient->Fn3A5470()) return;
    if (LobbyFn35E330()) LobbyFn35B9A0(::LobbyGlobal_FriendLeft_Sound.get());
    LobbyFn359100(new AnnounceFriendEvent(aFriend, 4, 1), true);
}

// FUNCTION: WON 0x007e9100
void RoutingLogic::GroupInvitation(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingGroupInvitationOp* inOp = static_cast<RoutingGroupInvitationOp*>(inBaseOp);
    if (inServer != mCurrentServer.get()) return;
    LobbyClient* aClient = inServer->GetClient(inOp->GetClientId());
    if (aClient == NULL || aClient->GetB22() || aClient->mB23) return;
    LobbyGame* aGame = mGameList->GetGame(inOp->mGroupId);
    if (aGame == NULL) return;
    aGame->mB14 = inOp->mInvited;
    LobbyFn359100(new GameChangedEvent(aGame, 2), true);
    if (LobbyFn35E340()) LobbyFn35B9A0(::LobbyGlobal_AskToJoinRecv_Sound.get());
    LobbyFn359100(new GroupInvitationEvent(4, aGame->mName, aGame->GetL62(),
        aClient, inOp->GetInvited(), GUIString(new GUIStringData(inOp->mComment)), false), true);
}

// FUNCTION: WON 0x007e92d0
void RoutingLogic::GroupJoinAttempt(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingGroupJoinAttemptOp* inOp = static_cast<RoutingGroupJoinAttemptOp*>(inBaseOp);
    if (inServer != mCurrentServer.get() || mGameGroup.get() == NULL ||
        inOp->mGroupId != mGameGroup->mId) return;
    LobbyClient* aClient = inServer->GetClient(inOp->GetClientId());
    if (aClient == NULL || aClient->mB22 || aClient->mB23) return;
    if (LobbyFn35E340()) LobbyFn35B9A0(::LobbyGlobal_AskToJoinRecv_Sound.get());
    LobbyFn359100(new GroupJoinAttemptEvent(6, aClient,
        GUIString(new GUIStringData(inOp->mComment))), true);
}

// FUNCTION: WON 0x007e7640
void RoutingLogic::Reset()
{
    if (mSuppressIdle || !IsConnected()) return;
    DWORD aLastInputTick = GetLobbyWindow()->mLastInputTick;
    if (GetTickCount() - aLastInputTick > 300000) {
        if (mIdle) return;
        RoutingConnection* aConnection = GetConnection();
        if (aConnection != NULL) {
            aConnection->SetClientFlags(8, 8, 0, 0, NULL);
            mIdle = true;
        }
    } else if (mIdle) {
        RoutingConnection* aConnection = GetConnection();
        if (aConnection != NULL) {
            aConnection->SetClientFlags(8, 0, 0, 0, NULL);
            mIdle = false;
        }
    }
}

// FUNCTION: WON 0x007e76c0
void RoutingLogic::PeerChat(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingPeerChatOp* inOp = static_cast<RoutingPeerChatOp*>(inBaseOp);
    if (inServer != mCurrentServer.get() ||
        (mChatGroup.get() == NULL && mGameGroup.get() == NULL) ||
        inOp->mRecipients.empty()) return;
    unsigned short aRecipientId = *inOp->mRecipients.begin();
    LobbyClient* aSender = inServer->GetClient(inOp->mClientId);
    if (aSender == NULL || aSender->GetB22() || aSender->mB23) return;
    LobbyClient* aTarget = NULL;
    unsigned int aRoomSpec = 4;
    if (inOp->GetFlags() & 0x100) {
        if (mGameGroup.get() == NULL) return;
        aRoomSpec = 0x16;
    } else if (aRecipientId < 0x4002) {
        aRoomSpec = GetRoomSpec(aRecipientId);
        if (aRoomSpec == 0) return;
    } else {
        aTarget = inServer->GetClient(aRecipientId);
        if (aTarget == NULL) return;
    }
    if (((inOp->GetFlags() & 4) ? true : false) && aTarget != NULL && aTarget->GetClientId() == inServer->mMyClientId) {
        if (LobbyFn35E330()) LobbyFn35B9A0(::LobbyGlobal_WhisperRecv_Sound.get());
        ChatCommandParser* aParser = LobbyFn35B710();
        if (aParser != NULL) aParser->mInput = aSender->GetName();
    }
    if ((inOp->GetFlags() & 0x10) && LobbyFn35E320())
        LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    LobbyFn359100(new RecvChatEvent(aRoomSpec, inOp->GetFlags(), aSender,
        GUIString(new GUIStringData(inOp->mText)), aTarget), true);
}

// FUNCTION: WON 0x007e8770
void RoutingLogic::ServerAlert(RoutingOp* inBaseOp, LobbyServer*)
{
    RoutingServerAlertOp* inOp = static_cast<RoutingServerAlertOp*>(inBaseOp);
    if (LobbyFn35E320()) LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    LobbyFn359100(new AlertEvent(4, 0, 0,
        GUIString(new GUIStringData(inOp->mText))), true);
}

// FUNCTION: WON 0x007e8890
void RoutingLogic::ServerShutdownStarted(RoutingOp* inBaseOp, LobbyServer*)
{
    RoutingServerShutdownStartedOp* inOp = static_cast<RoutingServerShutdownStartedOp*>(inBaseOp);
    if (LobbyFn35E320()) LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    LobbyFn359100(new AlertEvent(4, 1, inOp->GetSeconds(),
        GUIString(new GUIStringData(inOp->mText))), true);
}

// FUNCTION: WON 0x007e89b0
void RoutingLogic::ServerShutdownAborted(RoutingOp* inBaseOp, LobbyServer*)
{
    RoutingServerShutdownAbortedOp* inOp = static_cast<RoutingServerShutdownAbortedOp*>(inBaseOp);
    if (LobbyFn35E320()) LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    LobbyFn359100(new AlertEvent(4, 2, 0,
        GUIString(new GUIStringData(inOp->mText))), true);
}

// FUNCTION: WON 0x007e8580
void RoutingLogic::YouWereMuted(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingYouWereMutedOp* inOp = static_cast<RoutingYouWereMutedOp*>(inBaseOp);
    int aKind = inOp->GetIsMuted() ? 2 : 3;
    unsigned short aGroupId = inOp->mGroupId;
    GUIString aRoomName;
    int aRoomSpec;
    if (aGroupId == 0xffff) {
        aRoomSpec = 0;
        aRoomName = inServer->mName;
    } else if (mGameGroup.get() != NULL && mGameGroup->mId == aGroupId) {
        aRoomSpec = 2;
        aRoomName = mGameGroup->mName;
    } else if (mChatGroup.get() != NULL && mChatGroup->mId == aGroupId) {
        aRoomSpec = 1;
        aRoomName = mChatGroup->mName;
    } else return;
    LobbyFn359100(new ClientModeratedEvent(inServer->Fn3AA390(), aRoomSpec | 4,
        aRoomName, aKind, inOp->GetDuration(), true,
        GUIString(new GUIStringData(inOp->GetComment()))), true);
}

// FUNCTION: WON 0x007eb1f0
void RoutingLogic::Virtual40(LobbyGame*)
{
    if (mGameGroup.get() == NULL) return;
    if (mChatGroup.get() == NULL) {
        mGameGroup = NULL;
        LeaveServer();
    } else {
        RoutingConnection* aConnection = GetConnection();
        if (aConnection != NULL) aConnection->LeaveGroup(mGameGroup->GetId(), NULL);
        mGameGroup = NULL;
    }
    LobbyFn359100(new LeftRoomEvent(6, false, true), true);
}

// FUNCTION: WON 0x007eeda0
void RoutingLogic::Virtual44()
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection == NULL) return;
    if (mChatGroup.get() != NULL) {
        aConnection->LeaveGroup(mChatGroup->GetId(), NULL);
        mChatGroup = NULL;
        LobbyFn359100(new LeftRoomEvent(5, false, true), true);
    }
    aConnection->SetClientFlags(0, 0, ~0UL, 0xfffbffff, NULL);
}

// FUNCTION: WON 0x007eee90
void RoutingLogic::Virtual48()
{
    RoutingConnection* aConnection = GetConnection();
    if (aConnection != NULL) aConnection->SetClientFlags(0x40, 0x40, 0, 0, NULL);
    LobbyFn361310(false);
    LeaveServer();
    JoinDefaultRoom(true);
}

// FUNCTION: WON 0x007ee950
void RoutingLogic::LeaveServer()
{
    if (mCurrentServer.get() != NULL) {
        mCurrentServer->Fn3A96A0();
        LobbyFn35B960(NULL);
        LobbyFn359100(new LeftRoomEvent(4, false, false), true);
    }
    if (mGameListWaitOp.get() != NULL) mGameListWaitOp->Kill(true);
    mHaveGameList = false;
    mCurrentServer = NULL;
    mChatGroup = NULL;
    mGameList->Clear();
    LobbyFn359100(new SyncGameListEvent(mGameList.get()), true);
    if (mGameGroup.get() != NULL) {
        mGameGroup = NULL;
        LobbyFn361E50(true);
    }
}

// FUNCTION: WON 0x007e7c20
void RoutingLogic::GroupCaptainChanged(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingGroupCaptainChangedOp* inOp = static_cast<RoutingGroupCaptainChangedOp*>(inBaseOp);
    int aKey = inOp->GetGroupId();
    LobbyGroup* aGroup = inServer->Fn3AA3B0(aKey);
    if (aGroup == NULL) return;
    LobbyClient* anOldCaptain = NULL;
    LobbyClient* aNewCaptain = aGroup->Fn3A8F80(inOp->GetCaptainId(), &anOldCaptain);
    if (inServer != mCurrentServer.get()) return;
    unsigned int aRoomSpec = GetRoomSpec(aGroup, inServer);
    if (aRoomSpec == 0) return;
    if (anOldCaptain != NULL)
        LobbyFn359100(new ClientChangedEvent(anOldCaptain, aRoomSpec, 2), true);
    if (aNewCaptain != NULL)
        LobbyFn359100(new ClientChangedEvent(aNewCaptain, aRoomSpec, 2), true);
    if (aGroup == mGameGroup.get() && anOldCaptain != NULL) LobbyFn361E50(true);
}

// FUNCTION: WON 0x007e8d10
void RoutingLogic::DataObjectDeleted(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingDataObjectDeletedOp* inOp = static_cast<RoutingDataObjectDeletedOp*>(inBaseOp);
    if (inServer != mCurrentServer.get() || inOp->mDataType != mGameDataType) return;
    SmartPtr<LobbyGame> aGame = mGameList->RemoveGame(inOp->GetLinkId());
    if (aGame.get() != NULL) LobbyFn359100(new GameChangedEvent(aGame.get(), 1), true);
}

// FUNCTION: WON 0x007e8ad0
void RoutingLogic::DataObjectCreated(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingDataObjectCreatedOp* inOp = static_cast<RoutingDataObjectCreatedOp*>(inBaseOp);
    if (inServer != mCurrentServer.get() || inOp->mDataType != mGameDataType) return;
    int aLinkId = inOp->GetLinkId();
    LobbyGroup* aGroup = inServer->Fn3AA3B0(aLinkId);
    if (aGroup == NULL) return;
    bool wasExisting = false;
    LobbyGame* aGame = mGameList->GetOrCreateGame(aGroup->GetId(), &wasExisting);
    if (!aGame->Fn3A7200(inOp->GetData().get())) {
        SmartPtr<LobbyGame> aHoldGame(aGame);
        mGameList->RemoveGame(aGroup->GetId());
        if (wasExisting) LobbyFn359100(new GameChangedEvent(aGame, 1), true);
        return;
    }
    if (!wasExisting) {
        aGame->Fn3A6E00(aGroup);
        if (mGameGroup.get() != NULL) {
            LobbyGame* aMyGame = GetMyLobbyGame();
            if (aMyGame != NULL && aMyGame->mIsHost && aGame->GetL62() == aMyGame->mL62)
                aGame->mB14 = true;
        }
        if (LobbyFn35E370()) LobbyFn35B900(aGame, false);
    }
    LobbyFn359100(new GameChangedEvent(aGame, wasExisting ? 2 : 0), true);
}

// FUNCTION: WON 0x007e82b0
void RoutingLogic::YouWereBanned(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    RoutingYouWereBannedOp* inOp = static_cast<RoutingYouWereBannedOp*>(inBaseOp);
    if (!inOp->mIsBanned) return;
    int aKind = inOp->mDuration > 0 && inOp->mDuration < 10 ? 1 : 0;
    GUIString aRoomName;
    int aRoomSpec;
    if (inOp->GetGroupId() == 0xffff) {
        aRoomSpec = 0;
        aRoomName = inServer->mName;
    } else if (mGameGroup.get() != NULL && mGameGroup->mId == inOp->mGroupId) {
        aRoomSpec = 2;
        aRoomName = mGameGroup->mName;
    } else if (mChatGroup.get() != NULL && mChatGroup->mId == inOp->mGroupId) {
        aRoomSpec = 1;
        aRoomName = mChatGroup->mName;
    } else return;
    LobbyClient* aMyClient = inServer->Fn3AA390();
    if (aRoomSpec & 2) {
        LobbyFn361E90(aMyClient, !aKind);
        return;
    }
    LobbyFn359100(new ClientModeratedEvent(aMyClient, aRoomSpec | 4, aRoomName,
        aKind, inOp->GetDuration(), true, GUIString(inOp->mComment)), true);
    if (aRoomSpec & 1) {
        if (mGameGroup.get() == NULL) LeaveServer();
        else {
            mChatGroup = NULL;
            LobbyFn359100(new LeftRoomEvent(5, true, false), true);
        }
    } else {
        bool hadGameGroup = mGameGroup.get() != NULL;
        if (hadGameGroup) LobbyFn361310(false);
        LeaveServer();
        if (hadGameGroup) LobbyFn3610B0();
    }
}

// FUNCTION: WON 0x007ed2f0
bool RoutingLogic::JoinDefaultRoom(bool inShowStatus)
{
    LobbyServer* aServer = GetDefaultServer();
    if (aServer == NULL) return false;
    PendingServerCleanup aCleanup(this);
    if (!ConnectToServer(aServer, inShowStatus)) return false;
    LobbyGroup* aBestGroup = NULL;
    LobbyGroupMap::iterator anItr = aServer->mGroups.begin();
    while (anItr != aServer->mGroups.end()) {
        LobbyGroup* aGroup = anItr->second.get();
        if (aGroup->Fn3A9070()) {
            if (aGroup->mName == LobbyFn35DC60()) {
                aBestGroup = aGroup;
                break;
            }
            int aClientCount = aGroup->mNumClients;
            if (aBestGroup == NULL ||
                (aClientCount > static_cast<int>(aBestGroup->mNumClients) &&
                 aClientCount < 50)) aBestGroup = aGroup;
        }
        ++anItr;
    }
    int aGroupId = 0;
    if (aBestGroup != NULL) aGroupId = aBestGroup->GetId();
    if (!JoinGroup(mPendingServer.get(), aGroupId)) return false;
    JoinedGroup(aGroupId, false);
    return true;
}

// FUNCTION: WON 0x007ed160
void RoutingLogic::RefreshGameList()
{
    if (!LobbyFn361130()) return;
    mGameList->Rewind();
    while (mGameList->HasMore()) {
        LobbyGame* aGame = mGameList->Next();
        if (!aGame->mIsFiltered) LobbyFn35B900(aGame, false);
    }
    LobbyFn359120(100058, true);
}

// FUNCTION: WON 0x007eef10
void RoutingLogic::JoinedGroup(unsigned short inGroupId, bool inGameGroup)
{
    LobbyGroup* anOldGroup = inGameGroup ? mGameGroup.get() : mChatGroup.get();
    if (mPendingServer.get() != NULL && mPendingServer.get() != mCurrentServer.get()) {
        LeaveServer();
        mIdle = false;
        mSuppressIdle = false;
        mHaveGameList = false;
        mCurrentServer = mPendingServer.get();
        RoutingConnection* aConnection = GetConnection();
        if (aConnection != NULL)
            aConnection->ReadDataObject(0xffff, mGameDataType, false,
                new RoutingOpCompletion(&RoutingLogic::ReadGameListComplete, this));
    } else if (anOldGroup != NULL && mCurrentServer.get() != NULL &&
               mCurrentServer->mConnection.get() != NULL) {
        mCurrentServer->mConnection->LeaveGroup(anOldGroup->GetId(), NULL);
    }
    if (mCurrentServer.get() == NULL) return;
    LobbyGroup* aGroup = mCurrentServer->Fn3AA3B0(inGroupId);
    if (inGameGroup) mGameGroup = aGroup;
    else mChatGroup = aGroup;
    if (aGroup == NULL) return;
    if (aGroup->Fn3A9070() && mCurrentServer.get() != NULL)
        LobbyFn35DB70(mCurrentServer->mName, aGroup->mName);
    unsigned int aRoomSpec = (inGameGroup ? 2 : 1) | 4;
    LobbyFn359100(new JoinedRoomEvent(mCurrentServer->mName, aGroup->mName,
        aRoomSpec, mCurrentServer->GetMyClientId(), aGroup->mClients.get()), true);
}

// FUNCTION: WON 0x007e8df0
void RoutingLogic::ReadGameListComplete(RoutingOp* inBaseOp, LobbyServer* inServer)
{
    if (inServer != mCurrentServer.get()) return;
    mGameList->Clear();
    bool doAlert = LobbyFn35E370();
    RoutingReadDataObjectOp* inOp = static_cast<RoutingReadDataObjectOp*>(inBaseOp);
    for (RoutingDataObjectList::iterator anItr = inOp->mDataObjects.begin();
         anItr != inOp->mDataObjects.end(); ++anItr) {
        RoutingDataObject* anObject = anItr->get();
        int aKey = anObject->mLinkId;
        LobbyGroup* aGroup = inServer->Fn3AA3B0(aKey);
        if (aGroup == NULL) continue;
        SmartPtr<LobbyGame> aGame = LobbyGame::CreateGame(LobbyGameType_Internet);
        aGame->Fn3A6E00(aGroup);
        if (!aGame->Fn3A7200(anObject->mData.get())) continue;
        mGameList->AddGame(aGame.get());
        if (doAlert) LobbyFn35B900(aGame.get(), false);
    }
    LobbyFn359100(new SyncGameListEvent(mGameList.get()), true);
    mHaveGameList = true;
    if (mGameListWaitOp.get() != NULL) mGameListWaitOp->Fn29F220(0);
}

// FUNCTION: WON 0x007ea630
void RoutingLogic::AcceptClient(AcceptClientEvent* inEvent)
{
    if (mCurrentServer.get() == NULL || mGameGroup.get() == NULL) return;
    RoutingConnection* aConnection = mCurrentServer->mConnection.get();
    if (aConnection == NULL || inEvent->mClient.get() == NULL) return;
    LobbyClient* aClient = mCurrentServer->GetClient(inEvent->mClient->GetClientId());
    if (aClient != inEvent->mClient.get()) return;
    aConnection->AcceptClient(mGameGroup->GetId(), aClient->GetClientId(),
                              inEvent->mAccepted, gEmptyWideString, NULL);
}

// FUNCTION: WON 0x007ec790
void RoutingLogic::RemoveFriend(LobbyFriend* inFriend)
{
    if (inFriend == NULL) return;
    if (mCurrentServer.get() != NULL) {
        LobbyClient* aClient = mCurrentServer->Fn3AA380(inFriend->mUserId);
        if (aClient != NULL) {
            aClient->mB24 = false;
            LobbyFn359100(new ClientChangedEvent(aClient, 4, 2), true);
        }
    }
    LobbyFn359100(new FriendChangedEvent(inFriend, 1), true);
    LobbyFn35D2C0(inFriend->mUserId);
}

// FUNCTION: WON 0x007ec870
void RoutingLogic::FriendAction(FriendActionEvent* inEvent)
{
    LobbyFriend* aFriend = inEvent->mFriend.get();
    if (aFriend == NULL) return;
    int aRoomSpec = inEvent->mValue;
    int anAction = inEvent->mAction;
    if (aRoomSpec == 0) aRoomSpec = 5;
    switch (anAction) {
    case 5:
        if (mCurrentServer.get() != NULL) {
            LobbyClient* aClient = mCurrentServer->Fn3AA380(aFriend->mUserId);
            if (aClient != NULL) {
                Fn3EA490(aClient, aRoomSpec);
                return;
            }
        }
    case 4:
        Fn3EA570(aFriend, aRoomSpec);
        return;
    case 1:
        Fn3EBFD0(aFriend, aRoomSpec);
        return;
    case 2:
        Fn3EC4A0();
        return;
    case 3:
        RemoveFriend(aFriend);
        return;
    }
}

// FUNCTION: WON 0x007eccc0
LobbyGame* RoutingLogic::SelectGame(int inGameType, unsigned short inPingLimit)
{
    unsigned short aPingCeiling = 10000;
    LobbyConfig* aConfig = GetLobbyConfig();
    if (aConfig != NULL) {
        aPingCeiling = aConfig->mPingCeiling;
        if (aPingCeiling == 0 || aPingCeiling > 0xf000) aPingCeiling = 0xf000;
    }
    typedef std::multimap<unsigned short, LobbyGame*> GameScoreMap;
    GameScoreMap aScores;
    mGameList->Rewind();
    while (mGameList->HasMore()) {
        LobbyGame* aGame = mGameList->Next();
        if (aGame->mB10 || aGame->mB12 || !aGame->Fn3A7A20()) continue;
        unsigned short aPing = aGame->mL64;
        if (aPing >= aPingCeiling) {
            if (aPing == 0xfffe) LobbyFn35B900(aGame, false);
            continue;
        }
        int aRank;
        if (aGame->mL30 == inGameType) aRank = 0;
        else if (aGame->mL30 == 0) aRank = 10000;
        else if (aGame->mL30 + 1 == inGameType) aRank = 20000;
        else if (aGame->mL30 - 1 == inGameType) aRank = 30000;
        else continue;
        if (aPing < inPingLimit)
            aScores.insert(GameScoreMap::value_type(aRank + aPing, aGame));
    }
    if (aScores.empty()) return NULL;
    return aScores.begin()->second;
}

// FUNCTION: WON 0x007ecc30
void RoutingLogic::JumpToRoom(JumpToRoomEvent* inEvent)
{
    if (IsConnecting() || mCurrentServer.get() == NULL ||
        mCurrentServer->mAddress != inEvent->mAddress) return;
    LobbyGroup* aGroup = mCurrentServer->Fn3AA3B0(inEvent->mGroupId);
    if (aGroup == NULL) return;
    if (aGroup->Fn3A9060()) {
        LobbyGame* aGame = mGameList->GetGame(inEvent->mGroupId);
        if (aGame != NULL && aGame->mName == inEvent->mRoomName)
            LobbyFn3612D0(aGame);
    } else {
        JoinChatRoom(mCurrentServer.get(), inEvent->mGroupId);
    }
}

// FUNCTION: WON 0x007ecf30
// External resource/status spellings await canonical GUI/Core contracts.
void RoutingLogic::QuickPlay(QuickPlayEvent* inEvent)
{
    if (!LobbyFn361310(true)) return;
    if (mCurrentServer.get() == NULL) JoinDefaultRoom(false);
    if (mCurrentServer.get() == NULL) return;
    if (!mHaveGameList) {
        mGameListWaitOp = new AsyncOp;
        LobbyFn358E70(mGameListWaitOp.get(), ::RoutingLogic_GettingGameListTitle_String,
                      ::RoutingLogic_GettingGameListDetails_String, 1, 0);
        mGameListWaitOp = NULL;
        if (!mHaveGameList) return;
    }
    while (mCurrentServer.get() != NULL) {
        LobbyGame* aGame = SelectGame(inEvent->mGameType, 1000);
        if (aGame != NULL) {
            LobbyFn3612D0(aGame);
            return;
        }
        if (LobbyFn35B940() > 0) {
            SmartPtr<AsyncOp> aWaitOp = new AsyncOp;
            LobbyFn358E70(aWaitOp.get(), ::RoutingLogic_CollectingPingsTitle_String,
                          ::RoutingLogic_CollectingPingsDetails_String, 2, 1000);
            if (aWaitOp->mB39) return;
        } else {
            if (LobbyFn358EF0(::GameStagingLogic_NoQuickPlayGameTitle_String, ::GameStagingLogic_NoQuickPlayGameDetails_String, true) == 100001)
                LobbyFn3612F0();
            return;
        }
    }
}

// FUNCTION: WON 0x007ea740
// Core action APIs and SetChatInputEvent declaration are queued owner contracts.
void RoutingLogic::ClientAction(ClientActionEvent* inEvent)
{
    if (mCurrentServer.get() == NULL) return;
    RoutingConnection* aConnection = mCurrentServer->mConnection.get();
    if (aConnection == NULL) return;
    int aRoomSpec = inEvent->mRoomSpec;
    if (!((aRoomSpec & 4) ? true : false)) return;
    LobbyClient* aClient = inEvent->mClient.get();
    if (aClient == NULL) return;
    switch (inEvent->mAction) {
    case 18:
        Fn3EA490(aClient, aRoomSpec);
        break;
    case 2: case 7: case 8: case 14: case 15:
        Fn3E9440(inEvent->mAction, aRoomSpec, aClient);
        break;
    case 1: {
        if (aClient->mU14 == 0 || aClient->IsFriend() || !CanAddFriend()) return;
        aClient->mB24 = true;
        LobbyFn359100(new ClientChangedEvent(aClient, 4, 2), true);
        LobbyFriend* aFriend = LobbyFn35D460(aClient->mName, aClient->GetU14());
        if (aFriend == NULL) return;
        LobbyClient* aCurrentClient = mCurrentServer->Fn3AA380(aClient->mU14);
        if (aCurrentClient != NULL) {
            aFriend->Fn3A6440(mCurrentServer->mAddress, mCurrentServer->mName, 0);
            aFriend->Fn3A66B0(aCurrentClient);
        }
        LobbyFn359100(new FriendChangedEvent(aFriend, 0), true);
        break;
    }
    case 9: {
        if (!aClient->IsFriend()) return;
        aClient->mB24 = false;
        LobbyFn359100(new ClientChangedEvent(aClient, 4, 2), true);
        LobbyFriend* aFriend = LobbyFn35D3F0(aClient->GetU14());
        if (aFriend != NULL)
            LobbyFn359100(new FriendChangedEvent(aFriend, 1), true);
        LobbyFn35D2C0(aClient->mU14);
        break;
    }
    case 3:
        LobbyFn35B850(aClient, true, 4);
        break;
    case 4:
        mSuppressIdle = false;
        aConnection->SetClientFlags(8, 0, 0, 0, NULL);
        break;
    case 5:
        LobbyFn35B7B0(aClient, true, 4);
        break;
    case 6: {
        GUIString aComment(new GUIStringData(""));
        InviteClient(aClient, true, aComment);
        break;
    }
    case 13: {
        GUIString aComment(new GUIStringData(""));
        InviteClient(aClient, false, aComment);
        break;
    }
    case 10:
        mSuppressIdle = true;
        aConnection->SetClientFlags(8, 8, 0, 0, NULL);
        break;
    case 11:
        LobbyFn35B850(aClient, false, 4);
        break;
    case 12:
        LobbyFn35B7B0(aClient, false, 4);
        break;
    case 16:
        LobbyFn359100(new SetChatInputEvent(2, aClient->mName,
                                            aRoomSpec, false), true);
        break;
    case 17: {
        if (aClient->IsFriend()) {
            LobbyFriend* aFriend = LobbyFn35D3F0(aClient->GetU14());
            if (aFriend != NULL) {
                Fn3EBFD0(aFriend, aRoomSpec);
                return;
            }
        }
        AnnounceClientLocation(aClient, true, aRoomSpec, true);
        break;
    }
    }
}

// FUNCTION: WON 0x007ebb40
bool RoutingLogic::InviteClient(LobbyClient* inClient, bool inInvite,
                                const GUIString& inComment)
{
    if (inClient == NULL || mGameGroup.get() == NULL) return false;
    LobbyGame* aMyGame = GetMyLobbyGame();
    if (aMyGame == NULL || !aMyGame->mIsHost) return false;
    RoutingConnection* aConnection = GetConnection();
    if (aConnection == NULL) return false;
    inClient->mB27 = inInvite;
    aConnection->InviteClient(mGameGroup->GetId(), inClient->GetClientId(), inInvite,
                               inComment, NULL);
    LobbyFn359100(new GroupInvitationEvent(4, mGameGroup->mName,
        mGameGroup->GetId(), inClient, inInvite, inComment, true), true);
    return true;
}

// FUNCTION: WON 0x007eb350
// Native call contracts are in native_call_contracts.txt; no op subclasses allocated.
void RoutingLogic::ChatInput(Component* inTarget)
{
    if (mCurrentServer.get() == NULL) return;
    RoutingConnection* aConnection = mCurrentServer->mConnection.get();
    if (aConnection == NULL) return;
    ChatCommandParser* aParser = LobbyFn35B710();
    if (aParser == NULL || !(aParser->mRoomFlags & 4)) return;
    LobbyGroup* aGroup = GetGroup(aParser->mRoomFlags);
    if (aGroup == NULL) return;
    int aGroupId = aGroup->GetId();
    LobbyClient* aTarget = aParser->mTarget.get();
    switch (aParser->mCommand) {
    case 1: case 3: {
        unsigned short aFlags = aParser->mCommand == 3 ? 2 : 0;
        if (aParser->mRoomFlags & 0x10) {
            LobbyClient* aMyClient = LobbyFn361370();
            if (aMyClient == NULL) return;
            std::list<unsigned short> aRecipients;
            LobbyClientIterator anItr(aParser->mClients.get());
            while (anItr.HasMore()) {
                LobbyClient* aClient = anItr.Next();
                if (aMyClient->Fn3A55C0(aClient))
                    aRecipients.push_back(aClient->GetClientId());
            }
            aConnection->SendChatTo(aParser->mText,
                aFlags | 0x100, aRecipients, NULL);
        } else {
            aConnection->SendChat(aParser->mText, aFlags, aGroupId, NULL);
        }
        break;
    }
    case 2:
        if (aTarget != NULL)
            aConnection->SendChat(aParser->mText, 4, aTarget->mClientId, NULL);
        break;
    case 6: {
        LobbyClient* aMyClient = mCurrentServer->Fn3AA390();
        if (aMyClient == NULL) return;
        mSuppressIdle = !aMyClient->Fn3A54B0();
        aConnection->SetClientFlags(8, mSuppressIdle ? 8 : 0, 0, 0, NULL);
        break;
    }
    case 16: case 17:
        if (aTarget != NULL && !InviteClient(aTarget, aParser->mCommand == 16, aParser->mText))
            LobbyFn359260(new ChatCommandErrorEvent(::RoutingLogic_NotCaptain_String), inTarget);
        break;
    case 7: {
        LobbyClient* aMyClient = mCurrentServer->Fn3AA390();
        if (aMyClient == NULL) return;
        // Original5f is a requested moderator state; prior field label is provisional.
        mWantToBeModerator = !aMyClient->Fn3A54D0();
        aConnection->BecomeModerator(mWantToBeModerator, NULL);
        break;
    }
    case 10:
        if (aTarget != NULL)
            aConnection->MuteClient(aGroupId, true, aParser->mDuration,
                aParser->mText, aTarget->mClientId, NULL);
        break;
    case 8:
        if (aTarget != NULL)
            aConnection->MuteClient(0xffff, true, aParser->mDuration,
                aParser->mText, aTarget->mClientId, NULL);
        break;
    case 11:
        if (aTarget != NULL)
            aConnection->BanClient(aGroupId, true, aParser->mDuration,
                aParser->mText, aTarget->GetClientId(), NULL);
        break;
    case 9:
        if (aTarget != NULL)
            aConnection->BanClient(0xffff, true, aParser->mDuration,
                aParser->mText, aTarget->mClientId, NULL);
        break;
    case 12:
        if (aTarget != NULL)
            aConnection->MuteClient(aGroupId, false, aParser->mDuration,
                aParser->mText, aTarget->GetClientId(), NULL);
        else Fn3E9AB0(true, false, aParser->mRoomFlags);
        break;
    case 13:
        if (aTarget != NULL)
            aConnection->MuteClient(0xffff, false, aParser->mDuration,
                aParser->mText, aTarget->mClientId, NULL);
        else Fn3E9AB0(true, true, aParser->mRoomFlags);
        break;
    case 5:
        if (aTarget != NULL) LobbyFn35B7B0(aTarget, !aTarget->mB22, 4);
        break;
    case 4:
        if (aTarget != NULL) LobbyFn35B850(aTarget, !aTarget->mB23, 4);
        break;
    case 14:
        Fn3E9AB0(false, false, aParser->mRoomFlags);
        break;
    case 15:
        Fn3E9AB0(false, true, aParser->mRoomFlags);
        break;
    case 22: {
        LobbyClient* aMyClient = mCurrentServer->Fn3AA390();
        if (aMyClient != NULL && aMyClient->Fn3A54C0())
            aConnection->SendServerAlert(aParser->mText, NULL);
        break;
    }
    case 23: {
        LobbyClient* aMyClient = mCurrentServer->Fn3AA390();
        if (aMyClient != NULL && aMyClient->Fn3A54C0())
            aConnection->StartServerShutdown(aParser->mText,
                                               aParser->mDuration, NULL);
        break;
    }
    case 24: {
        LobbyClient* aMyClient = mCurrentServer->Fn3AA390();
        if (aMyClient != NULL && aMyClient->Fn3A54C0())
            aConnection->AbortServerShutdown(NULL);
        break;
    }
    case 25:
        if (aTarget != NULL)
            aConnection->SendChat(aParser->mText, 0x10, aTarget->GetClientId(), NULL);
        break;
    }
}

// FUNCTION: WON 0x007e7320
void RoutingLogic::ConnectionClosed(AsyncOp* inBaseOp)
{
    SocketOp* inOp = static_cast<SocketOp*>(inBaseOp);
    // Both actual RefCount subobjects are at0; no socket layout view is used.
    RefCount* aSocket = inOp->mSocket.get();
    RefCount* aConnection = GetConnection();
    if (aSocket != aConnection) return;
    LobbyFn359100(new LeftRoomEvent(4, true, true), true);
    if (mGameGroup.get() != NULL) LobbyFn361E50(false);
    LeaveServer();
    LobbyMessageBox(::RoutingLogic_DisconnectedTitle_String, ::RoutingLogic_DisconnectedDetails_String);
}

// FUNCTION: WON 0x007eac40
void RoutingLogic::CreateRoom(CreateRoomEvent* inEvent)
{
    mCreateRoomDialog->Reset(false);
    if (LobbyDoDialog(mCreateRoomDialog.get()) != 100001 || IsConnecting()) return;
    GUIString aName = mCreateRoomDialog->GetRoomName();
    GUIString aPassword = mCreateRoomDialog->GetPassword();
    if (aName.length() == 0) return;
    LobbyServer* aServer = GetServer(inEvent->mAddress);
    if (aServer == NULL) aServer = mCurrentServer.get();
    if (aServer == NULL) aServer = GetDefaultServer();
    if (aServer == NULL) return;
    PendingServerCleanup aCleanup(this);
    if (!ConnectToServer(aServer, false)) return;
    unsigned short aGroupId;
    if (!CreateGroup(mPendingServer.get(), aName, aPassword, 0x800, aGroupId)) return;
    JoinedGroup(aGroupId, false);
    if (mRoomDialog->GetParent() != NULL)
        GetLobbyWindow()->Fn32DDB0(-1, NULL, mRoomDialog.get());
}

// FUNCTION: WON 0x007ec910
bool RoutingLogic::JoinChatRoom(LobbyServer* inServer, unsigned short inGroupId)
{
    if (inServer == NULL) return false;
    PendingServerCleanup aCleanup(this);
    if (!ConnectToServer(inServer, false)) return false;
    if (mChatGroup.get() != NULL && inServer == mCurrentServer.get() &&
        inGroupId == mChatGroup->GetId()) {
        LobbyMessageBox(::RoutingLogic_AlreadyJoinedTitle_String, ::RoutingLogic_AlreadyInRoom_String);
        return false;
    }
    if (!JoinGroup(mPendingServer.get(), inGroupId)) return false;
    JoinedGroup(inGroupId, false);
    return true;
}

// FUNCTION: WON 0x007ec9e0
void RoutingLogic::JoinRoom(JoinRoomEvent* inEvent)
{
    if (IsConnecting()) return;
    LobbyServer* aServer = GetServer(inEvent->mAddress);
    if (aServer == NULL) return;
    unsigned long aGroupId = inEvent->mGroupId;
    if (aGroupId == 0xffff) aGroupId = 0;
    if (JoinChatRoom(aServer, aGroupId) && mRoomDialog->GetParent() != NULL)
        GetLobbyWindow()->Fn32DDB0(-1, NULL, mRoomDialog.get());
}

}

namespace WONAPI {
// FUNCTION: WON 0x007eca40
void RoutingLogic::JoinFriendsRoom(JoinFriendsRoomEvent* inEvent)
{
    if (IsConnecting()) return;
    LobbyFriend* aFriend = inEvent->mFriend.get();
    if (aFriend == NULL) return;
    IPAddr anAddress = aFriend->mAddress;
    if (!anAddress.IsValid()) return;
    LobbyServer* aServer = GetServer(anAddress);
    if (aServer == NULL) return;
    PendingServerCleanup aCleanup(this);
    if (!ConnectToServer(aServer, false)) return;
    LobbyClient* aClient = aServer->Fn3AA380(aFriend->GetUserId());
    if (aClient == NULL) return;
    unsigned short aGroupId = 0;
    if (aClient->Fn3A5330()) aGroupId = aClient->mGroupId1E;
    if (mChatGroup.get() != NULL && aServer == mCurrentServer.get() &&
        aGroupId == mChatGroup->mId) {
        LobbyMessageBox(::RoutingLogic_AlreadyJoinedTitle_String, ::RoutingLogic_AlreadyInRoom_String);
        return;
    }
    if (!JoinGroup(aServer, aGroupId)) return;
    JoinedGroup(aGroupId, false);
}

// Guard cleanup precedes copied IPAddr destruction on every guarded exit. No
// added server/client/friend SmartPtr hold, native cancel, modal closure or new
// operation allocation appears in the original. Full normal COFF/EH still needed.

}


// New FUNCTION markers intentionally inactive; separate candidate ledger only.
namespace WONAPI {
// FUNCTION: WON 0x007ed670
bool RoutingLogic::CreateGroup(LobbyServer* inServer,
    const GUIString& inName, const GUIString& inPassword,
    int inRoomFlags, unsigned short& outGroupId)
{
    if (inServer == NULL) return false;
    SmartPtr<RoutingConnection> aConnection = inServer->mConnection;
    if (aConnection.get() == NULL) return false;
    SmartPtr<RoutingCreateGroupOp> anOp = new RoutingCreateGroupOp(aConnection.get());
    anOp->SetGroupPassword(inPassword);
    anOp->SetGroupName(inName);
    anOp->SetGroupFlags(inRoomFlags);
    anOp->SetJoinFlags(1);
    anOp->SetLeaveGroupOnCancel(true);
    LobbyFn358E70(anOp.get(), ::RoutingLogic_CreateGroupTitle_String, ::RoutingLogic_CreateGroupDetails_String, 1, 0);
    if (anOp->GetStatus() != 0) return false;
    outGroupId = anOp->GetGroupId();
    return true;
}

// FUNCTION: WON 0x007ed8b0
bool RoutingLogic::JoinGroup(LobbyServer* inServer, int inGroupId)
{
    if (inServer == NULL) return false;
    SmartPtr<RoutingConnection> aConnection = inServer->mConnection;
    if (aConnection.get() == NULL) return false;
    SmartPtr<LobbyGroup> aGroup = inServer->Fn3AA3B0(inGroupId);
    {
        if (aGroup.get() == NULL) {
            LobbyFn358ED0(::RoutingLogic_JoinGroupTitle_String, (WONStatus)-2520);
        } else {
            GUIString aPassword;
            if (aGroup->Fn3A9020()) {
                mPasswordDialog->Reset();
                if (LobbyDoDialog(mPasswordDialog.get()) != 0x186a1) return false;
                aPassword = mPasswordDialog->GetPassword();
            }
            if (aConnection->Fn2B08D0()) {
                SmartPtr<RoutingJoinGroupOp> anOp = new RoutingJoinGroupOp(aConnection.get());
                anOp->SetGroupId(inGroupId);
                anOp->SetGroupPassword(aPassword);
                while (true) {
                    if (aGroup.get() != NULL && aGroup->Fn3A9030())
                        LobbyFn358E70(anOp.get(), ::RoutingLogic_AskingCaptainTitle_String, ::RoutingLogic_AskingCaptainDetails_String, 1, 0);
                    else
                        LobbyFn358E70(anOp.get(), ::RoutingLogic_JoinGroupTitle_String, ::RoutingLogic_JoinGroupDetails_String, 1, 0);
                    if (anOp->GetStatus() == 0) return true;
                    if (!aConnection->Fn2B08D0()) break;
                    if (anOp->mB39) {
                        aConnection->CancelJoinGroup(inGroupId, true, NULL);
                        return false;
                    }
                    if (anOp->GetStatus() != (WONStatus)-2524)
                        return anOp->GetStatus() == (WONStatus)-2504;
                    mPasswordDialog->Reset();
                    if (LobbyDoDialog(mPasswordDialog.get()) != 0x186a1) return false;
                    anOp->SetGroupPassword(mPasswordDialog->GetPassword());
                }
            }
        }
    }
    return false;

}
}


namespace WONAPI {
// FUNCTION: WON 0x007edd70
bool RoutingLogic::ConnectToServer(LobbyServer* inServer, bool inFlag)
{
    if (inServer == NULL) return false;
    mPendingServer = inServer;
    if (inServer == mCurrentServer.get() && inServer->Fn3AA430()) return true;
    if (mGameGroup.get() != NULL && !LobbyFn361310(true)) return false;

    SmartPtr<RoutingConnection> connection = new RoutingConnection;
    inServer->Fn3A9640(connection.get());
    AuthContext* auth = GetLobbyAuthContext();
    if (auth == NULL) return false;
    connection->Fn2AB0A0(auth, AUTH_TYPE_PERSISTENT); // native immediate2
    connection->mCompletions[8] = new ::RoutingOpCompletion(&RoutingLogic::ClientJoinedServer, this);
    connection->mCompletions[10] = new ::RoutingOpCompletion(&RoutingLogic::ClientLeftServer, this);
    connection->mCompletions[7] = new ::RoutingOpCompletion(&RoutingLogic::ClientJoinedGroup, this);
    connection->mCompletions[9] = new ::RoutingOpCompletion(&RoutingLogic::ClientLeftGroup, this);
    connection->mCompletions[39] = new ::RoutingOpCompletion(&RoutingLogic::PeerChat, this);
    connection->mCompletions[40] = new ::RoutingOpCompletion(&RoutingLogic::PeerData, this);
    connection->mCompletions[6] = new ::RoutingOpCompletion(&RoutingLogic::ClientFlagsChanged, this);
    connection->mCompletions[36] = new ::RoutingOpCompletion(&RoutingLogic::MemberFlagsChanged, this);
    connection->mCompletions[24] = new ::RoutingOpCompletion(&RoutingLogic::GroupCreated, this);
    connection->mCompletions[25] = new ::RoutingOpCompletion(&RoutingLogic::GroupDeleted, this);
    connection->mCompletions[23] = new ::RoutingOpCompletion(&RoutingLogic::GroupCaptainChanged, this);
    connection->mCompletions[13] = new ::RoutingOpCompletion(&RoutingLogic::DataObjectCreated, this);
    connection->mCompletions[14] = new ::RoutingOpCompletion(&RoutingLogic::DataObjectDeleted, this);
    connection->mCompletions[27] = new ::RoutingOpCompletion(&RoutingLogic::GroupInvitation, this);
    connection->mCompletions[28] = new ::RoutingOpCompletion(&RoutingLogic::GroupJoinAttempt, this);
    connection->mCompletions[58] = new ::RoutingOpCompletion(&RoutingLogic::YouWereBanned, this);
    connection->mCompletions[59] = new ::RoutingOpCompletion(&RoutingLogic::YouWereMuted, this);
    connection->mCompletions[48] = new ::RoutingOpCompletion(&RoutingLogic::ServerAlert, this);
    connection->mCompletions[50] = new ::RoutingOpCompletion(&RoutingLogic::ServerShutdownStarted, this);
    connection->mCompletions[49] = new ::RoutingOpCompletion(&RoutingLogic::ServerShutdownAborted, this);

    SmartPtr<ServerConnectionConnectOp> connectOp =
        new ServerConnectionConnectOp(connection.get(), inServer->mAddress);
    LobbyFn358E70(connectOp.get(), ::RoutingLogic_ConnectTitle_String, ::RoutingLogic_ConnectDetails_String, 1, 0);
    if (connectOp->mStatus != 0) return false;

    unsigned long registerFlags = mWantToBeModerator ? 0x1d : 0x15;
    SmartPtr<RoutingRegisterClientOp> registerOp = new RoutingRegisterClientOp(connection.get());
    registerOp->mRegisterFlags = registerFlags;
    registerOp->mReconnectId = LobbyFn35DB00(inServer->mAddress);
    unsigned long clientFlags = 0;
    if (!LobbyFn35E2F0()) clientFlags = 4;
    if (LobbyFn35E390() || inFlag) clientFlags |= 0x40;
    registerOp->mClientFlags = clientFlags;
    LobbyFn358E70(registerOp.get(), ::RoutingLogic_RegisterTitle_String, ::RoutingLogic_RegisterDetails_String, 1, 0);
    if (registerOp->GetStatus() != 0) return false;

    LobbyFn35D940(inServer->mAddress, registerOp->mReconnectId);
    inServer->Fn3A9AD0(registerOp->mClientMap);
    inServer->mMyClientId = registerOp->mClientId;
    connection->SetCloseCompletion(new LogicCompletion<RoutingLogic>(&RoutingLogic::ConnectionClosed, this));
    if (inFlag && !LobbyFn35E390()) connection->SetClientFlags(0x40, 0, 0, 0, NULL);

    SmartPtr<RoutingGetGroupListOp> groupOp = new RoutingGetGroupListOp(connection.get(), 0xff);
    LobbyFn358E70(groupOp.get(), ::RoutingLogic_QueryGroupsTitle_String, ::RoutingLogic_QueryGroupsDetails_String, 1, 0);
    if (groupOp->GetStatus() != 0) return false;
    inServer->Fn3A9BE0(groupOp->mGroupMap, true);
    LobbyFn359100(new ServerChangedEvent(inServer, 2), true);
    inServer->Fn3A9AF0();
    LobbyFn359120(100056, true);
    return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007ea490
void RoutingLogic::Fn3EA490(LobbyClient* inClient,int inRoomSpec)
{
    if(mCurrentServer.get()==NULL || inClient==NULL) return;
    LobbyClient* myClient=mCurrentServer->Fn3AA390();
    if(myClient==NULL) return;
    SmartPtr<ClientOptionsPopup> popup=new ClientOptionsPopup;
    popup->Init(inClient,myClient,inRoomSpec);
    LobbyFn358DF0(popup.get(),9191,-1,-1);
}
}

// BEGIN DEFERRED ORIGINAL BODY TEXT (ZERO CREDIT)
// Complete heavy-body source retained as comments; zero primary credit.
// Not included by the pilot and not eligible for production allocation yet.
// Requires full actual RoutingCreateGroupOp98 canonical declaration, hierarchy,
// virtual payload, ctor and typed normal/EH lifetime approval from root/model.
// Member names below match pinned nominal header and original target offsets.
// Full target: next_ready_original.txt3ed670..3ed8a1 (561B).

// bool RoutingLogic::CreateGroup(LobbyServer* inServer,
//     const GUIString& inName, const GUIString& inPassword,
//     int inRoomFlags, unsigned short& outGroupId)
// {
//     if (inServer == NULL) return false;
//     SmartPtr<RoutingConnection> aConnection = inServer->mConnection;
//     if (aConnection.get() == NULL) return false;
//     SmartPtr<RoutingCreateGroupOp> anOp = new RoutingCreateGroupOp(aConnection.get());
//     anOp->mGroupPassword = (std::wstring)inPassword; //78, temporary ends first
//     anOp->mGroupName = (std::wstring)inName; //68, separate temporary
//     anOp->mGroupFlags = inRoomFlags; //8c fullDWORD3ed794
//     anOp->mJoinFlags = 1; //8a byte3ed7a8; nominal BYTE, not bool
//     anOp->mLeaveGroupOnCancel = true; //943ed7b3
//     LobbyFn358E70(anOp.get(), ::RoutingLogic_CreateGroupTitle_String, ::RoutingLogic_CreateGroupDetails_String, 1, 0);
//     if (anOp->mStatus != 0) return false;
//     outGroupId = anOp->mGroupId; //64 before releasing op then connection
//     return true;
// }

// Original status runner mode1/timeout0; no inline wait or synthetic error path.
// Native null-new behavior is governed by the original compiler/toolchain.
// Actual full-TU COFF must validate the allocation cleanup state, both string
// temporaries, status-path duplication and release order; this text grants none.

// Full target3ed8b0..3edd6e (1214B). Same allocation gate for complete98 op34.
// Requires canonical PasswordDialog::Reset/GetPassword, actual socket bool
// getter2b08d0 and CancelJoinGroup completion-pointer correction. Address getter
// spelling Fn2B08D0 is a pending owner contract, not a parallel local declaration.
// 358ed0 void(title,WONStatus) independently proved by Dialogs owner.

// bool RoutingLogic::JoinGroup(LobbyServer* inServer, int inGroupId)
// {
//     if (inServer == NULL) return false;
//     SmartPtr<RoutingConnection> aConnection = inServer->mConnection;
//     if (aConnection.get() == NULL) return false;
//     SmartPtr<LobbyGroup> aGroup = inServer->Fn3AA3B0(inGroupId);
//     if (aGroup.get() == NULL) {
//         LobbyFn358ED0(::RoutingLogic_JoinGroupTitle_String, (WONStatus)-2520);
//         return false;
//     }
//     GUIString aPassword;
//     if (aGroup->Fn3A9020()) {
//         mPasswordDialog->Reset();
//         if (LobbyDoDialog(mPasswordDialog.get()) != 0x186a1) return false;
//         aPassword = mPasswordDialog->GetPassword();
//     }
//     if (!aConnection->Fn2B08D0()) return false;
//     SmartPtr<RoutingJoinGroupOp> anOp = new RoutingJoinGroupOp(aConnection.get());
//     anOp->mGroupId = inGroupId;
//     anOp->mGroupPassword = (std::wstring)aPassword;
//     for (;;) {
//         if (aGroup.get() != NULL && aGroup->Fn3A9030())
//             LobbyFn358E70(anOp.get(), ::RoutingLogic_AskingCaptainTitle_String, ::RoutingLogic_AskingCaptainDetails_String, 1, 0);
//         else
//             LobbyFn358E70(anOp.get(), ::RoutingLogic_JoinGroupTitle_String, ::RoutingLogic_JoinGroupDetails_String, 1, 0);
//         if (anOp->mStatus == 0) return true;
//         if (!aConnection->Fn2B08D0()) return false;
//         if (anOp->mB39) {
//             aConnection->CancelJoinGroup(inGroupId, true, NULL);
//             return false;
//         }
//         if (anOp->mStatus != (WONStatus)-2524)
//             return anOp->mStatus == (WONStatus)-2504;
//         mPasswordDialog->Reset();
//         if (LobbyDoDialog(mPasswordDialog.get()) != 0x186a1) return false;
//         anOp->mGroupPassword = (std::wstring)mPasswordDialog->GetPassword();
//     }
// }

// Password retry reuses the SAME held operation and returns directly to status
// execution; no extra pre-run socket test, new op, changed password local or
// synthetic rollback is introduced. Held locals die op/password/group/connection.
// Status-2504 returns true; cancellation sends CancelJoinGroup only while live.
// END DEFERRED ORIGINAL BODY TEXT

// Complete R5 admin/modal source; native source behavior, no runtime authority.
namespace WONAPI {
// FUNCTION: WON 0x007e9440
void RoutingLogic::Fn3E9440(int inAction, int inRoomSpec, LobbyClient* inClient)
{
    RoutingConnection* connection = GetConnection();
    if (connection == NULL) return;
    LobbyClient* myClient = mCurrentServer->Fn3AA390();
    if (myClient == NULL) return;
    bool showDialog = myClient->Fn3A54D0() || myClient->Fn3A54C0();
    if (showDialog) {
        mAdminActionDialog->Reset(inAction, inClient);
        if (LobbyDoDialog(mAdminActionDialog.get()) != 100001) return;
        connection = GetConnection();
        if (connection == NULL) return;
    }
    LobbyClient* currentClient = mCurrentServer->Fn3AA380(inClient->GetU14());
    int groupId = 0xffff;
    if (inRoomSpec & 2) {
        if (mGameGroup.get() == NULL) return;
        groupId = mGameGroup->GetId();
    } else if (inRoomSpec & 1) {
        if (mChatGroup.get() == NULL) return;
        groupId = mChatGroup->mId;
    }
    GUIString explanation;
    unsigned long seconds;
    if (showDialog) {
        if (mAdminActionDialog->GetApplyToServer()) groupId = 0xffff;
        else if (groupId == 0xffff && inAction != 15) return;
        explanation = mAdminActionDialog->GetExplanation();
        seconds = mAdminActionDialog->GetNumSeconds();
    } else {
        if (groupId == 0xffff) return;
        seconds = inAction == 7;
    }
    SmartPtr<RoutingOp> op;
    GUIString title;
    switch (inAction) {
    case 2:
    case 7: {
        if (!showDialog && (inRoomSpec & 2)) {
            LobbyFn361BC0(inClient, inAction == 2);
            return;
        }
        SmartPtr<RoutingBanClientOp> ban = new RoutingBanClientOp(connection);
        if (inAction == 2) {
            ban->mBanComment = explanation;
            ban->mBanTime = seconds;
            title = ::RoutingLogic_BanClientTitle_String;
        } else if (inAction == 7) {
            ban->mBanTime = 1;
            title = ::RoutingLogic_KickClientTitle_String;
        }
        ban->mGroupId = groupId;
        op = ban.get();
        if (currentClient != NULL) {
            ban->mClientId = currentClient->GetClientId();
            ban->mBanByWONId = false;
        } else {
            unsigned long id = inClient->mU18;
            if (id == 0) id = inClient->GetU14();
            if (id != 0) {
                ban->mWONId = id;
                ban->mBanByWONId = true;
                ban->mWONIdIsCDKeyId = true;
            }
        }
        break;
    }
    case 8:
    case 14: {
        SmartPtr<RoutingMuteClientOp> mute = new RoutingMuteClientOp(connection);
        mute->mMuteComment = explanation;
        mute->mMuteTime = seconds;
        mute->mGroupId = groupId;
        op = mute.get();
        if (currentClient != NULL) {
            mute->mClientId = currentClient->GetClientId();
            mute->mMuteByWONId = false;
        } else {
            unsigned long id = inClient->mU18;
            if (id == 0) id = inClient->mU14;
            if (id != 0) {
                mute->mWONId = id;
                mute->mMuteByWONId = true;
                mute->mWONIdIsCDKeyId = true;
            }
        }
        if (inAction == 14) {
            title = ::RoutingLogic_UnmuteClientTitle_String;
            mute->mIsMuted = false;
        } else {
            title = ::RoutingLogic_MuteClientTitle_String;
            mute->mIsMuted = true;
        }
        break;
    }
    case 15:
        if (currentClient != NULL)
            connection->SendChat(explanation, 0x10, currentClient->mClientId, NULL);
        return;
    }
    if (op.get() == NULL) return;
    if (showDialog) LobbyFn358E70(op.get(), title, ::RoutingLogic_ModerateDetails_String, 1, 0);
    else RunOp(op.get(), NULL, 0);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e9ab0
void RoutingLogic::Fn3E9AB0(bool inMuted, bool inServerWide, int inRoomSpec)
{
    RoutingConnection* connection = GetConnection();
    if (connection == NULL) return;
    LobbyClient* myClient = mCurrentServer->Fn3AA390();
    if (myClient == NULL) return;
    bool moderator = myClient->Fn3A54D0() || myClient->Fn3A54C0();
    RoutingBadUserListType listType = inMuted ? RoutingBadUserListType_MutedUsers
                                             : RoutingBadUserListType_BannedUsers;
    unsigned short groupId = 0xffff;
    if (!inServerWide) {
        LobbyGroup* group = GetGroup(inRoomSpec);
        if (group == NULL) return;
        groupId = group->mId;
    }
    SmartPtr<RoutingGetBadUserListOp> op =
        new RoutingGetBadUserListOp(connection, listType, groupId, 0x0f);
    LobbyFn358E70(op.get(), ::RoutingLogic_GetBadUsersTitle_String, ::RoutingLogic_GetBadUsersDetails_String, 1, 0);
    if (op->mStatus != 0) return;
    mBadUserDialog->SetUserList(op->mBadUserList, !moderator);
    if (LobbyDoDialog(mBadUserDialog.get()) != 100001) return;
    connection = GetConnection();
    if (connection == NULL) return;

    std::list<unsigned long> ids;
    mBadUserDialog->GetIds(ids);
    if (ids.empty()) return;
    std::list<unsigned long>::iterator i = ids.begin();
    while(i != ids.end()) {
        if (listType == RoutingBadUserListType_BannedUsers)
            connection->BanClientByWONId(groupId, false, 0, std::wstring(gEmptyWideString), *i, NULL);
        else if (listType == RoutingBadUserListType_MutedUsers)
            connection->MuteClientByWONId(groupId, false, 0, std::wstring(gEmptyWideString), *i, NULL);
        ++i;
    }
}
}

namespace WONAPI {
// Native3ebfd0..3ec1de hot526. Iterator advances after RunOp, not before callback.
// FUNCTION: WON 0x007ebfd0
void RoutingLogic::Fn3EBFD0(LobbyFriend* inFriend,int inRoomSpec)
{
    if(inFriend==NULL) return;
    if(!ShouldLocateFriend(inFriend,(unsigned long)time(NULL))) {
        if(!(inFriend->mFlags&4)) AnnounceFriendLocation(inFriend,inRoomSpec);
        return;
    }
    UDPManager* manager=LobbyFn35B8F0();
    if(manager==NULL) return;
    int count=0;
    LobbyServerList* servers=mServerList.get();
    for(LobbyServerMap::iterator it=servers->mServers.begin();it!=servers->mServers.end();++it) {
        if(mCurrentServer.get()!=NULL && mCurrentServer->mAddress==it->first) continue;
        SmartPtr<RoutingGetUserStatusOp> op=new RoutingGetUserStatusOp(it->first,manager);
        op->AddWONUserId(inFriend->GetUserId());
        RunOp(op.get(),new LogicCompletion<RoutingLogic>(&RoutingLogic::GetUserStatusComplete,this),10000);
        ++count;
    }
    if(count>0) {
        inFriend->Fn3A6700(inRoomSpec);
        inFriend->mLastSearch=count;
        LobbyFn359100(new FriendChangedEvent(inFriend,2),true);
    }
}

// Native3e7460..3e7632 hot466. Completion receives AsyncOp*, not RoutingOp*.
// FUNCTION: WON 0x007e7460
void RoutingLogic::GetUserStatusComplete(AsyncOp* inBaseOp)
{
    RoutingGetUserStatusOp* op=static_cast<RoutingGetUserStatusOp*>(inBaseOp);
    LobbyServer* server=mServerList->Fn3AA980(op->mAddress);
    unsigned long now=(unsigned long)time(NULL);
    SmartPtr<LobbyFriend> lastChanged;
    int changed=0;
    for(RoutingGetUserStatusOp::FriendStatusMap::iterator it=op->mFriendStatusMap.begin();
        it!=op->mFriendStatusMap.end();++it) {
        LobbyFriend* friendInfo=LobbyFn35D3F0(it->first);
        if(friendInfo==NULL || !(friendInfo->mFlags&4)) continue;
        bool found=false;
        if(it->second!=0 && server!=NULL) {
            friendInfo->Fn3A6440(server->mAddress,server->mName,it->second);
            found=true;
        }
        --friendInfo->mLastSearch;
        int remaining=friendInfo->mLastSearch;
        if(remaining>0 && !found) continue;
        lastChanged=friendInfo;
        ++changed;
        friendInfo->Fn3A6710(now);
        if(friendInfo->GetSearchStart()!=0)
            LobbyFn359100(new AnnounceFriendEvent(friendInfo,friendInfo->mSearchStart,2),true);
    }
    if(changed>1) LobbyFn359120(100056,true);
    else if(changed==1 && lastChanged.get()!=NULL)
        LobbyFn359100(new FriendChangedEvent(lastChanged.get(),2),true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007ec4a0
void RoutingLogic::Fn3EC4A0()
{
    UDPManager* manager = LobbyFn35B8F0();
    if (manager == NULL) return;
    unsigned long now = (unsigned long)time(NULL);
    const LobbyFriendMap& friends = LobbyFn35D270();
    LobbyServerList* servers = mServerList.get();
    int count = 0;
    for (LobbyServerMap::iterator scanServer = servers->mServers.begin();
         scanServer != servers->mServers.end(); ++scanServer) {
        if (mCurrentServer.get() != NULL && mCurrentServer->mAddress == scanServer->first)
            continue;
        ++count;
    }
    if (count == 0) return;
    RoutingGetUserStatusOp::FriendStatusMap requested;
    LobbyFriendMap::const_iterator friendItr = friends.begin();
    while(friendItr != friends.end()) {
        LobbyFriend* friendInfo = friendItr->second.get();
        if (ShouldLocateFriend(friendInfo,now)) {
            friendInfo->Fn3A6700(0);
            friendInfo->mLastSearch = count;
            requested[friendInfo->GetUserId()] = 0;
        }
        ++friendItr;
    }
    if (requested.empty()) return;
    for (LobbyServerMap::iterator queryServer = servers->mServers.begin();
         queryServer != servers->mServers.end(); ++queryServer) {
        if (mCurrentServer.get() != NULL && mCurrentServer->mAddress == queryServer->first)
            continue;
        SmartPtr<RoutingGetUserStatusOp> op = new RoutingGetUserStatusOp(queryServer->first,manager);
        op->mFriendStatusMap = requested;
        RunOp(op.get(),new LogicCompletion<RoutingLogic>(&RoutingLogic::GetUserStatusComplete,this),10000);
    }
    LobbyFn359120(100056,true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007ec210
void RoutingLogic::Fn3EC210()
{
    if(!CanAddFriend()) return;
    mAddFriendDialog->Reset();
    if(LobbyDoDialog(mAddFriendDialog.get())!=100001) return;
    ServerContext* servers=LobbyFn35B1F0();
    if(!servers) return;
    SmartPtr<GetUserIdFromNameOp> op=new GetUserIdFromNameOp(servers);
    GUIString name=mAddFriendDialog->GetName();
    op->mUserName=std::wstring(name);
    LobbyFn358E70(op.get(),::AddFriendDialog_StatusTitle_String,::AddFriendDialog_StatusDetails_String,1,0);
    if(op->mStatus!=0 || op->mUserId==0) return;
    LobbyFriend* friendEntry=LobbyFn35D460(name,op->mUserId);
    if(!friendEntry) return;
    if(mCurrentServer.get()) {
        LobbyClient* client=mCurrentServer->Fn3AA380(friendEntry->GetUserId());
        if(client) {
            client->mB24=true;
            friendEntry->Fn3A6440(mCurrentServer->mAddress,mCurrentServer->mName,0);
            friendEntry->Fn3A66B0(client);
            LobbyFn359100(new ClientChangedEvent(client,4,2),true);
        }
    }
    LobbyFn359100(new FriendChangedEvent(friendEntry,0),true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e9e60
void RoutingLogic::Fn3E9E60(AsyncOp* inBaseOp)
{
    LobbyServerList* servers=LobbyFn35B6B0();
    if(!servers) return;
    RoutingGetGroupListReqOp* op=static_cast<RoutingGetGroupListReqOp*>(inBaseOp);
    LobbyServer* server=servers->Fn3AA980(op->GetAddr());
    if(!server) return;
    server->mB4C=false;
    if(op->GetStatus()!=0) {
        mLastServerRefresh=0;
        server->mLastPing=0;
    }
    server->Fn3A9BE0(op->mOp->mGroupMap,false);
    LobbyFn359100(new ServerChangedEvent(server,2),true);
}
// FUNCTION: WON 0x007ed4c0
void RoutingLogic::Fn3ED4C0(QueryServerGroupsEvent* event)
{
    LobbyServer* server=GetServer(event->mAddress);
    if(!server) return;
    time_t now=time(NULL);
    if(server->Fn3AA430() || server->mB4C || static_cast<long>(now-server->GetLastPing())<30)
        return;
    server->Fn3AA440();
    LobbyFn359100(new ServerChangedEvent(server,2),true);
    server->mB4C=true;
    server->mLastPing=now;
    SmartPtr<RoutingGetGroupListReqOp> op=new RoutingGetGroupListReqOp(server->mAddress,0);
    op->mOp->mFlags=0x29;
    RunTrackedOp(op.get(),new LogicCompletion<RoutingLogic>(&RoutingLogic::Fn3E9E60,this),0);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e9f60
void RoutingLogic::Fn3E9F60()
{
    SmartPtr<ServerContext> context=LobbyFn35B310();
    LobbyConfig* config=GetLobbyConfig();
    if(!context.get() || !config) return;
    time_t refreshTime=time(NULL);
    if(static_cast<long>(static_cast<unsigned long>(refreshTime)-static_cast<unsigned long>(mLastServerRefresh))>=300 ||
       mServerList->mServers.empty()) {
        SmartPtr<GetDirOp> op=new GetDirOp(context.get());
        op->SetPath(config->mW88);
        op->SetFlags(0x04000622);
        op->AddDataType("_C");
        op->AddDataType("_L");
        LobbyFn358E70(op.get(),::LobbyContainer_GettingRoomsTitle_String,::LobbyContainer_GettingRoomsDetails_String,1,0);
        if(op->GetStatus()!=0) return;
        const DirEntityList& entities=op->GetDirEntityList();
        if(entities.empty()) context->NotifyFailed(op->GetCurAddr());
        mServerList->Fn3AA670(entities);
        mLastServerRefresh=refreshTime;
    }
    time_t now=time(NULL);
    LobbyServerList* servers=mServerList.get();
    LobbyServerMap::const_iterator it=servers->mServers.begin();
    while(it!=servers->mServers.end()) {
        LobbyServer* server=it->second.get();
        if(!server->Fn3AA430() && static_cast<long>(static_cast<unsigned long>(now)-server->GetLastPing())>=30)
            server->Fn3AA440();
        ++it;
    }
    LobbyFn359120(100053,true);
}
// FUNCTION: WON 0x007ed430
void RoutingLogic::EnterInternetScreen(bool joinDefaultRoom)
{
    Fn3E9F60();
    if(mServerList->mServers.empty()) {
        LobbyMessageBox(::RoutingLogic_NoServersTitle_String,::RoutingLogic_NoServersDetails_String);
        return;
    }
    if(joinDefaultRoom) {
        if(!JoinDefaultRoom(false)) {
            if(LobbyFn358F50()) {
                LobbyFn361330(false);
                return;
            }
            DoRoomDialog();
        }
    } else DoRoomDialog();
    LobbyFn360F00();
    if(LobbyFn35E320() && !LobbyFn35E350()) LobbyFn35B9A0(::LobbyGlobal_SierraLogon_Sound.get());
}
}

#include "ee/wonapi/WONLobbyResourceGlobals.h"
namespace WONAPI {
// FUNCTION: WON 0x007e9a90
void RoutingLogic::DoRoomDialog()
{
    LobbyDoDialog(mRoomDialog.get());
}

// FUNCTION: WON 0x007ea310
// Full native hotcode97B; switchtargetdata44B and3alignmentB are separate.
void RoutingLogic::HandleControlEvent(int controlId)
{
    switch(controlId) {
    case ID_ChangeRoom: Fn3E9F60();DoRoomDialog();break;
    case ID_AddFriend: Fn3EC210();break;
    case ID_AnonymousToFriendsCheck: HandleAnonymousModeChange();break;
    case ID_FilterDirtyWordsCheck: HandleDirtyWordFilterChange();break;
    case ID_RefreshFriends: Fn3EC4A0();break;
    case ID_RefreshGameList: RefreshGameList();break;
    }
}

// FUNCTION: WON 0x007ea3a0
// Full native hotcode137B; targetdata44B/indexdata42B/alignment3B separate.
void RoutingLogic::HandleComponentEvent(ComponentEvent* event)
{
    switch(event->mType) {
    case 100002: AcceptClient(static_cast<AcceptClientEvent*>(event));return;
    case 100005: ClientAction(static_cast<ClientActionEvent*>(event));return;
    case 100011: CreateRoom(static_cast<CreateRoomEvent*>(event));return;
    case 100012: ChatInput(event->mComponent);return;
    case 100015: FriendAction(static_cast<FriendActionEvent*>(event));return;
    case 100028: JoinRoom(static_cast<JoinRoomEvent*>(event));return;
    case 100026: JoinFriendsRoom(static_cast<JoinFriendsRoomEvent*>(event));return;
    case 100031: JumpToRoom(static_cast<JumpToRoomEvent*>(event));return;
    case 100042: Fn3ED4C0(static_cast<QueryServerGroupsEvent*>(event));return;
    case 100043: QuickPlay(static_cast<QuickPlayEvent*>(event));return;
    }
}

// FUNCTION: WON 0x007eb2f0
void RoutingLogic::HandleAnonymousModeChange()
{
    RoutingConnection* connection=GetConnection();
    if(connection==NULL) return;
    unsigned long flags=0;
    if(LobbyFn35E390()) flags|=0x40;
    connection->SetClientFlags(0x40,flags,0,0,NULL);
}

// FUNCTION: WON 0x007eb320
void RoutingLogic::HandleDirtyWordFilterChange()
{
    RoutingConnection* connection=GetConnection();
    if(connection==NULL) return;
    unsigned long flags=0;
    if(LobbyFn35E2F0()!=true) flags|=4;
    connection->SetClientFlags(4,flags,0,0,NULL);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007ec1e0
bool RoutingLogic::CanAddFriend()
{
    if(LobbyFn35D270().size()>=255) {
        LobbyMessageBox(::RoutingLogic_TooManyFriendsTitle_String,
                        ::RoutingLogic_TooManyFriendsDetails_String);
        return false;
    }
    return true;
}
// FUNCTION: WON 0x007eeed0
void RoutingLogic::Virtual4C()
{
    AuthContext* context=GetLobbyAuthContext();
    if(context!=NULL) context->Fn2DEB30(false);
    mWantToBeModerator=false;
    mCreateRoomDialog->Reset(true);
    ClearPendingServer();
    LeaveServer();
}
}

namespace WONAPI {
// Genuine SDK ShowFriendOptions; typed popup owner and original modal API.

// FUNCTION: WON 0x007ea570
void RoutingLogic::Fn3EA570(LobbyFriend* friendInfo,int roomSpecFlags)
{
    SmartPtr<FriendOptionsPopup> popup=new FriendOptionsPopup;
    popup->Init(friendInfo,roomSpecFlags);
    LobbyFn358DF0(popup.get(),PopupFlag_StandardPopup,-1,-1);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e7050
// (public: virtual void * __thiscall WONAPI::RoutingLogic::`scalar deleting destructor'(unsigned int)    [??_GRoutingLogic@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007e79f0
// (public: class WONAPI::SmartPtr<class WONAPI::ByteBuffer> __thiscall WONAPI::RoutingPeerDataOp::GetData(void)const     [?GetData@RoutingPeerDataOp@WONA)
// TEMPLATE: WON 0x007eae90
// (public: __thiscall WONAPI::PendingServerCleanup::~PendingServerCleanup(void)    [??1PendingServerCleanup@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007ee8a0
// (public: virtual void __thiscall RoutingOpCompletion::Run(class WONAPI::AsyncOp *)    [?Run@RoutingOpCompletion@@UAEXPAVAsyncOp@WONAPI@@@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1330
// (public: __thiscall std::list<struct WONAPI::RoutingBadUser,class std::allocator<struct WONAPI::RoutingBadUser> >::list<struct WONAPI::RoutingBadUser,c)
// TEMPLATE: WON 0x007f1bb0
// (public: struct std::pair<class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned char>,struct std::map<unsigned long,unsigned ch)
// TEMPLATE: WON 0x007f30b0
// (protected: struct std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned char>,struct std::map<unsigned long,unsigned char,struct std)
// TEMPLATE: WON 0x007f3e50
// TEMPLATE: WON 0x007f3fd0
// TEMPLATE: WON 0x007f4150
// TEMPLATE: WON 0x007f42d0
// TEMPLATE: WON 0x007f4450

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e73e0
// (public: virtual void * __thiscall WONAPI::LeftRoomEvent::`scalar deleting destructor'(unsigned int)    [??_GLeftRoomEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007e7400
// (public: virtual __thiscall WONAPI::LeftRoomEvent::~LeftRoomEvent(void)    [??1LeftRoomEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007ee8d0
// (public: virtual void * __thiscall RoutingOpCompletion::`scalar deleting destructor'(unsigned int)    [??_GRoutingOpCompletion@@UAEPAXI@Z])
// TEMPLATE: WON 0x007ee8f0
// (public: virtual __thiscall RoutingOpCompletion::~RoutingOpCompletion(void)    [??1RoutingOpCompletion@@UAE@XZ])
// TEMPLATE: WON 0x007f1490
// (public: virtual void * __thiscall WONAPI::LogicCompletion<class WONAPI::RoutingLogic>::`scalar deleting destructor'(unsigned int)    [??_G?$LogicCompl)
// TEMPLATE: WON 0x007f1630
// (public: virtual __thiscall WONAPI::LogicCompletion<class WONAPI::RoutingLogic>::~LogicCompletion<class WONAPI::RoutingLogic>(void)    [??1?$LogicCompl)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007ece80
// (public: __thiscall std::multimap<unsigned short,class WONAPI::LobbyGame *,struct std::less<unsigned short>,class std::allocator<struct std::pair<unsig)
// TEMPLATE: WON 0x007f1360
// (public: __thiscall std::map<unsigned long,unsigned char,struct std::less<unsigned long>,class std::allocator<struct std::pair<unsigned long const ,uns)
// TEMPLATE: WON 0x007f1ca0
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned char>,struct std::_Mapk<unsigned long,unsigned char,struct std::)
// TEMPLATE: WON 0x007f1d50
// (protected: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned char>,struct std::_Mapk<unsigned long,unsigned char)
// TEMPLATE: WON 0x007f1e20
// (std::_Tree::insert)
// TEMPLATE: WON 0x007f1ea0
// (std::_Tree::erase)
// TEMPLATE: WON 0x007f27f0
// (protected: struct std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned char>,struct std::_Mapk<unsigned long,unsigned char,struct s)
// TEMPLATE: WON 0x007f2880
// (public: class std::list<unsigned long,class std::allocator<unsigned long> >::iterator __thiscall std::list<unsigned long,class std::allocator<unsigned)
// TEMPLATE: WON 0x007f28c0
// (std::_Tree::erase)
// TEMPLATE: WON 0x007f2d70
// (std::_Tree::_Erase)
// TEMPLATE: WON 0x007f2db0
// (std::_Tree::_Insert)
// TEMPLATE: WON 0x006d9f60
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struc)
// TEMPLATE: WON 0x00769620
// (protected: struct std::list<unsigned long,class std::allocator<unsigned long> >::_Node * __thiscall std::list<unsigned long,class std::allocator<unsig)
