// Native ResourceConfig resource-key dispatch on the genuine borrowed ConfigObject prefix.
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
// FUNCTION: WON 0x00793350
bool ResourceConfig::Virtual08(ConfigParser* parser,const std::string& key,StringParser& value)
{
    if(key=="STRING" || key=="CODESTRING") return HandleString(value);
    else if(key=="IMAGE" || key=="CODEIMAGE") return HandleImage(value);
    else if(key=="SOUND" || key=="CODESOUND") return HandleSound(value);
    else if(key=="FONT") return HandleFont(value);
    else if(key=="COLOR") return HandleColor(value);
    else if(key=="BACKGROUND") return HandleBackground(value);
    else if(key=="CONTROLID") return true;
    else return ConfigObject::Virtual08(parser,key,value);
}
}
