// Actual horizontal-rule and list-marker behavior. Shared owner definitions belong to text_html.
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
#include "ee/wonapi/WONLobbyTextSegment.h"
#include "ee/wonapi/WONLobbyHTMLElementSegments.h"
namespace WONAPI {
// FUNCTION: WON 0x00709c10
void HorizontalRuleSegment::SetMaxWidth(int width)
{
    if(mWidthSpec<0)
        mWidth=(width*-mWidthSpec)/100;
    else if(mWidthSpec>0)
        mWidth=mWidthSpec;
    else
        mWidth=width;
    if(mWidth>width)
        mWidth=width;
}

// FUNCTION: WON 0x00709c50
void HorizontalRuleSegment::Paint(Graphics& graphics,int x,int y,ElementPaintInfo& info)
{
    DrawSelection1(graphics,x,y,info);
    graphics.SetColor(0x808080);
    if(mNoShade)
        graphics.FillRect(x,y+7,mWidth,mHeight);
    else {
        graphics.SetColor(0x848484);
        graphics.DrawLine(x,y+7,x+mWidth-1,y+7);
        graphics.DrawLine(x,y+7,x,y+7+mHeight-1);
        graphics.SetColor(0xd6d6d6);
        graphics.DrawLine(x+mWidth-1,y+7,x+mWidth-1,y+7+mHeight-1);
        graphics.DrawLine(x,y+7+mHeight-1,x+mWidth-1,y+7+mHeight-1);
    }
}

// FUNCTION: WON 0x00709d20
void HorizontalRuleSegment::GetSelText(GUIString& text,ElementSelectionInfo& info)
{
    if(mStartSelIndex<info.mStartSelSegment || mStartSelIndex>info.mEndSelSegment)
        return;
    text.append("-------------------------------------------------------------------------------\r\n");
}

// FUNCTION: WON 0x0070b470
void BulletElement::Paint(Graphics& graphics,int x,int y,ElementPaintInfo&)
{
    int drawY=y+1;
    int drawX=x;
    int type;
    if(mType>=0) {
        drawX-=mWidth+10;
        type=mType;
    }
    else
        type=-mType;
    graphics.SetColor(mColor);
    switch(type) {
    case 1: graphics.FillEllipse(drawX,drawY,mWidth,mHeight); break;
    case 2: graphics.DrawEllipse(drawX,drawY,mWidth+2,mHeight+2); break;
    case 3: graphics.FillRect(drawX,drawY,mWidth,mHeight); break;
    }
}

// FUNCTION: WON 0x0070b510
void OrderedListElement::Paint(Graphics& graphics,int x,int y,ElementPaintInfo&)
{
    graphics.SetColor(mColor);
    graphics.SetFont(mFont.get());
    int backWidth=mWidth+10;
    if(backWidth>40)
        backWidth=40;
    graphics.DrawString(mText,x-backWidth,y);
}
}
