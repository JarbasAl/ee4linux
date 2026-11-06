// Whole native ServerListCtrl.cpp main/controller/details source candidate.
// Private ServerListCtrlConfig follows in a separately reviewed source append; all support remains0.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/EELobbyGameCode.h"
using namespace WONAPI;
// Global original private RTTI .?AVFilterListItem@@, full38; no anonymous/fake namespace.
// TextListItem28/14slots, no appended slots, implicitnormal3c6970/243 support0.
class FilterListItem : public WONAPI::TextListItem {
public:
    FilterListItem(const WONAPI::GUIString& inColumn,WONAPI::Font* inFont)
        :WONAPI::TextListItem(WONAPI::gEmptyGUIString,inFont),mIsAdd(false),mSelPos(0),mColumnName(inColumn) {}
    bool mIsAdd; //28
    int mSelPos; //2c
    WONAPI::GUIString mFilterText; //30
    WONAPI::GUIString mColumnName; //34
};

namespace WONAPI {
// Scratch complete ServerDetails method bodies; shared actual28/16-slot migration remains gated.
// All callback third args are real cdecl controller context; unused where original ignores it.
// FUNCTION: WON 0x007c4c80
ServerDetailsItem::ServerDetailsItem() { mB09=true; }
// FUNCTION: WON 0x007c4d10
ServerDetailsItem::~ServerDetailsItem() {}
// FUNCTION: WON 0x007c4da0
void ServerDetailsItem::SetGame(LobbyGame* inGame)
{
    mGame=inGame;
    if(mGame->mL2C==3)
    {
        const std::string& aAddress=mGame->mAddress.GetHostAndPortString(true);
        mIPAddrStr=GUIString(new GUIStringData(aAddress));
    }
}
// FUNCTION: WON 0x007c5800
void ServerDetailsItem::CalcIcon()
{
    if(mGame->mB13) mIconId=0x176;
    else if(mGame->mB11)
    {
        bool aOwned=mGame->mB14 || mGame->mIsHost;
        if(aOwned) mIconId=0x171;
        else mIconId=0x175;
    }
    else if(mGame->mB12) mIconId=0x173;
    else if(mGame->mB10) mIconId=0x174;
    else mIconId=0x172;
}
// FUNCTION: WON 0x007c5080
int ServerDetailsItem::CompareIcon(const ServerDetailsItem* a,const ServerDetailsItem* b,ServerListCtrl*)
{ return a->mIconId-b->mIconId; }
// FUNCTION: WON 0x007c50a0
int ServerDetailsItem::CompareSkill(const ServerDetailsItem* a,const ServerDetailsItem* b,ServerListCtrl*)
{ return a->mGame->mL30-b->mGame->mL30; }
int ServerDetailsItem::CompareGameName(const ServerDetailsItem* a,const ServerDetailsItem* b,ServerListCtrl*)
{ return a->mGame->mName.CompareNoCase(b->mGame->mName); }
int ServerDetailsItem::CompareAddress(const ServerDetailsItem* a,const ServerDetailsItem* b,ServerListCtrl*)
{ return a->mGame->mAddress.StringCompare(b->mGame->mAddress); }
// FUNCTION: WON 0x007c5100
int ServerDetailsItem::CompareNumPlayers(const ServerDetailsItem* a,const ServerDetailsItem* b,ServerListCtrl*)
{
    int result=a->mGame->mL24-b->mGame->mL24;
    if(result==0) result=b->mGame->mNumOpenSlots-a->mGame->mNumOpenSlots;
    return result;
}
// FUNCTION: WON 0x007c5130
int ServerDetailsItem::ComparePing(const ServerDetailsItem* a,const ServerDetailsItem* b,ServerListCtrl*)
{ return a->mGame->mL64-b->mGame->mL64; }
// FUNCTION: WON 0x007c51e0
bool ServerDetailsItem::IsStartOfNumber(char c)
{
    return ((static_cast<unsigned int>(static_cast<int>(c))<256 && isdigit(c)) || c=='-' || c=='+') ? 1 : 0;
}
// FUNCTION: WON 0x007c5220
bool ServerDetailsItem::ParseIntFilter(const GUIString& inFilter,int& outLower,int& outUpper)
{
    std::string aFilter=static_cast<std::string>(inFilter);
    char aBuffer[512];
    strncpy(aBuffer,aFilter.c_str(),510);
    aBuffer[511]=0;
    bool aParsed=false;
    for(char* aToken=strtok(aBuffer," ");aToken!=NULL;aToken=strtok(NULL," "))
    {
        if(aToken[0]=='<')
        {
            if(aToken[1]=='=' && IsStartOfNumber(aToken[2])) { outUpper=atoi(aToken+2);aParsed=true; }
            else if(IsStartOfNumber(aToken[1])) { aParsed=true;outUpper=atoi(aToken+1)-1; }
        }
        else if(aToken[0]=='>')
        {
            if(aToken[1]=='=' && IsStartOfNumber(aToken[2])) { aParsed=true;outLower=atoi(aToken+2); }
            else if(IsStartOfNumber(aToken[1])) { aParsed=true;outLower=atoi(aToken+1)+1; }
        }
        else if(aToken[0]=='=')
        {
            if(IsStartOfNumber(aToken[1])) { outUpper=atoi(aToken+1);outLower=outUpper;aParsed=true; }
        }
        else if(IsStartOfNumber(aToken[0])) { outUpper=atoi(aToken);outLower=outUpper;aParsed=true; }
    }
    return aParsed;
}
// FUNCTION: WON 0x007c5460
void ServerDetailsItem::FilterSkill(const GUIString& inFilter,ServerDetailsList& ioList,ServerListCtrl*)
{
    int aLower=-100000,aUpper=100000;
    if(!ParseIntFilter(inFilter,aLower,aUpper)) return;
    for(ServerDetailsList::iterator it=ioList.begin();it!=ioList.end();)
    {
        long aSkill=(*it)->mGame->mL30;
        if(aSkill<aLower || aSkill>aUpper) ioList.erase(it++);
        else ++it;
    }
}
// FUNCTION: WON 0x007c54f0
void ServerDetailsItem::FilterGameName(const GUIString& inFilter,ServerDetailsList& ioList,ServerListCtrl*)
{
    for(ServerDetailsList::iterator it=ioList.begin();it!=ioList.end();)
    {
        if((*it)->mGame->mName.findNoCase(inFilter,0)>=0) ++it;
        else ioList.erase(it++);
    }
}
// FUNCTION: WON 0x007c5560
void ServerDetailsItem::FilterAddress(const GUIString& inFilter,ServerDetailsList& ioList,ServerListCtrl*)
{
    for(ServerDetailsList::iterator it=ioList.begin();it!=ioList.end();)
    {
        if((*it)->mIPAddrStr.findNoCase(inFilter,0)>=0) ++it;
        else ioList.erase(it++);
    }
}
// FUNCTION: WON 0x007c5750
void ServerDetailsItem::FilterPing(const GUIString& inFilter,ServerDetailsList& ioList,ServerListCtrl*)
{
    int aLower=-100000,aUpper=100000;
    if(!ParseIntFilter(inFilter,aLower,aUpper)) return;
    for(ServerDetailsList::iterator it=ioList.begin();it!=ioList.end();)
    {
        int aPing=10-(*it)->mGame->mL64/100;
        if(aPing<aLower || aPing>aUpper) ioList.erase(it++);
        else ++it;
    }
}
// Actual3c4f00/123: summary height intentionally truncates to a byte at each store.
// FUNCTION: WON 0x007c4f00
void ServerDetailsItem::Virtual08(ListArea* inList)
{
    Font* aFont=inList->Fn2F5B20();
    CalcIcon();
    mSummaryHeight=static_cast<unsigned char>(aFont->GetHeight());
    unsigned short imageHeight=::ServerListCtrl_GameExpanded_Image->mHeight;
    if(imageHeight>mSummaryHeight)
        mSummaryHeight=static_cast<unsigned char>(imageHeight);
    mSummaryHeight+=2;
    mHeight=mSummaryHeight;
    if(mGame->mB16)
    {
        int aHeight=GetDetailsHeightHook(static_cast<MultiListArea*>(inList));
        if(aHeight<aFont->GetHeight()) aHeight=aFont->GetHeight();
        mHeight+=aHeight;
    }
}
// FUNCTION: WON 0x007c4f80
bool ServerDetailsItem::Virtual14(ListArea* inList,int x,int y)
{
    return (mB09 && x>static_cast<MultiListArea*>(inList)->GetColumnWidth(0) && y<=mSummaryHeight) ? 1 : 0;
}
// FUNCTION: WON 0x007c4fc0
void ServerDetailsItem::Virtual30(ListArea* inList,int x,int y,int inButton)
{
    if(inButton!=0) return;
    if(x>static_cast<MultiListArea*>(inList)->GetColumnWidth(0) || y>mSummaryHeight) return;
    mGame->mB16=!mGame->mB16;
    inList->Fn2FC730(this);
    inList->EnsureVisible(this);
    if(mGame->mB16)
        inList->Fn2F53F0(new PingGameEvent(mGame.get(),true),true);
}
// Native cdecl3c58b0/43, real helper body; vertical integer division truncates toward zero.
// FUNCTION: WON 0x007c58b0
static void DrawServerImage(Graphics& g,Image* inImage,ServerDetailsItem* inItem,int x)
{
    g.DrawImage(inImage,x,(inItem->mSummaryHeight-inImage->mHeight)/2);
}
// FUNCTION: WON 0x007c5860
void ServerDetailsItem::PaintIcon(Graphics& g,ServerDetailsItem* inItem,ServerListCtrl*)
{
    if(inItem->mGame->mB16)
        DrawServerImage(g,::ServerListCtrl_GameExpanded_Image.get(),inItem,15);
    else
        DrawServerImage(g,::ServerListCtrl_GameNotExpanded_Image.get(),inItem,15);
    DrawServerImage(g,::LobbyResource_GetImage((LobbyResourceId)(inItem->mIconId)),inItem,0);
}
// FUNCTION: WON 0x007c58e0
void ServerDetailsItem::PaintSkill(Graphics& g,ServerDetailsItem* inItem,ServerListCtrl*)
{
    unsigned char aSkill=static_cast<unsigned char>(inItem->mGame->mL30);
    if(aSkill<5) DrawServerImage(g,::LobbyResource_GetImage((LobbyResourceId)(aSkill+0x177)),inItem,0);
}
void ServerDetailsItem::PaintGameName(Graphics& g,ServerDetailsItem* inItem,ServerListCtrl* inCtrl)
{
    g.SetFont(inCtrl->mBoldFont.get());
    g.DrawString(inItem->mGame->mName,5,0);
}
void ServerDetailsItem::PaintAddress(Graphics& g,ServerDetailsItem* inItem,ServerListCtrl*)
{
    g.DrawString(inItem->mIPAddrStr,0,0);
}
// FUNCTION: WON 0x007c55c0
void ServerDetailsItem::FilterNumPlayers(const GUIString& inFilter,ServerDetailsList& ioList,ServerListCtrl*)
{
    int aPos=inFilter.findNoCase(::ServerListCtrl_NotFullFilter_String,0);
    bool aNotFull=aPos==0 || (aPos>0 && inFilter[aPos-1]==' ');
    aPos=inFilter.findNoCase(::ServerListCtrl_FullFilter_String,0);
    bool aFull=aPos==0 || (aPos>0 && inFilter[aPos-1]==' ');
    aPos=inFilter.findNoCase(::ServerListCtrl_OpenFilter_String,0);
    bool aOpen=aPos==0 || (aPos>0 && inFilter[aPos-1]==' ');
    if(aFull && aNotFull) { ioList.clear();return; }
    int aLower=-100000,aUpper=100000;
    if(!ParseIntFilter(inFilter,aLower,aUpper) && !aFull && !aNotFull && !aOpen) return;
    for(ServerDetailsList::iterator it=ioList.begin();it!=ioList.end();)
    {
        LobbyGame* aGame=(*it)->mGame.get();
        int aCount=aGame->mL24;
        int aMax=aGame->mNumOpenSlots;
        if(aMax==0) aMax=0x7ffffff;
        if(aCount<aLower || aCount>aUpper || (aFull && aCount<aMax) ||
            (aNotFull && aCount>=aMax) || (aOpen && !aGame->Fn3A7A20())) ioList.erase(it++);
        else ++it;
    }
}
// FUNCTION: WON 0x007c5970
void ServerDetailsItem::PaintNumPlayers(Graphics& g,ServerDetailsItem* inItem,ServerListCtrl* inCtrl)
{
    char aBuffer[50];
    if(inItem->mGame->mNumOpenSlots>0) sprintf(aBuffer,"%d/%d",inItem->mGame->mL24,inItem->mGame->mNumOpenSlots);
    else sprintf(aBuffer,"%d",inItem->mGame->mL24);
    GUIString aText(new GUIStringData(aBuffer));
    int aWidth=inCtrl->mList->mL12C;
    int aTextWidth=g.mFont->GetStringWidth(aText,0,aText.length());
    g.DrawString(aText,(aWidth-aTextWidth)/2,0);
}
// FUNCTION: WON 0x007c5ac0
void ServerDetailsItem::Virtual04(Graphics& g,ListArea* inList)
{
    MultiListArea* aList=static_cast<MultiListArea*>(inList);
    int aColumn=aList->GetColumnPaintContext();
    ServerListCtrl* aCtrl=static_cast<ServerListCtrl*>(aList->mParent->mParent);
    unsigned long aColor=g.Fn326D20(mGame->GetB13()?6:14);
    if(mB08) aColor=aList->GetSelColor().GetForegroundColor(g,aColor);
    g.SetColor(aColor);
    g.SetFont(aList->mFont.get());
    if(aColumn==-1)
    {
        if(mB08)
        {
            g.SetColor(aList->GetSelColor().GetBackgroundColor(g));
            g.FillRect(0,0,aList->mL12C,mSummaryHeight);
        }
        if(!mGame->mB16) return;
        int aWidth=aList->GetColumnWidth(0);
        g.Translate(aWidth,mSummaryHeight);
        if(mGame->mB17) PaintDetailsHook(g,aList);
        else
        {
            g.SetFont(aList->mFont.get());
            g.SetSysColor(14);
            g.DrawString(mGame->Fn3A6D90()?::ServerListCtrl_Querying_String : ::ServerListCtrl_QueryFailed_String,0,0);
        }
        g.Translate(-aWidth,-mSummaryHeight);
        return;
    }
    ServerListCtrl::ColumnInfo::PaintFunc aPaint=aCtrl->mColumnInfo[aColumn].mPaintFunc;
    if(aPaint!=NULL) aPaint(g,this,aCtrl);
}
// FUNCTION: WON 0x007c5150
int ServerDetailsItem::Virtual10(const ListItem* inOther,ListArea* inList) const
{
    const ServerDetailsItem* anOther=static_cast<const ServerDetailsItem*>(inOther);
    if(this==anOther) return 0;
    MultiListArea* aList=static_cast<MultiListArea*>(inList);
    ServerListCtrl* aCtrl=static_cast<ServerListCtrl*>(aList->mParent->mParent);
    int aSecondaryColumn=aList->mSortColumnPrev; //actual184, shared reference migration pending
    ServerListCtrl::ColumnInfo::CompareFunc aCompare=aCtrl->mColumnInfo[aList->GetSortColumn()].mCompareFunc;
    int aResult;
    if(aCompare!=NULL)
    {
        aResult=aCompare(this,anOther,aCtrl);
        if(aResult!=0) return aResult;
    }
    aCompare=aCtrl->mColumnInfo[aSecondaryColumn].mCompareFunc;
    if(aCompare!=NULL)
    {
        aResult=aCompare(this,anOther,aCtrl);
        if(aResult!=0) return aResult;
    }
    return static_cast<const ListItem*>(this)-inOther;
}
// FUNCTION: WON 0x007c5a90
void ServerDetailsItem::PaintPing(Graphics& g,ServerDetailsItem* inItem,ServerListCtrl*)
{
    DrawLobbyPing(&g,inItem->mGame->mL64,inItem->mSummaryHeight,0,0);
}
// FUNCTION: WON 0x007c6340
void ServerListCtrl::AddDefaultFilters(const GUIString& inColumn,const GUIString& inStrings,bool inAddIfExists)
{
    LobbyAddGameOption(inColumn,inStrings,inAddIfExists);
}

// Scratch native ServerListCtrl family. No canonical markers/compile authority.
ServerListCtrl::ColumnMap ServerListCtrl::mColumnMap;
// FUNCTION: WON 0x007cc4b0
// FUNCTION: WON 0x007c5f80
ServerListCtrl::ColumnInfo::ColumnInfo()
{
    mPaintFunc=NULL;
    mCompareFunc=NULL;
    mFilterFunc=NULL;
    mGameTypeFlags=0xff;
}
ServerListCtrl::ServerListCtrl()
{
    mGameType=0;
    mNumColumns=4;
    mFilterTimeAccum=0;
    mNeedFilter=false;
}
// FUNCTION: WON 0x007c5d30
ServerListCtrl::~ServerListCtrl() {}
// FUNCTION: WON 0x007c6a70
void ServerListCtrl::FilterInProgressGames(ServerDetailsList& ioList)
{
    for(ServerDetailsList::iterator it=ioList.begin();it!=ioList.end();)
    {
        if((*it)->mGame->mB13) ioList.erase(it++);
        else ++it;
    }
}
// FUNCTION: WON 0x007c6ad0
void ServerListCtrl::DoFilter(ServerDetailsList& ioList)
{
    SmartPtr<LobbyGame> aSelectedGame=GetSelGame();
    for(unsigned int i=0;i<mColumnInfo.size();++i)
    {
        ColumnInfo& aColumn=mColumnInfo[i];
        if(aColumn.mFilterFunc!=NULL && aColumn.mFilterCombo.get()!=NULL && aColumn.mFilterCombo->mEdit->GetText().length()!=0)
            aColumn.mFilterFunc(aColumn.mFilterCombo->mEdit->GetText(),ioList,this);
    }
    if(mGameType!=3 && !LobbyFn35E380()) FilterInProgressGames(ioList);
}
// FUNCTION: WON 0x007c6be0
bool ServerListCtrl::FitsFilter(ServerDetailsItem* inItem)
{
    ServerDetailsList aList;
    aList.push_back(inItem);
    DoFilter(aList);
    return !aList.empty();
}
// FUNCTION: WON 0x007c6d00
void ServerListCtrl::UpdateJoinButton()
{
    bool aEnabled=false;
    LobbyGame* aGame=GetSelGame();
    if(aGame!=NULL) aEnabled=aGame->Fn3A7A20();
    mJoinButton->SetEnabled(aEnabled);
    if(mRemoveButton.get()!=NULL) mRemoveButton->SetEnabled(aGame!=NULL);
}
// FUNCTION: WON 0x007c7580
LobbyGame* ServerListCtrl::GetSelGame()
{
    ServerDetailsItem* aSelected=static_cast<ServerDetailsItem*>(mList->mSelItem);
    return aSelected==NULL?NULL:aSelected->mGame.get();
}
// FUNCTION: WON 0x007c6ff0
void ServerListCtrl::AddEntries(const ServerDetailsList& inList)
{
    LobbyGame* aSelectedGame=GetSelGame();
    mList->Fn2FAA00();
    mList->Fn2FAA50(true);
    for(ServerDetailsList::const_iterator it=inList.begin();it!=inList.end();++it)
    {
        ServerDetailsItem* aItem=*it;
        mList->ListArea::InsertItem(aItem,-1);
        aItem->mGame->mIsFiltered=false;
        if(aItem->mGame.get()==aSelectedGame)
        {
            int aPos=mList->GetItemPos(aItem);
            mList->SetSelItem(aPos,true);
        }
    }
    mList->Fn2FAA10();
    UpdateJoinButton();
    UpdateSummaryLabel();
}
// Native Container timer slot44, not void slot48. Needs GUI typed TimerFlags2a next window.
// FUNCTION: WON 0x007c66d0
bool ServerListCtrl::Virtual44(int inDelta)
{
    if(mNeedFilter)
    {
        mFilterTimeAccum+=inDelta;
        if(mFilterTimeAccum>=100 && (mFilterTimeAccum>=500 || static_cast<unsigned int>(mFilterTimeAccum)>=mServerDetailsMap.size()/4))
        {
            AddFilteredEntries_All(true);
            mNeedFilter=false;
        }
    }
    if(!mNeedFilter) mTimerFlags&=~2;
    return Container::Virtual44(inDelta);
}
// FUNCTION: WON 0x007c7220
void ServerListCtrl::AddFilteredEntries_All(bool inPreserve)
{
    ServerDetailsList aList;
    for(ServerDetailsMap::iterator it=mServerDetailsMap.begin();it!=mServerDetailsMap.end();++it)
    {
        it->second->mGame->mIsFiltered=true;
        aList.push_back(it->second.get());
    }
    AddFilteredEntries(inPreserve,aList);
}
// FUNCTION: WON 0x007c73c0
void ServerListCtrl::AddFilteredEntries_List(bool inPreserve)
{
    ServerDetailsList aList;
    int aCount=mList->GetNumItems();
    for(int i=0;i<aCount;++i) aList.push_back(static_cast<ServerDetailsItem*>(mList->GetItem(i)));
    AddFilteredEntries(inPreserve,aList);
}
// FUNCTION: WON 0x007c7090
void ServerListCtrl::AddFilteredEntries(bool inPreserve,ServerDetailsList& ioList)
{
    mNeedFilter=false;
    std::list<std::pair<ListItem*,int> > aOldItems;
    if(inPreserve)
    {
        int aOffset=mList->mVertOffset;
        int aBottom=aOffset+mList->mScrollArea.height;
        int anIndex=mList->GetItemPosAtYPos(aOffset);
        ListItem* anItem=mList->GetItem(anIndex);
        if(anItem!=NULL && (anItem->mHeight+anItem->mTop)/2<aOffset) ++anIndex;
        for(;;)
        {
            ListItem* anOldItem=mList->GetItem(anIndex++);
            if(anOldItem==NULL || anOldItem->mTop>aBottom) break;
            aOldItems.push_back(std::pair<ListItem*,int>(anOldItem,anOldItem->mTop-aOffset));
        }
    }
    DoFilter(ioList);
    AddEntries(ioList);
    if(inPreserve)
    {
        for(std::list<std::pair<ListItem*,int> >::iterator it=aOldItems.begin();it!=aOldItems.end();++it)
        {
            if(mList->GetItemPos(it->first)>=0)
            {
                mList->SetVertOffset(it->first->mTop-it->second);
                break;
            }
        }
    }
}

void ServerListCtrl::SetNumColumns(int inCount)
{
    mNumColumns=inCount;
    mColumnInfo.resize(inCount);
}
// FUNCTION: WON 0x007c80c0
bool ServerListCtrl::HandleRefreshButton()
{
    if(LobbyFn35B940()>0)
    {
        LobbyFn35B960(1);
        AddFilteredEntries_List(true);
        return true;
    }
    return false;
}
// FUNCTION: WON 0x007c7a50
void ServerListCtrl::HandleListItemClicked(ListItemClickedEvent* inEvent)
{
    if(!inEvent->IsSelectedItem()) return;
    ServerDetailsItem* aSelected=static_cast<ServerDetailsItem*>(mList->mSelItem);
    if(aSelected==NULL) return;
    LobbyGame* aGame=aSelected->mGame.get();
    if(inEvent->mButton==2) Fn2F53F0(new GameActionEvent(7,aGame),false);
    else if(inEvent->mDoubleClick && inEvent->mButton==0 && inEvent->mX>=mList->GetColumnWidth(0) && aGame->Fn3A7A20())
        Fn2F53F0(new JoinGameEvent(aGame),false);
}
// FUNCTION: WON 0x007c75a0
void ServerListCtrl::HandleGameChanged(GameChangedEvent* inEvent)
{
    LobbyGame* aGame=inEvent->mGame.get();
    if(aGame->GetGameType()!=mGameType) return;
    if(inEvent->IsAdd())
    {
        SmartPtr<ServerDetailsItem> anItem=ServerDetailsItem::CreateItem();
        anItem->SetGame(aGame);
        mServerDetailsMap[aGame]=anItem;
        bool aFits=FitsFilter(anItem.get());
        aGame->mIsFiltered=!aFits;
        if(aFits) mList->ListArea::InsertItem(anItem.get(),-1);
    }
    else if(inEvent->IsDelete())
    {
        ServerDetailsMap::iterator it=mServerDetailsMap.find(aGame);
        if(it!=mServerDetailsMap.end())
        {
            mList->RemoveItem(it->second.get());
            mServerDetailsMap.erase(it);
        }
    }
    else
    {
        ServerDetailsMap::iterator it=mServerDetailsMap.find(aGame);
        if(it==mServerDetailsMap.end()) return;
        ServerDetailsItem* anItem=it->second.get();
        bool aVisible=mList->GetItemPos(anItem)>=0;
        bool aFits=FitsFilter(anItem);
        anItem->mGame->mIsFiltered=!aFits;
        if(aVisible)
        {
            if(aFits) mList->Fn2FC730(anItem);
            else mList->RemoveItem(anItem);
        }
        else if(aFits) mList->ListArea::InsertItem(anItem,-1);
    }
    UpdateJoinButton();
    UpdateSummaryLabel();
}
// FUNCTION: WON 0x007c7870
void ServerListCtrl::HandleSyncGameList(SyncGameListEvent* inEvent)
{
    LobbyGameList* aGames=inEvent->mGames.get();
    if(aGames->mType!=mGameType) return;
    aGames->Rewind();
    mServerDetailsMap.clear();
    while(aGames->HasMore())
    {
        LobbyGame* aGame=aGames->Next();
        SmartPtr<ServerDetailsItem> anItem=ServerDetailsItem::CreateItem();
        anItem->SetGame(aGame);
        mServerDetailsMap[aGame]=anItem;
    }
    AddFilteredEntries_All(false);
}
// FUNCTION: WON 0x007c6d50
void ServerListCtrl::UpdateSummaryLabel()
{
    if(mSummaryLabel.get()==NULL) return;
    char aTotalCount[33];
    char aShownCount[33];
    sprintf(aTotalCount,"%d",mServerDetailsMap.size());
    if(LobbyFn35B940()>0)
    {
        sprintf(aShownCount,"%d",LobbyFn35B940());
        mSummaryLabel->SetText(LobbyFormatString(::ServerListCtrl_ListPingSummary_String,GUIString(new GUIStringData(aShownCount))),true);
        mRefreshButton->SetText(::ServerListCtrl_StopRefresh_String,true);
    }
    else
    {
        sprintf(aShownCount,"%d",mList->GetNumItems());
        // Native total-count allocation precedes shown-count allocation.
        {
            GUIString aTotal(new GUIStringData(aTotalCount));
            mSummaryLabel->SetText(LobbyFn34A130(::ServerListCtrl_ListSummary_String,GUIString(new GUIStringData(aShownCount)),aTotal),true);
        }
        mRefreshButton->SetText(::ServerListCtrl_RefreshList_String,true);
    }
}
// FUNCTION: WON 0x007c6740
void ServerListCtrl::LoadFilterCombos()
{
    for(ColumnInfoVector::iterator it=mColumnInfo.begin();it!=mColumnInfo.end();++it)
    {
        ComboBox* aCombo=it->mFilterCombo.get();
        if(aCombo==NULL) continue;
        aCombo->Clear(true);
        aCombo->InsertItem(new TextListItem(::ServerListCtrl_NoFilter_String,mBoldFont.get()),-1);
        aCombo->InsertItem(new FilterListItem(it->mColumnName,mBoldFont.get()),-1);
        const LobbyOptionSet& aFilters=LobbyFn35DC80(it->mColumnName);
        for(std::list<GUIString>::const_iterator jt=aFilters.mValues.begin();jt!=aFilters.mValues.end();++jt)
            aCombo->AddString(*jt,-1);
    }
}
// Actual external signed-DWORD storage4df4d4. Initial1 does not imply immutability.
extern int gServerFilterItemPos4DF4D4; // Root data binding; no local constant/definition.
// FUNCTION: WON 0x007c7b30
void ServerListCtrl::HandleComboTextChanged(Component* inComponent)
{
    ComboBox* aCombo=dynamic_cast<ComboBox*>(inComponent);
    if(aCombo==NULL) return;
    mNeedFilter=true;
    mFilterTimeAccum=0;
    Fn2F57D0(true);
    const GUIString& aText=aCombo->mEdit->GetText();
    FilterListItem* aFilter=static_cast<FilterListItem*>(aCombo->mListArea->GetItem(gServerFilterItemPos4DF4D4));
    if(aFilter==NULL) return;
    if(aText.length()==0)
    {
        aFilter->mText=::ServerListCtrl_AddRemove_String;
        aFilter->mIsAdd=true;
        aFilter->mFilterText=GUIString(new GUIStringData(""));
        aFilter->mSelPos=0;
        aCombo->UpdateItem(aFilter);
        return;
    }
    int aCount=aCombo->mListArea->GetNumItems();
    int aPos;
    for(aPos=0;aPos<aCount;++aPos)
    {
        TextListItem* anItem=static_cast<TextListItem*>(aCombo->mListArea->GetItem(aPos));
        if(anItem->mText==aText) break;
    }
    if(aPos<aCount && aPos<=gServerFilterItemPos4DF4D4) return;
    aFilter->mSelPos=aPos;
    aFilter->mIsAdd=aPos>=aCount;
    aFilter->mFilterText=aText;
    GUIString aLabel;
    if(aFilter->mIsAdd) aLabel=::ServerListCtrl_Add_String;
    else aLabel=::ServerListCtrl_Remove_String;
    aLabel.append(aText);
    aLabel.append(GUIString(new GUIStringData("\")")));
    aFilter->mText=aLabel;
    aCombo->UpdateItem(aFilter);
}
// FUNCTION: WON 0x007c7dd0
void ServerListCtrl::HandleComboSelChanged(ComboSelChangedEvent* inEvent)
{
    ComboBox* aCombo=dynamic_cast<ComboBox*>(inEvent->mComponent);
    if(aCombo==NULL) return;
    int aPos=aCombo->mListArea->GetItemPos(inEvent->mItem.get());
    if(aPos>gServerFilterItemPos4DF4D4) return;
    if(aPos==0 && inEvent->mActiveSel)
    {
        aCombo->SetText(GUIString(new GUIStringData("")),true);
        return;
    }
    FilterListItem* aFilter=static_cast<FilterListItem*>(aCombo->mListArea->GetItem(gServerFilterItemPos4DF4D4));
    if(!inEvent->mActiveSel)
    {
        aCombo->SetText(aFilter->mFilterText,true);
        aCombo->mEdit->SelectRange(-1,0);
        return;
    }
    if(aFilter->mIsAdd)
    {
        if(aFilter->mFilterText.length()!=0)
        {
            ListItem* anItem=aCombo->AddString(aFilter->mFilterText,-1);
            aCombo->SetSelItem(aCombo->mListArea->GetItemPos(anItem),true);
            LobbyFn35DEA0(aFilter->mColumnName,aFilter->mFilterText);
        }
        else aCombo->SetText(GUIString(new GUIStringData("")),true);
        return;
    }
    if(aFilter->mSelPos<=gServerFilterItemPos4DF4D4) return;
    aCombo->DeleteItem(aFilter->mSelPos);
    aCombo->SetText(GUIString(new GUIStringData("")),true);
    LobbyFn35DED0(aFilter->mColumnName,aFilter->mFilterText);
}
// FUNCTION: WON 0x007c80f0
void ServerListCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType)
    {
    case 15:
        if(inEvent->mComponent==mJoinButton.get())
        {
            LobbyGame* aGame=GetSelGame();
            if(aGame!=NULL) Fn2F53F0(new JoinGameEvent(aGame),false);
            return;
        }
        if(inEvent->mComponent==mRefreshButton.get() && HandleRefreshButton()) return;
        break;
    case 22: UpdateJoinButton();break; // original always calls Component fallback afterward
    case 21: HandleComboTextChanged(inEvent->mComponent);return;
    case 27: HandleComboSelChanged(static_cast<ComboSelChangedEvent*>(inEvent));return;
    case 23: HandleListItemClicked(static_cast<ListItemClickedEvent*>(inEvent));return;
    case 100020: HandleGameChanged(static_cast<GameChangedEvent*>(inEvent));return;
    case 100057: HandleSyncGameList(static_cast<SyncGameListEvent*>(inEvent));return;
    case 100014:
    case 100034: LoadFilterCombos();return;
    case 100059: AddFilteredEntries_All(true);return;
    case 100058: AddFilteredEntries_List(true);return;
    }
    Component::HandleComponentEvent(inEvent);
}
// FUNCTION: WON 0x007c6360
void ServerListCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig,mList,"ServerList",__FILE__,647);
    mBoldFont=mList->Fn2F5B50(1,0);
    SafeGetComponent(aConfig,mJoinButton,"JoinGameButton",__FILE__,650);
    SafeGetComponent(aConfig,mRefreshButton,"RefreshListButton",__FILE__,651);
    SafeGetComponent(aConfig,mSummaryLabel,"ServerListSummary",__FILE__,652);
    mRemoveButton=aConfig->Fn386860(std::string("RemoveButton"));
    UpdateJoinButton();
    UpdateSummaryLabel();
    Fn2F5360(1);
}
// FUNCTION: WON 0x007c5ff0
ServerListCtrl::ColumnInfo* ServerListCtrl::GetColumnInfo(const GUIString& inName)
{
    ColumnInfo& aColumn=mColumnMap[inName];
    aColumn.mColumnName=inName;
    return &aColumn;
}
// FUNCTION: WON 0x007c6050
void ServerListCtrl::InitColumnMap()
{
    ColumnInfo* aColumn=GetColumnInfo(GUIString(new GUIStringData("Icon")));
    aColumn->mPaintFunc=ServerDetailsItem::PaintIcon;
    aColumn->mCompareFunc=ServerDetailsItem::CompareIcon;
    aColumn=GetColumnInfo(GUIString(new GUIStringData("Skill")));
    aColumn->mPaintFunc=ServerDetailsItem::PaintSkill;
    aColumn->mCompareFunc=ServerDetailsItem::CompareSkill;
    aColumn->mFilterFunc=ServerDetailsItem::FilterSkill;
    aColumn=GetColumnInfo(GUIString(new GUIStringData("GameName")));
    aColumn->mPaintFunc=ServerDetailsItem::PaintGameName;
    aColumn->mCompareFunc=ServerDetailsItem::CompareGameName;
    aColumn->mFilterFunc=ServerDetailsItem::FilterGameName;
    aColumn=GetColumnInfo(GUIString(new GUIStringData("Address")));
    aColumn->mPaintFunc=ServerDetailsItem::PaintAddress;
    aColumn->mCompareFunc=ServerDetailsItem::CompareAddress;
    aColumn->mFilterFunc=ServerDetailsItem::FilterAddress;
    aColumn->mGameTypeFlags=4;
    aColumn=GetColumnInfo(GUIString(new GUIStringData("NumPlayers")));
    aColumn->mPaintFunc=ServerDetailsItem::PaintNumPlayers;
    aColumn->mCompareFunc=ServerDetailsItem::CompareNumPlayers;
    aColumn->mFilterFunc=ServerDetailsItem::FilterNumPlayers;
    aColumn=GetColumnInfo(GUIString(new GUIStringData("Ping")));
    aColumn->mPaintFunc=ServerDetailsItem::PaintPing;
    aColumn->mCompareFunc=ServerDetailsItem::ComparePing;
    aColumn->mFilterFunc=ServerDetailsItem::FilterPing;
    aColumn->mGameTypeFlags&=~2;
}

}

