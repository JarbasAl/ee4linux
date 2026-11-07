// Source-only reconstructed WONGUI StretchImage; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstring>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyStretchImage.h"
#include "LostDebug.h"


using namespace WONAPI;


static void ImageFilter_NewRawImage(SmartPtr<RawImage8>& image,int w,int h) { image=new RawImage8(w,h,NULL); }
static void ImageFilter_NewRawImage(SmartPtr<RawImage32>& image,int w,int h) { image=new RawImage32(w,h); }
inline void CopyRawImageInfo(RawImage8* dst,RawImage8* src) { dst->mPalette=src->mPalette.get(); dst->mTransparentColor=src->mTransparentColor; }
inline void CopyRawImageInfo(RawImage32* dst,RawImage32* src) { dst->mDoTransparency=src->mDoTransparency; }


template <class ImageType, class PixelType> 
// FUNCTION: WON 0x00755280
// FUNCTION: WON 0x00755560
static void CopyRawImage(ImageType *src, ImageType *dst, int destx, int desty, int x, int y, int width, int height)
{
	for(int i = 0; i<height; i++)
	{
		PixelType *aFromPixel = src->StartRowTraversal(y+i) + x;
		PixelType *aToPixel = dst->StartRowTraversal(desty+i) + destx;
		memcpy(aToPixel,aFromPixel,sizeof(PixelType)*width);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
// FUNCTION: WON 0x00755170
// FUNCTION: WON 0x00755410
static void RawTileHorz(ImageType *src, ImageType *dst, int x1, int x2, int y, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int endX = x2 - tileWidth;
	int x;
	for(x = x1; x<endX; x+=tileWidth)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,tileWidth,tileHeight);

	int finalWidth = x2 - x;
	if(finalWidth>0)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,finalWidth,tileHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static void RawTileVert(ImageType *src, ImageType *dst, int y1, int y2, int x, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int endY = y2 - tileHeight;
	int y;
	for(y = y1; y<endY; y+=tileHeight)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,tileWidth,tileHeight);

	int finalHeight = y2 - y;
	if(finalHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,tileWidth,finalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static void RawTileHorzVert(ImageType *src, ImageType *dst, int x1, int x2, int y1, int y2, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int endY = y2 - tileHeight;
	int y;
	for(y = y1; y<endY; y+=tileHeight)
		RawTileHorz<ImageType,PixelType>(src,dst,x1,x2,y,tileLeft,tileTop,tileWidth,tileHeight);

	int finalHeight = y2 - y;
	if(finalHeight>0)
		RawTileHorz<ImageType,PixelType>(src,dst,x1,x2,y,tileLeft,tileTop,tileWidth,finalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
// FUNCTION: WON 0x00755060
// FUNCTION: WON 0x007552f0
static void RawStretch(ImageType *src, ImageType *dst, int x1, int x2, int y1, int y2, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int theNewWidth = x2 - x1;
	int theNewHeight = y2 - y1;
	if(theNewWidth<=0 || theNewHeight<=0)
		return;


	int cy = 0, oy = tileTop;
	for(int y=0; y<theNewHeight; y++)
	{
		src->StartRowTraversal(oy);
		src->mRowPtr += tileLeft;
		dst->StartRowTraversal(y+y1);
		dst->mRowPtr += x1;

		PixelType curVal = *src->mRowPtr;

		int cx = 0;
		for(int x=0; x<theNewWidth; x++)
		{
			*dst->mRowPtr = curVal;
			++dst->mRowPtr;
			cx += tileWidth;
			if(cx >= theNewWidth)
			{
				src->mRowPtr += cx/theNewWidth;
				cx%=theNewWidth;
				curVal = *src->mRowPtr;
			}
		}

		cy += tileHeight;
		if(cy >= theNewHeight)
		{
			oy += cy/theNewHeight;
			cy%=theNewHeight;
		}
	}
}

// FUNCTION: WON 0x00749720
// (??_GStretchImage@WONAPI@@MAEPAXI@Z)

// FUNCTION: WON 0x00749e60
// (??_GStretchCenterImage@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00749e80
// (??1StretchCenterImage@WONAPI@@UAE@XZ)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007529b0
// FUNCTION: WON 0x007535d0
template <class ImageType, class PixelType> 
static SmartPtr<ImageType> DoFilter(ImageType *theImage, StretchImageFilter *theFilter)
{
    // LOST-DEBUG: none: exact only (RawImage32 instance)
    EE_LOST_DEBUG(3);
	int mWidth = theFilter->mWidth;
	int mHeight = theFilter->mHeight;
	int mLeftWidth = theFilter->mLeftWidth;
	int mRightWidth = theFilter->mRightWidth;
	int mTopHeight = theFilter->mTopHeight;
	int mBottomHeight = theFilter->mBottomHeight;
	bool mTileHorz = theFilter->mTileHorz, mTileVert = theFilter->mTileVert; 

	if(mBottomHeight + mTopHeight > mHeight)
		mHeight = mBottomHeight + mTopHeight;

	if(mRightWidth + mLeftWidth > mWidth)
		mWidth = mRightWidth + mLeftWidth;

	ImageType *src = theImage;
	SmartPtr<ImageType> newImage = new ImageType(mWidth,mHeight);

	ImageType *dst = newImage.get();
	CopyRawImageInfo(dst,src);


	int x = 0, y = 0, left = 0, top = 0;

	// Simplification: Assume left and top are 0, and width and height are mWidth and mHeight
	int imageLeft = 0;
	int imageTop = 0;
	int imageRight = src->GetWidth()-mRightWidth;
	int imageBottom = src->GetHeight()-mBottomHeight;
	
	left = x;
	top = y;
	int right = x + mWidth - mRightWidth;
	int bottom = y + mHeight - mBottomHeight;

	int centerWidth = mWidth - mRightWidth - mLeftWidth;
	int centerHeight = mHeight - mBottomHeight - mTopHeight;

	int imageCenterWidth = src->GetWidth() - mRightWidth - mLeftWidth;
	int imageCenterHeight = src->GetHeight() - mBottomHeight - mTopHeight;

	int minCenterWidth = centerWidth<imageCenterWidth?centerWidth:imageCenterWidth; 
	int minCenterHeight = centerHeight<imageCenterHeight?centerHeight:imageCenterHeight; 

	// 9 pieces

	// Center
	if(!theFilter->mSkipCenter && imageCenterHeight>0 && imageCenterWidth>0)
	{
		if(!mTileHorz && !mTileVert) // just stretch
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
		else if(mTileHorz && mTileVert) // just tile
			RawTileHorzVert<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
		else if(mTileVert) // stretch horz, tile vert
		{
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top+mTopHeight,top+mTopHeight+minCenterHeight,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
			RawTileVert<ImageType,PixelType>(dst,dst,top+mTopHeight+imageCenterHeight,bottom,left+mLeftWidth,left+mLeftWidth,top+mTopHeight,centerWidth,imageCenterHeight);
		}
		else if(mTileHorz) // stretch vert, tile horz
		{
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,left+mLeftWidth+minCenterWidth,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
			RawTileHorz<ImageType,PixelType>(dst,dst,left+mLeftWidth+imageCenterWidth,right,top+mTopHeight,left+mLeftWidth,top+mTopHeight,imageCenterWidth,centerHeight);
		}
	}


	// Sides
	if(mLeftWidth>0 && imageCenterHeight>0) // left side
	{
		if(mTileVert)
			RawTileVert<ImageType,PixelType>(src,dst,top+mTopHeight,bottom,left,imageLeft,imageTop+mTopHeight,mLeftWidth,imageCenterHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,left,left+mLeftWidth,top+mTopHeight,bottom,imageLeft,imageTop+mTopHeight,mLeftWidth,imageCenterHeight);
	}

	if(mRightWidth>0 && imageCenterHeight>0) // right side
	{
		if(mTileVert)
			RawTileVert<ImageType,PixelType>(src,dst,top+mTopHeight,bottom,right,imageRight,imageTop+mTopHeight,mRightWidth,imageCenterHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,right,right+mRightWidth,top+mTopHeight,bottom,imageRight,imageTop+mTopHeight,mRightWidth,imageCenterHeight);
	}


	if(mTopHeight>0 && imageCenterWidth>0) // top side
	{
		if(mTileHorz)
			RawTileHorz<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top,imageLeft+mLeftWidth,imageTop,imageCenterWidth,mTopHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top,top+mTopHeight,imageLeft+mLeftWidth,imageTop,imageCenterWidth,mTopHeight);
	}

	if(mBottomHeight>0 && imageCenterWidth>0) // bottom side
	{
		if(mTileHorz)
			RawTileHorz<ImageType,PixelType>(src,dst,left+mLeftWidth,right,bottom,imageLeft+mLeftWidth,imageBottom,imageCenterWidth,mBottomHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,bottom,bottom+mBottomHeight,imageLeft+mLeftWidth,imageBottom,imageCenterWidth,mBottomHeight);
	}

	// Corners
	if(mLeftWidth>0 && mTopHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,left,top,imageLeft,imageTop,mLeftWidth,mTopHeight); // top-left

	if(mRightWidth>0 && mTopHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,right,top,imageRight,imageTop,mRightWidth,mTopHeight); // top-right

	if(mLeftWidth>0 && mBottomHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,left,bottom,imageLeft,imageBottom,mLeftWidth,mBottomHeight); // bottom-left

	if(mRightWidth>0 && mBottomHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,right,bottom,imageRight,imageBottom,mRightWidth,mBottomHeight); // bottom-right


	return newImage;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749570
SmartPtr<RawImage> StretchImageFilter::Filter(RawImage *theImage)
{

	switch(theImage->mType)
	{
		case 1: return DoFilter<RawImage8,unsigned char>((RawImage8*)theImage,this).get(); 
		case 2: return DoFilter<RawImage32,DWORD>((RawImage32*)theImage,this).get(); 
		default: return NULL;
	}
};

///////////////////////////////////////////////////////////////////////////////

StretchImageFilter::StretchImageFilter()
{
	mWidth = mHeight = 0;
	mLeftWidth = mRightWidth = mTopHeight = mBottomHeight = 0;
	mTileHorz = mTileVert = true;
	mSkipCenter = false;
}

// FUNCTION: WON 0x00749650
StretchImage::StretchImage(RawImage *theImage, int leftWidth, int topHeight, int rightWidth, int bottomHeight)
{
	mImage = theImage;
	mWidth = mHeight = 0;
	mLeftWidth = leftWidth;
	mRightWidth = rightWidth;
	mTopHeight = topHeight;
	mBottomHeight = bottomHeight;
	mNeedRecalc = true;
	mTileHorz = mTileVert = true;
	mSkipCenter = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749740
StretchImage::~StretchImage()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007497e0
void StretchImage::SetSize(int theWidth, int theHeight)
{
	if(theWidth==mWidth && theHeight==mHeight)
		return;

	mWidth = theWidth;
	mHeight = theHeight;
	mNeedRecalc = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749810
void StretchImage::CalcImage(DisplayContext *context)
{
	SmartPtr<StretchImageFilter> aFilter = new StretchImageFilter;
	aFilter->SetBorders(mLeftWidth,mTopHeight,mRightWidth,mBottomHeight);
	aFilter->SetSize(GetWidth(),GetHeight());
	aFilter->SetTile(mTileHorz,mTileVert);
	aFilter->SetSkipCenter(mSkipCenter);
	mCachedImage = aFilter->Filter(mImage.get())->GetNative(context);
	mNeedRecalc = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007499a0
void StretchImage::Paint(Graphics &g)
{
	if(mNeedRecalc)
		CalcImage(g.mDisplayContext.get());

	if(mSkipCenter)
	{
		if(mLeftWidth>0)
		{
			g.PushClipRect(0,0,mLeftWidth,mHeight); // Left part
			g.DrawImage(mCachedImage.get(),0,0);
			g.PopClipRect();
		}

		if(mRightWidth>0)
		{
			g.PushClipRect(mWidth-mRightWidth,0,mRightWidth,mHeight); // right part
			g.DrawImage(mCachedImage.get(),0,0);
			g.PopClipRect();
		}

		if(mTopHeight>0)
		{	
			g.PushClipRect(0,0,mWidth,mTopHeight); // top part
			g.DrawImage(mCachedImage.get(),0,0);
			g.PopClipRect();
		}

		if(mBottomHeight>0)
		{
			g.PushClipRect(0,mHeight-mBottomHeight,mWidth,mBottomHeight);
			g.DrawImage(mCachedImage.get(),0,0);
			g.PopClipRect();
		}
	}
	else
		g.DrawImage(mCachedImage.get(),0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749ab0
void StretchImage::SetImage(RawImage *theImage)
{
	mImage = theImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749b10
void StretchImage::SetBorders(int left, int top, int right, int bottom)
{
	mLeftWidth = left;
	mTopHeight = top;
	mRightWidth = right;
	mBottomHeight = bottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749b30
void StretchImage::CopyAttributes(StretchImage *from)
{
	mLeftWidth = from->mLeftWidth;
	mTopHeight = from->mTopHeight; 
	mRightWidth = from->mRightWidth;
	mBottomHeight = from->mBottomHeight;
	mTileHorz = from->mTileHorz;
	mTileVert = from->mTileVert;
	mSkipCenter = from->mSkipCenter;
	mWidth = from->mWidth;
	mHeight = from->mHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749b80
SmartPtr<Image> StretchImage::Duplicate()
{
	SmartPtr<StretchImage> aCopy = new StretchImage(mImage.get(),mLeftWidth,mTopHeight,mRightWidth,mBottomHeight);
	aCopy->SetTile(mTileHorz,mTileVert);
	aCopy->SetSkipCenter(mSkipCenter);
	return aCopy.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749c60
SmartPtr<NativeImage> StretchImage::GetNative(DisplayContext *theContext)
{
	if(mNeedRecalc)
		CalcImage(theContext);

	return mCachedImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749cb0
void StretchCenterImage::Paint(Graphics &g)
{
	g.DrawImage(mStretchImage.get(),0,0);
	
	if(mCenterImage->mWidth<=mWidth && mCenterImage->mHeight<=mHeight)
		g.DrawImage(mCenterImage.get(),(mWidth-mCenterImage->mWidth)/2,(mHeight-mCenterImage->mHeight)/2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749d20
void StretchCenterImage::SetSize(int theWidth, int theHeight)
{
	mWidth = theWidth;
	mHeight = theHeight;
	mStretchImage->SetSize(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00749d40
SmartPtr<Image> StretchCenterImage::Duplicate()
{
	return new StretchCenterImage(mStretchImage->Duplicate().get(),mCenterImage.get());
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00749540
// (public: __thiscall WONAPI::StretchImageFilter::StretchImageFilter(void))
