// Native lobby address selection and complete original state adapters.
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
namespace WONAPI {
// State reset361440/403 includes expanded ordinaryIPAddr copy; support0.
// FUNCTION: WON 0x00761440
void LobbyFn361440()
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mFirewallTestDone=false;
    gLobbyData->mPublicAddr=IPAddr();
}
// Public address setter3615e0/391, original-owned address copy; support0.
// FUNCTION: WON 0x007615e0
void LobbyFn3615E0(const IPAddr& address)
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mPublicAddr=address;
}
// Coupled boolean setter361770/33; support0.
// FUNCTION: WON 0x00761770
void LobbyFn361770(bool behindFirewall)
{
    if(gLobbyData.get()==NULL) return;
    gLobbyData->mBehindFirewall=behindFirewall;
    gLobbyData->mFirewallTestDone=true;
}
// FUNCTION: WON 0x007617a0
IPAddr LobbyFn3617A0(int roomSpec)
{
    IPAddr address;
    if(gLobbyData.get()!=NULL) {
        if(roomSpec!=1 && gLobbyData->mPublicAddr.IsValid() && !gLobbyData->mConfig->mUseLocalIP)
            address=gLobbyData->mPublicAddr;
        else
            address=IPAddr::GetLocalAddr();
        address.SetPort(gLobbyData->mLobbyPort);
    }
    return address;
}
// FUNCTION: WON 0x00761990
IPAddr LobbyFn361990()
{
    if(gLobbyData.get()!=NULL) return gLobbyData->mPublicAddr;
    return IPAddr();
}
}

