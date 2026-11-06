// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK spinner value/event/control family.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <cstdio>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyMSBorderPainters.h"
#include "ee/wonapi/WONLobbySpinner.h"
namespace WONAPI {
Spinner::Spinner()
{
	mMinValue   = 0;
	mMaxValue   = 1000;

	mValue = 0;
	mValueIsValid = true;
	mWrapAround = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::Virtual0C(int &width, int &height)
{
	width = 40;
	height = 22;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::Virtual6C()
{
	if(mUpArrow->mParent!=this)
		AddControls();

	Container::Virtual6C();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::AddControls()
{
	mUpArrow->Fn2F3370(0x0002,true);
	mDownArrow->Fn2F3370(0x0002,true);	

	Fn2F6A60(mInputBox);
	Fn2F6A60(mUpArrow);
	Fn2F6A60(mDownArrow);

	SetRange(mMinValue,mMaxValue,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetTextToValue()
{
	if(!mValueIsValid)
	{
		mInputBox->Clear(true);
		return;
	}

	char aBuf[32];
	sprintf(aBuf,"%d",mValue);
	mInputBox->SetText(aBuf,false);
	mInputBox->SelectRange(-1,-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Spinner::Virtual54(int theKey)
{
	if(theKey==VK_UP)
		IncrementValue();
	else if(theKey==VK_DOWN)
		DecrementValue();
	else
		return Container::Virtual54(theKey);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::Virtual74(bool isEnabled)
{
	Container::Virtual74(isEnabled);

	mInputBox->SetEnabled(isEnabled);
	mUpArrow->SetEnabled(isEnabled);
	mDownArrow->SetEnabled(isEnabled);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::IncrementValue(int theAmount)
{
	bool oldValid = mValueIsValid;
	int oldValue = mValue;

	mValue += theAmount;
	if(mWrapAround)
	{
		if(mValue > mMaxValue)
			mValue = mMinValue;
		else if(mValue < mMinValue)
			mValue = mMaxValue;
	}
	else
		ForceValueCompliance();
	
	SetTextToValue();

	CheckValueChanged(oldValid,oldValue);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::DecrementValue(int theAmount)
{
	IncrementValue(-theAmount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetMax(int theMax) 
{ 
	SetRange(mMinValue, theMax);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetMin(int theMin) 
{ 
	SetRange(theMin, mMaxValue);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetRange(int theMin, int theMax, bool sendEvent) 
{ 
	bool oldValid = mValueIsValid;
	int oldValue = mValue;

	mMinValue = theMin;
	mMaxValue = theMax;
	ForceValueCompliance();
	SetTextToValue();

	int maxDigits = 0;
	int aNum = mMaxValue;
	do
	{
		maxDigits++;
		aNum/=10;
	} while(aNum>0);

	if(mMinValue<0)
		maxDigits++;

	mInputBox->mMaxChars=maxDigits;

	if(sendEvent)
		CheckValueChanged(oldValid,oldValue);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetValue(int theValue, bool sendEvent)
{
	bool oldValid = mValueIsValid;
	int oldValue = mValue;

	if(theValue>=mMinValue && theValue<=mMaxValue)
	{
		mValue = theValue;
		mValueIsValid = true;
		SetTextToValue();
		if(sendEvent)
			CheckValueChanged(oldValid,oldValue);
	}


}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::ForceValueCompliance()
{
	if(mMaxValue < mMinValue)
	{
		mValueIsValid = false;
		return;
	}
	
	if(mValue > mMaxValue)
		mValue = mMaxValue;
	else if(mValue < mMinValue)
		mValue = mMinValue;

	mValueIsValid = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetWrapAround(bool wrap)
{
	mWrapAround = wrap;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::SetNoType(bool noType)
{
	mInputBox->SetComponentFlags(0x000008,!noType);
	mInputBox->Fn2F84B0(!noType);
	if(noType && mFocusChild==mInputBox.get())
		VirtualB4(NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::CalcValueFromText()
{
	bool oldValid = mValueIsValid;
	int oldValue = mValue;

	if(mInputBox->GetText().length()==0)
		mValueIsValid = false;
	else
	{
		mValue = mInputBox->GetText().mData->ToInt(0);
		mValueIsValid = mValue>=mMinValue && mValue<=mMaxValue;
	}

	CheckValueChanged(oldValid, oldValue);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int Spinner::GetValue()
{
	return mValue;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Spinner::IsValueValid()
{
	return mValueIsValid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::CheckValueChanged(bool oldValid, int oldValue)
{
	bool changed = false;
	if(oldValid!=mValueIsValid)
		changed = true;
	else if(mValueIsValid && oldValue!=mValue)
		changed = true;

	if(changed)
		FireEvent(ComponentEvent_SpinnerValueChanged);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Spinner::HandleComponentEvent(ComponentEvent* pRawEvent)
{
	if (pRawEvent->mType == ComponentEvent_ButtonPressed)
	{
		int nAdj = 1;
		if ((GetKeyMod() & KEYMOD_SHIFT) != 0)
			nAdj = 10;
		if ((GetKeyMod() & KEYMOD_CTRL) != 0)
			nAdj = 100;

		if (pRawEvent->mComponent == mUpArrow)
		{
			IncrementValue(nAdj);
			return;
		}
		else if (pRawEvent->mComponent == mDownArrow)
		{
			DecrementValue(nAdj);
			return;
		}
	}
	else if (pRawEvent->mType == ComponentEvent_InputTextChanged)
	{
		CalcValueFromText();
		return;
	}

	Container::HandleComponentEvent(pRawEvent);
}

}
