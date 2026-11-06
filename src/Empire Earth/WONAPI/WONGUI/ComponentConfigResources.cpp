// Native resource-group parsing and selection; genuine ResourceConfig/Table contracts installed by Root H.
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
#include "ee/wonapi/WONLobbyParser.h"
namespace WONAPI {
// FUNCTION: WON 0x00786f20
bool ComponentConfig::HandleResourceGroup(ConfigParser& parser)
{
    std::string name,inheritName;
    if(!ReadGroupStart(parser,name,inheritName)) return true;
    bool alreadyExists=false;
    SmartPtr<ResourceConfig> resource=mResourceConfigTable->AddConfig(name,&alreadyExists);
    resource->SetAllowOverwrite(alreadyExists);
    return ParseChild(parser,resource.get(),false,false);
}
// FUNCTION: WON 0x00787250
bool ComponentConfig::Virtual08(ConfigParser* parser,const std::string& key,StringParser& value)
{
    if(ConfigObject::Virtual08(parser,key,value)) return true;
    if(key=="SETRESOURCE") {
        std::string name;
        value.ReadString(name,false);
        mCurResourceConfig=mResourceConfigTable->SafeGetConfig(name);
        return true;
    }
    return BaseHandleInstruction(*parser,key,value);
}
}
