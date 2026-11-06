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
// Native315030 support0.
// FUNCTION: WON 0x00715030
void TreeArea::SetSortType(ListSortType theType)
{
	if(mTreeSortType==theType)
		return;

	mTreeSortType = theType;
	Sort();
}

// Native315060 support0.
// FUNCTION: WON 0x00715060
void TreeArea::NotifyMouseItemGone()
{
	ListArea::NotifyMouseItemGone();
	mMouseColItem = NULL;
}

// FUNCTION: WON 0x00715080
TreeItem* TreeArea::Fn315080(TreeItem *theParent, ListItem *theItem, int thePos)
{
	if(theParent==NULL)
		theParent = mRoot.get();

	SmartPtr<TreeItem> anItem = new TreeItem;
	anItem->mItem = theItem;
	anItem->mB09 = theItem->mB09;
	theParent->InsertChild(this, anItem.get(),thePos);
	return anItem.get();
}

// Native315190 support0.
// FUNCTION: WON 0x00715190
void TreeArea::Fn315190(TreeItem *theItem)
{
	theItem->RemoveFromParent(this);
}

// Native3151a0 support0.
// FUNCTION: WON 0x007151a0
void TreeArea::Fn3151A0(bool fireEventOnChange)
{
	Fn3151B0(NULL);
}

// FUNCTION: WON 0x007151b0
void TreeArea::Fn3151B0(TreeItem *theParent)
{
	if(theParent==NULL)
	{
		ListArea::Fn2FAA50(true);
		mRoot->mChildren.clear();
	}
	else
		theParent->RemoveAllChildren(this);
}

// Native315220 support0.
// FUNCTION: WON 0x00715220
void TreeArea::Fn315220(TreeItem *theItem, bool sendEvent)
{
	theItem->SetOpen(this, true, false, sendEvent);
}

// Native315240 support0.
// FUNCTION: WON 0x00715240
void TreeArea::ToggleItem(TreeItem *theItem, bool closeAll, bool sendEvent)
{
	theItem->ToggleOpen(this, closeAll, sendEvent);
}

// Native315260 support0.
// FUNCTION: WON 0x00715260
void TreeArea::Virtual2C(int x, int y, int theButton)
{
	ListArea::Virtual2C(x,y,theButton);	
}

// FUNCTION: WON 0x00715280
void TreeArea::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==15 && theEvent->mComponent->mControlId==100003)
	{
			TreeItem* anItem = (TreeItem*)FindItemAt(theEvent->mComponent->Top());
			if(anItem!=NULL)
				ToggleItem(anItem,TreeFlagSet(4));
	}
	else
		ListArea::HandleComponentEvent(theEvent);

}

bool TreeArea::Sort() { return false; } //native315050 support0
}

namespace WONAPI {
// Native314ea0..314f9f255B: owned root creation/open and tree/list flags.
// FUNCTION: WON 0x00714ea0
TreeArea::TreeArea()
{
    mRoot=new TreeItem;
    mRoot->SetOpen(this,true,false,false);
    Fn2FB600(0x30,false);
    Fn2FB600(0x200,true);
    mTreeFlags=3;
    mMouseColItem=NULL;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK tree update methods.
void TreeArea::Sort(TreeItem *theItem, bool recursive)
{
	if(mTreeSortType==ListSortType_None)
		return;

	if(theItem==NULL)
	{
		// Do Root
		ListArea::Fn2FAA50(false);
		mRoot->Sort(this,recursive);
		mRoot->InsertChildren(this,0);
		return;
	}
		
	bool isVisible = theItem->IsOpen() && GetItemPos(theItem)>=0;

	// if the item open and visible then remove the children, sort them and then reinsert them
	if(isVisible)
		theItem->SetOpen(this,false,false,false);

	theItem->Sort(this, recursive);

	if(isVisible)
		theItem->SetOpen(this,true,false,false);
}

TreeItem* TreeArea::InsertString(TreeItem *theParent, const GUIString &theStr, Image *theIcon, int thePos)
{
	SmartPtr<TextListItem> anItem = new TextListItem;
	anItem->mText = theStr;
	anItem->mL1C = mTextColor;
	anItem->mFont = mFont;	
	anItem->mImage = theIcon;
	return Fn315080(theParent, anItem.get(), thePos);
}

void TreeArea::CloseItem(TreeItem *theItem, bool closeAll, bool sendEvent)
{
	theItem->SetOpen(this, false, closeAll, sendEvent); 
}

void TreeArea::SetTreeFlags(int theFlags, bool on)
{
	if(on)
		mTreeFlags |= theFlags;
	else
		mTreeFlags &= ~theFlags;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00714fa0
// (public: virtual void * __thiscall WONAPI::TreeArea::`scalar deleting destructor'(unsigned int)    [??_GTreeArea@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00714fc0
// (public: virtual __thiscall WONAPI::TreeArea::~TreeArea(void)    [??1TreeArea@WONAPI@@UAE@XZ])
