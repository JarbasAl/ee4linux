// Genuine ResourceConfig sound insertion on the complete native b0 owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
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
// FUNCTION: WON 0x007a2530
template<class Type,class Map>
static void ResourceConfig_AddToMap(Map& values,const std::string& key,Type& value,int generation)
{
    values[key]=Map::referent_type(value,generation);
}
// FUNCTION: WON 0x007934a0
void ResourceConfig::AddSound(const std::string& key,Sound* sound)
{
    ResourceConfig_AddToMap(mSoundMap,key,sound,mParseCount);
}

// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot SDK compatible resource API completion.
void ResourceConfig::AddString(const std::string &theKey, const GUIString &theStr)
{
	ResourceConfig_AddToMap(mStringMap,theKey,theStr,mParseCount);
}

void ResourceConfig::AddImage(const std::string &theKey, Image *theImage)
{
	ResourceConfig_AddToMap(mImageMap,theKey,theImage,mParseCount);
}

void ResourceConfig::AddFont(const std::string &theKey, Font *theFont)
{
	ResourceConfig_AddToMap(mFontMap,theKey,theFont,mParseCount);
}

void ResourceConfig::AddColor(const std::string &theKey, DWORD theColor)
{
	ResourceConfig_AddToMap(mColorMap,theKey,theColor,mParseCount);
}

void ResourceConfig::AddBackground(const std::string &theKey, const Background &theBackground)
{
	ResourceConfig_AddToMap(mBackgroundMap,theKey,theBackground,mParseCount);
}

void ResourceConfig::AddInt(const std::string &theKey, int theInt)
{
	ResourceConfig_AddToMap(mIntMap,theKey,theInt,mParseCount);
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007a39c0
// (public: __thiscall std::pair<class WONAPI::GUIString,int>::pair<class WONAPI::GUIString,int>(struct std::pair<class WONAPI::GUIString,int> const &)   )
