// Native rich-text line layout builder with genuine original lifecycle providers.
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
// Genuine SDK helper APIs, support0; definitions follow complete LineElement type.

// Support0 genuine indent setters and margin recomputation.
// FUNCTION: WON 0x0070be50
void MultiLineBuilder::SetIndentSize(int theLeftIndent, int theRightIndent)
{
	mLeftIndent = theLeftIndent;
	mRightIndent = theRightIndent;
	CalcMargins();

}

// FUNCTION: WON 0x0070BE70
void MultiLineBuilder::AlignCurLine()
{
	if(!mLineVector->empty()) // Calc alignment
	{
		CurLine()->FinishBuild();
		LineElement *anElement = CurLine()->mLine.get();
		int aDelta = 0;
		switch(mAlignment)
		{
			case HorzAlign_Center: 
			{
				int anExtraWidth = GetCurMaxLineWidth() - anElement->GetWidth();
				aDelta = anExtraWidth>>1; 
				if((anExtraWidth&1) && (anElement->GetWidth()&1) && aDelta<anExtraWidth)
					aDelta++;
				
				break;
			}
			case HorzAlign_Right: aDelta = GetCurMaxLineWidth() - anElement->GetWidth(); break;//(mSpecifiedWidth - mRightMargin - mRightIndent - anElement->GetWidth()); break;
		}
		anElement->mLeftMargin = aDelta + mLeftMargin;//+ mLeftIndent;
		if(anElement->mLeftMargin<0)
			anElement->mLeftMargin = 0;

		if(anElement->GetWidth() + anElement->mLeftMargin > mWidth)
			mWidth = anElement->GetWidth() + anElement->mLeftMargin;
	}
}

// FUNCTION: WON 0x0070BF20
bool MultiLineBuilder::FlushDeferredItems()
{
	if(mHaveDeferredItems)
	{
		if(!mDeferredLeftItems.empty())
		{
			for(LineSegmentList::iterator anItr = mDeferredLeftItems.begin(); anItr!=mDeferredLeftItems.end(); ++anItr)
				AddFloatingSegment(*anItr,true);

			mDeferredLeftItems.clear();
		}
		if(!mDeferredRightItems.empty())
		{
			for(LineSegmentList::iterator anItr = mDeferredRightItems.begin(); anItr!=mDeferredRightItems.end(); ++anItr)
				AddFloatingSegment(*anItr,false);
			
			mDeferredRightItems.clear();
		}
		mHaveDeferredItems = false;
		return true;
	}
	else
		return false;
}

// FUNCTION: WON 0x0070C000
LineElement* MultiLineBuilder::AddLine(bool clearLeft, bool clearRight)
{
	AlignCurLine();
	if(mHaveDeferredItems)
	{
		CurLine()->Clear();
		FlushDeferredItems();
	}

	if(clearLeft && mHeight<mMaxLeftHeight)
		mHeight = mMaxLeftHeight;

	if(clearRight && mHeight<mMaxRightHeight)
		mHeight = mMaxRightHeight;

	CalcMargins();
	LineElement *anElement = new LineElement;
	anElement->my = mHeight;

	mLineVector->push_back(anElement);
	mCurLine.StartBuild(anElement);

	return anElement;
}

// FUNCTION: WON 0x0070C150
void MultiLineBuilder::AddSegmentToCurLine(LineSegment *theSegment)
{

	int anOldHeight = CurLine()->GetHeight();
	CurLine()->AddSegment(theSegment);
	int aWidth = CurLine()->GetWidth()+mLeftMargin;//+mLeftIndent;
	int aNewHeight = CurLine()->GetHeight();
	
	if(aWidth>mWidth)
	{


		if(aWidth>mWidth)
			mWidth = aWidth;
	}

	if(aNewHeight!=anOldHeight)
		mHeight += aNewHeight-anOldHeight;
}

