// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyGame.cpp (name PROVISIONAL: no __FILE__; the class
// EELobbyGame's ctor opens the range, and it sorts between EELobbyFileTransfer and EELobbyGameOptionsCtrl). The lobby
// game: the sixteen game options and their wire types, the eight player slots, the saved game's file transfer, the
// players' sync events: 0x29222b..0x2942c8 (its template COMDATs close it). Module flags.
#include <windows.h>
#include <string.h>
#include <string>
#include <list>
#include <vector>
#include <map>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"
#include "ee/wonapi/WONLobbyModel.h"

using namespace WONAPI;

extern GUIString gEELobbyGame_StartGameError;          // 0x9658b0 (EELobbyResource.cpp)
extern GUIString gEELobbyGame_EveryoneOnSameTeam;      // 0x9659ac (EELobbyResource.cpp)
extern GUIString gEELobbyGameStatus_NotInSavedGame;    // 0x9658cc (EELobbyResource.cpp)
extern GUIString gEELobbyGameStatus_InvalidVersion;    // 0x96591c (EELobbyResource.cpp)
extern GUIString gEELobbyGameStatus_CorruptBinaries;   // 0x9659b4 (EELobbyResource.cpp)

// each option's wire type (0: bool, 1: byte, 2: short) and limit (SetOption keeps a value below it), set once by
// EELobbyInitOptionTypes
static int sOptionTypes[16];                // 0x965774
static unsigned long sOptionLimits[16];     // 0x965734
static bool sOptionTypesInitialized;        // 0x9657b4

static void EELobbyInitOptionTypes();

// FUNCTION: EXE 0x69222b
EELobbyGame::EELobbyGame()
{
    EELobbyInitOptionTypes();
    int i;
    for (i = 0; i < 16; i++)
        mOptions[i] = 0;
    for (i = 0; i < 8; i++)
        mSlots[i].mIndex = i;
    mHasFile = false;
}

// FUNCTION: EXE 0x6922b2
EELobbyGame::~EELobbyGame()
{
}

// FUNCTION: EXE 0x6922f2
int EELobbyGame::GetNumFiles()
{
    return mFiles.size();
}

// FUNCTION: EXE 0x692305
const GUIString& EELobbyGame::GetFile(int inIndex)
{
    if (inIndex >= 0 && inIndex < mFiles.size())
        return mFiles[inIndex];
    return gEmptyGUIString;
}

// FUNCTION: EXE 0x69232f
// NEAR: the original calls GUIString::append(const GUIString&) out of line (COMDAT 0x2923f3); ours expands it
// (inline budget; an operator+= wrapper does not change it)
void EELobbyGame::SetGameFile(const GUIString& inName, unsigned long inSize, unsigned long inChecksum, bool inFlag)
{
    if (mFileTransfer.mPtr == NULL)
        mFileTransfer = new EELobbyFileTransfer;
    mFileTransfer->SetFileInfo(inName, inSize, inChecksum);
    GUIString thePath = EELobbyFn2988E9();
    thePath.append(GetGameFileName());
    mFileTransfer->SetPath(thePath);
    if (mIsHost && inFlag)
        mFileTransfer->Serve(EELobbyGetFileTransferPort());
}

// FUNCTION: EXE 0x692419
void GUIStringData::append(const GUIStringData* inText, unsigned short inStart, unsigned short inLength)
{
    int theLength = mLength;
    int theNewLength = theLength + inLength;
    Reserve(theNewLength, true);
    mLength = theNewLength;
    mText[mLength] = 0;
    memcpy(mText + theLength, inText->mText + inStart, inLength * sizeof(wchar_t));
}

// FUNCTION: EXE 0x69246d
const GUIString& EELobbyGame::GetGameFileName()
{
    if (mFileTransfer.mPtr != NULL)
        return mFileTransfer->mName;
    return gEmptyGUIString;
}

// FUNCTION: EXE 0x69247e
void EELobbyGame::WriteGameFile(WriteBuffer& outBuffer)
{
    outBuffer.AppendWString(GetGameFileName(), 2);
    outBuffer.AppendLong(mFileTransfer->mSize);
    outBuffer.AppendLong(mFileTransfer->mChecksum);
    outBuffer.AppendShort(EELobbyGetFileTransferPort());
}

// FUNCTION: EXE 0x6924e0
void EELobbyGame::ReadGameFile(ReadBuffer& inBuffer)
{
    std::wstring theName;
    inBuffer.ReadWString(theName, 2);
    unsigned long theSize = inBuffer.ReadLong();
    unsigned long theChecksum = inBuffer.ReadLong();
    unsigned short thePort = inBuffer.ReadShort();
    SetGameFile(theName, theSize, theChecksum, false);
    IPAddr theAddress(mAddress);
    theAddress.SetPort(thePort);
    mFileTransfer->SetHost(theAddress);
}

