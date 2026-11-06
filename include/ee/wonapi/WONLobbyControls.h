// Original WONLobby control contracts: actual Container base84 and owned fields.
// Root-approved narrow header lease; implementation admission is separate.
#ifndef EE_WONAPI_WONLOBBYCONTROLS_H
#define EE_WONAPI_WONLOBBYCONTROLS_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
class LoginInitStatusEvent;
class LoginStatusCtrl : public Container {
public:
    LoginStatusCtrl();
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    SmartPtr<Label> mStatusLabel; // +84, allocation88
};
class InitFailedCtrl : public Container {
public:
    InitFailedCtrl();
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void HandleLoginInitStatus(LoginInitStatusEvent* inEvent);
    SmartPtr<Label> mDescLabel; // +84
    SmartPtr<Button> mRetryButton; // +88
    SmartPtr<Button> mIgnoreButton; // +8c; allocation90
};
class GameStagingCtrl : public Container {
public:
    GameStagingCtrl();
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void UpdateStartButton();
    void JoinedGame();
    void LeftGame();
    SmartPtr<Button> mLeaveGameButton; //84
    SmartPtr<Button> mStartGameButton; //88
    long mRoomSpecFlags; //8c
    bool mInGame; //90
    bool mLastReady; //91; allocation94 with nativealignment
};
}

// Proposal depends on the root-admitted cohesive GUI/events/rich-text batch.
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyRichText.h"
namespace WONAPI {
class ChatCommandParser;
// Truthful pointer-only forwards; canonical payload definitions remain Core-owned.
class LeftRoomEvent;
class SetChatInputEvent;
class ChatCommandErrorEvent;
}

// Proposed complete family: LoginCtrl.h.draft
// SCRATCH additional family declarations. Canonical shared event/widget contracts pending.
namespace WONAPI {
class InputBox;
class LoginEvent;
class LoginInitStatusEvent;
class SetLoginFieldsEvent;
class LoginCtrl : public Dialog {
public:
    LoginCtrl();
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void UpdateLoginButton();
    void FillUserNames();
    void LoadPassword();
    void SavePassword();
    void SetCredentials(SetLoginFieldsEvent* inEvent);
    void HandleLoginInitStatus(LoginInitStatusEvent* inEvent);
    SmartPtr<ComboBox> mUserNameCombo; //98
    SmartPtr<InputBox> mPasswordInput; //9c
    SmartPtr<Button> mRememberPasswordCheck; //a0
    SmartPtr<Button> mLoginButton; //a4
    bool mCanLogin; //a8; allocationac, native compiler alignment
};
}


// Proposed complete family: LobbyOptionsCtrl.h.draft
// SCRATCH class proposal, no shadow bases and no allocated partial widgets.
namespace WONAPI {
class ImageComponent;
class LobbyOptionsCtrl : public Container {
public:
    LobbyOptionsCtrl(bool inIsLan);
    virtual ~LobbyOptionsCtrl(); //actualownVTstore3be08f; emptybodycandidatezeroquota
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    bool HandleButton(Component* inComponent);
    void RefreshLanOptions();
    void RefreshOptions();
    void SyncOptions();
    void RefreshPublicAddress();
    bool mIsLan; //84, natural padding85..87
    SmartPtr<ComboBox> mTabSelectCombo; //88
    SmartPtr<ComboBox> mConnectionSpeedCombo; //8c
    SmartPtr<Button> mCrossPromotionCheck; //90
    SmartPtr<Button> mLobbySoundEffectsCheck; //94
    SmartPtr<Button> mChatSoundEffectsCheck; //98
    SmartPtr<Button> mStagingSoundEffectsCheck; //9c
    SmartPtr<Button> mLobbyMusicCheck; //a0
    SmartPtr<Button> mShowConfirmationsCheck; //a4
    SmartPtr<Button> mShowGamesInProgressCheck; //a8
    SmartPtr<Button> mAnonymousToFriendsCheck; //ac
    SmartPtr<Button> mAutoPingGamesCheck; //b0
    SmartPtr<Label> mLocalAddressDetailLabel; //b4
    SmartPtr<Label> mPublicAddressDetailLabel; //b8
    SmartPtr<ImageComponent> mFirewalledIcon; //bc, actual RTTI4d66c0
    SmartPtr<ComboBox> mUserNameCombo; //c0
    SmartPtr<Label> mLANBroadcastDetailLabel; //c4
    SmartPtr<Button> mFilterDirtyWordsCheck; //c8
    SmartPtr<Button> mDoAutoAFKCheck; //cc; allocationd0
};
}


