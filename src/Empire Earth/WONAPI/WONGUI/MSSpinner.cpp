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
static const int MSSpinner_ButtonWidth = 13;
void MSSpinner::Init()
{
	mLabel = new Label;
	mLabel->mAlign3C=VertAlign_Center;
	Fn2F6A60(mLabel);

	mInputBox = new MSInputBox();
	mInputBox->mInputFilter=InputBox::NumericFilter;

	mUpArrow = new MSArrowButton(Direction_Up);
	mDownArrow = new MSArrowButton(Direction_Down);
	mUpArrow->SetComponentFlags(0x000008,true);
	mDownArrow->SetComponentFlags(0x000008,true);
	mUpArrow->SetComponentFlags(0x004000,false);
	mDownArrow->SetComponentFlags(0x004000,false);

	Fn2F6140(mLabel,0x0002,this,0,0,0,0,0);
	Fn2F6140(mUpArrow,0x6000,this,0,0,0,0,0);
	Fn2F6140(mDownArrow,0xa000,this,0,0,0,0,0);
	Fn2F6140(mInputBox,0x0402,mLabel,2,0,0,0,0);
	Fn2F6140(mInputBox,0x20000,mUpArrow,0,0,0,0,0);

	AddControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSpinner::MSSpinner()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSpinner::MSSpinner(const GUIString &theText, int minValue, int maxValue)
{
	Init();
	SetLabelText(theText);
	SetRange(minValue,maxValue,false);
	Fn2F4E90(0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSpinner::SetLabelText(const GUIString &theText)
{
	mLabel->SetText(theText);
	mLabel->SetDesiredWidth(0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSpinner::Virtual0C(int &width, int &height)
{
	int maxDigits = 0;
	int aNum = mMaxValue;
	do
	{
		maxDigits++;
		aNum/=10;
	} while(aNum>0);

	if(mMinValue<0)
		maxDigits++;

	mInputBox->Virtual0C(width,height);

	Font *aFont = mInputBox->mFont.get();
	int anInputWidth = aFont->Virtual10('0')*maxDigits + 10;
	width = mLabel->Width() + 2 + anInputWidth + MSSpinner_ButtonWidth;

	// want height/2 is odd -> height/2 = 2n+1 -> height = 4n+2
	int rem = (height+2)%4; // make sure height is divisible by 4
	if(rem!=0)
		height += (4-rem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSpinner::Virtual08()
{
	int nWidth = Width();
	int nHeight = Height();
	int nBtnHt = nHeight/2;
	int nBtnWd = MSSpinner_ButtonWidth;
	mUpArrow->Fn2F4B90(nBtnWd, nBtnHt);
	mDownArrow->Fn2F4B90(nBtnWd, nBtnHt);

	Container::Virtual08();
}


}
