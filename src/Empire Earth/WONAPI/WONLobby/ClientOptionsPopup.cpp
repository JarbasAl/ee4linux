// ClientOptionsPopup behavior candidate; native constructor and PopupMenu allocation remain external.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
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
// FUNCTION: WON 0x7f5e00
ClientOptionsPopup::~ClientOptionsPopup() {}
// Native3f5ea0/28 forwarding support0.
// FUNCTION: WON 0x007f5ea0
void ClientOptionsPopup::AddItem(const GUIString& inText,int inAction)
{
    mPopupMenu->AddItem(inText,NULL,NULL,inAction);
}
// FUNCTION: WON 0x7f5ec0
void ClientOptionsPopup::Init(LobbyClient* inClient,LobbyClient* inMyClient,int inRoomSpec)
{
    if(inClient==NULL || inMyClient==NULL) return;
    mClient=inClient;
    mRoomSpecFlags=inRoomSpec;
    mPopupMenu->Fn2FAA50(false);
    bool isLAN=(inRoomSpec&2)!=0;
    bool isGame=(inRoomSpec&4)!=0;
    bool isModerator=inMyClient->Fn3A54D0();
    bool isCaptain=inMyClient->Fn3A54E0(isLAN);
    bool isSelf=inMyClient->mClientId==inClient->GetClientId();
    SmartPtr<Font> anOldFont=mPopupMenu->mFont;
    SmartPtr<Font> aBoldFont=Fn2F5A30(anOldFont.get(),1,0);
    mPopupMenu->mFont=aBoldFont;
    mPopupMenu->AddItem(inClient->mName,0)->mB09=false;
    if(isModerator) {
        char aBuffer[256];
        sprintf(aBuffer,"%d",inClient->GetU14());
        MenuItem* anItem=mPopupMenu->AddItem(GUIString(aBuffer),0);
        anItem->mB09=false;
    }
    mPopupMenu->mFont=anOldFont;
    AddItem(::ClientOptionsCtrl_Whisper_String,16);
    if(!isSelf && inMyClient->Fn3A54E0(1) && inClient->GetGroupId20()!=inMyClient->GetGroupId20()) {
        if(!mClient->mB27) AddItem(::ClientOptionsCtrl_Invite_String,6);
        else AddItem(::ClientOptionsCtrl_Uninvite_String,13);
    }
    if(isGame) {
        if(!isSelf) AddItem(::FriendOptionsCtrl_Locate_String,17);
        // Native shared physical getter2b31d0 reads receiver bool24; no call through unrelated IPAddr API.
        if(!inClient->mB24) AddItem(::ClientOptionsCtrl_AddToFriends_String,1);
        else AddItem(::ClientOptionsCtrl_RemoveFromFriends_String,9);
        mPopupMenu->AddSeperator();
    }
    if(isSelf && isGame) {
        if(inClient->Fn3A54B0()) AddItem(::ClientOptionsCtrl_ClearAFK_String,4);
        else AddItem(::ClientOptionsCtrl_SetAFK_String,10);
    } else {
        if(!inClient->mB22) AddItem(::ClientOptionsCtrl_Ignore_String,5);
        else AddItem(::ClientOptionsCtrl_Unignore_String,12);
        if(isGame) {
            if(!inClient->mB23) AddItem(::ClientOptionsCtrl_Block_String,3);
            else AddItem(::ClientOptionsCtrl_Unblock_String,11);
        }
    }
    if(isGame) {
        if(isModerator || isCaptain) {
            mPopupMenu->AddSeperator();
            if(isModerator) AddItem(::ClientOptionsCtrl_Warn_String,15);
            if(inClient->Fn3A5480(isLAN)) AddItem(::ClientOptionsCtrl_UnMute_String,14);
            else AddItem(::ClientOptionsCtrl_Mute_String,8);
            if(!isSelf) {
                AddItem(::ClientOptionsCtrl_Kick_String,7);
                AddItem(::ClientOptionsCtrl_Ban_String,2);
            }
        }
    } else if(isCaptain && !isSelf) {
        mPopupMenu->AddSeperator();
        AddItem(::ClientOptionsCtrl_Kick_String,7);
    }
    Fn2F4B90(mPopupMenu->Width(),mPopupMenu->Height());
}
// FUNCTION: WON 0x7f6310
void ClientOptionsPopup::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType!=24) {
        Container::HandleComponentEvent(inEvent);
        return;
    }
    ListItemClickedEvent* anEvent=static_cast<ListItemClickedEvent*>(inEvent);
    if(anEvent->mItem.get()==NULL || anEvent->mButton!=0) return;
    int anAction=static_cast<MenuItem*>(anEvent->mItem.get())->mMenuId;
    if(anAction!=0) Fn2F53F0(new ClientActionEvent(anAction,int(mRoomSpecFlags),mClient.get()),false);
    GetRoot()->ClosePopup(this);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007f5d10
ClientOptionsPopup::ClientOptionsPopup()
{
    mPopupMenu=new PopupMenu;
    Fn2F6A60(mPopupMenu.get());
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f5de0
// (protected: virtual void * __thiscall WONAPI::ClientOptionsPopup::`scalar deleting destructor'(unsigned int)    [??_GClientOptionsPopup@WONAPI@@MAEPAXI)
