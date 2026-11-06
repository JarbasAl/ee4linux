// Native typed child layout-point config/map/manager insertion.
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
#include "ee/wonapi/WONLobbyConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00789a70
bool ContainerConfig::HandleNewLayoutPoint()
{
    while(true) {
        SmartPtr<ComponentConfig> config=new ComponentConfig;
        SmartPtr<LayoutPoint> point=new LayoutPoint;
        config->Virtual34(point.get());
        config->mL38=this;
        mConfigMap[Fn3842F0(false)]=config;
        mContainer->AddLayoutPoint(point.get());
        if(EndOfString()) break;
        EnsureComma();
    }
    return true;
}
}
