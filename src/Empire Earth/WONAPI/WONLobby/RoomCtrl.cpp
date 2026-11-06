
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <string>
#include <list>
#include <map>
#include <vector>
#include <stdio.h>
#include <windows.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
using namespace WONAPI;
// Scratch RoomCtrl's private global list-item family. External base construction/lifetime actual.
// FUNCTION: WON 0x007c19a0
RoomListItem::RoomListItem(WONAPI::LobbyServer* inServer,bool inLoading)
    :mColor(-1),mLoading(inLoading),mAddress(inServer->mAddress),mGroupId(0xffff)
{
    Fn3C1D40(inServer,NULL);
}
// FUNCTION: WON 0x007c1c50
RoomListItem::RoomListItem(WONAPI::LobbyGroup* inGroup,WONAPI::LobbyServer* inServer)
    :mColor(-1),mLoading(false),mAddress(inServer->mAddress),mGroupId(inGroup->mId)
{
    Fn3C1D40(inServer,inGroup);
}
// FUNCTION: WON 0x007c20a0
int RoomListItem::Virtual10(const WONAPI::ListItem* inOther,WONAPI::ListArea*) const
{
    return mText.CompareNoCase(static_cast<const RoomListItem*>(inOther)->mText);
}
// Scratch full RoomListItem update, no member ownership shortcuts.
// FUNCTION: WON 0x007c1d40
void RoomListItem::Fn3C1D40(WONAPI::LobbyServer* inServer,WONAPI::LobbyGroup* inGroup)
{
    char aCount[50];
    if(mLoading) mText=::RoomCtrl_QueryingServer_String;
    else if(inGroup!=NULL)
    {
        sprintf(aCount," (%d)",inGroup->mNumClients);
        mText=inGroup->mName;
        if(static_cast<int>(inGroup->mNumClients)>0)
            mText.append(WONAPI::GUIString(new WONAPI::GUIStringData(aCount)));
    }
    else if(inServer!=NULL)
    {
        sprintf(aCount," (%d)",inServer->Fn3AA410());
        mText=inServer->mName;
        if(inServer->Fn3AA410()!=0)
            mText.append(WONAPI::GUIString(new WONAPI::GUIStringData(aCount)));
    }
    else mText=WONAPI::GetEmptyGUIStringData();
    if(inGroup!=NULL && inGroup->Fn3A9020()) mImage=::RoomCtrl_Password_Image;
    else mImage=NULL;
}
// FUNCTION: WON 0x007c1fd0
void RoomListItem::Virtual08(WONAPI::ListArea* inList)
{
    if(mFont.get()==NULL)
    {
        if(mGroupId==0xffff && !mLoading)
            mFont=inList->Fn2F5A30(inList->mFont.get(),1,0);
        else mFont=inList->mFont;
    }
    if(mColor<0) mColor=inList->GetTextColor();
    mL10=mFont->GetStringWidth(mText,0,mText.length())+2;
    mHeight=mFont->GetHeight();
    if(mImage.get()!=NULL)
    {
        mL10+=mImage->mWidth+2;
        if(mImage->mHeight>mHeight) mHeight=mImage->mHeight;
    }
}
// Requires GUI-owned typed SelectionColor atListArea140/listFlags124.
// FUNCTION: WON 0x007c20c0
void RoomListItem::Virtual04(WONAPI::Graphics& g,WONAPI::ListArea* inList)
{
    int aColor=mColor;
    if(mB08)
    {
        g.SetColor(inList->GetSelColor().GetBackgroundColor(g));
        int aWidth=inList->mL12C;
        if(!inList->ListFlagSet(0x10)) aWidth=mL10;
        g.FillRect(0,0,aWidth,mHeight);
        aColor=inList->GetSelColor().GetForegroundColor(g,mColor);
    }
    int x=2;
    if(mImage.get()!=NULL)
    {
        g.DrawImage(mImage.get(),x,(mHeight-mImage->mHeight)/2);
        x=mImage->mWidth+4;
    }
    g.SetColor(aColor);
    g.SetFont(mFont.get());
    g.DrawString(mText,x,(mHeight-mFont->GetHeight())/2);
}


