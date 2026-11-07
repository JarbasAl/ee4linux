// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x00704fd0
void ScrollArea::SetDelegatePos()
{
    mViewport->Fn2F4BF0(mScrollArea.x-mHorzOffset,mScrollArea.y-mVertOffset,mL0A4,mTotalHeight);
}
// FUNCTION: WON 0x00705030
void ScrollArea::VirtualCC(bool scrollbar)
{
    if(scrollbar) mUserHasScrolled=true;
    if(mVertOffset+mScrollArea.height>mTotalHeight+mExtraHeight)
        mVertOffset=mTotalHeight+mExtraHeight-mScrollArea.height;
    if(mVertOffset<0) mVertOffset=0;
    if(mHorzOffset+mScrollArea.width>mL0A4) mHorzOffset=mL0A4-mScrollArea.width;
    if(mHorzOffset<0) mHorzOffset=0;
    if(mScroller!=NULL) mScroller->NotifyScrollChange(scrollbar);
    mAVertOffset=mVertOffset;
    mAHorzOffset=mHorzOffset;
    mATotalHeight=mTotalHeight;
    mATotalWidth=mL0A4;
    mOldScrollAreaWidth=mScrollArea.width;
    mOldScrollAreaHeight=mScrollArea.height;
    SetDelegatePos();
    Invalidate();
}
// Native23B forwarding setter support0.
// FUNCTION: WON 0x00705130
void ScrollArea::SetVertOffset(int offset) { mVertOffset=offset; VirtualCC(false); }
// Native33B forwarding setter support0.
// FUNCTION: WON 0x00705150
void ScrollArea::SetOffsets(int horizontal,int vertical)
{
    mHorzOffset=horizontal;
    mVertOffset=vertical;
    VirtualCC(false);
}
// FUNCTION: WON 0x00705190
bool ScrollArea::OnBottom()
{
    return mVertOffset>0 && mVertOffset+mScrollArea.height>=mTotalHeight;
}
// FUNCTION: WON 0x007051c0
void ScrollArea::Virtual08()
{
    Container::Virtual08();
    bool bottom=OnBottom();
    mScrollArea.x=mLeftPad;
    mScrollArea.y=mTopPad;
    mScrollArea.width=Width()-mRightPad-mLeftPad;
    mScrollArea.height=Height()-mBottomPad-mTopPad;
    if(mScrollArea.Width()<0) mScrollArea.width=0;
    if(mScrollArea.Height()<0) mScrollArea.height=0;
    if(bottom) mVertOffset=mTotalHeight+mExtraHeight-mScrollArea.Height();
    VirtualD0();
    VirtualCC(false);
}
// FUNCTION: WON 0x00705270
void ScrollArea::SetBorderPadding(int left,int top,int right,int bottom)
{
    if(mLeftPad==left && mRightPad==right && mTopPad==top && mBottomPad==bottom) return;
    mLeftPad=left;mTopPad=top;mRightPad=right;mBottomPad=bottom;
    Virtual08();
}
// FUNCTION: WON 0x00705390
void ScrollArea::ScrollTranslate(int& x,int& y)
{
    x+=mHorzOffset-mScrollArea.x;
    y+=mVertOffset-mScrollArea.y;
}
// FUNCTION: WON 0x007053d0
void ScrollArea::ScrollUntranslate(int& x,int& y)
{
    x+=mScrollArea.x-mHorzOffset;
    y+=mScrollArea.y-mVertOffset;
}
// FUNCTION: WON 0x00705410
bool ScrollArea::Virtual3C(int amount)
{
    if(Container::Virtual3C(amount)) return true;
    mUserHasScrolled=true;
    mVertOffset-=amount;
    VirtualCC(false);
    return true;
}
// FUNCTION: WON 0x00705450
void ScrollArea::SetSizeEqualToTotal(int minWidth,int minHeight)
{
    int width=mL0A4,height=mTotalHeight;
    if(minWidth>width) width=minWidth;
    if(minHeight>height) height=minHeight;
    Fn2F4B90(width+mLeftPad+mRightPad,height+mTopPad+mBottomPad);
}
// FUNCTION: WON 0x007054a0
void ScrollArea::Virtual0C(int& width,int& height)
{
    if(mL0A4<300) width=mL0A4;
    else width=300;
    if(mTotalHeight<300) height=mTotalHeight;
    else height=300;
}
// Original80B rectangle predicate wrapper. Direct borrowed rectangle expression
// expands actual external349450 (edge inequalities intentionally asymmetric);support0.
// FUNCTION: WON 0x007054f0
bool ScrollArea::IsRectVisible(const WONRectangle& r)
{
    WONRectangle view=mScrollArea;
    view.x=mHorzOffset;
    view.y=mVertOffset;
    return r.Intersects(view);
}
}
namespace WONAPI {
// FUNCTION: WON 0x007052d0
void ScrollArea::Paint(Graphics &g)
{
	mBackground.Paint(g,0,0,Width(),Height(),mHorzOffset,mVertOffset);
	g.PushClipRect(mScrollArea.x,mScrollArea.y,mScrollArea.width,mScrollArea.height);

	Container::Paint(g);

	int dx = 0, dy = 0;
	ScrollTranslate(dx,dy);
	g.Translate(-dx,-dy);

	VirtualD4(g);

	g.Translate(dx,dy);

	g.PopClipRect();
}
}

