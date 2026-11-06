// Genuine ResourceConfig string lookup on the complete native b0 owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;
// FUNCTION: WON 0x007a2680
template<class Map>
static Map::referent_type::first_type& ResourceConfig_SafeGetRes(
    const char* typeName,const std::string& key,Map& values,ResourceConfig* config)
{
    Map::iterator found=values.find(key);
    if(found!=values.end()) return found->second.first;
    char text[512];
    // Established ConfigObject::mS18 is SDK mName/GetName(); native reads its data at +1c.
    sprintf(text,"%s resource not found: %s (ResourceGroup: %s)",
        typeName,key.c_str(),config->mS18.c_str());
    throw ConfigObjectException(text);
}
// FUNCTION: WON 0x00793890
const GUIString& ResourceConfig::SafeGetString(const std::string& key)
{
    return ResourceConfig_SafeGetRes("String",key,mStringMap,this);
}

// FUNCTION: WON 0x007938b0
Image* ResourceConfig::SafeGetImage(const std::string& key)
{
    return ResourceConfig_SafeGetRes("Image",key,mImageMap,this).get();
}
// FUNCTION: WON 0x007938d0
Sound* ResourceConfig::SafeGetSound(const std::string& key)
{
    return ResourceConfig_SafeGetRes("Sound",key,mSoundMap,this).get();
}

// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot SDK compatible resource API completion.
Font* ResourceConfig::SafeGetFont(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Font",theKey,mFontMap,this);
}

DWORD ResourceConfig::SafeGetColor(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Color",theKey,mColorMap,this);

}

const Background& ResourceConfig::SafeGetBackground(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Background",theKey,mBackgroundMap,this);

}

int ResourceConfig::SafeGetInt(const std::string &theKey)
{
	return ResourceConfig_SafeGetRes("Int",theKey,mIntMap,this);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a27f0
// (class WONAPI::SmartPtr<class WONAPI::Image> & __cdecl ResourceConfig_SafeGetRes(char const *,class std::basic_string<char,struct std::char_traits<char)
// TEMPLATE: WON 0x007a2960
// (class WONAPI::SmartPtr<class WONAPI::Sound> & __cdecl ResourceConfig_SafeGetRes(char const *,class std::basic_string<char,struct std::char_traits<char)
