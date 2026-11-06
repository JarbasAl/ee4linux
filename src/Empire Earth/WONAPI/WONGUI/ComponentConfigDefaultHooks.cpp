// Actual SDK component default dispatch and borrowed component reader.
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
// FUNCTION: WON 0x007868c0
bool ComponentConfig::Virtual28(const std::string&,ComponentConfig*&,Component*&)
{
    return false;
}
// FUNCTION: WON 0x00786950
Component* ComponentConfig::ReadComponent()
{
    ComponentConfig* config=ReadComponentConfig();
    return config->mComponent.get();
}
}
