// Original-backed WONLobby dialog contracts. The first three classes have full allocation/layout proof.
// LanName/AddFriend/Password each allocate a4 bytes: Dialog98 plus three owning widget pointers.
#ifndef EE_WONAPI_WONLOBBYDIALOGS_H
#define EE_WONAPI_WONLOBBYDIALOGS_H
namespace WONAPI {
class LanNameDialog : public Dialog {
public:
    LanNameDialog();
    virtual ~LanNameDialog(); //explicit original3f8400 writes derived vptr
    virtual void InitComponent(ComponentInit& inInit);
    void Reset();
    const GUIString& GetName(); //actual external nonconst getter3f90c0; SDKLanNameDialog
    SmartPtr<InputBox> mNameInput; //98
    SmartPtr<Button> mOkButton; //9c
    SmartPtr<Button> mCancelButton; //a0
};
class PasswordDialog : public Dialog {
public:
    PasswordDialog();
    // Original3f8f70 destructor is implicit: no own vptr store. No forced empty body.
    virtual void InitComponent(ComponentInit& inInit);
    void Reset(); //shared physical3f8560; actual Password caller/field98 proof, zero duplicate credit
    const GUIString& GetPassword();
    SmartPtr<InputBox> mPasswordInput; //98
    SmartPtr<Button> mOkButton; //9c
    SmartPtr<Button> mCancelButton; //a0
};
class AddFriendDialog : public Dialog {
public:
    AddFriendDialog();
    // Original3f48a0 destructor is implicit: no own vptr store. No forced empty body.
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void CheckInput();
    void Reset();
    const GUIString& GetName() { return mNameInput->GetText(); }
    SmartPtr<InputBox> mNameInput; //98, raw mText is used by CheckInput
    SmartPtr<Button> mOkButton; //9c
    SmartPtr<Button> mCancelButton; //a0
};
// Reference and external-lifetime contracts below carry explicit per-class restrictions.
// Partial pointees remain unallocated; no private/widget lifetime implementations are supplied.
// Complete real ScrollingTextComponent54 fields; genuine explicit SDK ctor declared below.
// Header API approval alone does not admit follow-on Source; external destructor retained.
class CreditLineInfo {
protected:
    GUIString mLine; //00
    DWORD mColor; //04
public:
    CreditLineInfo(const GUIString& text,DWORD color) {mLine=text;mColor=color;}
    GUIString GetLine(); //actual3f7450, owned GUIString result; original external support0
    DWORD GetColor() {return mColor;}
};
typedef std::list<CreditLineInfo> CreditsList;
class ScrollingTextComponent : public Component {
protected:
    SmartPtr<Font> mFont; //30; exact primary SDK type, known real Font owner
    CreditsList mLines; //34 allocator,38head,3c count; native node10/value8
    DWORD mLastColor; //40
    int mTimeSinceLastScroll; //44
    int mScrollSpeed; //48
    int mScrollTop; //4c
    int mTotalTextHeight; //50
public:
    ScrollingTextComponent(); //3f6de0 true SDK explicit initialization, complete54
    virtual ~ScrollingTextComponent(); //external actual3f6e90
    virtual void Paint(Graphics&); //actual18=3f71b0, discoverygap672
    virtual bool Virtual44(int); //actual44=3f70b0
    void Start(); //3f6f50 actualSDK timer activation/reset19B
    void Stop(); //3f6f70 actualSDK timer stop8B
    void Reset(); //3f6f80 actualSDK scrollTop=height7B
    void AddLine(const GUIString&,unsigned long); //actual3f6f90,ret8
};
// Complete Credits owna4 tail; constructor/implicit normal destructor remain original external calls.
// No ScrollingTextComponent allocation or lifetime implementation is authorized.
class CreditsDialog : public Dialog {
protected:
    void UpdateCredits(); //native SDK identity; actual Init calls shared empty4142a0, call-only0
public:
    CreditsDialog();
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*); //external actual shared3fa1f0
    void StartScrolling();
    void StopScrolling();
    void ResetScrolling();
    void AddLine(const GUIString&,Component*,unsigned long);
    SmartPtr<ScrollingTextComponent> mScrollingText; //98
    SmartPtr<Button> mCreditsButton; //9c
    SmartPtr<Button> mCloseButton; //a0
};
class GotHTTPDocumentEvent;
class TOUDialog : public Dialog {
public:
    TOUDialog(); //fullac, simple false-state/default ownership initializer support0
    // Actual normal destructor implicit; no forced declaration/definition.
    virtual void Paint(Graphics&); //existing actual reference slot18, support0
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    void HandleHTTPDocEvent(GotHTTPDocumentEvent*);
    void Reset();
    SmartPtr<BrowserComponent> mBrowser; //98, original external widget lifetime
    SmartPtr<Button> mAgreeCheck; //9c
    SmartPtr<Button> mAcceptButton; //a0
    SmartPtr<Button> mRejectButton; //a4
    bool mNeedGenHTML; //a8, natural paddinga9..ab
};
class ChooseNetworkAdapterDialog : public Dialog {
public:
    ChooseNetworkAdapterDialog(); //fulla0; empty/default owner support0
    virtual ~ChooseNetworkAdapterDialog(); //actual explicit normal3f9c80, source gate separately
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*); //actual shared delegate3fa1f0,0
    void Reset();
    unsigned long GetSelectedAddress(); //existing canonical rawIPv4 ABI retained, getter0
    SmartPtr<MultiListArea> mAdapterList; //98
    SmartPtr<Button> mActivateButton; //9c
};
// Complete external-lifetime contracts below permit exact caller allocation only.
// Constructors/destructors remain original external calls; no PopupMenu allocation or body is defined here.
class LobbyGame;
// Completec0: Dialog98 plus nine owning widget pointers and int game type.
class CreateGameDialog : public Dialog {
protected:
    virtual ~CreateGameDialog(); //3f6450 explicit original external lifetime
public:
    CreateGameDialog();
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void Reset(bool inClear);
    void SetGameType(int inType);
    SmartPtr<LobbyGame> GetGame();
    void CheckInput();
    void UpdatePasswordVisibility();
    SmartPtr<InputBox> mNameInput;
    SmartPtr<ComboBox> mSkillCombo;
    SmartPtr<InputBox> mPasswordInput;
    SmartPtr<Label> mPasswordLabel;
    SmartPtr<Button> mOpenToAllRadio;
    SmartPtr<Button> mPasswordRadio;
    SmartPtr<Button> mInviteOnlyRadio;
    SmartPtr<Button> mAskToJoinRadio;
    SmartPtr<Button> mCreateButton;
    int mGameType;
};

