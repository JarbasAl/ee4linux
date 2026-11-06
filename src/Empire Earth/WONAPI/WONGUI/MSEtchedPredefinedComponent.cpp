// Native predefined child configuration dispatch; no runtime authority.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x0078f980
bool MSEtchedFrameGroupConfig::Virtual28(const std::string& name,ComponentConfig*& config,Component*& component)
{
    if(name=="LABEL") {
        config=LabelConfig::CfgFactory();
        component=mFrame->GetLabel();
    } else return ContainerConfig::Virtual28(name,config,component);
    return true;
}
}
