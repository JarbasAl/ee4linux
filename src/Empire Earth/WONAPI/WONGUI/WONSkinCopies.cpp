// Named skin controls: case-insensitive lookup and conditional attribute copy.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"

using namespace WONAPI;

// FUNCTION: WON 0x00715c90
bool WONSkin::CopyButtonType(const GUIString& type,WONButton* copyTo)
{
    ButtonTypeMap::iterator it=mButtonTypeMap.find(type);
    if(it==mButtonTypeMap.end())
        return false;
    copyTo->CopyAttributes(it->second.get());
    return true;
}

// FUNCTION: WON 0x00715d00
bool WONSkin::CopySimpleButtonType(const GUIString& type,WONSimpleButton* copyTo)
{
    SimpleButtonTypeMap::iterator it=mSimpleButtonTypeMap.find(type);
    if(it==mSimpleButtonTypeMap.end())
        return false;
    copyTo->CopyAttributes(it->second.get());
    return true;
}
