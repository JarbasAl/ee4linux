// R3 reviewed Item/Ctrl source; actual normal/EH/data admission pending.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
// Original3ca6a0..3ca717,119B. Full30 typed item construction.
// FUNCTION: WON 0x007ca6a0
UserListItem::UserListItem() { mSortKey=0; mShowSeparator=true; }
// Original3ca820..3ca840,32B including terminalRET8 only (verify boundary).
// FUNCTION: WON 0x007ca820
int UserListItem::Virtual10(const ListItem* inOther,ListArea* inArea) const
{
    const UserListItem* other=static_cast<const UserListItem*>(inOther);
    int result=other->mSortKey-mSortKey;
    if(result!=0) return result;
    return TextListItem::Virtual10(inOther,inArea);
}
// Original3ca840..3ca873,51B.
// FUNCTION: WON 0x007ca840
void UserListItem::Virtual08(ListArea* inArea)
{
    if(mClient.get()==NULL) {
        mL10=1;
        if(mShowSeparator) mHeight=8;
        else mHeight=1;
    } else TextListItem::Virtual08(inArea);
}
// Original3ca880..3ca8e1,97B. Canonical SetSysColor is actual326d40.
// FUNCTION: WON 0x007ca880
void UserListItem::Virtual04(Graphics& inGraphics,ListArea* inArea)
{
    if(mClient.get()==NULL) {
        if(mShowSeparator) {
            int width=inArea->mL0A4+10;
            if(inArea->mL12C>width) width=inArea->mL12C;
            int y=mHeight>>2;
            inGraphics.SetSysColor(14);
            inGraphics.DrawLine(0,y,width,y);
        }
    } else TextListItem::Virtual04(inGraphics,inArea);
}
// Original3ca8f0..3ca932,66B; flags5 real initialization, source-kind gate separate.
// FUNCTION: WON 0x007ca8f0
UserListCtrl::UserListCtrl() { mUpdating = false; mModeFlags = 5; }
// Original3ca960..3caa68,264B, own-vptr store3ca97f plus five typed owners.
// Admitted-normal-destruction candidate, not hand-authorship or implicit support.
// FUNCTION: WON 0x007ca960
UserListCtrl::~UserListCtrl() {}
// Original3cac50..3cace7,151B. Resource owner producer proofs required.
// FUNCTION: WON 0x007cac50
Image* GetLobbyClientIcon(LobbyClient* inClient,bool inMode)
{
    if(inClient==NULL) return NULL;
    if(inClient->mB22) return ::UserListCtrl_Ignored_Image.get();
    if(inClient->mB23) return ::UserListCtrl_Blocked_Image.get();
    if(inClient->Fn3A5480(inMode)) return ::UserListCtrl_Muted_Image.get();
    if(inClient->Fn3A54B0()) return ::UserListCtrl_Away_Image.get();
    if(inClient->Fn3A54C0()) return ::UserListCtrl_Admin_Image.get();
    if(inClient->Fn3A54D0()) return ::UserListCtrl_Moderator_Image.get();
    return inClient->Fn3A5470()?::UserListCtrl_Captain_Image.get() : ::UserListCtrl_Normal_Image.get();
}
// Original3caa70..3cac49,473B. Raw incomingclient is not additionally pinned.
// FUNCTION: WON 0x007caa70
void UserListCtrl::UpdateItem(LobbyClient* inClient,UserListItem* inItem)
{
    inItem->mClient=inClient;
    unsigned char sortKey;
    if(inClient->Fn3A54C0()) sortKey=5;
    else if(inClient->Fn3A54D0()) sortKey=4;
    else if(inClient->IsFriend() && !inClient->Fn3A5470()) sortKey=2;
    else sortKey=0;
    inItem->mSortKey=sortKey;
    inItem->mText=inClient->mName;
    inItem->mImage=GetLobbyClientIcon(inClient,(mModeFlags&2)!=0);
    if(inClient->IsFriend() && !inClient->Fn3A5470()) inItem->mFont=Fn2F5B50(1,0);
    else inItem->mFont=Fn2F5B20();
}
// Original3cacf0..3cade2,242B. Current member receivers reload at each external call.
// FUNCTION: WON 0x007cacf0
void UserListCtrl::RefreshSeparators()
{
    mUserList->Fn2FC730(mFirstSeparator.get());
    mUserList->Fn2FC730(mSecondSeparator.get());
    int first=mUserList->GetItemPos(mFirstSeparator.get());
    int second=mUserList->GetItemPos(mSecondSeparator.get());
    bool before=first>0;
    bool middle=second>first+1;
    bool after=second<mUserList->GetNumItems()-1;
    bool showFirst=before&&(middle||after);
    bool showSecond=middle&&after;
    if(mFirstSeparator->mShowSeparator!=showFirst) {
        mFirstSeparator->mShowSeparator=showFirst;
        mUserList->Fn2FC730(mFirstSeparator.get());
    }
    if(mSecondSeparator->mShowSeparator!=showSecond) {
        mSecondSeparator->mShowSeparator=showSecond;
        mUserList->Fn2FC730(mSecondSeparator.get());
    }
}
// Original3cadf0..3cb0dd,749B. Both30-byte allocations use complete UserListItem.
// FUNCTION: WON 0x007cadf0
void UserListCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(config,mUserList,"UserList",__FILE__,194);
    SafeGetComponent(config,mWhisperButton,"WhisperButton",__FILE__,195);
    SafeGetComponent(config,mIgnoreButton,"IgnoreButton",__FILE__,196);
    Fn2F5360(1);
    mFirstSeparator=new UserListItem;
    mFirstSeparator->mB09=false;
    mFirstSeparator->mSortKey=3;
    mSecondSeparator=new UserListItem;
    mSecondSeparator->mB09=false;
    mSecondSeparator->mSortKey=1;
    UpdateButtons();
}
// Original3cb0e0..3cb130,80B. ID comparison, not object identity.
// FUNCTION: WON 0x007cb0e0
UserListItem* UserListCtrl::FindItem(LobbyClient* inClient)
{
    for(int i=0;i<mUserList->GetNumItems();++i) {
        UserListItem* item=static_cast<UserListItem*>(mUserList->GetItem(i));
        if(item->mClient.get()!=NULL && item->mClient->mClientId==inClient->mClientId)
            return item;
    }
    return NULL;
}
// Original3cb130..3cb13d,13B and3cb140..3cb155,21B. Borrowed selection.
// SUPPORT0: EXE 0x007cb130
// FUNCTION: WON 0x007cb130
UserListItem* UserListCtrl::GetSelectedItem()
{
    return static_cast<UserListItem*>(mUserList->mSelItem);
}
// SUPPORT0: EXE 0x007cb140
// FUNCTION: WON 0x007cb140
LobbyClient* UserListCtrl::GetSelectedClient()
{
    UserListItem* item=static_cast<UserListItem*>(mUserList->mSelItem);
    return item==NULL?NULL:item->mClient.get();
}
// Original3cb160..3cb1f6,150B. Current button receiver reloaded after each call.
// FUNCTION: WON 0x007cb160
void UserListCtrl::UpdateButtons()
{
    LobbyClient* client=GetSelectedClient();
    if(client==NULL) {
        mIgnoreButton->SetText(::UserListCtrl_Ignore_String,true);
        mIgnoreButton->SetEnabled(false);
        mWhisperButton->SetEnabled(false);
    } else {
        mWhisperButton->SetEnabled(true);
        mIgnoreButton->SetEnabled(true);
        if(client->mB22) mIgnoreButton->SetText(::UserListCtrl_Unignore_String,true);
        else mIgnoreButton->SetText(::UserListCtrl_Ignore_String,true);
    }
}
// Original3cb200..3cb354,340B. Item releases before incrementing borrowed map.
// FUNCTION: WON 0x007cb200
void UserListCtrl::JoinedRoom(JoinedRoomEvent* inEvent)
{
    if((mModeFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
    mUpdating=true;
    mUserList->Fn2FAA00();
    mUserList->Fn2FAA50(true);
    mUserList->InsertItem(mFirstSeparator.get(),-1);
    mUserList->InsertItem(mSecondSeparator.get(),-1);
    LobbyClientList* clients=inEvent->mClients.get();
    for(LobbyClientMap::iterator i=clients->mClients.begin();i!=clients->mClients.end();++i) {
        LobbyClient* client=i->second.get();
        SmartPtr<UserListItem> item=new UserListItem;
        UpdateItem(client,item.get());
        mUserList->InsertItem(item.get(),-1);
    }
    mUserList->Fn2FAA10();
    UpdateButtons();
    RefreshSeparators();
}
// Original3cb360..3cb395,53B.
// FUNCTION: WON 0x007cb360
void UserListCtrl::LeftRoom(LeftRoomEvent* inEvent)
{
    if((mModeFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
    mUpdating=false;
    mUserList->Fn2FAA50(true);
    UpdateButtons();
}
// Original3cb3a0..3cb4ce,302B. Creation/release block ends before UI refresh.
// FUNCTION: WON 0x007cb3a0
void UserListCtrl::ClientChanged(ClientChangedEvent* inEvent)
{
    if((mModeFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
    if(inEvent->IsAdd()) {
        SmartPtr<UserListItem> item=new UserListItem;
        LobbyClient* client=inEvent->mClient.get();
        UpdateItem(client,item.get());
        mUserList->InsertItem(item.get(),-1);
    } else if(inEvent->IsDelete()) {
        UserListItem* item=FindItem(inEvent->mClient.get());
        if(item==NULL) return;
        mUserList->RemoveItem(item);
    } else {
        UserListItem* item=FindItem(inEvent->mClient.get());
        if(item==NULL) return;
        UpdateItem(inEvent->mClient.get(),item);
        mUserList->Fn2FC730(item);
    }
    UpdateButtons();
    RefreshSeparators();
}
// Original missing scan start3cb4d0..3cb761,657B. Four allocation guards, full events.
// FUNCTION: WON 0x007cb4d0
void UserListCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 100006: ClientChanged(static_cast<ClientChangedEvent*>(inEvent)); return;
    case 100030: JoinedRoom(static_cast<JoinedRoomEvent*>(inEvent)); return;
    case 100033: LeftRoom(static_cast<LeftRoomEvent*>(inEvent)); return;
    case 22: UpdateButtons(); return;
    case 23: {
        LobbyClient* client=GetSelectedClient();
        if(client==NULL) return;
        ListItemClickedEvent* clicked=static_cast<ListItemClickedEvent*>(inEvent);
        ListItem* item=clicked->mItem.get();
        if(GetSelectedItem()!=item) return;
        if(clicked->mButton==2)
            Fn2F53F0(new ClientActionEvent(18,int(mModeFlags),client),false);
        else if(clicked->mButton==0 && clicked->mDoubleClick)
            LobbyFn359100(new SetChatInputEvent(2,client->mName,mModeFlags,false),true);
        return;
    }
    case 15: {
        Component* component=inEvent->mComponent;
        if(component==mIgnoreButton.get()) {
            LobbyClient* client=GetSelectedClient();
            if(client!=NULL)
                Fn2F53F0(new ClientActionEvent(client->mB22?12:5,int(mModeFlags),client),false);
        } else if(component==mWhisperButton.get()) {
            LobbyClient* client=GetSelectedClient();
            if(client!=NULL)
                LobbyFn359100(new SetChatInputEvent(2,client->mName,mModeFlags,false),true);
        }
        return;
    }
    default: Component::HandleComponentEvent(inEvent); return;
    }
}
}
// Private UserListCtrlConfig family is separate and not yet drafted here.
// No quota/admission until peer, actual normal/EH/data and Root closure.

namespace WONAPI {
// FUNCTION: WON 0x00765a60
ComponentConfig* UserListCtrlConfig::CfgFactory()
{
    return new UserListCtrlConfig;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007cb770
bool UserListCtrlConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="GROUPTYPE") { }
    else if(instruction=="CTRLLINKID") { }
    else return ContainerConfig::Virtual24(instruction);
    return true;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007ca940
// (public: virtual void * __thiscall WONAPI::UserListCtrl::`scalar deleting destructor'(unsigned int)    [??_GUserListCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007ca720
// (public: virtual void * __thiscall WONAPI::UserListItem::`scalar deleting destructor'(unsigned int)    [??_GUserListItem@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007ca740
// (public: virtual __thiscall WONAPI::UserListItem::~UserListItem(void)    [??1UserListItem@WONAPI@@UAE@XZ])
