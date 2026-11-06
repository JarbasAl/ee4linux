// Splitter geometry and input behavior on original-owned typed receivers.
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
#include "ee/wonapi/WONLobbySplitterConfig.h"
namespace WONAPI {

// FUNCTION: WON 0x00707300
void Splitter::RegisterMaxButton(Button *theButton, QuadrantPosition thePosition)
{
	theButton->mControlId=thePosition + 100005;
	mButtonList.push_back(theButton);
}

// FUNCTION: WON 0x00707440
void Splitter::AddQuadrant(Component *theComponent, QuadrantPosition thePosition)
{
	mQuadrantList.push_back(Quadrant(theComponent,thePosition));
	Fn2F6A60(theComponent);
}

// FUNCTION: WON 0x007074b0
void Splitter::AddSplitterPrv(Component *theSplitter, SplitterType theType, Component *theDragComponent)
{
	mSplitterList.push_back(SplitterParam(theSplitter,theType));
	SplitterParam &aParam = mSplitterList.back();

	theDragComponent->SetComponentFlags(0x20,true);
	if(theDragComponent->Fn2F4B00()==NULL)
	{
		if(theType==Splitter_Center)
			theDragComponent->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_NorthSouthEastWest));
		else if(theType & Splitter_LeftRight)
			theDragComponent->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_EastWest));
		else if(theType & Splitter_UpDown)
			theDragComponent->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_NorthSouth));
	}
	aParam.mDragComponent = theDragComponent;

	Fn2F6A60(theSplitter);

	if(mLeftRightSplitterWidth==0 && (theType & Splitter_LeftRight))
		mLeftRightSplitterWidth = theSplitter->Width();

	if(mUpDownSplitterHeight==0 && (theType & Splitter_UpDown))
		mUpDownSplitterHeight = theSplitter->Height();
}

// FUNCTION: WON 0x00707670
void Splitter::AddSplitter(Component *theComponent, SplitterType theType)
{
	AddSplitterPrv(theComponent,theType,theComponent);
}

// FUNCTION: WON 0x00707690
void Splitter::AddComplexSplitter(Container *theContainer, SplitterType theType, Component *theDragComponent)
{
	if(theDragComponent==NULL)
	{
		SmartPtr<Component> aDragComponent = new Component;
		theContainer->Fn2F6140(aDragComponent.get(),3,theContainer,0,0,0,0,0);
		theContainer->AddChildEx(aDragComponent.get(),0);
		theDragComponent = aDragComponent.get();
	}

	AddSplitterPrv(theContainer,theType,theDragComponent);
}

// FUNCTION: WON 0x00707760
void Splitter::AddInvisibleSplitter(SplitterType theType, int theSize)
{
	SmartPtr<Component> aComp = new Component;
	aComp->Fn2F4B90(theSize,theSize);
	AddSplitter(aComp.get(),theType);
}

// FUNCTION: WON 0x00707810
void Splitter::MaxQuadrant(QuadrantPosition thePosition)
{
	mHaveMaxQuadrant = true;
	mCurMaxQuadrant = thePosition;

	mLeftRightMax = mUpDownMax = true;

	int x = mSplitX, y = mSplitY;
	if(thePosition & Position_Left)
		x = Width()-mLeftRightSplitterWidth/2;
	else if(thePosition & Position_Right)
		x = mLeftRightSplitterWidth/2;
	else 
		mLeftRightMax = false;

	if(thePosition & Position_Top)
		y = Height()-mUpDownSplitterHeight/2;
	else if(thePosition & Position_Bottom)
		y = mUpDownSplitterHeight/2;
	else
		mUpDownMax = false;


	for(ButtonList::iterator aButtonItr = mButtonList.begin(); aButtonItr!=mButtonList.end(); ++aButtonItr)
	{
		Button *aButton = (*aButtonItr).get();
		int aPosition = aButton->mControlId - 100006 + Position_Left;
		aButton->SetCheck(aPosition==thePosition,false);
	}

	SplitterList::iterator aSplitterItr = mSplitterList.begin();
	while(aSplitterItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *aSplitterItr;
		Component *aComponent = aParam.mComponent;
		if(aParam.mType & thePosition)
			aParam.mDragComponent->SetEnabled(false);
		else
			aParam.mDragComponent->SetEnabled(true);

		++aSplitterItr;
	}

	QuadrantList::iterator aQuadrantItr = mQuadrantList.begin();
	while(aQuadrantItr!=mQuadrantList.end())
	{
		Quadrant &aParam = *aQuadrantItr;
		Component *aComponent = aParam.mComponent;
		if(aParam.mPosition & thePosition)
			aComponent->SetVisible(true);
		else
			aComponent->SetVisible(false);

		++aQuadrantItr;
	}

	SetSplitterPos(x,y,false);
}