// FUNCTION: EXE 0x69258f
GUIString::GUIString(const std::wstring& inText) : mData(new GUIStringData(inText))
{
}

// FUNCTION: EXE 0x6925cb
void EELobbyGame::StopTransfer()
{
    if (mFileTransfer.mPtr != NULL)
        mFileTransfer->Stop();
}

// FUNCTION: EXE 0x6925d8
void EELobbyGame::UpdateTransferProgress(int inDone, int inTotal)
{
    if (inDone == inTotal) {
        mHasFile = true;
        SetTransferPercent(100);
        LobbyFn3613E0();
    } else
        SetTransferPercent((unsigned char)(int)((double)inDone / inTotal * 100.0));
}

// FUNCTION: EXE 0x692616
void EELobbyGame::SetTransferPercent(unsigned char inPercent)
{
    ChangePlayerOption(GetMySlot(), 5, inPercent);
}

// FUNCTION: EXE 0x692630
// NEAR: as 0x29232f, GUIString::append(const GUIString&) is out of line in the original
void EELobbyGame::StartTransfer()
{
    if (mIsHost)
        return;
    mFileTransfer->CloseClients();
    GUIString thePath = EELobbyFn2988E9();
    thePath.append(GetGameFileName());
    mFileTransfer->SetPath(thePath);
    mHasFile = mFileTransfer->IsFileHere();
    LobbyFn3613E0();
    if (!mHasFile) {
        SetTransferPercent(0);
        mFileTransfer->Download();
    } else
        SetTransferPercent(100);
}

// FUNCTION: EXE 0x6926df
bool EELobbyGame::Virtual28()
{
    if (HasGameFile() && !mHasFile)
        return false;
    return true;
}

// FUNCTION: EXE 0x6926f9
long EELobbyGame::Virtual24()
{
    if (mNumHumanPlayers > 0 && mNumHumanPlayers + mB7E > 1 && mNumComputerPlayers == 0)
        return 1;
    return 0;
}

// FUNCTION: EXE 0x692719
bool EELobbyGame::Virtual20()
{
    bool theDifferent = false;
    unsigned char theTeam = 0xff;
    for (int i = 0; i < 8 && !theDifferent; i++) {
        EELobbyPlayerSlot* theSlot = GetPlayerSlot(i);
        if (theSlot != NULL) {
            unsigned char theState = theSlot->mOptions[0];
            if ((theState == 3 && theSlot->mOptions[4] == 0) || theState == 1) {
                if (theTeam == 0xff)
                    theTeam = theSlot->mOptions[1];
                else
                    theDifferent = theTeam != theSlot->mOptions[1];
            }
        }
    }
    if (!theDifferent && theTeam != 0) {
        LobbyMessageBox(gEELobbyGame_StartGameError, gEELobbyGame_EveryoneOnSameTeam);
        return false;
    }
    StopTransfer();
    return EELobbyFn296D1E();
}

// FUNCTION: EXE 0x692797
void EELobbyGame::Virtual18()
{
    StopTransfer();
}

void EELobbyGame::Virtual1C()
{
    StopTransfer();
}

// FUNCTION: EXE 0x69279c
void EELobbyGame::ResetSlots()
{
    mB7E = 0;
    mNumComputerPlayers = 0;
    mNumOpenSlots = 8;
    mNumHumanPlayers = 1;
    for (int i = 0; i < 8; i++) {
        mSlots[i].Fn299973();
        mSlots[i].mOptions[1] = 0;
        mSlots[i].mOptions[3] = 5;
    }
}

// FUNCTION: EXE 0x6927d0
void EELobbyGame::CountPlayers(bool inNotify)
{
    if (!LobbyFn3613C0())
        return;
    int theOpen = 8;
    mNumHumanPlayers = 0;
    mB7E = 0;
    mNumComputerPlayers = 0;
    for (int i = 0; i < 8; i++) {
        int theState = mSlots[i].mOptions[0];
        if (theState == 1 || theState == 2)
            theOpen--;
        if (theState == 3 && mSlots[i].mOptions[4] == 0)
            mNumHumanPlayers++;
        if (theState == 1)
            mB7E++;
        if (theState == 4 && mSlots[i].mOptions[4] == 0)
            mNumComputerPlayers++;
    }
    if (theOpen != mNumOpenSlots) {
        mNumOpenSlots = theOpen;
        if (inNotify)
            LobbyFn361260();
    }
    LobbyFn3613E0();
}