// FUNCTION: WON 0x0070C1B0
void MultiLineBuilder::CalcMargins()
{
	FloatingList::iterator anItr;

	int aBottom;
	int aLeftMargin = 0;
	int aRightMargin = mSpecifiedWidth;
	for(anItr=mCurLeftItems.begin(); anItr!=mCurLeftItems.end(); )
	{
		FloatingSegment *aSegment = &*anItr;
		aBottom = aSegment->my + aSegment->GetHeight();
		if(aBottom > mHeight)
		{
			if(aSegment->mx + aSegment->GetWidth() > aLeftMargin)
				aLeftMargin = aSegment->mx + aSegment->GetWidth();

			if(aBottom>mMaxLeftHeight)
				mMaxLeftHeight = aBottom;

			++anItr;
		}
		else
			mCurLeftItems.erase(anItr++);
	}

	for(anItr=mCurRightItems.begin(); anItr!=mCurRightItems.end(); )
	{
		FloatingSegment *aSegment = &*anItr;
		aBottom = aSegment->my + aSegment->GetHeight();
		if(aBottom > mHeight)
		{
			if(aSegment->mx < aRightMargin)
				aRightMargin = aSegment->mx;

			if(aBottom>mMaxRightHeight)
				mMaxRightHeight = aBottom;

			++anItr;
		}
		else
			mCurRightItems.erase(anItr++);
	}

	if(aLeftMargin<mLeftIndent)
		aLeftMargin = mLeftIndent;
	if(mSpecifiedWidth-aRightMargin<mRightIndent)
		aRightMargin = mSpecifiedWidth-mRightIndent;

	mLeftMargin = aLeftMargin;
	mRightMargin = aRightMargin;
}

// FUNCTION: WON 0x0070C310
void MultiLineBuilder::AddFloatingSegment(LineSegment *theSegment, bool left)
{

	if(!CurLine()->IsEmpty())
	{
		mHaveDeferredItems = true;
		if(left)
			mDeferredLeftItems.push_back(theSegment);
		else
			mDeferredRightItems.push_back(theSegment);

		return;
	}

	int aWidth = theSegment->GetWidth();
	if(left)
	{

		if(mLeftMargin+aWidth>mRightMargin)
			mLeftMargin = mRightMargin-aWidth;
		if(mLeftMargin<0)
			mLeftMargin = 0;

		if(mLeftMargin+aWidth>mWidth)
			mWidth = mLeftMargin+aWidth;

		mLeftItems->push_back(FloatingSegment(theSegment,mLeftMargin,mHeight));
		mCurLeftItems.push_back(mLeftItems->back());
	}
	else 
	{
		if(mWidth<mSpecifiedWidth)
			mWidth = mSpecifiedWidth;

		if(mRightMargin-aWidth<mLeftMargin)
		{
			mRightMargin = mLeftMargin+aWidth;
			if(mRightMargin>mWidth)
				mWidth = mRightMargin;
		}

		mRightItems->push_back(FloatingSegment(theSegment,mRightMargin - aWidth,mHeight));
		mCurRightItems.push_back(mRightItems->back());
	}

	CalcMargins();
}

// FUNCTION: WON 0x0070C560
void MultiLineBuilder::AddSegments(SegmentList *theSegmentList, LineVector *theLineVector,
								   FloatingVector *theLeftItems, FloatingVector *theRightItems,
								   int theMaxWidth)
{
	mSegmentList = theSegmentList;
	mLineVector = theLineVector;
	mLeftItems = theLeftItems;
	mRightItems = theRightItems;
	mSpecifiedWidth = theMaxWidth;
	mWidth = mHeight = 0;
	mLeftMargin = mRightMargin = 0;
	mLeftIndent = mRightIndent = 0;
	mMaxLeftHeight =  mMaxRightHeight = 0;
	mAlignment = HorzAlign_Left;
	mHaveDeferredItems = false;

	mLeftItems->clear();
	mRightItems->clear();
	mLineVector->clear();

	AddLine();

	SegmentList::iterator anItr = mSegmentList->begin();
	while(anItr!=mSegmentList->end())
	{
		SegmentInfo &anInfo = *anItr;
		LineSegment *aSegment = anInfo.mSegment.get();
		unsigned short aFlags = anInfo.mFlags;

		aSegment->SetMaxWidth(theMaxWidth-mLeftIndent-mRightIndent);
		if(aFlags&4 && (!CurLine()->IsEmpty() || 
			(aFlags&0x10) || (aFlags&0x20)))
				AddLine(aFlags&0x10?true:false,aFlags&0x20?true:false);

		if(aSegment->PerformInstructions(this))
		{
			if(aFlags&(1 | 2))
				AddFloatingSegment(aSegment,aFlags&1?true:false);
			else
			{
				if(CurLine()->GetWidth()+aSegment->GetWidth() > GetCurMaxLineWidth())
					aSegment->Wrap(this, CurLine()->IsEmpty());
				else
					AddSegmentToCurLine(aSegment);
			}
		}

		if(aFlags&8 && (!CurLine()->IsEmpty() || 
			(aFlags&0x10) || (aFlags&0x20)))
				AddLine(aFlags&0x10?true:false,aFlags&0x20?true:false);
	
		++anItr;
	}

	AlignCurLine();
	if(mHaveDeferredItems)
	{
		CurLine()->Clear();
		FlushDeferredItems();
	}

	if(mHeight<mMaxLeftHeight)
		mHeight = mMaxLeftHeight;

	if(mHeight<mMaxRightHeight)
		mHeight = mMaxRightHeight;
}

