// Original skin frame and simple image button behavior on borrowed owners.
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
#include "ee/wonapi/WONLobbySkinFrame.h"
namespace WONAPI {

// FUNCTION: WON 0x00717A90
void WONSimpleButton::Virtual0C(int &width, int &height)
{
	width = height = 0;
	for(int i=0; i<4; i++)
	{
		Image *anImage = mImages[i].get();
		if(anImage!=NULL)
		{
			if(anImage->mWidth>width)
				width = anImage->mWidth;

			if(anImage->mHeight>height)
				height = anImage->mHeight;
		}
	}
}

// FUNCTION: WON 0x00717BD0
void WONSimpleButton::Paint(Graphics &g)
{
	Button::Paint(g);
	Image *anImage = NULL;
	if(IsDisabled())
		anImage = mImages[2].get();
	else if((mCheckMouseButtonDown && (mMouseDownState!=0)) || (!mCheckMouseButtonDown && IsPressed()))
		anImage = mImages[1].get();
	else if(IsMouseOver())
		anImage = mImages[3].get();
	
	if(anImage==NULL)
		anImage = mImages[0].get();

	if(mScaleImage && (anImage->mWidth!=Width() || anImage->mHeight!=Height()))
		anImage->SetSize(Width(),Height());

	g.DrawImage(anImage,0,0);
}
}
