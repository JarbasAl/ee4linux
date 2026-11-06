#ifndef EE_WONAPI_WONLOBBYEVENTS_H
#define EE_WONAPI_WONLOBBYEVENTS_H
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
class LobbyFriend;
class LobbyGroup;
class LobbyGameList;
// Original RTTI451af0; base20, change kind20, size24.
class LobbyChangeEvent : public LobbyEvent {
public:
    LobbyChangeEvent(int inType, int inChangeKind); //3593a0
    bool IsAdd();                                //359450
    bool IsDelete();                             //359460
    bool Fn359470();                             //359470; kind==2
    int mChangeKind;
};
class AcceptClientEvent : public LobbyEvent {
public:
    virtual ~AcceptClientEvent(); //original explicit destructor stores its own vptr
    AcceptClientEvent(LobbyClient* inClient, bool inAccepted); //359480
    SmartPtr<LobbyClient> mClient; //20
    bool mAccepted; //24
};
class AlertEvent : public LobbyEvent {
public:
    virtual ~AlertEvent(); //original explicit destructor stores its own vptr
    AlertEvent(int inKind, int inValue, int inSeverity, const GUIString& inText); //359580
    int mKind; //20
    int mValue; //24
    int mSeverity; //28
    GUIString mText; //2c
};
class AnnounceClientLocationEvent : public LobbyEvent {
public:
    virtual ~AnnounceClientLocationEvent(); //original explicit destructor stores its own vptr
    AnnounceClientLocationEvent(LobbyClient* inClient, const GUIString& inServer,
        const GUIString& inRoom, int inRoomId); //359710
    SmartPtr<LobbyClient> mClient; //20
    GUIString mServer; //24
    GUIString mRoom; //28
    int mRoomId; //2c
    bool mHandled; //30
};
class AnnounceFriendEvent : public LobbyChangeEvent {
public:
    virtual ~AnnounceFriendEvent(); //original explicit destructor stores its own vptr
    AnnounceFriendEvent(LobbyFriend* inFriend, int inLocation, int inChangeKind); //359890
    SmartPtr<LobbyFriend> mFriend; //24
    int mLocation; //28
    bool mHandled; //2c
};
class FriendActionEvent : public LobbyEvent {
public:
    virtual ~FriendActionEvent(); //original explicit destructor stores its own vptr
    FriendActionEvent(int inAction, LobbyFriend* inFriend, int inValue); //359ea0
    int mAction; //20
    SmartPtr<LobbyFriend> mFriend; //24
    int mValue; //28
};
class FriendChangedEvent : public LobbyChangeEvent {
public:
    virtual ~FriendChangedEvent(); //original explicit destructor stores its own vptr
    FriendChangedEvent(LobbyFriend* inFriend, int inChangeKind); //359fa0
    SmartPtr<LobbyFriend> mFriend; //24
};
class GameActionEvent : public LobbyEvent {
public:
    virtual ~GameActionEvent(); //original explicit destructor stores its own vptr
    GameActionEvent(int inAction, LobbyGame* inGame); //35a0a0
    int mAction; //20
    SmartPtr<LobbyGame> mGame; //24
};
class GameChangedEvent : public LobbyChangeEvent {
public:
    virtual ~GameChangedEvent(); //original explicit destructor stores its own vptr
    GameChangedEvent(LobbyGame* inGame, int inChangeKind); //35a1a0
    SmartPtr<LobbyGame> mGame; //24
};
class JoinFriendsRoomEvent : public LobbyEvent {
public:
    virtual ~JoinFriendsRoomEvent(); //original explicit destructor stores its own vptr
    JoinFriendsRoomEvent(LobbyFriend* inFriend); //35a7b0
    SmartPtr<LobbyFriend> mFriend; //20
};
class JoinGameEvent : public LobbyEvent {
public:
    virtual ~JoinGameEvent(); //original explicit destructor stores its own vptr
    JoinGameEvent(LobbyGame* inGame); //35a8b0
    SmartPtr<LobbyGame> mGame; //20
};
class PingGameEvent : public LobbyEvent {
public:
    virtual ~PingGameEvent(); //original explicit destructor stores its own vptr
    PingGameEvent(LobbyGame* inGame, bool inFlag); //35ab30
    SmartPtr<LobbyGame> mGame; //20
    bool mFlag; //24
};
class PlayerChangedEvent : public LobbyChangeEvent {
public:
    virtual ~PlayerChangedEvent(); //original explicit destructor stores its own vptr
    PlayerChangedEvent(LobbyClient* inClient, int inChangeKind); //35ac30
    SmartPtr<LobbyClient> mClient; //24
};
class PlayerTeamChangedEvent : public LobbyChangeEvent {
public:
    virtual ~PlayerTeamChangedEvent(); //original explicit destructor stores its own vptr
    PlayerTeamChangedEvent(LobbyClient* inClient, int inChangeKind); //35ad30
    SmartPtr<LobbyClient> mClient; //24
};
class ServerChangedEvent : public LobbyChangeEvent {
public:
    virtual ~ServerChangedEvent(); //original explicit destructor stores its own vptr
    ServerChangedEvent(LobbyServer* inServer, int inChangeKind); //35aff0
    SmartPtr<LobbyServer> mServer; //24
};
class ClientActionEvent : public LobbyEvent {
public:
    virtual ~ClientActionEvent(); //original explicit destructor stores its own vptr
    ClientActionEvent(int inAction, const int& inRoomSpec, LobbyClient* inClient); //359a90
    int mAction; int mRoomSpec; SmartPtr<LobbyClient> mClient; //20,24,28
};
class GotHTTPDocumentEvent : public LobbyEvent {
public:
    virtual ~GotHTTPDocumentEvent(); //original explicit destructor stores its own vptr
    GotHTTPDocumentEvent(GetHTTPDocumentOp* inOp); //35a2a0; Init3de280 consumer
    SmartPtr<GetHTTPDocumentOp> mOp; //20; size24
};
class ChangeDirectConnectEvent : public LobbyChangeEvent {
public:
    virtual ~ChangeDirectConnectEvent(); //original explicit destructor stores its own vptr
    ChangeDirectConnectEvent(int inChangeKind, LobbyGame* inGame); //359990; Staging3f00c3
    SmartPtr<LobbyGame> mGame; //24; size28
};
class ClientChangedEvent : public LobbyChangeEvent {
public:
    virtual ~ClientChangedEvent(); //original explicit destructor stores its own vptr
    ClientChangedEvent(LobbyClient* inClient, const int& inRoomSpec, int inChangeKind); //359b90
    SmartPtr<LobbyClient> mClient; int mRoomSpec; //24,28
};
class ClientModeratedEvent : public LobbyEvent {
public:
    virtual ~ClientModeratedEvent(); //original explicit destructor stores its own vptr
    ClientModeratedEvent(LobbyClient* inClient, int inRoomSpec, const GUIString& inRoomName,
        int inModerationKind, unsigned long inDuration, bool inFlag, const GUIString& inComment); //359c90
    SmartPtr<LobbyClient> mClient; int mRoomSpec; GUIString mRoomName;
    int mModerationKind; unsigned long mDuration; bool mFlag; GUIString mComment; //20,24,28,2c,30,34,38
};
class GroupChangedEvent : public LobbyChangeEvent {
public:
    virtual ~GroupChangedEvent(); //original explicit destructor stores its own vptr
    GroupChangedEvent(LobbyGroup* inGroup, LobbyServer* inServer, int inChangeKind); //35a3a0
    SmartPtr<LobbyServer> mServer; SmartPtr<LobbyGroup> mGroup; //24,28
};
class GroupInvitationEvent : public LobbyEvent {
public:
    virtual ~GroupInvitationEvent(); //original explicit destructor stores its own vptr
    GroupInvitationEvent(int inKind, const GUIString& inGameName, unsigned short inGameId,
        LobbyClient* inClient, bool inInvite, const GUIString& inComment, bool inFlag); //35a4e0
    int mKind; GUIString mGameName; unsigned short mGameId; SmartPtr<LobbyClient> mClient;
    bool mInvite; bool mFlag; GUIString mComment; //20,24,28,2c,30,31,34
};
class GroupJoinAttemptEvent : public LobbyEvent {
public:
    virtual ~GroupJoinAttemptEvent(); //original explicit destructor stores its own vptr
    GroupJoinAttemptEvent(int inKind, LobbyClient* inClient, const GUIString& inComment); //35a670
    int mKind; SmartPtr<LobbyClient> mClient; GUIString mComment; //20,24,28
};
class JoinedRoomEvent : public LobbyEvent {
public:
    virtual ~JoinedRoomEvent(); //original explicit destructor stores its own vptr
    JoinedRoomEvent(const GUIString& inServerName, const GUIString& inGroupName,
        const int& inRoomSpec, unsigned short inClientId, LobbyClientList* inClients); //35a9b0
    SmartPtr<LobbyClientList> mClients; GUIString mServerName; GUIString mGroupName;
    unsigned short mClientId; int mRoomSpec; //20,24,28,2c,30
};
class RecvChatEvent : public LobbyEvent {
public:
    virtual ~RecvChatEvent(); //original explicit destructor stores its own vptr
    RecvChatEvent(const int& inRoomSpec, unsigned long inFlags, LobbyClient* inSender,
        const GUIString& inText, LobbyClient* inTarget); //35ae30
    bool IsEmote(); bool IsWhisper(); bool IsWarning(); bool IsTeam();
    SmartPtr<LobbyClient> mSender; SmartPtr<LobbyClient> mTarget; int mRoomSpec;
    unsigned long mFlags; GUIString mText; //20,24,28,2c,30
};
// Producer-owned declarations only; implementation and emitted markers remain in original owner.
class SetRoomSpecFlagsEvent : public LobbyEvent {
public:
    SetRoomSpecFlagsEvent(int inFlags); //3c4620; RootScreen family
    int mFlags; //20; size24
};
class LoginInitStatusEvent : public LobbyEvent {
public:
    LoginInitStatusEvent(const GUIString& inStatus, const GUIString& inDescription,
        bool inAllowIgnore, int inKind)
        : LobbyEvent(100036), mStatus(inStatus), mDescription(inDescription),
          mAllowIgnore(inAllowIgnore), mKind(inKind) {}
    GUIString mStatus; //20
    GUIString mDescription; //24
    bool mAllowIgnore; //28
    int mKind; //2c
};
class SetLoginFieldsEvent : public LobbyEvent {
public:
    SetLoginFieldsEvent(const GUIString& inUser, const GUIString& inPassword, bool inRemember)
        : LobbyEvent(100051), mUserName(inUser), mPassword(inPassword), mRemember(inRemember) {}
    GUIString mUserName; //20
    GUIString mPassword; //24
    bool mRemember; //28
};
class QueryAccountResultEvent : public LobbyEvent {
public:
    QueryAccountResultEvent(const GUIString& inEmail, bool inNewsletter)
        : LobbyEvent(100041), mEmail(inEmail), mNewsletter(inNewsletter) {}
    GUIString mEmail; //20
    bool mNewsletter; //24
};
class LoginEvent : public LobbyEvent {
public:
    LoginEvent(const GUIString& inUser, const GUIString& inPassword, bool inRemember)
        : LobbyEvent(100035), mUserName((std::wstring)inUser),
          mPassword((std::wstring)inPassword), mRemember(inRemember) {}
    std::wstring mUserName; std::wstring mPassword; bool mRemember; //20,30,40; size44
};
// Complete producer-event contracts; bodies/support remain with actual producer families.
class GetLostPasswordEvent : public LobbyEvent {
public:
    GetLostPasswordEvent(const GUIString& inUserName)
        : LobbyEvent(100018), mUserName((std::wstring)inUserName) {}
    std::wstring mUserName; //20;size30;VT453a38
    // Normal3c06c0 no own-vptr store; implicit, producer Controls.
};
class GetLostUserNameEvent : public LobbyEvent {
public:
    GetLostUserNameEvent(const GUIString& inEmail)
        : LobbyEvent(100019), mEmail((std::string)inEmail) {}
    std::string mEmail; //20;size30;VT453b14
    // Normal3c0ab0 no own-vptr store; implicit, producer Controls.
};
class LeftRoomEvent : public LobbyEvent {
public:
    LeftRoomEvent(int inRoomSpec, bool inForced, bool inFlag)
        : LobbyEvent(100033), mRoomSpec(inRoomSpec), mForced(inForced), mFlag(inFlag) {}
    int mRoomSpec; bool mForced; bool mFlag; //20,24,25;size28
    // No explicit destructor: actual3e7400 is implicit,producerRouting owns support.
};
class SetChatInputEvent : public LobbyEvent {
public:
    SetChatInputEvent(int inCommand, const GUIString& inText, int inRoomSpec, bool inHandled)
        : LobbyEvent(100048), mCommand(inCommand), mText(inText), mRoomSpec(inRoomSpec), mHandled(inHandled) {}
    int mCommand; GUIString mText; int mRoomSpec; bool mHandled; //20,24,28,2c;size30
    // Actual deleting3bd9c0Ã¢â€ â€™normal3bd9e0..3bda67=135B, NO ownvptr: implicit destructor.
};
class ChatCommandErrorEvent : public LobbyEvent {
public:
    ChatCommandErrorEvent(const GUIString& inText) : LobbyEvent(100009), mText(inText) {}
    GUIString mText; //20;size24;VT4559e0,Routing inline3eb622..3eb63c
    // Original3e3200 has no own-vptr store; implicit producer-owned destructor; normal COFF remains required.
};
class CreateRoomEvent : public LobbyEvent {
public:
    CreateRoomEvent(const IPAddr& inAddress) : LobbyEvent(100011), mAddress(inAddress) {}
    IPAddr mAddress; //20;size48
    // Original3c2a80 has no own-vptr store; implicit producer-owned destructor.
};
class JoinRoomEvent : public LobbyEvent {
public:
    JoinRoomEvent(const IPAddr& inAddress, unsigned long inGroupId)
        : LobbyEvent(100028), mAddress(inAddress), mGroupId(inGroupId) {}
    IPAddr mAddress; unsigned long mGroupId; //20,48;size4c
    // Original3c2860 has no own-vptr store; implicit producer-owned destructor.
};
// Original VT452f0c, producer3ba990; defaultstrings precede widget assignments.
class CreateAccountEvent : public LobbyEvent {
public:
    CreateAccountEvent() : LobbyEvent(100010) {}
    std::wstring mUserName; //20
    std::wstring mPassword; //30
    std::string mEmail; //40
    unsigned char mMonth; //50, deliberately uninitialized until producer assignment
    unsigned char mDay; //51
    unsigned short mYear; //52
    bool mNewsletter; //54, natural end58
    // Normal3bac50 has no own-vptr store: implicit producer-owned destructor.
};
class SyncGameListEvent : public LobbyEvent {
public:
    SyncGameListEvent(LobbyGameList* inGames); //35b0f0,coreoriginalfamily
    virtual ~SyncGameListEvent(); //35b160,originalexplicitownvptr
    SmartPtr<LobbyGameList> mGames; //20;size24
};
class JumpToRoomEvent : public LobbyEvent {
public:
    JumpToRoomEvent(const GUIString& inRoomName, unsigned short inGroupId, const IPAddr& inAddress)
        : LobbyEvent(100031), mRoomName(inRoomName), mGroupId(inGroupId), mAddress(inAddress) {}
    GUIString mRoomName; unsigned short mGroupId; IPAddr mAddress; //20,24,28;size50
    // Actual normal3b9bc0 has NO own vptr store: implicit destructor, Controls support.
};
class QuickPlayEvent : public LobbyEvent {
public:
    QuickPlayEvent(int inGameType) : LobbyEvent(100043), mGameType(inGameType) {}
    int mGameType; //20;size24;VT453cc0,producer3c1750 coldtail3c18aa
};
// Actual VT455904/type100049,size34: raw non-owning pointer20/string24.
// Archived header names Image*; nominal spelling is version-qualified.
class SetCrossPromotionEvent : public LobbyEvent {
public:
    SetCrossPromotionEvent(Image* inImage, const std::string& inLink)
        : LobbyEvent(100049), mImage(inImage), mLink(inLink) {}
    Image* mImage; //20, no retain/release in actual producers or destructor
    std::string mLink; //24
    // Original3ddb70 has no own-vptr store: natural implicit destructor.
};
// Actual VT454444/type100060,size2c; complete producer3ca410 field/constructor proof.
class UpdateAccountEvent : public LobbyEvent {
public:
    UpdateAccountEvent(const GUIString& inEmail, const GUIString& inNewPassword, bool inNewsletter)
        : LobbyEvent(100060), mEmail(inEmail), mNewPassword(inNewPassword), mNewsletter(inNewsletter) {}
    GUIString mEmail; //20
    GUIString mNewPassword; //24
    bool mNewsletter; //28
    // Normal3ca5f0 no own-vptr store: implicit producer-owned destructor, no authored marker.
};
// Actual VT453df0/type100042, producer3c2f00; complete48B with IPAddr20.
class QueryServerGroupsEvent : public LobbyEvent {
public:
    QueryServerGroupsEvent(const IPAddr& inAddress) : LobbyEvent(100042), mAddress(inAddress) {}
    IPAddr mAddress; //20..48
    // Normal3c3120 no own-vptr store: implicit producer-owned destructor, zero source marker.
};
}
#endif