// FUNCTION: WON 0x00707990
void Splitter::UnMaxQuadrant()
{
	mHaveMaxQuadrant = false;
	mUpDownMax = mLeftRightMax = false;

	SplitterList::iterator aSplitterItr = mSplitterList.begin();
	while(aSplitterItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *aSplitterItr;
		Component *aComponent = aParam.mComponent;
		aParam.mDragComponent->SetEnabled(true);

		++aSplitterItr;
	}

	QuadrantList::iterator aQuadrantItr = mQuadrantList.begin();
	while(aQuadrantItr!=mQuadrantList.end())
	{
		Quadrant &aParam = *aQuadrantItr;
		Component *aComponent = aParam.mComponent;
		aComponent->SetVisible(true);
		++aQuadrantItr;
	}

	SetSplitterPos(Width()*mLeftPercent/100,Height()*mTopPercent/100,false);
}

// FUNCTION: WON 0x00707a50
void Splitter::CalcQuadrants()
{
	QuadrantList::iterator anItr = mQuadrantList.begin();
	while(anItr!=mQuadrantList.end())
	{
		Quadrant &aParam = *anItr;
		Component *aComponent = aParam.mComponent;
		int aPosition = aParam.mPosition;

		int left = 0, right = Width(), top = 0, bottom = Height();

		if(aPosition & Position_Left)
			right = mSplitLeft;

		if(aPosition & Position_Right)
			left = mSplitRight;

		if(aPosition & Position_Top)
			bottom = mSplitTop;

		if(aPosition & Position_Bottom)
			top = mSplitBottom;

		aComponent->Fn2F4BF0(left,top,right-left,bottom-top);
		++anItr;
	}

}

// FUNCTION: WON 0x00707ad0
void Splitter::CalcSplitterPositions()
{
	SplitterList::iterator anItr = mSplitterList.begin();
	while(anItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *anItr;
		Component *aComponent = aParam.mComponent;
		if(aComponent->IsVisible())
		{
			int left = 0, top = 0, right = Width(), bottom = Height();

			if(aParam.mType & Splitter_LeftRight)
			{
				left = mSplitLeft;
				right = mSplitRight;
			}

			if(aParam.mType & Splitter_UpDown)
			{
				top = mSplitTop;
				bottom = mSplitBottom;
			}

			switch(aParam.mType)
			{
				case Splitter_LeftRight_Top: bottom = mSplitTop; break;
				case Splitter_LeftRight_Bottom: top = mSplitBottom; break;
				case Splitter_UpDown_Left: right = mSplitLeft; break;
				case Splitter_UpDown_Right: left = mSplitRight; break;
			}

			aComponent->Fn2F4BF0(left,top,right-left,bottom-top);
		}
		++anItr;
	}
}

// FUNCTION: WON 0x00707c30
void Splitter::SetMinLeftRight(int minLeft, int minRight)
{
	mMinLeft = minLeft;
	mMinRight = minRight;
}

// FUNCTION: WON 0x00707c50
void Splitter::SetMinTopBottom(int minTop, int minBottom)
{
	mMinTop = minTop;
	mMinBottom = minBottom;
}

// FUNCTION: WON 0x00707c70
void Splitter::SetPercentages(int theLeftPercent, int theTopPercent)
{
	mLeftPercent = theLeftPercent;
	mTopPercent = theTopPercent;
}

// FUNCTION: WON 0x00707c90
void Splitter::SetSplitterPos(int x, int y, bool rememberPercentage)
{
	if(!mLeftRightMax)
	{
		if(x < mMinLeft)
			x = mMinLeft;
		if(x > Width()-mMinRight)
			x = Width()-mMinRight;
	}

	if(!mUpDownMax)
	{
		if(y < mMinTop)
			y = mMinTop;
		if(y > Height()-mMinBottom)
			y = Height()-mMinBottom;
	}

	if(mSplitCoordsValid && x==mSplitX && y==mSplitY)
		return;

	mSplitCoordsValid = true;
	mSplitX = x;
	mSplitY = y;
	mSplitLeft = x - mLeftRightSplitterWidth/2;
	mSplitRight = x + mLeftRightSplitterWidth/2;
	mSplitTop = y - mUpDownSplitterHeight/2;
	mSplitBottom = y + mUpDownSplitterHeight/2;

	if(rememberPercentage)
	{
		if(Width() > 0)
			mLeftPercent = x*100.0/Width();
		if(Height() > 0)
			mTopPercent = y*100.0/Height();
	}

	CalcSplitterPositions();
	CalcQuadrants();
	Invalidate();

}

// FUNCTION: WON 0x00707de0
void Splitter::Virtual08()
{
	Container::Virtual08();

	mSplitCoordsValid = false;
	double aLeftPercent = mLeftPercent;
	double aTopPercent = mTopPercent;
	if(mHaveMaxQuadrant)
	{
		int xoff = 0, yoff = 0;
		if(mCurMaxQuadrant & Position_Left)
		{
			aLeftPercent = 100;
			xoff = -mLeftRightSplitterWidth/2;
		}
		else if(mCurMaxQuadrant & Position_Right)
		{
			aLeftPercent = 0;
			xoff = mLeftRightSplitterWidth/2;
		}

		if(mCurMaxQuadrant & Position_Top)
		{
			aTopPercent = 100;
			yoff = -mUpDownSplitterHeight/2;
		}
		else if(mCurMaxQuadrant & Position_Bottom)
		{
			aTopPercent = 0;
			yoff = mUpDownSplitterHeight/2;
		}

		SetSplitterPos(Width()*aLeftPercent/100 + xoff, Height()*aTopPercent/100 + yoff,false);
	}
	else		
		SetSplitterPos(Width()*aLeftPercent/100,Height()*aTopPercent/100,false);
}

