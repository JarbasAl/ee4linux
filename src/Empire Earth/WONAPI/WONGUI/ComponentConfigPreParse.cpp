// Component configuration inherited-group preparation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"
#include "ee/wonapi/WONLobbyWONInputBox.h"
#include "ee/wonapi/WONLobbySkinListControls.h"
namespace WONAPI {
// FUNCTION: WON 0x00786070
bool ComponentConfig::Virtual0C(ConfigParser* parser)
{
    if(!ConfigObject::Virtual0C(parser))
        return false;
    if(!mS28.empty())
        return IncludeGroup(*parser,mS28);
    return true;
}
}