// FUNCTION: EXE 0x692847
EELobbyPlayerSlot* EELobbyGame::GetPlayerSlot(int inSlot)
{
    if (inSlot >= 0 && inSlot < 8)
        return &mSlots[inSlot];
    return NULL;
}

// FUNCTION: EXE 0x692862
bool EELobbyGame::HasGameFile()
{
    return GetOption(0) != 0;
}

// FUNCTION: EXE 0x692870
long EELobbyGame::GetOption(int inOption)
{
    if (inOption >= 0 && inOption < 16)
        return mOptions[inOption];
    return 0;
}

// FUNCTION: EXE 0x69288b
void EELobbyGame::WriteOption(WriteBuffer& outBuffer, int inOption)
{
    switch (sOptionTypes[inOption]) {
    case 2:
        outBuffer.AppendShort((short)mOptions[inOption]);
        break;
    case 1:
        outBuffer.AppendByte((unsigned char)mOptions[inOption]);
        break;
    case 0:
        outBuffer.AppendBool(mOptions[inOption] != 0);
        break;
    }
}

// FUNCTION: EXE 0x6928e0
void EELobbyGame::ReadOption(ReadBuffer& inBuffer, int inOption)
{
    long theValue;
    switch (sOptionTypes[inOption]) {
    case 2:
        theValue = inBuffer.ReadShort();
        break;
    case 1:
        theValue = (char)inBuffer.ReadByte();
        break;
    case 0:
        theValue = inBuffer.ReadBool() ? 1 : 0;
        break;
    default:
        return;
    }
    SetOption(inOption, theValue);
}

// FUNCTION: EXE 0x692933
static void EELobbyInitOptionTypes()
{
    if (sOptionTypesInitialized)
        return;
    sOptionTypesInitialized = true;
    sOptionTypes[0] = 1;
    sOptionTypes[1] = 1;
    sOptionTypes[2] = 1;
    sOptionTypes[3] = 1;
    sOptionTypes[4] = 1;
    sOptionTypes[5] = 1;
    sOptionTypes[6] = 1;
    sOptionTypes[7] = 1;
    sOptionTypes[8] = 1;
    sOptionTypes[9] = 2;
    sOptionTypes[10] = 1;
    sOptionTypes[11] = 0;
    sOptionTypes[12] = 0;
    sOptionTypes[13] = 0;
    sOptionTypes[14] = 0;
    sOptionTypes[15] = 0;
    sOptionLimits[0] = 3;
    sOptionLimits[1] = 6;
    sOptionLimits[2] = EELobbyFn298824();
    sOptionLimits[3] = 5;
    sOptionLimits[4] = 16;
    sOptionLimits[5] = 16;
    sOptionLimits[6] = 2;
    sOptionLimits[7] = 3;
    sOptionLimits[8] = 4;
    sOptionLimits[9] = 1201;
    sOptionLimits[10] = 7;
    sOptionLimits[11] = 2;
    sOptionLimits[12] = 2;
    sOptionLimits[13] = 2;
    sOptionLimits[14] = 2;
    sOptionLimits[15] = 2;
}

// FUNCTION: EXE 0x692a23
// a new game hosted by inClient: the file transfer, the default options, the preferences' last options
void EELobbyGame::Virtual30(LobbyClient* inClient)
{
    mFileTransfer = new EELobbyFileTransfer;
    mOptions[0] = 0;
    mOptions[1] = 1;
    mOptions[2] = 0;
    mOptions[3] = 0;
    mOptions[4] = 0;
    mOptions[5] = 0xe;
    mOptions[6] = 1;
    mOptions[7] = 1;
    mOptions[8] = 1;
    mOptions[9] = 0x1f4;
    mOptions[10] = 3;
    mOptions[11] = 0;
    mOptions[12] = 1;
    mOptions[13] = 0;
    mOptions[14] = 0;
    mOptions[15] = 1;
    EELobbyFn2988D8(this);
    ResetSlots();
    mSlots[0].SetPlayer(inClient, (EELobbyPlayer*)inClient->mPlayer.mPtr);
}

// FUNCTION: EXE 0x692ade
// the colours taken by the other slots (a slot reference: the strength-reduced pointer is based on mOptions[0])
void EELobbyGame::GetTakenColors(bool* outTaken, int inExcept)
{
    int i;
    for (i = 0; i < 16; i++)
        outTaken[i] = false;
    for (i = 0; i < 8; i++) {
        if (i != inExcept) {
            EELobbyPlayerSlot& theSlot = mSlots[i];
            if ((theSlot.mOptions[0] == 3 || theSlot.mOptions[0] == 4) && theSlot.mOptions[4] == 0 && theSlot.mOptions[2] < 16)
                outTaken[theSlot.mOptions[2]] = true;
        }
    }
}

