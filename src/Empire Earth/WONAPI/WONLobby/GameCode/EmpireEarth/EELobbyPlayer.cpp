// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyPlayer.cpp (name PROVISIONAL: no __FILE__; its classes
// EELobbyPlayerSlot and EELobbyPlayer from RTTI). A lobby game's player slots and the game's lobby player:
// 0x299838..0x299b17 (the string/map COMDATs at 0x29976f..0x299838 close the unit before; EELobbyPlayerOptionsCtrl.cpp
// follows). Module flags (r12-misc-h4, round 12).
#include <windows.h>
#include <string>
#include <list>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"

using namespace WONAPI;

// FUNCTION: EXE 0x699838
EELobbyPlayerSlot::EELobbyPlayerSlot()
{
    mIndex = 0;
    mOptions[0] = 0;
    Reset();
}

// FUNCTION: EXE 0x699880
void EELobbyPlayerSlot::Reset()
{
    mOptions[1] = 0;
    mOptions[2] = 0;
    mOptions[3] = 5;
    mOptions[4] = 0;
    mOptions[5] = 100;
}

// FUNCTION: EXE 0x699894
EELobbyPlayerSlot::~EELobbyPlayerSlot()
{
    ClearPlayer();
}

// FUNCTION: EXE 0x6998c8
const GUIString& EELobbyPlayerSlot::GetName()
{
    if (mClient.mPtr == NULL) {
        if (mOptions[0] == 4)
            return mName;
        return gEmptyGUIString;
    }
    return mClient->mName;
}

// FUNCTION: EXE 0x6998e3
void EELobbyPlayerSlot::SetState(int inState)
{
    mOptions[0] = inState;
    if (inState != 3)
        ClearPlayer();
    if (inState != 4)
        mName = GetEmptyGUIStringData();
}

// FUNCTION: EXE 0x699910
unsigned char EELobbyPlayerSlot::GetOption(int inOption)
{
    if (inOption >= 0 && inOption < 6)
        return mOptions[inOption];
    return 0;
}

// FUNCTION: EXE 0x699928
void EELobbyPlayerSlot::SetOption(int inOption, unsigned char inValue)
{
    if (inOption == 0) {
        if (inValue < 5)
            SetState(inValue);
    } else
        mOptions[inOption] = inValue;
}

// FUNCTION: EXE 0x69994f
// NEAR: ICF alias: both assignments call SmartPtr<T>::operator=(T*) 0x299ae3 (bound to the GUIStringData instance)
void EELobbyPlayerSlot::ClearPlayer()
{
    if (mPlayer.mPtr != NULL) {
        mPlayer->mPlayerSlot = NULL;
        mPlayer = NULL;
    }
    mClient = NULL;
}

// FUNCTION: EXE 0x699973
void EELobbyPlayerSlot::Fn299973()
{
    SetState(0);
}

// FUNCTION: EXE 0x69997b
void EELobbyPlayerSlot::Fn29997B()
{
    SetState(2);
}

// FUNCTION: EXE 0x699983
void EELobbyPlayerSlot::Fn299983()
{
    SetState(1);
}

// FUNCTION: EXE 0x69998b
void EELobbyPlayerSlot::SetName(const GUIString& inName)
{
    SetState(4);
    mName = inName;
}

// FUNCTION: EXE 0x6999a7
// NEAR: ICF alias, as ClearPlayer
void EELobbyPlayerSlot::SetPlayer(LobbyClient* inClient, EELobbyPlayer* inPlayer)
{
    ClearPlayer();
    if (inClient != NULL && inPlayer != NULL) {
        mClient = inClient;
        mPlayer = inPlayer;
        mPlayer->mPlayerSlot = this;
        mPlayer->mSlot = mIndex;
        SetState(3);
    }
}

// FUNCTION: EXE 0x6999f3
void EELobbyPlayerSlot::ReadData(ReadBuffer& inBuffer)
{
    for (int i = 0; i < 6; i++)
        mOptions[i] = inBuffer.ReadByte();
    if (mOptions[0] != 3)
        ClearPlayer();
}

// FUNCTION: EXE 0x699a1e
void EELobbyPlayerSlot::WriteData(WriteBuffer& outBuffer)
{
    for (int i = 0; i < 6; i++)
        outBuffer.AppendByte(mOptions[i]);
}

// FUNCTION: EXE 0x699a3f
EELobbyPlayer::EELobbyPlayer()
{
    mSlot = 0;
    mPlayerSlot = NULL;
}

// FUNCTION: EXE 0x699a75
EELobbyPlayer::~EELobbyPlayer()
{
}

// FUNCTION: EXE 0x699a80
bool EELobbyPlayer::ReadData(ReadBuffer& inBuffer)
{
    mSlot = inBuffer.ReadByte();
    return true;
}

// FUNCTION: EXE 0x699a95
void EELobbyPlayer::WriteData(WriteBuffer& outBuffer)
{
    outBuffer.AppendByte(mSlot);
}

// FUNCTION: EXE 0x699aa7
bool EELobbyPlayer::IsTeammate(LobbyPlayer* inOther)
{
    unsigned char theTeam = ((EELobbyPlayer*)inOther)->GetTeam();
    unsigned char myTeam = GetTeam();
    return myTeam != 0 && myTeam == theTeam;
}

// FUNCTION: EXE 0x699ad1
unsigned char EELobbyPlayer::GetTeam()
{
    if (mPlayerSlot != NULL)
        return mPlayerSlot->mOptions[1];
    return 0;
}

// FUNCTION: EXE 0x699adf
unsigned char EELobbyPlayer::GetSlot()
{
    return mSlot;
}

// The unit's other compiler-generated functions:
// SYNTHETIC: EXE 0x699864
// (??_G EELobbyPlayerSlot)
// SYNTHETIC: EXE 0x699a59
// (??_G EELobbyPlayer)
// TEMPLATE: EXE 0x699ae3
// (SmartPtr<T>::operator=(T*), ICF-shared by every instantiation; NEAR: the original returns the raw pointer and
// stores it through the ternary shape: proposal P2 in build/scratch_r12-misc/proposals_h4.md)
