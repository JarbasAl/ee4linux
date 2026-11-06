// WONGUI native bitmap/DC/mask/icon ownership and GDI drawing.
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
// FUNCTION: WON 0x007cfe50
SmartPtr<RawImage32> MSNativeImage::GetRaw() {
    SmartPtr<RawImage32> image=new RawImage32(mWidth,mHeight);
    SelectBitmap(false);
    char infoBuffer[sizeof(BITMAPINFO)+8];
    BITMAPINFO& info=*(BITMAPINFO*)infoBuffer;
    BITMAPINFOHEADER& header=info.bmiHeader;
    header.biSize=sizeof(BITMAPINFOHEADER);
    header.biWidth=mWidth;
    header.biHeight=-mHeight;
    header.biPlanes=1;
    header.biBitCount=32;
    header.biCompression=BI_RGB;
    GetDIBits(mDC,mBitmap,0,mHeight,image->mImageData,&info,DIB_RGB_COLORS);
    if(mTransparentMask.get()!=NULL) {
        image->mDoTransparency=true;
        SmartPtr<RawImage1> mask=new RawImage1(mWidth,mHeight);
        header.biBitCount=1;
        GetDIBits(mDC,((MSNativeMask*)mTransparentMask.get())->mMask,0,mHeight,
            mask->mImageData,&info,DIB_RGB_COLORS);
        mask->StartRowTraversal(0);
        image->StartRowTraversal(0);
        for(int row=0;row<mHeight;++row) {
            while(image->mRowPtr<image->mRowEnd) {
                bool transparent=(*mask->mRowPtr&mask->mRowMask)!=0;
                if(!transparent) *image->mRowPtr|=0xff000000;
                ++image->mRowPtr;
                mask->mRowMask>>=1;
                if(mask->mRowMask==0) { ++mask->mRowPtr; mask->mRowMask=128; }
            }
            image->mRowPtr=image->mRowEnd;
            image->mRowEnd+=image->mWidth;
            mask->mRowPtr=mask->mRowEnd+mask->mRowPad;
            mask->mRowEnd+=mask->mBytesPerRow;
            mask->mRowMask=128;
        }
    }
    SelectBitmap(true);
    return image.get();
}

// FUNCTION: WON 0x007cf9b0
MSNativeMask::~MSNativeMask()
{
	DeleteObject(mMask); 
}

// FUNCTION: WON 0x007cf9f0
MSNativeIcon::~MSNativeIcon()
{
	DestroyIcon(mIcon);
}

// FUNCTION: WON 0x007cfa10
void MSNativeImage::SelectBitmap(bool select)
{
	if(mBitmapIsSelected==select)
		return;

	if(select)
		mOldBitmap = (HBITMAP)SelectObject(mDC, mBitmap);
	else
		SelectObject(mDC,mOldBitmap);

	mBitmapIsSelected = select;
}

// FUNCTION: WON 0x007cfa50
void MSNativeImage::Init(DisplayContext *theContext, HBITMAP theBitmap, MSNativeMask *theTransparentMask)
{	
	mBitmap = theBitmap;
	mTransparentMask = theTransparentMask;
	mBitmapIsSelected = false;

	mDC = CreateCompatibleDC(theContext->mDC);
	mDisplayContext = new DisplayContext(mDC);

	SelectBitmap(true);

	BITMAP     bm;
	POINT      size;
	GetObject(mBitmap, sizeof(BITMAP), (LPSTR)&bm);
	
	size.x = bm.bmWidth;            // Get width of bitmap
	size.y = bm.bmHeight;           // Get height of bitmap
	DPtoLP(mDC, &size, 1);			// Convert from
	mWidth = size.x;
	mHeight = size.y;
}

// FUNCTION: WON 0x007cfb70
MSNativeImage::MSNativeImage(DisplayContext *theContext, HBITMAP theBitmap, MSNativeMask *theTransparentMask) : NativeImage(theContext)
{
	Init(theContext, theBitmap, theTransparentMask);
}

