// Source-only reconstructed WONGUI Background; no runtime substitution authority.
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
#include "ee/wonapi/WONLobbyBackground.h"



using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735cb0
void Background::Init()
{
	mColor = -1;
	mBorderColor = -1;
	mUseOffsets = true;
	mStretchImage = false;
	mWantGrabBG = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background::Background()
{
	Init();
	mColor = (int)0x8000000d;
}

// FUNCTION: WON 0x00735cd0
// (??0Background@WONAPI@@QAE@XZ)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735cf0
Background::Background(Image *theImage)
{
	Init();
	mImage = theImage;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Background::Background(int theColor, int theBorderColor)
{
	Init();
	mColor = theColor;
	mBorderColor = theBorderColor;
}

// FUNCTION: WON 0x00735d80
// (??0Background@WONAPI@@QAE@HH@Z)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735db0
Background::Background(Component *theComponent)
{
	Init();
	mComponent = theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735e50
void Background::SetColor(int theColor)
{
	mColor = theColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735e60
void Background::Paint(Graphics &g, int xpos, int ypos, int width, int height, int xoff, int yoff)
{
	if(mStretchImage && mImage.get()!=NULL)
	{
		if(mImage->mWidth!=width || mImage->mHeight!=height)
			mImage->SetSize(width,height);

		g.DrawImage(mImage.get(),xpos,ypos);
		return;
	}

	g.Translate(xpos,ypos);

	if(mColor!=-1)
	{
		g.SetColor(mColor);
		g.FillRect(0,0,width,height);
	}

	if(mBorderColor!=-1)
	{
		g.SetColor(mBorderColor);
		g.DrawRect(0,0,width,height);
	}
	
	if(mImage.get()!=NULL)
	{
		g.PushClipRect(0,0,width,height);
		int startX = mUseOffsets?-(xoff%mImage->mWidth):0;
		int startY = mUseOffsets?-(yoff%mImage->mHeight):0;
		for(int y = startY; y<height; y+=mImage->mHeight)
			for(int x = startX; x<width; x+=mImage->mWidth)
				g.DrawImage(mImage.get(), x, y);

		g.PopClipRect();
	}

	if(mComponent.get()!=NULL)
	{
		mComponent->Fn2F4B90(width,height);
		mComponent->Invalidate();
		mComponent->Paint(g);
	}
	
	g.Translate(-xpos,-ypos);
}