// Scratch private reference-only ServerListCtrlConfiga4. No construction, sizeof,
// by-value/copy, factory, own vtable or runtime authority; five call contracts queued.
namespace WONAPI {
class MultiListHeader;
// ServerListCtrlConfig declaration hoisted to WONLobbyControlConfig.h; existing bodies unchanged.
// FUNCTION: WON 0x007c8380
void ServerListCtrlConfig::Virtual34(Component* inComponent)
{
    ContainerConfig::Virtual34(inComponent);
    mServerList=static_cast<ServerListCtrl*>(inComponent);
}
// FUNCTION: WON 0x007c83a0
bool ServerListCtrlConfig::Virtual2C(const std::string& inName,int& outValue)
{
    if(inName=="NUMCOLUMNS") { outValue=mServerList->mColumnInfo.size();return true; }
    return ComponentConfig::Virtual2C(inName,outValue);
}
// FUNCTION: WON 0x007c8c30
bool ServerListCtrlConfig::Virtual24(const std::string& inInstruction)
{
    if(ContainerConfig::Virtual24(inInstruction)) return true;
    if(inInstruction=="LISTID")
    {
        GUIString aType=Fn3842F0(false);
        if(aType.CompareNoCase("net")==0) mServerList->mGameType=2;
        else if(aType.CompareNoCase("lan")==0) mServerList->mGameType=1;
        else if(aType.CompareNoCase("direct")==0) mServerList->mGameType=3;
        else throw ConfigObjectException("Invalid server list id: "+static_cast<std::string>(aType));
        return true;
    }
    if(inInstruction=="ADDCOLUMN") { HandleAddColumn();return true; }
    if(inInstruction=="DEFAULTFILTER")
    {
        GUIString aColumn=Fn3842F0(false);
        SkipComma();
        GUIString aFilters=Fn3842F0(false);
        ServerListCtrl::AddDefaultFilters(aColumn,aFilters,false);
        return true;
    }
    if(inInstruction=="NUMCOLUMNS") { mServerList->SetNumColumns(ReadInt());return true; }
    if(inInstruction=="SETCOLUMNFILTERTYPE")
    {
        ReadInt();
        SkipComma();
        GUIString aType=Fn3842F0(false);
        if(aType.CompareNoCase("int")!=0 && aType.CompareNoCase("string")!=0 && aType.CompareNoCase("player")!=0)
            throw ConfigObjectException("Unknown filter type: "+static_cast<std::string>(aType));
        return true;
    }
    return false;
}
// FUNCTION: WON 0x007c8410
void ServerListCtrlConfig::HandleAddColumn()
{
    MultiListArea* aList=mServerList->mList.get();
    if(aList==NULL)
    {
        Component* aComponent=Fn386860(std::string("ServerList"));
        if(aComponent==NULL) throw ConfigObjectException("Need ServerList before calling AddColumn");
        aList=dynamic_cast<MultiListArea*>(aComponent);
        if(aList==NULL) throw ConfigObjectException("ServerList needs to be a MultiListArea");
        mServerList->mList=aList;
        aComponent=Fn386860(std::string("ServerListHeader"));
        if(aComponent==NULL) throw ConfigObjectException("Need ServerList header before calling AddColumn");
        mHeader=dynamic_cast<MultiListHeader*>(aComponent);
        if(mHeader==NULL) throw ConfigObjectException("ServerListHeader needs to be a MultiListHeader");
    }
    GUIString aColumn=Fn3842F0(false);
    SkipComma();
    Component* aComponent=Fn386860(aColumn);
    if(aComponent==NULL) throw ConfigObjectException("Component not found: "+static_cast<std::string>(aColumn));
    int aWidth=ReadInt();
    ComboBox* aCombo=NULL;
    if(!EndOfString())
    {
        SkipComma();
        aCombo=dynamic_cast<ComboBox*>(ReadComponent());
        if(aCombo==NULL) throw ConfigObjectException(std::string("Invalid ")+"ComboBox");
    }
    ServerListCtrl::ColumnMap::iterator it=ServerListCtrl::mColumnMap.find(aColumn);
    if(it==ServerListCtrl::mColumnMap.end())
        throw ConfigObjectException("Column not defined: "+static_cast<std::string>(aColumn));
    ServerListCtrl::ColumnInfo& aInfo=it->second;
    int aType=mServerList->mGameType;
    if(aType==1) { if(!(aInfo.mGameTypeFlags&2)) return; }
    else if(aType==2) { if(!(aInfo.mGameTypeFlags&1)) return; }
    else if(aType==3) { if(!(aInfo.mGameTypeFlags&4)) return; }
    else return;
    aList->SetNumColumns(++mNumColumns);
    if(aWidth==0) aList->mDynamicSizeColumn=mNumColumns-1;
    else aList->SetColumnWidth(mNumColumns-1,aWidth,false);
    mHeader->SetColumnItem(mNumColumns-1,aComponent);
    mServerList->mColumnInfo.push_back(aInfo);
    mServerList->mColumnInfo.back().mFilterCombo=aCombo;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00765640
ComponentConfig* ServerListCtrlConfig::CfgFactory()
{
    return new ServerListCtrlConfig;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007c8360
ServerListCtrlConfig::ServerListCtrlConfig()
{
    mHeader=NULL;
    mNumColumns=0;
}
}

namespace WONAPI {
// Genuine SDK protected class-static factory storage; one mutable typed pointer.
ServerDetailsItem::ItemFactoryFunc ServerDetailsItem::mItemFactoryFunc = ServerDetailsItem::DefaultItemFactory;
}
// TEMPLATE: WON 0x007c5ef0
// TEMPLATE: WON 0x007cc350

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bad40
// (protected: static class WONAPI::ServerDetailsItem * __cdecl WONAPI::ServerDetailsItem::DefaultItemFactory(void)    [?DefaultItemFactory@ServerDetailsI)
// TEMPLATE: WON 0x007c4cf0
// (public: virtual void * __thiscall WONAPI::ServerDetailsItem::`scalar deleting destructor'(unsigned int)    [??_GServerDetailsItem@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c5c10
// (public: __thiscall WONAPI::ServerListCtrl::ServerListCtrl(void)    [??0ServerListCtrl@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007c5d10
// (public: virtual void * __thiscall WONAPI::ServerListCtrl::`scalar deleting destructor'(unsigned int)    [??_GServerListCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c68f0
// (public: __thiscall WONAPI::TextListItem::TextListItem(class WONAPI::GUIString const &,class WONAPI::Font *)    [??0TextListItem@WONAPI@@QAE@ABVGUIStri)
// TEMPLATE: WON 0x007c6970
// (public: virtual __thiscall FilterListItem::~FilterListItem(void)    [??1FilterListItem@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007cb7c0
// (public: __thiscall std::vector<struct WONAPI::ServerListCtrl::ColumnInfo,class std::allocator<struct WONAPI::ServerListCtrl::ColumnInfo> >::~vector<st)
// TEMPLATE: WON 0x007cc1d0
// (public: class std::_Ptrit<struct WONAPI::ServerListCtrl::ColumnInfo,int,struct WONAPI::ServerListCtrl::ColumnInfo *,struct WONAPI::ServerListCtrl::Col)
// TEMPLATE: WON 0x007cc1f0
// (public: class std::_Ptrit<struct WONAPI::ServerListCtrl::ColumnInfo,int,struct WONAPI::ServerListCtrl::ColumnInfo *,struct WONAPI::ServerListCtrl::Col)
// TEMPLATE: WON 0x007cc400
// (public: __thiscall std::pair<class WONAPI::SmartPtr<class WONAPI::LobbyGame> const ,class WONAPI::SmartPtr<class WONAPI::ServerDetailsItem> >::pair<cl)
// TEMPLATE: WON 0x007ccf60
// (protected: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::ServerListCtrl::ColumnInfo>,struct )
// TEMPLATE: WON 0x007cd840
// (public: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::ServerListCtrl::ColumnInfo>,)
// TEMPLATE: WON 0x007ce4d0
// (protected: struct WONAPI::ServerListCtrl::ColumnInfo * __thiscall std::vector<struct WONAPI::ServerListCtrl::ColumnInfo,class std::allocator<struct WO)
// TEMPLATE: WON 0x007ce510
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::ServerListCtrl::ColumnInfo,int,struct WONAPI::ServerListCtrl::ColumnInfo *,struct WONAPI::Serv)
// TEMPLATE: WON 0x007ce5d0
// (class std::_Ptrit<struct WONAPI::ServerListCtrl::ColumnInfo,int,struct WONAPI::ServerListCtrl::ColumnInfo *,struct WONAPI::ServerListCtrl::ColumnInfo )
// TEMPLATE: WON 0x007ce6a0
// (struct WONAPI::ServerListCtrl::ColumnInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::ServerListCtrl::ColumnInfo,int,struct WONAPI::ServerLis)
// TEMPLATE: WON 0x007ce770
// (void __cdecl std::_Construct(struct std::pair<class WONAPI::GUIString const ,struct WONAPI::ServerListCtrl::ColumnInfo> *,struct std::pair<class WONAP)
// TEMPLATE: WON 0x007ce7f0
// (void __cdecl std::_Construct(struct WONAPI::ServerListCtrl::ColumnInfo *,struct WONAPI::ServerListCtrl::ColumnInfo const &)    [?_Construct@std@@YAXPA)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007cbfc0
// (public: __thiscall std::pair<class WONAPI::SmartPtr<class WONAPI::LobbyGame> const ,class WONAPI::SmartPtr<class WONAPI::ServerDetailsItem> >::~pair<c)
// TEMPLATE: WON 0x007cd260
// (protected: void __thiscall std::_Tree<class WONAPI::SmartPtr<class WONAPI::LobbyGame>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::LobbyGame>)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c6950
// (public: virtual void * __thiscall FilterListItem::`scalar deleting destructor'(unsigned int)    [??_GFilterListItem@@UAEPAXI@Z])
// TEMPLATE: WON 0x007cb9d0
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::LobbyGame>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::LobbyGame> const ,class)
// TEMPLATE: WON 0x007cc210
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::ServerListCtrl::ColumnInfo>,struct std)