class PopupMenu;
class LobbyClient;
class ClientOptionsPopup : public Container {
protected:
    SmartPtr<PopupMenu> mPopupMenu; //84
    SmartPtr<LobbyClient> mClient; //88
    int mRoomSpecFlags; //8c, constructor does not initialize this scalar
    int GetRoomSpecFlags() const { return mRoomSpecFlags; }
    void AddItem(const GUIString&,int); //actual3f5ea0, forwarding support0; class-correct receiver
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3f6310, slot7c
    virtual ~ClientOptionsPopup(); //3f5e00, explicit original external
public:
    ClientOptionsPopup(); //3f5d10, external, fullobject90
    void Init(LobbyClient* inClient, LobbyClient* inMyClient, int inRoomSpecFlags); //3f5ec0, retc
};

// Original allocation8c and raw slots/field proof independently checked; SDK names supplement identity only.
class PopupMenu;
class GameOptionsPopup : public Container {
protected:
    SmartPtr<LobbyGame> mGame; //84, owned
    SmartPtr<PopupMenu> mPopupMenu; //88, owned; no allocation or sizeof of PopupMenu here
    void AddItem(const GUIString&,int); //actual3f80f0, forwarding support0; class-correct receiver
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3f82f0, slot7c
    virtual ~GameOptionsPopup(); //3f8050; external original lifetime remains carved
public:
    GameOptionsPopup(); //3f7f60; external original, full object8c
    void Init(LobbyGame* inGame); //3f8110; nominal name independently from retained SDK header
};

// Original-backed dialog field and slot contracts; widget lifetimes remain external.

// Header addition draft, needs actual GUI callback and parsed integer contracts.
class LanPortDialog : public Dialog {
public:
    LanPortDialog();
    virtual ~LanPortDialog();
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void Reset();
    void SetPort(unsigned short inPort);
    void SetGamePort(unsigned short inPort);
    unsigned short GetPort();
    void CheckInput();
    SmartPtr<InputBox> mPortInput;
    SmartPtr<Label> mGamePortLabel;
    SmartPtr<Button> mOkButton;
    unsigned short mGamePort;
};

class OkCancelDialog : public Dialog {
public:
    OkCancelDialog();
    virtual void InitComponent(ComponentInit& inInit);
    void SetText(const GUIString& inTitle, const GUIString& inBody,
                 const GUIString& inOkText, const GUIString& inCancelText);
    SmartPtr<Label> mTitleLabel;
    SmartPtr<Label> mBodyLabel;
    SmartPtr<Button> mOkButton;
    SmartPtr<Button> mCancelButton;
    SmartPtr<Button> mCloseButton;
    SmartPtr<Component> mDialogComponent;
    SmartPtr<WONFrameComponent> mBack;
};

class StatusDialog : public Dialog {
public:
    StatusDialog();
    virtual void InitComponent(ComponentInit& inInit);
    void SetStatus(const GUIString& inStatus, bool inCancel);
    void SetText(const GUIString& inTitle, const GUIString& inStatus, bool inCancel);
    SmartPtr<Label> mTitleLabel;
    SmartPtr<Button> mCloseButton;
    SmartPtr<Label> mStatusLabel;
    SmartPtr<Button> mCancelButton;
    SmartPtr<Component> mDialogComponent;
    SmartPtr<WONFrameComponent> mBack;
    unsigned long mStartTick;
};

