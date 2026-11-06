// Genuine namespace shutdown; original globals retain their sole providers.
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
extern SmartPtr<Dialog> gLobbyFailureDialog5662F4;
extern GUIString gLobbyInitError5662FC;
extern bool gLobbyInitFailed56633C;

// FUNCTION: WON 0x00762170
void LobbyFn362170()
{
    gLobbyFailureDialog5662F4=NULL;
    gLobbyInitFailed56633C=false;
    gLobbyInitError5662FC=GetEmptyGUIStringData();
    if(gLobbyData.get()==NULL)
        return;
    LobbyFn361330(false);
    gLobbyData->Destroy();
    LobbyFn359040();
    ::LobbyGlobal_LobbyMusic_Sound->Stop();
    LobbyResource_UnloadResources();
    gLobbyData=NULL;
}
}