// FUNCTION: EXE 0x692b29
bool EELobbyGame::IsColorFree(unsigned char inColor)
{
    bool theTaken[16];
    GetTakenColors(theTaken, -1);
    if (inColor < 16)
        return !theTaken[inColor];
    return false;
}

// FUNCTION: EXE 0x692b57
void EELobbyGame::Virtual50(WriteBuffer& outBuffer)
{
    outBuffer.AppendByte(GetOption(4));
    outBuffer.AppendByte(GetOption(0));
    outBuffer.AppendByte(GetOption(2));
}

// FUNCTION: EXE 0x692b95
bool EELobbyGame::Virtual58(ReadBuffer& inBuffer)
{
    SetOption(4, (char)inBuffer.ReadByte());
    SetOption(0, (char)inBuffer.ReadByte());
    SetOption(2, (char)inBuffer.ReadByte());
    return true;
}

// FUNCTION: EXE 0x692be0
void EELobbyGame::Virtual54(WriteBuffer& outBuffer)
{
    int i;
    for (i = 0; i < 16; i++)
        WriteOption(outBuffer, i);
    unsigned char theCount = 0;
    unsigned long thePos = outBuffer.mLength;
    outBuffer.AppendByte(0);
    for (i = 0; i < 8; i++) {
        EELobbyPlayerSlot* theSlot = GetPlayerSlot(i);
        if (theSlot != NULL && theSlot->mClient.mPtr != NULL) {
            theCount++;
            if (mL2C == 2)
                outBuffer.AppendShort(theSlot->mClient->mClientId);
            else
                outBuffer.AppendWString(theSlot->mClient->mName, 2);
        }
    }
    outBuffer.SetByte(thePos, theCount);
}

// FUNCTION: EXE 0x692c7e
// the details: every option, then the players' names (by client id when mL2C is 2) into mFiles
bool EELobbyGame::Virtual5C(ReadBuffer& inBuffer)
{
    mFiles.clear();
    int i;
    for (i = 0; i < 16; i++)
        ReadOption(inBuffer, i);
    if (inBuffer.mL08 - inBuffer.mL00 > 0) {
        unsigned char theCount = inBuffer.ReadByte();
        LobbyServer* theClients = LobbyFn35B6F0();
        if (theClients == NULL && mL2C == 2)
            return true;
        mFiles.reserve(theCount);
        for (i = theCount; i > 0; i--) {
            if (mL2C == 2) {
                unsigned short theId = inBuffer.ReadShort();
                LobbyClient* theClient = theClients->GetClient(theId);
                if (theClient != NULL)
                    mFiles.push_back(theClient->mName);
            } else {
                std::wstring theName;
                inBuffer.ReadWString(theName, 2);
                GUIString theString(theName);
                mFiles.push_back(theString);
            }
        }
    }
    return true;
}

// FUNCTION: EXE 0x692d71
unsigned char EELobbyGame::GetFreeColor(int inExcept, unsigned char inColor)
{
    bool theTaken[16];
    GetTakenColors(theTaken, inExcept);
    if (inColor < 16 && !theTaken[inColor])
        return inColor;
    for (int i = 0; i < 16; i++) {
        if (!theTaken[i])
            return i;
    }
    return 0;
}

// FUNCTION: EXE 0x692daa
void EELobbyGame::MakeColorsDistinct()
{
    for (int i = 0; i < 8; i++) {
        EELobbyPlayerSlot* theSlot = GetPlayerSlot(i);
        if (theSlot != NULL && (theSlot->mOptions[0] == 3 || theSlot->mOptions[0] == 4) && theSlot->mOptions[4] == 0)
            theSlot->mOptions[2] = GetFreeColor(i, theSlot->mOptions[2]);
    }
}

// FUNCTION: EXE 0x692df3
GUIString EELobbyGame::Virtual64(short inStatus)
{
    switch (inStatus) {
    case 10000:
        return gEELobbyGameStatus_NotInSavedGame;
    case 10001:
        return gEELobbyGameStatus_InvalidVersion;
    case 10002:
        return gEELobbyGameStatus_CorruptBinaries;
    }
    return gEmptyGUIString;
}

// FUNCTION: EXE 0x692e42
void EELobbyGame::Virtual34(WriteBuffer& outBuffer)
{
    GUIString theVersion;
    unsigned long theChecksum;
    EELobbyGetVersion(theVersion, theChecksum);
    outBuffer.AppendWString(theVersion, 2);
    outBuffer.AppendLong(theChecksum);
}

