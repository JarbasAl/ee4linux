// Complete native rectangle algorithms on the actual four-int POD.
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
namespace WONAPI {
// Native349300/16 support-only default initialization.
WONRectangle::WONRectangle() { x=y=width=height=0; }
// Native349310/32 support-only direct initialization.
WONRectangle::WONRectangle(int inX,int inY,int inWidth,int inHeight)
    :x(inX),y(inY),width(inWidth),height(inHeight) {}
// FUNCTION: WON 0x00749300
// (??0WONRectangle@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x00749310
// (??0WONRectangle@WONAPI@@QAE@HHHH@Z)

// FUNCTION: WON 0x00749330
void WONRectangle::DoIntersect(const WONRectangle& other)
{
    int right=x+width;
    int bottom=y+height;
    int otherRight=other.x+other.width;
    int otherBottom=other.y+other.height;
    if(other.x>x) x=other.x;
    if(other.y>y) y=other.y;
    if(otherRight<right) right=otherRight;
    if(otherBottom<bottom) bottom=otherBottom;
    width=right-x;
    height=bottom-y;
    if(width<0) width=0;
    if(height<0) height=0;
}
// FUNCTION: WON 0x007493b0
void WONRectangle::DoUnion(const WONRectangle& other)
{
    if(width==0 || height==0)
    {
        x=other.x; y=other.y; width=other.width; height=other.height;
        return;
    }
    int right=x+width;
    int bottom=y+height;
    int otherRight=other.x+other.width;
    int otherBottom=other.y+other.height;
    if(other.x<x) x=other.x;
    if(other.y<y) y=other.y;
    if(otherRight>right) right=otherRight;
    if(otherBottom>bottom) bottom=otherBottom;
    width=right-x;
    height=bottom-y;
}
// FUNCTION: WON 0x00749450
bool WONRectangle::Intersects(const WONRectangle& other) const
{
    return x+width>=other.x && x<other.x+other.width &&
           y+height>=other.y && y<other.y+other.height;
}
}