namespace WONAPI {
// Scratch whole RoomCtrl source-intent. No new partial widget or fake payload allocation.
RoomCtrl::RoomCtrl() {}
// FUNCTION: WON 0x007c22a0
void RoomCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig,mRoomList,"RoomList",__FILE__,168);
    SafeGetComponent(aConfig,mJoinButton,"JoinButton",__FILE__,169);
    SafeGetComponent(aConfig,mCreateButton,"CreateButton",__FILE__,170);
    Fn2F5360(1);
    EnableControls();
}
// FUNCTION: WON 0x007c2320
void RoomCtrl::EnableControls()
{
    mJoinButton->SetEnabled(mRoomList->mSelItem!=NULL);
}
// FUNCTION: WON 0x007c2350
void RoomCtrl::AddGroup(TreeItem* inParent,LobbyServer* inServer,LobbyGroup* inGroup)
{
    RoomListItem* aItem=new RoomListItem(inGroup,inServer);
    mRoomList->Fn315080(inParent,aItem,-1);
}
// FUNCTION: WON 0x007c23d0
void RoomCtrl::AddServer(LobbyServer* inServer,bool inOpen,int inPos)
{
    mRoomList->Fn2FAA00();
    RoomListItem* aItem=new RoomListItem(inServer,false);
    TreeItem* aServer=mRoomList->Fn315080(NULL,aItem,inPos);
    for(LobbyGroupMap::iterator it=inServer->mGroups.begin();it!=inServer->mGroups.end();++it)
    {
        LobbyGroup* aGroup=it->second.get();
        if(aGroup->Fn3A9050()) AddGroup(aServer,inServer,aGroup);
    }
    if(!inServer->Fn3AA430() && inServer->mGroups.empty())
    {
        mRoomList->Fn3151B0(aServer);
        RoomListItem* aLoading=new RoomListItem(inServer,true);
        TreeItem* aLoadingTree=mRoomList->Fn315080(aServer,aLoading,0);
        aLoadingTree->mB09=false;
    }
    if(inOpen) mRoomList->Fn315220(aServer,true);
    mRoomList->Fn2FAA10();
    EnableControls();
}
// FUNCTION: WON 0x007c2b10
TreeItem* RoomCtrl::GetServerItem(const IPAddr& inAddress)
{
    for(int i=0;i<mRoomList->GetNumItems();++i)
    {
        TreeItem* aTree=static_cast<TreeItem*>(mRoomList->GetItem(i));
        RoomListItem* aItem=static_cast<RoomListItem*>(aTree->mItem.get());
        if(aItem->mAddress==inAddress) return aTree;
    }
    return NULL;
}
// FUNCTION: WON 0x007c2b70
TreeItem* RoomCtrl::GetGroupItem(const IPAddr& inAddress,int inGroupId)
{
    TreeItem* aServer=GetServerItem(inAddress);
    if(aServer!=NULL)
    {
        for(int i=0;i<static_cast<int>(aServer->mChildren.size());++i)
        {
            TreeItem* aChild=aServer->Fn314440(i);
            if(static_cast<RoomListItem*>(aChild->mItem.get())->mGroupId==static_cast<unsigned long>(inGroupId)) return aChild;
        }
    }
    return NULL;
}
// FUNCTION: WON 0x007c2bc0
TreeItem* RoomCtrl::GetServerItem(LobbyServer* inServer)
{
    if(inServer!=NULL) return GetServerItem(inServer->mAddress);
    return NULL;
}
// FUNCTION: WON 0x007c2be0
TreeItem* RoomCtrl::GetGroupItem(LobbyServer* inServer,LobbyGroup* inGroup)
{
    if(inServer!=NULL && inGroup!=NULL) return GetGroupItem(inServer->mAddress,inGroup->GetId());
    return NULL;
}