// FUNCTION: EXE 0x692e9f
// a join request: the same version and checksum, then a free slot (a saved game: the slot of the same name);
// 10001 / 10002 / 10000 / 2 refuse it
short EELobbyGame::Virtual38(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    GUIString theVersion(GetEmptyGUIStringData());
    unsigned long theChecksum;
    EELobbyGetVersion(theVersion, theChecksum);
    std::wstring theText;
    inBuffer.ReadWString(theText, 2);
    if (theVersion != GUIString(theText))
        return 10001;
    if (inBuffer.ReadLong() != theChecksum)
        return 10002;
    bool theSaved = HasGameFile();
    int i;
    for (i = 0; i < 8; i++) {
        if (!theSaved) {
            if (mSlots[i].mOptions[0] == 0)
                break;
        } else if (mSlots[i].mOptions[0] == 4 && inClient->mName == mSlots[i].GetName()) {
            break;
        }
    }
    if (i >= 8)
        return theSaved ? 10000 : 2;
    mSlots[i].SetPlayer(inClient, (EELobbyPlayer*)inClient->mPlayer.mPtr);
    if (!theSaved)
        mSlots[i].Reset();
    return 0;
}

// FUNCTION: EXE 0x693029
void EELobbyGame::Virtual3C(LobbyClient* inClient, WriteBuffer& outBuffer)
{
    int i;
    for (i = 0; i < 16; i++)
        WriteOption(outBuffer, i);
    for (i = 0; i < 8; i++)
        mSlots[i].WriteData(outBuffer);
    if (HasGameFile())
        WriteGameFile(outBuffer);
}

// FUNCTION: EXE 0x693075
int EELobbyGame::SeatClient(LobbyClient* inClient)
{
    if (inClient != NULL) {
        EELobbyPlayer* thePlayer = (EELobbyPlayer*)inClient->mPlayer.mPtr;
        if (thePlayer != NULL) {
            unsigned char theSlot = thePlayer->GetSlot();
            if (theSlot < 8) {
                mSlots[theSlot].SetPlayer(inClient, thePlayer);
                return theSlot;
            }
        }
    }
    return -1;
}

// FUNCTION: EXE 0x6930b5
// a joined game's state: a new file transfer, every option and slot, the room's clients into their slots
void EELobbyGame::Virtual44(ReadBuffer& inBuffer)
{
    mFileTransfer = new EELobbyFileTransfer;
    int i;
    for (i = 0; i < 16; i++)
        ReadOption(inBuffer, i);
    for (i = 0; i < 8; i++)
        mSlots[i].ReadData(inBuffer);
    if (HasGameFile())
        ReadGameFile(inBuffer);
    LobbyClientSetIterator theIterator(mClients.mPtr);
    while (theIterator.HasMore()) {
        LobbyClient* theClient = NULL;
        theIterator.Next(&theClient);
        SeatClient(theClient);
    }
    EELobbyPlayerSlot* theSlot = GetPlayerSlot(GetMySlot());
    if (theSlot != NULL)
        theSlot->mOptions[2] = GetFreeColor(theSlot->GetIndex(), 0);
    if (HasGameFile())
        StartTransfer();
}

// FUNCTION: EXE 0x693195
void EELobbyGame::Virtual4C(LobbyClient* inClient)
{
    int theSlot = SeatClient(inClient);
    if (theSlot < 0)
        return;
    EELobbyPlayerSlot* thePlayerSlot = GetPlayerSlot(theSlot);
    if (thePlayerSlot != NULL && !HasGameFile()) {
        thePlayerSlot->Reset();
        thePlayerSlot->mOptions[2] = GetFreeColor(thePlayerSlot->GetIndex(), 0);
    }
    CountPlayers(false);
    LobbyFn359100(new EELobbySyncPlayerEvent(theSlot), true);
}

// FUNCTION: EXE 0x69321e
void EELobbyGame::Virtual48(LobbyClient* inClient, LobbyPlayer* inPlayer)
{
    unsigned char theSlot = ((EELobbyPlayer*)inPlayer)->GetSlot();
    if (theSlot >= 8 || mSlots[theSlot].mClient.mPtr != inClient)
        return;
    if (HasGameFile()) {
        mSlots[theSlot].SetName(inClient->mName);
    } else {
        mSlots[theSlot].Fn299973();
        mSlots[theSlot].Reset();
    }
    CountPlayers(false);
    LobbyFn359100(new EELobbySyncPlayerEvent(theSlot), true);
    if (LobbyFn3613C0())
        Fn2933CE(theSlot, false);
}

