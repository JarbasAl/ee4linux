// Complete free component lookup/error support used by typed SafeGetComponent.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstdio>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x0078aa60
Component* ComponentConfigGet(ComponentConfig* config,const char* name)
{
    return config->Fn386860(std::string(name));
}

// FUNCTION: WON 0x0078ab30
void ComponentConfigMissing(const std::string& name,const char* file,int line)
{
    char message[512];
    sprintf(message,"InitComponent error: %s (%s:%d)",name.c_str(),file,line);
    throw ConfigObjectException(message);
}
}
