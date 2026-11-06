// HTML static/animated images, image links and floating alignment.
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
#include "ee/wonapi/WONLobbyAnimationComponent.h"
namespace WONAPI {
// FUNCTION: WON 0x0073a540
void HTMLDisplayGen::HandleImage(HTMLImageTag* tag)
{
    if(tag->IsEnd()) return;
    if(tag->mAnimation.get()!=NULL)
    {
        SmartPtr<AnimationComponent> animation=new AnimationComponent(tag->mAnimation.get());
        animation->Start();
        AddDisplaySegment(new ComponentLineSegment(animation.get()));
        mCanBreak=false;
        return;
    }
    else if(tag->mImage.get()==NULL) return;
    Image* image=tag->mImage.get();
    SmartPtr<ImageLineSegment> segment;
    ImageMap* map=tag->mImageMap.get();
    int border=mInAnchor?2:0;
    int borderColor=mCurColor;
    if(mInAnchor)
        segment=new ImageLinkLineSegment(mCurLinkParam.get(),mCurColor,mDefaultALinkColor,image);
    else if(map!=NULL)
    {
        border=2;
        borderColor=mDefaultLinkColor;
        segment=new ImageLinkLineSegment(NULL,mCurColor,mDefaultALinkColor,image);
        static_cast<ImageLinkLineSegment*>(segment.get())->mImageMap=map;
    }
    else segment=new ImageLineSegment(image);
    if(tag->mBorder>=0) border=tag->mBorder;
    int flags=0;
    int align=0;
    switch(tag->mAlign)
    {
    case HTMLAlign_Left: flags|=LineSegmentFlag_FloatLeft; break;
    case HTMLAlign_Right: flags|=LineSegmentFlag_FloatRight; break;
    case HTMLAlign_Middle: align=1; break;
    case HTMLAlign_Top: align=2; break;
    }
    segment->mHSpace=tag->mHSpace;
    segment->mVSpace=tag->mVSpace;
    segment->mAlign=(char)align;
    segment->mBorder=border;
    segment->mBorderColor=borderColor;
    ClearAccumulatedSegment();
    AddDisplaySegment(segment.get(),flags);
    if(flags==0)
    {
        mCanBreak=false;
    }
}
}

// FUNCTION: WON 0x0073a9b0
// (??_GComponentLineSegment@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073a9d0
// (??1ComponentLineSegment@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x0073aaf0
// (??1ImageLineSegment@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x0073ab50
// (??_GImageLinkLineSegment@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073ab70
// (??1ImageLinkLineSegment@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x0074afc0
// (??0?$SmartPtr@VImageMap@WONAPI@@@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x0074afd0
// (?push_back@?$list@PAVLineSegment@WONAPI@@V?$allocator@PAVLineSegment@WONAPI@@@std@@@std@@QAEXABQAVLineSegment@WONAPI@@@Z)
