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
// FUNCTION: WON 0x007989f0
bool WONScrollContainerConfig::Virtual28(const std::string& name,ComponentConfig*& config,Component*& component)
{
    if(name=="HORZSCROLLBAR") {
        config=new WONScrollbarConfig;
        component=mScrollContainer->mHorzScrollbar;
    } else if(name=="VERTSCROLLBAR") {
        config=new WONScrollbarConfig;
        component=mScrollContainer->mVertScrollbar;
    } else return ScrollbarScrollerConfig::Virtual28(name,config,component);
    return true;
}
}