// FUNCTION: WON 0x007cfbf0
MSNativeImage::MSNativeImage(DisplayContext* context,int width,int height) : NativeImage(context) {
    Init(context,CreateCompatibleBitmap(context->mDC,width,height),NULL);
}

// FUNCTION: WON 0x007cfc60
MSNativeImage::~MSNativeImage()
{
	delete mGraphics;
	mGraphics = NULL;
	DeleteDC(mDC);
	DeleteObject(mBitmap);
}

// FUNCTION: WON 0x007cfce0
Graphics* MSNativeImage::Virtual14()
{
	return new MSGraphics;
}

// FUNCTION: WON 0x007cfd40
void MSNativeImage::Draw(DisplayContext *theContext, int x, int y, int left, int top, int width, int height)
{
	if(!mBitmapIsSelected)
		SelectBitmap(true);

	HDC theDC = theContext->mDC;
	if(mTransparentMask.get()!=NULL)
	{
		HDC aMaskDC = CreateCompatibleDC(theDC);
		SelectObject(aMaskDC,((MSNativeMask*)mTransparentMask.get())->mMask);

		DWORD aTextColor = SetTextColor(theDC,0x000000); //off/non-transparent pixels -> black
		DWORD aBackColor = SetBkColor(theDC,0xFFFFFF);	// on/transparent pixels -> white
		
		BitBlt(theDC,x,y,width,height,aMaskDC,left,top,SRCAND);
		BitBlt(theDC,x,y,width,height,mDC,left,top,SRCPAINT);

		SetBkColor(theDC, aBackColor);
		SetTextColor(theDC, aTextColor);

		DeleteDC(aMaskDC);
	}
	else
		BitBlt(theDC, x, y, width, height, mDC, left, top, SRCCOPY);
}

// FUNCTION: WON 0x007d0130
SmartPtr<Image> MSNativeImage::Duplicate()
{
	SelectBitmap(true);
	HDC aCopyDC = CreateCompatibleDC(mDC);
	HBITMAP aBitmap = CreateCompatibleBitmap(mDC,mWidth,mHeight);
	SelectObject(aCopyDC,aBitmap);
	BitBlt(aCopyDC,0,0,mWidth,mHeight,mDC,0,0,SRCCOPY);
	DeleteDC(aCopyDC);

	return new MSNativeImage(mDisplayContext.get(), aBitmap, (MSNativeMask*)mTransparentMask.get());
}

// FUNCTION: WON 0x007d0350
SmartPtr<NativeIcon> MSNativeImage::GetIcon()
{
	ICONINFO anInfo;
	anInfo.fIcon = TRUE;
	anInfo.xHotspot = 0; 
	anInfo.yHotspot = 0; 
	anInfo.hbmMask = NULL;
	if(mTransparentMask.get()!=NULL)
		anInfo.hbmMask = ((MSNativeMask*)mTransparentMask.get())->mMask;
   
	anInfo.hbmColor = mBitmap; 

	HICON anIcon = CreateIconIndirect(&anInfo);
	return new MSNativeIcon(mDisplayContext.get(), anIcon);
}

// FUNCTION: WON 0x007d0230
SmartPtr<Cursor> MSNativeImage::GetCursor(int hotX,int hotY) {
    ICONINFO info;
    info.fIcon=FALSE;
    info.xHotspot=0;
    info.yHotspot=0;
    info.hbmMask=NULL;
    if(mTransparentMask.get()!=NULL)
        info.hbmMask=((MSNativeMask*)mTransparentMask.get())->mMask;
    info.hbmColor=mBitmap;
    return new MSCursor((HCURSOR)CreateIconIndirect(&info),true);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007cf9d0
// (public: virtual void * __thiscall WONAPI::MSNativeMask::`scalar deleting destructor'(unsigned int)    [??_GMSNativeMask@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007cfbd0
// (public: virtual void * __thiscall WONAPI::MSNativeImage::`scalar deleting destructor'(unsigned int)    [??_GMSNativeImage@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007d0300
// (protected: virtual void * __thiscall WONAPI::MSCursor::`scalar deleting destructor'(unsigned int)    [??_GMSCursor@WONAPI@@MAEPAXI@Z])
