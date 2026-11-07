// New R3 GameUser whole source family; actual ordinary-object review pending.
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
#include "LostDebug.h"
namespace WONAPI {
// Original3bcad0..3bcb0f,63B. Native new24 proved in both event/refresh loops.
// FUNCTION: WON 0x007bcad0
GameUserListItem::GameUserListItem(LobbyClient* inClient) : mClient(inClient) {}
// Original3bcbe0..3bcdaa,458B. Preserve predicate order and member reloads.
// FUNCTION: WON 0x007bcbe0
void GameUserListItem::UpdateIcon()
{
    if(mClient->GetB22()) { mImage=::UserListCtrl_Ignored_Image; return; }
    if(mClient->GetB23()) { mImage=::UserListCtrl_Blocked_Image; return; }
    if(mClient->Fn3A5480(true)) { mImage=::UserListCtrl_Muted_Image; return; }
    if(mClient->Fn3A54B0()) { mImage=::UserListCtrl_Away_Image; return; }
    if(mClient->Fn3A54C0()) { mImage=::UserListCtrl_Admin_Image; return; }
    if(mClient->Fn3A54D0()) { mImage=::UserListCtrl_Moderator_Image; return; }
    if(mClient->Fn3A54E0(1)) { mImage=::UserListCtrl_Captain_Image; return; }
    mImage=::UserListCtrl_Normal_Image;
}
// Original3bcdb0..3bce07,87B. Font ownership changes before metric reads.
// FUNCTION: WON 0x007bcdb0
void GameUserListItem::Virtual08(ListArea* inArea)
{
    mFont=inArea->mFont;
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(2);
    mHeight=mFont->GetHeight();
    if(mImage.get()!=NULL && mImage->mHeight>mHeight) mHeight=mImage->mHeight;
}
// Original3bce10..3bce5f,79B. Full bool-normalization then reversed rank.
// FUNCTION: WON 0x007bce10
int GameUserListItem::Virtual10(const ListItem* inOther,ListArea*) const
{
    const GameUserListItem* other=static_cast<const GameUserListItem*>(inOther);
    int mine=mClient->Fn3A54E0(1)?1:0;
    int theirs=other->mClient->Fn3A54E0(1)?1:0;
    if(mine!=theirs) return theirs-mine;
    return mClient->mName.CompareNoCase(other->mClient->mName);
}
// Original3bce60..3bcfac,332B. Owned5-entry jump table20B excluded.
// Canonical GUI D field/API spellings; all getters read proved original offsets.
// FUNCTION: WON 0x007bce60
void GameUserListItem::Virtual04(Graphics& inGraphics,ListArea* inArea)
{
    MultiListArea* area=static_cast<MultiListArea*>(inArea);
    int column=area->GetColumnPaintContext();
    unsigned long color=inGraphics.Fn326D20(14);
    if(mB08) color=area->GetSelColor().GetForegroundColor(inGraphics,color);
    inGraphics.SetColor(color);
    inGraphics.SetFont(mFont.get());
    switch(column) {
    case -1:
        if(mB08) {
            inGraphics.SetColor(area->GetSelColor().GetBackgroundColor(inGraphics));
            inGraphics.FillRect(0,0,area->mL12C,mHeight);
        }
        return;
    case 0:
        inGraphics.DrawImage(mImage.get(),0,(mHeight-mImage->mHeight)/2);
        return;
    case 1:
        if(mClient->Fn3A5590())
            inGraphics.DrawImage(::GameUserListCtrl_Ready_Image.get(),0,(mHeight-mImage->mHeight)/2);
        return;
    case 2:
        inGraphics.SetFont(mFont.get());
        inGraphics.DrawString(mClient->mName,0,(mHeight-mFont->GetHeight())/2);
        return;
    case 3:
        DrawLobbyPing(&inGraphics,mClient->Fn3A55A0(),mHeight-2,0,0);
        return;
    }
}

// FUNCTION: WON 0x007bcfc0
GameUserListCtrl::GameUserListCtrl() : mInGame(false) { mRoomSpecFlags = 2; }
// Original3bd2e0..3bd2f5,21B.
// FUNCTION: WON 0x007bd2e0
LobbyClient* GameUserListCtrl::GetSelectedClient()
{
    GameUserListItem* item=static_cast<GameUserListItem*>(mUserList->mSelItem);
    return item==NULL?NULL:item->mClient.get();
}
// Original3bd300..3bd350,80B. Compare client IDs; current list each iteration.
// FUNCTION: WON 0x007bd300
GameUserListItem* GameUserListCtrl::GetListItem(LobbyClient* inClient)
{
    for(int i=0;i<int(mUserList->mItems.size());++i) {
        GameUserListItem* item=static_cast<GameUserListItem*>(mUserList->GetItem(i));
        LobbyClient* client=item->mClient.get();
        if(client!=NULL && client->mClientId==inClient->mClientId) return item;
    }
    return NULL;
}
// Original3bd350..3bd36a,26B.
// FUNCTION: WON 0x007bd350
void GameUserListCtrl::HandleKickPlayer(bool inBan)
{
    LobbyClient* client=GetSelectedClient();
    if(client!=NULL) LobbyFn361BC0(client,inBan);
}
// Original3bd370..3bd3e8,120B. No host/selection ownership pin in original.
// FUNCTION: WON 0x007bd370
void GameUserListCtrl::UpdateKickButton()
{
    if(!mInGame || mKickButton.get()==NULL) return;
    LobbyGame* game=GetMyLobbyGame();
    if(game==NULL) return;
    if(game->mIsHost) {
        LobbyClient* client=GetSelectedClient();
        if(client!=NULL && client->mClientId!=game->mL60) mKickButton->SetEnabled(true);
        else mKickButton->SetEnabled(false);
    } else mKickButton->SetEnabled(false);
}
// Original3bd3f0..3bd44c,92B.
// FUNCTION: WON 0x007bd3f0
void GameUserListCtrl::HandleClientChanged(ClientChangedEvent* inEvent)
{
    if(!mInGame) return;
    if((mRoomSpecFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
    if(!inEvent->Fn359470()) return;
    GameUserListItem* item=GetListItem(inEvent->mClient.get());
    if(item==NULL) return;
    item->UpdateIcon();
    mUserList->Fn2FC730(item);
    UpdateKickButton();
}
// Original3bd450..3bd587,311B. New item construction reads event after rawnew.
// FUNCTION: WON 0x007bd450
void GameUserListCtrl::HandlePlayerChanged(PlayerChangedEvent* inEvent)
{
    if(!mInGame) return;
    if(inEvent->IsAdd()) {
        SmartPtr<GameUserListItem> item=new GameUserListItem(inEvent->mClient.get());
        item->UpdateIcon();
        mUserList->ListArea::InsertItem(item.get(),-1);
    } else if(inEvent->IsDelete()) {
        GameUserListItem* item=GetListItem(inEvent->mClient.get());
        if(item==NULL) return;
        mUserList->RemoveItem(item);
    } else {
        GameUserListItem* item=GetListItem(inEvent->mClient.get());
        if(item==NULL) return;
        item->UpdateIcon();
        mUserList->Fn2FC730(item);
    }
}
// Original3bd710..3bd72f,31B.
// FUNCTION: WON 0x007bd710
void GameUserListCtrl::HandleLeftGame()
{
    if(mInGame) {
        mInGame=false;
        mUserList->Fn2FAA50(true);
    }
}
// Original3bd0b0..3bd2de,558B. Optional KickButton, type2 configuration only.
// FUNCTION: WON 0x007bd0b0
void GameUserListCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mUserList,"UserList",__FILE__,159);
        mKickButton=dynamic_cast<Button*>(config->Fn386860("KickButton"));
        if(mKickButton.get()!=NULL) mKickButton->SetEnabled(false);
        Fn2F5360(1);
    }
}
// Original3bd590..3bd706,374B. Borrow client map after room/game tests.
// FUNCTION: WON 0x007bd590
void GameUserListCtrl::HandleJoinedGame()
{
    LobbyGame* game=GetMyLobbyGame();
    if(game==NULL || !game->Fn3A6D40(mRoomSpecFlags)) return;
    LobbyClientList* clients=game->mClients.get();
    if(clients==NULL) return;
    if(mKickButton.get()!=NULL) {
        if(mRoomSpecFlags&4) mKickButton->SetText(::GameUserListCtrl_Ban_String,true);
        else mKickButton->SetText(::GameUserListCtrl_Kick_String,true);
    }
    mUserList->Fn2FAA00();
    mUserList->Fn2FAA50(true);
    for(LobbyClientMap::iterator i=clients->mClients.begin();i!=clients->mClients.end();++i) {
        LobbyClient* client=i->second.get();
        if(client->Fn3A5580()) {
            SmartPtr<GameUserListItem> item=new GameUserListItem(client);
            item->UpdateIcon();
            mUserList->ListArea::InsertItem(item.get(),-1);
        }
    }
    mUserList->Fn2FAA10();
    mInGame=true;
    UpdateKickButton();
}
// Original3bd730..3bd9ba,650B. No jump table; rawnew event guards separate.
// FUNCTION: WON 0x007bd730
void GameUserListCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 22: UpdateKickButton(); return;
    case 15:
        if(inEvent->mComponent==mKickButton.get()) {
            HandleKickPlayer((mRoomSpecFlags&4)!=0);
            return;
        }
        break;
    case 23: {
        LobbyClient* client=GetSelectedClient();
        if(client==NULL) return;
        ListItemClickedEvent* clicked=static_cast<ListItemClickedEvent*>(inEvent);
        if(!clicked->IsSelectedItem()) return;
        if(clicked->mButton==2)
            Fn2F53F0(new ClientActionEvent(18,GetRoomSpecFlags(),client),false);
        else if(clicked->mButton==0 && clicked->mDoubleClick)
            LobbyFn359100(new SetChatInputEvent(2,client->mName,mRoomSpecFlags,false),true);
        return;
    }
    case 100006: HandleClientChanged(static_cast<ClientChangedEvent*>(inEvent)); return;
    case 100029: HandleJoinedGame(); return;
    case 100032: HandleLeftGame(); return;
    case 100038: HandlePlayerChanged(static_cast<PlayerChangedEvent*>(inEvent)); return;
    case 100054: mRoomSpecFlags|=static_cast<SetRoomSpecFlagsEvent*>(inEvent)->mFlags; return;
    }
    Component::HandleComponentEvent(inEvent);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007654c0
ComponentConfig* GameUserListCtrlConfig::CfgFactory()
{
    return new GameUserListCtrlConfig;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007bda70
bool GameUserListCtrlConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="CTRLLINKID") { }
    else return ContainerConfig::Virtual24(instruction);
    return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bcb30
// (public: virtual __thiscall WONAPI::GameUserListItem::~GameUserListItem(void)    [??1GameUserListItem@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007bd9e0
// (public: virtual __thiscall WONAPI::SetChatInputEvent::~SetChatInputEvent(void)    [??1SetChatInputEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bcb10
// (public: virtual void * __thiscall WONAPI::GameUserListItem::`scalar deleting destructor'(unsigned int)    [??_GGameUserListItem@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bcff0
// (public: virtual void * __thiscall WONAPI::GameUserListCtrl::`scalar deleting destructor'(unsigned int)    [??_GGameUserListCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bd010
// (public: virtual __thiscall WONAPI::GameUserListCtrl::~GameUserListCtrl(void)    [??1GameUserListCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007bd9c0
// (public: virtual void * __thiscall WONAPI::SetChatInputEvent::`scalar deleting destructor'(unsigned int)    [??_GSetChatInputEvent@WONAPI@@UAEPAXI@Z])
