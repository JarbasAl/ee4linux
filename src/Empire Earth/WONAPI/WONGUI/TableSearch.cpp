// Table text search and selection navigation over original-owned line segments.
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
#include "ee/wonapi/WONLobbyTextSegment.h"
namespace WONAPI {
// FUNCTION: WON 0x007107b0
bool Table::FindStr(const GUIString &theStr, bool next, bool caseSensitive)
{
	if(mContents.get()==NULL)
		return false;

	ElementFindInfo anInfo;
	ElementFindInfo aFoundInfo;

	anInfo.mFindText = theStr;
	anInfo.mCaseSensitive = caseSensitive;
	anInfo.mFindPrev = !next;

	bool haveSel = mStartSelSegment>=0 && (mEndSelSegment>mStartSelSegment || mEndSelSegmentPos>mStartSelSegmentPos);
	if(next)
	{
		anInfo.mEndSelSegment = 0x7ffffff;
		anInfo.mEndSelSegmentPos = 0;
		if(!haveSel)
		{
			anInfo.mStartSelSegment = -1;
			anInfo.mStartSelSegmentPos = 0;
		}
		else
		{
			anInfo.mStartSelSegment = mStartSelSegment;
			anInfo.mStartSelSegmentPos = mStartSelSegmentPos+1;
		}
	}
	else // find Prev
	{
		anInfo.mStartSelSegment = -1;
		anInfo.mStartSelSegmentPos = 0;
		if(!haveSel)
		{
			anInfo.mEndSelSegment = 0x7ffffff;
			anInfo.mEndSelSegmentPos = 0;
		}
		else
		{
			anInfo.mEndSelSegment = mEndSelSegment;
			anInfo.mEndSelSegmentPos = mEndSelSegmentPos-1;
		}
	}

	anInfo.mCurFoundLength = 0;
	anInfo.mFoundStartSegment = -1;
	anInfo.mFoundX = 0;
	anInfo.mFoundY = 0;

	if(!mContents->FindStr(anInfo))
		return false;

	mStartSelSegment = anInfo.mFoundStartSegment;
	mEndSelSegment = anInfo.mFoundEndSegment;
	mStartSelSegmentPos = anInfo.mFoundStartChar;
	mEndSelSegmentPos = anInfo.mFoundEndChar;
	int horzOffset = mHorzOffset;
	int vertOffset = mVertOffset;
	if(anInfo.mFoundX > horzOffset + mScrollArea.width-20 || anInfo.mFoundX<horzOffset+20)
		horzOffset = anInfo.mFoundX;
	if(anInfo.mFoundY > vertOffset + mScrollArea.height-20 || anInfo.mFoundY<vertOffset+20)
		vertOffset = anInfo.mFoundY;

	SetOffsets(horzOffset,vertOffset);
	Invalidate();
	return true;
}
}