// Proposed complete family: FriendsListCtrl.h.draft
// SCRATCH full family contract, canonical TextListItem fields/slots pending.
namespace WONAPI {
class FriendsListItem : public TextListItem {
public:
    FriendsListItem();
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void Virtual04(Graphics& inGraphics,ListArea* inList);
    virtual void Virtual08(ListArea* inList);
    virtual int Virtual10(const ListItem* inOther,ListArea* inList) const;
    SmartPtr<LobbyFriend> mFriend; //28
    bool mSeparator; //2c
    signed char mSortGroup; //2d;allocation30
};
class FriendsListCtrl : public Container {
public:
    static ComponentConfig* CfgFactory(); //365320 true SDK external factory
    static Component* CompFactory(); //3652c0 true SDK external factory
    FriendsListCtrl();
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void UpdateSeparator();
    void UpdateFriend(LobbyFriend* inFriend,FriendsListItem* inItem);
    FriendsListItem* FindFriend(LobbyFriend* inFriend);
    LobbyFriend* GetSelectedFriend();
    void HandleFriendChanged(FriendChangedEvent* inEvent);
    void RefreshFriends();
    void FillFriends();
    void UpdateButtons();
    SmartPtr<ListArea> mFriendsList; //84
    SmartPtr<Button> mRemoveButton; //88
    SmartPtr<Button> mLocateButton; //8c
    SmartPtr<FriendsListItem> mSeparator; //90;allocation94
};
}


// Proposed complete family: LostControls.h.draft
// SCRATCH whole narrow LostPasswordCtrl/LostUserNameCtrl families.
namespace WONAPI {
class LostPasswordCtrl : public Container {
public:
    LostPasswordCtrl(); //3c0320; empty source-kind candidate, zero primary credit until COFF
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit); //3c0400
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3c0520
    void UpdateRetrieveButton(); //3c0460
    void FillUserNames(); //3c0490
    SmartPtr<ComboBox> mUserNameCombo; //84
    SmartPtr<Button> mRetrieveButton; //88
};
class LostUserNameCtrl : public Container {
public:
    LostUserNameCtrl(); //3c0750; empty source-kind candidate, zero primary credit until COFF
    // Implicit inherited virtual destructor; original no own-vptr store, zero primary credit.
    virtual void InitComponent(ComponentInit& inInit); //3c0830
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3c0930
    void UpdateRetrieveButton(); //3c0890
    SmartPtr<InputBox> mEmailInput; //84
    SmartPtr<Button> mRetrieveButton; //88
};
}


