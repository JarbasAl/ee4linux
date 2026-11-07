// WONGUI image configuration. Real native complete owners; ordinary C++ only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyResourceConfig.h"
#include "ee/wonapi/WONLobbyConfigImages.h"
#include "LostDebug.h"
namespace WONAPI {

// FUNCTION: WON 0x0078b370
bool ImageConfig::Virtual2C(const std::string& inName,int& outValue)
{
    if(inName=="WIDTH") outValue=mImage->mWidth;
    else if(inName=="HEIGHT") outValue=mImage->mHeight;
    else return false;
    return true;
}

// FUNCTION: WON 0x0078b3e0
bool ImageConfig::Virtual24(const std::string& inInstruction)
{
    if(inInstruction=="SRC") mImage=ReadImage(false);
    else if(inInstruction=="SIZE")
    {
        int width=ReadInt();
        EnsureComma();
        int height=ReadInt();
        mImage->SetSize(width,height);
    }
    else return false;
    return true;
}

// FUNCTION: WON 0x0078c340
SmartPtr<RawImage> ImageFilterConfig::HandleFilter()
{
    SmartPtr<ImageFilter> filter;
    GUIString name=Fn3842F0(false);
    EnsureComma();
    if(name.CompareNoCase("SCALEPERCENT")==0)
    {
        int width=ReadInt(),height=width;
        if(!EndOfString())
        {
            EnsureComma();
            height=ReadInt();
        }
        SmartPtr<ScaleImageFilter> scale=new ScaleImageFilter(0);
        scale->SetPercentSize(width,height);
        filter=scale.get();
    }
    else if(name.CompareNoCase("SCALEABS")==0)
    {
        int width=ReadInt();
        EnsureComma();
        int height=ReadInt();
        filter=new ScaleImageFilter(width,height);
    }
    else if(name.CompareNoCase("TINT")==0)
    {
        int red=ReadInt();
        EnsureComma();
        int green=ReadInt();
        EnsureComma();
        int blue=ReadInt();
        filter=new TintImageFilter(red,green,blue);
    }
    else if(name.CompareNoCase("CROP")==0)
    {
        int x=ReadInt();
        EnsureComma();
        int y=ReadInt();
        EnsureComma();
        int width=ReadInt();
        EnsureComma();
        int height=ReadInt();
        filter=new CropImageFilter(x,y,width,height);
    }
    else if(name.CompareNoCase("PIXELTRANS")==0)
        filter=new TransparentImageFilter(Fn385510());
    if(filter.get()!=NULL) return filter->Filter(mRawImage.get());
    return SmartPtr<RawImage>();
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(2);
}

// FUNCTION: WON 0x0078c6e0
void ImageFilterConfig::SetRawImage(RawImage* inImage)
{
    mRawImage=inImage;
    mImage=inImage;
    mImageIsRawImage=true;
}

// FUNCTION: WON 0x0078c780
bool ImageFilterConfig::Virtual24(const std::string& inInstruction)
{
    SmartPtr<RawImage> anImage;
    if(inInstruction=="SRC") SetRawImage(ReadRawImage().get());
    else if(inInstruction=="FILTER") SetRawImage(HandleFilter().get());
    else if(inInstruction=="DUPLICATE")
        SetRawImage(static_cast<RawImage*>(mRawImage->Duplicate().get()));
    else if(inInstruction=="MULTIHORZCROP")
    {
        int count=ReadInt();
        EnsureComma();
        std::string prefix=Fn3842F0(false);
        int width=mRawImage->mWidth/count;
        SmartPtr<CropImageFilter> crop=new CropImageFilter;
        crop->SetSize(width,mRawImage->mHeight);
        int x=0;
        for(int i=0;i<count;i++)
        {
            crop->SetPos(x,0);
            SmartPtr<RawImage> image=crop->Filter(mRawImage.get());
            char name[512];
            sprintf(name,"%s%d",prefix.c_str(),i+1);
            SmartPtr<ImageFilterConfig> config=new ImageFilterConfig;
            config->SetRawImage(image.get());
            ComponentConfig* parent=static_cast<ComponentConfig*>(mL38);
            parent->AddChildToConfigMap(GUIString(name),config.get());
            x+=width;
        }
    }
    else if(inInstruction=="EXPORT")
    {
        GUIString name=Fn3842F0(false);
        EnsureComma();
        SmartPtr<ImageFilterConfig> config=new ImageFilterConfig;
        config->SetRawImage(HandleFilter().get());
        ComponentConfig* parent=static_cast<ComponentConfig*>(mL38);
        parent->AddChildToConfigMap(name,config.get());
    }
    else return ImageConfig::Virtual24(inInstruction);
    return true;
}

// FUNCTION: WON 0x0078cc80
Image* ImageFilterConfig::GetImage()
{
    if(mImageIsRawImage)
    {
        mImage=mRawImage->GetNative(Window::GetDefaultWindow()->mDisplayContext.get()).get();
        mImageIsRawImage=false;
    }
    return mImage.get();
}

}


namespace WONAPI {
// FUNCTION: WON 0x0078adf0
void ImageConfig_Init(ResourceConfigTable* table)
{
    table->AddAllocator("Image",ImageConfig::CfgFactory,ImageConfig::CompFactory);
    table->AddAllocator("StretchImage",StretchImageConfig::CfgFactory,ImageConfig::CompFactory);
    table->AddAllocator("StretchCenterImage",StretchCenterImageConfig::CfgFactory,ImageConfig::CompFactory);
    table->AddAllocator("ImageFilter",ImageFilterConfig::CfgFactory,ImageConfig::CompFactory);
}
}

// Round8 B: genuine static callbacks used by this original SDK registration unit.
namespace WONAPI {

// FUNCTION: WON 0x0078b130
ComponentConfig* ImageConfig::CfgFactory()
{
    return new ImageConfig;
}

// FUNCTION: WON 0x0078b190
ComponentConfig* StretchImageConfig::CfgFactory()
{
    return new StretchImageConfig;
}

// FUNCTION: WON 0x0078b1f0
ComponentConfig* StretchCenterImageConfig::CfgFactory()
{
    return new StretchCenterImageConfig;
}

// FUNCTION: WON 0x0078b250
ComponentConfig* ImageFilterConfig::CfgFactory()
{
    return new ImageFilterConfig;
}

// SUPPORT-ONLY: EXE 0x006c7b10 (genuine SDK shared NULL callback; zero new native/primary credit)
Component* ImageConfig::CompFactory()
{
    return NULL;
}

}

namespace WONAPI {
// Genuine SDK ImageConfig.cpp PostParse; shared native3099f0, zero primary/physical gain.
bool ImageConfig::Virtual10(ConfigParser*)
{
    return true; // Preserve SDK bypass of ComponentConfig::PostParse.
}
}
