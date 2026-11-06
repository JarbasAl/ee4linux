// PROVISIONAL advanced original child-layout algorithms; historical filename/grouping unproved.
// SOURCE ONLY; no runtime-substitution authority.
// Shared exact typed layout contracts; existing algorithm bodies unchanged by owner promotion.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// Original337370..3373ec: normalize missing edges using matching axis, then last supplied anchor.
// FUNCTION: WON 0x00737370
void PercentLayoutParam::SetBorders(Component* inLeft, Component* inTop, Component* inRight, Component* inBottom)
{
    left=inLeft; top=inTop; right=inRight; bottom=inBottom;
    Component* last=NULL;
    if(inLeft==NULL) left=inRight; else last=inLeft;
    if(inRight==NULL) right=left; else last=inRight;
    if(inTop==NULL) top=inBottom; else last=inTop;
    if(inBottom==NULL) bottom=top; else last=inBottom;
    if(left==NULL) left=last;
    if(right==NULL) right=last;
    if(top==NULL) top=last;
    if(bottom==NULL) bottom=last;
}
// Original337430..33757d: percentage position and size use independently selected padded anchor edges.
// FUNCTION: WON 0x00737430
void PercentLayoutParam::DoCalc(Component* target, Component* parent)
{
    int x=target->Left(), y=target->Top(), width=target->Width(), height=target->Height();
    if(px>=0 || pw>=0)
    {
        int leftEdge=left==parent?0:left->Left();
        int rightEdge=right==parent?parent->Width():right->Left()+right->Width();
        leftEdge+=lp;
        rightEdge-=rp;
        if(px>=0) x=leftEdge+(rightEdge-leftEdge)*px/100;
        if(pw>=0) width=(rightEdge-leftEdge)*pw/100;
    }
    if(py>=0 || ph>=0)
    {
        int topEdge=top==parent?0:top->Top();
        int bottomEdge=bottom==parent?parent->Height():bottom->Top()+bottom->Height();
        topEdge+=tp;
        bottomEdge-=bp;
        if(py>=0) y=topEdge+(bottomEdge-topEdge)*py/100;
        if(ph>=0) height=(bottomEdge-topEdge)*ph/100;
    }
    target->Fn2F4BF0(x,y,width,height);
}
// Original337800..3378c1: first measure requested maxima, then apply current axis flags to every child.
// FUNCTION: WON 0x00737800
void SameSizeLayout::Execute(Component*)
{
    int width=0,height=0;
    std::list<Component*>::iterator i;
    for(i=mComponentList.begin();i!=mComponentList.end();++i)
    {
        Component* child=*i;
        if(mHorz && child->Width()>width) width=child->Width();
        if(mVert && child->Height()>height) height=child->Height();
    }
    for(i=mComponentList.begin();i!=mComponentList.end();++i)
    {
        Component* child=*i;
        if(mHorz && mVert) child->Fn2F4B90(width,height);
        else if(mHorz) child->Fn2F4B90(width,child->Height());
        else if(mVert) child->Fn2F4B90(child->Width(),height);
    }
}
// Original3378d0..33791e.
// FUNCTION: WON 0x007378d0
void ClearRightLayout::Execute(Component*)
{
    int right=0;
    for(std::list<Component*>::iterator i=mComponentList.begin();i!=mComponentList.end();++i)
    {
        Component* child=*i;
        int r=child->Right();
        if(r>right) right=r;

    }
    mComponent->Fn2F4B70(right+mPad,mComponent->Top());
}
// Original337920..337a37: clamp leftover space, distribute quotient and the first remainder pixels.
// FUNCTION: WON 0x00737920
void SpaceEvenlyLayout::Execute(Component* parent)
{
    if(mComponentList.empty()) return;
    int total=0;
    std::list<Component*>::iterator i;
    for(i=mComponentList.begin();i!=mComponentList.end();++i)
        total+=mIsHorz?(*i)->Width():(*i)->Height();
    Component* box=mSpaceBox;
    int available=(mIsHorz?box->Width():box->Height())-mRightPad-mLeftPad-total;
    if(available<0) available=0;
    int spacing=0,remainder=0;
    if(mComponentList.size()>1)
    {
        spacing=available/(mComponentList.size()-1);
        remainder=available%(mComponentList.size()-1);
    }
    int position=mIsHorz?box->Left():box->Top();
    if(box==parent) position=0;
    position+=mLeftPad;
    for(i=mComponentList.begin();i!=mComponentList.end();++i)
    {
        Component* child=*i;
        if(mIsHorz) child->Fn2F4B70(position,child->Top());
        else child->Fn2F4B70(child->Left(),position);
        if(mIsHorz) position+=child->Width()+spacing;
        else position+=child->Height()+spacing;
        if(remainder>0) { ++position; --remainder; }
    }
}
// Original337a40..337b8b: directional displacement is clamped differently for parent and sibling anchors.
// FUNCTION: WON 0x00737a40
void NoOverlapLayout::Execute(Component* parent)
{
    int dx=0,dy=0;
    if(mRef==parent)
    {
        if(mType&0x800) { dx=mHorzPad-mTarget->Left(); if(dx<0) dx=0; }
        else if(mType&0x400) { const WONRectangle& b=mTarget->GetBounds(); dx=mRef->Width()-b.width+mHorzPad-b.x; if(dx>0) dx=0; }
        if(mType&0x100) { dy=mVertPad-mTarget->Top(); if(dy<0) dy=0; }
        else if(mType&0x200) { const WONRectangle& b=mTarget->GetBounds(); dy=mRef->Height()-b.height-b.y+mVertPad; if(dy>0) dy=0; }
    }
    else
    {
        if(mType&0x800) { const WONRectangle& b=mTarget->GetBounds(); dx=mRef->Left()-b.width+mHorzPad-b.x; if(dx>0) dx=0; }
        else if(mType&0x400) { dx=mRef->Right()-mTarget->Left()+mHorzPad; if(dx<0) dx=0; }
        if(mType&0x100) { const WONRectangle& b=mTarget->GetBounds(); dy=mRef->Top()-b.height-b.y+mVertPad; if(dy>0) dy=0; }
        else if(mType&0x200) { dy=mRef->Height()-mTarget->Top()+mRef->Top()+mVertPad; if(dy<0) dy=0; }
    }
    if(dx==0 && dy==0) return;
    mTarget->Fn2F4B70(mTarget->Left()+dx,mTarget->Top()+dy);
    for(std::list<Component*>::iterator i=mComponentList.begin();i!=mComponentList.end();++i)
    {
        Component* child=*i;
        child->Fn2F4B70(child->Left()+dx,child->Top()+dy);
    }
}
// Original337b90..337be2: grow target to at least maximum requested child dimensions plus padding.
// FUNCTION: WON 0x00737b90
void SizeAtLeastLayout::Execute(Component*)
{
    int width=0,height=0;
    for(std::list<Component*>::iterator i=mComponentList.begin();i!=mComponentList.end();++i)
    {
        Component* child=*i;
        if(child->Width()>width) width=child->Width();
        if(child->Height()>height) height=child->Height();
    }
    mTarget->Fn2F4BC0(width+mHorzPad,height+mVertPad);
}
}
#include "ee/wonapi/WONPercentLayout.h"

