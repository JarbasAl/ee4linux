// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyGameOptionsCtrl.cpp (__FILE__ 0x8d09ac, InitComponent's
// lines 74..89). The game room's game options: 16 named combo/check boxes showing the lobby game's options:
// 0x2942c8..0x294d62 (the option table's chain first; EELobbyInit.cpp follows). Module flags (r12-misc-h4, round 12).
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"

using namespace WONAPI;

// one option control: its resource name and kind (0: combo box, 1: check box)
struct EELobbyOptionInfo {
    EELobbyOptionInfo() {}
    EELobbyOptionInfo(const char* inName, int inType) : mName(inName), mType(inType) {}

    const char* mName;
    int mType;
};

static EELobbyOptionInfo sOptionTable[16];
static bool sOptionTableInitialized;

static void EELobbyInitOptionTable();

// FUNCTION: EXE 0x6942ce
EELobbyGameOptionsCtrl::EELobbyGameOptionsCtrl()
{
    EELobbyInitOptionTable();
    mB94 = false;
    mL90 = 2;
}

// FUNCTION: EXE 0x694336
EELobbyGameOptionsCtrl::~EELobbyGameOptionsCtrl()
{
}

// FUNCTION: EXE 0x694394
static void EELobbyInitOptionTable()
{
    if (sOptionTableInitialized)
        return;
    sOptionTableInitialized = true;
    sOptionTable[0] = EELobbyOptionInfo("GameTypeCombo", 0);
    sOptionTable[1] = EELobbyOptionInfo("MapSizeCombo", 0);
    sOptionTable[2] = EELobbyOptionInfo("MapTypeCombo", 0);
    sOptionTable[3] = EELobbyOptionInfo("ResourcesCombo", 0);
    sOptionTable[4] = EELobbyOptionInfo("StartingEpochCombo", 0);
    sOptionTable[5] = EELobbyOptionInfo("EndingEpochCombo", 0);
    sOptionTable[6] = EELobbyOptionInfo("GameVariantCombo", 0);
    sOptionTable[7] = EELobbyOptionInfo("DifficultyLevelCombo", 0);
    sOptionTable[8] = EELobbyOptionInfo("GameSpeedCombo", 0);
    sOptionTable[9] = EELobbyOptionInfo("GameUnitLimitCombo", 0);
    sOptionTable[10] = EELobbyOptionInfo("WondersCombo", 0);
    sOptionTable[11] = EELobbyOptionInfo("RevealMapCheck", 1);
    sOptionTable[12] = EELobbyOptionInfo("UseCustomCivsCheck", 1);
    sOptionTable[13] = EELobbyOptionInfo("LockTeamsCheck", 1);
    sOptionTable[14] = EELobbyOptionInfo("LockSpeedCheck", 1);
    sOptionTable[15] = EELobbyOptionInfo("VictoryCheck", 1);
}

// FUNCTION: EXE 0x6944ae
void EELobbyGameOptionsCtrl::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* theConfig = ((ComponentConfigInit&)inInit).mConfig;
        SafeGetComponent(theConfig, mCampaignInfoLabel, "CampaignInfoLabel", __FILE__, __LINE__);
        SafeGetComponent(theConfig, mCampaignInfoBox1, "CampaignInfoBox1", __FILE__, __LINE__);
        SafeGetComponent(theConfig, mCampaignInfoBox2, "CampaignInfoBox2", __FILE__, __LINE__);

        int i;
        for (i = 0; i < 16; i++) {
            SafeGetComponent(theConfig, mOptions[i], sOptionTable[i].mName, __FILE__, __LINE__);
            bool theTypeOK = true;
            switch (sOptionTable[i].mType) {
            case 1: theTypeOK = dynamic_cast<Button*>(mOptions[i].get()) != NULL; break;
            case 0: theTypeOK = dynamic_cast<ComboBox*>(mOptions[i].get()) != NULL; break;
            }
            // (the original spends two more lines here: __LINE__ 89 below)

            if (!theTypeOK)
                ComponentConfigMissing(std::string("Invalid type for ") + sOptionTable[i].mName, __FILE__, __LINE__);
            mOptions[i]->mControlId = i + 1;
        }

        ComboBox* theCombo = GetComboBox(9);
        if (theCombo != NULL) {
            for (int theLimit = 50; theLimit <= 1200; theLimit += 50) {
                char theBuffer[50];
                sprintf(theBuffer, "%d", theLimit);
                theCombo->AddString(theBuffer);
            }
        }
        EELobbyFn29876A(GetComboBox(2));
        Fn2F5360(1);
        GetCheckBox(15)->SetVisible(false);
    }
}

