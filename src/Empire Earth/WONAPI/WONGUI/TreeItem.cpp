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
static void DrawHorzDottedLine(Graphics &g, int x, int y, int length)
{
	DWORD aColor = g.mColor;
	int i=0;
	if(x&1)
		i++;

	for(; i<length; i+=2)
		g.SetPixel(x+i,y,aColor);
}

static void DrawVertDottedLine(Graphics &g, int x, int y, int length)
{
	DWORD aColor = g.mColor;
	int i=0;
	if(y&1)
		i++;

	for(; i<length; i+=2)
		g.SetPixel(x,y+i,aColor);
}

// FUNCTION: WON 0x00714210
ListItem* TreeItem::GetMouseItem(ListArea *theListArea, int &x, int &y)
{
	TreeArea *aTree = (TreeArea*)theListArea;
	ListItem *aNewMouseItem = mItem.get();
	ListItem *aMouseColItem = aTree->mMouseColItem;
	if(aMouseColItem!=NULL && (aMouseColItem!=aNewMouseItem || !aMouseColItem->Virtual1C(x,y+mTop)))
	{
		aMouseColItem->Virtual24(theListArea);
		theListArea->Virtual78(NULL);

		aTree->mMouseColItem = NULL;
	}


	if(aMouseColItem==NULL)
	{
		if(aNewMouseItem!=NULL && aNewMouseItem->Virtual1C(x,y+mTop))
		{
			aTree->mMouseColItem = aNewMouseItem;
			aNewMouseItem->Virtual20(theListArea,x,y);
		}
	}

	x-=mX;
	return aTree->mMouseColItem;

}

// Native3142c0 support0.
// FUNCTION: WON 0x007142c0
void TreeItem::Virtual20(ListArea *theListArea, int x, int y)
{
	GetMouseItem(theListArea,x,y);
}

// Native3142e0 support0.
// FUNCTION: WON 0x007142e0
void TreeItem::Virtual24(ListArea *theListArea)
{
	ListItem *anItem = ((TreeArea*)theListArea)->mMouseColItem;
	if(anItem!=NULL)
	{
		anItem->Virtual24(theListArea);
		((TreeArea*)theListArea)->mMouseItem = NULL;
	}
}

// Native314310 support0.
// FUNCTION: WON 0x00714310
void TreeItem::Virtual28(ListArea *theListArea, int x, int y)
{
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->Virtual28(theListArea,x,y);
}

// Native314340 support0.
// FUNCTION: WON 0x00714340
void TreeItem::Virtual2C(ListArea *theListArea, int x, int y)
{
	TreeArea *aList = (TreeArea*)theListArea;
	ListItem *anItem = aList->mMouseColItem;
	if(anItem!=NULL)
		anItem->Virtual2C(theListArea,x-mX,y);
}

// Native314370 support0.
// FUNCTION: WON 0x00714370
void TreeItem::Virtual34(ListArea *theListArea, int x, int y, int theButton)
{	
	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->Virtual34(theListArea,x,y,theButton);
}

// FUNCTION: WON 0x007143b0
void TreeItem::Virtual30(ListArea *theListArea, int x, int y, int theButton)
{
	if(theButton==0)
	{
		TreeArea *aTreeArea = (TreeArea*)theListArea;
		if((x < mX && x > mX-14) || (theListArea->CheckDoubleClick(theButton) && aTreeArea->TreeFlagSet(8)))
		{
			ToggleOpen(aTreeArea, aTreeArea->TreeFlagSet(4));
			return;
		}
	}

	ListItem *anItem = GetMouseItem(theListArea,x,y);
	if(anItem!=NULL)
		anItem->Virtual30(theListArea,x,y,theButton);
}

// Native314440 support0.
// FUNCTION: WON 0x00714440
TreeItem* TreeItem::Fn314440(int thePos)
{
	if(thePos<0 || thePos>=mChildren.size())
		return NULL;

	return mChildren[thePos].get();
}

// Native314470 support0.
// FUNCTION: WON 0x00714470
TreeItem* TreeItem::GetNextNonChild()
{
	if(mParent==NULL)
		return NULL;
	else if(mIndex>=mParent->mChildren.size()-1)
		return mParent->GetNextNonChild();
	else
		return mParent->mChildren[mIndex+1].get();
}

// Native3144b0 support0.
// FUNCTION: WON 0x007144b0
int TreeItem::GetNextNonChildPos(TreeArea *theTreeArea)
{
	TreeItem *anItem = GetNextNonChild();
	if(anItem!=NULL)
		return theTreeArea->GetItemPos(anItem);
	else
		return -1;
}

// FUNCTION: WON 0x007144d0
int TreeItem::GetInsertPos(TreeArea* tree,TreeItem* item,int position) {
    if(tree->mTreeSortType==2) return position;
    ListArea::LineSortPred less(tree,tree->GetSortFunc(),tree->mTreeSortType);
    int first=0,count=mChildren.size();
    while(count>0) {
        int step=count/2,mid=first+step;
        if(less(mChildren[mid].get(),item)) { first=mid+1;count-=step+1; }
        else count=step;
    }
    return first;
}

