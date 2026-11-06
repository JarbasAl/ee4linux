// Actual menu rendering/input on original-owned receivers; no construction or lifetime bodies.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyPopupEvent.h"

namespace WONAPI {
// FUNCTION: WON 0x006fcee0
void MenuItem::Virtual08(ListArea* area)
{
    TextListItem::Virtual08(area);
    mHeight+=4;
    mL10+=32;
}

// FUNCTION: WON 0x006fcf10
void MenuItem::Virtual04(Graphics& g,ListArea* area)
{
    int color=mL1C;
    if(mImage.get()!=NULL)
    {
        int y=(mHeight-mImage->mHeight)/2;
        if(y<0) y=0;
        g.DrawImage(mImage.get(),2,y);
    }
    if(mB08)
    {
        g.SetColor(area->mSelColor.GetBackgroundColor(g));
        int width=area->mL12C;
        if(!area->ListFlagSet(0x10)) width=mL10;
        int x=0;
        if(mImage.get()!=NULL) x=20;
        g.FillRect(x,0,width,mHeight);
        if(mImage.get()!=NULL)
        {
            g.SetSysColor(StandardColor_3DHilight);
            int boxHeight=mHeight;
            g.DrawRect(0,0,20,boxHeight);
            g.SetSysColor(StandardColor_3DShadow);
            g.DrawLine(19,0,19,boxHeight-1);
            g.DrawLine(0,boxHeight-1,boxHeight-1,boxHeight-1);
        }
        color=area->mSelColor.GetForegroundColor(g,mL1C);
    }
    g.SetColor(color);
    g.SetFont(mFont.get());
    int y=(mHeight-mFont->GetHeight())/2;
    if(mDisabled)
    {
        g.SetColor(0xffffff);
        if(!mB08)
        {
            g.DrawString(mText,23,y+1);
            g.SetColor(0x848484);
        }
        else g.SetColor(0xd0d0d0);
        g.DrawString(mText,22,y);
    }
    else g.DrawString(mText,22,y);
    if(mSubMenu.get()!=NULL)
    {
        int width=area->mScrollArea.width;
        int height=mHeight;
        bool disabled=mDisabled && !mB08;
        int x=width-8,y=height/2;
        for(int i=0;i<4;++i)
        {
            g.DrawLine(x+3-i,y-i,x+3-i,y+i);
            if(disabled && i<3)
            {
                g.SetColor(0xffffff);
                g.DrawLine(x+3-i,y+i+1,x+3-i,y+i+2);
                g.SetColor(0x848484);
            }
        }
        if(disabled) g.SetPixel(x+4,y+1,0xffffff);
    }
}

// FUNCTION: WON 0x006fd580
void PopupMenu::EndSubMenu()
{
    if(mCurSubMenu.get()!=NULL)
    {
        GetRoot()->ClosePopup(mCurSubMenu.get());
        if(!mKeyboardMode) Fn2FB600(4,true);
        mCurSubMenu=(PopupMenu*)NULL;
        mCurSubMenuItem=(ListItem*)NULL;
    }
}

// FUNCTION: WON 0x006fd7d0
void PopupMenu::HandleComponentEvent(ComponentEvent* event)
{
    if(event->mComponent==mCurSubMenu.get() && event->mType==ComponentEvent_ListSelChanged)
    {
        if(mCurSubMenu->mSelItem!=NULL)
            SetSelItem(GetItemPos(mCurSubMenuItem.get()));
    }
    else ScrollArea::HandleComponentEvent(event);
}

// FUNCTION: WON 0x006fd820
bool PopupMenu::Virtual44(int delta)
{
    if(mKeyboardMode) return false;
    mTimerAcc+=delta;
    if(mTimerAcc>=350)
    {
        CheckSubMenu();
        return false;
    }
    return true;
}

// FUNCTION: WON 0x006fd860
void PopupMenu::ListSelChangedHook()
{
    mTimerAcc=0;
    if(!mKeyboardMode)
    {
        if(mCurSubMenu.get()!=NULL && mSelItem!=mCurSubMenuItem.get())
            mCurSubMenu->SetSelItem(-1);
        Fn2F57D0(true);
    }
}

// FUNCTION: WON 0x006fdb10
void PopupMenu::Virtual34(int x,int y)
{
    if(mKeyboardMode)
    {
        if(x==mLastX && y==mLastY) return;
        mKeyboardMode=false;
        Fn2FB600(4,mCurSubMenu.get()==NULL);
    }
    mLastX=x;
    mLastY=y;
    ListArea::Virtual34(x,y);
}

// FUNCTION: WON 0x006fdc10
void PopupMenu::Virtual30(int x,int y,int button)
{
    if(button==0)
    {
        ++mMouseUpCount;
        if(mMouseUpCount<2) return;
    }
    ListArea::Virtual30(x,y,button);
}
// Additional true popup behavior; append inside WONAPI after the reviewed first eight.
// Required standalone true DoPopupEvent3c contract in H_MENU_EVENT_APPLY.json.
// FUNCTION: WON 0x006fd610
void PopupMenu::CheckSubMenu()
{
    mTimerAcc=0;
    MenuItem* item=dynamic_cast<MenuItem*>(mSelItem);
    if(item==NULL)
    {
        if(mCurSubMenu.get()!=NULL) EndSubMenu();
        return;
    }
    PopupMenu* submenu=item->mSubMenu.get();
    if(mCurSubMenu.get()==submenu) return;
    EndSubMenu();
    if(item->mSubMenu.get()!=NULL && !item->mDisabled)
    {
        Fn2FB600(4,false);
        mCurSubMenu=submenu;
        mCurSubMenuItem=item;
        int x=mScrollArea.width-2,y=item->mTop;
        ScrollUntranslate(x,y);
        Fn2F56D0(x,y);
        submenu->Reset();
        PopupParams params(submenu,0x33c7,this,x,y);
        params.mAvoidRect=WONRectangle(x-mScrollArea.width+2,0,mScrollArea.width-2,0);
        GetRoot()->Virtual48(new DoPopupEvent(params));
    }
}

// FUNCTION: WON 0x006fdb70
void PopupMenu::Virtual2C(int x,int y,int button)
{
    if(button==0)
    {
        mMouseUpCount=0;
        Window* window=GetRoot();
        Fn2F56D0(x,y);
        window->Virtual48(new MouseEvent(InputEvent_MouseUp,x,y,window->GetKeyMod(),button));
        return;
    }
    ListArea::Virtual2C(x,y,button);
}

}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine recursive submenu keyboard policy, separate from TimerEvent.
bool PopupMenu::TryKeyDown(int theKey)
{
	mKeyboardMode = true;
	if(mCurSubMenu.get()!=NULL)
	{
		if(mCurSubMenu->TryKeyDown(theKey))
			return true;
		else if(theKey==VK_LEFT || theKey==VK_ESCAPE)
		{
			EndSubMenu();
			return true;
		}
		else if(theKey==VK_RIGHT && mCurSubMenu->GetSelItem()==NULL)
		{
			mCurSubMenu->SetSelItem(0);
			return true;
		}
		else
			return false;
	}
	else if(theKey==VK_RIGHT)
	{
		CheckSubMenu();
		if(mCurSubMenu.get()!=NULL)
		{
			mCurSubMenu->SetSelItem(0);
			return true;
		}
		else
			return false;
	}
	else if(theKey==VK_UP || theKey==VK_DOWN)
	{
		ListArea::Virtual54(theKey);
		return true;
	}
	else
		return false;
}
}
