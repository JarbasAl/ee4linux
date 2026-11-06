// Rich text image selection, link state and image-map hit testing.
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
#include "ee/wonapi/WONLobbyImageSegment.h"
namespace WONAPI {

SmartPtr<Image> LineSegment::mSelectImageMask;

// FUNCTION: WON 0x00709570
bool LineSegment::DrawSelection1(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return false;

	if(mStartSelIndex==theInfo.mStartSelSegment && theInfo.mStartSelSegmentPos>0)
		return false;

	if(mStartSelIndex==theInfo.mEndSelSegment && theInfo.mEndSelSegmentPos<1)
		return false;

	g.SetSysColor(7);
	g.FillRect(x,theInfo.mLineY,GetWidth(),theInfo.mLineHeight);
	return true;
}

// FUNCTION: WON 0x007095E0
void LineSegment::DrawSelection2(Graphics &g, int x, int y)
{
	const int MASK_WIDTH = 100;
	const int MASK_HEIGHT = 100;
	if(mSelectImageMask.get()==NULL)
	{
		SmartPtr<Palette> aPalette = new Palette(2);
		aPalette->mColors[0]=0x000000;
		aPalette->mColors[1]=0x000080;
		SmartPtr<RawImage8> anImage = new RawImage8(MASK_WIDTH,MASK_HEIGHT,aPalette.get());
		anImage->mTransparentColor=0;

		for(int i=0; i<MASK_WIDTH; i++)
		{
			int pixel = i%2;
			anImage->StartRowTraversal(i);
			for(int j=0; j<MASK_HEIGHT; j++)
			{
				*anImage->mRowPtr=(unsigned char)pixel;
				++anImage->mRowPtr;
				pixel = 1-pixel;
			}
		}
		mSelectImageMask = anImage->GetNative(g.mDisplayContext.get()).get();
	}

	int aWidth = GetWidth();
	int aHeight = GetHeight();
	g.PushClipRect(x,y,aWidth,aHeight);
	for(int i=0; i<aWidth; i+=MASK_WIDTH)
		for(int j=0; j<aHeight; j+=MASK_HEIGHT)
			g.DrawImage(mSelectImageMask.get(),i+x,j+y);

	g.PopClipRect();
}

// FUNCTION: WON 0x0070B390
void ImageLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	bool selected = DrawSelection1(g,x,y,theInfo);
	if(mBorder)
	{
		int aWidth = mImage->mWidth+2*mBorder;
		int aHeight = mImage->mHeight+2*mBorder;
		g.SetColor(mBorderColor);
		for(int i=0; i<mBorder; i++)
			g.DrawRect(x+mHSpace+i,y+mVSpace+i,aWidth-2*i,aHeight-2*i);
	}

	g.DrawImage(mImage.get(),x+mHSpace+mBorder,y+mVSpace+mBorder);
	if(selected)
		DrawSelection2(g,x,y);
}

// FUNCTION: WON 0x0070AB50
void TextLinkParam::LinkDown()
{
	mLinkIsDown = true;
	for(std::list<LineSegment*>::iterator anItr = mLinkList.begin(); anItr!=mLinkList.end(); ++anItr)
		(*anItr)->LinkDown();
}

// FUNCTION: WON 0x0070AB80
bool TextLinkParam::LinkUp()
{
	if(mLinkIsDown)
	{
		mLinkIsDown = false;
		for(std::list<LineSegment*>::iterator anItr = mLinkList.begin(); anItr!=mLinkList.end(); ++anItr)
			(*anItr)->LinkUp();

		return true;
	}
	else
		return false;
}

// FUNCTION: WON 0x0070AED0
TextLinkParam* ImageMap::GetLink(int x, int y)
{
	std::list<ShapeLink>::iterator anItr = mShapeList.begin();
	if(anItr!=mShapeList.end())
	{
		while(true)
		{
			ShapeLink &aLink = *anItr;
			if(x>=aLink.mShape.x && x<aLink.mShape.x+aLink.mShape.width && y>=aLink.mShape.y && y<aLink.mShape.y+aLink.mShape.height)
				return aLink.mLinkParam.get();
			++anItr;
			if(anItr==mShapeList.end())
				break;
		}
	}
	
	return NULL;
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00709860
// (public: virtual void * __thiscall WONAPI::Palette::`scalar deleting destructor'(unsigned int)    [??_GPalette@WONAPI@@UAEPAXI@Z])
// SYNTHETIC: WON 0x7094d0
// SYNTHETIC: WON 0x7094e0
// SYNTHETIC: WON 0x7094f0
// SYNTHETIC: WON 0x709500
