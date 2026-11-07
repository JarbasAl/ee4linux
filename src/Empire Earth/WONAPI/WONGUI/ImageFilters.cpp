// WONGUI primitive image filters and authored pixel algorithms.
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
#include "ee/wonapi/WONLobbyConfigImages.h"
#include "LostDebug.h"
namespace WONAPI {
static inline void NewRaw(SmartPtr<RawImage8>& image,int width,int height) { image=new RawImage8(width,height,NULL); }
static inline void NewRaw(SmartPtr<RawImage32>& image,int width,int height) { image=new RawImage32(width,height); }
static inline void CopyInfo(RawImage8* dst,RawImage8* src) { dst->mPalette=src->mPalette;dst->mTransparentColor=src->mTransparentColor; }
static inline void CopyInfo(RawImage32* dst,RawImage32* src) { dst->mDoTransparency=src->mDoTransparency; }

// FUNCTION: WON 0x00752220
// FUNCTION: WON 0x00752380
template<class ImageType,class PixelType>
static SmartPtr<ImageType> CropPixels(ImageType* src,int x,int y,int width,int height)
{
    SmartPtr<ImageType> dst=new ImageType(width,height);
    CopyInfo(dst.get(),src);
    for(int row=0;row<height;++row)
    {
        PixelType* from=src->StartRowTraversal(row+y)+x;
        PixelType* to=dst->StartRowTraversal(row);
        memcpy(to,from,sizeof(PixelType)*width);
    }
    return dst;
}

// FUNCTION: WON 0x00748a60
SmartPtr<RawImage> CropImageFilter::Filter(RawImage* image)
{
    switch(image->mType)
    {
    case 1:return CropPixels<RawImage8,unsigned char>((RawImage8*)image,mx,my,mWidth,mHeight).get();
    case 2:return CropPixels<RawImage32,DWORD>((RawImage32*)image,mx,my,mWidth,mHeight).get();
    }
    return NULL;
}

// FUNCTION: WON 0x00748ba0
bool ScaleImageFilter::CheckSize(Image* image)
{
    if(mIsPercent)
    {
        mWidth=image->mWidth*mPercentWidth/100;
        mHeight=image->mHeight*mPercentHeight/100;
    }
    if(mWidth<=0) mWidth=1;
    if(mHeight<=0) mHeight=1;
    if(mWidth==image->mWidth && mHeight==image->mHeight)
        return true;

    return false;
}

// FUNCTION: WON 0x00752690
// FUNCTION: WON 0x00752820
template<class ImageType,class PixelType>
static SmartPtr<ImageType> ScalePixels(ImageType* src,int width,int height)
{
    SmartPtr<ImageType> dst=new ImageType(width,height);
    CopyInfo(dst.get(),src);
    int cy=0,oy=0;
    for(int y=0;y<height;++y)
    {
        src->StartRowTraversal(oy);
        dst->StartRowTraversal(y);
        PixelType value=src->GetRowPixel();
        int cx=0;
        for(int x=0;x<width;++x)
        {
            dst->SetRowPixel(value);
            dst->NextRowPixel();
            cx+=src->GetWidth();
            if(cx>=width)
            {
                src->NextRowPixel(cx/width);
                cx%=width;
                value=src->GetRowPixel();
            }
        }
        cy+=src->GetHeight();
        if(cy>=height) { oy+=cy/height;cy%=height; }
    }
    return dst;
}

static inline bool CurrentBit(RawImage1* image) { return (*image->mRowPtr&image->mRowMask)!=0; }
static inline void NextBit(RawImage1* image)
{
    image->mRowMask>>=1;
    if(image->mRowMask==0) { image->mRowMask=0x80;++image->mRowPtr; }
}
static inline void PutBit(RawImage1* image,bool bit)
{
    if(bit) *image->mRowPtr|=image->mRowMask;
    else *image->mRowPtr&=~image->mRowMask;
    NextBit(image);
}

// FUNCTION: WON 0x007524b0
static SmartPtr<RawImage1> ScaleBits(RawImage1* src,int width,int height)
{
    SmartPtr<RawImage1> dst=new RawImage1(width,height);
    int cy=0,oy=0;
    for(int y=0;y<height;++y)
    {
        src->StartRowTraversal(oy);
        dst->StartRowTraversal(y);
        bool value=src->GetRowPixel();
        int cx=0;
        for(int x=0;x<width;++x)
        {
            dst->SetRowPixel(value);
            dst->NextRowPixel();
            cx+=src->GetWidth();
            if(cx>=width)
            {
                src->NextRowPixel(cx/width);
                cx%=width;
                value=src->GetRowPixel();
            }
        }
        cy+=src->GetHeight();
        if(cy>=height) { oy+=cy/height;cy%=height; }
    }
    return dst;
}

// FUNCTION: WON 0x00748c30
SmartPtr<RawImage> ScaleImageFilter::Filter(RawImage* image)
{
    if(CheckSize(image)) return image;
    switch(image->mType)
    {
    case 0:
        // LOST-DEBUG: none: exact only
        EE_LOST_DEBUG(6);
        return ScaleBits((RawImage1*)image,mWidth,mHeight).get();
    case 1:return ScalePixels<RawImage8,unsigned char>((RawImage8*)image,mWidth,mHeight).get();
    case 2:return ScalePixels<RawImage32,DWORD>((RawImage32*)image,mWidth,mHeight).get();
    default:return image;
    }
}

static inline DWORD TintColor(DWORD color,int redPercent,int greenPercent,int bluePercent)
{
    int red=((color>>16)&0xff)*redPercent/100;
    int green=((color>>8)&0xff)*greenPercent/100;
    int blue=(color&0xff)*bluePercent/100;
    if(red>255) red=255;
    if(green>255) green=255;
    if(blue>255) blue=255;
    return (((red<<8)|green)<<8)|blue;
}

// FUNCTION: WON 0x00748dd0
SmartPtr<RawImage> TintImageFilter::Filter(RawImage* image)
{
    if(image->mType==0) return image;
    if(image->mType==1)

    {
        RawImage8* raw=(RawImage8*)image;
        SmartPtr<Palette> palette=raw->mPalette->Duplicate();
        for(int i=0;i<palette->mNumColors;++i)
            palette->mColors[i]=TintColor(palette->mColors[i],mRed,mGreen,mBlue);
        raw->mPalette=palette;
        return image;
    }
    else if(image->mType==2)
    {
        RawImage32* raw=(RawImage32*)image;
        raw->StartRowTraversal(0);
        while(raw->HasMoreRows())
        {
            while(raw->HasMoreRowPixels())
            {
                DWORD color=raw->GetRowPixel();
                raw->SetRowPixel((color&0xff000000)|TintColor(color,mRed,mGreen,mBlue));
                raw->NextRowPixel();
            }
            raw->NextRow();
        }
        return image;
    }
    return image;
}

// FUNCTION: WON 0x00749110
SmartPtr<RawImage> TransparentImageFilter::Filter(RawImage* image)
{
    if(image->mType==0) return image;
    DWORD color=mTransparentColor;
    if(mUseSample)
    {
        if(image->mHeight<=0 || image->mWidth<=0) return image;
        int x=0,y=0;
        switch(mPixelSample)
        {
        case UpperLeft:x=0;y=0;break;
        case UpperRight:x=image->mWidth-1;y=0;break;
        case LowerLeft:x=0;y=image->mHeight-1;break;
        case LowerRight:x=image->mWidth-1;y=image->mHeight-1;break;
        }
        if(image->mType==1)
        {
            RawImage8* raw=(RawImage8*)image;
            raw->mTransparentColor=raw->GetPixel(x,y);
            return image;
        }
        if(image->mType==2) color=((RawImage32*)image)->GetPixel(x,y);
    }
    if(image->mType==1)
    {
        RawImage8* raw=(RawImage8*)image;
        Palette* palette=raw->mPalette.get();
        for(int i=0;i<palette->mNumColors;++i)
            if(palette->mColors[i]==color) { raw->mTransparentColor=i;break; }
        return image;
    }
    else if(image->mType==2)
    {
        RawImage32* raw=(RawImage32*)image;
        raw->mDoTransparency=true;
        raw->StartRowTraversal(0);
        while(raw->HasMoreRows())
        {
            while(raw->HasMoreRowPixels())
            {
                DWORD aColor=raw->GetRowPixel();
                if(aColor==color) raw->SetRowPixel(0);
                else raw->SetRowPixel(aColor|0xff000000);
                raw->NextRowPixel();
            }
            raw->NextRow();
        }
        return image;
    }
    return image;
}
}


