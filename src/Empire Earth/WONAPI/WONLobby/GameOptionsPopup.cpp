// GameOptionsPopup behavior candidate; existing complete8c, native constructor remains external.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// FUNCTION: WON 0x7f8050
GameOptionsPopup::~GameOptionsPopup() {}
// Native3f80f0/28 wrapper, zero quota.
// FUNCTION: WON 0x007f80f0
void GameOptionsPopup::AddItem(const GUIString& inText,int inAction)
{
    mPopupMenu->AddItem(inText,NULL,NULL,inAction);
}
// FUNCTION: WON 0x7f8110
void GameOptionsPopup::Init(LobbyGame* inGame)
{
    mPopupMenu->Fn2FAA50(false);
    mGame=inGame;
    SmartPtr<Font> anOldFont=mPopupMenu->mFont;
    SmartPtr<Font> aBoldFont=mPopupMenu->Fn2F5A30(anOldFont.get(),1,0);
    mPopupMenu->mFont=aBoldFont;
    mPopupMenu->AddItem(inGame->mName,0)->mB09=false;
    mPopupMenu->mFont=anOldFont;
    mPopupMenu->AddSeperator();
    if(inGame->Fn3A7A20()) AddItem(::GameOptionsPopup_Join_String,1);
    bool allowPing=true;
    if(inGame->mB13) {
        LobbyConfig* aConfig=GetLobbyConfig();
        if(aConfig!=NULL) allowPing=!aConfig->mNoPingInProgress;
    }
    if(allowPing) {
        if(inGame->mL2C!=1) AddItem(::GameOptionsPopup_Ping_String,2);
        if(allowPing) AddItem(::GameOptionsPopup_QueryDetails_String,3);
    }
    if(inGame->mB16) AddItem(::GameOptionsPopup_HideDetails_String,5);
    else AddItem(::GameOptionsPopup_ShowDetails_String,4);
    if(inGame->mL2C==3) {
        mPopupMenu->AddSeperator();
        AddItem(::GameOptionsPopup_Remove_String,6);
    }
    Fn2F4B90(mPopupMenu->Width(),mPopupMenu->Height());
}
// FUNCTION: WON 0x7f82f0
void GameOptionsPopup::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType!=24) {
        Container::HandleComponentEvent(inEvent);
        return;
    }
    ListItemClickedEvent* anEvent=static_cast<ListItemClickedEvent*>(inEvent);
    if(anEvent->mItem.get()==NULL || anEvent->mButton!=0) return;
    int anAction=static_cast<MenuItem*>(anEvent->mItem.get())->mMenuId;
    if(anAction!=0) Fn2F53F0(new GameActionEvent(anAction,mGame.get()),false);
    GetRoot()->ClosePopup(this);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007f7f60
GameOptionsPopup::GameOptionsPopup()
{
    mPopupMenu=new PopupMenu;
    Fn2F6A60(mPopupMenu.get());
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f8030
// (protected: virtual void * __thiscall WONAPI::GameOptionsPopup::`scalar deleting destructor'(unsigned int)    [??_GGameOptionsPopup@WONAPI@@MAEPAXI@Z])
