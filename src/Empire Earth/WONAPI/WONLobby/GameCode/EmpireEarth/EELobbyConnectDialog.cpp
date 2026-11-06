// C:\EEX\Empire Earth\WONAPI\WONLobby\GameCode\EmpireEarth\EELobbyConnectDialog.cpp (__FILE__ 0x8d03b0, InitComponent's
// lines 61..62). The dialog the players wait in while a game launches: one list line per player, in the "connected"
// font once the player answered; it ends by itself when all have (End) or with its cancel button:
// 0x2909a1..0x290e89 (the two SafeGetComponent COMDATs close it; EELobbyFileDialog.cpp's ctor follows). The list
// line class lives in the file's anonymous namespace (RTTI ConnectListItem@?%<file><hash>@@). Module flags
//.
#include <windows.h>
#include <string>
#include <list>
#include <vector>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"

using namespace WONAPI;

extern GUIString gEELobbyConnectDialog_AbortLaunch;    // 0x96598c (EELobbyResource.cpp)
extern GUIString gEELobbyConnectDialog_LeaveGame;      // 0x965948 (EELobbyResource.cpp)

namespace {

// one player's line (0x2c bytes; vtable 0x84c2c0: TextListItem's slots)
class ConnectListItem : public TextListItem {
public:
    ConnectListItem(const GUIString& inName, Image* inImage, unsigned char inSlot) : TextListItem(inName, inImage)
    {
        mSlot = inSlot;
    }

    unsigned char mSlot;                    // +0x28
};

}

// FUNCTION: EXE 0x6909a1
EELobbyConnectDialog::EELobbyConnectDialog() : mIsRunning(false)
{
}

// FUNCTION: EXE 0x6909f6
EELobbyConnectDialog::~EELobbyConnectDialog()
{
}

// The original's InitComponent asks for its two components on lines 61 and 62 (__LINE__); the lines below keep
// them there.





// FUNCTION: EXE 0x690a41
void EELobbyConnectDialog::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* theConfig = ((ComponentConfigInit&)inInit).mConfig;
        SafeGetComponent(theConfig, mConnectList, "ConnectList", __FILE__, __LINE__);
        SafeGetComponent(theConfig, mCancelButton, "CancelButton", __FILE__, __LINE__);
    }
}

// FUNCTION: EXE 0x690a8a
// NEAR: equal but for ConnectListItem's vtable: VC6 names the anonymous namespace ?%<file><number>, the number
// changes with every compile (original 2082521601), so the vtable cannot be bound by name (a tool item)
bool EELobbyConnectDialog::Run(EELobbyGame* inGame)
{
    if (mIsRunning)
        return false;
    bool theAllConnected = true;
    mCancelButton->SetText(inGame->mIsHost ? gEELobbyConnectDialog_AbortLaunch : gEELobbyConnectDialog_LeaveGame, true);
    mConnectList->Fn2FAA00();
    mConnectList->Fn2FAA50(true);
    for (int i = 0; i < 8; i++) {
        EELobbyPlayerSlot* theSlot = inGame->GetPlayerSlot(i);
        if (theSlot != NULL && theSlot->mOptions[0] == 3) {
            Image* theImage = (i == 0 ? ::GameUserListCtrl_Ready_Image : ::LobbyGlobal_Searching_Image).mPtr;
            mConnectList->InsertItem(new ConnectListItem(theSlot->GetName(), theImage, i), -1);
            if (theImage != ::GameUserListCtrl_Ready_Image.mPtr)
                theAllConnected = false;
        }
    }
    mConnectList->Fn2FAA10();
    if (theAllConnected)
        return true;
    mConnectList->Fn2F5890();
    mIsEnded = false;
    mIsRunning = true;
    int theResult = LobbyDoDialog(this);
    mIsRunning = false;
    return theResult == 100001;
}

// FUNCTION: EXE 0x690c85
void EELobbyConnectDialog::End(bool inOK)
{
    Window* theRoot = Fn2F5940();
    if (theRoot != NULL) {
        mIsEnded = true;
        theRoot->Fn32B0A0(inOK ? 100001 : 100002, this);
    }
}

// FUNCTION: EXE 0x690cb3
void EELobbyConnectDialog::SetConnected(unsigned char inSlot, bool inConnected)
{
    if (!mIsRunning)
        return;
    bool theAllConnected = true;
    SmartPtr<ListItem> theChanged;
    for (int i = 0; i < mConnectList->GetNumItems(); i++) {
        ConnectListItem* theItem = (ConnectListItem*)mConnectList->GetItem(i);
        if (theItem->mSlot == inSlot) {
            theItem->mImage = (inConnected ? ::GameUserListCtrl_Ready_Image : ::ServerListCtrl_PingFailed_Image).mPtr;
            theChanged = theItem;
        }
        if (theItem->mImage.mPtr != ::GameUserListCtrl_Ready_Image.mPtr)
            theAllConnected = false;
    }
    if (theChanged.mPtr != NULL)
        mConnectList->Fn2FC730(theChanged.mPtr);
    if (theAllConnected)
        End(true);
}

// The unit's compiler-generated functions:
// SYNTHETIC: EXE 0x6909c7
// (Container::VirtualB0, inline)
// SYNTHETIC: EXE 0x6909da
// (??_G EELobbyConnectDialog)
// SYNTHETIC: EXE 0x690a26
// (Dialog's implicit dtor)
// SYNTHETIC: EXE 0x690bb4
// (ListItem::Virtual14, inline)
// SYNTHETIC: EXE 0x690bba
// (ListItem::Virtual18, inline)
// SYNTHETIC: EXE 0x690bc4
// (ListItem::Virtual1C, inline)
// SYNTHETIC: EXE 0x690be2
// (TextListItem's inline ctor)
// SYNTHETIC: EXE 0x690c34
// (??_G TextListItem; ICF-shared with ConnectListItem's)
// SYNTHETIC: EXE 0x690c50
// (TextListItem's inline dtor)
// SYNTHETIC: EXE 0x690d70
// (ListArea::GetNumItems, inline)
// TEMPLATE: EXE 0x690d89
// (SafeGetComponent<ListArea>)
// TEMPLATE: EXE 0x690e09
// (SafeGetComponent<Button>)
