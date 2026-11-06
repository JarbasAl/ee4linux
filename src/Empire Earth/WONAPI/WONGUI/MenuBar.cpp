// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONLobbyPopupEvent.h"
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-snapshot MenuBar.cpp methods.
// Existing popup/window protocols retain their own inherited behavior and debts.
MenuBarItem::MenuBarItem()
{
	mState = MenuBarState_None;
	mFont = GetNamedFont("Menu");
}

void MenuBarItem::SetMenuBarState(MenuBarState theState)
{
	mState = theState;
	Invalidate();
}

void MenuBarItem::SetText(const GUIString &theText)
{
	mText = theText;
	Fn2F4B90(mFont->GetStringWidth(mText)+10,mFont->GetHeight()+4);
}

void MenuBarItem::SetPopup(PopupMenu *thePopup)
{
	mPopup = thePopup;	
}

void MenuBarItem::Paint(Graphics &g)
{
	Component::Paint(g);

	bool down = mState==MenuBarState_Down;
	bool over = mState==MenuBarState_Up;
	int offset = down?1:0;

	g.SetSysColor(StandardColor_3DFace);
	g.FillRect(0,0,Width(),Height());

	if(down || over)
	{
		g.SetColor(down?g.Fn326D20(StandardColor_3DShadow):g.Fn326D20(StandardColor_3DHilight));
		g.DrawLine(0,0,Width()-1,0);
		g.DrawLine(0,0,0,Height()-1);
	
		g.SetColor(over?g.Fn326D20(StandardColor_3DShadow):g.Fn326D20(StandardColor_3DHilight));
		g.DrawLine(0,Height()-1,Width()-1,Height()-1);
		g.DrawLine(Width()-1,0,Width()-1,Height()-1);
	}

	int x = 3;
	int y = (Height() - mFont->GetHeight())/2;
	g.SetFont(mFont);
	g.SetColor(g.Fn326D20(StandardColor_ButtonText));
	g.DrawString(mText,x,y);
}

void MenuItemSelectedEvent::Deliver()
{
	if(mItem->mListener.get()!=NULL)
		mItem->mListener->Virtual04(this);
	else
		ComponentEvent::Deliver();
}

MenuBar::MenuBar()
{
	mState = MenuBarState_None;
	mCurItem = NULL;
	mBackground.SetColor(ColorScheme::GetColorRef(StandardColor_3DFace));

	SetComponentFlags(8,true);
}

void MenuBar::AddSeperator(Component *theComponent)
{
	SmartPtr<MSSeperator> aSep;
	if(theComponent==NULL)
	{
		aSep = new MSSeperator(2);
		theComponent = aSep;
	}

	Fn2F6140(theComponent,0x8001,this,0,0,0,0,0);
	Fn2F6A60(theComponent);
}

void MenuBar::AddItem(const GUIString &theText, PopupMenu *thePopup)
{
	SmartPtr<MenuBarItem> anItem = new MenuBarItem;
	int anId = mItemVector.size();
	if(anId > 0)
	{
		MenuBarItem *aPrev = mItemVector.back();
		anItem->Fn2F4B70((aPrev->Left()+aPrev->Width())+1,aPrev->Top());
	}
	else
		anItem->Fn2F4B70(2,2);

	anItem->mControlId=anId;
	anItem->SetText(theText);
	if(thePopup==NULL)
		thePopup = new PopupMenu;

	anItem->SetPopup(thePopup);
	if(thePopup!=NULL)
		thePopup->AddListener(new ComponentListenerComp(this));

	mItemVector.push_back(anItem);
	Fn2F6A60(anItem);
}

void MenuBar::CheckItem(Component *theCheck)
{
	if(mState==MenuBarState_None || mState==MenuBarState_Up)
	{
		if(mCurItem!=NULL && theCheck!=mCurItem)
			mCurItem->SetMenuBarState(MenuBarState_None);

		if(theCheck==NULL)
			return;

		mCurItem = dynamic_cast<MenuBarItem*>(theCheck);
		if(mCurItem!=NULL)
			mCurItem->SetMenuBarState(MenuBarState_Up);
	}
	else if(mState==MenuBarState_Down)
	{
		MenuBarItem *anItem = dynamic_cast<MenuBarItem*>(theCheck);
		if(anItem!=NULL && anItem!=mCurItem)
		{
			MenuBarItem *anOldCurItem = mCurItem;
			mCurItem = anItem;
			if(anOldCurItem!=NULL)
			{
				anOldCurItem->SetMenuBarState(MenuBarState_None);
				if(anOldCurItem->GetPopup()!=NULL)
					GetRoot()->ClosePopup(anOldCurItem->GetPopup());
			}

			anItem->SetMenuBarState(MenuBarState_Down);
			PopupMenu *aPopup = mCurItem->GetPopup();
			if(aPopup!=NULL)
			{
				int x = mCurItem->Left();
				int y = (mCurItem->Top()+mCurItem->Height());
				Fn2F56D0(x,y);

//				SetComponentFlags(0x8000,true);
				int aFlags = PopupFlag_StandardPopup;
				aFlags &= ~PopupFlag_WantInput;
				aFlags &= ~PopupFlag_EndOnEscape;

				aPopup->Reset();
				PopupParams aParams(aPopup,aFlags,this,x,y);
				aParams.mInputComponent = this;
				aParams.mAvoidRect = WONRectangle(0,y-mCurItem->Height(),0,mCurItem->Height());
//				GetRoot()->DoPopup(aParams);
				GetRoot()->Virtual48(new DoPopupEvent(aParams));
			}
		}
	}

}

