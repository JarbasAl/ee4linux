// Skinned combo and scroll-container geometry and frame painting on borrowed owners.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySkinnedContainers.h"
namespace WONAPI {

// FUNCTION: WON 0x00719A70
void WONComboBox::Virtual0C(int &width, int &height)
{
	if(mEdit.get()!=NULL)
	{
		mEdit->Virtual0C(width,height);
		height+=2;
//		if(mDrawFrame)
//			height+=mFrame.mTop + mFrame.mBottom;
	}
	else 
		height = 22;

	if(mListArea.get()!=NULL && mListArea->GetNumItems()>0)
	{
		width = 10 + mListArea->GetTotalWidth();
		if(mButton.get()!=NULL)
			width += mButton->Width();
	}
	else
		width = 100;
}

// FUNCTION: WON 0x00719B00
void WONComboBox::Virtual08()
{
	ComboBox::Virtual08();
	int aTopPad = 0, aBottomPad = 0, aRightPad = 0, aLeftPad = 0;
	if(mDrawFrame)
	{
		aLeftPad = mFrame.mLeft;
		aTopPad = mFrame.mTop;
		aBottomPad = mFrame.mBottom;
		aRightPad = mFrame.mRight;
	}

	int anInputBoxHeight = Height(); //mEdit->mHeight;
	int aButtonHeight = anInputBoxHeight-aBottomPad-aTopPad;
	int aButtonWidth = mButton->Width();
	if(!mButton->IsVisible())
		aButtonWidth = 0;
//	if(aButtonWidth>aButtonHeight)
//		aButtonWidth = aButtonHeight;
 
	int anInputBoxWidth = Width()-aButtonWidth-aRightPad;


	mEdit->mLeftPad = aLeftPad + 2;
	mEdit->Fn2F4BF0(0,0,anInputBoxWidth,anInputBoxHeight);
	
	if(mButton->IsVisible())
	{
		if(mButtonInFrame)
			mButton->Fn2F4BF0(mEdit->Right(),aTopPad,aButtonWidth,aButtonHeight);
		else
			mButton->Fn2F4BF0(Width()-aButtonWidth,0,aButtonWidth,Height());
	}

}

// FUNCTION: WON 0x00719BF0
void WONComboBox::Paint(Graphics& g)
{
	ComboBox::Paint(g);

	if(mDrawFrame)
	{
		if(mButtonInFrame || !mButton->IsVisible())
			mFrame.Paint(g,this);
		else
			mFrame.Paint(g,this,0,0,mButton->Width(),0);
	}
}

// FUNCTION: WON 0x00719DF0
void WONScrollContainer::Virtual08()
{
	Container::Virtual08();
	if(mScrollArea.get()==NULL)
		return;

	bool horzOn = mHorzScrollbar->IsVisible();
	bool vertOn = mVertScrollbar->IsVisible();

	int left = mFrame.mLeft, right = mFrame.mRight, top = mFrame.mTop, bottom = mFrame.mBottom;

	int aScrollbarWidth = mVertScrollbar->Width();
	int aScrollbarHeight = mHorzScrollbar->Height();

	int aScrollWidth = Width() - left - right;
	if(vertOn)
		aScrollWidth -= aScrollbarWidth;

	int aScrollHeight = Height() - top - bottom;
	if(horzOn)
		aScrollHeight -= aScrollbarHeight;

	if(vertOn)
		mVertScrollbar->Fn2F4BF0(Width()-aScrollbarWidth-right,top,aScrollbarWidth,aScrollHeight);

	if(horzOn)
		mHorzScrollbar->Fn2F4BF0(left,Height()-aScrollbarHeight-bottom,aScrollWidth,aScrollbarHeight);

	mScrollArea->Fn2F4BF0(left,mHeaderHeight+top,aScrollWidth,aScrollHeight-mHeaderHeight);
}

// FUNCTION: WON 0x00719FF0
void WONScrollContainer::Paint(Graphics &g)
{
	Container::Paint(g);

	if(mVertScrollbar->IsVisible() && mHorzScrollbar->IsVisible() && !mTransparent && mCorner.get()!=NULL) // grey square in corner
	{
		g.DrawImage(mCorner.get(),mVertScrollbar->Left(),(mVertScrollbar->Top()+mVertScrollbar->Height()));

//		g.ApplyColorSchemeColor(StandardColor_Scrollbar);
//		g.FillRect(mVertScrollbar->mX,(mVertScrollbar->mY+mVertScrollbar->mHeight),mVertScrollbar->mWidth,mHorzScrollbar->mHeight);
	}

	mFrame.Paint(g,this);
}
}
