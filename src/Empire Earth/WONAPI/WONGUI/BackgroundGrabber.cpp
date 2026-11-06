// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"

using namespace WONAPI;

unsigned __int64 BackgroundGrabber::mImageOrderCounter = 0;

// FUNCTION: WON 0x00736000
BackgroundGrabber::BackgroundGrabber()
{
    mImageX = mImageY = 0;
    mNeedPutBG = false;
    mHaveBG = false;
    mTranslucentColor = 0;
    mTranslucentLevel = 4;
}

// FUNCTION: WON 0x00736060
void BackgroundGrabber::Clear()
{
    mHaveBG = mNeedPutBG = false;
}

// FUNCTION: WON 0x00736070
void BackgroundGrabber::PrePaint(Component* theComponent, Graphics& g)
{
    if(mHaveBG && mNeedPutBG)
    {
        int x = 0, y = 0;
        theComponent->Fn2F56D0(x,y);
        g.DrawImage(mBGImage.get(),mImageX-x,mImageY-y);
        mHaveBG = false;
        mNeedPutBG = false;
    }
}

// FUNCTION: WON 0x007360d0
void BackgroundGrabber::SetTranslucentColor(int theColor,int theLevel)
{
    mTranslucentLevel = theLevel;
    mTranslucentColor = theColor;
}

#define COLOR_1_8(c) (((c)>>3)&0xff1f1f1f)
#define COLOR_2_8(c) (((c)>>2)&0xff3f3f3f)
#define COLOR_3_8(c) (COLOR_1_8(c)+COLOR_2_8(c))
#define COLOR_4_8(c) (((c)>>1)&0xff7f7f7f)
#define COLOR_5_8(c) (COLOR_4_8(c)+COLOR_1_8(c))
#define COLOR_6_8(c) (COLOR_4_8(c)+COLOR_2_8(c))
#define COLOR_7_8(c) (COLOR_4_8(c)+COLOR_3_8(c))
//#define COLOR_7_8(c) ((c)-COLOR_1_8(c)) // doesn't work right because it rounds up

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007365b0
static int GetColor8th(int theColor, int theNumerator)
{
	switch(theNumerator)
	{
		case 1: return COLOR_1_8(theColor);
		case 2: return COLOR_2_8(theColor);
		case 3: return COLOR_3_8(theColor);
		case 4: return COLOR_4_8(theColor);
		case 5: return COLOR_5_8(theColor);
		case 6: return COLOR_6_8(theColor);
		case 7: return COLOR_7_8(theColor);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007360f0
void BackgroundGrabber::Paint(Component *theComponent, Graphics &g)
{
	if(!mHaveBG)
	{
		mClipRect = g.mClipRect;
		mImageOrder = mImageOrderCounter++;
		int aWidth = theComponent->Width();
		int aHeight = theComponent->Height();
		if(mBGImage.get()==NULL || aWidth!=mBGImage->mWidth || aHeight!=mBGImage->mHeight)
		{
			Window *aWindow = theComponent->GetRoot();
			mBGImage = aWindow->CreateImage(aWidth, aHeight);
			
			if(theComponent->ComponentFlagSet(0x20000) && mDebugWindow.get()==NULL)
			{
				mDebugWindow = new MSWindow;
				mDebugWindow->SetParent(theComponent->GetRoot());
				mDebugWindow->mStyle=0x40;
				WONRectangle aRect(0,0,mBGImage->mWidth,mBGImage->mHeight);
				mDebugWindow->Create(aRect);
			}
			
			if(mDebugWindow.get()!=NULL)
			{
				mDebugWindow->mRoot.get()->RemoveAllChildren();
				mDebugWindow->AddComponent(new ImageComponent(mBGImage.get()));
				int x=0,y=0;
				theComponent->Fn2F56D0(x,y);
				WONRectangle aScreenRect;
				theComponent->GetRoot()->GetRect(aScreenRect,false);
				mDebugWindow->SetPosSize(x+aScreenRect.x,y+aScreenRect.y,-1,-1);
			}

		}

		NativeImage *anImage = theComponent->GetRoot()->mOffscreenImage.get();
		int x = 0, y = 0;
		theComponent->RootToChild(x,y);
		mBGImage->GetGraphics().DrawImage(anImage, x, y);
		mImageX = 0;//theComponent->Left();
		mImageY = 0;//theComponent->Top();
		theComponent->Fn2F56D0(mImageX,mImageY);

		if(mDebugWindow.get()!=NULL)
			mDebugWindow->mRoot.get()->Invalidate();

		mHaveBG = true;

		if(theComponent->ComponentFlagSet(0x200))
		{

			int aColor = GetColor8th(mTranslucentColor,8-mTranslucentLevel);
			if(g.BlendSupported())
			{
				g.SetColor(mTranslucentColor);
				g.FillRectBlend(0,0,theComponent->Width(),theComponent->Height(),(8-mTranslucentLevel)*255/8);
				return;
			}
	
			SmartPtr<RawImage32> aRawImage = mBGImage->GetRaw();
			int aNumPixels = aRawImage->mWidth*aRawImage->mHeight;
			DWORD *aPixels =aRawImage->mImageData;
			DWORD *anEndPixels = aPixels+aNumPixels;
			switch(mTranslucentLevel)
			{
			case 1:
				do
				{
					*aPixels = COLOR_1_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 2:
				do
				{
					*aPixels = COLOR_2_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 3:
				do
				{
					*aPixels = COLOR_3_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 4:
				do
				{
					*aPixels = COLOR_4_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 5:
				do
				{
					*aPixels = COLOR_5_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			case 6:
				do
				{
					*aPixels = COLOR_6_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;
			
			case 7:
				do
				{
					*aPixels = COLOR_7_8(*aPixels) + aColor;
				} while(++aPixels!=anEndPixels);
				break;

			}
			g.DrawImage(aRawImage.get(),0,0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////


namespace WONAPI {
// FUNCTION: WON 0x00736680
void BackgroundGrabber::Invalidate(Component *theComponent)
{
	if(mHaveBG)
		mNeedPutBG = true;
}
}

// Genuine SDK BackgroundGrabber.cpp SizeChanged; native2fa500 RET4, support only.
void BackgroundGrabber::SizeChanged(Component* theComponent)
{
    // SDK intentionally leaves the captured background valid here.
}
