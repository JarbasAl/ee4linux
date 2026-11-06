// Target raw image classes include their real Win32 pixel conversion methods.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;

// FUNCTION: WON 0x007d0410
SmartPtr<NativeImage> RawImage32::Fn3D0410(DisplayContext* context) {
    BITMAPINFO info;
    BITMAPINFOHEADER& header=info.bmiHeader;
    header.biSize=sizeof(BITMAPINFOHEADER);
    header.biWidth=mWidth;
    header.biHeight=-mHeight;
    header.biPlanes=1;
    header.biBitCount=32;
    header.biCompression=BI_RGB;
    HBITMAP bitmap=CreateDIBitmap(context->mDC,&header,CBM_INIT,mImageData,&info,DIB_RGB_COLORS);
    return new MSNativeImage(context,bitmap);
}

// FUNCTION: WON 0x007d04f0
void RawImage8::Fn3D04F0() {
    mBytesPerRow=mWidth;
    if(mBytesPerRow%4!=0) mBytesPerRow+=4-mBytesPerRow%4;
}

// FUNCTION: WON 0x007d0520
SmartPtr<NativeImage> RawImage8::Fn3D0520(DisplayContext* context) {
    char* buffer=new char[sizeof(BITMAPINFO)+mPalette->mNumColors*4];
    std::auto_ptr<char> releaseBuffer(buffer);
    BITMAPINFO* info=(BITMAPINFO*)buffer;
    BITMAPINFOHEADER& header=info->bmiHeader;
    header.biSize=sizeof(BITMAPINFOHEADER);
    header.biWidth=mWidth;
    header.biHeight=-mHeight;
    header.biPlanes=1;
    header.biBitCount=8;
    header.biCompression=BI_RGB;
    header.biSizeImage=0;
    header.biXPelsPerMeter=0;
    header.biYPelsPerMeter=0;
    header.biClrUsed=mPalette->mNumColors;
    header.biClrImportant=0;
    for(int i=0;i<mPalette->mNumColors;++i) {
        DWORD rgb=mPalette->mColors[i];
        RGBQUAD color;
        color.rgbRed=(rgb>>16)&0xff;
        color.rgbGreen=(rgb>>8)&0xff;
        color.rgbBlue=rgb&0xff;
        color.rgbReserved=0;
        info->bmiColors[i]=color;
    }
    if(mTransparentColor!=-1 && mTransparentColor<mPalette->mNumColors) {
        RGBQUAD black={0,0,0,0};
        info->bmiColors[mTransparentColor]=black;
    }
    HBITMAP bitmap=CreateDIBitmap(context->mDC,&header,CBM_INIT,mImageData,info,DIB_RGB_COLORS);
    return new MSNativeImage(context,bitmap);
}

// FUNCTION: WON 0x007d0690
void RawImage1::Fn3D0690() {
    mBytesPerRow=mBytesPerRowMinusPad;
    if(mBytesPerRow%4!=0) mBytesPerRow+=4-mBytesPerRow%4;
}

// FUNCTION: WON 0x007d06b0
SmartPtr<NativeMask> RawImage1::GetMask(DisplayContext* context) {
    char* buffer=new char[sizeof(BITMAPINFO)+8];
    std::auto_ptr<char> releaseBuffer(buffer);
    BITMAPINFO* info=(BITMAPINFO*)buffer;
    BITMAPINFOHEADER& header=info->bmiHeader;
    header.biSize=sizeof(BITMAPINFOHEADER);
    header.biWidth=mWidth;
    header.biHeight=-mHeight;
    header.biPlanes=1;
    header.biBitCount=1;
    header.biCompression=BI_RGB;
    header.biSizeImage=0;
    header.biXPelsPerMeter=0;
    header.biYPelsPerMeter=0;
    header.biClrUsed=2;
    header.biClrImportant=0;
    RGBQUAD black={0,0,0,0},white={0xff,0xff,0xff,0};
    info->bmiColors[0]=black;
    info->bmiColors[1]=white;
    HBITMAP bitmap=CreateDIBitmap(context->mDC,&header,CBM_INIT,mImageData,info,DIB_RGB_COLORS);
    return new MSNativeMask(context,bitmap);
}

// FUNCTION: WON 0x007d07e0
SmartPtr<NativeImage> RawImage1::GetNative(DisplayContext* context) {
    return new MSNativeImage(context,((MSNativeMask*)GetMask(context).get())->mMask);
}
