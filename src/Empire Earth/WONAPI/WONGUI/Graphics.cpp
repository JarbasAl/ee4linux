// WONGUI Graphics native behavior; archived SDK corroborates source contract.
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
using namespace WONAPI;
// FUNCTION: WON 0x00726ad0
Graphics::Graphics()
{
	mTransX = mTransY = 0;
	mColorScheme = NULL;
}

// FUNCTION: WON 0x00726b90
Graphics::~Graphics()
{
	Detach();
}

// FUNCTION: WON 0x00726c70
void Graphics::Attach(DisplayContext *theContext)
{
	Detach();
	mDisplayContext = theContext;
	mDisplayContext->mGraphics = this;
}

// FUNCTION: WON 0x00726ce0
void Graphics::SetColor(DWORD theColor)
{
	if(theColor>=0x80000000 && theColor<0x80000000+17 && mColorScheme)
	{
		// Do it this way instead of calling ApplyColorSchemeColor in order to avoid an infinite loop
		// If a colorscheme color is a reference to itself
		DWORD aColor = mColorScheme->GetStandardColor((int)(theColor-0x80000000));
		if(aColor!=mColor)
		{
			mColor = aColor;
			SetColorHook();
		}
	}
	else if(mColor!=theColor)
	{
		mColor = theColor;
		SetColorHook();
	}
}

// FUNCTION: WON 0x00726d20
DWORD Graphics::Fn326D20(int theColorRef)
{
	if(mColorScheme)
		return mColorScheme->GetStandardColor(theColorRef);
	else
		return 0;
}

// FUNCTION: WON 0x00726d40
void Graphics::SetSysColor(int theColorRef)
{
	if(mColorScheme)
		SetColor(mColorScheme->GetStandardColor(theColorRef));
}

// FUNCTION: WON 0x00726d60
void Graphics::Translate(int dx, int dy)
{
	mTransX += dx;
	mTransY += dy;
}

// FUNCTION: WON 0x00726d80
void Graphics::DoTranslate(int &x, int &y)
{
	x+=mTransX;
	y+=mTransY;
}

// FUNCTION: WON 0x00726da0
void Graphics::SetPixel(int x, int y, DWORD color)
{
    DoTranslate(x,y);
    SetPixelHook(x,y,color);
}

// FUNCTION: WON 0x00726dd0
void Graphics::DrawImage(Image *theImage, int x, int y)
{
	DoTranslate(x,y);
	theImage->DrawEntire(mDisplayContext.get(),x,y);
}

// FUNCTION: WON 0x00726e00
void Graphics::DrawImage(Image *theImage, int x, int y, int left, int top, int width, int height)
{
	DoTranslate(x,y);
	theImage->Draw(mDisplayContext.get(), x, y, left, top, width, height);
}

// FUNCTION: WON 0x00726e50
void Graphics::DrawLine(int x1, int y1, int x2, int y2)
{

	DoTranslate(x1,y1);
	DoTranslate(x2,y2);

	DrawLineHook(x1,y1,x2,y2);	
}

// FUNCTION: WON 0x00726ea0
void Graphics::DrawRect(int x, int y, int width, int height)
{
	DoTranslate(x,y);
	DrawRectHook(x,y,width,height);
}

// FUNCTION: WON 0x00726ee0
void Graphics::FillRect(int x, int y, int width, int height)
{
	DoTranslate(x,y);
	FillRectHook(x,y,width,height);
}

// FUNCTION: WON 0x00726f20
void Graphics::FillRectBlend(int x, int y, int width, int height, int theBlendAmount)
{
	DoTranslate(x,y);
	FillRectBlendHook(x,y,width,height,theBlendAmount);
}

// FUNCTION: WON 0x00726f60
void Graphics::DrawDottedRect(int x, int y, int width, int height)
{
	DoTranslate(x,y);
	DrawDottedRectHook(x,y,width,height);
}

// FUNCTION: WON 0x00726fa0
void Graphics::DrawEllipse(int x, int y, int width, int height)
{
	DoTranslate(x,y);
	DrawEllipseHook(x,y,width,height);
}

// FUNCTION: WON 0x00726fe0
void Graphics::FillEllipse(int x, int y, int width, int height)
{
	DoTranslate(x,y);
	FillEllipseHook(x,y,width,height);
}

// FUNCTION: WON 0x00727020
void Graphics::DrawArc(int x, int y, int width, int height, int startAngle, int arcAngle)
{
	DoTranslate(x,y);
	DrawArcHook(x,y,width,height,startAngle,arcAngle);
}

// FUNCTION: WON 0x00727060
void Graphics::SetFont(Font *theFont)
{
	if(theFont!=mFont.get())
	{
		mFont = theFont;
		mFont->Virtual04(mDisplayContext.get());
	}
}

// FUNCTION: WON 0x007270c0
void Graphics::DrawString(const GUIString &theStr, int x, int y)
{
	DoTranslate(x,y);
	mFont->Virtual08(mDisplayContext.get(),theStr,0,theStr.length(),x, y);
}

// FUNCTION: WON 0x00727100
void Graphics::DrawString(const GUIString &theStr, unsigned short theOffset, unsigned short theLength, int x, int y)
{
	DoTranslate(x,y);
	mFont->Virtual08(mDisplayContext.get(),theStr,theOffset,theLength,x, y);
}

// FUNCTION: WON 0x00727140
void Graphics::PerformClip()
{
	ClearClipRectHook();
	mClipRect.x = mClipRect.y = -10000000;
	mClipRect.width = mClipRect.height = 20000001;
	if(mClipStack.empty())
		return;

	WONRectangle aRect = mClipStack.front();
	ClipStack::iterator anItr = mClipStack.begin();
	++anItr;

	while(anItr!=mClipStack.end())
	{
		aRect.DoIntersect(*anItr);
		++anItr;
	}

	SetClipRectHook(aRect.x, aRect.y, aRect.width, aRect.height);
	mClipRect = aRect;
}

// FUNCTION: WON 0x00727200
void Graphics::PushClipRect(int x, int y, int width, int height)
{
	DoTranslate(x,y);
	WONRectangle aRect(x , y, width, height);
	mClipStack.push_back(aRect);
	PerformClip();
}

// FUNCTION: WON 0x007272b0
void Graphics::PopClipRect()
{
	if(!mClipStack.empty())
		mClipStack.pop_back();

	PerformClip();

}

// FUNCTION: WON 0x00727300
void Graphics::DrawRectHook(int x, int y, int width, int height)
{
	DrawLineHook(x,y,x+width,y);
	DrawLineHook(x+width,y,x+width,y+height);
	DrawLineHook(x+width,y+height,x,y+height);
	DrawLineHook(x,y+height,x,y);
}

// FUNCTION: WON 0x00727360
void Graphics::DrawDottedRectHook(int x, int y, int width, int height)
{
//	DrawRectHook(x,y,width,height);
	int i;
	for(i=0; i<width; i+=2)
	{
		SetPixelHook(x+i,y,mColor);
		SetPixelHook(x+i,y+height-1,mColor);
	}

	for(i=0; i<height; i+=2)
	{
		SetPixelHook(x,y+i,mColor);
		SetPixelHook(x+width-1,y+i,mColor);
	}
}

// FUNCTION: WON 0x007273f0
void Graphics::FillRectHook(int x, int y, int width, int height)
{
	for(int i=0; i<height; i++)
		DrawLineHook(x,y+i,x+width,y+i);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00726b70
// (public: virtual void * __thiscall WONAPI::Graphics::`scalar deleting destructor'(unsigned int)    [??_GGraphics@WONAPI@@UAEPAXI@Z])
