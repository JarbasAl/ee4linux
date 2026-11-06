// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine same-SDK specialized controls.
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyBorderComponent.h"
namespace WONAPI {
// Real SimpleComponent.cpp BorderComponent family, no other existing types duplicated.
BorderComponent::BorderComponent()
{
//	SetComponentFlags(ComponentFlag_NoInput,true);
	mOnlyChildInput = true;
	mControlsAdded = false;
}

void BorderComponent::SetComponent(BorderPos thePos, Component *theComponent, int theWidth, int theHeight)
{
	mComponents[thePos] = theComponent;
	if(theWidth!=-1 && theHeight!=-1)
		theComponent->Fn2F4B90(theWidth,theHeight);	
}

void BorderComponent::AddControls()
{
	for(int i=0; i<8; i++)
	{
		if(mComponents[i].get()==NULL)
			mComponents[i] = new Component;
			
		Fn2F6A60(mComponents[i]);
	}

	mLeftTop = mComponents[Pos_LeftTop];
	mRightTop = mComponents[Pos_RightTop];
	mLeftBottom = mComponents[Pos_LeftBottom];
	mRightBottom = mComponents[Pos_RightBottom];
	
	mLeft = mComponents[Pos_Left];
	mTop = mComponents[Pos_Top];
	mRight = mComponents[Pos_Right];
	mBottom = mComponents[Pos_Bottom];

	mControlsAdded = true;
	Virtual08();
}

void BorderComponent::Virtual08()
{
	Container::Virtual08();

	if(!mControlsAdded)
		return;

	mLeftTop->Fn2F4B70(0,0); 
	mRightTop->Fn2F4B70(Width()-mRightTop->Width(),0);
	mLeftBottom->Fn2F4B70(0,Height()-mLeftBottom->Height()); 
	mRightBottom->Fn2F4B70(Width()-mRightBottom->Width(),Height()-mRightBottom->Height());
	
	mLeft->Fn2F4BF0(0,(mLeftTop->Top()+mLeftTop->Height()), mLeft->Width(), mLeftBottom->Top() - (mLeftTop->Top()+mLeftTop->Height()));
	mRight->Fn2F4BF0(Width()-mRight->Width(), (mRightTop->Top()+mRightTop->Height()), mRight->Width(), mRightBottom->Top() - (mRightTop->Top()+mRightTop->Height()));
	mTop->Fn2F4BF0((mLeftTop->Left()+mLeftTop->Width()), 0, mRightTop->Left() - (mLeftTop->Left()+mLeftTop->Width()), mTop->Height());
	mBottom->Fn2F4BF0((mLeftBottom->Left()+mLeftBottom->Width()), Height()-mBottom->Height(), mRightBottom->Left() - (mLeftBottom->Left()+mLeftBottom->Width()), mBottom->Height());
}
}
