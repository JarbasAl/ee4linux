// Native Table hover, child hit-testing and mouse selection transition policy.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLImageSegments.h"
namespace WONAPI {
// FUNCTION: WON 0x0070ff20
void Table::MouseAt(int &x, int &y, bool mouseDown)
{
	LineSegment::mTableContext = this;
	if(mContents.get()!=NULL)
	{
		int oldx = x;
		int oldy = y;
		ScrollTranslate(x,y);
		LineSegment *aSeg = mContents->GetSegmentAt(x,y,mouseDown);
		if(mModalCount && aSeg!=NULL && !aSeg->AllowModalInput()) // fix for when combo box popup is up
			aSeg = NULL;

		if(aSeg!=mMouseSegment)
		{
			if(mMouseSegment!=NULL)
			{
				mMouseSegment->MouseExit();
				Virtual78(Fn2F4B00());
			}

			mMouseSegment = aSeg;
			if(mMouseSegment!=NULL)
			{
				if(mouseDown && (x<0 || y<0 || x>mMouseSegment->GetWidth() || y>mMouseSegment->GetHeight()))
					return;

				mMouseSegment->MouseEnter(x,y);
			}
		}
		
		mMouseSegmentDX = x-oldx;
		mMouseSegmentDY = y-oldy;
	}
	else
		mMouseSegment = NULL;
}

// FUNCTION: WON 0x00710040
Component* Table::VirtualB0(int x, int y) 
{ 
	if(mContents.get()!=NULL)
	{
		LineSegment::mTableContext = this;
		ScrollTranslate(x,y);
		ComponentLineSegment *aSeg = dynamic_cast<ComponentLineSegment*>(mContents->GetSegmentAt(x,y,false));
		if(aSeg!=NULL)
			return aSeg->mComponent.get();
	}

	return NULL;
}

// FUNCTION: WON 0x007100b0
void Table::Virtual24(int x, int y)
{
	Container::Virtual24(x,y);
	MouseAt(x,y);
}

// FUNCTION: WON 0x007100e0
void Table::Virtual28()
{
	Container::Virtual28();
	mSelecting = false;
	if(mMouseSegment!=NULL)
	{
		mMouseSegment->MouseExit();
	}
}

// FUNCTION: WON 0x00710100
void Table::Virtual2C(int x, int y, int theButton)
{
	Container::Virtual2C(x,y,theButton);
	MouseAt(x,y,true);
	if(mMouseSegment!=NULL)
	{	
		if(theButton==MouseButton_Left)
		{
			bool canStartSelection = mMouseSegment->CanStartSelection();
			if(!canStartSelection)
			{
				if(x<0 || y<0 || x>mMouseSegment->GetWidth() || y>mMouseSegment->GetHeight())
					canStartSelection = true;
			}

			if(canStartSelection && mMouseSegment->mStartSelIndex>=0)
			{
				if(mFocusChild!=NULL)
				{
					mFocusChild->Virtual4C();
					mFocusChild = NULL;
				}

				mStartSelSegment = mEndSelSegment = mMouseSegment->mStartSelIndex;
				mStartSelSegmentPos = mEndSelSegmentPos = mMouseSegment->GetSelChar(x,y);
				mBackwardsSel = false;
				mMouseSegment->MouseExit();
				mMouseSegment = NULL;
				mSelecting = true;
				Fn2F57D0(true);
				Invalidate();
				return;
			}
		}

		mMouseSegment->MouseDown(x,y,static_cast<MouseButton>(theButton));
	}
	
	if(mStartSelSegment>=0)
	{
		mStartSelSegment = mEndSelSegment = -1;
		Invalidate();
	}
}

// FUNCTION: WON 0x00710250
void Table::Virtual30(int x, int y, int theButton)
{
	Container::Virtual30(x,y,theButton);

	if(theButton==MouseButton_Left)
		mSelecting = false;

	MouseAt(x,y);
	if(mMouseSegment!=NULL)
	{
		mMouseSegment->MouseUp(x,y,static_cast<MouseButton>(theButton));
	}
}

// FUNCTION: WON 0x007102b0
void Table::Virtual34(int x, int y)
{
	Container::Virtual34(x,y);
	MouseAt(x,y);
	if(mMouseSegment!=NULL)
	{
		mMouseSegment->MouseMove(x,y);
	}
}

// FUNCTION: WON 0x00710300
void Table::Virtual38(int x, int y)
{
	Container::Virtual38(x,y);
	if(mMouseSegment!=NULL)
	{
		LineSegment::mTableContext = this;
		x+=mMouseSegmentDX;
		y+=mMouseSegmentDY;
		mMouseSegment->MouseDrag(x,y);
	}
	else if(mStartSelSegment>=0)
	{
		ScrollTranslate(x,y);
		LineSegment *aSeg = mContents->GetSegmentAt(x,y,true);
		if(aSeg!=NULL && aSeg->mStartSelIndex>=0)
		{
			if(mBackwardsSel)
			{
				mStartSelSegment = aSeg->mStartSelIndex;
				mStartSelSegmentPos = aSeg->GetSelChar(x,y);
			}
			else
			{
				mEndSelSegment = aSeg->mStartSelIndex;
				mEndSelSegmentPos = aSeg->GetSelChar(x,y);
			}

			if(mEndSelSegment<mStartSelSegment || (mEndSelSegment==mStartSelSegment && mEndSelSegmentPos<mStartSelSegmentPos))
			{
				int temp = mStartSelSegment; mStartSelSegment = mEndSelSegment; mEndSelSegment = temp;
				temp = mStartSelSegmentPos; mStartSelSegmentPos = mEndSelSegmentPos; mEndSelSegmentPos = temp;
				mBackwardsSel = !mBackwardsSel;
			}
			Invalidate();
		}
	}
}
}
