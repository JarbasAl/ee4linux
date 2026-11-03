// C:\EEX\Empire Earth\Scenario Editor\EESEPlayerScreen.h (RECONSTRUCTION; file name inferred). The editor's player
// tab (RTTI EESEPlayerScreen > EEUIFSEMainGroup; vtable 0x4430dc; ctor 0x1733bf):
// the editor's group 5. Only what matched code uses; provisional names. Requires ee/EEUIFSEMainGroup.h.
#ifndef EE_EESEPLAYERSCREEN_H
#define EE_EESEPLAYERSCREEN_H

class EEPlayer;
class EESETab;
class UITextLabel;
class EESECBMaxPlayer;
class EESEPlayerRow;
class EESEPSSetPlayers;
class UIComboBox;
class EESERBUnitTypes;
class EESEUITechListBox;
class EESEBSelect;
class UICheckBox;
class EESEGamePage;
class UIForm;
class GEFont;

// Original173313 initializes each two-byte pair;175d58 and175d7e prove both flags.
struct EESEPlayerChange {
    EESEPlayerChange() : mChanged(false), mRefresh(false) {}
    bool mChanged;
    bool mRefresh;
};

class EESEPlayerScreen : public EEUIFSEMainGroup {
public:
    EESEPlayerScreen(unsigned long& ioIndex, UIForm* inForm, GEFont* inFont, UIComboBox* inPlayerSelection); //173313
    virtual ~EESEPlayerScreen(); //1733bf
    virtual void Leave();     //1737ac
    virtual void Load();      //175c0b
    virtual void Enter();     //175c48
    virtual void Store();     //175d7e
    // Actual vtable+24 points157269, independently disassembled ret4.
    virtual void MouseMove(const GE2DPoint& inLocation) {}
    void CreatePlayerControls(unsigned long& ioIndex, UIForm* inForm, GEFont* inFont);
    void CreateTechControls(unsigned long& ioIndex, UIForm* inForm, GEFont* inFont);
    void Refresh();
    void ApplyPlayerSettings();
    void BuildTechTypeLists();
    // 0x175a1f: the player combo box's (+0x7c) list selection + 2 (EESETSCondition::Activate)
    unsigned long GetPlayerCount();
    // 0x173653: rebuilds the max-player combo box (+0xd8) for inCount players and posts an
    // EESEAChangeWorldPlayerCount(inCount + 1) (EESECBMaxPlayer::ChangeSelection; provisional name)
    void SetPlayerCount(unsigned long inCount);
    void SelectPage(unsigned long inPage);  // 0x1761c8 (EESETab::Check: shows page 0..4) (provisional name)
    void FillTechLists();                   // 0x1760ad (EESERBUnitTypes::Check; provisional name)
    // 0x175eb2: for players 1..gPlayerCount-1, 0x179b27 on the per-player control (+0x84[i]) with +0x80
    // (EESEPSSetPlayers::ButtonUp; provisional name)
    void CopyFirstPlayerToAll();
    // 0x199ae9 (inline: the COMDAT closes EESEUITechListBox.cpp, its first user): the player of the player combo box
    // (+0xd8; its +0xd0 selection + 1: gWorld.mPlayers[sel + 1]); not defined (the layout up to +0xd8 is not
    // declared) (provisional name)
    EEPlayer* GetCurrentPlayer();
    // 0x175d58: m114[inPlayer].a = true; m114[inPlayer].b |= inFlag (2-byte pairs; EESEUITechListBox::Fill)
    // (provisional name)
    void MarkPlayerChanged(unsigned long inPlayer, bool inFlag);
    void RebuildTechLists();                // 0x1766ed (`ret`; EESEEDisplayEpoch::ProcessEvent; provisional name)
    EESETab* mTabs[5];                    //04 actual five tab constructors
    UITextLabel* mTitle;                  //18
    UITextLabel* mColumnLabels[5];        //1c actual five-element constructor loop
    UITextLabel* mStartEpochLabel;         //30
    UITextLabel* mEndEpochLabel;           //34
    UITextLabel* mCivilizationLabel;       //38
    UITextLabel* mPlayerLabel;             //3c
    UITextLabel* mCPUOnlyTopLabel;         //40
    UITextLabel* mCPUOnlyBottomLabel;      //44
    UITextLabel* mSharedLOSTopLabel;       //48
    UITextLabel* mSharedLOSBottomLabel;    //4c
    UITextLabel* mAllPlayersLabel;         //50
    UITextLabel* mMaxPlayersLabel;         //54
    UITextLabel* mActiveLabel;             //58
    UITextLabel* mCheatLabel;              //5c
    UITextLabel* mAttackLabel;             //60
    UITextLabel* mDefendLabel;             //64
    UITextLabel* mBuildRepairLabel;        //68
    UITextLabel* mExploreLabel;            //6c
    UITextLabel* mIgnoreLabel;             //70
    UITextLabel* mGatherLabel;             //74
    UITextLabel* mCitizenPercentageLabel;  //78
    EESECBMaxPlayer* mMaxPlayer;           //7c
    EESEPlayerRow* mPlayers[17];           //80 actual17 owned helper pointers
    EESEPSSetPlayers* mSetPlayers;         //c4
    UITextLabel* mTechPlayerLabel;         //c8
    UITextLabel* mUnitTypesLabel;          //cc
    UITextLabel* mEnableTechnologiesLabel;  //d0
    UITextLabel* mDisableTechnologiesLabel;  //d4
    UIComboBox* mPlayerSelection;          //d8, borrowed constructor fourth arg
    EESERBUnitTypes* mTypeButtons[4];       //dc: actual 17509c constructor loop, four elements
    EESEUITechListBox* mTechList;          //ec
    EESEUITechListBox* mOtherTechList;     //f0
    EESEBSelect* mAdd;                     //f4
    EESEBSelect* mRemove;                  //f8
    EESEBSelect* mAddAll;                  //fc
    EESEBSelect* mRemoveAll;               //100
    UICheckBox* mDisplayEpoch;             //104
    EESEGamePage* mGame;                   //108
    // Unaccessed interior storage, anchored by actual108/110/114 readers.
    // Root-approved explicit PLACEHOLDER; no nominal type/owner/semantics claimed.
    unsigned char mUndecoded10C[4];
    unsigned long mPage;                  //110
    EESEPlayerChange mChanges[17];        //114..135
    bool mDirty;                          //136

};

#endif
