// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyInit.cpp (__FILE__ 0x8d0be8, passed by the component
// getters of lines 328/329). The game's set-up of the WON lobby: its factories, the server list's column handlers, the
// component types of the resource file, and the accessors of the lobby's dialogs: 0x294d62..0x296aaa (its two
// SmartPtr chains first; EELobbyGameOptionsCtrl.cpp ends at 0x294d62). Unit flags: the exe's module set
// (r12-misc-h2, round 12).
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
using namespace WONAPI;

SmartPtr<EELobbyConnectDialog> gEELobbyConnectDialog;
SmartPtr<EELobbyFileDialog> gEELobbyFileDialog;

// FUNCTION: EXE 0x694dbe
EELobbyConfig::EELobbyConfig()
{
    KeyAdd("EEFileTransferPort", mFileTransferPort, 8889);
}

// FUNCTION: EXE 0x694ebc
GUIString EELobbyServerDetailsItem::GetOptionText(int inOption, int inFirstString, unsigned long inCount)
{
    unsigned long theValue = GetGame()->GetOption(inOption);
    if (theValue < inCount)
        return EELobbyResource_GetString(theValue + inFirstString);
    return gEmptyGUIString;
}

// FUNCTION: EXE 0x694f02
GUIString EELobbyServerDetailsItem::GetNumberText(int inOption)
{
    char theBuffer[50];
    sprintf(theBuffer, "%d", GetGame()->GetOption(inOption));
    return GUIString(theBuffer);
}

// FUNCTION: EXE 0x694f39
GUIString EELobbyServerDetailsItem::GetWondersText()
{
    if (GetGame()->GetOption(10) == 0)
        return gEELobbyGameOptionsCtrl_WondersOff;
    return GetNumberText(10);
}

// FUNCTION: EXE 0x694f7f
GUIString EELobbyServerDetailsItem::GetEpochText(bool inStart)
{
    int theOption = inStart ? 4 : 5;
    if (GetGame()->GetOption(theOption) == 15)
        return gEELobbyGameOptionsCtrl_Epoch16;
    return GetOptionText(theOption, 0, 16);
}

// FUNCTION: EXE 0x694fd7
GUIString EELobbyServerDetailsItem::GetOnOffText(int inOption)
{
    return GetGame()->GetOption(inOption) != 0 ? gEELobbyGameOptionsCtrl_OptionOn : gEELobbyGameOptionsCtrl_OptionOff;
}

// FUNCTION: EXE 0x695010
void EELobbyServerDetailsItem::PaintDetailsHook(Graphics& inGraphics, MultiListArea* inList)
{
    EELobbyGame* theGame = GetGame();
    int theCounts[4] = { 8, 8, 7, 7 };
    GUIString theLabels1[8] = { gEELobbyGameOptionsCtrl_GameType, gEELobbyGameOptionsCtrl_MapSize,
        gEELobbyGameOptionsCtrl_MapType, gEELobbyGameOptionsCtrl_Resources, gEELobbyGameOptionsCtrl_StartingEpoch,
        gEELobbyGameOptionsCtrl_EndingEpoch, gEELobbyGameOptionsCtrl_GameUnitLimit, gEELobbyGameOptionsCtrl_GameVariant };
    GUIString theValues1[8] = { GetOptionText(0, 0x10, 3), GetOptionText(1, 0x13, 6),
        EELobbyGetMapTypeName(theGame->GetOption(2)), GetOptionText(3, 0x19, 5), GetEpochText(true),
        GetEpochText(false), GetNumberText(9), GetOptionText(6, 0x1e, 2) };
    GUIString theLabels2[7] = { gEELobbyGameOptionsCtrl_DifficultyLevel, gEELobbyGameOptionsCtrl_GameSpeed,
        gEELobbyGameOptionsCtrl_RevealMap, gEELobbyGameOptionsCtrl_UseCustomCivs, gEELobbyGameOptionsCtrl_LockTeams,
        gEELobbyGameOptionsCtrl_LockSpeed, gEELobbyGameOptionsCtrl_NumberOfWonders };
    GUIString theValues2[7] = { GetOptionText(7, 0x20, 3), GetOptionText(8, 0x23, 4), GetOnOffText(0xb),
        GetOnOffText(0xc), GetOnOffText(0xd), GetOnOffText(0xe), GetWondersText() };
    GUIString* theColumns[4] = { theLabels1, theValues1, theLabels2, theValues2 };
    int theGaps[4] = { 10, 50, 10, 50 };
    unsigned long theColors[4] = { 0x8000000f, 0x8000000e, 0x8000000f, 0x8000000e };
    Font* theFont = inGraphics.mFont.get(); int theX = 0, theY, i;
    for (i = 0; i < 4; i++) {
        theY = 0;
        int theWidth = 0;
        inGraphics.SetColor(theColors[i]);
        for (int j = 0; j < theCounts[i]; j++) {
            int theTextWidth = theFont->GetStringWidth(theColumns[i][j], 0, theColumns[i][j].length());
            if (theTextWidth > theWidth)
                theWidth = theTextWidth;
            inGraphics.DrawString(theColumns[i][j], theX, theY);
            theY += theFont->GetHeight();
        }
        theX += theGaps[i] + theWidth;
    }
    theY = 0;
    inGraphics.SetColor(0x8000000f);
    inGraphics.DrawString(gEELobbyInterface_Players, theX, 0);
    theX += theFont->GetStringWidth(gEELobbyInterface_Players, 0, gEELobbyInterface_Players.length()) + 10;
    inGraphics.SetColor(0x8000000e);
    int theNumFiles = theGame->GetNumFiles();
    for (i = 0; i < theNumFiles; i++) {
        const GUIString& theFile = theGame->GetFile(i);
        inGraphics.DrawString(theFile, theX, theY);
        theY += theFont->GetHeight();
    }
}

