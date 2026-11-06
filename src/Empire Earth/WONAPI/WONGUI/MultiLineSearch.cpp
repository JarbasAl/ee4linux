// Original multiline geometry search and inclusive floating-segment picking.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x0070d040
MultiLineElement::LineVector::iterator MultiLineElement::FindLineAt(int y)
{
    mSearchLine->my=y;
    LineVector::iterator line=mLineVector.begin();
    int count=(int)(mLineVector.end()-line);
    while(count>0)
    {
        int step=count/2;
        LineVector::iterator middle=line+step;
        if(middle->get()->my<mSearchLine->my)
        {
            line=middle+1;
            count-=step+1;
        }
        else count=step;
    }
    if(line==mLineVector.end() || line->get()->my>y)
    {
        if(line!=mLineVector.begin()) --line;
    }
    return line;
}
// FUNCTION: WON 0x0070d0c0
FloatingSegment* MultiLineElement::FindFloatingSegmentAt(int x,int y,bool left)
{
    FloatingVector* items=left?&mFloatingItems[0]:&mFloatingItems[1];
    FloatingVector::iterator item=items->begin();
    int count=(int)(items->end()-item);
    while(count>0)
    {
        int step=count/2;
        FloatingVector::iterator middle=item+step;
        if(middle->my<y)
        {
            item=middle+1;
            count-=step+1;
        }
        else count=step;
    }
    if(item==items->end() || item->my>y)
    {
        if(item!=items->begin()) --item;
    }
    if(item==items->end()) return NULL;
    FloatingSegment& segment=*item;
    if(segment.mx>x) return NULL;
    int width=segment.GetWidth();
    if(segment.mx+width<x) return NULL;
    int height=segment.GetHeight();
    if(segment.my+height<y) return NULL;
    return &segment;
}
}