// Proposed complete family: CreateAccountCtrl.h.draft
// Scratch complete derived fields; no allocation/production/compile authority.
// Actual RTTI CreateAccountCtrl>Container>Component>RefCount, VT452e3c, sizebc.
namespace WONAPI {
class CreateAccountCtrl : public Container {
public:
    CreateAccountCtrl(); //3ba290, empty14ownedPtr constructor candidate, zeroquota
    // Implicit virtual normal destructor3ba320,455B, no own-vptr; zeroquota.
    virtual void InitComponent(ComponentInit& inInit); //3ba4f0,394B
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3ba990,639B
    bool Fn3BA680(bool inVisible, Label* inLabel); //24B, this unused; pinnednominalSetDesc
    bool Fn3BA790(InputBox* inMonth,InputBox* inDay,InputBox* inYear); //112B,this unused
    void UpdateCreateButton(); //3ba800,393B
    SmartPtr<InputBox> mUserNameInput; //84
    SmartPtr<InputBox> mPasswordInput; //88
    SmartPtr<InputBox> mConfirmPasswordInput; //8c
    SmartPtr<InputBox> mEmailInput; //90
    SmartPtr<InputBox> mMonthInput; //94
    SmartPtr<InputBox> mDayInput; //98
    SmartPtr<InputBox> mYearInput; //9c
    SmartPtr<Button> mNewsletterCheck; //a0
    SmartPtr<Label> mUserNameDesc; //a4
    SmartPtr<Label> mPasswordDesc; //a8
    SmartPtr<Label> mConfirmPasswordDesc; //ac
    SmartPtr<Label> mEmailDesc; //b0
    SmartPtr<Label> mBirthdateDesc; //b4
    SmartPtr<Button> mCreateButton; //b8
};
}


// Proposed complete family: ChatCtrl.h.draft
// SCRATCH full controller fields; global ChatLink is private to its actual CPP.
class ChatLink;
namespace WONAPI {
class ChatCtrl : public Container {
public:
    ChatCtrl(); //3b6b80
    virtual ~ChatCtrl(); //actualownVTstore3b6cbf; emptybodycandidatezeroquota
    virtual void InitComponent(ComponentInit& inInit); //3b6dd0
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3b9c80
    void Fn3B72E0(AnnounceClientLocationEvent* inEvent);
    void Fn3B7660(AnnounceFriendEvent* inEvent);
    void Fn3B7C60(GroupInvitationEvent* inEvent);
    void Fn3B7EC0(GroupJoinAttemptEvent* inEvent);
    void Fn3B81F0(PlayerTeamChangedEvent* inEvent);
    void Fn3B82F0(SetChatInputEvent* inEvent);
    void Fn3B85B0(RecvChatEvent* inEvent);
    void Fn3B9680(AlertEvent* inEvent);
    void Fn3B88F0(JoinedRoomEvent* inEvent);
    void Fn3B89A0(LeftRoomEvent* inEvent);
    void Fn3B8A20(ChatCommandParser* inParser);
    void Fn3B8BF0();
    void Fn3B8BD0(ChatCommandErrorEvent* inEvent);
    void Fn3B9240(ClientModeratedEvent* inEvent);
    bool Fn3B6F00();
    void Fn3B6F10(const GUIString& inFormat,const GUIString*const* inStrings,int inNumStrings,
        TextSegmentBase*const* inSegments,int inNumSegments,bool inLineBreak);
    void Fn3B6FD0(const GUIString& inFormat,const GUIString& inText,bool inLineBreak);
    void Fn3B7000(const GUIString& inFormat,TextSegmentBase* inSegment,bool inLineBreak);
    SmartPtr<ChatLink> Fn3B7030(LobbyClient* inClient);
    void Fn3B9910(TextLinkActivatedEvent* inEvent);
    void Fn3B98A0(ChatLink* inLink,int inColor,int inLinkedColor);
    SmartPtr<Font> mFont84;
    SmartPtr<Font> mFont88;
    int mDefaultColor; int mUserColor; int mWhisperColor; int mEmoteColor; int mWarningColor;
    int mErrorColor; int mHelpColor; int mGroupColor; int mMessageColor; int mAcceptColor; int mRejectColor;
    SmartPtr<InputBox> mChatInput; //b8
    SmartPtr<Button> mSendButton; //bc
    SmartPtr<TextArea> mChatOutput; //c0, pointer only
    int mRoomSpecFlags; //c4
    unsigned short mClientId; //c8
    GUIString mRoomName; //cc
};
}

#endif
