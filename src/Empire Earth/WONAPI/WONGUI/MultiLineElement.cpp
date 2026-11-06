// Rich text multi-line hit testing, selection/search, extents and rendering.
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

// FUNCTION: WON 0x0070CA30
LineSegment* MultiLineElement::GetSegmentAt(int &x, int &y, bool lenient) 
{ 
	LineVector::iterator anItr = FindLineAt(y);
	FloatingSegment *aFloat = NULL; 
	if(anItr!=mLineVector.end())
	{
		LineElement *aLine = anItr->get();
		if(y>=aLine->my && y<aLine->my+aLine->GetHeight() && aLine->mLeftMargin<=x && aLine->mLeftMargin+aLine->mWidth>=x)
		{
			y-=aLine->my;
			return aLine->GetSegmentAt(x,y,lenient);
		}
		else if(aLine->mLeftMargin>x) // find left floating element
			aFloat = FindFloatingSegmentAt(x,y,true);
		else // find right floating element
			aFloat = FindFloatingSegmentAt(x,y,false);

		if(lenient && aFloat==NULL)
		{
			y-=aLine->my;
			return aLine->GetSegmentAt(x,y,lenient);
		}
	}
	else
	{
		aFloat = FindFloatingSegmentAt(x,y,true);
		if(aFloat==NULL)
			aFloat = FindFloatingSegmentAt(x,y,false);
	}

	if(aFloat==NULL)
		return this;

	x-=aFloat->mx;
	y-=aFloat->my;
	return aFloat->mSegment->GetSegmentAt(x,y,lenient);
}

// FUNCTION: WON 0x0070CB60
void MultiLineElement::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	LineVector::iterator anItr = mLineVector.begin();
	while(anItr!=mLineVector.end())
	{
		LineElement *aLine = anItr->get();
		aLine->SetViewport(theViewport,x,y+aLine->my);
		++anItr;
	}

	for(int i=0; i<2; i++)
	{
		for(FloatingVector::iterator anItr =  mFloatingItems[i].begin(); anItr!=mFloatingItems[i].end(); ++anItr)
		{
			FloatingSegment &aSeg = *anItr;
			aSeg.mSegment->SetViewport(theViewport,x + aSeg.mx, y + aSeg.my);
		}
	}

}

// FUNCTION: WON 0x0070CBF0
void MultiLineElement::SetSelIndex()
{
	LineSegment::SetSelIndex();
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		anItr->mSegment->SetSelIndex();
		++anItr;
	}
}

// FUNCTION: WON 0x0070CC50
void MultiLineElement::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->mSegment.get();
		if(aSeg->GetEndSelIndex()>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	bool hadText = false;
	while(anItr!=mSegmentList.end())
	{
		SegmentInfo &aSegInfo = *anItr;
		LineSegment *aSeg = aSegInfo.mSegment.get();
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
		{
			if((aSegInfo.mFlags&4) && hadText && aSeg->mStartSelIndex!=theInfo.mStartSelSegment)
			{
				hadText = false;
			}

			int anOldLen = theText.length();
			aSeg->GetSelText(theText,theInfo);
			hadText = theText.length()!=anOldLen;
		
			if((aSegInfo.mFlags&8) && hadText && aSeg->mStartSelIndex!=theInfo.mEndSelSegment)
			{
				hadText = false;
			}
		}
		else
			break;			

		++anItr;
	}
}

// FUNCTION: WON 0x0070CCD0
bool MultiLineElement::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	LineVector::iterator aLineItr = mLineVector.begin();
	while(aLineItr!=mLineVector.end())
	{
		LineElement *anElement = aLineItr->get();
		if(anElement->GetSelCharPos(x,y,theSelIndex,theSelChar))
			return true;

		++aLineItr;
	}

	for(int i=0; i<2; i++)
	{
		FloatingVector::iterator anItr = mFloatingItems[i].begin();
		while(anItr!=mFloatingItems[i].end())
		{
			FloatingSegment &aSegment = *anItr;
			if(aSegment.mSegment->GetSelCharPos(x,y,theSelIndex,theSelChar))
				return true;

			++anItr;
		}
	}

	return false;
}

