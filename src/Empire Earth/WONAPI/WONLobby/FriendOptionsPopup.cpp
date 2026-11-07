// FriendOptionsPopup source-intent candidate. No new PopupMenu construction or allocation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// FUNCTION: WON 0x7f7c50
FriendOptionsPopup::~FriendOptionsPopup() {}

void FriendOptionsPopup::AddItem(const GUIString& inText,int inAction)
{
    mPopupMenu->AddItem(inText,NULL,NULL,inAction);
}
// FUNCTION: WON 0x7f7cf0
void FriendOptionsPopup::Init(LobbyFriend* inFriend,int inRoomSpec)
{
    if(inFriend==NULL) return;
    mFriend=inFriend;
    mRoomSpecFlags=inRoomSpec;
    mPopupMenu->Fn2FAA50(false);
    SmartPtr<Font> anOldFont=mPopupMenu->mFont.get();
    SmartPtr<Font> aBoldFont=Fn2F5A30(anOldFont.get(),1,0);
    mPopupMenu->SetFont(aBoldFont);
    mPopupMenu->AddItem(inFriend->mName,0)->mB09=false;
    mPopupMenu->SetFont(anOldFont);
    mPopupMenu->AddSeperator();
    AddItem(::FriendOptionsCtrl_Locate_String,1);
    AddItem(::FriendOptionsCtrl_RefreshList_String,2);
    mPopupMenu->AddSeperator();
    AddItem(::FriendOptionsCtrl_RemoveFromFriends_String,3);
    Fn2F4B90(mPopupMenu->Width(),mPopupMenu->Height());
}
// FUNCTION: WON 0x7f7ea0
void FriendOptionsPopup::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType!=24) {
        Container::HandleComponentEvent(inEvent);
        return;
    }
    ListItemClickedEvent* anEvent=static_cast<ListItemClickedEvent*>(inEvent);
    if(anEvent->mItem.get()==NULL || anEvent->mButton!=0) return;
    int anAction=static_cast<MenuItem*>(anEvent->mItem.get())->mMenuId;
    if(anAction!=0) Fn2F53F0(new FriendActionEvent(anAction,mFriend.get(),mRoomSpecFlags),false);
    GetRoot()->ClosePopup(this);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007f7b60
FriendOptionsPopup::FriendOptionsPopup()
{
    mPopupMenu=new PopupMenu;
    Fn2F6A60(mPopupMenu.get());
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f7c30
// (protected: virtual void * __thiscall WONAPI::FriendOptionsPopup::`scalar deleting destructor'(unsigned int)    [??_GFriendOptionsPopup@WONAPI@@MAEPAXI)