// FUNCTION: WON 0x007c2630
void RoomCtrl::DoJoinRoom()
{
    TreeItem* aSelected=static_cast<TreeItem*>(mRoomList->mSelItem);
    if(aSelected!=NULL)
    {
        RoomListItem* aItem=static_cast<RoomListItem*>(aSelected->mItem.get());
        Fn2F53F0(new JoinRoomEvent(aItem->mAddress,aItem->mGroupId),false);
    }
}
// FUNCTION: WON 0x007c28f0
void RoomCtrl::DoCreateRoom()
{
    IPAddr aAddress;
    TreeItem* aSelected=static_cast<TreeItem*>(mRoomList->mSelItem);
    if(aSelected!=NULL) aAddress=static_cast<RoomListItem*>(aSelected->mItem.get())->mAddress;
    Fn2F53F0(new CreateRoomEvent(aAddress),false);
}
// FUNCTION: WON 0x007c2550
void RoomCtrl::HandleSetRoomList()
{
    LobbyServerList* aServers=LobbyFn35B6B0();
    if(aServers==NULL) return;
    mRoomList->Fn2FAA00();
    mRoomList->Fn3151A0(true);
    for(LobbyServerMap::iterator it=aServers->mServers.begin();it!=aServers->mServers.end();++it)
        AddServer(it->second.get(),false,-1);
    mRoomList->Fn2FAA10();
    LobbyGroup* aGroup=LobbyFn35B6D0();
    LobbyServer* aServer=LobbyFn35B6F0();
    if(aServer!=NULL)
    {
        TreeItem* aItem=GetServerItem(aServer);
        if(aItem!=NULL)
        {
            mRoomList->Fn315220(aItem,false);
            if(aGroup!=NULL)
            {
                aItem=GetGroupItem(aServer,aGroup);
                if(aItem==NULL) { EnableControls(); return; }
            }
            ListArea* aList=mRoomList.get();
            aList->SetSelItem(aList->GetItemPos(aItem),true);
            mRoomList->EnsureVisible(aItem);
        }
    }
    EnableControls();
}
// FUNCTION: WON 0x007c2e20
void RoomCtrl::HandleGroupChanged(GroupChangedEvent* inEvent)
{
    LobbyGroup* aGroup=inEvent->mGroup.get();
    LobbyServer* aServer=inEvent->mServer.get();
    if(aGroup==NULL || aServer==NULL || !aGroup->Fn3A9050()) return;
    if(inEvent->IsAdd())
    {
        TreeItem* aServerItem=GetServerItem(aServer);
        if(aServerItem!=NULL) AddGroup(aServerItem,aServer,aGroup);
        EnableControls();
        return;
    }
    if(inEvent->IsDelete())
    {
        TreeItem* aGroupItem=GetGroupItem(aServer,aGroup);
        mRoomList->Fn315190(aGroupItem);
        EnableControls();
        return;
    }
    TreeItem* aGroupItem=GetGroupItem(aServer,aGroup);
    if(aGroupItem!=NULL)
    {
        static_cast<RoomListItem*>(aGroupItem->mItem.get())->Fn3C1D40(aServer,aGroup);
        static_cast<RoomListItem*>(aGroupItem->mParent->mItem.get())->Fn3C1D40(aServer,NULL);
        mRoomList->Fn2FC730(aGroupItem->mParent);
        mRoomList->Fn2FC730(aGroupItem);
    }
    EnableControls();
}
// FUNCTION: WON 0x007c2f00
void RoomCtrl::HandleItemOpened(TreeItemOpenedEvent* inEvent)
{
    TreeItem* aTree=inEvent->mItem.get();
    if(aTree->mOpen)
    {
        RoomListItem* aItem=static_cast<RoomListItem*>(aTree->mItem.get());
        Fn2F53F0(new QueryServerGroupsEvent(aItem->mAddress),false);
    }
}
// FUNCTION: WON 0x007c2c10
void RoomCtrl::HandleServerChanged(ServerChangedEvent* inEvent)
{
    if(inEvent->IsAdd())
    {
        AddServer(inEvent->mServer.get(),false,-1);
        return;
    }
    if(inEvent->IsDelete())
    {
        TreeItem* aServerItem=GetServerItem(inEvent->mServer.get());
        if(aServerItem!=NULL) mRoomList->Fn315190(aServerItem);
        EnableControls();
        return;
    }
    TreeItem* aServerItem=GetServerItem(inEvent->mServer.get());
    if(aServerItem==NULL) return;
    bool aOpen=aServerItem->mOpen;
    TreeItem* aSelected=static_cast<TreeItem*>(mRoomList->mSelItem);
    unsigned long aGroupId=0xffff;
    IPAddr aAddress;
    if(aSelected!=NULL && aSelected->mParent==aServerItem)
    {
        RoomListItem* aItem=static_cast<RoomListItem*>(aSelected->mItem.get());
        aGroupId=aItem->mGroupId;
        aAddress=aItem->mAddress;
    }
    int aPosition=aServerItem->mIndex;
    int aVertOffset=mRoomList->mVertOffset;
    mRoomList->Fn2FAA00();
    mRoomList->Fn315190(aServerItem);
    AddServer(inEvent->mServer.get(),aOpen,aPosition);
    if(aGroupId!=0xffff && aAddress.IsValid())
    {
        TreeItem* aItem=GetGroupItem(aAddress,aGroupId);
        if(aItem!=NULL)
        {
            ListArea* aList=mRoomList.get();
            aList->SetSelItem(aList->GetItemPos(aItem),true);
        }
    }
    mRoomList->Fn2FAA10();
    mRoomList->SetVertOffset(aVertOffset);
    EnableControls();
}
// FUNCTION: WON 0x007c31b0
void RoomCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType)
    {
    case 26:
        HandleItemOpened(static_cast<TreeItemOpenedEvent*>(inEvent));
        return;
    case 23:
        {
            EnableControls();
            ListItemClickedEvent* aEvent=static_cast<ListItemClickedEvent*>(inEvent);
            if(!aEvent->mDoubleClick || aEvent->mComponent!=mRoomList.get() || !aEvent->IsSelectedItem()) return;
            TreeItem* aItem=static_cast<TreeItem*>(aEvent->mItem.get());
            if(aItem->Fn314E60(aEvent->mX,aEvent->mY)) DoJoinRoom();
        }
        return;
    case 15:
        if(inEvent->mComponent==mJoinButton.get()) { DoJoinRoom(); return; }
        if(inEvent->mComponent==mCreateButton.get()) { DoCreateRoom(); return; }
        break;
    case 100021:
        HandleGroupChanged(static_cast<GroupChangedEvent*>(inEvent));
        return;
    case 100047:
        HandleServerChanged(static_cast<ServerChangedEvent*>(inEvent));
        return;
    case 100053:
        HandleSetRoomList();
        return;
    }
    Component::HandleComponentEvent(inEvent);
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c1b70
// (public: virtual __thiscall RoomListItem::~RoomListItem(void)    [??1RoomListItem@@UAE@XZ])
// TEMPLATE: WON 0x007c2190
// (public: __thiscall WONAPI::RoomCtrl::RoomCtrl(void)    [??0RoomCtrl@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007ce350

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c21c0
// (public: virtual void * __thiscall WONAPI::RoomCtrl::`scalar deleting destructor'(unsigned int)    [??_GRoomCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c21e0
// (public: virtual __thiscall WONAPI::RoomCtrl::~RoomCtrl(void)    [??1RoomCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c2840
// (public: virtual void * __thiscall WONAPI::JoinRoomEvent::`scalar deleting destructor'(unsigned int)    [??_GJoinRoomEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c2860
// (public: virtual __thiscall WONAPI::JoinRoomEvent::~JoinRoomEvent(void)    [??1JoinRoomEvent@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c2a60
// (public: virtual void * __thiscall WONAPI::CreateRoomEvent::`scalar deleting destructor'(unsigned int)    [??_GCreateRoomEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c2a80
// (public: virtual __thiscall WONAPI::CreateRoomEvent::~CreateRoomEvent(void)    [??1CreateRoomEvent@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c3100
// (public: virtual void * __thiscall WONAPI::QueryServerGroupsEvent::`scalar deleting destructor'(unsigned int)    [??_GQueryServerGroupsEvent@WONAPI@@UA)
// TEMPLATE: WON 0x007c3120
// (public: virtual __thiscall WONAPI::QueryServerGroupsEvent::~QueryServerGroupsEvent(void)    [??1QueryServerGroupsEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c1b50
// (public: virtual void * __thiscall RoomListItem::`scalar deleting destructor'(unsigned int)    [??_GRoomListItem@@UAEPAXI@Z])