// FUNCTION: EXE 0x6932d9
// a game message from inFrom
void EELobbyGame::Virtual60(unsigned char inType, ReadBuffer& inBuffer, LobbyClient* inFrom)
{
    switch (inType) {
    case 0x64:
        HandleOptionChange(inBuffer, inFrom);
        break;
    case 0x65:
        HandlePlayerOptionChange(inBuffer, inFrom);
        break;
    case 0x66:
        HandlePlayerOptionRequest(inBuffer, inFrom);
        break;
    case 0x67:
        HandleGameState(inBuffer, inFrom);
        break;
    case 0x68:
        Fn293364(inBuffer, inFrom, true);
        break;
    case 0x69:
        Fn293364(inBuffer, inFrom, false);
        break;
    case 0x6a:
        Fn29334D(inBuffer, inFrom);
        break;
    }
}

// FUNCTION: EXE 0x69334d
// message 0x6a
void EELobbyGame::Fn29334D(ReadBuffer& inBuffer, LobbyClient* inFrom)
{
    if (inFrom->Fn3A54E0(1))
        EELobbyFn295CC3();
}

// FUNCTION: EXE 0x693364
// messages 0x68 / 0x69: a slot's flag, from its own client (the host passes it on) or from the captain
void EELobbyGame::Fn293364(ReadBuffer& inBuffer, LobbyClient* inFrom, bool inFromClient)
{
    unsigned char theSlot = inBuffer.ReadByte();
    bool theFlag = inBuffer.ReadBool();
    if (inFromClient) {
        EELobbyPlayerSlot* thePlayerSlot = GetPlayerSlot(theSlot);
        if (thePlayerSlot != NULL && thePlayerSlot->mClient.mPtr == inFrom)
            Fn2933CE(theSlot, theFlag);
    } else if (inFrom->Fn3A54E0(1)) {
        EELobbyFn295CAB(theSlot, theFlag);
    }
}

// FUNCTION: EXE 0x6933ce
// message 0x69 to the lobby: a slot and a flag
void EELobbyGame::Fn2933CE(unsigned char inSlot, bool inFlag)
{
    if (!EELobbyFn295D52())
        return;
    WriteBuffer theBuffer(0, true);
    theBuffer.AppendByte(0x69);
    theBuffer.AppendByte(inSlot);
    theBuffer.AppendBool(inFlag);
    LobbyFn361B80(theBuffer.ToByteBuffer(true).get());
}

// FUNCTION: EXE 0x693437
// message 0x68 to the lobby: the local player's slot and inFlag
void EELobbyGame::Fn293437(bool inFlag)
{
    WriteBuffer theBuffer(0, true);
    theBuffer.AppendByte(0x68);
    theBuffer.AppendByte(GetMySlot());
    theBuffer.AppendBool(inFlag);
    LobbyFn361B60(theBuffer.ToByteBuffer(true).get());
}

// FUNCTION: EXE 0x6934a0
// message 0x6a to the lobby
void EELobbyGame::Fn2934A0()
{
    WriteBuffer theBuffer(0, true);
    theBuffer.AppendByte(0x6a);
    LobbyFn361B80(theBuffer.ToByteBuffer(true).get());
}

// FUNCTION: EXE 0x6934e8
// message 0x66 (the host): a client's own player option; accepted ones go to everyone, a refused one back to it
void EELobbyGame::HandlePlayerOptionRequest(ReadBuffer& inBuffer, LobbyClient* inFrom)
{
    if (!mIsHost)
        return;
    EELobbyPlayer* thePlayer = (EELobbyPlayer*)inFrom->mPlayer.mPtr;
    if (thePlayer == NULL)
        return;
    int theIndex = thePlayer->GetSlot();
    EELobbyPlayerSlot* theSlot = GetPlayerSlot(theIndex);
    if (theSlot == NULL || theSlot->mClient.mPtr != inFrom)
        return;
    int theWhat = inBuffer.ReadByte();
    unsigned char theValue = inBuffer.ReadByte();
    if (thePlayer->GetSlot() != theIndex)
        return;
    bool theOK = false;
    switch (theWhat) {
    case 1:
    case 4:
    case 5:
        theOK = true;
        break;
    case 2:
        theOK = IsColorFree(theValue);
        break;
    }
    WriteBuffer theBuffer(0, true);
    theBuffer.AppendByte(0x65);
    theBuffer.AppendByte(theIndex);
    theBuffer.AppendByte(theWhat);
    theBuffer.AppendByte(theOK ? theValue : theSlot->GetOption(theWhat));
    if (theOK)
        LobbyFn361B80(theBuffer.ToByteBuffer(true).get());
    else
        LobbyFn361BA0(inFrom, theBuffer.ToByteBuffer(true).get());
}

