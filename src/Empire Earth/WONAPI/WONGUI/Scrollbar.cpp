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
// Native305fa0 support0, noquota.
// FUNCTION: WON 0x00705fa0
void Scrollbar::CopyAttributes(Scrollbar *theCopyFrom)
{
	mLineUpScrollSize =  theCopyFrom->mLineUpScrollSize;
	mLineDownScrollSize =  theCopyFrom->mLineDownScrollSize;
	mResizeThumb = theCopyFrom->mResizeThumb;
	mMinThumbSize = theCopyFrom->mMinThumbSize;
}

// Native305fe0 support0, noquota.
// FUNCTION: WON 0x00705fe0
void Scrollbar::Virtual6C()
{
	if(mUpArrow->mParent!=this)
		AddControls();

	Container::Virtual6C();
}

// FUNCTION: WON 0x00706000
void Scrollbar::AddControls()
{
	if(mScrollRect.get()==NULL)
		mScrollRect = new Button;

	mUpArrow->Fn2F3370(2,true);
	mDownArrow->Fn2F3370(2,true);	
	mScrollRect->Fn2F3370(2,true);

	Fn2F6A60(mScrollRect.get());
	Fn2F6A60(mThumb.get());
	Fn2F6A60(mUpArrow.get());
	Fn2F6A60(mDownArrow.get());
}

// Native306100 support0, noquota.
// FUNCTION: WON 0x00706100
void Scrollbar::S_SetPos(Component *theComponent, int x, int y)
{
	if(mVertical)
		theComponent->Fn2F4B70(x,y);
	else
		theComponent->Fn2F4B70(y,x);
}

// Native306140 support0, noquota.
// FUNCTION: WON 0x00706140
void Scrollbar::S_SetSize(Component *theComponent, int width, int height)
{
	if(mVertical)
		theComponent->Fn2F4B90(width,height);
	else
		theComponent->Fn2F4B90(height,width);
}

// Native306180 support0, noquota.
// FUNCTION: WON 0x00706180
void Scrollbar::S_SetPosSize(Component *theComponent, int x, int y, int width, int height)
{
	if(mVertical)
		theComponent->Fn2F4BF0(x,y,width,height);
	else
		theComponent->Fn2F4BF0(y,x,height,width);
}

// FUNCTION: WON 0x007061d0
void Scrollbar::LayoutThumb(bool changePos)
{
	if(mPageSize >= mTotalSize)
	{
		mThumb->SetVisible(false);
		return;
	}

	mThumb->SetVisible(!IsDisabled());

	int aScrollHeight = S_Height(mScrollRect.get());
	int aThumbSize = S_Height(mThumb.get());
	if(mResizeThumb)
	{
		aThumbSize = aScrollHeight*mPageSize/mTotalSize;
		if(aThumbSize<mMinThumbSize)
			aThumbSize = mMinThumbSize;
	}

	int aThumbPos = S_Top(mThumb.get());
	if(changePos)
		aThumbPos = S_Top(mScrollRect.get())+(aScrollHeight-aThumbSize)*mPosition/(mTotalSize-mPageSize);

	if(aThumbPos<S_Top(mScrollRect.get()))
	{
		aThumbPos = S_Top(mScrollRect.get());
		changePos = true;
	}
	if(aThumbPos+aThumbSize >= S_Bottom(mScrollRect.get()))
	{
		aThumbPos = S_Bottom(mScrollRect.get())-aThumbSize;
		changePos = true;
	}

	int aThumbWidth = S_Width(mThumb.get());
	if(aThumbWidth==0)
		aThumbWidth = S_Width(this);

	if(changePos)
		S_SetPosSize(mThumb.get(),S_Left(mThumb.get()),aThumbPos,aThumbWidth,aThumbSize);
	else
		S_SetSize(mThumb.get(),aThumbWidth,aThumbSize);
}

