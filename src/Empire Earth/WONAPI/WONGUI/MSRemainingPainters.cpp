// Actual standard MS control painting on original-owned receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyMSRemainingPainters.h"

namespace WONAPI {
// FUNCTION: WON 0x006ff000
void MSHeaderButton::Paint(Graphics& g)
{
    MSButton::Paint(g);
    ListSortType sortType=MultiListHeader::GetSortTypeForButton(this);
    if(sortType==ListSortType_None)
        return;
    g.SetSysColor(StandardColor_ButtonText);
    int x;
    if(mText->GetWidth()==0)
        x=(Width()-4)/2;
    else
        x=mText->GetWidth()+10+mLeftTextPad;
    int y=Height()/2-2;
    int offset=(mButtonFlags&0x20)!=0?1:0;
    x+=offset;
    y+=offset;
    if(sortType==ListSortType_Ascending)
    {
        for(int i=0;i<4;i++)
            g.DrawLine(x-i,y+i,x+i,y+i);
    }
    else if(sortType==ListSortType_Descending)
    {
        for(int i=0;i<4;i++)
            g.DrawLine(x-(3-i),y+i,x+(3-i),y+i);
    }
}

// FUNCTION: WON 0x006ff2a0
void MSCloseButton::Paint(Graphics& g)
{
    Button::Paint(g);
    bool down=(mButtonFlags&0x20)!=0;
    int offset=down?1:0;
    g.SetSysColor(StandardColor_3DFace);
    g.FillRect(0,0,Width(),Height());
    LobbyDraw3DFrame(g,0,0,Width(),Height(),down);
    MSGraphicsUtil::DrawCloseX(g,offset,offset,Width(),Height());
}

// FUNCTION: WON 0x006ff360
void MSScrollThumb::Paint(Graphics& g)
{
    Component::Paint(g);
    g.SetSysColor(StandardColor_3DFace);
    g.FillRect(0,0,Width(),Height());
    LobbyDrawFrame(g,0,0,Width(),Height(),false);
}

// FUNCTION: WON 0x006ff7d0
void MSLabel::Paint(Graphics& g)
{
    MSButtonBase::Paint(g);
    g.SetSysColor(StandardColor_3DFace);
    if(!mIsTransparent)
        g.FillRect(0,0,Width(),Height());
    DrawButtonText(g,0,0);
}
}
