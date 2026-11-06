// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK MS scroll/tool controls.
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySimpleConfig.h"
#include "ee/wonapi/WONLobbyMSToolControls.h"
namespace WONAPI {
MSScrollButtonContainer::MSScrollButtonContainer(ScrollArea *theScrollArea)
{
	SmartPtr<RectangleComponent> aBack = new RectangleComponent();
	aBack->SetSolidColor(StandardColor_3DFace);
	Fn2F6140(aBack.get(),3,this,0,0,0,0,0);
	Fn2F6A60(aBack.get());
	mLeftButton = new MSArrowButton(Direction_Left);
	mLeftButton->Fn2F4B90(15,15);
	mRightButton = new MSArrowButton(Direction_Right);
	mRightButton->Fn2F4B90(15,15);
	mUpButton = new MSArrowButton(Direction_Up);
	mUpButton->Fn2F4B90(15,15);
	mDownButton = new MSArrowButton(Direction_Down);
	mDownButton->Fn2F4B90(15,15);

	mScrollArea = theScrollArea;
//	AddControls();
}

MSToolTip::MSToolTip(const GUIString &theText)
{
//	mFont = GetFont(FontDescriptor("Tahoma",FontStyle_Plain,8));
	mWrappedText->SetFont(GetNamedFont("MSButton"));
	SetHorzAlign(HorzAlign_Center);
	mAlign3C=VertAlign_Center;
	SetText(theText,true);
	mBackground=Background(0xffffdd,0x000000);
	if(mBackground.GetWantGrabBG()) SetComponentFlags(0x100,true);
	SetComponentFlags(0x100,true);
//	SetSize(mFont->GetStringWidth(theText)+6,mFont->GetHeight()+4);
	int width=0,height=0;
	Virtual0C(width,height);
	Fn2F4B90(width+6,height+4);
}

MSToolButton::MSToolButton(const GUIString &theText)
{
	mFocusStyle = FocusStyle_None;
	mHAlign = HorzAlign_Left;
	mLeftTextPad = 3;

	SetText(theText,true);
//	SetSize(mFont->GetStringWidth(theText)+10,mFont->GetHeight()+4);
	int width=0,height=0;
	Virtual0C(width,height);
	Fn2F4B90(width+10,height+4);
	Fn2F3370(0x0001,true);
}

void MSToolButton::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);

	bool down = (mButtonFlags&0x0020)!=0;
	bool over = !down && IsMouseOver();
	int offset = down?1:0;

	g.SetColor(0xD0D0D0);
	g.FillRect(0,0,Width(),Height());

	if(down || over)
	{
		g.SetColor(down?0x848484:0xffffff);
		g.DrawLine(0,0,Width()-1,0);
		g.DrawLine(0,0,0,Height()-1);
	
		g.SetColor(over?0x848484:0xffffff);
		g.DrawLine(0,Height()-1,Width()-1,Height()-1);
		g.DrawLine(Width()-1,0,Width()-1,Height()-1);
	}

	DrawButtonText(g,offset,offset);
}
}