class DirectConnectDialog : public Dialog {
public:
    DirectConnectDialog();
    virtual void InitComponent(ComponentInit& inInit);
    void Reset();
    const GUIString& GetAddress();
    const GUIString& GetName(); //shared physical getter3f90c0; root alias gate pending
    SmartPtr<InputBox> mNameInput;
    SmartPtr<InputBox> mAddressInput;
    SmartPtr<Button> mAddButton;
    SmartPtr<Button> mCancelButton;
};

class CreateRoomDialog : public Dialog {
public:
    CreateRoomDialog();
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void Reset(bool inClear);
    void UpdatePasswordVisibility();
    void CheckInput();
    const GUIString& GetPassword();
    const GUIString& GetRoomName(); //nominal SDK; original physical getter3f90c0, root alias gate pending
    SmartPtr<InputBox> mNameInput;
    SmartPtr<InputBox> mPasswordInput;
    SmartPtr<Label> mPasswordLabel;
    SmartPtr<Button> mPasswordCheck;
    SmartPtr<Button> mCreateButton;
};

// Complete original0xc8 own-tail contract; external widgets remain reference-only.
class AdminActionDialog : public Dialog {
public:
    AdminActionDialog();
    virtual ~AdminActionDialog();
    virtual void InitComponent(ComponentInit& inInit);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void UpdateDoActionButton();
    void Reset(int inAction, LobbyClient* inTarget);
    bool GetApplyToServer();
    const GUIString& GetExplanation();
    unsigned long GetNumSeconds();
    int mActionType; //98, existing canonical int ABI retained
    SmartPtr<Label> mTitleLabel; //9c
    SmartPtr<Label> mExplanationHeader; //a0
    SmartPtr<InputBox> mExplanation; //a4
    bool mIsReasonOptional; //a8, natural paddinga9..ab
    SmartPtr<Label> mDurationHeader; //ac
    SmartPtr<ComboBox> mDurationCombo; //b0
    SmartPtr<ComboBox> mTimeSegmentCombo; //b4
    SmartPtr<Button> mDoActionButton; //b8
    SmartPtr<Button> mCancelButton; //bc
    SmartPtr<Button> mApplyToRoom; //c0
    SmartPtr<Button> mApplyToServer; //c4
};

class RoomCtrl; //complete90 in sole owner WONLobbyRoomServer.h; no duplicate class
// Full original9c own tail. External ctor/lifetime declarations do not supply implementations.
class RoomDialog : public Dialog {
protected:
    SmartPtr<RoomCtrl> mRoomCtrl; //98
    virtual ~RoomDialog(); //native SDK protected lifetime; source-kind gate remains separate
public:
    RoomDialog(); //external3f90d0, empty/default owner support0
    virtual void InitComponent(ComponentInit&); //actual slot1c shared empty2fa500,0
};

struct RoutingBadUser; //Model owns the record definition; reference-only here
// Complete original0xac own tail; actual51 slots differ only00/1c.
// The original normal destructor is implicit (no own vptr store); no written destructor body.
class BadUserDialog : public Dialog {
public:
    typedef std::list<unsigned long> IdList; //actual native nested alias
    BadUserDialog();
    virtual void InitComponent(ComponentInit& inInit); //3f5a90
    void SetUserList(const std::list<RoutingBadUser>& inList, bool inSimple); //3f5b20
    void GetIds(IdList& outIds); //3f5c20
    SmartPtr<MultiListArea> mUserList; //98
    SmartPtr<ListArea> mSimpleUserList; //9c
    SmartPtr<Component> mUserListBox; //a0
    SmartPtr<Component> mSimpleUserListBox; //a4
    bool mIsSimple; //a8, natural paddinga9..ab
};

// PARTIAL REFERENCE-ONLY CreditsDialogConfig: true14slots and borrowed9c field.
// No constructor/destructor implementation, allocation, sizeof, copy or by-value authority.
class CreditsDialogConfig : public DialogConfig {
public:
    static ComponentConfig* CfgFactory() { return new CreditsDialogConfig; } //genuine SDK inline, native365260/83 completea0
    virtual bool Virtual24(const std::string& inInstruction); //3f7780 actualADDLINE behavior
    virtual void Virtual34(Component* inComponent); //38f100 actual25B setter bridge,0quota
    CreditsDialog* mCredits; //9c, borrowed; baseDialogConfig9c actual layout
};

class LobbyFriend;
// Complete90 external-lifetime Friend popup; no PopupMenu allocation or implementation is authorized.
// Native45626851 slots differs Container only00/7c; peer E_FRIEND_HEADER_PEER.json.
class FriendOptionsPopup : public Container {
protected:
    SmartPtr<PopupMenu> mPopupMenu; //84, owning
    SmartPtr<LobbyFriend> mFriend; //88, owning
    int mRoomSpecFlags; //8c, original external ctor intentionally leaves uninitialized
    void AddItem(const GUIString&,int); //actual3f5ea0, forwarding support0; class-correct receiver
    virtual void HandleComponentEvent(ComponentEvent*); //external3f7ea0
    virtual ~FriendOptionsPopup(); //explicit original external3f7c50
public:
    FriendOptionsPopup(); //external3f7b60; native child180 stays entirely original
    void Init(LobbyFriend*,int); //external3f7cf0,ret8; full DWORD room flags
};

}
#endif
