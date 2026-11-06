// Skin list headers: sort indicator rendering and initial header-height policy.
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
#include "ee/wonapi/WONLobbySkinListControls.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"
namespace WONAPI {

// FUNCTION: WON 0x007193C0
void WONHeaderButton::Paint(Graphics &g)
{
	WONButton::Paint(g);

	ListSortType aSortType = MultiListHeader::GetSortTypeForButton(this);
	if(aSortType==ListSortType_None)
		return;

	int aColor;
	WrappedText *aText;

	GetButtonState(g,&aColor, &aText);
	g.SetColor(aColor);
	int x;
	if(aText==NULL || aText->GetWidth()==0)
		x = (Width()-4)/2;
	else
		x = aText->GetWidth() + 10 + mLeftTextPad;
	
	int y = Height()/2-2;
	int offset = ((mButtonFlags&0x20)!=0)?1:0;
	x+=offset;
	y+=offset;

	int i;
	if(aSortType==ListSortType_Ascending)
	{
		for(i=0; i<4; i++)
			g.DrawLine(x-i, y+i, x+i, y+i);	

	} 
	else if(aSortType==ListSortType_Descending)
	{
		for(i=0; i<4; i++)
			g.DrawLine(x-(3-i), y+i, x+(3-i), y+i);			
	}

}

// FUNCTION: WON 0x0071A450
void WONListCtrl::SetStandarColumnHeader(int theCol, const GUIString &theTitle, int theWidth)
{
	int aDesiredHeight = 0;
	mHeader->SetColumn(theCol,theTitle, &aDesiredHeight);

	if(mHeaderHeight==0)
	{
		if(aDesiredHeight>0)
			mHeaderHeight = aDesiredHeight;
		else
			mHeaderHeight = 15;
	}
		
	static_cast<MultiListArea*>(mScrollArea.get())->SetColumnWidth(theCol,theWidth,false);
}

// FUNCTION: WON 0x0071A4C0
void WONListCtrl::SetColumnHeader(int theCol, Component *theHeader, int theWidth)
{
	if(mHeaderHeight==0)
	{
		if(theHeader->Height()>0)
			mHeaderHeight = theHeader->Height();
		else
			mHeaderHeight = 15;
	}

	mHeader->SetColumnItem(theCol,theHeader);
	static_cast<MultiListArea*>(mScrollArea.get())->SetColumnWidth(theCol,theWidth,false);
}

// Support0 small geometry forwarding override31a580/48, no primary quota.
// FUNCTION: WON 0x0071a580
void WONListCtrl::Virtual08()
{
	WONScrollContainer::Virtual08();

	mHeader->Fn2F4BF0(mScrollArea->Left(),mScrollArea->Top()-mHeaderHeight,mScrollArea->Width(),mHeaderHeight);
}
}

namespace WONAPI {
// FUNCTION: WON 0x0071a060
WONHeaderControl::WONHeaderControl(WONSkin* skin)
{
    mSkin=skin;
    mTextLeftPad=0;
}
// FUNCTION: WON 0x0071a1c0
void WONHeaderControl::SetColumn(int column,const GUIString& title,int* desiredHeight)
{
    SmartPtr<WONHeaderButton> button=mSkin->GetHeaderButton();
    button->SetText(title,true);
    if(desiredHeight!=NULL) {
        int width;
        button->Virtual0C(width,*desiredHeight);
    }
    SetColumnItem(column,button.get());
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0071a100
// (public: virtual void * __thiscall WONAPI::WONHeaderControl::`scalar deleting destructor'(unsigned int)    [??_GWONHeaderControl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0071a120
// (public: virtual __thiscall WONAPI::WONHeaderControl::~WONHeaderControl(void)    [??1WONHeaderControl@WONAPI@@UAE@XZ])