// FUNCTION: EXE 0x6954fc
int EELobbyServerDetailsItem::GetDetailsHeightHook(MultiListArea* inList)
{
    return inList->mFont->GetHeight() * 9;
}

static LobbyPlayer* EELobbyNewPlayer();
static LobbyGame* EELobbyNewGame();
static ComponentConfig* EELobbyNewButtonConfig();
static ComponentConfig* EELobbyNewContainerConfig();
static ComponentConfig* EELobbyNewDialogConfig();
static Component* EELobbyNewGameOptionsCtrl();
static Component* EELobbyNewPlayerOptionsCtrl();
static Component* EELobbyNewColorButton();
static Component* EELobbyNewFileDialog();
static Component* EELobbyNewConnectDialog();
static LobbyConfig* EELobbyNewConfig();
static ServerDetailsItem* EELobbyNewServerDetailsItem();
static void EELobbyEpochGetText(Graphics& inGraphics, ServerDetailsItem* inItem, ServerListCtrl*);
static int EELobbyEpochCompare(const ServerDetailsItem* inA, const ServerDetailsItem* inB, ServerListCtrl*);
static void EELobbyEpochFilter(const GUIString& inText, std::list<ServerDetailsItem*>& ioItems, ServerListCtrl*);
static void EELobbyMapTypeGetText(Graphics& inGraphics, ServerDetailsItem* inItem, ServerListCtrl*);
static int EELobbyMapTypeCompare(const ServerDetailsItem* inA, const ServerDetailsItem* inB, ServerListCtrl*);
static void EELobbyMapTypeFilter(const GUIString& inText, std::list<ServerDetailsItem*>& ioItems, ServerListCtrl*);
static void EELobbyGetComponents(ComponentConfig* inConfig);