// FUNCTION: WON 0x00707f30
void Splitter::Virtual2C(int x, int y, int theButton)
{
	Container::Virtual2C(x,y,theButton);
	if(theButton!=0 || mMouseChild==NULL)
		return;

	mLeftRightDrag = mUpDownDrag = false;
	mDragX = x - mMouseChild->Left();
	mDragY = y - mMouseChild->Top();

	Component *aGrandChild = mMouseChild->Virtual98();
	if(aGrandChild==NULL)
	{
		if(!mMouseChild->ComponentFlagSet(0x20))
			return;
	}
	else if(!aGrandChild->ComponentFlagSet(0x20))
		return;

	SplitterList::iterator anItr = mSplitterList.begin();
	while(anItr!=mSplitterList.end())
	{
		SplitterParam &aParam = *anItr;
		if(aParam.mComponent==mMouseChild)
		{
			if(aParam.mType & Splitter_UpDown)
				mUpDownDrag = true;

			if(aParam.mType & Splitter_LeftRight)
				mLeftRightDrag = true;

			break;
		}

		++anItr;
	}
}

// FUNCTION: WON 0x00707ff0
void Splitter::Virtual30(int x, int y, int theButton)
{
	Container::Virtual30(x,y,theButton);
	if(theButton!=0 || mMouseChild==NULL)
		return;

	mLeftRightDrag = mUpDownDrag = false;
}

// FUNCTION: WON 0x00708030
void Splitter::Virtual38(int x, int y)
{
	Container::Virtual38(x,y);
	if(mMouseChild!=NULL)
	{
		int aDeltaX = 0, aDeltaY = 0;
		if(mLeftRightDrag && !mLeftRightMax)
			aDeltaX = x - mMouseChild->Left() - mDragX;

		if(mUpDownDrag && !mUpDownMax)
			aDeltaY = y - mMouseChild->Top() - mDragY;

		SetSplitterPos(mSplitX+aDeltaX, mSplitY+aDeltaY);


	}
}

// FUNCTION: WON 0x007080b0
bool Splitter::CheckMaxButton(ComponentEvent *theEvent)
{
	if(theEvent->mType!=15)
		return false;

	if(theEvent->GetControlId()<100006 || theEvent->GetControlId()>100017)
		return false;

	Button *aButton = dynamic_cast<Button*>(theEvent->mComponent);
	if(aButton==NULL)
		return false;
	
	if(!aButton->IsChecked())
	{
		UnMaxQuadrant();
		return true;
	}

	int aType = theEvent->GetControlId() - 100006 + Position_Left;
	MaxQuadrant((QuadrantPosition)aType);
	return true;
}

// FUNCTION: WON 0x00708140
void Splitter::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(CheckMaxButton(theEvent))
		return;
	else 
		Container::HandleComponentEvent(theEvent);
}
}

namespace WONAPI {
// FUNCTION: WON 0x00707060
Splitter::Splitter()
{
    mSplitLeft=0;
    mSplitRight=0;
    mSplitTop=0;
    mSplitBottom=0;
    mUpDownSplitterHeight=0;
    mLeftRightSplitterWidth=0;
    mHaveMaxQuadrant=false;
    mMinLeft=mMinRight=mMinTop=mMinBottom=0;
    mLeftPercent=mTopPercent=50;
    mUpDownMax=false;
    mLeftRightMax=false;
    mLeftRightDrag=false;
    mUpDownDrag=false;
    mSplitX=0;
    mSplitY=0;
    mSplitCoordsValid=false;
    SetComponentFlags(0x2000,true);
}
// FUNCTION: WON 0x007071e0
// The real three lists clean up naturally in reverse declaration order.
Splitter::~Splitter() {}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007071c0
// (protected: virtual void * __thiscall WONAPI::Splitter::`scalar deleting destructor'(unsigned int)    [??_GSplitter@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x00707640
// (public: __thiscall WONAPI::Splitter::SplitterParam::~SplitterParam(void)    [??1SplitterParam@Splitter@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x0071b720
// (public: __thiscall std::list<struct WONAPI::Splitter::SplitterParam,class std::allocator<struct WONAPI::Splitter::SplitterParam> >::~list<struct WONAP)
// TEMPLATE: WON 0x007221c0
// (public: class std::list<struct WONAPI::Splitter::SplitterParam,class std::allocator<struct WONAPI::Splitter::SplitterParam> >::iterator __thiscall std)
// TEMPLATE: WON 0x00724cd0
// (void __cdecl std::_Construct(struct WONAPI::Splitter::SplitterParam *,struct WONAPI::Splitter::SplitterParam const &)    [?_Construct@std@@YAXPAUSplit)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00722070
// (protected: void __thiscall std::list<struct WONAPI::Splitter::Quadrant,class std::allocator<struct WONAPI::Splitter::Quadrant> >::_Incsize(unsigned in)