// FUNCTION: EXE 0x693623
// message 0x65 (from the captain): a slot's option; a team change involving the local player's team is announced
void EELobbyGame::HandlePlayerOptionChange(ReadBuffer& inBuffer, LobbyClient* inFrom)
{
    if (!inFrom->Fn3A54E0(1))
        return;
    int theIndex = (char)inBuffer.ReadByte();
    int theWhat = inBuffer.ReadByte();
    unsigned char theValue = inBuffer.ReadByte();
    EELobbyPlayerSlot* theSlot = GetPlayerSlot(theIndex);
    if (theSlot != NULL) {
        if (theWhat == 1) {
            if (theSlot->mClient.mPtr != NULL && theIndex != GetMySlot()) {
                unsigned char theOld = theSlot->mOptions[1];
                unsigned char theTeam = GetMyTeam();
                if (theTeam != 0 && theOld != theValue) {
                    if (theOld == theTeam)
                        LobbyFn359100(new PlayerTeamChangedEvent(theSlot->mClient.get(), true), true);
                    else if (theValue == theTeam)
                        LobbyFn359100(new PlayerTeamChangedEvent(theSlot->mClient.get(), false), true);
                }
            }
        }
        if (theWhat == 4 && theValue == 0) {
            theSlot->mOptions[2] = GetFreeColor(theSlot->mIndex, theSlot->mOptions[2]);
        }
        theSlot->SetOption(theWhat, theValue);
        if (theWhat == 0 || theWhat == 4)
            CountPlayers(true);
    }
    LobbyFn359100(new EELobbySyncPlayerEvent(theIndex), true);
    if (theWhat == 3)
        LobbyFn361290();
}

// FUNCTION: EXE 0x69379c
// message 0x64 (from the captain): one option
void EELobbyGame::HandleOptionChange(ReadBuffer& inBuffer, LobbyClient* inFrom)
{
    if (!inFrom->Fn3A54E0(1))
        return;
    int theOption = inBuffer.ReadByte();
    ReadOption(inBuffer, theOption);
    LobbyFn359100(new EELobbySyncOptionEvent(theOption), true);
    LobbyFn361290();
}

// SYNTHETIC: EXE 0x693804
// (EELobbySyncOptionEvent's scalar deleting destructor; ICF: EELobbySyncPlayerEvent's)

// FUNCTION: EXE 0x69382d
// message 0x64 to the lobby: one option and its value
void EELobbyGame::SendOption(int inOption)
{
    WriteBuffer theBuffer(0, true);
    theBuffer.AppendByte(0x64);
    theBuffer.AppendByte(inOption);
    WriteOption(theBuffer, inOption);
    LobbyFn361B80(theBuffer.ToByteBuffer(true).get());
}

// FUNCTION: EXE 0x693894
void EELobbyGame::SetOption(int inOption, unsigned long inValue)
{
    if (inOption >= 0 && inOption < 16 && inValue < sOptionLimits[inOption])
        mOptions[inOption] = inValue;
}

// FUNCTION: EXE 0x6938b8
// an option set from the lobby: below its limit and changed (inFlag: SendOption reports it); the starting epoch never
// passes the ending one (unless the end is 15); an out-of-range value makes the controls resync the option
void EELobbyGame::ChangeOption(int inOption, unsigned long inValue, bool inFlag)
{
    if (inOption < 0 || inOption >= 16)
        return;
    if (inValue >= sOptionLimits[inOption]) {
        LobbyFn359100(new EELobbySyncOptionEvent(inOption), true);
        return;
    }
    if (mOptions[inOption] == inValue)
        return;
    mOptions[inOption] = inValue;
    if (inFlag)
        SendOption(inOption);
    if (inOption == 4) {
        if ((unsigned long)mOptions[5] < (unsigned long)mOptions[4] && mOptions[4] != 0xf)
            ChangeOption(5, mOptions[4], inFlag);
    } else if (inOption == 5) {
        if ((unsigned long)mOptions[5] < (unsigned long)mOptions[4] && mOptions[4] != 0xf)
            ChangeOption(4, mOptions[5], inFlag);
        return;
    } else if (inOption != 2) {
        return;
    }
    LobbyFn361260();
}

// FUNCTION: EXE 0x69398e
int EELobbyGame::GetMySlot()
{
    EELobbyPlayer* thePlayer = (EELobbyPlayer*)GetMyLobbyPlayer();
    if (thePlayer != NULL)
        return thePlayer->GetSlot();
    return -1;
}

// FUNCTION: EXE 0x6939a6
int EELobbyGame::GetMyTeam()
{
    EELobbyPlayer* thePlayer = (EELobbyPlayer*)GetMyLobbyPlayer();
    if (thePlayer != NULL)
        return thePlayer->GetTeam();
    return -1;
}

