// Original multiline geometry search and inclusive floating-segment picking.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
struct LineYLess {
    bool operator()(const SmartPtr<LineElement>& a,const SmartPtr<LineElement>& b) const { return a->my<b->my; }
};
struct FloatingYLess {
    bool operator()(const FloatingSegment& a,const FloatingSegment& b) const { return a.my<b.my; }
};
// FUNCTION: WON 0x0070d040
MultiLineElement::LineVector::iterator MultiLineElement::FindLineAt(int y)
{
    mSearchLine->my=y;
    LineVector::iterator line=std::lower_bound(mLineVector.begin(),mLineVector.end(),mSearchLine,LineYLess());
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
    FloatingVector::iterator item=std::lower_bound(items->begin(),items->end(),FloatingSegment(NULL,x,y),FloatingYLess());
    if(item==items->end() || item->my>y)
    {
        if(item!=items->begin()) --item;
    }
    if(item==items->end()) return NULL;
    FloatingSegment& segment=*item;
    if(segment.mx>x || segment.mx+segment.GetWidth()<x || segment.my+segment.GetHeight()<y)
        return NULL;
    return &segment;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00723be0
// (class std::_Ptrit<class WONAPI::SmartPtr<class WONAPI::LineElement>,int,class WONAPI::SmartPtr<class WONAPI::LineElement> *,class WONAPI::SmartPtr<cla)
// TEMPLATE: WON 0x00723c20
// (class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::FloatingSegment )
// TEMPLATE: WON 0x007253f0
// (class std::_Ptrit<class WONAPI::SmartPtr<class WONAPI::LineElement>,int,class WONAPI::SmartPtr<class WONAPI::LineElement> *,class WONAPI::SmartPtr<cla)
// TEMPLATE: WON 0x00725480
// (class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::FloatingSegment )
// TEMPLATE: WON 0x00725df0
// (void __cdecl std::_Distance(class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,str)
// TEMPLATE: WON 0x00725f50
// (void __cdecl std::_Distance(class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,str)