// FUNCTION: WON 0x00714570
void TreeItem::InsertChild(TreeArea *theTreeArea, TreeItem *theItem, int thePos)
{
	thePos = GetInsertPos(theTreeArea,theItem,thePos);
	if(mParent==NULL && !theTreeArea->TreeFlagSet(2 | 1))
		theItem->mX = 0;
	else
		theItem->mX = mX + 15;

	if(thePos<0 || thePos>mChildren.size())
		thePos = mChildren.size();

	bool onEnd = thePos==mChildren.size();

	mChildren.insert(mChildren.begin() + thePos, SmartPtr<TreeItem>(theItem));
	theItem->mParent = this;

	for(int i=thePos; i<mChildren.size(); i++)
	{
		TreeItem *anItem = mChildren[i].get();
		anItem->mIndex = i;
	}

	if(IsOpen())
	{	
		int aPos;
		if(onEnd)
			aPos = GetNextNonChildPos(theTreeArea);
		else
			aPos = theTreeArea->GetItemPos(mChildren[thePos+1].get());

		theTreeArea->ListArea::InsertItem(theItem, aPos);
	}
}

// FUNCTION: WON 0x00714710
void TreeItem::RemoveAllChildren(TreeArea *theTreeArea)
{
	SetOpen(theTreeArea,false,false,false);
	mChildren.clear();
	theTreeArea->Invalidate();
}

// FUNCTION: WON 0x00714770
void TreeItem::RemoveChild(TreeArea *theTreeArea, TreeItem *theItem)
{
	theTreeArea->Fn2FAA00();

	if(theItem->IsOpen())
		theItem->SetOpen(theTreeArea, false,false,false);

	if(IsOpen())
		theTreeArea->ListArea::RemoveItem(theItem);

	mChildren.erase(mChildren.begin() + theItem->mIndex);
	for(int i=0; i<mChildren.size(); i++)
		mChildren[i]->mIndex = i;

	if(mChildren.empty())
		theTreeArea->Invalidate(); // need to do this in order to redraw the button (don't need button anymore)

	theTreeArea->Fn2FAA10();

}

// Native314830 support0.
// FUNCTION: WON 0x00714830
void TreeItem::RemoveFromParent(TreeArea *theTreeArea)
{
	if(mParent==NULL)
		return;

	mParent->RemoveChild(theTreeArea, this);	
}

// Native314850 support0.
// FUNCTION: WON 0x00714850
void TreeItem::CloseAll()
{
	mOpen = false;
	std::vector<SmartPtr<TreeItem> >::iterator anItr = mChildren.begin();
	while(anItr!=mChildren.end())
	{
		(*anItr)->CloseAll();
		++anItr;
	}
}

// FUNCTION: WON 0x00714880
void TreeItem::InsertChildren(TreeArea *theTreeArea, int thePos)
{
	for(int i=mChildren.size()-1; i>=0; i--)
	{
		TreeItem *anItem = mChildren[i].get();
		theTreeArea->ListArea::InsertItem(anItem,thePos);
		if(anItem->IsOpen())
			anItem->InsertChildren(theTreeArea, thePos+1);
	}
}

// FUNCTION: WON 0x007148e0
void TreeItem::SetOpen(TreeArea *theTreeArea, bool isOpen, bool closeAll, bool sendEvent)
{
	if(mOpen==isOpen)
		return;

	mOpen = isOpen;

	if(mChildren.empty())
		return;

	if(!isOpen)
	{
		int aPos1 = theTreeArea->GetItemPos(mChildren.front().get());
		int aPos2 = GetNextNonChildPos(theTreeArea);
		theTreeArea->DeleteRange(aPos1, aPos2);
		if(closeAll)
			CloseAll();
	}
	else
	{
		theTreeArea->Fn2FAA00();
		InsertChildren(theTreeArea, theTreeArea->GetItemPos(this)+1);
		theTreeArea->Fn2FAA10();

		int aSiblingYPos = theTreeArea->mTotalHeight;
		TreeItem *anItem = GetNextNonChild();
		if(anItem!=NULL)
			aSiblingYPos = anItem->mTop;

		int aVertOffset = theTreeArea->mVertOffset;
		if(aSiblingYPos > aVertOffset + theTreeArea->mScrollArea.height)
		{
			aVertOffset = aSiblingYPos - theTreeArea->mScrollArea.height;
			if(mTop < aVertOffset)
				aVertOffset = mTop;

			theTreeArea->SetVertOffset(aVertOffset);
		}		
	}

	if(sendEvent)
		theTreeArea->Fn2F53F0(new TreeItemOpenedEvent(theTreeArea,this),false);
}

// Native314af0 support0.
// FUNCTION: WON 0x00714af0
void TreeItem::ToggleOpen(TreeArea *theTreeArea, bool closeAll, bool sendEvent)
{
	SetOpen(theTreeArea, !mOpen, closeAll, sendEvent);
}

// Native314b10 support0.
// FUNCTION: WON 0x00714b10
bool TreeItem::IsLastChild()
{
	return mParent!=NULL && mIndex>=mParent->mChildren.size()-1;
}