// FUNCTION: EXE 0x695511
// NEAR: the original calls the GUIString temporaries' dtor out of line (0x2941d4); ours expands it
void EELobbyInit()
{
    ServerListCtrl::ColumnInfo* theHandlers = ServerListCtrl::GetColumnInfo("Epoch");
    theHandlers->mPaintFunc = EELobbyEpochGetText;
    theHandlers->mCompareFunc = EELobbyEpochCompare;
    theHandlers->mFilterFunc = EELobbyEpochFilter;
    theHandlers = ServerListCtrl::GetColumnInfo("MapType");
    theHandlers->mPaintFunc = EELobbyMapTypeGetText;
    theHandlers->mCompareFunc = EELobbyMapTypeCompare;
    theHandlers->mFilterFunc = EELobbyMapTypeFilter;

    LobbyGame::SetGameFactory(EELobbyNewGame);
    LobbyPlayer::SetPlayerFactory(EELobbyNewPlayer);
    ServerDetailsItem::SetItemFactory(EELobbyNewServerDetailsItem);
    LobbyConfig::SetAllocator(EELobbyNewConfig);

    ResourceConfigTablePtr theCollection = EELobbyResource_Init(NULL);
    theCollection->AddAllocator("GameOptionsCtrl", EELobbyNewContainerConfig, EELobbyNewGameOptionsCtrl);
    theCollection->AddAllocator("PlayerOptionsCtrl", EELobbyNewContainerConfig, EELobbyNewPlayerOptionsCtrl);
    theCollection->AddAllocator("EELobbyColorButton", EELobbyNewButtonConfig, EELobbyNewColorButton);
    theCollection->AddAllocator("EELobbyFileDialog", EELobbyNewDialogConfig, EELobbyNewFileDialog);
    theCollection->AddAllocator("EELobbyConnectDialog", EELobbyNewDialogConfig, EELobbyNewConnectDialog);
    EELobbyFn2989EF(theCollection.mPtr);
    LobbyAddComponentLoader(theCollection.mPtr, EELobbyGetComponents);

    for (int i = 0; i < 16; i++)
        LobbyAddGameOption("Epoch", EELobbyResource_GetString(i), true);
    EELobbyFn2987BD();
    EELobbyFn298829(gEELobbyColors, 16);
    LobbyAddGameOption("MapType", gEELobbyGameOptionsCtrl_GameType2, true);
    LobbySetProductName(L"EEAOC", 0);
}

// FUNCTION: EXE 0x6957fa
static LobbyPlayer* EELobbyNewPlayer()
{
    return new EELobbyPlayer;
}

// FUNCTION: EXE 0x695811
static LobbyGame* EELobbyNewGame()
{
    return new EELobbyGame;
}

// FUNCTION: EXE 0x69582b
static ComponentConfig* EELobbyNewButtonConfig()
{
    return new ButtonConfig;
}

// FUNCTION: EXE 0x695842
static ComponentConfig* EELobbyNewContainerConfig()
{
    return new ContainerConfig;
}

// FUNCTION: EXE 0x69585c
static ComponentConfig* EELobbyNewDialogConfig()
{
    return new DialogConfig;
}

// FUNCTION: EXE 0x69589f
static Component* EELobbyNewGameOptionsCtrl()
{
    return new EELobbyGameOptionsCtrl;
}

// FUNCTION: EXE 0x6958b9
static Component* EELobbyNewPlayerOptionsCtrl()
{
    return new EELobbyPlayerOptionsCtrl;
}

// FUNCTION: EXE 0x6958d3
static Component* EELobbyNewColorButton()
{
    return new EELobbyColorButton;
}

// FUNCTION: EXE 0x6958ea
static Component* EELobbyNewFileDialog()
{
    return new EELobbyFileDialog;
}

// FUNCTION: EXE 0x695904
static Component* EELobbyNewConnectDialog()
{
    return new EELobbyConnectDialog;
}

// FUNCTION: EXE 0x69591e
static LobbyConfig* EELobbyNewConfig()
{
    return new EELobbyConfig;
}

// FUNCTION: EXE 0x695938
static ServerDetailsItem* EELobbyNewServerDetailsItem()
{
    return new EELobbyServerDetailsItem;
}

static GUIString EELobbyGetEpochText(ServerDetailsItem* inItem, bool inStart);
static GUIString EELobbyGetMapTypeText(EELobbyGame* inGame);

// FUNCTION: EXE 0x695978
static void EELobbyEpochGetText(Graphics& inGraphics, ServerDetailsItem* inItem, ServerListCtrl*)
{
    GUIString theText = EELobbyGetEpochText(inItem, true);
    inGraphics.DrawString(theText, 0, 0);
}

// FUNCTION: EXE 0x6959aa
// the flag is not passed on: every caller asks for the start epoch
static GUIString EELobbyGetEpochText(ServerDetailsItem* inItem, bool inStart)
{
    return ((EELobbyServerDetailsItem*)inItem)->GetEpochText(true);
}

// FUNCTION: EXE 0x6959be
static int EELobbyEpochCompare(const ServerDetailsItem* inA, const ServerDetailsItem* inB, ServerListCtrl*)
{
    EELobbyGame* theA = static_cast<EELobbyGame*>(inA->mGame.get());
    long theB = (static_cast<EELobbyGame*>(inB->mGame.get()))->GetOption(4);
    return theA->GetOption(4) - theB;
}

