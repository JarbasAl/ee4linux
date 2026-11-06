// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyPlayerOptionsCtrl.cpp (__FILE__ 0x8d0e70, InitComponent's
// lines 50..70). The game room's player rows: 0x299b17..0x29a43d (EELobbyPlayer.cpp's slot and player classes end at
// 0x299b17). Module flags.
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

// FUNCTION: EXE 0x699b17
EELobbyPlayerOptionsCtrl::EELobbyPlayerOptionsCtrl()
{
    mB88 = false;
    mIsHost = false;
    mMySlot = -1;
    mL84 = 2;
}

// FUNCTION: EXE 0x699ba5
EELobbyPlayerOptionsCtrl::~EELobbyPlayerOptionsCtrl()
{
}

// FUNCTION: EXE 0x699c56
// NEAR: the original strength-reduces the six control ids from i * 6 + 3 (ours from another base)
void EELobbyPlayerOptionsCtrl::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* theConfig = ((ComponentConfigInit&)inInit).mConfig;
        char theName[50];
        for (int i = 0; i < 8; i++) {

            // (the original's line 50 is the first getter below)







            sprintf(theName, "UserNameCombo%d", i + 1);
            SafeGetComponent(theConfig, mUserNameCombo[i], theName, __FILE__, __LINE__);
            mUserNameCombo[i]->mControlId = i * 6 + 1;

            sprintf(theName, "TeamCombo%d", i + 1);
            SafeGetComponent(theConfig, mTeamCombo[i], theName, __FILE__, __LINE__);
            mTeamCombo[i]->mControlId = i * 6 + 2;

            sprintf(theName, "ColorButton%d", i + 1);
            SafeGetComponent(theConfig, mColorButton[i], theName, __FILE__, __LINE__);
            mColorButton[i]->mControlId = i * 6 + 3;

            sprintf(theName, "CitizenCombo%d", i + 1);
            SafeGetComponent(theConfig, mCitizenCombo[i], theName, __FILE__, __LINE__);
            mCitizenCombo[i]->mControlId = i * 6 + 4;

            sprintf(theName, "ObserveCheck%d", i + 1);
            SafeGetComponent(theConfig, mObserveCheck[i], theName, __FILE__, __LINE__);
            mObserveCheck[i]->mControlId = i * 6 + 5;

            sprintf(theName, "FileTransferLabel%d", i + 1);
            SafeGetComponent(theConfig, mFileTransferLabel[i], theName, __FILE__, __LINE__);
            mFileTransferLabel[i]->mControlId = i * 6 + 6;
        }
        Fn2F5360(1);
    }
}

// (0x699dcd: ICF-shared with EELobbyGameOptionsCtrl::GetGame, the name names.txt binds there)
EELobbyGame* EELobbyPlayerOptionsCtrl::GetGame()
{
    return (EELobbyGame*)GetMyLobbyGame();
}

// FUNCTION: EXE 0x699dd2
void EELobbyPlayerOptionsCtrl::UpdatePlayer(int inSlot, EELobbyGame* inGame)
{
    if (inGame == NULL || inSlot < 0 || inSlot >= 8)
        return;
    EELobbyPlayerSlot* theSlot = inGame->GetPlayerSlot(inSlot);
    if (theSlot == NULL)
        return;
    SmartPtr<ComboBox>& theTeam = mTeamCombo[inSlot];
    theTeam->SetSelItem(theSlot->mOptions[1]);
    SmartPtr<ComboBox>& theCitizen = mCitizenCombo[inSlot];
    theCitizen->SetSelItem(theSlot->mOptions[3] - 1);
    bool theTaken = false;
    bool theShowTeam = false;
    bool theNotRandomMap = inGame->GetOption(0) != 0;
    bool theIsMe = false;
    if (inSlot == mMySlot && !GetMyLobbyPlayer()->mBA)
        theIsMe = true;
    if (theSlot->mOptions[0] == 1)
        theShowTeam = true;
    SmartPtr<ComboBox>* theName;
    if (theSlot->mOptions[0] != 3 && theSlot->mOptions[0] != 4) {
        theName = &mUserNameCombo[inSlot];
        (*theName)->SetSelItem(theSlot->mOptions[0]);
        (*theName)->mEdit->mTextColor = 0x8000000e;
        mObserveCheck[inSlot]->SetVisible(false);
        mColorButton[inSlot]->SetVisible(false);
        mFileTransferLabel[inSlot]->SetVisible(false);
    } else {
        theShowTeam = theSlot->mOptions[4] == 0;
        theName = &mUserNameCombo[inSlot];
        theTaken = true;
        (*theName)->SetText(theSlot->GetName());
        if (theSlot->mOptions[0] == 4)
            (*theName)->mEdit->mTextColor = 0xff0000;
        else
            (*theName)->mEdit->mTextColor = 0x8000000e;
        mObserveCheck[inSlot]->SetVisible(true);
        mObserveCheck[inSlot]->SetEnabled(theIsMe && !theNotRandomMap);
        mObserveCheck[inSlot]->SetCheck(theSlot->mOptions[4] != 0);
        mColorButton[inSlot]->SetVisible(theSlot->mOptions[4] == 0);
        mColorButton[inSlot]->SetEnabled(theIsMe && !theNotRandomMap);
        mColorButton[inSlot]->SetColor(theSlot->mOptions[2]);
        if (theSlot->mOptions[5] >= 100)
            mFileTransferLabel[inSlot]->SetVisible(false);
        else {
            mFileTransferLabel[inSlot]->SetVisible(true);
            char theBuffer[50];
            sprintf(theBuffer, "%d", theSlot->mOptions[5]);
            mFileTransferLabel[inSlot]->SetText(
                LobbyFormatString(gEELobbyPlayerOptionsCtrl_TransferPercent, theBuffer));
        }
    }
    (*theName)->SetEnabled(mIsHost && inSlot != mMySlot && !theNotRandomMap);
    theCitizen->SetEnabled(mIsHost && !theNotRandomMap);
    theTeam->SetEnabled((theIsMe || (mIsHost && !theTaken)) && !theNotRandomMap);
    theCitizen->SetVisible(theShowTeam);
    theTeam->SetVisible(theShowTeam);
}

