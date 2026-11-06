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
#include "ee/wonapi/WONLobbyDelayLoadImage.h"

namespace WONAPI {
// FUNCTION: WON 0x00737c00
DelayLoadImage::DelayLoadImage(const std::string& path,int width,int height) : mImagePath(path)
{
    mHaveLoaded=false;
    mWidth=(unsigned short)width;
    mHeight=(unsigned short)height;
}
// FUNCTION: WON 0x00737c90
// (??_GDelayLoadImage@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00737cb0
// (??1DelayLoadImage@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x00737d50
void DelayLoadImage::Draw(DisplayContext* context,int x,int y,int left,int top,int width,int height)
{
    if(!mHaveLoaded)
    {
        mHaveLoaded=true;
        mImage=WONAPI::WindowManager::GetDefaultWindowManager()->DecodeImage(mImagePath.c_str());
    }
    if(mImage.get()!=NULL) mImage->Draw(context,x,y,left,top,width,height);
}
// FUNCTION: WON 0x00737e50
SmartPtr<NativeImage> DelayLoadImage::GetNative(DisplayContext*)
{
    if(!mHaveLoaded)
    {
        mHaveLoaded=true;
        mImage=WONAPI::WindowManager::GetDefaultWindowManager()->DecodeImage(mImagePath.c_str());
    }
    return mImage;
}
}
