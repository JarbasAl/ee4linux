
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
// FUNCTION: WON 0x0075b1f0
ServerContext* LobbyFn35B1F0() { return gLobbyData.get()!=NULL?gLobbyData->mAccountServers:SmartPtr<ServerContext>(); }
// FUNCTION: WON 0x0075b250
ServerContext* LobbyFn35B250() { return gLobbyData.get()!=NULL?gLobbyData->mAuthServers:SmartPtr<ServerContext>(); }
// FUNCTION: WON 0x0075b2b0
ServerContext* LobbyFn35B2B0() { return gLobbyData.get()!=NULL?gLobbyData->mDirServers:SmartPtr<ServerContext>(); }
// FUNCTION: WON 0x0075b310
ServerContext* LobbyFn35B310() { return gLobbyData.get()!=NULL?gLobbyData->mRoomDirServers:SmartPtr<ServerContext>(); }
// FUNCTION: WON 0x0075b370
ServerContext* LobbyFn35B370() { return gLobbyData.get()!=NULL?gLobbyData->mFirewallDetectServers:SmartPtr<ServerContext>(); }
// FUNCTION: WON 0x0075b3d0
ServerContext* LobbyFn35B3D0() { return gLobbyData.get()!=NULL?gLobbyData->mPatchServers:SmartPtr<ServerContext>(); }
// FUNCTION: WON 0x0075b430
void LobbyFn35B430() { if(gLobbyData.get()!=NULL) gLobbyData->mRoomDirServers=gLobbyData->mDirServers; }
// FUNCTION: WON 0x0075b500
void LobbyFn35B500(WONStatus status) { if(gLobbyData.get()!=NULL) gLobbyData->mVersionStatus=status; }
// FUNCTION: WON 0x0075b520
WONStatus LobbyFn35B520() { return gLobbyData.get()!=NULL?gLobbyData->mVersionStatus:100004; }
// FUNCTION: WON 0x0075b5d0
void LobbyFn35B5D0(long systemTime,long gameTime)
{
    if(gLobbyData.get()!=NULL) {
        gLobbyData->mSysTOUTime=systemTime;
        gLobbyData->mGameTOUTime=gameTime;
    }
}
// FUNCTION: WON 0x0075b670
bool LobbyFn35B670(long systemTime,long gameTime)
{
    if(gLobbyData.get()!=NULL)
        return gLobbyData->mSysTOUTime>systemTime || gLobbyData->mGameTOUTime>gameTime;
    return false;
}
// FUNCTION: WON 0x00761400
unsigned short LobbyFn361400() { return gLobbyData.get()!=NULL?gLobbyData->mLobbyPort:0; }
}
