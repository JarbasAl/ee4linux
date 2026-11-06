// Native adapter selection: preserve persistent state only after successful host switch.
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
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
// FUNCTION: WON 0x0075b9c0
void LobbyFn35B9C0(long address)
{
    bool changed=false;
    if(WONAPI::IPAddr::GetLocalHost()!=address) {
        changed=true;
        if(!IPAddr::SetLocalHost(address,true)) return;
    }
    if(WONAPI::IPAddr::GetLocalHost()!=LobbyFn35E3E0()) LobbyFn35E620();
    if(changed) LobbyFn35BA00();
}
}