namespace WONAPI {
// Native304df0..304f73: actualviewport creation/child ownership and scroll setup,387B.
// FUNCTION: WON 0x00704df0
ScrollArea::ScrollArea()
{
    mViewport=new ScrollAreaViewport(this);
    Fn2F6A60(mViewport.get());
    mUpScrollAmount=mDownScrollAmount=10;
    mLeftScrollAmount=mRightScrollAmount=10;
    mVertOffset=mHorzOffset=mTotalHeight=mL0A4=0;
    mAVertOffset=mAHorzOffset=mATotalHeight=mATotalWidth=0;
    mExtraHeight=0;
    mOldScrollAreaWidth=mOldScrollAreaHeight=0;
    mLeftPad=mRightPad=mTopPad=mBottomPad=0;
    mScroller=NULL;
    mUserHasScrolled=false;
}
}

namespace WONAPI {
// Genuine SDK ScrollPaint empty default, sharednative2fa500/3; primary/physicalnew0.
void ScrollArea::VirtualD4(Graphics&) {}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK scroll content dimensions.
void ScrollArea::SetScrollDimension(int theWidth, int theHeight)
{
	mL0A4 = theWidth;
	mTotalHeight = theHeight;
	VirtualCC(false);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK scroller selection and registration.
void ScrollArea::SetScrollbar(Scrollbar *theScrollbar)
{
	ScrollbarScroller *aScroller = dynamic_cast<ScrollbarScroller*>(mScroller);
	if(aScroller==NULL)
	{
		aScroller = new ScrollbarScroller;
		SetScroller(aScroller);
	}

	aScroller->SetScrollbar(theScrollbar);
}

void ScrollArea::SetScrollButton(Button *theButton, Direction theDirection)
{
	ButtonScroller *aScroller = dynamic_cast<ButtonScroller*>(mScroller);
	if(aScroller==NULL)
	{
		aScroller = new ButtonScroller;
		SetScroller(aScroller);
	}

	aScroller->SetButton(theButton, theDirection);
}
}

namespace WONAPI {
// Genuine SDK explicit out-of-line ScrollSizeChanged empty default; Source support only.
// SOURCE_ONLY_NO_NATIVE_ID: existing mapped slot/signature, zero Native/primary/physical credit.
void ScrollArea::VirtualD0()
{
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00704f80
// (public: virtual void * __thiscall WONAPI::ScrollArea::`scalar deleting destructor'(unsigned int)    [??_GScrollArea@WONAPI@@UAEPAXI@Z])
