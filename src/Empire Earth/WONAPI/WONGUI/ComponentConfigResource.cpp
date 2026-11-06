// WONGUI resource path configuration. Original unit grouping provisional.
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
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {

// FUNCTION: WON 0x00787ca0
void ComponentConfig::SetResourceFolder(const std::string& inFolder)
{
    mResourceFolder=inFolder;
    if(mResourceFolder[mResourceFolder.length()-1]!='/' &&
        mResourceFolder[mResourceFolder.length()-1]!='\\')
        mResourceFolder+='/' ;
}

// FUNCTION: WON 0x00787e50
std::string ComponentConfig::GetResourceFile(const std::string& inFile)
{
    if(inFile.length()>=2)
    {
        if(inFile[1]==':') return inFile;
        if(inFile[0]=='.' && (inFile[1]=='/' || inFile[1]=='\\'))
            return inFile.substr(2);
    }
    else if(_strnicmp(inFile.c_str(),"\\\\",2)==0 ||
            _strnicmp(inFile.c_str(),"html",4)==0)
        return inFile;
    return mResourceFolder+inFile;
}

// FUNCTION: WON 0x00783bd0
GUIString ComponentConfig::GetStringResource(const std::string& inKey)
{
    GUIString value;
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetString(inKey,value)) return value;
    if(mL38!=NULL) return static_cast<ComponentConfig*>(mL38)->GetStringResource(inKey);
    throw ConfigObjectException("String resource not found: "+inKey);
}

// FUNCTION: WON 0x00783d60
Image* ComponentConfig::GetImageResource(const std::string& inKey)
{
    Image* value=NULL;
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetImage(inKey,value)) return value;
    if(mL38!=NULL) return static_cast<ComponentConfig*>(mL38)->GetImageResource(inKey);
    throw ConfigObjectException("Image resource not found: "+inKey);
}

// FUNCTION: WON 0x00783e50
Font* ComponentConfig::GetFontResource(const std::string& inKey)
{
    Font* value=NULL;
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetFont(inKey,value)) return value;
    if(mL38!=NULL) return static_cast<ComponentConfig*>(mL38)->GetFontResource(inKey);
    throw ConfigObjectException("Font resource not found: "+inKey);
}

// FUNCTION: WON 0x00783f40
SmartPtr<Sound> ComponentConfig::GetSoundResource(const std::string& inKey)
{
    Sound* value=NULL;
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetSound(inKey,value)) return value;
    if(mL38!=NULL) return static_cast<ComponentConfig*>(mL38)->GetSoundResource(inKey);
    throw ConfigObjectException("Sound resource not found: "+inKey);
}

// FUNCTION: WON 0x00784050
DWORD ComponentConfig::GetColorResource(const std::string& inKey)
{
    DWORD value=0;
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetColor(inKey,value)) return value;
    if(mL38!=NULL) return static_cast<ComponentConfig*>(mL38)->GetColorResource(inKey);
    throw ConfigObjectException("Color resource not found: "+inKey);
}

// FUNCTION: WON 0x00784140
void ComponentConfig::GetBackgroundResource(const std::string& inKey,Background& outValue)
{
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetBackground(inKey,outValue)) return;
    if(mL38!=NULL)
    {
        static_cast<ComponentConfig*>(mL38)->GetBackgroundResource(inKey,outValue);
        return;
    }
    throw ConfigObjectException("Background resource not found: "+inKey);
}

// FUNCTION: WON 0x00785510
int ComponentConfig::Fn385510()
{
    mStringParser->SkipWhitespace();
    if(mStringParser->GetChar(0)=='@')
    {
        mStringParser->IncrementPos(1);
        std::string key;
        StringParser& aParser=*mStringParser;
        aParser.ReadString(key,true);
        return GetColorResource(key);
    }
    return ResourceConfig::SafeReadColor(*mStringParser);
}

// FUNCTION: WON 0x007855f0
SmartPtr<Sound> ComponentConfig::ReadSound()
{
    mStringParser->SkipWhitespace();
    if(mStringParser->GetChar(0)=='@')
    {
        mStringParser->IncrementPos(1);
        std::string key;
        StringParser& aParser=*mStringParser;
        aParser.ReadString(key,true);
        return GetSoundResource(key);
    }
    return ResourceConfig::SafeReadSound(*mStringParser);
}

// FUNCTION: WON 0x00785ea0
Background ComponentConfig::ReadBackground()
{
    if(mStringParser->GetChar(0)=='@')
    {
        mStringParser->IncrementPos(1);
        std::string key;
        StringParser* aParser=mStringParser;
        aParser->ReadString(key,true);
        Background value;
        GetBackgroundResource(key,value);
        if(value.mWantGrabBG) mComponent->SetComponentFlags(0x100,true);
        return value;
    }
    return ResourceConfig::SafeReadBackground(*mStringParser);
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a3180
// (public: __thiscall WONAPI::Background::~Background(void)    [??1Background@WONAPI@@QAE@XZ])
