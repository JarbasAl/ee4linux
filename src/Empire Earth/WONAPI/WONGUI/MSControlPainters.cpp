// Borrowed actual MS control render/layout methods; no allocation or lifetime implementation.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"

namespace WONAPI {
// FUNCTION: WON 0x006ffa70
void MSScrollbar::Paint(Graphics& g)
{
    if(mScrollRect->mInvalidState!=0 && !mIsTransparent)
    {
        g.SetSysColor(StandardColor_Scrollbar);
        g.FillRect(mScrollRect->Left(),mScrollRect->Top(),mScrollRect->Width(),mScrollRect->Height());
    }
    Container::Paint(g);
}

// FUNCTION: WON 0x006ffdf0
void MSComboBox::Paint(Graphics& g)
{
    ComboBox::Paint(g);
    if(mDrawFrame)
        MSGraphicsUtil::DrawFrame3(g,0,0,Width(),Height(),true);
}

// Genuine default desired-size getter2ffb00/23 support0.
void MSComboBox::Virtual0C(int& width,int& height)
{
    width=100;
    height=22;
}

// FUNCTION: WON 0x006ffe30
void MSComboBox::Virtual08()
{
    ComboBox::Virtual08();
    int topPad=0,bottomPad=0,rightPad=0,leftPad=0;
    if(mDrawFrame)
        topPad=bottomPad=rightPad=2;
    int inputHeight=Height();
    int buttonHeight=inputHeight-bottomPad-topPad;
    int buttonWidth=mButton->Width();
    if(!mButton->IsVisible()) buttonWidth=0;
    int inputWidth=Width()-buttonWidth-leftPad-rightPad;
    if(mButton->IsVisible())
        mButton->Fn2F4BF0(inputWidth+leftPad,topPad,buttonWidth,buttonHeight);
    mEdit->Fn2F4BF0(leftPad,0,inputWidth,inputHeight);
}

// FUNCTION: WON 0x00700290
void MSScrollContainer::Virtual08()
{
    Container::Virtual08();
    if(mScrollArea.get()==NULL) return;
    bool horzOn=mHorzScrollbar->IsVisible();
    bool vertOn=mVertScrollbar->IsVisible();
    int xoff=0,yoff=0;
    if(mDrawFrame && mDraw3DFrame) xoff=yoff=2;
    int scrollbarWidth=mVertScrollbar->Width();
    int scrollbarHeight=mHorzScrollbar->Height();
    int scrollWidth=Width()-xoff*2;
    if(vertOn) scrollWidth-=scrollbarWidth;
    int scrollHeight=Height()-yoff*2;
    if(horzOn) scrollHeight-=scrollbarHeight;
    if(vertOn)
        mVertScrollbar->Fn2F4BF0(Width()-scrollbarWidth-xoff,yoff,scrollbarWidth,scrollHeight);
    if(horzOn)
        mHorzScrollbar->Fn2F4BF0(xoff,Height()-scrollbarHeight-yoff,scrollWidth,scrollbarHeight);
    mScrollArea->Fn2F4BF0(xoff,mHeaderHeight+yoff,scrollWidth,scrollHeight-mHeaderHeight);
}

// FUNCTION: WON 0x00700390
void MSScrollContainer::Paint(Graphics& g)
{
    Container::Paint(g);
    if(mVertScrollbar->IsVisible() && mHorzScrollbar->IsVisible() && !mTransparent)
    {
        g.SetSysColor(StandardColor_Scrollbar);
        g.FillRect(mVertScrollbar->Left(),mVertScrollbar->Top()+mVertScrollbar->Height(),
                   mVertScrollbar->Width(),mHorzScrollbar->Height());
    }
    if(mDrawFrame)
    {
        if(mDraw3DFrame)
            LobbyDraw3DFrame(g,0,0,Width(),Height(),true);
        else
        {
            g.SetSysColor(StandardColor_3DDarkShadow);
            g.DrawRect(0,0,Width(),Height());
        }
    }
}
}
