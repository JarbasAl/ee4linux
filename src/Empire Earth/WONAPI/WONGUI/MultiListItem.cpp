// Source-intent queue: MultiListItem behavior family reserved by Root; GUI header gate pending.
// No allocation or original lifetime implementation is introduced here.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x7026e0
void MultiListItem::Virtual08(ListArea* inList)
{
    mHeight=0;
    for(ItemVector::iterator anItr=mItems.begin();anItr!=mItems.end();++anItr) {
        ListItem* anItem=anItr->get();
        if(anItem!=NULL) {
            anItem->Virtual08(inList);
            int aHeight=anItem->mHeight;
            if(aHeight>mHeight) mHeight=aHeight;
        }
    }
    mL10=static_cast<MultiListArea*>(inList)->mTotalColumnWidth;
}
// FUNCTION: WON 0x702740
int MultiListItem::Virtual10(const ListItem* inOther,ListArea* inList) const
{
    MultiListArea* anArea=static_cast<MultiListArea*>(inList);
    int aColumn=anArea->GetSortColumn();
    ListItem* anItem=GetItem(aColumn);
    ListItem* anOther=static_cast<const MultiListItem*>(inOther)->GetItem(aColumn);
    if(anItem==anOther) return 0;
    if(anItem==NULL) return -1;
    if(anOther==NULL) return 1;
    if(anArea->mSortColFunc!=NULL) return anArea->mSortColFunc(inList,anItem,anOther);
    return anItem->Virtual10(anOther,inList);
}
// FUNCTION: WON 0x702910
void MultiListItem::Virtual18(int inX,int inY)
{
    mTop=inY;
    for(ItemVector::iterator anItr=mItems.begin();anItr!=mItems.end();++anItr) {
        ListItem* anItem=anItr->get();
        if(anItem!=NULL) anItem->Virtual18(0,inY);
    }
}
// Actual3027b0..3027e7,55B. Column lookup changes the caller's horizontal coordinate.
// FUNCTION: WON 0x007027b0
ListItem* MultiListItem::GetMouseItem(ListArea* inList,int& ioX)
{
    MultiListArea* anArea=static_cast<MultiListArea*>(inList);
    int aColumn=anArea->GetColumnAt(ioX);
    ioX-=anArea->GetColumnPos(aColumn);
    return GetItem(aColumn);
}
// FUNCTION: WON 0x7027f0
ListItem* MultiListItem::GetMouseItem(ListArea* inList,int& ioX,int& ioY)
{
    MultiListArea* anArea=static_cast<MultiListArea*>(inList);
    int aColumn=anArea->GetColumnAt(ioX);
    ioX-=anArea->GetColumnPos(aColumn);
    ListItem* anItem=GetItem(aColumn);
    ListItem* anOld=anArea->mMouseColItem;
    if(anOld!=NULL) {
        if(anOld!=anItem || !anOld->Virtual1C(ioX,ioY+mTop)) {
            anOld->Virtual24(inList);
            anArea->Virtual78(NULL);
            anArea->mMouseColItem=NULL;
        }
    } else if(anItem!=NULL && anItem->Virtual1C(ioX,ioY+mTop)) {
        anArea->mMouseColItem=anItem;
        anArea->mMouseColNum=aColumn;
        anItem->Virtual20(inList,ioX,ioY);
    }
    return anArea->mMouseColItem;
}
// FUNCTION: WON 0x702530
void MultiListItem::Virtual04(Graphics& inGraphics,ListArea* inList)
{
    MultiListArea* anArea=static_cast<MultiListArea*>(inList);
    int aColumn=anArea->mColumnPaintContext;
    if(aColumn<0 && mB08) {
        if(anArea->ListFlagSet(0x10)) {
            inGraphics.SetColor(anArea->GetSelColor().GetBackgroundColor(inGraphics));
            inGraphics.FillRect(0,0,anArea->mTotalColumnWidth,mHeight);
        }
        return;
    }
    if(static_cast<unsigned int>(aColumn)>=mItems.size()) return;
    ListItem* anItem=mItems[aColumn].get();
    if(anItem==NULL) return;
    anItem->mHeight=mHeight;
    if(mB08 && (anArea->ListFlagSet(0x10) || aColumn==0)) anItem->mB08=mB08; else anItem->mB08=false;
    unsigned char anAlign=anArea->mColumnInfoVector[aColumn].mHAlign;
    int anOffset=0;
    if(anAlign==1) anOffset=(anArea->mL12C-anItem->mL10)/2;
    else if(anAlign==2) anOffset=anArea->mL12C-anItem->mL10;
    if(anOffset<0) anOffset=0;
    inGraphics.Translate(anOffset,0);
    anItem->Virtual04(inGraphics,inList);
    inGraphics.Translate(-anOffset,0);
}
// FUNCTION: WON 0x702650
void MultiListItem::Virtual0C(ListArea* inList,Container* inViewport)
{
    unsigned int aColumn=0;
    for(ItemVector::iterator anItr=mItems.begin();anItr!=mItems.end();aColumn++,++anItr) {
        std::vector<MultiListArea::ColumnInfo>& aVec=static_cast<MultiListArea*>(inList)->mColumnInfoVector;
        if(anItr->get()!=NULL && aColumn<aVec.size()) {
            Container* aViewport=inViewport==NULL ? NULL : aVec[aColumn].mContainer.get();
            (*anItr)->Virtual0C(inList,aViewport);
        }
    }
}
// FUNCTION: WON 0x7028d0
bool MultiListItem::Virtual14(ListArea* inList,int inX,int inY)
{
    if(!mB09) return false;
    ListItem* anItem=GetMouseItem(inList,inX);
    if(anItem!=NULL) return anItem->Virtual14(inList,inX,inY);
    return true;
}
// Actual302950..302967,23B, forwarding support0.
// FUNCTION: WON 0x00702950
void MultiListItem::Virtual20(ListArea* inList,int inX,int inY)
{
    GetMouseItem(inList,inX,inY);
}
// Actual302970..302993,35B; field154 is the base list's mouse item, distinct from column-item194.
// FUNCTION: WON 0x00702970
void MultiListItem::Virtual24(ListArea* inList)
{
    MultiListArea* anArea=static_cast<MultiListArea*>(inList);
    if(anArea->mMouseColItem!=NULL) {
        anArea->mMouseColItem->Virtual24(inList);
        inList->mMouseItem=NULL;
    }
}
// Actual3029a0..3029cf,47B; route to the current cell after GetMouseItem updates row hover state.
// FUNCTION: WON 0x007029a0
void MultiListItem::Virtual28(ListArea* inList,int inX,int inY)
{
    ListItem* anItem=GetMouseItem(inList,inX,inY);
    if(anItem!=NULL) anItem->Virtual28(inList,inX,inY);
}
// Actual3029d0..302a09,57B; drag stays with the saved cell, using its saved column's horizontal origin.
// FUNCTION: WON 0x007029d0
void MultiListItem::Virtual2C(ListArea* inList,int inX,int inY)
{
    MultiListArea* anArea=static_cast<MultiListArea*>(inList);
    ListItem* anItem=anArea->mMouseColItem;
    if(anItem!=NULL) anItem->Virtual2C(inList,inX-anArea->GetColumnPos(anArea->mMouseColNum),inY);
}

