// PROVISIONAL original event family; complete source draft, not admitted or compiled.
// All markers below refer to original functional bodies; scalar deleting destructors are generated support.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyEvents.h"
namespace WONAPI {
// FUNCTION: WON 0x007593a0
LobbyChangeEvent::LobbyChangeEvent(int inType, int inChangeKind)
    : LobbyEvent(inType), mChangeKind(inChangeKind) {}
// FUNCTION: WON 0x00759450
bool LobbyChangeEvent::IsAdd() { return mChangeKind == 0; }
// FUNCTION: WON 0x00759460
bool LobbyChangeEvent::IsDelete() { return mChangeKind == 1; }
// FUNCTION: WON 0x00759470
bool LobbyChangeEvent::Fn359470() { return mChangeKind == 2; }
// FUNCTION: WON 0x00759480
AcceptClientEvent::AcceptClientEvent(LobbyClient* inClient, bool inAccepted)
    : LobbyEvent(100002), mClient(inClient) { mAccepted = inAccepted; }
// FUNCTION: WON 0x00759580
AlertEvent::AlertEvent(int inKind, int inValue, int inSeverity, const GUIString& inText)
    : LobbyEvent(100001), mKind(0)
{
    mKind = inKind;
    mValue = inValue;
    mSeverity = inSeverity;
    mText = inText;
}
// FUNCTION: WON 0x00759710
AnnounceClientLocationEvent::AnnounceClientLocationEvent(LobbyClient* inClient,
    const GUIString& inServer, const GUIString& inRoom, int inRoomId)
    : LobbyEvent(100004), mClient(inClient), mServer(inServer), mRoom(inRoom),
      mRoomId(inRoomId) { mHandled = false; }
// FUNCTION: WON 0x00759890
AnnounceFriendEvent::AnnounceFriendEvent(LobbyFriend* inFriend, int inLocation, int inChangeKind)
    : LobbyChangeEvent(100003, inChangeKind), mFriend(inFriend), mLocation(inLocation) { mHandled = false; }
// FUNCTION: WON 0x00759ea0
FriendActionEvent::FriendActionEvent(int inAction, LobbyFriend* inFriend, int inValue)
    : LobbyEvent(100015), mFriend(inFriend)
{ mAction = inAction; mValue = inValue; }
// FUNCTION: WON 0x00759fa0
FriendChangedEvent::FriendChangedEvent(LobbyFriend* inFriend, int inChangeKind)
    : LobbyChangeEvent(100016, inChangeKind), mFriend(inFriend) {}
// FUNCTION: WON 0x0075a0a0
GameActionEvent::GameActionEvent(int inAction, LobbyGame* inGame)
    : LobbyEvent(100017), mGame(inGame) { mAction = inAction; }
// FUNCTION: WON 0x0075a1a0
GameChangedEvent::GameChangedEvent(LobbyGame* inGame, int inChangeKind)
    : LobbyChangeEvent(100020, inChangeKind), mGame(inGame) {}
// FUNCTION: WON 0x0075a7b0
JoinFriendsRoomEvent::JoinFriendsRoomEvent(LobbyFriend* inFriend)
    : LobbyEvent(100026), mFriend(inFriend) {}
// FUNCTION: WON 0x0075a8b0
JoinGameEvent::JoinGameEvent(LobbyGame* inGame)
    : LobbyEvent(100027), mGame(inGame) {}
// FUNCTION: WON 0x0075ab30
PingGameEvent::PingGameEvent(LobbyGame* inGame, bool inFlag)
    : LobbyEvent(100037), mGame(inGame) { mFlag = inFlag; }
// FUNCTION: WON 0x0075ac30
PlayerChangedEvent::PlayerChangedEvent(LobbyClient* inClient, int inChangeKind)
    : LobbyChangeEvent(100038, inChangeKind), mClient(inClient) {}
// FUNCTION: WON 0x0075ad30
PlayerTeamChangedEvent::PlayerTeamChangedEvent(LobbyClient* inClient, int inChangeKind)
    : LobbyChangeEvent(100039, inChangeKind), mClient(inClient) {}
// FUNCTION: WON 0x0075aff0
ServerChangedEvent::ServerChangedEvent(LobbyServer* inServer, int inChangeKind)
    : LobbyChangeEvent(100047, inChangeKind), mServer(inServer) {}
// FUNCTION: WON 0x0075a2a0
GotHTTPDocumentEvent::GotHTTPDocumentEvent(GetHTTPDocumentOp* inOp)
    : LobbyEvent(100024), mOp(inOp) {}
// FUNCTION: WON 0x00759990
ChangeDirectConnectEvent::ChangeDirectConnectEvent(int inChangeKind, LobbyGame* inGame)
    : LobbyChangeEvent(100008, inChangeKind), mGame(inGame) {}
// FUNCTION: WON 0x00759a90
ClientActionEvent::ClientActionEvent(int inAction, const int& inRoomSpec, LobbyClient* inClient)
    : LobbyEvent(100005), mRoomSpec(inRoomSpec), mClient(inClient)
{ mAction = inAction; }
// FUNCTION: WON 0x00759b90
ClientChangedEvent::ClientChangedEvent(LobbyClient* inClient, const int& inRoomSpec, int inChangeKind)
    : LobbyChangeEvent(100006, inChangeKind), mClient(inClient), mRoomSpec(inRoomSpec) {}
// FUNCTION: WON 0x00759c90
ClientModeratedEvent::ClientModeratedEvent(LobbyClient* inClient, int inRoomSpec, const GUIString& inRoomName,
    int inModerationKind, unsigned long inDuration, bool inFlag, const GUIString& inComment)
    : LobbyEvent(100007), mClient(inClient), mRoomSpec(0), mRoomName(inRoomName)
{
    mRoomSpec = inRoomSpec;
    mDuration = inDuration;
    mModerationKind = inModerationKind;
    mFlag = inFlag;
    mComment = inComment;
}
// FUNCTION: WON 0x0075a3a0
GroupChangedEvent::GroupChangedEvent(LobbyGroup* inGroup, LobbyServer* inServer, int inChangeKind)
    : LobbyChangeEvent(100021, inChangeKind), mServer(inServer), mGroup(inGroup) {}
// FUNCTION: WON 0x0075a4e0
GroupInvitationEvent::GroupInvitationEvent(int inKind, const GUIString& inGameName, unsigned short inGameId,
    LobbyClient* inClient, bool inInvite, const GUIString& inComment, bool inFlag)
    : LobbyEvent(100022), mKind(inKind), mGameName(inGameName), mClient(inClient), mComment(inComment)
{ mGameId = inGameId; mInvite = inInvite; mFlag = inFlag; }
// FUNCTION: WON 0x0075a670
GroupJoinAttemptEvent::GroupJoinAttemptEvent(int inKind, LobbyClient* inClient, const GUIString& inComment)
    : LobbyEvent(100023), mKind(inKind), mClient(inClient), mComment(inComment) {}
// FUNCTION: WON 0x0075a9b0
JoinedRoomEvent::JoinedRoomEvent(const GUIString& inServerName, const GUIString& inGroupName,
    const int& inRoomSpec, unsigned short inClientId, LobbyClientList* inClients)
    : LobbyEvent(100030), mClients(inClients), mServerName(inServerName), mGroupName(inGroupName), mClientId(inClientId)
{ mRoomSpec = inRoomSpec; }
// FUNCTION: WON 0x0075ae30
RecvChatEvent::RecvChatEvent(const int& inRoomSpec, unsigned long inFlags, LobbyClient* inSender,
    const GUIString& inText, LobbyClient* inTarget)
    : LobbyEvent(100044), mSender(inSender), mTarget(inTarget), mRoomSpec(inRoomSpec), mText(inText)
{ mFlags = inFlags; }
// FUNCTION: WON 0x0075afb0
bool RecvChatEvent::IsEmote() { return (mFlags & 4) != 0; }
// FUNCTION: WON 0x0075afc0
bool RecvChatEvent::IsWhisper() { return (mFlags & 2) != 0; }
// FUNCTION: WON 0x0075afd0
bool RecvChatEvent::IsWarning() { return (mFlags & 16) != 0; }
// FUNCTION: WON 0x0075afe0
bool RecvChatEvent::IsTeam() { if (mFlags & 256) return true; return false; }
// FUNCTION: WON 0x007594f0
AcceptClientEvent::~AcceptClientEvent() {}
// FUNCTION: WON 0x00759680
AlertEvent::~AlertEvent() {}
// FUNCTION: WON 0x007597b0
AnnounceClientLocationEvent::~AnnounceClientLocationEvent() {}
// FUNCTION: WON 0x00759900
AnnounceFriendEvent::~AnnounceFriendEvent() {}
// FUNCTION: WON 0x00759a00
ChangeDirectConnectEvent::~ChangeDirectConnectEvent() {}
// FUNCTION: WON 0x00759b00
ClientActionEvent::~ClientActionEvent() {}
// FUNCTION: WON 0x00759c00
ClientChangedEvent::~ClientChangedEvent() {}
// FUNCTION: WON 0x00759dc0
ClientModeratedEvent::~ClientModeratedEvent() {}
// FUNCTION: WON 0x00759f10
FriendActionEvent::~FriendActionEvent() {}
// FUNCTION: WON 0x0075a010
FriendChangedEvent::~FriendChangedEvent() {}
// FUNCTION: WON 0x0075a110
GameActionEvent::~GameActionEvent() {}
// FUNCTION: WON 0x0075a210
GameChangedEvent::~GameChangedEvent() {}
// FUNCTION: WON 0x0075a310
GotHTTPDocumentEvent::~GotHTTPDocumentEvent() {}
// FUNCTION: WON 0x0075a430
GroupChangedEvent::~GroupChangedEvent() {}
// FUNCTION: WON 0x0075a590
GroupInvitationEvent::~GroupInvitationEvent() {}
// FUNCTION: WON 0x0075a700
GroupJoinAttemptEvent::~GroupJoinAttemptEvent() {}
// FUNCTION: WON 0x0075a820
JoinFriendsRoomEvent::~JoinFriendsRoomEvent() {}
// FUNCTION: WON 0x0075a920
JoinGameEvent::~JoinGameEvent() {}
// FUNCTION: WON 0x0075aa50
JoinedRoomEvent::~JoinedRoomEvent() {}
// FUNCTION: WON 0x0075aba0
PingGameEvent::~PingGameEvent() {}
// FUNCTION: WON 0x0075aca0
PlayerChangedEvent::~PlayerChangedEvent() {}
// FUNCTION: WON 0x0075ada0
PlayerTeamChangedEvent::~PlayerTeamChangedEvent() {}
// FUNCTION: WON 0x0075aed0
RecvChatEvent::~RecvChatEvent() {}
// FUNCTION: WON 0x0075b060
ServerChangedEvent::~ServerChangedEvent() {}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007594d0
// (public: virtual void * __thiscall WONAPI::AcceptClientEvent::`scalar deleting destructor'(unsigned int)    [??_GAcceptClientEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00759660
// (public: virtual void * __thiscall WONAPI::AlertEvent::`scalar deleting destructor'(unsigned int)    [??_GAlertEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00759790
// (public: virtual void * __thiscall WONAPI::AnnounceClientLocationEvent::`scalar deleting destructor'(unsigned int)    [??_GAnnounceClientLocationEvent@)
// TEMPLATE: WON 0x007598e0
// (public: virtual void * __thiscall WONAPI::AnnounceFriendEvent::`scalar deleting destructor'(unsigned int)    [??_GAnnounceFriendEvent@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x007599e0
// (public: virtual void * __thiscall WONAPI::ChangeDirectConnectEvent::`scalar deleting destructor'(unsigned int)    [??_GChangeDirectConnectEvent@WONAPI)
// TEMPLATE: WON 0x00759ae0
// (public: virtual void * __thiscall WONAPI::ClientActionEvent::`scalar deleting destructor'(unsigned int)    [??_GClientActionEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00759be0
// (public: virtual void * __thiscall WONAPI::ClientChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GClientChangedEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00759da0
// (public: virtual void * __thiscall WONAPI::ClientModeratedEvent::`scalar deleting destructor'(unsigned int)    [??_GClientModeratedEvent@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x00759ef0
// (public: virtual void * __thiscall WONAPI::FriendActionEvent::`scalar deleting destructor'(unsigned int)    [??_GFriendActionEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00759ff0
// (public: virtual void * __thiscall WONAPI::FriendChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GFriendChangedEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075a0f0
// (public: virtual void * __thiscall WONAPI::GameActionEvent::`scalar deleting destructor'(unsigned int)    [??_GGameActionEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075a1f0
// (public: virtual void * __thiscall WONAPI::GameChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GGameChangedEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075a2f0
// (public: virtual void * __thiscall WONAPI::GotHTTPDocumentEvent::`scalar deleting destructor'(unsigned int)    [??_GGotHTTPDocumentEvent@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x0075a410
// (public: virtual void * __thiscall WONAPI::GroupChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GGroupChangedEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075a570
// (public: virtual void * __thiscall WONAPI::GroupInvitationEvent::`scalar deleting destructor'(unsigned int)    [??_GGroupInvitationEvent@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x0075a6e0
// (public: virtual void * __thiscall WONAPI::GroupJoinAttemptEvent::`scalar deleting destructor'(unsigned int)    [??_GGroupJoinAttemptEvent@WONAPI@@UAEP)
// TEMPLATE: WON 0x0075a800
// (public: virtual void * __thiscall WONAPI::JoinFriendsRoomEvent::`scalar deleting destructor'(unsigned int)    [??_GJoinFriendsRoomEvent@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x0075a900
// (public: virtual void * __thiscall WONAPI::JoinGameEvent::`scalar deleting destructor'(unsigned int)    [??_GJoinGameEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075aa30
// (public: virtual void * __thiscall WONAPI::JoinedRoomEvent::`scalar deleting destructor'(unsigned int)    [??_GJoinedRoomEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075ab80
// (public: virtual void * __thiscall WONAPI::PingGameEvent::`scalar deleting destructor'(unsigned int)    [??_GPingGameEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075ac80
// (public: virtual void * __thiscall WONAPI::PlayerChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GPlayerChangedEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075ad80
// (public: virtual void * __thiscall WONAPI::PlayerTeamChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GPlayerTeamChangedEvent@WONAPI@@UA)
// TEMPLATE: WON 0x0075aeb0
// (public: virtual void * __thiscall WONAPI::RecvChatEvent::`scalar deleting destructor'(unsigned int)    [??_GRecvChatEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0075b040
// (public: virtual void * __thiscall WONAPI::ServerChangedEvent::`scalar deleting destructor'(unsigned int)    [??_GServerChangedEvent@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007593d0
// (public: virtual void * __thiscall WONAPI::LobbyChangeEvent::`scalar deleting destructor'(unsigned int)    [??_GLobbyChangeEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007593f0
// (public: virtual __thiscall WONAPI::LobbyChangeEvent::~LobbyChangeEvent(void)    [??1LobbyChangeEvent@WONAPI@@UAE@XZ])
