// Native TabCtrl instruction and predefined-control configuration handlers.
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
#include "ee/wonapi/WONLobbyTabCtrlConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00795740
bool TabCtrlConfig::Virtual24(const std::string& key)
{
    if(key=="ADDTAB") {
        Button* button=SafeConfigGetComponent<Button*>(ReadComponent(),"Button");
        EnsureComma();
        Container* container=SafeConfigGetComponent<Container*>(ReadComponent(),"Container");
        mTabCtrl->AddTab(button,container);
    } else if(key=="TABBARONRIGHT") {
        mTabCtrl->SetTabBarOnRight(Fn3837F0());
    } else if(key=="ARROWSONLEFT") {
        mTabCtrl->SetArrowsOnLeft(Fn3837F0());
    } else if(key=="TABBARHEIGHT") {
        int height=ReadInt(); mTabCtrl->mTabBarHeight=height;
    } else if(key=="TABOVERLAP") {
        int overlap=ReadInt(); mTabCtrl->mBottomTabBar->mOverlap=overlap;
    } else if(key=="TABBAROVERLAP") {
        int overlap=ReadInt(); mTabCtrl->mTabBarOverlap=overlap;
    } else if(key=="TABVERTOFFSET") {
        int offset=ReadInt(); mTabCtrl->mBottomTabTopOffset=offset;
    } else if(key=="TABHORZOFFSET") {
        int offset=ReadInt(); mTabCtrl->mBottomTabLeftOffset=offset;
    } else return ContainerConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x007958e0
bool TabCtrlConfig::Virtual28(const std::string& name,ComponentConfig*& config,Component*& component)
{
    if(name=="CONTAINERLAYOUT") {
        config=new ComponentConfig;
        component=mTabCtrl->mContainerLayout;
    } else return ContainerConfig::Virtual28(name,config,component);
    return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00795720
TabCtrlConfig::TabCtrlConfig()
{
}
}
