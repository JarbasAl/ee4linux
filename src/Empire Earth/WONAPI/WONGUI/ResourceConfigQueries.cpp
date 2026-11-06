
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
namespace WONAPI {
// FUNCTION: WON 0x00793340
bool ResourceConfig::Virtual0C(ConfigParser*)
{
    ++mParseCount;
    return true;
}
// FUNCTION: WON 0x007934c0
bool ResourceConfig::GetString(const std::string& name,GUIString& output)
{
    ResourceConfig::StringMap& aMap=mStringMap;
    ResourceConfig::StringMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first;
        return true;
    }
    return false;
}
// FUNCTION: WON 0x00793570
bool ResourceConfig::GetImage(const std::string& name,Image*& output)
{
    ResourceConfig::ImageMap& aMap=mImageMap;
    ResourceConfig::ImageMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first.get();
        return true;
    }
    return false;
}
// FUNCTION: WON 0x007935d0
bool ResourceConfig::GetFont(const std::string& name,Font*& output)
{
    ResourceConfig::FontMap& aMap=mFontMap;
    ResourceConfig::FontMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first.get();
        return true;
    }
    return false;
}
// FUNCTION: WON 0x00793630
bool ResourceConfig::GetSound(const std::string& name,Sound*& output)
{
    ResourceConfig::SoundMap& aMap=mSoundMap;
    ResourceConfig::SoundMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first.get();
        return true;
    }
    return false;
}
// FUNCTION: WON 0x007936a0
bool ResourceConfig::GetColor(const std::string& name,DWORD& output)
{
    ResourceConfig::ColorMap& aMap=mColorMap;
    ResourceConfig::ColorMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first;
        return true;
    }
    return false;
}
// FUNCTION: WON 0x00793710
bool ResourceConfig::GetBackground(const std::string& name,Background& output)
{
    ResourceConfig::BackgroundMap& aMap=mBackgroundMap;
    ResourceConfig::BackgroundMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first;
        return true;
    }
    return false;
}
// FUNCTION: WON 0x00793820
bool ResourceConfig::GetInt(const std::string& name,int& output)
{
    ResourceConfig::IntMap& aMap=mIntMap;
    ResourceConfig::IntMap::iterator i=aMap.find(name);
    if(i!=aMap.end())
    {
        output=i->second.first;
        return true;
    }
    return false;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a4c00
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic)