// FUNCTION: WON 0x00706380
void Scrollbar::Virtual08()
{
	Container::Virtual08();

	if(S_Height(mUpArrow.get())==0)
		S_SetSize(mUpArrow.get(), S_Width(this),S_Width(this));
	if(S_Height(mDownArrow.get())==0)
		S_SetSize(mDownArrow.get(),S_Width(this),S_Width(this));

	S_SetPos(mDownArrow.get(),0,S_Height(this)-S_Height(mDownArrow.get()));
	S_SetPosSize(mScrollRect.get(),0,S_Bottom(mUpArrow.get()),S_Width(this),S_Height(this)-S_Height(mUpArrow.get())-S_Height(mDownArrow.get()));
	LayoutThumb(false);
}

// FUNCTION: WON 0x007064c0
void Scrollbar::Virtual2C(int x, int y, int theButton)
{
	Container::Virtual2C(x,y,theButton);
	if(theButton==0 && mMouseChild==mThumb.get())
	{
		mDragX = x-mThumb->Left();
		mDragY = y-mThumb->Top();
	}
}

// FUNCTION: WON 0x00706510
void Scrollbar::Virtual38(int x, int y)
{
	Container::Virtual38(x,y);
	if(mMouseChild==mThumb.get())
	{
		int dx = (x-mThumb->Left())-mDragX;
		int dy = (y-mThumb->Top())-mDragY;

		int aNewY = mVertical? (mThumb->Top() + dy) : (mThumb->Left() + dx);

		if(aNewY<S_Top(mScrollRect.get()))
			aNewY = S_Top(mScrollRect.get());
		if(aNewY+S_Height(mThumb.get()) > S_Bottom(mScrollRect.get()))
			aNewY = S_Bottom(mScrollRect.get())-S_Height(mThumb.get());

		int oldPosition = mPosition;
		int aHeightDiff = S_Height(mScrollRect.get()) - S_Height(mThumb.get());
		if(aHeightDiff<=0)
			mPosition = 0;
		else
			mPosition = (mTotalSize-mPageSize)*(aNewY-S_Top(mScrollRect.get()))/aHeightDiff;

		if(aNewY!=S_Top(mThumb.get()))
		{
			S_SetPos(mThumb.get(),S_Left(mThumb.get()),aNewY);
			mScrollRect->Invalidate();
		}
		if(mPosition!=oldPosition)
		{
			Fn2F5380(16,0);
		}

	}
}

// FUNCTION: WON 0x00706690
void Scrollbar::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType!=15)
	{
		Container::HandleComponentEvent(theEvent);
		return;
	}

	int anOldPosition = mPosition;
	if(theEvent->mComponent==mUpArrow.get())
		mPosition-=mLineUpScrollSize;
	else if(theEvent->mComponent==mDownArrow.get())
		mPosition+=mLineDownScrollSize;
	else if(theEvent->mComponent==mScrollRect.get())
	{
		int aMouseX, aMouseY;
		GetMousePos(aMouseX, aMouseY);
		if(!mVertical)
			aMouseY = aMouseX;

		if(aMouseY < S_Top(mThumb.get()))
			mPosition-=mPageScrollSize;
		else if(aMouseY >= S_Bottom(mThumb.get()))
			mPosition+=mPageScrollSize;
	}
	else
	{
		Container::HandleComponentEvent(theEvent);
		return;
	}


	int aMaxPos = mTotalSize - mPageSize;
	if(mPosition>aMaxPos)
		mPosition = aMaxPos;
	if(mPosition<0)
		mPosition = 0;

	if(mPosition!=anOldPosition)
	{
		LayoutThumb(true);
		mScrollRect->Invalidate();
		Fn2F5380(16,0);
	}
}

// FUNCTION: WON 0x007067e0
void Scrollbar::SetEnabled(bool isEnabled)
{
	if(isEnabled==!IsDisabled())
		return;

	Component::SetEnabled(isEnabled);
	mUpArrow->SetEnabled(isEnabled);
	mDownArrow->SetEnabled(isEnabled);
	if(mTotalSize > mPageSize)
		mThumb->SetVisible(isEnabled);
}

}