namespace WONAPI {
// FUNCTION: WON 0x00748b60
void ScaleImageFilter::SetPercentSize(int thePercentWidth, int thePercentHeight)
{
	mPercentWidth = thePercentWidth;
	mPercentHeight = thePercentHeight;
	mIsPercent = true;
}

// FUNCTION: WON 0x00748b80
void ScaleImageFilter::SetAbsSize(int theWidth, int theHeight)
{
	mWidth = theWidth;
	mHeight = theHeight;
	mIsPercent = false;
}
}


// Genuine SDK ImageFilter.cpp additions; SOURCE_ONLY_NO_NATIVE_ID.
// Native/primary/physical credit remains zero until independently identified.
namespace WONAPI {
MultiImageFilter* MultiImageFilter::Add(ImageFilter *theFilter)
{
	mFilterList.push_back(theFilter);
	return this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SmartPtr<RawImage> MultiImageFilter::Filter(RawImage *theImage) 
{ 
	FilterList::iterator anItr = mFilterList.begin();
	SmartPtr<RawImage> anImage = theImage;
	while(anItr!=mFilterList.end())
	{
		ImageFilter *aFilter = *anItr;
		anImage = aFilter->Filter(anImage);
		++anItr;
	}
	return anImage; 
}

///////////////////////////////////////////////////////////////////////////////

static bool TrimFilter_IsRowEmpty(unsigned char *pixels, unsigned char transparentPixel, int width)
{
	for(int i=0; i<width; i++)
		if(*pixels++!=transparentPixel)
			return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsRowEmpty(DWORD *pixels, int width)
{
	for(int i=0; i<width; i++)
		if((*pixels++)&0xff000000)
			return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsColEmpty(unsigned char *pixels, unsigned char transparentPixel, int height, int rowInc)
{
	for(int i=0; i<height; i++, pixels+=rowInc)
	{
		if(*pixels!=transparentPixel)
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool TrimFilter_IsColEmpty(DWORD *pixels, int height, int rowInc)
{
	for(int i=0; i<height; i++, pixels+=rowInc)
	{
		if((*pixels)&0xff000000)
			return false;
	}

	return true;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SmartPtr<RawImage> TrimImageFilter::Filter(RawImage *theImage)
{
	mx = my = 0;
	mWidth = theImage->mWidth;
	mHeight = theImage->mHeight;

	if(!theImage->HasTransparency())
		return theImage;

	int x,y;
	int left = 0,top = 0,right = mWidth-1, bottom = mHeight-1;
	if(theImage->mType==1)
	{
		RawImage8 *anImage = (RawImage8*)theImage;
		unsigned char transparentPixel = anImage->mTransparentColor;
		unsigned char *pixels = anImage->mImageData;
		int bytesPerRow = anImage->mBytesPerRow;

		// Find Top
		for(y=0; y<mHeight; y++, pixels+=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,transparentPixel,mWidth))
				break;
		}
		if(y>=mHeight)
			return NULL;

		top = y;

		// Find Bottom
		pixels = anImage->mImageData + (mHeight-1)*bytesPerRow;
		for(y=bottom; y>top; y--, pixels-=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,transparentPixel,mWidth))
				break;
		}
		bottom = y;
		mHeight = bottom-top+1;

		// Find Left
		pixels = anImage->mImageData + top*bytesPerRow;
		for(x=0; x<mWidth; x++, pixels++)
		{
			if(!TrimFilter_IsColEmpty(pixels,transparentPixel,mHeight,bytesPerRow))
				break;
		}
		if(x>=mWidth)
			return NULL;

		left = x;
		
		// Find Right
		pixels = anImage->mImageData + top*bytesPerRow + (mWidth-1);
		for(x=right; x>left; x--, pixels--)
		{
			if(!TrimFilter_IsColEmpty(pixels,transparentPixel,mHeight,bytesPerRow))
				break;
		}
		right = x;
		mWidth = right-left+1;
	}
	else if(theImage->mType==2)
	{
		RawImage32 *anImage = (RawImage32*)theImage;
		DWORD *pixels = anImage->mImageData;
		int bytesPerRow = mWidth;

		// Find Top
		for(y=0; y<mHeight; y++, pixels+=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,mWidth))
				break;
		}
		if(y>=mHeight)
			return NULL;

		top = y;

		// Find Bottom
		pixels = anImage->mImageData + (mHeight-1)*bytesPerRow;
		for(y=bottom; y>top; y--, pixels-=bytesPerRow)
		{
			if(!TrimFilter_IsRowEmpty(pixels,mWidth))
				break;
		}
		bottom = y;
		mHeight = bottom-top+1;

		// Find Left
		pixels = anImage->mImageData + top*bytesPerRow;
		for(x=0; x<mWidth; x++, pixels++)
		{
			if(!TrimFilter_IsColEmpty(pixels,mHeight,bytesPerRow))
				break;
		}
		if(x>=mWidth)
			return NULL;

		left = x;
		
		// Find Right
		pixels = anImage->mImageData + top*bytesPerRow + (mWidth-1);
		for(x=right; x>left; x--, pixels--)
		{
			if(!TrimFilter_IsColEmpty(pixels,mHeight,bytesPerRow))
				break;
		}
		right = x;
		mWidth = right-left+1;
	}
	else
		return NULL;

	mx = left; my = top;
	mWidth = right-left+1;
	mHeight = bottom-top+1;
	return CropImageFilter::Filter(theImage);
}
///////////////////////////////////////////////////////////////////////////////
}
