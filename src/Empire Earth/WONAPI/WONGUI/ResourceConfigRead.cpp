// ResourceConfig scalar literals. The project's ResourceConfig nominal is the
// SDK ResourceConfig owner; preserve the already-established project identity.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cctype>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {


static int GetHexDigit(char inChar)
{
    if(inChar>='0' && inChar<='9') return inChar-'0';
    inChar=toupper(inChar);
    if(inChar>='A' && inChar<='F') return inChar-'A'+10;
    return -1;
}

// FUNCTION: WON 0x00790f20
void ResourceConfig::EnsureComma(StringParser& inParser)
{
    if(!inParser.CheckNextChar(','))
        throw ConfigObjectException("Expecting ','");
}

// FUNCTION: WON 0x00791060
void ResourceConfig::EnsureCloseParen(StringParser& inParser)
{
    if(!inParser.CheckNextChar(')'))
        throw ConfigObjectException("Expecting ')'");
}

// FUNCTION: WON 0x007911a0
int ResourceConfig::SafeReadColor(StringParser& inParser)
{
    inParser.SkipWhitespace();
    if(inParser.GetChar(0)=='(')
    {
        std::string type;
        inParser.IncrementPos(1);
        inParser.ReadString(type,false);
        int color=0;
        if(_stricmp(type.c_str(),"STD")==0)
        {
            EnsureComma(inParser);
            std::string name;
            inParser.ReadString(name,false);
            const char* aName=name.c_str();
            if(_stricmp(aName,"3DDarkShadow")==0) color=0x80000000;
            else if(_stricmp(aName,"3DFace")==0) color=0x80000001;
            else if(_stricmp(aName,"3DHilight")==0) color=0x80000002;
            else if(_stricmp(aName,"3DShadow")==0) color=0x80000003;
            else if(_stricmp(aName,"Scrollbar")==0) color=0x80000004;
            else if(_stricmp(aName,"ButtonText")==0) color=0x80000005;
            else if(_stricmp(aName,"GrayText")==0) color=0x80000006;
            else if(_stricmp(aName,"Hilight")==0) color=0x80000007;
            else if(_stricmp(aName,"HilightText")==0) color=0x80000008;
            else if(_stricmp(aName,"ToolTipBack")==0) color=0x80000009;
            else if(_stricmp(aName,"ToolTipText")==0) color=0x8000000a;
            else if(_stricmp(aName,"MenuBack")==0) color=0x8000000b;
            else if(_stricmp(aName,"MenuText")==0) color=0x8000000c;
            else if(_stricmp(aName,"Back")==0) color=0x8000000d;
            else if(_stricmp(aName,"Text")==0) color=0x8000000e;
            else if(_stricmp(aName,"Link")==0) color=0x8000000f;
            else if(_stricmp(aName,"LinkDown")==0) color=0x80000010;
            else throw ConfigObjectException("Invalid standard color: "+type);
        }
        else if(_stricmp(type.c_str(),"DEC")==0)
        {
            int red,green,blue;
            EnsureComma(inParser);
            inParser.ReadValue(red);
            EnsureComma(inParser);
            inParser.ReadValue(green);
            EnsureComma(inParser);
            inParser.ReadValue(blue);
            color=((red&0xff)<<16)|((green&0xff)<<8)|(blue&0xff);
        }
        EnsureCloseParen(inParser);
        return color;
    }
    int color=0;
    for(int i=0;i<6;i++)
    {
        color<<=4;
        int digit=GetHexDigit(inParser.GetChar(0));
        if(digit<0) throw ConfigObjectException("Invalid color specification.");
        inParser.IncrementPos(1);
        color|=digit;
    }
    return color;
}

// FUNCTION: WON 0x007916c0
int ResourceConfig::SafeReadFontStyle(StringParser& inParser)
{
    int style=0;
    std::string name;
    while(true)
    {
        inParser.ReadString(name,true);
        if(name.empty()) break;
        if(_stricmp(name.c_str(),"Bold")==0) style|=1;
        else if(_stricmp(name.c_str(),"Italic")==0) style|=2;
        else if(_stricmp(name.c_str(),"Plain")==0) {}
        else if(_stricmp(name.c_str(),"Underline")==0) style|=4;
        else throw ConfigObjectException("Unknown font style: "+name);
    }
    return style;
}

