// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"

using namespace WONAPI;

// FUNCTION: WON 0x00706b10
ImageComponent::ImageComponent(Image* image,bool tile)
{
    VirtualAC(image,tile);
    mStretch=false;
}

// Genuine normal destructor306ba0 remains original external: no own-vptr store.

// FUNCTION: WON 0x00706c10
void ImageComponent::VirtualB0(Image* image)
{
    mImage=image;
    if(image!=NULL)
        Fn2F4B90(image->mWidth,image->mHeight);
}

// FUNCTION: WON 0x00706c80
void ImageComponent::VirtualAC(Image* image,bool tile)
{
    mImage=image;
    mTile=tile;
    if(image!=NULL)
        Fn2F4B90(image->mWidth,image->mHeight);
}

// FUNCTION: WON 0x00706cf0
void ImageComponent::Virtual0C(int& width,int& height)
{
    if(mImage.get()!=NULL)
    {
        width=mImage->mWidth;
        height=mImage->mHeight;
    }
    else
        width=height=10;
}

// FUNCTION: WON 0x00706d30
void ImageComponent::Paint(Graphics& g)
{
    Component::Paint(g);
    if(mImage.get()!=NULL)
    {
        if(mStretch)
        {
            if(mImage->mWidth!=Width() || mImage->mHeight!=Height())
                mImage->SetSize(Width(),Height());
            g.DrawImage(mImage.get(),0,0);
        }
        else if(!mTile)
            g.DrawImage(mImage.get(),0,0);
        else
        {
            g.PushClipRect(0,0,Width(),Height());
            for(int x=0;x<Width();x+=mImage->mWidth)
                for(int y=0;y<Height();y+=mImage->mHeight)
                    g.DrawImage(mImage.get(),x,y);
            g.PopClipRect();
        }
    }
}


#include "ee/wonapi/WONLobbySimpleConfig.h"
namespace WONAPI {
RectangleComponent::RectangleComponent()
{
	mSolidColor = -1;
	mOutlineColor = -1;
}

// FUNCTION: WON 0x00706870
void RectangleComponent::SetSolidColor(int theSolidColor)
{
	mSolidColor = theSolidColor;
}

// FUNCTION: WON 0x00706880
void RectangleComponent::SetOutlineColor(int theOutlineColor)
{
	mOutlineColor = theOutlineColor;
}

// FUNCTION: WON 0x00706890
void RectangleComponent::Paint(Graphics &g)
{
	Component::Paint(g);
	if(mSolidColor!=-1)
	{
		g.SetColor(mSolidColor);
		g.FillRect(0,0,Width(),Height());
	}

	if(mOutlineColor!=-1)
	{
		g.SetColor(mOutlineColor);
		g.DrawRect(0,0,Width(),Height());
	}
}
}


namespace WONAPI {
RectangleComponent3D::RectangleComponent3D(bool up, int theThickness)
{
	mUp = up;
	mThickness = theThickness;

	mLightColor = ColorScheme::GetColorRef(StandardColor_3DHilight);
	mDarkColor = ColorScheme::GetColorRef(StandardColor_3DShadow);
	mSolidColor = -1;
}

// FUNCTION: WON 0x00706930
void RectangleComponent3D::Paint(Graphics &g)
{
	int width = Width()-1;
	int height = Height()-1;
	bool up = mUp;
	if(mSolidColor!=COLOR_INVALID)
	{
		g.SetColor(mSolidColor);
		g.FillRect(0,0,Width(),Height());
	}

	for(int i=0; i<mThickness; i++)
	{
		if(up)
			g.SetColor(mLightColor);
		else
			g.SetColor(mDarkColor);

		g.DrawLine(i,i,width-i,i);
		g.DrawLine(i,i,i,height-i);
		if(up)
			g.SetColor(mDarkColor);
		else
			g.SetColor(mLightColor);

		g.DrawLine(width-i,i,width-i,height);
		g.DrawLine(i,height-i,width,height-i);
	}
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00706ba0
// (public: virtual __thiscall WONAPI::ImageComponent::~ImageComponent(void)    [??1ImageComponent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00706b80
// (public: virtual void * __thiscall WONAPI::ImageComponent::`scalar deleting destructor'(unsigned int)    [??_GImageComponent@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00706840
// (public: __thiscall WONAPI::RectangleComponent::RectangleComponent(void)    [??0RectangleComponent@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007068f0
// (public: __thiscall WONAPI::RectangleComponent3D::RectangleComponent3D(bool,int)    [??0RectangleComponent3D@WONAPI@@QAE@_NH@Z])