// FUNCTION: WON 0x0070CD70
bool MultiLineElement::FindStr(ElementFindInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->mSegment.get();
		if(aSeg->GetEndSelIndex()>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	bool found = false;
	SegmentList::iterator aStartFindItr = mSegmentList.end();
	ElementFindInfo aFoundInfo;

	while(anItr!=mSegmentList.end())
	{
		SegmentInfo &aSegInfo = *anItr;
		LineSegment *aSeg = aSegInfo.mSegment.get();
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
		{
			if((aSegInfo.mFlags&4))
				theInfo.mFoundStartSegment = -1;

			if(aSeg->FindStr(theInfo))
			{
				found = true;
				aFoundInfo = theInfo;
				if(theInfo.mFindPrev)
				{
					if(aStartFindItr!=mSegmentList.end())
						anItr = aStartFindItr;

					theInfo.mStartSelSegment = theInfo.mFoundStartSegment;
					theInfo.mStartSelSegmentPos = theInfo.mFoundStartChar+1;
					theInfo.mCurFoundLength = 0;
					theInfo.mFoundStartSegment = -1;
					continue;
				}
				else
					break;
			}
			else if(theInfo.mCurFoundLength>0 && aStartFindItr==mSegmentList.end())
				aStartFindItr = anItr;
			else
				aStartFindItr = mSegmentList.end();


			if((aSegInfo.mFlags&8))
				theInfo.mFoundStartSegment = -1;
		}
		else
			break;			

		++anItr;
	}

	if(!found)
	{
		theInfo.mStartSelSegment = -1;
		return false;
	}
	else
	{
		theInfo = aFoundInfo;
		GetSelCharPos(theInfo.mFoundX,theInfo.mFoundY,theInfo.mFoundStartSegment,theInfo.mFoundStartChar);
		return true;
	}
}

// FUNCTION: WON 0x0070D570
void MultiLineElement::AddSegment(LineSegment *theSegment, unsigned short theFlags)
{
	int aMinWidth = theSegment->GetMinWidth();
	if(aMinWidth>mMinWidth)
		mMinWidth = aMinWidth;

	if(theFlags & 4)
		mCurLineWidth = 0;

	mCurLineWidth += theSegment->GetMaxWidth();
	if(mCurLineWidth>mMaxWidth)
		mMaxWidth = mCurLineWidth;

	if(theFlags & 8)
		mCurLineWidth = 0;


	mSegmentList.push_back(SegmentInfo(theSegment,theFlags));
}

// FUNCTION: WON 0x0070D6E0
void MultiLineElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	int aTop = theInfo.mClipY-y;
	int aBottom = (theInfo.mClipY+theInfo.mClipHeight)-y;
	if(aTop>mHeight || theInfo.mClipX-x>mWidth || aBottom<0 || (theInfo.mClipX+theInfo.mClipWidth)-x<0)
		return;

	LineVector::iterator anItr;
	if(aTop>0)
		anItr = FindLineAt(aTop);
	else
		anItr = mLineVector.begin();

	while(anItr!=mLineVector.end())
	{
		LineElement *anElement = anItr->get();
		if(anElement->my > aBottom)
			break;

		anElement->Paint(g,x,y+anElement->my,theInfo);
		++anItr;
	}

	FloatingVector::iterator aFloatItr;
	for(int i=0; i<2; i++)
	{
		for(aFloatItr=mFloatingItems[i].begin(); aFloatItr!=mFloatingItems[i].end(); ++aFloatItr)
		{
			FloatingSegment *aSegment = &*aFloatItr;
			theInfo.mLineY = y+aSegment->my;
			theInfo.mLineHeight = aSegment->GetHeight();
			aSegment->mSegment->Paint(g,x+aSegment->mx,y+aSegment->my,theInfo);
		}
	}
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00724380
// (void __cdecl std::_Construct(struct WONAPI::SegmentInfo *,struct WONAPI::SegmentInfo const &)    [?_Construct@std@@YAXPAUSegmentInfo@WONAPI@@ABU23@@Z])
