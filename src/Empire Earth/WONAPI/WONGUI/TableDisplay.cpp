// Actual Table display sizing, selection autoscroll and viewport painting.
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
namespace WONAPI {
// FUNCTION: WON 0x00710450
bool Table::Virtual44(int theDelta)
{
	bool needTimer = ScrollArea::Virtual44(theDelta);
	needTimer = needTimer || mSelecting;

	if(!mSelecting)
		return needTimer;

	int x,y;
	GetMousePos(x,y);
	if(x>=0 && x<=Width() && y>=0 && y<=Height())
		return true;

	if(y>Height())
	{
		int aDelta = y-Height();
		mVertOffset+=aDelta;
	}
	else if(y<0)
	{
		int aDelta = -y;
		mVertOffset-=aDelta;
	}

	if(x>Width())
		mHorzOffset+=(x-Width());
	else if(x<0)
		mHorzOffset+=x;
		
	VirtualCC(false);
	Virtual38(x,y);

	return true;
}

// FUNCTION: WON 0x00710500
void Table::SetContents(LineSegment *theContents)
{
	mViewport->Reset(); // this will end any popup being done by the contents
	if(mContents.get()!=NULL)
	{
		LineSegment::mTableContext = this;
		LineSegment::mCounter = 0;
		mContents->SetViewport(NULL,0,0);  // this will set the parents to null of any component line segment
	}

	mContents = theContents;
	mMouseSegment = NULL;
	mStartSelSegment = mEndSelSegment = -1;
	mSelecting = false;
	SetMaxWidth();
}

// FUNCTION: WON 0x007105b0
void Table::SetMaxWidth(bool adjustPosition)
{
	mMouseSegment = NULL;
	mAnchorMap.clear();
	if(mContents.get()!=NULL)
	{
		mContents->SetMaxWidth(mScrollArea.width);

		LineSegment::mTableContext = this;
		LineSegment::mCounter = 0;
		mContents->SetViewport(GetViewport(),0,0);

		mL0A4 = mContents->GetWidth();
		mTotalHeight = mContents->GetHeight();
	}
	else
	{
		mL0A4 = mTotalHeight = 0;
	}
	
	if(adjustPosition)
		VirtualCC(false);
}

// FUNCTION: WON 0x00710660
void Table::VirtualD0()
{
	SetMaxWidth(false);
}

// FUNCTION: WON 0x00710670
void Table::VirtualD4(Graphics &g)
{
	if(mContents.get()==NULL)
		return;

	ElementPaintInfo anInfo;
	anInfo.mClipX = mHorzOffset;
	anInfo.mClipY = mVertOffset;
	anInfo.mClipWidth = mScrollArea.width;
	anInfo.mClipHeight = mScrollArea.height;
	anInfo.mStartSelSegment = mStartSelSegment;
	anInfo.mEndSelSegment = mEndSelSegment;
	anInfo.mStartSelSegmentPos = mStartSelSegmentPos;
	anInfo.mEndSelSegmentPos = mEndSelSegmentPos;

	mContents->Paint(g,0,0,anInfo);
}

// FUNCTION: WON 0x00710700
void Table::AddAnchor(const GUIString &theName, int x, int y)
{
	mAnchorMap[theName] = AnchorCoordinate(x,y);
}

// FUNCTION: WON 0x00710730
bool Table::GotoAnchor(const GUIString &theName)
{
	AnchorMap::iterator anItr = mAnchorMap.find(theName);
	if(anItr==mAnchorMap.end())
		return false;

//	mHorzOffset = anItr->second.mx;
	mVertOffset = anItr->second.my;
	VirtualCC(false);
	return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071b680
// (public: int __thiscall WONAPI::GUIString::CompareNoCase(class WONAPI::GUIString const &)const     [?CompareNoCase@GUIString@WONAPI@@QBEHABV12@@Z])
// TEMPLATE: WON 0x007206f0
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::Table::AnchorCoordinate>,struct std::m)
// TEMPLATE: WON 0x00720f20
// (protected: struct std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::Table::AnchorCoordinate>,struct st)
// TEMPLATE: WON 0x00723720
// (public: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::Table::AnchorCoordinate>,str)
// TEMPLATE: WON 0x00723a40
// (public: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::Table::AnchorCoordinate>,str)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071b510
// (public: struct WONAPI::Table::AnchorCoordinate & __thiscall std::map<class WONAPI::GUIString,struct WONAPI::Table::AnchorCoordinate,struct WONAPI::GUI)
// TEMPLATE: WON 0x00720c20
// (protected: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::Table::AnchorCoordinate>,struct std)
