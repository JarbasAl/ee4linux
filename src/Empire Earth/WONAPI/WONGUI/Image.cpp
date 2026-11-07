// Source-only reconstructed WONGUI Animation; no runtime substitution authority.
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
#include "ee/wonapi/WONLobbyImageHelpers.h"

// Image, NativeImage and RawImage are one original object: the base destructor ~Image is defined first, so
// VC6 knows it cannot throw and the derived destructors call it without the -1 EH state store.
namespace WONAPI {
// SUPPORT-ONLY: actual shared empty normal328590; zero primary credit.
Image::~Image() {}
// SUPPORT-ONLY: actual Image vslot04 ->3285d0, RET1c.
// FUNCTION: WON 0x007285d0
void Image::Draw(DisplayContext*,int,int,int,int,int,int) {}
// SUPPORT-ONLY: actual Image vslot08 ->2fa2a0, RET8.
void Image::SetSize(int,int) {}
// SUPPORT-ONLY: actual Image vslot0c ->32b9d0, owning-return NULL.
SmartPtr<Image> Image::Duplicate() { return NULL; }
}

namespace WONAPI {
// FUNCTION: WON 0x007285e0
SmartPtr<NativeImage> Image::GetNative(DisplayContext* context)
{
    SmartPtr<NativeImage> image=context->CreateImage(mWidth,mHeight);
    image->GetGraphics().DrawImage(this,0,0);
    return image;
}
// FUNCTION: WON 0x007286d0
SmartPtr<Image> ProgramImage::Duplicate()
{
    return new ProgramImage(mWidth,mHeight,mPaintFunc);
}
ProgramImage::ProgramImage(int width,int height,PaintFunc paint)
{
    mWidth=(unsigned short)width;
    mHeight=(unsigned short)height;
    mPaintFunc=paint;
}
// FUNCTION: WON 0x007287c0
void ProgramImage::Paint(Graphics& g)
{
    if(mPaintFunc!=NULL) mPaintFunc(g,mWidth,mHeight);
}
// FUNCTION: WON 0x007287f0
void ProgramImage::Draw(DisplayContext* context,int x,int y,int left,int top,int width,int height)
{
    Graphics* g=context->mGraphics;
    if(g==NULL) return;
    int tx=g->mTransX, ty=g->mTransY;
    g->Translate(x-tx,y-ty);
    bool clipped=false;
    if(left!=0 || top!=0 || width!=mWidth || height!=mHeight)
    {
        clipped=true;
        g->PushClipRect(left,top,width,height);
    }
    Paint(*g);
    if(clipped) g->PopClipRect();
    g->Translate(tx-x,ty-y);
}
}


