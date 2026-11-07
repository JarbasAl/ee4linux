#ifndef EE_WONAPI_WONLOBBYSCREENS_H
#define EE_WONAPI_WONLOBBYSCREENS_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
class JoinedRoomEvent;
class LeftRoomEvent;
class ClientChangedEvent;
// Full30: actual new30/ctor3ca6a0, TextListItem28 plus client28/sort2c/show2d.
class UserListItem : public TextListItem {
public:
    UserListItem();
    virtual void Virtual04(Graphics&,ListArea*);
    virtual void Virtual08(ListArea*);
    virtual int Virtual10(const ListItem*,ListArea*) const;
    SmartPtr<LobbyClient> mClient; //28
    signed char mSortKey; //2c: actual comparison uses MOVSX
    bool mShowSeparator; //2d; ctor true, visibility updates strict0/1
};
// Fulla0 member ownership matches original ctor3ca8f0/cleanup3ca960.
class UserListCtrl : public Container {
public:
    UserListCtrl();
    virtual ~UserListCtrl(); // normal3ca960 actual own-vptr store3ca97f,264B
    virtual void InitComponent(ComponentInit&); //3cadf0
    virtual void HandleComponentEvent(ComponentEvent*); //3cb4d0
    void UpdateItem(LobbyClient*,UserListItem*); //3caa70
    void RefreshSeparators(); //3cacf0
    UserListItem* FindItem(LobbyClient*); //3cb0e0
    UserListItem* GetSelectedItem(); //3cb130
    LobbyClient* GetSelectedClient(); //3cb140
    void UpdateButtons(); //3cb160
    void JoinedRoom(JoinedRoomEvent*); //3cb200
    void LeftRoom(LeftRoomEvent*); //3cb360
    void ClientChanged(ClientChangedEvent*); //3cb3a0
    bool mUpdating; //84
    int GetRoomSpecFlags() const { return mModeFlags; }
    int mModeFlags; //88, ctor5; pinned nominal UserListCtrl.h room-spec flags
    SmartPtr<ListArea> mUserList; //8c
    SmartPtr<UserListItem> mFirstSeparator; //90
    SmartPtr<UserListItem> mSecondSeparator; //94
    SmartPtr<Button> mWhisperButton; //98
    SmartPtr<Button> mIgnoreButton; //9c
};
Image* GetLobbyClientIcon(LobbyClient*,bool); //3cac50, substantive cdecl selection
// Full QuickPlayCtrl b0: original ctor3c14e0/baseDialog2f7910, VT453bf0
// has51 slots; six owning members98..ac, original normalcleanup3c1540.
class QuickPlayCtrl : public Dialog {
public:
    QuickPlayCtrl();
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    void RefreshUserName();
    SmartPtr<Label> mWelcomeLabel; //98
    SmartPtr<Button> mNoviceRadio; //9c
    SmartPtr<Button> mIntermediateRadio; //a0
    SmartPtr<Button> mAdvancedRadio; //a4
    SmartPtr<Button> mExpertRadio; //a8
    SmartPtr<Button> mPlayButton; //ac
};
// Complete GameBrowserCtrl b8: native new36539d, ctor3bbdc0, implicit normal cleanup0.
class BrowserComponent;
class GameBrowserCtrl : public Container {
public:
    GameBrowserCtrl(const GUIString& = "");
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    virtual void Paint(Graphics&);
    void EnableButtons();
    void DoFind(bool);
    void Fn3BC2F0();
    void GoHomeIfReset();
    void GoHome();
    void SetHomeLocation(const GUIString& theLocation) { mHomeLocation=theLocation; } //genuine SDK inline; actualownedGUIStringb4
    SmartPtr<BrowserComponent> mBrowser;
    SmartPtr<Button> mBackButton,mForwardButton,mStopButton,mHomeButton,mRefreshButton;
    SmartPtr<InputBox> mFindInput;
    SmartPtr<Button> mFindNextButton,mFindPrevButton,mCaseSensitiveCheck;
    SmartPtr<Label> mStatusLabel;
    bool mIsReset;
    GUIString mHomeLocation;
};
// Root D: independently source/ABI-reviewed GameUser and UpdateAccount families.
class QueryAccountResultEvent;
void DrawLobbyPing(Graphics*,unsigned short,int,int,int); // native3c12b0 call-only, no body credit
// Full24 item: ListItem18 plus three owned pointer fields.
class GameUserListItem : public ListItem {
public:
    GameUserListItem(LobbyClient*);
    virtual void Virtual04(Graphics&,ListArea*);
    virtual void Virtual08(ListArea*);
    virtual int Virtual10(const ListItem*,ListArea*) const;
    void UpdateIcon();
    SmartPtr<LobbyClient> mClient;
    SmartPtr<Image> mImage;
    SmartPtr<Font> mFont;
};
class MultiListArea;
class PlayerChangedEvent;
// Full94: Container84, two owned pointers, room flags8c and active90.
class GameUserListCtrl : public Container {
public:
    GameUserListCtrl();
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    LobbyClient* GetSelectedClient();
    GameUserListItem* GetListItem(LobbyClient*);
    void HandleKickPlayer(bool);
    void UpdateKickButton();
    void HandlePlayerChanged(PlayerChangedEvent*);
    void HandleClientChanged(ClientChangedEvent*);
    void HandleJoinedGame();
    void HandleLeftGame();
    SmartPtr<MultiListArea> mUserList;
    SmartPtr<Button> mKickButton;
    int GetRoomSpecFlags() const { return mRoomSpecFlags; }
    int mRoomSpecFlags;
    bool mInGame;
};

