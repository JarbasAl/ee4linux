// Reviewed original-backed controls; source-only, near code/support remain carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SCRATCH ONLY. Full TextListItem/Image/ListArea contracts and source gate pending.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
//3bb000,119B;defaultTextListItem constructor actual20font/24image.
// FUNCTION: WON 0x007bb000
FriendsListItem::FriendsListItem() { mSeparator=true; mSortGroup=0; }
//3bb0a0,209B. Friend28 release then actualTextListItem destructor.
// Implicit destructor support is generated from the actual class/vtable.
//3bb180..3bb1b3,51B, missed by oldinventory.
// FUNCTION: WON 0x007bb180
void FriendsListItem::Virtual08(ListArea* inList)
{
    if(mFriend.get()==NULL) {
        mL10=1;
        if(mSeparator) mHeight=8;
        else mHeight=1;
    } else TextListItem::Virtual08(inList);
}
//3bb1c0..3bb1e0,32B.
// FUNCTION: WON 0x007bb1c0
int FriendsListItem::Virtual10(const ListItem* inOther,ListArea* inList) const
{
    int result=static_cast<const FriendsListItem*>(inOther)->mSortGroup-mSortGroup;
    if(result!=0) return result;
    return TextListItem::Virtual10(inOther,inList);
}
//3bb1e0..3bb241,97B.
// FUNCTION: WON 0x007bb1e0
void FriendsListItem::Virtual04(Graphics& inGraphics,ListArea* inList)
{
    if(mFriend.get()==NULL) {
        if(mSeparator) {
            int width=inList->mL0A4+10;
            if(inList->mL12C>width) width=inList->mL12C;
            int y=mHeight>>2;
            inGraphics.SetSysColor(14);
            inGraphics.DrawLine(0,y,width,y);
        }
    } else TextListItem::Virtual04(inGraphics,inList);
}
//3bb250,44B.
FriendsListCtrl::FriendsListCtrl() {}
//3bb2a0,221B;fourmemberguards thenContainer.
// Implicit destructor support is generated from the actual class/vtable.
//3bb380..3bb61d,669B. Properallocationguard before FriendsListItem construction.
// FUNCTION: WON 0x007bb380
void FriendsListCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mFriendsList,"FriendsList",__FILE__,109);
        SafeGetComponent(config,mRemoveButton,"RemoveButton",__FILE__,110);
        SafeGetComponent(config,mLocateButton,"LocateButton",__FILE__,111);
        mSeparator=new FriendsListItem;
        mSeparator->mSortGroup=1;
        mSeparator->mB09=false;
        Fn2F5360(1);
        UpdateButtons();
    }
}
//3bb620..3bb697,119B.
// FUNCTION: WON 0x007bb620
void FriendsListCtrl::UpdateSeparator()
{
    mFriendsList->Fn2FC730(mSeparator.get());
    int pos=mFriendsList->GetItemPos(mSeparator.get());
    bool show=pos>0&&pos<mFriendsList->GetNumItems()-1;
    if(mSeparator->mSeparator!=show) {
        mSeparator->mSeparator=show;
        mFriendsList->Fn2FC730(mSeparator.get());
    }
}
//3bb6a0..3bb809,361B. AllactualImageglobals requestedGUI canonicalrenaming.
// FUNCTION: WON 0x007bb6a0
void FriendsListCtrl::UpdateFriend(LobbyFriend* inFriend,FriendsListItem* inItem)
{
    inItem->mFriend=inFriend;
    if((inFriend->GetFlags()&3)!=0&&!inFriend->GetAnonymous()) inItem->mSortGroup=2;
    else inItem->mSortGroup=0;
    Image* anImage;
    if((inFriend->GetFlags()&4)!=0) anImage=::LobbyGlobal_Searching_Image;
    else if(!inFriend->GetAnonymous()) {
        if((inFriend->GetFlags()&2)!=0) anImage=::FriendsListCtrl_InGame_Image;
        else if((inFriend->GetFlags()&1)!=0) anImage=::FriendsListCtrl_InChat_Image;
        else anImage=::FriendsListCtrl_Unknown_Image;
    } else anImage=::FriendsListCtrl_Unknown_Image;
    inItem->mImage=anImage;
    inItem->mText=inFriend->GetName();
    if((inFriend->GetFlags()&3)!=0&&!inFriend->GetAnonymous()) {
        inItem->mL1C=0x8000000e;
        inItem->mFont=Fn2F5B20();
    } else {
        inItem->mL1C=0x80000006;
        inItem->mFont=Fn2F5B50(2,0);
    }
}
//3bb810..3bb860,80B.
// FUNCTION: WON 0x007bb810
FriendsListItem* FriendsListCtrl::FindFriend(LobbyFriend* inFriend)
{
    for(int i=0;i<mFriendsList->GetNumItems();i++) {
        FriendsListItem* item=static_cast<FriendsListItem*>(mFriendsList->GetItem(i));
        if(item->mFriend.get()!=NULL&&item->mFriend->mUserId==inFriend->mUserId)
            return item;
    }
    return NULL;
}
//3bb860..3bb877,23B.
// FUNCTION: WON 0x007bb860
LobbyFriend* FriendsListCtrl::GetSelectedFriend()
{
    FriendsListItem* item=static_cast<FriendsListItem*>(mFriendsList->mSelItem);
    if(item!=NULL) return item->mFriend.get();
    return NULL;
}
//3bb880..3bb99b,283B. Eventkind0add/1remove/elsechange.
// FUNCTION: WON 0x007bb880
void FriendsListCtrl::HandleFriendChanged(FriendChangedEvent* inEvent)
{
    if(inEvent->IsAdd()) {
        SmartPtr<FriendsListItem> item=new FriendsListItem;
        UpdateFriend(inEvent->mFriend.get(),item.get());
        mFriendsList->InsertItem(item.get(),-1);
    } else if(inEvent->IsDelete()) {
        FriendsListItem* item=FindFriend(inEvent->mFriend.get());
        if(item==NULL) return;
        mFriendsList->RemoveItem(item);
    } else {
        FriendsListItem* item=FindFriend(inEvent->mFriend.get());
        if(item==NULL) return;
        UpdateFriend(inEvent->mFriend.get(),item);
        mFriendsList->Fn2FC730(item);
    }
    UpdateButtons();
    UpdateSeparator();
}
//3bb9a0..3bba0d,109B.
// FUNCTION: WON 0x007bb9a0
void FriendsListCtrl::RefreshFriends()
{
    mFriendsList->Fn2FAA00();
    for(int i=0;i<mFriendsList->GetNumItems();i++) {
        FriendsListItem* item=static_cast<FriendsListItem*>(mFriendsList->GetItem(i));
        if(item!=NULL&&item->mFriend.get()!=NULL) UpdateFriend(item->mFriend.get(),item);
    }
    mFriendsList->Fn2FAA10();
    UpdateButtons();
    UpdateSeparator();
}
//3bba10..3bbb38,296B. Exact ULONG/friend map getter confirmed by core.
// FUNCTION: WON 0x007bba10
void FriendsListCtrl::FillFriends() {
    mFriendsList->Fn2FAA00();
    mFriendsList->Fn2FAA50(true);
    mFriendsList->InsertItem(mSeparator.get(),-1);
    const LobbyFriendMap& friends=LobbyFn35D270();
    for(LobbyFriendMap::const_iterator i=friends.begin();i!=friends.end();++i) {
        SmartPtr<FriendsListItem> item=new FriendsListItem;
        UpdateFriend(i->second.get(),item.get());
        mFriendsList->InsertItem(item.get(),-1);
    }
    mFriendsList->Fn2FAA10();
    UpdateButtons();
    UpdateSeparator();
}
//3bbb40..3bbb8f,79B.
// FUNCTION: WON 0x007bbb40
void FriendsListCtrl::UpdateButtons()
{
    if(mFriendsList->mSelItem==NULL) {
        mLocateButton->SetEnabled(false);
        mRemoveButton->SetEnabled(false);
    } else {
        mLocateButton->SetEnabled(true);
        mRemoveButton->SetEnabled(true);
    }
}
//3bbb90..3bbdb8,552B. Four independent newevent allocationguards required.
// FUNCTION: WON 0x007bbb90
void FriendsListCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 15:
        if(inEvent->mComponent==mRemoveButton.get()) {
            LobbyFriend* aFriend=GetSelectedFriend();
            if(aFriend!=NULL) Fn2F53F0(new FriendActionEvent(3,aFriend,0),false);
            return;
        } else if(inEvent->mComponent==mLocateButton.get()) {
            LobbyFriend* aFriend=GetSelectedFriend();
            if(aFriend!=NULL) Fn2F53F0(new FriendActionEvent(1,aFriend,0),false);
            return;
        }
        break;
    case 22:UpdateButtons();return;
    case 23: {
        FriendsListItem* item=static_cast<FriendsListItem*>(mFriendsList->mSelItem);
        ListItemClickedEvent* click=static_cast<ListItemClickedEvent*>(inEvent);
        if(item!=NULL&&item==click->mItem.get()&&item->mFriend.get()!=NULL) {
            if(click->GetButton()==2) {
                Fn2F53F0(new FriendActionEvent(5,item->mFriend.get(),0),false);
                return;
            }
            if(click->GetButton()==0&&click->IsDoubleClick())
                Fn2F53F0(new FriendActionEvent(1,item->mFriend.get(),0),false);
        }
        return;
    }
    case 100014:FillFriends();return;
    case 100016:HandleFriendChanged(static_cast<FriendChangedEvent*>(inEvent));return;
    case 100056:RefreshFriends();return;
    }
    Component::HandleComponentEvent(inEvent);
}
}

/* Rejected empty explicit destructor candidates; zero handwritten credit.
FriendsListItem::~FriendsListItem() {}
FriendsListCtrl::~FriendsListCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bb2a0
// (public: virtual __thiscall WONAPI::FriendsListCtrl::~FriendsListCtrl(void)    [??1FriendsListCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bb080
// (public: virtual void * __thiscall WONAPI::FriendsListItem::`scalar deleting destructor'(unsigned int)    [??_GFriendsListItem@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bb0a0
// (public: virtual __thiscall WONAPI::FriendsListItem::~FriendsListItem(void)    [??1FriendsListItem@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007bb280
// (public: virtual void * __thiscall WONAPI::FriendsListCtrl::`scalar deleting destructor'(unsigned int)    [??_GFriendsListCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bb250
// (public: __thiscall WONAPI::FriendsListCtrl::FriendsListCtrl(void)    [??0FriendsListCtrl@WONAPI@@QAE@XZ])