// FUNCTION: EXE 0x6959e5
// NEAR: the compared text's pointer goes through a register first in the original (an inline accessor?)
static void EELobbyEpochFilter(const GUIString& inText, std::list<ServerDetailsItem*>& ioItems, ServerListCtrl*)
{
    std::list<ServerDetailsItem*>::iterator theIterator = ioItems.begin();
    while (theIterator != ioItems.end()) {
        GUIString theText = EELobbyGetEpochText(*theIterator, true);
        if (theText.Compare(inText, false) >= 0)
            ++theIterator;
        else
            ioItems.erase(theIterator++);
    }
}

// FUNCTION: EXE 0x695a59
static void EELobbyMapTypeGetText(Graphics& inGraphics, ServerDetailsItem* inItem, ServerListCtrl*)
{
    GUIString theText = EELobbyGetMapTypeText(static_cast<EELobbyGame*>(inItem->mGame.get()));
    inGraphics.DrawString(theText, 0, 0);
}

// FUNCTION: EXE 0x695a8b
static GUIString EELobbyGetMapTypeText(EELobbyGame* inGame)
{
    if (inGame->GetOption(0) == 0)
        return EELobbyGetMapTypeName(inGame->GetOption(2));
    return gEELobbyGameOptionsCtrl_GameType2;
}

// FUNCTION: EXE 0x695ad1
// a case-insensitive compare (0x328030; r28: was the Find 0x328340)
static int EELobbyMapTypeCompare(const ServerDetailsItem* inA, const ServerDetailsItem* inB, ServerListCtrl*)
{
    GUIString theA = EELobbyGetMapTypeText(static_cast<EELobbyGame*>(inA->mGame.get()));
    GUIString theB = EELobbyGetMapTypeText(static_cast<EELobbyGame*>(inB->mGame.get()));
    return theA.CompareNoCase(theB);
}

// FUNCTION: EXE 0x695b21
// NEAR: as the epoch filter
static void EELobbyMapTypeFilter(const GUIString& inText, std::list<ServerDetailsItem*>& ioItems, ServerListCtrl*)
{
    std::list<ServerDetailsItem*>::iterator theIterator = ioItems.begin();
    while (theIterator != ioItems.end()) {
        GUIString theText = EELobbyGetMapTypeText(static_cast<EELobbyGame*>((*theIterator)->mGame.get()));
        if (theText.Compare(inText, false) >= 0)
            ++theIterator;
        else
            ioItems.erase(theIterator++);
    }
}
// (the original's line 328 is the first getter call below: blank lines stand in for code not reconstructed)
// FUNCTION: EXE 0x695b95
static void EELobbyGetComponents(ComponentConfig* inConfig)
{
    SafeGetComponent(inConfig, gEELobbyFileDialog, "EELobbyFileDialog", __FILE__, __LINE__);
    SafeGetComponent(inConfig, gEELobbyConnectDialog, "EELobbyConnectDialog", __FILE__, __LINE__);
}

// FUNCTION: EXE 0x695bd2
// NEAR: ICF alias: both assignments call SmartPtr<T>::operator=(T*) 0x299ae3 (bound to the GUIStringData instance)
void EELobbyShutdown()
{
    EELobbyFn297E51();
    EELobbyResource_Destroy();
    gEELobbyFileDialog = NULL;
    gEELobbyConnectDialog = NULL;
}

// FUNCTION: EXE 0x695bf5
unsigned short EELobbyGetFileTransferPort()
{
    EELobbyConfig* theConfig = (EELobbyConfig*)GetLobbyConfig();
    if (theConfig == NULL)
        return 8889;
    return theConfig->mFileTransferPort;
}

// FUNCTION: EXE 0x695c0b
EELobbyFileDialog* EELobbyGetFileDialog()
{
    return gEELobbyFileDialog.mPtr;
}

// FUNCTION: EXE 0x695c11
EELobbyConnectDialog* EELobbyGetConnectDialog()
{
    return gEELobbyConnectDialog.mPtr;
}

