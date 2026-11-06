// Actual rich-text line positioning, selection traversal, drawing and build metrics.
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
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {

// FUNCTION: WON 0x0070B5D0
void LineBuilder::AddSegment(LineSegment *theSegment)
{
	mLine->mSegmentList.push_back(theSegment);
	mWidth += theSegment->GetWidth();

	int anAscent;
	int aDescent;
	int aTopHeight;
	switch(theSegment->GetVertAlign())
	{
		case 0: // align with baseline
			anAscent = theSegment->GetAscent();
			aDescent = theSegment->GetHeight() - anAscent;
			break;

		case 1: // align middle
			anAscent = aDescent = theSegment->GetHeight()/2;
			break;

		case 2: // align top
			aTopHeight = theSegment->GetHeight();
			if(aTopHeight>mTopAlignHeight)
				mTopAlignHeight = aTopHeight;
			return;
	}

	if(anAscent > mMaxAscent)
		mMaxAscent = anAscent;

	if(aDescent > mMaxDescent)
		mMaxDescent = aDescent;

}

// FUNCTION: WON 0x0070b7c0
LineElement::LineElement()
{
    mBaseline=mWidth=mHeight=mLeftMargin=0;
    mIsGlueLine=false;
}
// FUNCTION: WON 0x0070B920
LineSegment* LineElement::GetSegmentAt(int &x, int &y, bool lenient)
{
	if(x<mLeftMargin && !lenient)
		return this;

	int anXPos = mLeftMargin;
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = (*anItr++).get();
		int aWidth = aSeg->GetWidth();
		if(anXPos + aWidth >= x || (lenient && anItr==mSegmentList.end()))
		{
			int aYPos = 0;
			int aHeight = aSeg->GetHeight();
			switch(aSeg->GetVertAlign())
			{
				case 0: aYPos = mBaseline-aSeg->GetAscent(); break;
				case 1: aYPos = mBaseline-aSeg->GetAscent()/2; break;
			}

			if((y>=aYPos && y<=aYPos+aHeight) || lenient)
			{
				x-=anXPos;
				y-=aYPos;
				return aSeg->GetSegmentAt(x, y, lenient);
			}
			else
				return this;
		}
		anXPos += aWidth;
	}

	return this;
}

// FUNCTION: WON 0x0070BA00
void LineElement::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	x+=mLeftMargin;
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		int aYPos = y;
		switch(aSeg->GetVertAlign())
		{
			case 0: aYPos = y+mBaseline-aSeg->GetAscent(); break;
			case 1: aYPos = y+mBaseline-aSeg->GetAscent()/2; break;
			case 2: aYPos = y; break;
		}

		aSeg->SetViewport(theViewport,x,aYPos);
		x += aSeg->GetWidth();
		++anItr;
	}
}

// FUNCTION: WON 0x0070BAB0
void LineElement::SetSelIndex()
{
	LineSegment::SetSelIndex();
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		(*anItr)->SetSelIndex();
		++anItr;
	}

}

// FUNCTION: WON 0x0070BB10
void LineElement::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		if(aSeg->mStartSelIndex>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
			aSeg->GetSelText(theText,theInfo);
		else
			break;			

		++anItr;
	}
}

// FUNCTION: WON 0x0070BB70
bool LineElement::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		if(aSeg->GetSelCharPos(x,y,theSelIndex,theSelChar))
			return true;

		++anItr;
	}
	return false;
}

// FUNCTION: WON 0x0070BBD0
bool LineElement::FindStr(ElementFindInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		if(aSeg->mStartSelIndex>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
		{
			if(aSeg->FindStr(theInfo))
				return true;
		}
		else
			break;			

		++anItr;
	}

	return false;
}

// FUNCTION: WON 0x0070BC40
void LineElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	int oldY = theInfo.mLineY, oldHeight = theInfo.mLineHeight;

	if(!mIsGlueLine)
	{
		theInfo.mLineY = y; 
		theInfo.mLineHeight = mHeight;

	}


	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->get();
		switch(aSeg->GetVertAlign())
		{
			case 0: aSeg->Paint(g,x+mLeftMargin,y+mBaseline-aSeg->GetAscent(),theInfo); break;
			case 1: aSeg->Paint(g,x+mLeftMargin,y+mBaseline-aSeg->GetAscent()/2,theInfo); break;
			case 2: aSeg->Paint(g,x+mLeftMargin,y,theInfo); break;
		}
		x+=aSeg->GetWidth();
		++anItr;
	}

	theInfo.mLineY = oldY; theInfo.mLineHeight = oldHeight;
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0070b880
// (public: virtual void * __thiscall WONAPI::LineElement::`scalar deleting destructor'(unsigned int)    [??_GLineElement@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0070b8a0
// (public: virtual __thiscall WONAPI::LineElement::~LineElement(void))
