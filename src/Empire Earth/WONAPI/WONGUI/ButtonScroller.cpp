// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK algorithms, no Native/primary/physical claim.
ButtonScroller::ButtonScroller()
{
	mSettingScrollButtonConditions = false;
	mVertCondition = mHorzCondition = ScrollButtonCondition_Enable;
}

void ButtonScroller::Virtual6C()
{

	if(mScrollArea.get()!=NULL && mScrollArea->GetParent()!=this)
		AddControls();

	Scroller::Virtual6C();
}

void ButtonScroller::AddControls()
{
	mScrollArea->SetScroller(this);
	SmartPtr<Button>* buttons[] = {&mLeftButton, &mRightButton, &mUpButton, &mDownButton};
	for(int i=0; i<4; i++)
	{
		SmartPtr<Button> &aButton = *buttons[i];
		if(aButton.get()==NULL)
		{
			aButton = new Button;
			aButton->SetComponentFlags(0x800,true);
		}
	}

	Fn2F6A60(mScrollArea);
	Fn2F6A60(mLeftButton);
	Fn2F6A60(mRightButton);
	Fn2F6A60(mUpButton);
	Fn2F6A60(mDownButton);

	if(mScrollArea->mL0A4>0 || mScrollArea->mTotalHeight>0)
		NotifyScrollChange(false);
}

bool ButtonScroller::EnableScrollButtons(Button *theLeft, Button *theRight, ScrollButtonCondition theCondition)
{	
	if(theLeft==NULL || theRight==NULL)
		return false;

	bool showLeft = theLeft->IsVisible(), showRight = theRight->IsVisible();
	switch(theCondition)
	{
		case ScrollButtonCondition_Disable: showLeft = false; showRight = false; break;
		case ScrollButtonCondition_Enable: showLeft = true; showRight = true; break;
		case ScrollButtonCondition_Conditional: showLeft = showRight = !(theLeft->IsDisabled() && theRight->IsDisabled()); break;
		case ScrollButtonCondition_PartialConditional: showLeft = !theLeft->IsDisabled(); showRight = !theRight->IsDisabled(); break;
	}

	// Prevent situations where conditional scrollbars are enabled and disabled recursively since
	// it turns out that it's possible for the appearance of a scrollbar to change the wordwrapping
	// such that the scrollbar is no longer necessary.
	if(mSettingScrollButtonConditions)
	{
		if(!showLeft && theLeft->IsVisible())
			return false;
		if(!showRight && theRight->IsVisible())
			return false;
	}

	// Did anything change?
	bool changed = showLeft!=theLeft->IsVisible() || showRight!=theRight->IsVisible();
	theLeft->SetVisible(showLeft);
	theRight->SetVisible(showRight);
	return changed;
}

void ButtonScroller::SetScrollButtonConditions(ScrollButtonCondition theHorzCondition, ScrollButtonCondition theVertCondition)
{
	bool needChange = false;
	mHorzCondition = theHorzCondition;
	mVertCondition = theVertCondition;

	needChange = EnableScrollButtons(mLeftButton,mRightButton, mHorzCondition) || needChange;
	needChange = EnableScrollButtons(mUpButton, mDownButton, mVertCondition) || needChange;

	mSettingScrollButtonConditions = true;
	if(needChange)
	{
		Invalidate();
		Virtual08();
	}
	mSettingScrollButtonConditions = false;
}

void ButtonScroller::Virtual08()
{
	Scroller::Virtual08();
/*	if(mLeftButton.get()==NULL || mRightButton.get()==NULL || mUpButton.get()==NULL || mDownButton.get()==NULL)
		return;

	int l = 0, r = 0, u = 0, d = 0;
	if(mLeftButton->IsVisible())
	{
		mLeftButton->SetPos(0,(Height()-mLeftButton->Height())/2);
		l = mLeftButton->Width();
	}

	if(mRightButton->IsVisible())
	{
		mRightButton->SetPos(Width()-mRightButton->Width(),(Height()-mRightButton->Height())/2);
		r = mRightButton->Width();
	}

	if(mUpButton->IsVisible())
	{
		mUpButton->SetPos((Width()-mUpButton->Width())/2,0);
		u = mUpButton->Height();
	}

	if(mDownButton->IsVisible())
	{
		mDownButton->SetPos((Width()-mDownButton->Width())/2,(Height()-mDownButton->Height()));
		d = mDownButton->Height();
	}

	mScrollArea->SetPosSize(l,u,Width()-l-r,Height()-u-d);*/
}

void ButtonScroller::SetButton(Button *theButton, Direction theDirection)
{
	switch(theDirection)
	{
		case Direction_Left: mLeftButton = theButton; break;
		case Direction_Right: mRightButton = theButton; break;
		case Direction_Up:	mUpButton = theButton; break;
		case Direction_Down: mDownButton = theButton; break;
		default:
			return;
	}

	theButton->AddListener(new ComponentListenerComp(this));
}

void ButtonScroller::NotifyScrollChange(bool becauseOfScroller)
{
	if(mLeftButton.get()!=NULL)
		mLeftButton->SetEnabled(mScrollArea->mHorzOffset>0);

	if(mUpButton.get()!=NULL)
		mUpButton->SetEnabled(mScrollArea->mVertOffset>0);

	if(mRightButton.get()!=NULL)
		mRightButton->SetEnabled(mScrollArea->mHorzOffset + mScrollArea->mScrollArea.width < mScrollArea->mL0A4);

	if(mDownButton.get()!=NULL)
		mDownButton->SetEnabled(mScrollArea->mVertOffset + mScrollArea->mScrollArea.height < (mScrollArea->mTotalHeight+mScrollArea->mExtraHeight));

	SetScrollButtonConditions(mHorzCondition, mVertCondition);
}

void ButtonScroller::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==15)
	{
		if(theEvent->mComponent==mLeftButton)
			SetHorzOffset(mScrollArea->mHorzOffset-mScrollArea->mLeftScrollAmount);
		else if(theEvent->mComponent==mRightButton)
			SetHorzOffset(mScrollArea->mHorzOffset+mScrollArea->mRightScrollAmount);
		else if(theEvent->mComponent==mUpButton)
			SetVertOffset(mScrollArea->mVertOffset-mScrollArea->mUpScrollAmount);
		else if(theEvent->mComponent==mDownButton)
			SetVertOffset(mScrollArea->mVertOffset+mScrollArea->mDownScrollAmount);
		else
			Scroller::HandleComponentEvent(theEvent);
	}
	else
		Scroller::HandleComponentEvent(theEvent);
}
}
