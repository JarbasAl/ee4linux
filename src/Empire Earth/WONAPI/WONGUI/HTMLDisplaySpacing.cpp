// HTML line breaks, alignment stack and nested-list margin state.
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
namespace WONAPI {
// FUNCTION: WON 0x00738560
void HTMLDisplayGen::StartFreshLine()
{
    if(!mCurLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(0),LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x007385c0
void HTMLDisplayGen::StartFreshParagraph()
{
    if(!mCurLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(20),LineSegmentFlag_PreLineBreak|LineSegmentFlag_PostLineBreak);
    else if(!mPrevLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(20),LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x00738650
void HTMLDisplayGen::AddLineBreak(int extraFlags)
{
    if(!mCurLineEmpty)
        AddNonDisplaySegment(new LineBreakSegment(0),extraFlags|LineSegmentFlag_PostLineBreak);
    else
        AddNonDisplaySegment(new LineBreakSegment(20),extraFlags|LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x00738bd0
void HTMLDisplayGen::PushAlign(HTMLAlign align,bool force)
{
    HTMLAlign oldAlign=GetCurAlign();
    if(align==HTMLAlign_None) align=oldAlign;
    mAlignStack.push_back(align);
    if(oldAlign!=align || force)
        AddEmptySegment(new AlignLineSegment((HorzAlign)align),LineSegmentFlag_PreLineBreak);
}
// FUNCTION: WON 0x00738c70
// (??1MarginLineSegment@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x00738c80
void HTMLDisplayGen::PopAlign()
{
    if(mAlignStack.empty()) return;
    HTMLAlign oldAlign=GetCurAlign();
    mAlignStack.pop_back();
    HTMLAlign newAlign=GetCurAlign();
    if(newAlign!=oldAlign)
        AddEmptySegment(new AlignLineSegment((HorzAlign)newAlign),LineSegmentFlag_PreLineBreak);
}
// FUNCTION: WON 0x0073ac40
void HTMLDisplayGen::HandleList(HTMLListTag* tag)
{
    if(tag->IsEnd())
    {
        if(mListDepth<=0) return;
        --mListDepth;
        AddEmptySegment(new MarginLineSegment(mListDepth*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
        if(mListDepth==0) StartFreshParagraph();
    }
    else
    {
        ++mListDepth;
        if(mListDepth==1) StartFreshParagraph();
        AddEmptySegment(new MarginLineSegment(mListDepth*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
    }
}
}
