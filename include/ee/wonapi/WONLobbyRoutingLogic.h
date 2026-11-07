// Original-backed Routing family declaration; root released this source cohort.
// Identity/extent/layout evidence: phase0.md, original_eh.json, rtti.txt.
#ifndef EE_WONAPI_WONLOBBYROUTINGLOGIC_H
#define EE_WONAPI_WONLOBBYROUTINGLOGIC_H
#include <time.h>
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyEvents.h"
class RoutingOpCompletion;
namespace WONAPI {
class AcceptClientEvent;
class ClientActionEvent;
class FriendActionEvent;
class JumpToRoomEvent;
class CreateRoomEvent;
class JoinRoomEvent;
class JoinFriendsRoomEvent;
class QuickPlayEvent;
class LobbyGameList;
class AddFriendDialog;
class AdminActionDialog;
class BadUserDialog;
class CreateRoomDialog;
class RoomDialog;
class PasswordDialog;
// Actual RTTI VT455a60: RoutingLogic > NetLogic > LobbyLogic > RefCount.
// Data begins40; final SmartPtr88; constructor/deleting extent supports8c.
// No independently invented NetLogic data or virtual padding.
class RoutingLogic : public NetLogic {
public:
    virtual void InitComponent(ComponentConfig* inConfig);
    RoutingLogic();
    virtual ~RoutingLogic();
    virtual bool Virtual18(LobbyGame* inGame);
    virtual bool Virtual1C(LobbyGame* inGame);
    void ClearPendingServer();
    int GetRoomSpec(unsigned short inGroupId);
    int GetRoomSpec(LobbyGroup* inGroup, LobbyServer* inServer);
    LobbyClientList* GetClientList();
    bool IsConnected();
    RoutingConnection* GetConnection();
    LobbyGroup* GetGroup(int inRoomSpec);
    LobbyServer* GetServer(const IPAddr& inAddress);
    LobbyServer* GetServer(RoutingOp* inOp);
    LobbyServer* GetDefaultServer();
    bool IsConnecting();
    virtual LobbyClient* Virtual20();
    virtual void Virtual24(const ByteBuffer* inData);
    virtual void Virtual28(const ByteBuffer* inData);
    virtual void Virtual2C(LobbyClient* inClient, const ByteBuffer* inData);
    virtual void Virtual30(LobbyGame* inGame);
    virtual void Virtual34(LobbyGame*);
    virtual void Virtual38(LobbyClient* inClient, bool inBan);
    virtual void Virtual3C(bool inClose);
    void MemberFlagsChanged(RoutingOp* inBaseOp, LobbyServer* inServer);
    void ClientFlagsChanged(RoutingOp* inBaseOp, LobbyServer* inServer);
    bool ShouldLocateFriend(LobbyFriend* inFriend, unsigned long inNow);
    bool AnnounceClientLocation(LobbyClient* inClient, bool inForce, int inRoomSpec, bool inFindByUserId);
    void PeerData(RoutingOp* inBaseOp, LobbyServer* inServer);
    void ClientJoinedGroup(RoutingOp* inBaseOp, LobbyServer* inServer);
    void ClientLeftGroup(RoutingOp* inBaseOp, LobbyServer* inServer);
    void GroupCreated(RoutingOp* inBaseOp, LobbyServer* inServer);
    void GroupDeleted(RoutingOp* inBaseOp, LobbyServer* inServer);
    void UpdateFriend(LobbyClient* inClient);
    void AnnounceFriendLocation(LobbyFriend* inFriend, int inRoomSpec);
    void ClientJoinedServer(RoutingOp* inBaseOp, LobbyServer* inServer);
    void ClientLeftServer(RoutingOp* inBaseOp, LobbyServer* inServer);
    void GroupInvitation(RoutingOp* inBaseOp, LobbyServer* inServer);
    void GroupJoinAttempt(RoutingOp* inBaseOp, LobbyServer* inServer);
    virtual void Reset();
    void PeerChat(RoutingOp* inBaseOp, LobbyServer* inServer);
    void ServerAlert(RoutingOp* inBaseOp, LobbyServer*);
    void ServerShutdownStarted(RoutingOp* inBaseOp, LobbyServer*);
    void ServerShutdownAborted(RoutingOp* inBaseOp, LobbyServer*);
    void YouWereMuted(RoutingOp* inBaseOp, LobbyServer* inServer);
    virtual void Virtual40(LobbyGame*);
    virtual void Virtual44();
    virtual void Virtual48();
    void LeaveServer();
    void GroupCaptainChanged(RoutingOp* inBaseOp, LobbyServer* inServer);
    void DataObjectDeleted(RoutingOp* inBaseOp, LobbyServer* inServer);
    void DataObjectCreated(RoutingOp* inBaseOp, LobbyServer* inServer);
    void YouWereBanned(RoutingOp* inBaseOp, LobbyServer* inServer);
    bool JoinDefaultRoom(bool inShowStatus);
    void RefreshGameList();
    void JoinedGroup(unsigned short inGroupId, bool inGameGroup);
    void ReadGameListComplete(RoutingOp* inBaseOp, LobbyServer* inServer);
    void AcceptClient(AcceptClientEvent* inEvent);
    void RemoveFriend(LobbyFriend* inFriend);
    void FriendAction(FriendActionEvent* inEvent);
    LobbyGame* SelectGame(int inGameType, unsigned short inPingLimit);
    void JumpToRoom(JumpToRoomEvent* inEvent);
    void QuickPlay(QuickPlayEvent* inEvent);
    void ClientAction(ClientActionEvent* inEvent);
    bool InviteClient(LobbyClient* inClient, bool inInvite, const GUIString& inComment);
    void ChatInput(Component* inTarget);
    void ConnectionClosed(AsyncOp* inBaseOp);
    void CreateRoom(CreateRoomEvent* inEvent);
    bool JoinChatRoom(LobbyServer* inServer, unsigned short inGroupId);
    void JoinRoom(JoinRoomEvent* inEvent);
    void JoinFriendsRoom(JoinFriendsRoomEvent* inEvent);
    // Original-backed calls whose complete source bodies remain deferred.
    bool CanAddFriend(); //3ec1e0
    void Fn3E9E60(AsyncOp*); //3e9e60 group query completion
    void Fn3ED4C0(QueryServerGroupsEvent*); //3ed4c0 tracked server group query
    void Fn3EC210(); //3ec210 complete add-friend workflow
    void EnterInternetScreen(bool); //3ed430 complete internet-entry workflow
    void Fn3E9F60(); //3e9f60 full directory refresh
    void DoRoomDialog(); //3e9a90 native14B, actual SDK void call-only helper
    void Fn3E9AB0(bool, bool, int); //3e9ab0 moderator list dialog workflow
    void Fn3E9440(int, int, LobbyClient*); //3e9440 admin dialog workflow
    void Fn3EA490(LobbyClient*, int); //216B dialog allocation gate
    void Fn3EA570(LobbyFriend*, int); //178B dialog allocation gate
    void GetUserStatusComplete(AsyncOp*); //3e7460 actual typed completion member
    void Fn3EBFD0(LobbyFriend*, int); //526B friend-locate workflow
    void Fn3EC4A0(); //744B dialog/search workflow
    bool ConnectToServer(LobbyServer*, bool); //3edd70; true RoutingConnection allocation gate
    bool CreateGroup(LobbyServer*, const GUIString&, const GUIString&,
                     int, unsigned short&); //3ed670; true RoutingOp subclass gate
    bool JoinGroup(LobbyServer*, int); //3ed8b0; allocation+dialog gates