// Genuine SDK Image.cpp remaining original bodies; SOURCE_ONLY_NO_NATIVE_ID.
namespace WONAPI {
ImagePiece::ImagePiece(Image *theImage, int left, int top, int width, int height) : mImage(theImage)
{
	mLeft = left;
	mTop = top;
	mWidth = width;
	mHeight = height;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePiece::~ImagePiece()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImagePiece::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	mImage->Draw(theContext,x,y,left+mLeft,top+mTop,width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ProgramImage::ProgramImage(PaintFunc thePaintFunc)
{
	mWidth = mHeight = 0;
	mPaintFunc = thePaintFunc;
}

///////////////////////////////////////////////////////////////////////////////
}

using namespace WONAPI;

// FUNCTION: WON 0x007288b0
NativeMask::NativeMask(DisplayContext* context) : mDisplayContext(context) {}
// FUNCTION: WON 0x00728920
NativeMask::~NativeMask() {}

// FUNCTION: WON 0x00728a70
NativeImage::NativeImage(DisplayContext* context) {
    mGraphics=NULL;
    mDisplayContext=context;
}

// FUNCTION: WON 0x00728b20
NativeImage::~NativeImage() {
    delete mGraphics;
    mGraphics=NULL;
}

// FUNCTION: WON 0x00728bf0
SmartPtr<NativeImage> NativeImage::GetNative(DisplayContext*) { return this; }

// FUNCTION: WON 0x00728c20
Graphics& NativeImage::GetGraphics() {
    if(mGraphics==NULL) {
        mGraphics=Virtual14();
        mGraphics->Attach(mDisplayContext.get());
    }
    return *mGraphics;
}


// FUNCTION: WON 0x00728c50
void RawImage::Draw(DisplayContext* context,int x,int y,int left,int top,int width,int height) {
    SmartPtr<NativeImage> native=GetNative(context);
    native->Draw(context,x,y,left,top,width,height);
}

// FUNCTION: WON 0x00728cf0
RawImage32::RawImage32(int width,int height) : RawImage(2) {
    mWidth=width;
    mHeight=height;
    mDoTransparency=false;
    mImageData=new DWORD[width*height];
    mImageEnd=mImageData+width*height;
}

// FUNCTION: WON 0x00728da0
RawImage32::~RawImage32() { delete [] mImageData; }

// FUNCTION: WON 0x00728df0
SmartPtr<Image> RawImage32::Duplicate() {
    SmartPtr<RawImage32> image=new RawImage32(mWidth,mHeight);
    image->mDoTransparency=mDoTransparency;
    memcpy(image->mImageData,mImageData,mWidth*mHeight*4);
    return image.get();
}

// FUNCTION: WON 0x00728ee0
DWORD RawImage32::GetPixel(int x,int y) { return mImageData[y*mWidth+x]; }

// FUNCTION: WON 0x00728f00
DWORD* RawImage32::StartRowTraversal(int row) {
    mRowPtr=mImageData+row*mWidth;
    mRowEnd=mRowPtr+mWidth;
    return mRowPtr;
}

// FUNCTION: WON 0x00729170
RawImage8::RawImage8(int width,int height,Palette* palette) : RawImage(1) {
    mWidth=width;
    mHeight=height;
    mPalette=palette;
    mTransparentColor=-1;
    Fn3D04F0();
    mRowPad=mBytesPerRow-mWidth;
    mImageData=new unsigned char[height*mBytesPerRow];
    mImageEnd=mImageData+mBytesPerRow*height;
}

// FUNCTION: WON 0x00729280
RawImage8::~RawImage8() { delete mImageData; }

// FUNCTION: WON 0x00729300
SmartPtr<Image> RawImage8::Duplicate() {
    SmartPtr<RawImage8> image=new RawImage8(mWidth,mHeight,mPalette.get());
    image->mPalette=mPalette;
    image->mTransparentColor=mTransparentColor;
    memcpy(image->mImageData,mImageData,mBytesPerRow*mHeight);
    return image.get();
}

// FUNCTION: WON 0x00729420
unsigned char RawImage8::GetPixel(int x,int y) { return mImageData[y*mBytesPerRow+x]; }

// FUNCTION: WON 0x00729440
unsigned char* RawImage8::StartRowTraversal(int row) {
    mRowPtr=mImageData+row*mBytesPerRow;
    mRowEnd=mRowPtr+mWidth;
    return mRowPtr;
}

// FUNCTION: WON 0x00729690
RawImage1::RawImage1(int width,int height) : RawImage(0) {
    mWidth=width;
    mHeight=height;
    mBytesPerRowMinusPad=mWidth/8;
    mCompleteBytesPerRow=mBytesPerRowMinusPad;
    mWidthMod8=mWidth%8;
    if(mWidthMod8!=0) ++mBytesPerRowMinusPad;
    Fn3D0690();
    mRowPad=mBytesPerRow-mBytesPerRowMinusPad;
    if(mWidthMod8!=0) ++mRowPad;
    mEndRowMask=1<<(8-mWidthMod8);
    mImageData=new unsigned char[mBytesPerRow*mHeight];
    mImageEnd=mImageData+mBytesPerRow*mHeight;
}

// FUNCTION: WON 0x00729790
RawImage1::~RawImage1() { delete mImageData; }

// FUNCTION: WON 0x007297e0
unsigned char* RawImage1::StartRowTraversal(int row) {
    mRowMask=128;
    mRowPtr=mImageData+row*mBytesPerRow;
    mRowEnd=mRowPtr+mCompleteBytesPerRow;
    return mRowPtr;
}


// Appended to owned RawImage.cpp only after Root sourceRESUME.
// FUNCTION: WON 0x00728f30
SmartPtr<NativeImage> RawImage32::GetNative(DisplayContext* context) {
    if(mDoTransparency) {
        int width=GetWidth(),height=GetHeight();
        SmartPtr<RawImage1> mask=new RawImage1(width,height);
        for(int y=0;y<height;++y) {
            mask->StartRowTraversal(y);
            StartRowTraversal(y);
            while(mRowPtr<mRowEnd) {
                if((*mRowPtr&0xff000000)==0) {
                    *mRowPtr=0;
                    mask->SetRowPixel(true);
                } else mask->SetRowPixel(false);
                ++mRowPtr;
                mask->NextRowPixel();
            }
        }
        SmartPtr<NativeImage> native=Fn3D0410(context);
        native->SetTransparentMask(mask->GetMask(context).get());
        return native;
    }
    return Fn3D0410(context);
}

// FUNCTION: WON 0x00729460
SmartPtr<NativeImage> RawImage8::GetNative(DisplayContext* context) {
    SmartPtr<NativeImage> native=Fn3D0520(context);
    if(mTransparentColor!=-1) {
        SmartPtr<RawImage1> mask=new RawImage1(mWidth,mHeight);
        for(int y=0;y<mHeight;++y) {
            StartRowTraversal(y);
            mask->StartRowTraversal(y);
            while(mRowPtr<mRowEnd) {
                mask->SetRowPixel(GetRowPixel()==mTransparentColor);
                mask->NextRowPixel();
                NextRowPixel();
            }
        }
        native->SetTransparentMask(mask->GetMask(context).get());
    }
    return native;
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00728790
// (public: __thiscall WONAPI::ProgramImage::ProgramImage(int,int,void (__cdecl*)(class WONAPI::Graphics &,int,int))    [??0ProgramImage@WONAPI@@QAE@HHP6A)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00728900
// (public: virtual void * __thiscall WONAPI::NativeMask::`scalar deleting destructor'(unsigned int)    [??_GNativeMask@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00728b00
// (public: virtual void * __thiscall WONAPI::NativeImage::`scalar deleting destructor'(unsigned int)    [??_GNativeImage@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00728d80
// (public: virtual void * __thiscall WONAPI::RawImage32::`scalar deleting destructor'(unsigned int)    [??_GRawImage32@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00729260
// (public: virtual void * __thiscall WONAPI::RawImage8::`scalar deleting destructor'(unsigned int)    [??_GRawImage8@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00729770
// (public: virtual void * __thiscall WONAPI::RawImage1::`scalar deleting destructor'(unsigned int)    [??_GRawImage1@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00728d70
// (public: virtual bool __thiscall WONAPI::RawImage32::HasTransparency(void))
// TEMPLATE: WON 0x00729250
// (public: virtual bool __thiscall WONAPI::RawImage8::HasTransparency(void))