// FUNCTION: EXE 0x6939be
// message 0x67 (from the captain): the whole game, every option and slot
void EELobbyGame::HandleGameState(ReadBuffer& inBuffer, LobbyClient* inFrom)
{
    if (!inFrom->Fn3A54E0(1))
        return;
    int i;
    for (i = 0; i < 16; i++)
        ReadOption(inBuffer, i);
    for (i = 0; i < 8; i++) {
        mSlots[i].ReadData(inBuffer);
        if (mSlots[i].mOptions[0] == 3) {
            LobbyClient* theClient = mClients.mPtr->FindClient(inBuffer.ReadShort());
            if (theClient != NULL && theClient->mPlayer.mPtr != NULL)
                mSlots[i].SetPlayer(theClient, (EELobbyPlayer*)theClient->mPlayer.mPtr);
            else
                mSlots[i].Fn299973();
        }
    }
    if (HasGameFile()) {
        ReadGameFile(inBuffer);
        StartTransfer();
        MakeColorsDistinct();
    }
    LobbyFn359120(100065, true);
    LobbyFn361290();
    if (mIsHost) {
        CountPlayers(false);
        LobbyFn361260();
    }
}

// FUNCTION: EXE 0x693a8b
// message 0x67 to the lobby: every option, every slot (a client slot with its client's id), the saved game
void EELobbyGame::SendGameState()
{
    WriteBuffer theBuffer(0, true);
    theBuffer.AppendByte(0x67);
    int i;
    for (i = 0; i < 16; i++)
        WriteOption(theBuffer, i);
    for (i = 0; i < 8; i++) {
        mSlots[i].WriteData(theBuffer);
        if (mSlots[i].mOptions[0] == 3)
            theBuffer.AppendShort(mSlots[i].mClient->mClientId);
    }
    if (HasGameFile())
        WriteGameFile(theBuffer);
    LobbyFn361B80(theBuffer.ToByteBuffer(true).get());
}

// FUNCTION: EXE 0x693b36
// a player option changed in the lobby's controls: the host (mB15) or the slot's own player sends it on (0x65 / 0x66);
// a refused change makes the controls resync the slot
void EELobbyGame::ChangePlayerOption(int inSlot, int inWhat, unsigned char inValue)
{
    EELobbyPlayerSlot* theSlot = GetPlayerSlot(inSlot);
    if (theSlot == NULL || theSlot->GetOption(inWhat) == inValue)
        return;
    bool theToHost = false;
    bool theOK = true;
    switch (inWhat) {
    case 1:
    case 2:
    case 4:
    case 5:
        theOK = inSlot == GetMySlot() || (mIsHost && theSlot->mOptions[0] != 3);
        theToHost = true;
        break;
    case 0:
        if (mIsHost && inSlot != 0)
            LobbyFn361BC0(theSlot->mClient.get(), false);
        else
            theOK = false;
    default:
        if (!mIsHost)
            theOK = false;
        break;
    }
    if (inWhat == 2 && theOK)
        theOK = IsColorFree(inValue);
    if (!theOK) {
        LobbyFn359100(new EELobbySyncPlayerEvent(inSlot), true);
        return;
    }
    if (mIsHost)
        theToHost = false;
    WriteBuffer theBuffer(0, true);
    if (theToHost) {
        theBuffer.AppendByte(0x66);
        theBuffer.AppendByte(inWhat);
        theBuffer.AppendByte(inValue);
        LobbyFn361B60(theBuffer.ToByteBuffer(true).get());
    } else {
        theBuffer.AppendByte(0x65);
        theBuffer.AppendByte(inSlot);
        theBuffer.AppendByte(inWhat);
        theBuffer.AppendByte(inValue);
        LobbyFn361B80(theBuffer.ToByteBuffer(true).get());
    }
}

// The unit's compiler-generated functions:
// SYNTHETIC: EXE 0x692296
// (??_G EELobbyGame)
// SYNTHETIC: EXE 0x6923f3
// (GUIString::append(const GUIString&), inline)
// Not written yet: 0x292a23, 0x292b57..0x293894, 0x2938b8, 0x2939be..0x2942c8 (the sync events, the remaining
// LobbyGame overrides, ChangeOption, SendGameState, ChangePlayerOption, the template COMDATs).

// TEMPLATE: EXE 0x693cd4
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x693cf7
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x693dc8
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x693df4
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x693e2f
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x693e7f
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x693ee9
// (vector<GUIString>::_Tidy)

// TEMPLATE: EXE 0x693f21
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x6941d4
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x6941e0
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x694207
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x69422f
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x694254
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x694273
// (compiler-generated; name in analysis/exe/names.txt)

// TEMPLATE: EXE 0x6942a1
// (compiler-generated; name in analysis/exe/names.txt)