// FUNCTION: EXE 0x694680
Component* EELobbyGameOptionsCtrl::GetControl(int inOption)
{
    if (inOption >= 0 && inOption < 16)
        return mOptions[inOption].mPtr;
    return NULL;
}

// FUNCTION: EXE 0x69469b
ComboBox* EELobbyGameOptionsCtrl::GetComboBox(int inOption)
{
    if (inOption >= 0 && inOption < 16 && sOptionTable[inOption].mType == 0)
        return (ComboBox*)mOptions[inOption].mPtr;
    return NULL;
}

// FUNCTION: EXE 0x6946c0
Button* EELobbyGameOptionsCtrl::GetCheckBox(int inOption)
{
    if (inOption >= 0 && inOption < 16 && sOptionTable[inOption].mType == 1)
        return (Button*)mOptions[inOption].mPtr;
    return NULL;
}

// FUNCTION: EXE 0x6946e5
// the range test lets 16 through (`> 16`, as the original)
void EELobbyGameOptionsCtrl::UpdateOption(int inOption, EELobbyGame* inGame)
{
    if (inGame == NULL || inOption < 0 || inOption > 16)
        return;
    if (inOption != 9) {
        switch (sOptionTable[inOption].mType) {
        case 1: {
            Button* theCheck = GetCheckBox(inOption);
            if (theCheck != NULL)
                theCheck->SetCheck(inGame->GetOption(inOption) != 0);
            break;
        }
        case 0: {
            ComboBox* theCombo = GetComboBox(inOption);
            if (theCombo != NULL)
                theCombo->SetSelItem(inGame->GetOption(inOption));
            break;
        }
        }
    } else {
        ComboBox* theCombo = GetComboBox(9);
        if (theCombo != NULL)
            theCombo->SetSelItem((unsigned long)inGame->GetOption(9) / 50 - 1);
    }
}

// FUNCTION: EXE 0x694788
void EELobbyGameOptionsCtrl::UpdateOption(int inOption)
{
    UpdateOption(inOption, GetGame());
}

// FUNCTION: EXE 0x6947a0
void EELobbyGameOptionsCtrl::UpdateAll()
{
    EELobbyGame* theGame = GetGame();
    if (theGame == NULL)
        return;
    int i;
    for (i = 0; i < 16; i++)
        UpdateOption(i, theGame);
    bool theNotRandomMap = theGame->GetOption(0) != 0;
    bool theFlag = theGame->mIsHost;
    for (int j = 0; j < 16; j++) {
        Component* theControl = GetControl(j);
        bool theEnabled;
        if (j == 0)
            theEnabled = theFlag;
        else if (theFlag && !theNotRandomMap)
            theEnabled = true;
        else
            theEnabled = false;
        if (theControl != NULL)
            theControl->SetEnabled(theEnabled);
    }
    if (theGame->HasGameFile()) {
        mCampaignInfoBox1->SetVisible(false);
        mCampaignInfoBox2->SetVisible(true);
        mCampaignInfoLabel->SetText(theGame->GetGameFileName());
    } else {
        mCampaignInfoBox1->SetVisible(true);
        mCampaignInfoBox2->SetVisible(false);
    }
}

// FUNCTION: EXE 0x69488a
void EELobbyGameOptionsCtrl::Fn29488A()
{
    EELobbyGame* theGame = GetGame();
    if (theGame == NULL || !theGame->Fn3A6D40(mL90))
        return;
    mB94 = true;
    UpdateAll();
    bool theFlag = theGame->mIsHost;
    for (int i = 0; i < 16; i++)
        mOptions[i]->SetEnabled(theFlag);
}

// FUNCTION: EXE 0x6948e2
void EELobbyGameOptionsCtrl::Fn2948E2()
{
    if (mB94)
        mB94 = false;
}