// FUNCTION: EXE 0x69a0f5
void EELobbyPlayerOptionsCtrl::UpdatePlayer(int inSlot)
{
    UpdatePlayer(inSlot, GetGame());
}

// FUNCTION: EXE 0x69a10d
void EELobbyPlayerOptionsCtrl::UpdateAll()
{
    EELobbyGame* theGame = GetGame();
    if (theGame != NULL)
        for (int i = 0; i < 8; i++)
            UpdatePlayer(i, theGame);
}

// FUNCTION: EXE 0x69a132
void EELobbyPlayerOptionsCtrl::Fn29A132()
{
    EELobbyGame* theGame = GetGame();
    if (theGame == NULL || !theGame->Fn3A6D40(mL84))
        return;
    mB88 = true;
    mIsHost = LobbyFn3613C0();
    EELobbyPlayer* thePlayer = (EELobbyPlayer*)GetMyLobbyPlayer();
    if (thePlayer != NULL)
        mMySlot = thePlayer->GetSlot();
    UpdateAll();
}

// FUNCTION: EXE 0x69a183
void EELobbyPlayerOptionsCtrl::Fn29A183()
{
    if (mB88) {
        mMySlot = -1;
        mB88 = false;
        mIsHost = false;
    }
}

// FUNCTION: EXE 0x69a19f
void EELobbyPlayerOptionsCtrl::PlayerOptionChanged(int inSlot, int inWhat)
{
    EELobbyGame* theGame = GetGame();
    if (theGame == NULL || inSlot < 0 || inSlot >= 8 || inWhat < 0 || inWhat >= 6)
        return;
    unsigned char theValue = 0;
    switch (inWhat) {
    case 4:
        theValue = mObserveCheck[inSlot]->mChecked;
        break;
    case 3:
        theValue = mCitizenCombo[inSlot]->GetSelItem() + 1;
        break;
    case 2:
        theValue = mColorButton[inSlot]->mColor;
        break;
    case 1:
        theValue = mTeamCombo[inSlot]->GetSelItem();
        break;
    case 0:
        theValue = mUserNameCombo[inSlot]->GetSelItem();
        break;
    }
    theGame->ChangePlayerOption(inSlot, inWhat, theValue);
}

// FUNCTION: EXE 0x69a240
void EELobbyPlayerOptionsCtrl::Fn29A240(ComponentEvent* inEvent)
{
    PlayerChangedEvent* theEvent = static_cast<PlayerChangedEvent*>(inEvent);
    if (theEvent->Fn359470() && theEvent->mClient.mPtr->mPlayer.mPtr != NULL
        && ((EELobbyPlayer*)theEvent->mClient.mPtr->mPlayer.mPtr)->GetSlot() == mMySlot)
        UpdatePlayer(mMySlot);
}

// FUNCTION: EXE 0x69a27c
void EELobbyPlayerOptionsCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch (inEvent->mType) {
    case 100032:
        Fn29A183();
        break;
    case 100029:
        Fn29A132();
        break;
    case 100038:
        Fn29A240(inEvent);
    case 100063:
        if (mB88)
            UpdatePlayer(static_cast<EELobbySyncPlayerEvent*>(inEvent)->mParam);
        break;
    case 100065:
        if (mB88)
            UpdateAll();
        break;
    case 15:
    case 27:
    case 100064:
        if (mB88) {
            int theId = inEvent->GetControlId() - 1;
            PlayerOptionChanged(theId / 6, theId % 6);
        }
        break;
    case 100054:
        mL84 |= static_cast<SetRoomSpecFlagsEvent*>(inEvent)->mFlags;
        break;
    }
}

// The unit's other compiler-generated functions:
// SYNTHETIC: EXE 0x699b89
// (??_G EELobbyPlayerOptionsCtrl)
// TEMPLATE: EXE 0x69a33d
// (SafeGetComponent<ComboBox>)
// TEMPLATE: EXE 0x69a3bd
// (SafeGetComponent<EELobbyColorButton>)

//: the ICF-shared copy LINK kept in this unit's range
// SYNTHETIC: EXE 0x699dcd