// FUNCTION: WON 0x00702a10
void MultiListItem::Virtual30(ListArea* inList,int inX,int inY,int inButton)
{
    ListItem* anItem=GetMouseItem(inList,inX,inY);
    if(anItem!=NULL) anItem->Virtual30(inList,inX,inY,inButton);
}
// Actual302a50..302a84,52B; does not include the next class constructor302a90.
// FUNCTION: WON 0x00702a50
void MultiListItem::Virtual34(ListArea* inList,int inX,int inY,int inButton)
{
    ListItem* anItem=GetMouseItem(inList,inX,inY);
    if(anItem!=NULL) anItem->Virtual34(inList,inX,inY,inButton);
}
// Actual302480..3024f7,119B, cell setter/resizer support0 under conservative breadth policy.
// No invented negative-index guard; the original compares vector size to the unsigned-converted index.
// FUNCTION: WON 0x00702480
void MultiListItem::SetItem(ListItem* inItem,int inColumn)
{
    if(mItems.size()<=inColumn) mItems.resize(inColumn+1);
    mItems[inColumn]=inItem;
}
}
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK multicolumn construction/size-mode policy.
void MultiListItem::InsertItem(ListItem *theItem)
{
	mItems.push_back(theItem);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071c4b0
// (public: void __thiscall std::vector<class WONAPI::SmartPtr<class WONAPI::ListItem>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::ListItem>)
// TEMPLATE: WON 0x0071b3c0
// (public: void __thiscall std::vector<class WONAPI::SmartPtr<class WONAPI::ListItem>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::ListItem>)
