// Complete native configuration value readers; external resources remain borrowed until retained.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyConfigImages.h"
namespace WONAPI {
// FUNCTION: WON 0x007842f0
GUIString ComponentConfig::Fn3842F0(bool stopAtWhitespace)
{
    mStringParser->SkipWhitespace();
    if(mStringParser->GetChar(0)=='@') {
        mStringParser->IncrementPos(1);
        std::string key;
        mStringParser->ReadString(key,false);
        return GetStringResource(key);
    }
    std::wstring value;
    mStringParser->ReadString(value,stopAtWhitespace);
    return GUIString(value);
}
// FUNCTION: WON 0x00784480
SmartPtr<Font> ComponentConfig::ReadFont()
{
    mStringParser->SkipWhitespace();
    if(mStringParser->GetChar(0)=='@') {
        mStringParser->IncrementPos(1);
        std::string name;
        mStringParser->ReadString(name,false);
        return GetFontResource(name);
    }
    if(mStringParser->GetChar(0)=='(') {
        mStringParser->IncrementPos(1);
        std::string type;
        StringParser* aParser=mStringParser;
        aParser->ReadString(type,false);
        if(_stricmp(type.c_str(),"CFG")!=0)
            throw ConfigObjectException("Bad Font Type: "+type);
        EnsureComma();
        std::string name;
        mStringParser->ReadString(name,false);
        ComponentConfig* current=this;
        FontConfig* fontConfig=NULL;
        while(true) {
            fontConfig=dynamic_cast<FontConfig*>(current->GetComponentConfig(name,false));
            if(fontConfig!=NULL) break;
            current=(ComponentConfig*)current->mL38;
        }
        if(fontConfig->GetFont()==NULL)
            throw ConfigObjectException("NULL Font: "+name);
        if(!mStringParser->CheckNextChar(')'))
            throw ConfigObjectException("Expecting ')'");
        return fontConfig->GetFont();
    }
    return ResourceConfig::SafeReadFont(*mStringParser);
}
// FUNCTION: WON 0x00784890
SmartPtr<Image> ComponentConfig::GetImageFromStr(const std::string& spec,bool getRaw)
{
    if(spec[0]=='@') {
        std::string::size_type split=spec.find_first_of('_');
        if(split!=std::string::npos) {
            std::string type=spec.substr(1,split-1);
            if(_stricmp(type.c_str(),"CFG")==0) {
                std::string name=spec.substr(split+1);
                ComponentConfig* current=this;
                ImageConfig* config=NULL;
                while(current!=NULL) {
                    config=dynamic_cast<ImageConfig*>(current->GetComponentConfig(name,false));
                    if(config!=NULL) break;
                    current=(ComponentConfig*)current->mL38;
                }
                if(config==NULL)
                    throw ConfigObjectException("Invalid ImageConfig: "+spec);
                SmartPtr<Image> image;
                if(getRaw) image=config->GetRawImage();
                else image=config->GetImage();
                if(image.get()==NULL)
                    throw ConfigObjectException("Image Doesn't Exist: "+spec);
                return image;
            }
        } else if(_stricmp(spec.c_str(),"@NULL")==0) return NULL;
        else return GetImageResource(spec.c_str()+1);
        throw ConfigObjectException("Bad Image Specification: "+spec);
    }
    SmartPtr<Image> image;
    if(getRaw)
        image=Window::GetDefaultWindow()->DecodeImageRaw(GetResourceFile(spec).c_str()).get();
    else image=WONAPI::WindowManager::GetDefaultWindowManager()->DecodeDelayImage(GetResourceFile(spec).c_str()).get();
    if(image.get()==NULL)
        throw ConfigObjectException("Failed to decode image: "+spec);
    return image;
}
// FUNCTION: WON 0x00784f10
// Owning-result conversion support0.
SmartPtr<RawImage> ComponentConfig::GetRawImageFromStr(const std::string& spec)
{
    return (RawImage*)GetImageFromStr(spec,true).get();
}
// FUNCTION: WON 0x00784fa0
// Cohesive parser/forwarding support0.
SmartPtr<Image> ComponentConfig::ReadImage(bool getRaw)
{
    std::string name;
    StringParser& aParser=*mStringParser;
    aParser.ReadString(name,false);
    return GetImageFromStr(name,getRaw);
}
// FUNCTION: WON 0x00785050
// Owning-result conversion support0.
SmartPtr<RawImage> ComponentConfig::ReadRawImage()
{
    SmartPtr<RawImage> result=(RawImage*)ReadImage(true).get();
    return result;
}
// FUNCTION: WON 0x00785120
StandardCursor ComponentConfig::ReadStandardCursor()
{
    GUIString name=Fn3842F0(false);
    if(name.CompareNoCase("Arrow")==0) return StandardCursor_Arrow;
    if(name.CompareNoCase("Hand")==0) return StandardCursor_Hand;
    if(name.CompareNoCase("IBeam")==0) return StandardCursor_IBeam;
    if(name.CompareNoCase("EastWest")==0) return StandardCursor_EastWest;
    if(name.CompareNoCase("NorthSouth")==0) return StandardCursor_NorthSouth;
    if(name.CompareNoCase("NorthSouthEastWest")==0) return StandardCursor_NorthSouthEastWest;
    throw ConfigObjectException("Invalid predefined cursor type: "+(std::string)name);
}
// FUNCTION: WON 0x00785380
SmartPtr<Cursor> ComponentConfig::ReadCursor()
{
    GUIString type=Fn3842F0(false);
    if(type.CompareNoCase("Predefined")==0) {
        EnsureComma();
        return Cursor::GetStandardCursor(ReadStandardCursor());
    }
    throw ConfigObjectException("Invalid cursor type: "+(std::string)type);
}
// FUNCTION: WON 0x00785cd0
SelectionColor ComponentConfig::ReadSelectionColor()
{
    DWORD background=Fn385510();
    if(EndOfString()) return background;
    EnsureComma();
    GUIString type=Fn3842F0(false);
    if(type.CompareNoCase("USEFOREGROUND")==0) {
        EnsureComma();
        DWORD foreground=Fn385510();
        return SelectionColor(background,foreground);
    }
    if(type.CompareNoCase("INVERTORIGINAL")==0)
        return SelectionColor(background,ForegroundSelectionMode_InvertOriginalColor);
    if(type.CompareNoCase("USEORIGINAL")==0)
        return SelectionColor(background,ForegroundSelectionMode_UseOriginalColor);
    throw ConfigObjectException("Invalid SelectionColor");
}
}
