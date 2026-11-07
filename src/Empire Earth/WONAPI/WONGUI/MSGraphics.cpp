// WONGUI MSGraphics real GDI ownership/drawing; library loader keepsactualcleanup.
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
#include <math.h>



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf0f0
MSGraphics::MSGraphics()
{
	mDC = NULL;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf130
MSGraphics::~MSGraphics()
{
	Detach();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf180
void MSGraphics::Attach(DisplayContext *theContext)
{
	Graphics::Attach(theContext);
	mDC = theContext->mDC;

	LOGBRUSH aHollowBrush = { BS_NULL,0,0};
	mHollowBrush = CreateBrushIndirect(&aHollowBrush);

	mColor = 0;
	mSolidBrush = CreateSolidBrush(RGB(0,0,0));
	mPen = CreatePen(PS_SOLID,0,0);

	mOriginalBrush = (HBRUSH)SelectObject(mDC,mSolidBrush);
	mOriginalPen = (HPEN)SelectObject(mDC,mPen);
	SetBkMode(mDC, TRANSPARENT); 	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf210
void MSGraphics::Detach()
{
	Graphics::Detach();
	if(mDC!=NULL)
	{
		SelectObject(mDC, mOriginalBrush);
		SelectObject(mDC, mOriginalPen);

		DeleteObject(mSolidBrush);
		DeleteObject(mHollowBrush);
		DeleteObject(mPen);
		mDC = NULL;
		mDisplayContext = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf290
void MSGraphics::SetColorHook()
{
	DWORD aColor = ((mColor&0xff)<<16) | (mColor&0x00FF00) | (mColor>>16);
	HBRUSH aBrush = CreateSolidBrush(aColor);
	HPEN aPen = CreatePen(PS_SOLID,0,aColor);
	SelectObject(mDC,aPen);
	SelectObject(mDC,aBrush);
	DeleteObject(mSolidBrush);
	DeleteObject(mPen);
	mPen = aPen;
	mSolidBrush = aBrush;

	SetTextColor(mDC,aColor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf310
DWORD MSGraphics::GetPixelHook(int x, int y)
{
	return ::GetPixel(mDC, x, y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf330
void MSGraphics::SetPixelHook(int x, int y, DWORD theColor)
{
	DWORD aColor = ((theColor&0xff)<<16) | (theColor&0x00FF00) | (theColor>>16);
	::SetPixel(mDC,x,y,aColor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf370
void MSGraphics::DrawLineHook(int x1, int y1, int x2, int y2)
{
	POINT points[3] = {x1, y1, x2, y2, x2+1,y2};
	Polyline(mDC,points,3);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf3c0
void MSGraphics::DrawRectHook(int x, int y, int width, int height)
{
	SelectObject(mDC,mHollowBrush);
	Rectangle(mDC,x,y,x+width,y+height);
	SelectObject(mDC,mSolidBrush);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf410
void MSGraphics::DrawDottedRectHook(int x, int y, int width, int height)
{
    Graphics::DrawDottedRectHook(x,y,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf430
void MSGraphics::FillRectHook(int x, int y, int width, int height)
{
	Rectangle(mDC,x,y,x+width,y+height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace
{
typedef BOOL(WINAPI *AlphaBlendFunc)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
AlphaBlendFunc gAlphaBlendFunc;
struct AutoUnloadLibrary
{
	HMODULE mModule;

	AutoUnloadLibrary() { mModule = NULL; }
	~AutoUnloadLibrary() { if(mModule!=NULL) FreeLibrary(mModule); }

};

} // namespace

// FUNCTION: WON 0x007cf460
bool MSGraphics::BlendSupported()
{
	static HMODULE aModule = NULL;
	static bool haveLoaded = false;
	static AutoUnloadLibrary anUnload;

	if(!haveLoaded)
	{
		aModule = LoadLibrary("msimg32.dll");
		if(aModule!=NULL)
		{
			anUnload.mModule = aModule;
			gAlphaBlendFunc = (AlphaBlendFunc)GetProcAddress(aModule,"AlphaBlend");
		}

		haveLoaded = true;
	}
	return gAlphaBlendFunc!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf500
void MSGraphics::FillRectBlendHook(int tx, int ty, int width, int height, int theBlendAmount)
{
	if(!BlendSupported())
		return;

	static SmartPtr<NativeImage> anAlphaImage;
	static DWORD *anImageData;
	static int aWidth = 100;
	static int aHeight = 100;
	static int aNumPixels = aWidth*aHeight;
	static int aLastColor = mColor+1;
	if(anAlphaImage.get()==NULL)
	{	
		BITMAPINFO anInfo;
		BITMAPINFOHEADER &aHeader = anInfo.bmiHeader;
		aHeader.biSize = sizeof(BITMAPINFOHEADER);
		aHeader.biWidth = aWidth;
		aHeader.biHeight = -aHeight;
		aHeader.biPlanes = 1;
		aHeader.biBitCount = 32;
		aHeader.biCompression = BI_RGB;

		HBITMAP anAlpha = CreateDIBSection(mDC,&anInfo,DIB_RGB_COLORS,(void**)&anImageData,NULL,0);
		anAlphaImage = new MSNativeImage(new DisplayContext(mDC),anAlpha);
	}

	BLENDFUNCTION aFunc;
	aFunc.AlphaFormat = 0; //AC_SRC_ALPHA;
	aFunc.BlendFlags = 0;
//	aFunc.SourceConstantAlpha = 255;
	aFunc.SourceConstantAlpha = theBlendAmount;
	aFunc.BlendOp = AC_SRC_OVER;
	
	MSGraphics &g = (MSGraphics&)anAlphaImage->GetGraphics();
	g.SetColor(mColor);
	g.FillRect(0,0,aWidth,aHeight);


	for(int x=0; x<width; x+=aWidth)
	{
		for(int y=0; y<height; y+=aHeight)
		{
			int useWidth = aWidth, useHeight = aHeight;
			if(x+aWidth > width)
				useWidth = width-x;
			if(y+aHeight > height)
				useHeight = height-y;
	
			gAlphaBlendFunc(mDC,x+tx,y+ty,useWidth,useHeight,anAlphaImage->mDisplayContext->mDC,0,0,useWidth,useHeight,aFunc);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf790
void MSGraphics::DrawPolygonHook(POINT thePoints[], int nPoints)
{	
	SelectObject(mDC,mHollowBrush);
	Polygon(mDC,thePoints,nPoints);
	SelectObject(mDC,mSolidBrush);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf7d0
void MSGraphics::FillPolygonHook(POINT thePoints[], int nPoints)
{
	Polygon(mDC,thePoints,nPoints);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf7f0
void MSGraphics::DrawEllipseHook(int x, int y, int width, int height)
{
	SelectObject(mDC,mHollowBrush);
	Ellipse(mDC,x,y,x+width,y+height);
	SelectObject(mDC,mSolidBrush);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf840
void MSGraphics::FillEllipseHook(int x, int y, int width, int height)
{
	Ellipse(mDC,x,y,x+width,y+height);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf870
void MSGraphics::DrawArcHook(int x, int y, int width, int height, int startAngle, int arcAngle)
{
	static const double PI = 3.14159;
	static const int MULT = 100;

	SelectObject(mDC,mHollowBrush);
	int cx = x+width/2;
	int cy = y+height/2;

	Arc(mDC,x,y,x+width,y+height,cx+MULT*cos(PI*startAngle/180),cy-MULT*sin(PI*startAngle/180),cx+MULT*cos(PI*(startAngle+arcAngle)/180),cy-MULT*sin(PI*(startAngle+arcAngle)/180));
	SelectObject(mDC,mSolidBrush);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf970
void MSGraphics::ClearClipRectHook()
{
	SelectClipRgn(mDC, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007cf980
void MSGraphics::SetClipRectHook(int x, int y, int width, int height)
{
	IntersectClipRect(mDC, x, y, x+width, y+height);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007cf110
// (public: virtual void * __thiscall WONAPI::MSGraphics::`scalar deleting destructor'(unsigned int)    [??_GMSGraphics@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007cf4e0
// TEMPLATE: WON 0x007cf780
// (_$E74)
