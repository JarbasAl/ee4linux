// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// ListItem's out-of-line members precede ListArea in the original object: VC6 sees the ctor's body before
// `new ListItem` in ListArea::ListArea and drops the delete-on-throw guard (0x6fa570).
// FUNCTION: WON 0x006fa280
ListItem::ListItem()
{
    mTop=mL10=mHeight=0;
    mB08=false;
    mB09=true;
}
// FUNCTION: WON 0x006fa2a0
void ListItem::Virtual04(Graphics&,ListArea*) {}
// FUNCTION: WON 0x006fa2b0
int ListItem::Virtual10(const ListItem* other,ListArea*) const { return this<other; }
// FUNCTION: WON 0x006faa00
void ListArea::Fn2FAA00() { ++mDoingMultiChange; }
// FUNCTION: WON 0x006faa10
void ListArea::Fn2FAA10()
{
    if(--mDoingMultiChange<=0) {
        mDoingMultiChange=0;
        if(!Sort()) EnforceInvariants();
    }
}
// FUNCTION: WON 0x006facf0
std::vector<SmartPtr<ListItem> >::iterator ListArea::FindLineAt(int y)
{
    mSearchLine->mTop=y;
    std::vector<SmartPtr<ListItem> >::iterator first=mItems.begin(),last=mItems.end();
    // lower_bound by item top; the original uses its persistent search item.
    int count=last-first;
    while(count>0) {
        int step=count/2;
        std::vector<SmartPtr<ListItem> >::iterator mid=first+step;
        if((*mid)->mTop<mSearchLine->mTop) { first=mid+1; count-=step+1; }
        else count=step;
    }
    if((first==mItems.end() || (*first)->mTop>y) && first!=mItems.begin()) --first;
    return first;
}
// FUNCTION: WON 0x006fad80
ListItem* ListArea::FindItemAt(int y)
{
    std::vector<SmartPtr<ListItem> >::iterator i=FindLineAt(y);
    if(i==mItems.end()) return NULL;
    ListItem* item=i->get();
    return item->mTop+item->mHeight<y ? NULL : item;
}
// FUNCTION: WON 0x006fadc0
void ListArea::FindTopLine()
{
    std::vector<SmartPtr<ListItem> >::iterator i=FindLineAt(mVertOffset);
    mTopLineVectorPos=i-mItems.begin();
}
// FUNCTION: WON 0x006faf10
bool ListArea::IsItemVisible(ListItem* item)
{
    int top=item->mTop;
    int height=item->mHeight;
    if(top+height<mVertOffset) return false;
    int last=top;
    if(!ListFlagSet(0x80)) last=top+height;
    return last<=mVertOffset+mScrollArea.height;
}
// FUNCTION: WON 0x006faf60
void ListArea::EnsureVisible(ListItem* item)
{
    int offset=mVertOffset;
    if(item->mTop+item->mHeight>mVertOffset+mScrollArea.height)
        offset=item->mTop-mScrollArea.height+item->mHeight;
    if(offset>item->mTop) offset=item->mTop;
    if(offset!=mVertOffset) SetVertOffset(offset);
}
// FUNCTION: WON 0x006fb200
bool ListArea::SetSelItem(ListItem* item,bool add,bool fire)
{
    if(item==NULL) return ClearSelectionPrv(fire)>0;
    ListItem* old=mSelItem;
    if(item!=mSelItem) {
        if(mSelItem!=NULL && (!ListFlagSet(0x400) || !add)) {
            bool selected=item->mB08;
            ClearSelectionPrv(false);
            item->mB08=selected;
        }
        mSelItem=item;
    }
    if(mSelItem!=NULL && !mSelItem->mB08) mSelItem=NULL;
    if(item!=old) Invalidate();
    if(mSelItem!=old) {
        Invalidate();
        if(mFocusChild!=NULL) { mFocusChild->Virtual4C(); mFocusChild=NULL; }
        if(fire) Fn2F5380(22,0);
        ListSelChangedHook();
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006fb2e0
void ListArea::SelectRange(int start,int end)
{
    ClearSelectionPrv(false);
    int count=mItems.size();
    if(start>=count) start=count-1;
    if(end>=count) end=count-1;
    if(start<0) start=0;
    if(end<0) end=0;
    if(start>=count || end>=count) return;
    int step=start>end ? -1 : 1;
    for(int i=start;i!=end+step;i+=step) GetItem(i)->mB08=true;
    SetSelItem(GetItem(start),false,true);
}
// FUNCTION: WON 0x006fb410
void ListArea::SetSelItem(int index,bool fire)
{
    if(index>=0 && index<(int)mItems.size()) {
        ListItem* item=mItems[index].get();
        item->mB08=true;
        mKeyItem=item;
        SetSelItem(item,false,fire);
    } else {
        SetSelItem((ListItem*)NULL,false,fire);
        mKeyItem=NULL;
    }
}
// Native 15B selected-iterator reset; support only.
// FUNCTION: WON 0x006fb480
void ListArea::RewindSelections() { mSelItemItr=-1; FindNextSelection(); }
// Native49B iterator availability; support only.
// FUNCTION: WON 0x006fb490
bool ListArea::HasMoreSelections() { return (unsigned int)mSelItemItr<mItems.size(); }
// FUNCTION: WON 0x006fb4d0
void ListArea::FindNextSelection()
{
    ++mSelItemItr;
    while((unsigned int)mSelItemItr<mItems.size() && !mItems[mSelItemItr]->mB08) ++mSelItemItr;
}
// FUNCTION: WON 0x006fb530
ListItem* ListArea::GetNextSelection()
{
    if(HasMoreSelections()) {
        ListItem* item=mItems[mSelItemItr].get();
        FindNextSelection();
        return item;
    }
    return NULL;
}
// FUNCTION: WON 0x006fb560
int ListArea::ClearSelectionPrv(bool fire)
{
    if(mSelItem==NULL) return 0;
    int count=0;
    if(ListFlagSet(0x400)) {
        for(std::vector<SmartPtr<ListItem> >::iterator i=mItems.begin();i!=mItems.end();++i)
            if((*i)->mB08) { (*i)->mB08=false; ++count; }
    } else { count=1; mSelItem->mB08=false; }
    mSelItem=NULL;
    Invalidate();
    if(fire) { Fn2F5380(22,0); ListSelChangedHook(); }
    return count;
}
// Native13B forwarding support.
// FUNCTION: WON 0x006fb5f0
void ListArea::ClearSelection(bool fire) { ClearSelectionPrv(fire); }
// FUNCTION: WON 0x006fc450
int ListArea::GetItemPos(ListItem* item)
{
    if(item==NULL) return -1;
    std::vector<SmartPtr<ListItem> >::iterator i=FindLineAt(item->mTop);
    while(i!=mItems.end()) {
        if(i->get()==item) return i-mItems.begin();
        if((*i)->mTop>item->mTop) break;
        ++i;
    }
    return -1;
}
// Native37B position conversion support.
int ListArea::GetItemPosAtYPos(int y) { return FindLineAt(y)-mItems.begin(); }
// Native46B bounds getter support.
// FUNCTION: WON 0x006fc4e0
ListItem* ListArea::GetItem(int index)
{
    if(index<0 || (unsigned int)index>=mItems.size()) return NULL;
    return mItems[index].get();
}
// FUNCTION: WON 0x006faa50
void ListArea::Fn2FAA50(bool fire)
{
    mTotalHeight=0;
    mL0A4=0;
    mHorzOffset=0;
    mVertOffset=0;
    ClearSelection(fire);
    mSelItem=NULL;
    mKeyItem=NULL;
    if(mMouseItem!=NULL) NotifyMouseItemGone();
    mItems.clear();
    VirtualCC(false);
}
// FUNCTION: WON 0x006fc3c0
void ListArea::EnforceInvariants()
{
    if(mDoingMultiChange) return;
    std::vector<SmartPtr<ListItem> >::iterator i=mItems.begin();
    mTotalHeight=mL0A4=0;
    for(;i!=mItems.end();++i) {
        ListItem* item=i->get();
        item->Virtual18(0,mTotalHeight);
        mTotalHeight+=item->mHeight;
        if(item->mL10>mL0A4) mL0A4=item->mL10;
    }
    VirtualCC(false);
}
// Actual 11B default mouse-item reset; support0.
// FUNCTION: WON 0x006fcad0
void ListArea::NotifyMouseItemGone() { mMouseItem=NULL; }
// Native16B sort callback selector; support0.
// FUNCTION: WON 0x006fab20
ListArea::SortFunc ListArea::GetSortFunc() { return mSortFunc==NULL ? DefaultSortFunc : mSortFunc; }
// FUNCTION: WON 0x006fab50
bool ListArea::Sort()
{
    if(mSortType==2) return false;
    std::sort(mItems.begin(),mItems.end(),LineSortPred(this,GetSortFunc(),mSortType));
    EnforceInvariants();
    return true;
}
// FUNCTION: WON 0x006fabe0
int ListArea::FindInsertPos(ListItem* item,int position)
{
    if(mDoingMultiChange || mSortType==2) {
        if(position<0 || (unsigned int)position>mItems.size()) position=mItems.size();
        return position;
    }
    LineSortPred less(this,GetSortFunc(),mSortType);
    int first=0,count=mItems.size();
    while(count>0) {
        int step=count/2,mid=first+step;
        if(less(mItems[mid].get(),item)) { first=mid+1;count-=step+1; }
        else count=step;
    }
    return first;
}
// FUNCTION: WON 0x006fadf0
void ListArea::VirtualCC(bool scrollbar)
{
    mExtraHeight=0;
    if(ListFlagSet(0x40) && mTotalHeight>mScrollArea.height) {
        std::vector<SmartPtr<ListItem> >::iterator i=FindLineAt(mTotalHeight-mScrollArea.height+1);
        if(i!=mItems.end()) {
            ListItem* item=i->get();
            mExtraHeight=item->mTop+item->mHeight+mScrollArea.height-mTotalHeight;
            if(mExtraHeight<0) mExtraHeight=0;
        }
    }
    ScrollArea::VirtualCC(scrollbar);
    FindTopLine();
    if(ListFlagSet(0x40) && (unsigned int)mTopLineVectorPos<mItems.size()) {
        ListItem* item=mItems[mTopLineVectorPos].get();
        mVertOffset=item->mTop;
        mDownScrollAmount=item->mHeight;
        if(mTopLineVectorPos>0) mUpScrollAmount=mItems[mTopLineVectorPos-1]->mHeight;
        ScrollArea::VirtualCC(true);
    }
}
// Native22B size-change forwarding support0.
// FUNCTION: WON 0x006faef0
void ListArea::Virtual08() { ScrollArea::Virtual08();VirtualCC(false); }
// FUNCTION: WON 0x006fafb0
void ListArea::VirtualD4(Graphics& g)
{
    std::vector<SmartPtr<ListItem> >::iterator i=mItems.begin()+mTopLineVectorPos;
    int width=mL0A4;
    if(mScrollArea.width>width) width=mScrollArea.width;
    if(mL12C==-1) mL12C=width;
    while(i!=mItems.end()) {
        ListItem* item=i->get();
        if(!IsItemVisible(item)) break;
        g.Translate(0,item->mTop);
        item->Virtual04(g,this);
        g.Translate(0,-item->mTop);
        ++i;
    }
    mL12C=-1;
}
// FUNCTION: WON 0x006fb060
void ListArea::CheckMouseSel(ListItem* item,int x,int y,bool down)
{
    if(item==NULL) { if(ListFlagSet(4)) ClearSelection(true);return; }
    if(!IsItemVisible(item)) return;
    if(!item->Virtual14(this,x,y-item->mTop)) { if(ListFlagSet(4)) ClearSelection(true);return; }
    mKeyItem=item;
    bool add=false;
    if(down && ListFlagSet(0x400)) {
        int modifiers=GetRoot()->GetKeyMod();
        if(modifiers&4) {
            add=true;item->mB08=!item->mB08;Invalidate();
        } else if((modifiers&1) && mSelItem!=NULL) {
            SelectRange(GetItemPos(mSelItem),GetItemPos(item));
            return;
        } else {
            bool noChange=mSelItem==item && ClearSelectionPrv(false)==1;
            item->mB08=true;
            if(noChange) mSelItem=item;
        }
    } else if(ListFlagSet(2) || !ListFlagSet(8)) item->mB08=true;
    else item->mB08=!item->mB08;
    SetSelItem(item,add,true);
}
// FUNCTION: WON 0x006fc0a0
ListItem* ListArea::FindSelectableItem(int start,bool up,bool opposite)
{
    int step=up ? -1 : 1;
    int i=start;
    ListItem* item=GetItem(start);
    for(;item!=NULL;item=GetItem(i)) {
        if(item->mB09) return item;
        i+=step;
    }
    if(ListFlagSet(0x800)) {
        if(up) return FindSelectableItem(mItems.size()-1,false,false);
        else return FindSelectableItem(0,true,false);
    }
    if(opposite) return FindSelectableItem(start,!up,false);
    return NULL;
}
// FUNCTION: WON 0x006fc510
ListItem* ListArea::InsertItem(ListItem* item,int position)
{
    position=FindInsertPos(item,position);
    item->Virtual0C(this,GetViewport());
    item->Virtual08(this);
    if(item->mHeight<=0) item->mHeight=1;
    item->mB08=false;
    bool end=(unsigned int)position==mItems.size();
    mItems.insert(mItems.begin()+position,SmartPtr<ListItem>(item));
    if(mSelItemItr>=position) ++mSelItemItr;
    if(end) {
        item->Virtual18(0,mTotalHeight);
        mTotalHeight+=item->mHeight;
        if(item->mL10>mL0A4) mL0A4=item->mL10;
        if(!ListFlagSet(0x1000) && OnBottom()) mVertOffset+=item->mHeight;
        VirtualCC(false);
    } else {
        if(!ListFlagSet(0x1000) && (mTopLineVectorPos>=position || OnBottom()) && mTotalHeight>mScrollArea.height)
            mVertOffset+=item->mHeight;
        if(mDoingMultiChange) {
            int prev=position==0 ? 0 : mItems[position-1]->mTop;
            int next=mItems[position+1]->mTop;
            int y=prev+1;
            if(y>next) y=next;
            item->mTop=y;
        } else EnforceInvariants();
    }
    return item;
}
// FUNCTION: WON 0x006fc730
void ListArea::Fn2FC730(ListItem* item)
{
    int oldHeight=item->mHeight;
    item->Virtual0C(this,GetViewport());
    item->Virtual08(this);
    if(item->mHeight<=0) item->mHeight=1;
    if(!mDoingMultiChange) {
        int position=GetItemPos(item);
        if(position>=0 && mSortType!=2) {
            LineSortPred less(this,GetSortFunc(),mSortType);
            ListItem* prev=GetItem(position-1);
            ListItem* next=GetItem(position+1);
            bool sort=false;
            if(prev!=NULL && less(item,prev)) sort=true;
            else if(next!=NULL && less(next,item)) sort=true;
            if(sort) {
                SmartPtr<ListItem> hold=item;
                mItems[position]=prev!=NULL ? prev : next;
                int insert=FindInsertPos(item,0);
                if(position<insert) --insert;
                if(insert>=(int)mItems.size()) insert=mItems.size()-1;
                if(insert<0) insert=0;
                if(position>insert) {
                    for(int i=position;i>insert;--i) mItems[i]=mItems[i-1];
                } else {
                    for(int i=position;i<insert;++i) mItems[i]=mItems[i+1];
                }
                mItems[insert]=item;
            }
        }
        if(position>=0) {
            if(!ListFlagSet(0x1000) && (mVertOffset>=item->mTop || OnBottom()) && mTotalHeight>mScrollArea.height)
                mVertOffset+=item->mHeight-oldHeight;
            EnforceInvariants();
        }
    }
}
// FUNCTION: WON 0x006fca50
void ListArea::DeleteItemPrv(int position)
{
    ListItem* item=mItems[position].get();
    item->Virtual0C(this,NULL);
    if(!ListFlagSet(0x1000) && mVertOffset>=item->mTop) mVertOffset-=item->mHeight;
    if(item==mSelItem) { item->mB08=false;SetSelItem(-1,true); }
    if(item==mKeyItem) mKeyItem=NULL;
    if(item==mMouseItem) NotifyMouseItemGone();
}
// Native17B range forwarding support0.
// FUNCTION: WON 0x006fcae0
void ListArea::DeleteItem(int position) { DeleteRange(position,position+1); }
// Native29B lookup/delete support0.
// FUNCTION: WON 0x006fcb00
void ListArea::RemoveItem(ListItem* item)
{
    int position=GetItemPos(item);
    if(position>=0) DeleteItem(position);
}
// FUNCTION: WON 0x006fcb20
void ListArea::DeleteRange(int start,int end)
{
    if(end<0 || (unsigned int)end>mItems.size()) end=mItems.size();
    if(start<0 || end<=start) return;
    int amount=end-start;
    if((unsigned int)(start+amount)>mItems.size()) amount=mItems.size()-start;
    for(int i=0;i<amount;++i) DeleteItemPrv(start+i);
    if(mSelItemItr>=start) mSelItemItr-=amount;
    std::vector<SmartPtr<ListItem> >::iterator first=mItems.begin()+start;
    mItems.erase(first,first+amount);
    EnforceInvariants();
}
}

// Build-only ListArea mouse hit/drag/key behavior; borrowed pointers, no allocation.
namespace WONAPI {
static bool ListMouseContains(ListItem* item,int x,int y,bool widthCheck) {
    if(!item->Virtual1C(x,y)) return false;
    if(widthCheck && x>item->mL10) return false;
    return true;
} //actual CheckContains2fb770, support0
// FUNCTION: WON 0x006fb6a0
void ListArea::ItemMouseAt(int &x, int &y)
{
	ScrollTranslate(x,y);
	bool itemWidthCheckOnSel = ListFlagSet(0x200);
	if(mMouseItem!=NULL && !ListMouseContains(mMouseItem,x,y,itemWidthCheckOnSel))
	{
		mMouseItem->Virtual24(this);
		Virtual78(Fn2F4B00());
		NotifyMouseItemGone();
	}

	if(mMouseItem==NULL)
	{
		mMouseItem = FindItemAt(y);
		if(mMouseItem!=NULL)
		{
			if(ListMouseContains(mMouseItem,x,y,itemWidthCheckOnSel))
				mMouseItem->Virtual20(this,x,y-mMouseItem->mTop);
			else
				mMouseItem = NULL;
		}
	}
}

// FUNCTION: WON 0x006fbc70
void ListArea::Virtual38(int x, int y)
{
	ScrollArea::Virtual38(x,y);


	if(ListFlagSet(2))
	{
		ItemMouseAt(x,y);
		CheckMouseSel(mMouseItem,x,y);
	}
	else
		ScrollTranslate(x,y);

	if(mMouseItem!=NULL)
		mMouseItem->Virtual2C(this,x,y-mMouseItem->mTop);
}

// FUNCTION: WON 0x006fc160
bool ListArea::Virtual54(int theKey)
{
	if(ScrollArea::Virtual54(theKey))
		return true;

	if(mKeyItem==NULL)
	{
		if(theKey==VK_DOWN || theKey==VK_UP || theKey==VK_NEXT || theKey==VK_PRIOR)
		{
			ListItem *anItem = FindItemAt(mVertOffset);
			if(anItem==NULL)
				return false;

			int aPos = GetItemPos(anItem);
			anItem = FindSelectableItem(aPos,theKey==VK_UP || theKey==VK_PRIOR);
			if(anItem!=NULL)
			{
				mKeyItem = anItem;
				anItem->mB08 = true;
				SetSelItem(anItem,false,true);
			}

			return true;
		}
		return false;
	}


	int y = mKeyItem->mTop;
	int height = mKeyItem->mHeight+1;
	int anItemPos = -1;

	switch(theKey)
	{
		case VK_RIGHT: mHorzOffset+=10; break;
		case VK_LEFT: mHorzOffset-=10; break;
		case VK_DOWN: anItemPos = y+height; break;
		case VK_UP: anItemPos = y-1; break;
		case VK_NEXT: anItemPos = y+mScrollArea.height; break;
		case VK_PRIOR: anItemPos = y+height-mScrollArea.height; break;
		default: return false;
	}

	bool up = anItemPos<y;
	if(ListFlagSet(0x800))
	{
		if(anItemPos<0)
		{
			anItemPos = mTotalHeight;
			up = true;
		}
		else if(anItemPos>=mTotalHeight)
		{
			anItemPos = 0;
			up = false;
		}
	}

	if(anItemPos!=-1)
	{
		std::vector<SmartPtr<ListItem> >::iterator anItr = FindLineAt(anItemPos);
		if(anItr==mItems.end())
			return false;

		ListItem *anItem = anItr->get();
		int aPos = GetItemPos(anItem);
		anItem = FindSelectableItem(aPos,up);

		if(anItem!=NULL)
		{
			if(mSelItem!=NULL && ListFlagSet(0x400) && GetRoot()->GetKeyMod()&1)
			{
				SelectRange(GetItemPos(mSelItem),GetItemPos(anItem));
			}
			else
			{
				anItem->mB08 = true;
				SetSelItem(anItem,false,true);
			}

			mKeyItem = anItem;
			if(anItem->mTop+anItem->mHeight > mVertOffset+mScrollArea.height)
				mVertOffset = anItem->mTop+anItem->mHeight-mScrollArea.height + mExtraHeight;
			if(anItem->mTop < mVertOffset)
				mVertOffset = anItem->mTop;
		}
	}

	VirtualCC(false);
	return true;
}

}

namespace WONAPI {
// FUNCTION: WON 0x006fb600
void ListArea::Fn2FB600(unsigned long flags,bool on) {
    bool over=ListFlagSet(4);
    if(on) mListFlags|=flags;else mListFlags&=~flags;
    if(!over && ListFlagSet(4)) {
        Window* window=GetRoot();
        if(window==NULL) return;
        int x=window->mMouseX,y=window->mMouseY;
        Container* parent=mParent;
        if(parent!=NULL) {
            parent->RootToChild(x,y);
            if(!Contains(x,y)) ClearSelection();
        }
    }
}
// FUNCTION: WON 0x006fbad0
void ListArea::Virtual34(int x,int y) {
    ScrollArea::Virtual34(x,y);
    ItemMouseAt(x,y);
    if(ListFlagSet(2)) CheckMouseSel(mMouseItem,x,y);
    if(mMouseItem!=NULL) mMouseItem->Virtual28(this,x,y-mMouseItem->mTop);
    if(ListFlagSet(0x2000)) {
        bool show=false;
        if(mMouseItem!=NULL && mMouseItem->mL10>mScrollArea.width) {
            int half=mMouseItem->mHeight>>1;
            if(mMouseItem->mTop+half>mVertOffset && mMouseItem->mTop+half<mVertOffset+mScrollArea.height) show=true;
        }
        if(show) {
            if(mItemToolTip.get()==NULL) mItemToolTip=new ListAreaToolTip(this);
            mItemToolTip->DoTip(mMouseItem);
        } else if(mItemToolTip.get()!=NULL) mItemToolTip->DoTip(NULL);
    }
}
// FUNCTION: WON 0x006fbd00
void ListArea::Virtual2C(int x,int y,int button) {
    ScrollArea::Virtual2C(x,y,button);
    ItemMouseAt(x,y);
    if(button!=1) {
        Fn2F5890();
        CheckMouseSel(mMouseItem,x,y,true);
    }
    if(mMouseItem!=NULL) {
        mMouseItem->Virtual30(this,x,y-mMouseItem->mTop,button);
        Fn2F53F0(new ListItemClickedEvent(this,mMouseItem,static_cast<MouseButton>(button),CheckDoubleClick(button),false,x,y-mMouseItem->mTop),false);
    }
}
// FUNCTION: WON 0x006fbef0
void ListArea::Virtual30(int x,int y,int button) {
    ScrollArea::Virtual30(x,y,button);
    ItemMouseAt(x,y);
    if(mMouseItem!=NULL) {
        mMouseItem->Virtual34(this,x,y-mMouseItem->mTop,button);
        Fn2F53F0(new ListItemClickedEvent(this,mMouseItem,static_cast<MouseButton>(button),CheckDoubleClick(button),true,x,y-mMouseItem->mTop),false);
    }
}
// FUNCTION: WON 0x006fc010
void ListArea::Virtual24(int x,int y) {
    ScrollArea::Virtual24(x,y);
    ItemMouseAt(x,y);
}
// FUNCTION: WON 0x006fc040
void ListArea::Virtual28() {
    ScrollArea::Virtual28();
    if(mMouseItem!=NULL) {
        mMouseItem->Virtual24(this);
        NotifyMouseItemGone();
    }
    if(ListFlagSet(4)) ClearSelection();
    if(ListFlagSet(0x2000) && mItemToolTip.get()!=NULL) mItemToolTip->DoTip(NULL);
}
}

namespace WONAPI {
// Native2fa570..2fa76c508B: search-item/font creation and list focus/layout setup.
// FUNCTION: WON 0x006fa570
ListArea::ListArea()
{
    mSearchLine=new ListItem;
    mTopLineVectorPos=0;
    mListFlags=0xb0;
    mSortType=ListSortType_None;
    mTextColor=(int)0x8000000e;
    mFont=GetNamedFont("ListArea");
    mSelItem=NULL;
    mKeyItem=NULL;
    mMouseItem=NULL;
    SetComponentFlags(8,true);
    mTopPad=mLeftPad=mRightPad=mBottomPad=2;
    mL12C=-1;
    mDoingMultiChange=0;
    mSortFunc=NULL;
}
// Native2fa860..2fa9fd413B: end live tooltip before natural owning-member cleanup,0quota.
// FUNCTION: WON 0x006fa860
ListArea::~ListArea()
{
    if(mItemToolTip.get()!=NULL) mItemToolTip->EndTip();
}
}

// Genuine SDK ListBox.cpp explicit selected-item query; no ownership changes.
// FUNCTION: WON 0x006fa550
bool WONAPI::ListItemClickedEvent::IsSelectedItem()
{
    return mItem.get()==((ListArea*)mComponent)->GetSelItem();
}

namespace WONAPI {

void ListArea::ListSelChangedHook()
{
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fa770
// (public: virtual void * __thiscall WONAPI::ListArea::`scalar deleting destructor'(unsigned int)    [??_GListArea@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006fa790
// (public: virtual __thiscall WONAPI::ScrollArea::~ScrollArea(void)    [??1ScrollArea@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006fbe40
// (public: virtual void * __thiscall WONAPI::ListItemClickedEvent::`scalar deleting destructor'(unsigned int)    [??_GListItemClickedEvent@WONAPI@@UAEPAX)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fab30
// (public: static int __cdecl WONAPI::ListArea::DefaultSortFunc(class WONAPI::ListArea *,class WONAPI::ListItem const *,class WONAPI::ListItem const *)  )

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fb770
// (bool __cdecl WONAPI::ListMouseContains(class WONAPI::ListItem *,int,int,bool)    [?ListMouseContains@WONAPI@@YA_NPAVListItem@1@HH_N@Z])
// TEMPLATE: WON 0x0071d2b0
// (public: __thiscall std::_Ptrit<class WONAPI::SmartPtr<class WONAPI::ListItem>,int,class WONAPI::SmartPtr<class WONAPI::ListItem> *,class WONAPI::Smart)
// TEMPLATE: WON 0x006fbe60
// (ListItemClickedEvent implicit destructor)
