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
// SUPPORT-ONLY: EXE 0x0078f960 (shared25B actual base setter; zero primary/global-new native credit)
void ScrollerConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mScroller=(Scroller*)component;
}
// SUPPORT-ONLY: EXE 0x0078f860 (shared25B actual base setter; zero primary/global-new native credit)
void ScrollbarScrollerConfig::Virtual34(Component* component)
{
    ScrollerConfig::Virtual34(component);
    mScrollbarScroller=(ScrollbarScroller*)component;
}
// SUPPORT-ONLY: EXE 0x0078f960 (shared25B actual TabCtrl setter; zero primary/global-new native credit)
void TabCtrlConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mTabCtrl=(TabCtrl*)component;
}
}