// Native314b40 support0.
// FUNCTION: WON 0x00714b40
void TreeItem::Virtual18(int x, int y)
{
	ListItem::Virtual18(x,y);
	mItem->Virtual18(mX,y);
}

// FUNCTION: WON 0x00714b60
void TreeItem::Virtual04(Graphics &g, ListArea *theListArea)
{
	TreeArea *aTreeArea = (TreeArea*)theListArea;

	bool drawButton = aTreeArea->TreeFlagSet(2) && !mChildren.empty();

	int squareLeft		= mX - 14;
	int squareRight		= squareLeft+8;
	int squareTop		= (mHeight - 8)/2;
	int squareBottom	= squareTop+8;

	int lineTop = squareTop;
	int lineBottom = squareBottom;
	int lineLeft = squareRight;
	if(!drawButton)
	{
		lineTop = lineBottom = squareTop+4;
		lineLeft = squareLeft+6;
	}

	if(((TreeArea*)theListArea)->TreeFlagSet(1))
	{
		g.SetSysColor(3);
		if(mTop!=0)
			DrawVertDottedLine(g,squareLeft+4,0,lineTop+1);

		if(!IsLastChild())
			DrawVertDottedLine(g,squareLeft+4,lineBottom,mHeight-lineBottom);

		DrawHorzDottedLine(g,lineLeft,squareTop+4,mX-lineLeft);
		TreeItem *anItem = mParent;
		while(anItem!=NULL && anItem->mX > 0)
		{
			if(!anItem->IsLastChild())
				DrawVertDottedLine(g,anItem->mX-10,0,mHeight);

			anItem = anItem->mParent;
		}
	}

	if(drawButton)
	{
		g.SetSysColor(3);
		g.DrawRect(squareLeft,squareTop,9,9);
		g.SetSysColor(14);
		g.DrawLine(squareLeft+2, squareTop+4, squareRight-2, squareTop+4);
		if(!mOpen)
			g.DrawLine(squareLeft+4, squareTop+2, squareLeft+4, squareBottom-2);
	}

	g.Translate(mX,0);//mTop);
	mItem->mB08 = mB08;
	mItem->Virtual04(g, theListArea);
	g.Translate(-mX,0);//-mTop);
}

// Native314dd0 support0.
// FUNCTION: WON 0x00714dd0
void TreeItem::Virtual0C(ListArea *theListArea, Container *theViewport)
{
	mItem->Virtual0C(theListArea, theViewport);
}

// Native314df0 support0.
// FUNCTION: WON 0x00714df0
void TreeItem::Virtual08(ListArea *theListArea)
{
	mItem->Virtual08(theListArea);
	mL10 = mX+mItem->mL10;
	mHeight = mItem->mHeight;
}

// Native314e20 support0.
// FUNCTION: WON 0x00714e20
int TreeItem::Virtual10(const ListItem *theItem, ListArea *theListArea) const
{
	return mItem->Virtual10(((TreeItem*)theItem)->mItem.get(), theListArea);
}

// Native314e40 support0.
// FUNCTION: WON 0x00714e40
bool TreeItem::Virtual14(ListArea *theListArea, int x, int y)
{
	return mB09 && x>=mX;
}

// Native314e60 support0.
// FUNCTION: WON 0x00714e60
bool TreeItem::Fn314E60(int x, int y)
{
	if(x<mX)
		return false;

	return mItem->Virtual1C(x-mX,y+mTop);
}

}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK recursive tree sorting.
void TreeItem::Sort(TreeArea *theTreeArea, bool recursive)
{
	if(mChildren.empty())
		return;

	std::sort(mChildren.begin(), mChildren.end(), ListArea::LineSortPred(theTreeArea, theTreeArea->GetSortFunc(), theTreeArea->mTreeSortType));

	std::vector<SmartPtr<TreeItem> >::iterator anItr = mChildren.begin();
	int i=0;
	while(anItr!=mChildren.end())
	{
		TreeItem *anItem = anItr->get();
		anItem->mIndex = i++;
	
		if(recursive)
			anItem->Sort(theTreeArea,recursive);
		
		++anItr;
	}
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00714d30
// (void __cdecl WONAPI::DrawHorzDottedLine(class WONAPI::Graphics &,int,int,int)    [?DrawHorzDottedLine@WONAPI@@YAXAAVGraphics@1@HHH@Z])
// TEMPLATE: WON 0x00714d80
// (void __cdecl WONAPI::DrawVertDottedLine(class WONAPI::Graphics &,int,int,int)    [?DrawVertDottedLine@WONAPI@@YAXAAVGraphics@1@HHH@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00714a40
// (public: virtual void * __thiscall WONAPI::TreeItemOpenedEvent::`scalar deleting destructor'(unsigned int)    [??_GTreeItemOpenedEvent@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x00714a60
// (public: virtual __thiscall WONAPI::TreeItemOpenedEvent::~TreeItemOpenedEvent(void)    [??1TreeItemOpenedEvent@WONAPI@@UAE@XZ])
