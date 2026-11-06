// Original RoomLabelCtrl complete0xa0, actual Container0x84/51slots.

#ifndef EE_WONAPI_WONLOBBYROOMSERVER_H
#define EE_WONAPI_WONLOBBYROOMSERVER_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyPersistence.h" //actual NoCase static-column-map comparator
namespace WONAPI {
class RoomLabelCtrl : public Container {
public:
    RoomLabelCtrl(); //3c32b0
    // Implicit normal3c33a0: full original has no own-vptr write.
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    void CalcLabel();
    int mRoomSpecFlags; //84
    SmartPtr<Label> mLabel; //88
    GUIString mPrefix; //8c
    GUIString mServer; //90
    GUIString mRoom; //94
    int mUserCt; //98
    bool mShowServer; //9c
};
}
// Reference-only widget contracts scoped to RootScreen. All actual RTTI bases/tables
// recovered in identities.json; no allocation/construction/sizeof/copy/byvalue/ownVT use.
namespace WONAPI {
// Complete actualMSSeperator30: fieldlessComponent30, nativeAddSeperator new30.
// Overrides inherited reference-only default for thisoneclass; originalexternal lifetime.
class MSSeperator : public Component {
public:
    MSSeperator(int=6); //3006a0 originalexternal constructor
    virtual void Virtual0C(int& width,int& height) { width=100; height=6; } //genuine SDK inline GetDesiredSize, native300700/23
    virtual void Paint(Graphics&); //300720 actual18slot
};
class RectangleComponent3D : public Component {
public:
    RectangleComponent3D(bool up=true,int thickness=2); //3068f0 genuine SDK complete44
    virtual void Paint(Graphics&); //306930 genuine SDK override, existing slot18
    // Genuine SDK implicit destructor; natural Component cleanup, no own resource.
    int mSolidColor,mLightColor,mDarkColor; //30,34,38 actual SDK/native394c3b/394c5f/394c83
    int mThickness; //3c, targetRoot3c47fa+primaryGetThickness nominal
    bool mUp; //40, actual native394ca7; complete44 scalar owner
    void SetSolidColor(int c) { mSolidColor=c; }
    void SetLightColor(int c) { mLightColor=c; }
    void SetDarkColor(int c) { mDarkColor=c; }
    void SetThickness(int n) { mThickness=n; }
    void SetIsUp(bool up) { mUp=up; }
};
}

// Complete original RootScreen0xcc. Requires pointer-only widget reference contracts
// (TabCtrl/MSSeperator/RectangleComponent3D) and screens-owned GameBrowserCtrl.
namespace WONAPI {
class RootScreen : public Container {
public:
    RootScreen(bool);
    virtual ~RootScreen(); // own-vptr3c3d2f, actual10member+base cleanup
    virtual void InitComponent(ComponentInit&); //3c3f40
    virtual void Virtual08(); //3c46d0, actual SizeChanged inheritedslot08
    virtual void HandleComponentEvent(ComponentEvent*); //3c4ae0
    bool OnDirectConnectTab(); //3c3ec0
    bool OnLanTab(); //3c3f00
    void EnterGameTab(bool); //3c4880
    void ShowLobbyTab(); //3c4930
    void HandleJoinedGame(); //3c4950
    void HandleLeftGame(); //3c4980
    void HandleEnteredScreen(); //3c4990
    void HandleTabCtrlTabSelected(ComponentEvent*); //3c49d0
    bool mIsLan; //84
    SmartPtr<TabCtrl> mTabCtrl; //88
    GUIString mDefaultTabName; //8c
    SmartPtr<GameBrowserCtrl> mHelpTab; //90
    SmartPtr<GameBrowserCtrl> mNewsTab; //94
    SmartPtr<Component> mLowerLayoutPoint; //98
    SmartPtr<MSSeperator> mLowerDivider; //9c
    SmartPtr<Component> mSierraLogo; //a0
    SmartPtr<Component> mLowerBanner; //a4
    SmartPtr<RectangleComponent3D> mLowerBannerFrame; //a8
    SmartPtr<Component> mBackButton; //ac
    bool mHideLowerControls; //b0
    bool mIsSwat; //b1
    int mLastJoinTab; //b4
    int mGameTab; //b8, deliberately not initialized byctor
    int mDirectConnectTab; //bc
    int mLanTab; //c0
    int mLobbyTab; //c4
    int mRoomSpecFlags; //c8
};
}

// E domain contracts: Tree classes are reference-only; Room classes complete.
// Reference-only actual TreeItem3c prefix; NEVER new/sizeof/ctor/copy/byvalue/ownVT.
// 1c is deliberately not interpreted. Full external lifetime3140f0/314170.
namespace WONAPI {
class ServerChangedEvent;
class GroupChangedEvent;
// Tree records now live in the common GUI header with existing Fn names/fields preserved.
class RoomCtrl : public Container {
public:
    RoomCtrl(); // empty initialization body38 support0
    // Implicit destructor184 support0; no own-vptr write.
    virtual void InitComponent(ComponentInit&);
    virtual void HandleComponentEvent(ComponentEvent*);
    void EnableControls();
    void DoJoinRoom();
    void DoCreateRoom();
    TreeItem* GetServerItem(const IPAddr&);
    TreeItem* GetServerItem(LobbyServer*);
    TreeItem* GetGroupItem(const IPAddr&,int);
    TreeItem* GetGroupItem(LobbyServer*,LobbyGroup*);
    void AddServer(LobbyServer*,bool,int);
    void AddGroup(TreeItem*,LobbyServer*,LobbyGroup*);
    void HandleSetRoomList();
    void HandleServerChanged(ServerChangedEvent*);
    void HandleGroupChanged(GroupChangedEvent*);
    void HandleItemOpened(TreeItemOpenedEvent*);
    SmartPtr<TreeArea> mRoomList; //84
    SmartPtr<Button> mJoinButton; //88
    SmartPtr<Button> mCreateButton; //8c
}; // complete90, actualContainer84/51slots
}

