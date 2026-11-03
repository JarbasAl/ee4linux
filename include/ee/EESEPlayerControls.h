// Original nonpolymorphic editor helper records. Nominal class/member names
// provisional; actual allocations178a29=0x5c,176be4=0xf4 and field readers prove layout.
#ifndef EE_EESEPLAYERCONTROLS_H
#define EE_EESEPLAYERCONTROLS_H
class UITextLabel;
class UIEditBox;
class UIComboBox;
class UICheckBox;
class UISlider;
class EEUIBColor;
class EESEUIBDiplomacy;
class UIForm;
class GEFont;

class EESEPlayerRow {
public:
    EESEPlayerRow(unsigned long& ioID, float inY, GEFont* inFont, unsigned long& ioIndex,
                   unsigned long inPlayer, UIForm* inForm, bool inGlobal); //178a29
    ~EESEPlayerRow();                     //179a2b
    void CopyFrom(EESEPlayerRow* inSource);//179b27
    void Show(unsigned long inPage, unsigned long inPlayer); //179d63
    void Hide();                          //179ebe
    void Save(unsigned long inPlayer);     //179f94
    void Load(unsigned long inPlayer);     //179fee
    void ApplySettings(unsigned long inPlayer); //17a53e, caller175ee5 readsRowthis from84+i*4
    UITextLabel* mTitle;                   //00
    UIEditBox* mValues[5];                 //04: actual5-element constructor loop
    UIComboBox* mStartEpoch;               //18
    UIComboBox* mEndEpoch;                 //1c
    EEUIBColor* mColor;                    //20
    UIEditBox* mName;                      //24
    UIComboBox* mControl;                  //28
    UICheckBox* mCPUOnlyCheckBox;          //2c
    UICheckBox* mSharedLOSCheckBox;        //30
    UICheckBox* mActiveCheckBox;           //34
    UICheckBox* mCheatCheckBox;            //38
    UICheckBox* mAttackCheckBox;           //3c
    UICheckBox* mDefendCheckBox;           //40
    UICheckBox* mBuildRepairCheckBox;      //44
    UICheckBox* mExploreCheckBox;          //48
    UICheckBox* mIgnoreCheckBox;           //4c
    UICheckBox* mGatherCheckBox;           //50
    UISlider* mSlider;                     //54
    UITextLabel* mSliderLabel;             //58
};

class EESEGamePage {
public:
    EESEGamePage(unsigned long& ioIndex, unsigned long& ioID, UIForm* inForm); //176be4
    ~EESEGamePage();                       //177b83
    void SetPlayerCount(unsigned long inCount); //177cc1
    unsigned long PairIndex(unsigned long inPlayer); //177da2, actualret4 even thoughthis unused
    void CreateDiplomacyRow(unsigned long inPlayer,unsigned long& ioIndex,
                              unsigned long& ioID,UIForm* inForm); //177dd3
    void Load();                          //177ea6
    void CreateLabels(unsigned long& ioIndex,unsigned long& ioID,UIForm* inForm); //17802c
    void Save();                          //178342
    UITextLabel* mTitle;                   //00
    UITextLabel* mPlayerTitle;             //04
    UITextLabel* mPlayerNames[15];         //08, constructor/dtor actual15 loops
    UITextLabel* mColumnLabels[15];        //44
    UITextLabel* mRowLabels[15];           //80
    UITextLabel* mUnitLimitLabel;          //bc
    UITextLabel* mUnitLimitCountLabel;     //c0
    UITextLabel* mPerPlayerLabel;          //c4
    UITextLabel* mPerPlayerCountLabel;     //c8
    UITextLabel* mWondersForVictoryLabel;  //cc
    UITextLabel* mWondersForVictoryCountLabel;  //d0
    UISlider* mUnitLimit;                  //d4
    UISlider* mWondersForVictory;          //d8
    UICheckBox* mVictoryCheckBox;          //dc
    UICheckBox* mTeamsLockedCheckBox;      //e0
    UICheckBox* mCheatCodesCheckBox;       //e4
    UICheckBox* mUnitImprovementsCheckBox;  //e8
    UICheckBox* mUseCarbonCheckBox;        //ec
    EESEUIBDiplomacy** mDiplomacy;          //f0: new0x1e0=120 pointers,177dd3/177b83
};
#endif