// Fullbc original-backed external-base reconstruction: Container84 +13 owning controls +GUIStringb8.
class UpdateAccountCtrl : public Container {
public:
    UpdateAccountCtrl();
    virtual ~UpdateAccountCtrl();
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    static bool SetDesc(bool,Label*);
    void CheckInput();
    void HandleEnteredScreen();
    void HandleQueryAccountResult(QueryAccountResultEvent*);
    SmartPtr<InputBox> mUserNameInput,mOldPasswordInput,mPasswordInput,mConfirmPasswordInput,mEmailInput;
    SmartPtr<Button> mNewsletterCheck;
    SmartPtr<Label> mUserNameDesc,mOldPasswordDesc,mPasswordDesc,mConfirmPasswordDesc,mEmailDesc;
    SmartPtr<Button> mQueryButton,mUpdateButton;
    GUIString mActualOldPassword;
};
// Complete screen consumers; actual external GUI bases and Browser lifetime remain owner-managed.
class GetHTTPDocumentOp;
class LoginScreen : public Container {
public:
    LoginScreen();
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    void ShowMOTD();
    void ShowCreateAccountCtrl();
    void ShowLostPasswordCtrl();
    void ShowLostUsernameCtrl();
    SmartPtr<ScreenContainer> mRightPanel; //84
    SmartPtr<Component> mMOTDCtrl; //88
    SmartPtr<Component> mCreateAccountCtrl; //8c
    SmartPtr<Component> mLostUserNameCtrl; //90
    SmartPtr<Component> mLostPasswordCtrl; //94
    SmartPtr<Component> mLoginHelpCtrl; //98
};

class BrowserComponent;
class LoginHelpCtrl : public Container {
public:
    LoginHelpCtrl();
    virtual void InitComponent(ComponentInit&);
    virtual void Paint(Graphics&);
    SmartPtr<BrowserComponent> mBrowser; //84
    bool mHaveLoadedHelp; //88, full8c
};

class BrowserComponent;
class MOTDCtrl : public Container {
public:
    MOTDCtrl();
    virtual ~MOTDCtrl();
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    virtual void Paint(Graphics&);
    void ShowMessage(GetHTTPDocumentOp*);
    SmartPtr<BrowserComponent> mBrowser; //84
    std::string mGameMOTDPath; //88
    std::string mSysMOTDPath; //98
    bool mNeedGenHTML; //a8, fullac
};

// Complete LobbyContainer94; RootScreen remains forward-only to avoid include cycle.
class RootScreen;
class LobbyContainer : public ScreenContainer {
public:
    LobbyContainer();
    virtual ~LobbyContainer();
    virtual void InitComponent(ComponentInit&);
    void ShowInternetScreen();
    void ShowLanScreen();
    void ShowLoginScreen();
    bool OnInternetScreen();
    bool OnLanScreen();
    bool OnLoginScreen();
    int GetGameScreenType();
    SmartPtr<RootScreen> mInternetScreen; //88
    SmartPtr<RootScreen> mLanScreen; //8c
    SmartPtr<LoginScreen> mLoginScreen; //90; full94
};

}
#endif