// Global nominal type is proved by RTTI .?AVRoomListItem@@ (not WONAPI namespace).
// Complete0x58, original ListItem14slot base0x18. No unknown ownership fields.
class RoomListItem : public WONAPI::ListItem {
public:
    RoomListItem(WONAPI::LobbyServer*,bool); //3c19a0
    RoomListItem(WONAPI::LobbyGroup*,WONAPI::LobbyServer*); //3c1c50: group FIRST
    // Implicit normal3c1b70 contains no own-vptr store; support only.
    virtual void Virtual04(WONAPI::Graphics&,WONAPI::ListArea*); //3c20c0
    virtual void Virtual08(WONAPI::ListArea*); //3c1fd0
    virtual int Virtual10(const WONAPI::ListItem*,WONAPI::ListArea*) const; //3c20a0
    void Fn3C1D40(WONAPI::LobbyServer*,WONAPI::LobbyGroup*);
    int mColor; //18
    bool mLoading; //1c
    WONAPI::SmartPtr<WONAPI::Image> mImage; //20
    WONAPI::SmartPtr<WONAPI::Font> mFont; //24
    WONAPI::GUIString mText; //28
    WONAPI::IPAddr mAddress; //2c
    unsigned long mGroupId; //54, group id zeroextends from ushort, server sentinelffff
};


// Scratch complete original ServerListCtrl0xc8. Shared ServerDetails28/16-slot migration pending Root.
// Actual constructor3c5c10, explicit normal3c5d30 own-vptr; vector84/mapbc own real elements.
namespace WONAPI {
class ComboSelChangedEvent;
class GameChangedEvent;
class SyncGameListEvent;
class ServerListCtrl : public Container {
public:
    struct ColumnInfo {
        typedef void (*PaintFunc)(Graphics&,ServerDetailsItem*,ServerListCtrl*);
        typedef int (*CompareFunc)(const ServerDetailsItem*,const ServerDetailsItem*,ServerListCtrl*);
        typedef void (*FilterFunc)(const GUIString&,std::list<ServerDetailsItem*>&,ServerListCtrl*);
        PaintFunc mPaintFunc; //00
        CompareFunc mCompareFunc; //04
        FilterFunc mFilterFunc; //08
        SmartPtr<ComboBox> mFilterCombo; //0c
        unsigned char mGameTypeFlags; //10
        GUIString mColumnName; //14
        ColumnInfo(); //3c5f80
        // Implicit normal3c5ef0 support0, no classvptr.
    }; //complete18
    typedef std::vector<ColumnInfo> ColumnInfoVector;
    typedef std::map<GUIString,ColumnInfo,GUIStringLessNoCase> ColumnMap;
    typedef std::list<ServerDetailsItem*> ServerDetailsList;
    typedef std::map<SmartPtr<LobbyGame>,SmartPtr<ServerDetailsItem> > ServerDetailsMap;
    ServerListCtrl(); //3c5c10, meaningful game/filter/columns initialization
    virtual ~ServerListCtrl(); //explicit3c5d30 own-vptr, actualownedcleanup
    virtual void InitComponent(ComponentInit&); //3c6360
    virtual bool Virtual44(int); //3c66d0 TimerEvent
    virtual void HandleComponentEvent(ComponentEvent*); //3c80f0
    void LoadFilterCombos(); //3c6740
    void FilterInProgressGames(ServerDetailsList&); //3c6a70
    void DoFilter(ServerDetailsList&); //3c6ad0
    bool FitsFilter(ServerDetailsItem*); //3c6be0
    void UpdateJoinButton(); //3c6d00
    void UpdateSummaryLabel(); //3c6d50
    void AddEntries(const ServerDetailsList&); //3c6ff0
    void AddFilteredEntries(bool,ServerDetailsList&); //3c7090
    void AddFilteredEntries_All(bool); //3c7220
    void AddFilteredEntries_List(bool); //3c73c0
    LobbyGame* GetSelGame(); //3c7580, reference-only accessor bodyquota0
    void HandleGameChanged(GameChangedEvent*); //3c75a0
    void HandleSyncGameList(SyncGameListEvent*); //3c7870
    void HandleComboTextChanged(Component*); //3c7b30
    void HandleComboSelChanged(ComboSelChangedEvent*); //3c7dd0
    bool HandleRefreshButton(); //3c80c0
    void HandleListItemClicked(ListItemClickedEvent*); //3c7a50
    void SetNumColumns(int); //3c8340 fieldstore+vectorresize sequence25, zeroquota
    static ColumnInfo* GetColumnInfo(const GUIString&); //3c5ff0 actualnestedrecord, oldglobalprefixaliasRootgate
    static void InitColumnMap(); //3c6050
    static void AddDefaultFilters(const GUIString&,const GUIString&,bool); //3c6340 sequence wrapper24, zeroquota
    ColumnInfoVector mColumnInfo; //84..93
    int mGameType; //94
    bool mNeedFilter; //98
    int mFilterTimeAccum; //9c
    int mNumColumns; //a0
    SmartPtr<Font> mBoldFont; //a4
    SmartPtr<MultiListArea> mList; //a8
    SmartPtr<Button> mJoinButton; //ac
    SmartPtr<Button> mRefreshButton; //b0
    SmartPtr<Component> mRemoveButton; //b4
    SmartPtr<Label> mSummaryLabel; //b8
    ServerDetailsMap mServerDetailsMap; //bc..c7
    static ColumnMap mColumnMap; //566b60
};
}

#endif
