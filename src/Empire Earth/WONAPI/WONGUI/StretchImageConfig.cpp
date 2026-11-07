// WONGUI stretch image configuration: real80 owners, ordinary C++.
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
#include "ee/wonapi/WONLobbyConfigImages.h"
namespace WONAPI {
// FUNCTION: WON 0x0078b4e0
StretchImageConfig::StretchImageConfig()
{
    mStretchImage=new StretchImage(NULL,0,0,0,0);
    mImage=mStretchImage;
}

// FUNCTION: WON 0x0078b630
bool StretchImageConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SRC") {
        mStretchImage->SetImage(ReadRawImage().get());
    }
    else if(instruction=="TILE") {
        bool horz=Fn3837F0();EnsureComma();
        bool vert=Fn3837F0();
        mStretchImage->SetTile(horz,vert);
    }
    else if(instruction=="BORDERS") {
        int left=ReadInt();EnsureComma();
        int top=ReadInt();EnsureComma();
        int right=ReadInt();EnsureComma();
        mStretchImage->SetBorders(left,top,right,ReadInt());
        if(!EndOfString()) {
            EnsureComma();
            bool horz=Fn3837F0();EnsureComma();
            bool vert=Fn3837F0();
            mStretchImage->SetTile(horz,vert);
        }
    }
    else if(instruction=="SKIPCENTER") {
        mStretchImage->SetSkipCenter(Fn3837F0());
    }
    else if(instruction=="MULTIEXPORT") {
        int count=ReadInt();EnsureComma();
        std::string imagePrefix=ReadRawString(false);EnsureComma();
        std::string configPrefix=ReadRawString(false);
        int offset=0;
        if(!EndOfString()) {
            EnsureComma();
            offset=ReadInt()-1;
        }
        for(int i=0;i<count;++i) {
            char imageName[512],configName[512];
            sprintf(imageName,"%s%d",imagePrefix.c_str(),i+1+offset);
            sprintf(configName,"%s%d",configPrefix.c_str(),i+1);
            SmartPtr<RawImage> image=GetRawImageFromStr(imageName);
            SmartPtr<StretchImageConfig> child=new StretchImageConfig;
            child->mStretchImage->CopyAttributes(mStretchImage);
            child->mStretchImage->SetImage(image.get());
            ComponentConfig* parent=static_cast<ComponentConfig*>(GetParentObject());
            parent->AddChildToConfigMap(configName,child.get());
        }
    }
    else if(instruction=="MULTICROPEXPORT") {
        int count=ReadInt();EnsureComma();
        SmartPtr<RawImage> image=ReadRawImage();EnsureComma();
        std::string configPrefix=ReadRawString(false);
        int width=image->GetWidth()/count;
        SmartPtr<CropImageFilter> crop=new CropImageFilter;
        crop->SetSize(width,image->GetHeight());
        for(int i=0;i<count;i++) {
            crop->SetPos(i*width,0);
            char configName[512];
            sprintf(configName,"%s%d",configPrefix.c_str(),i+1);
            SmartPtr<RawImage> cropped=crop->Filter(image.get());
            SmartPtr<StretchImageConfig> child=new StretchImageConfig;
            child->mStretchImage->CopyAttributes(mStretchImage);
            child->mStretchImage->SetImage(cropped.get());
            ComponentConfig* parent=static_cast<ComponentConfig*>(GetParentObject());
            parent->AddChildToConfigMap(configName,child.get());
        }
    }
    else if(instruction=="EXPORT") {
        GUIString name=Fn3842F0(false);EnsureComma();
        SmartPtr<StretchImageConfig> child=new StretchImageConfig;
        child->mStretchImage->CopyAttributes(mStretchImage);
        child->mStretchImage->SetImage(ReadRawImage().get());
        ComponentConfig* parent=static_cast<ComponentConfig*>(GetParentObject());
        parent->AddChildToConfigMap(name,child.get());
    }
    else return ImageConfig::Virtual24(instruction);
    return true;
}

// FUNCTION: WON 0x0078bf90
StretchCenterImageConfig::StretchCenterImageConfig()
{
    mStretchCenterImage=new StretchCenterImage;
    mImage=mStretchCenterImage;
}

// FUNCTION: WON 0x0078c0e0
bool StretchCenterImageConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="STRETCHIMAGE") mStretchCenterImage->SetStretchImage(ReadImage(false).get());
    else if(instruction=="CENTERIMAGE") mStretchCenterImage->SetCenterImage(ReadImage(false).get());
    else return ImageConfig::Virtual24(instruction);
    return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0078b2d0
// (public: virtual class WONAPI::Image * __thiscall WONAPI::ImageConfig::GetImage(void)    [?GetImage@ImageConfig@WONAPI@@UAEPAVImage@2@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0078b300
// (public: virtual __thiscall WONAPI::ImageConfig::~ImageConfig(void)    [??1ImageConfig@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0078b5a0
// (public: virtual void * __thiscall WONAPI::StretchImageConfig::`scalar deleting destructor'(unsigned int)    [??_GStretchImageConfig@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0078b5c0
// (public: virtual __thiscall WONAPI::StretchImageConfig::~StretchImageConfig(void)    [??1StretchImageConfig@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0078c050
// (public: virtual void * __thiscall WONAPI::StretchCenterImageConfig::`scalar deleting destructor'(unsigned int)    [??_GStretchCenterImageConfig@WONAPI)
// TEMPLATE: WON 0x0078c070
// (public: virtual __thiscall WONAPI::StretchCenterImageConfig::~StretchCenterImageConfig(void)    [??1StretchCenterImageConfig@WONAPI@@UAE@XZ])