// Additional real ResourceConfig (SDK ResourceConfig) literals.
// Requires complete Background, SoundDescriptor, Window and WindowManager.
// FUNCTION: WON 0x00791870
SmartPtr<Font> ResourceConfig::SafeReadFont(StringParser& parser)
{
    std::string face;
    parser.ReadString(face,false);
    EnsureComma(parser);
    int style=SafeReadFontStyle(parser);
    EnsureComma(parser);
    int size;
    if(!parser.ReadValue(size) || size<=0)
        throw ConfigObjectException("Invalid font size.");
    return Window::GetDefaultWindow()->GetFont(FontDescriptor(face,style,size));
}

// FUNCTION: WON 0x00791ab0
SmartPtr<Sound> ResourceConfig::SafeReadSound(StringParser& parser)
{
    parser.SkipWhitespace();
    SoundDescriptor desc;
    std::string path;
    if(parser.GetChar(0)=='(')
    {
        parser.IncrementPos(1);
        parser.ReadString(path,false);
        EnsureComma(parser);
        int flags=0;
        while(true)
        {
            std::string flag;
            parser.ReadString(flag,true);
            if(flag.empty()) break;
            if(_stricmp(flag.c_str(),"Preload")==0) flags|=1;
            else if(_stricmp(flag.c_str(),"Music")==0) flags|=2;
            else if(_stricmp(flag.c_str(),"Muted")==0) flags|=4;
            else throw ConfigObjectException("Unknown sound flag: "+flag);
        }
        desc.SetSoundFlags(flags,true);
        EnsureCloseParen(parser);
    }
    else parser.ReadString(path,false);
    desc.mPath=ComponentConfig::GetResourceFile(path);
    SmartPtr<Sound> sound=WONAPI::WindowManager::GetDefaultWindowManager()->DecodeSound(desc);
    if(sound.get()==NULL)
        throw ConfigObjectException("Failed to decode sound: "+desc.mPath);
    return sound;
}

// FUNCTION: WON 0x00791ea0
SmartPtr<Image> ResourceConfig::SafeReadImage(StringParser& parser)
{
    std::string path;
    parser.ReadString(path,false);
    SmartPtr<Image> image=WONAPI::WindowManager::GetDefaultWindowManager()->DecodeDelayImage(ComponentConfig::GetResourceFile(path).c_str());
    if(image.get()==NULL)
        throw ConfigObjectException("Failed to decode image: "+path);
    return image;
}

// FUNCTION: WON 0x00792050
Background ResourceConfig::SafeReadBackground(StringParser& parser)
{
    std::string type;
    parser.ReadString(type,false);
    if(_stricmp(type.c_str(),"COLOR")==0)
    {
        EnsureComma(parser);
        return Background(SafeReadColor(parser));
    }
    if(_stricmp(type.c_str(),"IMAGE")==0)
    {
        EnsureComma(parser);
        return Background(SafeReadImage(parser).get());
    }
    if(_stricmp(type.c_str(),"STRETCHIMAGE")==0)
    {
        EnsureComma(parser);
        Background background(SafeReadImage(parser).get());
        background.mStretchImage=true;
        return background;
    }
    if(_stricmp(type.c_str(),"WATERMARK")==0)
    {
        EnsureComma(parser);
        Background background(SafeReadImage(parser).get());
        background.mUseOffsets=false;
        return background;
    }
    if(_stricmp(type.c_str(),"NONE")==0) return Background(-1);
    throw ConfigObjectException("Invalid background specification");
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00791680
// (int __cdecl WONAPI::GetHexDigit(char)    [?GetHexDigit@WONAPI@@YAHD@Z])
// TEMPLATE: WON 0x00791e60
// (public: __thiscall WONAPI::SoundDescriptor::~SoundDescriptor(void)    [??1SoundDescriptor@WONAPI@@QAE@XZ])