namespace WONAPI {
// FUNCTION: WON 0x007372c0
void PercentLayoutParam::Init(Component *left,Component *top,Component *right,Component *bottom,int px,int py,int pw,int ph,int lp,int tp,int rp,int bp)
{
    SetBorders(left,top,right,bottom);
    SetPercentages(px,py,pw,ph);
    SetPadding(lp,tp,rp,bp);
}
// FUNCTION: WON 0x007373f0
void PercentLayoutParam::SetPercentages(int thePercentX,int thePercentY,int thePercentWidth,int thePercentHeight)
{
    px=thePercentX;
    py=thePercentY;
    pw=thePercentWidth;
    ph=thePercentHeight;
}
// FUNCTION: WON 0x00737410
void PercentLayoutParam::SetPadding(int theLeftPad,int theTopPad,int theRightPad,int theBottomPad)
{
    lp=theLeftPad;
    tp=theTopPad;
    rp=theRightPad;
    bp=theBottomPad;
}
// FUNCTION: WON 0x00737580
void PercentLayout::Execute(Component* theParent)
{
    mParam.DoCalc(mTarget,theParent);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007377a0
SameSizeLayout* SameSizeLayout::Add(Component* theComp)
{
    mComponentList.push_back(theComp);
    return this;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00737320
PercentLayoutParam::PercentLayoutParam(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	Init(left,top,right,bottom,px,py,pw,ph,lp,tp,rp,bp);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot original ChildLayouts.cpp, no Native/primary/span credit.
VerticalFlowLayout::VerticalFlowLayout(Component* theTop)
{
    mTopComp = theTop;
}
VerticalFlowLayout* VerticalFlowLayout::Add(Component* theChild, int theTopPad, int theLeftPad)
{
    mComponentList.push_back(ItemInfo(theChild,theTopPad,theLeftPad));
    return this;
}
void VerticalFlowLayout::Execute(Component* theParent)
{
    if(mComponentList.empty())
        return;
    ComponentList::iterator anItr = mComponentList.begin();
    int aTop, aLeft;
    if(anItr->mComponent->GetParent()==mTopComp)
    {
        aTop = 0;
        aLeft = 0;
    }
    else
    {
        aTop = mTopComp->Top()+mTopComp->Height();
        aLeft = mTopComp->Left();
    }
    for(; anItr!=mComponentList.end(); ++anItr)
    {
        aTop += anItr->mTopPad;
        Component* aComp = anItr->mComponent;
        int aLeftPad = anItr->mLeftPad;
        if(aLeftPad>=0)
            aComp->Fn2F4B70(aLeft+aLeftPad,aTop);
        else
            aComp->Fn2F4B70(aComp->Left(),aTop);
        aTop += aComp->Height();
    }
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot SDK ordered borrowed-component lists.
ChildLayoutList::ChildLayoutList(Component *c1)
{
	mChildList.push_back(c1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2)
{
	mChildList.push_back(c1); mChildList.push_back(c2);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5); mChildList.push_back(c6);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5); mChildList.push_back(c6); mChildList.push_back(c7);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ChildLayoutList::ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7, Component *c8)
{
	mChildList.push_back(c1); mChildList.push_back(c2); mChildList.push_back(c3); mChildList.push_back(c4); 
	mChildList.push_back(c5); mChildList.push_back(c6); mChildList.push_back(c7); mChildList.push_back(c8);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutList::Rewind() const
{
	mItr = mChildList.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ChildLayoutList::HasMore() const
{	
	return mItr!=mChildList.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* ChildLayoutList::GetNext() const
{
	if(mItr==mChildList.end())
		return NULL;
	else
		return *mItr++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutList::AddTo(ChildList &theList) const
{
	Rewind();
	while(HasMore())
		theList.push_back(GetNext());
}

///////////////////////////////////////////////////////////////////////////////

}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: remaining genuine SDK Percent parameter/layout constructor overloads.
PercentLayoutParam::PercentLayoutParam(Component *ref, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	Init(ref,ref,ref,ref,px,py,pw,ph,lp,tp,rp,bp);
}

PercentLayoutParam::PercentLayoutParam(Component *ref1, Component *ref2, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
{
	Init(ref1,ref2,ref1,ref2,px,py,pw,ph,lp,tp,rp,bp);
}

PercentLayout::PercentLayout(Component *target, Component *ref, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
: mParam(ref,px,py,pw,ph,lp,tp,rp,bp) 
{
	mTarget = target;
}

PercentLayout::PercentLayout(Component *target, Component *ref1, Component *ref2, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
: mParam(ref1,ref2,px,py,pw,ph,lp,tp,rp,bp) 
{
	mTarget = target;
}

PercentLayout::PercentLayout(Component *target, Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp)
: mParam(left,top,right,bottom,px,py,pw,ph,lp,tp,rp,bp) 
{
	mTarget = target;
}
}