// FUNCTION: WON 0x0070d1d0
void MultiLineElement::SetMaxWidth(int theMaxWidth)
{
	GetMinWidth();


	MultiLineBuilder aBuilder;
	aBuilder.AddSegments(&mSegmentList, &mLineVector, &mFloatingItems[0], &mFloatingItems[1], theMaxWidth);

	if(aBuilder.mWidth > theMaxWidth)
		mWidth = aBuilder.mWidth;
	else
		mWidth = theMaxWidth;

	mHeight = aBuilder.mHeight;
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071c9b0
// (public: void __thiscall std::list<class WONAPI::LineSegment *,class std::allocator<class WONAPI::LineSegment *> >::_Insert(class std::list<class WONAP)
// TEMPLATE: WON 0x0071cba0
// (public: class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::Floating)
// TEMPLATE: WON 0x0071f570
// (public: class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::Floating)
// TEMPLATE: WON 0x00724060
// (struct WONAPI::FloatingSegment * __cdecl std::copy(class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI)
// TEMPLATE: WON 0x00724280
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct W)
// TEMPLATE: WON 0x007242f0
// (class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::FloatingSegment )

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071cb20
// (public: class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::Floating)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071ee80
// (protected: void __thiscall std::list<class WONAPI::LineSegment *,class std::allocator<class WONAPI::LineSegment *> >::_Incsize(unsigned int)    [?_Inc)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071ca60
// (public: void __thiscall std::list<struct WONAPI::FloatingSegment,class std::allocator<struct WONAPI::FloatingSegment> >::_Insert(class std::list<struc)
// TEMPLATE: WON 0x0071f010
// (protected: void __thiscall std::list<struct WONAPI::FloatingSegment,class std::allocator<struct WONAPI::FloatingSegment> >::_Incsize(unsigned int))
// TEMPLATE: WON 0x00724200
// (void __cdecl std::_Construct(struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment const &))
// TEMPLATE: WON 0x00724240
// (protected: struct WONAPI::FloatingSegment * __thiscall std::vector<struct WONAPI::FloatingSegment,class std::allocator<struct WONAPI::FloatingSegment>)
// TEMPLATE: WON 0x0071cac0
// (public: class std::_Ptrit<struct WONAPI::FloatingSegment,int,struct WONAPI::FloatingSegment *,struct WONAPI::FloatingSegment &,struct WONAPI::Floating)
// TEMPLATE: WON 0x0071cbc0
// (public: class std::_Ptrit<class WONAPI::SmartPtr<class WONAPI::LineElement>,int,class WONAPI::SmartPtr<class WONAPI::LineElement> *,class WONAPI::Smar)
// TEMPLATE: WON 0x0071f160
// (public: void __thiscall std::vector<struct WONAPI::FloatingSegment,class std::allocator<struct WONAPI::FloatingSegment> >::insert(class std::_Ptrit<st)
// TEMPLATE: WON 0x0071f590
// (public: void __thiscall std::vector<class WONAPI::SmartPtr<class WONAPI::LineElement>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::LineEl)
// TEMPLATE: WON 0x0070d3e0
// (public: __thiscall WONAPI::MultiLineBuilder::~MultiLineBuilder(void))
