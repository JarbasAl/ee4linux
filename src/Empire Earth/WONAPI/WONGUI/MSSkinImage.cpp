// Native MS skin program-image rendering; no receiver allocation/lifecycle source.
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
#include "ee/wonapi/WONLobbyMSSkinImage.h"
namespace WONAPI {

// FUNCTION: WON 0x00717110
void WONMSSkin::MSImage::DrawPushButton(Graphics &g, bool useFrame2, bool checkDefault)
{

	bool down = FlagSet(MSImageFlag_Down);

	int x = 0, y = 0, width = mWidth, height = mHeight;

	g.SetSysColor(StandardColor_3DFace);
	g.FillRect(0,0,width,height);

	if(checkDefault && (FlagSet(MSImageFlag_Default) || down))
	{
		g.SetSysColor(StandardColor_3DDarkShadow);
		g.DrawRect(0,0,width,height);
		x++;
		y++;
		width-=2;
		height-=2;
	
		if(down)
			useFrame2 = true;
	}

	if(useFrame2)	
		LobbyDrawFrame(g,x,y,width,height,down);
	else
		LobbyDraw3DFrame(g,x,y,width,height,down);
}

// FUNCTION: WON 0x007171E0
void WONMSSkin::MSImage::DrawArrowButton(Graphics &g, Direction theDirection)
{
	DrawPushButton(g,true,false);
	if(FlagSet(MSImageFlag_Down))
		MSGraphicsUtil::DrawArrow(g,theDirection,1,1,mWidth,mHeight,FlagSet(MSImageFlag_Disabled));
	else
		MSGraphicsUtil::DrawArrow(g,theDirection,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Disabled));
}

// FUNCTION: WON 0x00717250
void WONMSSkin::MSImage::DrawCloseButton(Graphics &g)
{
	DrawPushButton(g,false,false);
	if(FlagSet(MSImageFlag_Down))
		MSGraphicsUtil::DrawCloseX(g,1,1,mWidth,mHeight);
	else
		MSGraphicsUtil::DrawCloseX(g,0,0,mWidth,mHeight);
}

// FUNCTION: WON 0x007172B0
void WONMSSkin::MSImage::Paint(Graphics &g)
{
	switch(mImageType)
	{
		case MSImageType_Frame1:		LobbyDraw3DFrame(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down)); return;
		case MSImageType_Frame2:		LobbyDrawFrame(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down)); return;
		case MSImageType_Frame3:		MSGraphicsUtil::DrawFrame3(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down)); return;

		case MSImageType_LeftArrow:		DrawArrowButton(g,Direction_Left);	return;
		case MSImageType_UpArrow:		DrawArrowButton(g,Direction_Up);	return;
		case MSImageType_RightArrow:	DrawArrowButton(g,Direction_Right);	return;
		case MSImageType_DownArrow:		DrawArrowButton(g,Direction_Down);	return;
		
		case MSImageType_PushButton:	DrawPushButton(g,false,true);	return;
		case MSImageType_TabCtrlFrame:	DrawPushButton(g,false,false); return;
		case MSImageType_ListHeader:	DrawPushButton(g,true,false);   return;
		case MSImageType_ScrollThumb:	DrawPushButton(g,true,false); return;
		case MSImageType_CloseButton:	DrawCloseButton(g); return;	

		case MSImageType_TabButton:		MSGraphicsUtil::DrawTab(g,0,0,mWidth,mHeight); return;
		case MSImageType_Checkbox:		MSGraphicsUtil::DrawCheck(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down),FlagSet(MSImageFlag_Checked)); return;
		case MSImageType_RadioButton:	MSGraphicsUtil::DrawRadio(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down),FlagSet(MSImageFlag_Checked)); return;

		case MSImageType_ScrollGutter:	g.SetSysColor(StandardColor_Scrollbar); g.FillRect(0,0,mWidth,mHeight); return;
		case MSImageType_ScrollCorner:  g.SetSysColor(StandardColor_3DFace); g.FillRect(0,0,mWidth,mHeight); return;
		
		case MSImageType_ComboPopupFrame: g.SetSysColor(StandardColor_3DDarkShadow); g.DrawRect(0,0,mWidth,mHeight); return;
	}

}
}
