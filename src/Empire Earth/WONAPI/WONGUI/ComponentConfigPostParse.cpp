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
// SUPPORT-ONLY: EXE 0x00786140 (10B actual SDK post-parse hook)
bool ComponentConfig::Virtual10(ConfigParser*)
{
    SetDesiredSizeIfNecessary();
    return true;
}
}