void MenuBar::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_EndPopup && mState==MenuBarState_Down)
	{
		if(mCurItem!=NULL && mCurItem->GetPopup()==theEvent->mComponent)
		{
			mState = MenuBarState_None;
			if(GetRoot()->mMouseIsInClientArea)
				MouseAt();
			else
				Virtual28();
		}		
	}
	else if(theEvent->mType==ComponentEvent_ListItemUpClicked)
	{
		ListItemClickedEvent *anEvent = (ListItemClickedEvent*)theEvent;
		MenuItem *anItem = dynamic_cast<MenuItem*>(anEvent->mItem.get());
		if(anItem!=NULL && anItem->mSubMenu.get()==NULL && !anItem->mDisabled)
		{
			Fn2F53F0(new MenuItemSelectedEvent(theEvent->mComponent,anItem),false);
			GetRoot()->ClosePopup(NULL);
		}
	}
	else
		Container::HandleComponentEvent(theEvent);
}

void MenuBar::Virtual34(int x, int y)
{
	Container::Virtual34(x,y);
	if(x!=mLastX || y!=mLastY)
	{
		mLastX = x;
		mLastY = y;
		CheckItem(mMouseChild);
	}
}

bool MenuBar::Virtual54(int theKey)
{
	if(mCurItem==NULL)
		return false;

	if(mState==MenuBarState_Down)
	{
		PopupMenu *aPopup = mCurItem->GetPopup();
		if(aPopup->TryKeyDown(theKey))
			return true;
	}
	else 
		mState = MenuBarState_Up;

	int anId = mCurItem->mControlId;

	if(theKey==VK_ESCAPE)
	{
		mState = MenuBarState_Up;
		if(mCurItem!=NULL)
		{
			GetRoot()->ClosePopup(mCurItem->GetPopup());
			CheckItem(mCurItem);
		}
	}
	else if(theKey==VK_RIGHT)
	{
		anId++;
		if(anId>=mItemVector.size())
			anId = 0;
	}
	else if(theKey==VK_LEFT)
	{
		anId--;
		if(anId<0)
			anId = mItemVector.size()-1;
	}
	else if(mState==MenuBarState_Up && (theKey==VK_RETURN || theKey==VK_DOWN))
	{
		mCurItem = NULL;
		mState = MenuBarState_Down;
	}

	CheckItem(mItemVector[anId]);
	if(mState==MenuBarState_Down && mCurItem!=NULL)
	{
		PopupMenu *aPopup = mCurItem->GetPopup();
		if(aPopup!=NULL)
			aPopup->SetSelItem(0);
	}

	return true;
}

void MenuBar::Virtual2C(int x, int y, int theButton)
{
	Container::Virtual2C(x,y,theButton);
	if(theButton!=0)
		return;

	Fn2F5890();
	if(dynamic_cast<MenuBarItem*>(mMouseChild)!=NULL)
	{
		if(mState==MenuBarState_Down)
		{
			mState = MenuBarState_Up;
			if(mCurItem!=NULL)
				GetRoot()->ClosePopup(mCurItem->GetPopup());
		}
		else
		{
			mCurItem = NULL;
			mState = MenuBarState_Down;
			Window *aWindow = GetRoot();
			Fn2F56D0(x,y);
			aWindow->Virtual48(new MouseEvent(InputEvent_MouseUp,x,y,aWindow->GetKeyMod(),theButton));
		}


		CheckItem(mMouseChild);
	}
}

void MenuBar::Virtual38(int x, int y)
{
	if(mState==MenuBarState_Down)
	{
		Fn2F6650(x,y);
		CheckItem(mMouseChild);
	}
	else
		Container::Virtual38(x,y);
}

void MenuBar::Virtual30(int x, int y, int theButton)
{
	Container::Virtual30(x,y,theButton);
	if(theButton!=0)
		return;
}

void MenuBar::Virtual28()
{
	Container::Virtual28();
	CheckItem(NULL);
}

void MenuBar::Paint(Graphics &g)
{
	if(mInvalidState>=2)
		mBackground.Paint(g,0,0,Width(),Height(),0,0);
	
	Container::Paint(g);
}
}
