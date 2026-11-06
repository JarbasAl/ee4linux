// HTML list bullets, horizontal rules and embedded GUI-component segments.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyHTMLSegments.h"
#include "ee/wonapi/WONLobbyHTMLImageSegments.h"
#include "ee/wonapi/WONLobbyHTMLElementSegments.h"
namespace WONAPI {
// FUNCTION: WON 0x00739d80
void HTMLDisplayGen::HandleHorizontalRule(HTMLHorizontalRuleTag* tag)
{
    SmartPtr<HorizontalRuleSegment> segment=new HorizontalRuleSegment;
    segment->mHeight=tag->mSize;
    segment->mWidthSpec=tag->mWidthSpec;
    segment->mNoShade=tag->mNoShade;
    PushAlign(tag->mAlign);
    StartFreshLine();
    AddDisplaySegment(segment.get(),LineSegmentFlag_PostLineBreak);
    PopAlign();
}
// FUNCTION: WON 0x0073ad40
void HTMLDisplayGen::HandleListItem(HTMLListItemTag* tag)
{
    if(tag->IsEnd()) return;
    StartFreshLine();
    if(mListDepth<1)
        AddDisplaySegment(new BulletElement(mCurColor));
    else if(!tag->mBulletText.empty())
        AddDisplaySegment(new OrderedListElement(mCurColor,mCurFont.get(),tag->mBulletText));
    else
        AddDisplaySegment(new BulletElement(mCurColor,tag->mListType-HTMLListType_Disc+1));
    mCanBreak=false;
    mLastWasSpace=true;
    mCurLineEmpty=mPrevLineEmpty=true;
}
// FUNCTION: WON 0x0073af00
// (?GetAscent@OrderedListElement@WONAPI@@UAEHXZ)

// FUNCTION: WON 0x0073af10
// (??_GOrderedListElement@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073af30
// (??1OrderedListElement@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x0073b670
void HTMLDisplayGen::HandleGUIComponent(HTMLGUIComponentTag* tag)
{
    AddDisplaySegment(new ComponentLineSegment(tag->mComponent.get()));
    mCanBreak=false;
    mLastWasSpace=false;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0073aef0
// (public: virtual int __thiscall WONAPI::OrderedListElement::GetHeight(void))
