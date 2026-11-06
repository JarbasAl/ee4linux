// HTML document background, colors and body-margin setup.
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
// FUNCTION: WON 0x00738d30
void HTMLDisplayGen::HandleBody(HTMLBodyTag* tag)
{
    if(tag->IsEnd())
    {
        if(mBottomMargin>0)
            AddNonDisplaySegment(new LineBreakSegment(mBottomMargin),LineSegmentFlag_PreLineBreak|LineSegmentFlag_PostLineBreak);
        mDone=true;
        return;
    }
    if(mDisplayContainer.get()!=NULL)
    {
        if(mNoBackground)
            mDisplayContainer->mBackground=Background(-1);
        else if(tag->mBGImage.get()!=NULL)
        {
            mDisplayContainer->mBackground=Background(tag->mBGImage.get());
            if(tag->mTextColor<0) mDefaultTextColor=0;
            if(tag->mLinkColor<0) mDefaultLinkColor=0x0000ff;
            if(tag->mALinkColor<0) mDefaultALinkColor=0xff0000;
        }
        else if(tag->mBGColor<0)
            mDisplayContainer->mBackground=Background();
        else
            mDisplayContainer->mBackground=Background(tag->mBGColor);
    }
    if(tag->mTextColor>=0) mDefaultTextColor=tag->mTextColor;
    if(tag->mLinkColor>=0) mDefaultLinkColor=tag->mLinkColor;
    if(tag->mALinkColor>=0) mDefaultALinkColor=tag->mALinkColor;
    mFontStack.clear();
    PushFont(mDefaultFontFace.c_str(),3,-1);
    mLeftMargin=tag->mLeftMargin;
    mRightMargin=tag->mRightMargin;
    mTopMargin=tag->mTopMargin;
    mBottomMargin=tag->mBottomMargin;
    if(mLeftMargin>0 || mRightMargin>0)
        AddNonDisplaySegment(new MarginLineSegment(mLeftMargin,mRightMargin));
    if(mTopMargin>0)
        AddNonDisplaySegment(new LineBreakSegment(mTopMargin),LineSegmentFlag_PostLineBreak);
}
// FUNCTION: WON 0x00739710
// (??0LineSegment@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x00739d50
void HTMLDisplayGen::HandleLineBreak(HTMLLineBreakTag* tag)
{
    int flags=0;
    if(tag->mClearLeft) flags|=LineSegmentFlag_BreakClearLeft;
    if(tag->mClearRight) flags|=LineSegmentFlag_BreakClearRight;
    AddLineBreak(flags);
}
}

// FUNCTION: WON 0x0074af60
// (??4?$SmartPtr@VComponent@WONAPI@@@WONAPI@@QAEAAV01@ABV01@@Z)