// FUNCTION: EXE 0x6948f5
// NEAR: the original calls the GUIString copy's SmartPtr(T*) out of line (0x293cd4: inline budget); ours expands it
void EELobbyGameOptionsCtrl::Fn2948F5()
{
    ComboBox* theCombo = GetComboBox(0);
    if (theCombo == NULL || theCombo->mListArea->mSelItem == NULL)
        return;
    EELobbyGame* theGame = GetGame();
    if (theGame == NULL)
        return;
    long theOldType = theGame->GetOption(0);
    if (theCombo->GetSelItem() == 1) {
        GUIString theDirectory = EELobbyFn2988E9();
        SmartPtr<EELobbyFileDialog> theDialog = EELobbyGetFileDialog();
        int theResult = -1;
        if (theDialog.mPtr != NULL) {
            theDialog->ListFiles(theDirectory, ".ees");
            theResult = LobbyDoDialog(theDialog.mPtr);
        }
        theGame = GetGame();
        if (theGame != NULL) {
            if (theResult == 100001) {
                theGame->SetOption(0, 1);
                theGame->SendGameState();
            } else
                theCombo->SetSelItem(theOldType);
        }
    } else {
        theCombo->SetSelItem(0);
        theGame->SetOption(0, 0);
        for (int i = 0; i < 8; i++) {
            EELobbyPlayerSlot* theSlot = theGame->GetPlayerSlot(i);
            if (theSlot != NULL && theSlot->mOptions[0] == 4)
                theSlot->Fn299973();
        }
        theGame->SendGameState();
    }
}

// FUNCTION: EXE 0x694a77
// NEAR: tail merge: the original merges the combo/limit calls into the check box's call; ours the other way round
void EELobbyGameOptionsCtrl::OptionChanged(int inOption)
{
    if (inOption < 0 || inOption >= 16)
        return;
    if (!LobbyFn3613C0()) {
        UpdateOption(inOption);
    } else {
    EELobbyGame* theGame = GetGame();
    if (theGame == NULL)
        return;
    if (inOption != 0) {
        if (inOption != 9) {
            switch (sOptionTable[inOption].mType) {
            case 1: {
                Button* theCheck = GetCheckBox(inOption);
                if (theCheck != NULL)
                    theGame->ChangeOption(inOption, theCheck->mChecked, true);
                break;
            }
            case 0: {
                ComboBox* theCombo = GetComboBox(inOption);
                if (theCombo != NULL && theCombo->mListArea->mSelItem != NULL)
                    theGame->ChangeOption(inOption, theCombo->GetSelItem(), true);
                break;
            }
            }
        } else {
            ComboBox* theCombo = GetComboBox(9);
            if (theCombo != NULL && theCombo->mListArea->mSelItem != NULL)
                theGame->ChangeOption(9, (theCombo->GetSelItem() + 1) * 50, true);
        }
    } else
        Fn2948F5();
    }
}

// FUNCTION: EXE 0x694b57
void EELobbyGameOptionsCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch (inEvent->mType) {
    case 100065:
        if (mB94)
            UpdateAll();
        break;
    case 100062:
        if (mB94)
            UpdateOption(static_cast<EELobbySyncOptionEvent*>(inEvent)->mParam);
        break;
    case 100054:
        mL90 |= static_cast<SetRoomSpecFlagsEvent*>(inEvent)->mFlags;
        break;
    case 100032:
        Fn2948E2();
        break;
    case 100029:
        Fn29488A();
        break;
    case 27:
    case 15:
        if (mB94)
            OptionChanged(inEvent->GetControlId() - 1);
        break;
    }
}

// ICF-folded into 0x299dcd (its twin in EELobbyPlayerOptionsCtrl.cpp)
EELobbyGame* EELobbyGameOptionsCtrl::GetGame()
{
    return (EELobbyGame*)GetMyLobbyGame();
}

// The unit's other compiler-generated functions:
// SYNTHETIC: EXE 0x6942c8
// SYNTHETIC: EXE 0x6942cd
// (the option table's `$E` chain: the element ctor loop, empty)
// SYNTHETIC: EXE 0x69431a
// (??_G EELobbyGameOptionsCtrl)
// TEMPLATE: EXE 0x694be5
// (SafeGetComponent<Label>)
// TEMPLATE: EXE 0x694c65
// (SafeGetComponent<Container>)
// TEMPLATE: EXE 0x694ce5
// (SafeGetComponent<Component>)