// FUNCTION: EXE 0x695c17
bool EELobbyFn295C17()
{
    SmartPtr<EELobbyConnectDialog> theDialog = EELobbyGetConnectDialog();
    if (theDialog.mPtr == NULL)
        return false;
    EELobbyGame* theGame = (EELobbyGame*)GetMyLobbyGame();
    if (theGame == NULL)
        return false;
    bool theFlag = theGame->mIsHost;
    bool theResult = theDialog->Run(theGame);
    theGame = (EELobbyGame*)GetMyLobbyGame();
    if (theGame == NULL)
        return false;
    if (!theResult) {
        if (theFlag) {
            LobbyFn3612B0(0);
            theGame->Fn2934A0();
        } else if (!theDialog->mIsEnded) {
            theGame->Fn293437(false);
            LobbyFn361310(false);
        }
    }
    return theResult;
}

// FUNCTION: EXE 0x695cab
void EELobbyFn295CAB(unsigned char inSlot, bool inConnected)
{
    if (gEELobbyConnectDialog.mPtr != NULL)
        gEELobbyConnectDialog->SetConnected(inSlot, inConnected);
}

// FUNCTION: EXE 0x695cc3
void EELobbyFn295CC3()
{
    if (gEELobbyConnectDialog.mPtr != NULL && !LobbyFn3613C0())
        gEELobbyConnectDialog->End(0);
}

static void EELobbyFn295D38(int inFlag);

// FUNCTION: EXE 0x695ce3
void EELobbyFn295CE3(bool inFlag)
{
    WindowManager* theWindow = GetLobbyWindow();
    if (theWindow != NULL)
        theWindow->PostEvent(new UserEvent(EELobbyFn295D38, inFlag != false), true);
}

// FUNCTION: EXE 0x695d38
static void EELobbyFn295D38(int inFlag)
{
    EELobbyGame* theGame = (EELobbyGame*)GetMyLobbyGame();
    if (theGame != NULL)
        theGame->Fn293437(inFlag != 0);
}

// FUNCTION: EXE 0x695d52
bool EELobbyFn295D52()
{
    if (gEELobbyConnectDialog.mPtr != NULL)
        return gEELobbyConnectDialog->mIsRunning;
    return false;
}

// SYNTHETIC: EXE 0x694d62
// SYNTHETIC: EXE 0x694d6c
// SYNTHETIC: EXE 0x694d74
// SYNTHETIC: EXE 0x694d80
// SYNTHETIC: EXE 0x694d90
// SYNTHETIC: EXE 0x694d9a
// SYNTHETIC: EXE 0x694da2
// SYNTHETIC: EXE 0x694dae

// The unit's other compiler-generated functions (the vtables' ??_G, inline virtuals, the getter templates):
// SYNTHETIC: EXE 0x694de8
// (??_G EELobbyConfig)
// SYNTHETIC: EXE 0x695883
// (??_G DialogConfig, emitted with EELobbyNewDialogConfig's inline ctor)
// SYNTHETIC: EXE 0x69595c
// (??_G EELobbyServerDetailsItem)
// SYNTHETIC: EXE 0x695d30
// (UserEvent::Deliver)
// TEMPLATE: EXE 0x696948
// (SafeGetComponent<EELobbyFileDialog>)
// TEMPLATE: EXE 0x6969c8
// (SafeGetComponent<EELobbyConnectDialog>)
// The implicit dtors of EELobbyConfig (LobbyConfig's, ICF) and ConfigObject (r24 ui P1 layouts), then the map/list
// COMDATs they instantiate (r24-lobby-player-gfx):
// TEMPLATE: EXE 0x694e04
// TEMPLATE: EXE 0x694e83
// TEMPLATE: EXE 0x695d65
// TEMPLATE: EXE 0x695d90
// TEMPLATE: EXE 0x695dbb
// TEMPLATE: EXE 0x695de6
// TEMPLATE: EXE 0x695e57
// TEMPLATE: EXE 0x695ec8
// TEMPLATE: EXE 0x695f39
// TEMPLATE: EXE 0x6961f1
// TEMPLATE: EXE 0x69622d
// TEMPLATE: EXE 0x6964e3
// TEMPLATE: EXE 0x69651d
// TEMPLATE: EXE 0x6967d3
// TEMPLATE: EXE 0x69680d
// TEMPLATE: EXE 0x696848
// TEMPLATE: EXE 0x69688a
// TEMPLATE: EXE 0x6968ce
// TEMPLATE: EXE 0x696910
// TEMPLATE: EXE 0x696a48
// TEMPLATE: EXE 0x696a74
// TEMPLATE: EXE 0x696a8f