    virtual void Virtual4C(); //3eeed0; AuthContext/Dialogs canonical contracts
    void HandleAnonymousModeChange(); //3eb2f0 actualSDK flag update48B
    void HandleDirtyWordFilterChange(); //3eb320 actualSDK flag update48B
    virtual void HandleControlEvent(int); //3ea310
    virtual void HandleComponentEvent(ComponentEvent*); //3ea3a0
    time_t mLastServerRefresh; //40 signed time() delta:3e9fb2..3e9fc7; ctorzero
    std::string mGameDataType; //44 constructor literal "G", trueCOWstring10
    SmartPtr<LobbyServerList> mServerList; //54
    SmartPtr<LobbyGameList> mGameList; //58
    bool mSuppressIdle; //5c
    bool mIdle; //5d
    bool mHaveGameList; //5e
    bool mWantToBeModerator; //5f; ChatInput3eb688 writes requested moderator state
    SmartPtr<AsyncOp> mGameListWaitOp; //60 trueAsyncOp44 allocation3ecf30
    SmartPtr<LobbyServer> mCurrentServer; //64
    SmartPtr<LobbyServer> mPendingServer; //68
    SmartPtr<LobbyGroup> mChatGroup; //6c
    SmartPtr<LobbyGroup> mGameGroup; //70
    SmartPtr<AddFriendDialog> mAddFriendDialog; //74 SafeGetRTTI4e1608
    SmartPtr<AdminActionDialog> mAdminActionDialog; //78 SafeGetRTTI4e1630
    SmartPtr<BadUserDialog> mBadUserDialog; //7c SafeGetRTTI4e1658
    SmartPtr<CreateRoomDialog> mCreateRoomDialog; //80 SafeGetRTTI4e1680
    SmartPtr<RoomDialog> mRoomDialog; //84 SafeGetRTTI4e16a8
    SmartPtr<PasswordDialog> mPasswordDialog; //88 SafeGetRTTI4e15e0
};
}
#endif
