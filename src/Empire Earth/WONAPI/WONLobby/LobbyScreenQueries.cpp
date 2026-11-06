// Complete native singleton orchestration; genuine external lifetime providers.
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
#include "ee/wonapi/WONLobbyAccountLogic.h"
#include "ee/wonapi/WONLobbyDialogLogic.h"
#include "ee/wonapi/WONLobbyLan.h"
#include "ee/wonapi/WONLobbyRoutingLogic.h"
#include "ee/wonapi/WONLobbyStaging.h"
#include "ee/wonapi/WONLobbyPing.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONCRC16.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {
// FUNCTION: WON 0x007610b0
void LobbyFn3610B0() { if(gLobbyData.get()!=NULL) gLobbyData->mLobbyContainer->mInternetScreen->ShowLobbyTab(); }
// FUNCTION: WON 0x007610d0
void LobbyFn3610D0() { if(gLobbyData.get()!=NULL) gLobbyData->mLobbyContainer->mLoginScreen->ShowCreateAccountCtrl(); }
// FUNCTION: WON 0x007610f0
void LobbyFn3610F0() { if(gLobbyData.get()!=NULL) gLobbyData->mLobbyContainer->mLoginScreen->ShowLostPasswordCtrl(); }
// FUNCTION: WON 0x00761110
void LobbyFn361110() { if(gLobbyData.get()!=NULL) gLobbyData->mLobbyContainer->mLoginScreen->ShowLostUsernameCtrl(); }
// FUNCTION: WON 0x00761130
bool LobbyFn361130() { return gLobbyData.get()!=NULL?gLobbyData->mLobbyContainer->OnInternetScreen():false; }
// FUNCTION: WON 0x00761150
bool LobbyFn361150() { return gLobbyData.get()!=NULL?gLobbyData->mLobbyContainer->OnLanScreen():false; }
// FUNCTION: WON 0x00761170
bool LobbyFn361170() { return gLobbyData.get()!=NULL?gLobbyData->mLobbyContainer->OnLoginScreen():false; }
// FUNCTION: WON 0x00761190
int LobbyFn361190()
{
    if(gLobbyData.get()==NULL) return 0;
    return gLobbyData->mLobbyContainer->GetGameScreenType();
}
// FUNCTION: WON 0x00761210
ColorScheme* LobbyFn361210() { return gLobbyData.get()!=NULL?gLobbyData->mLobbyContainer->GetColorScheme():NULL; }
// FUNCTION: WON 0x00761230
Window* LobbyFn361230() { return gLobbyData.get()!=NULL?gLobbyData->mLobbyContainer->GetRoot():NULL; }
// FUNCTION: WON 0x00761250
WindowManager* GetLobbyWindow() { return WindowManager::GetDefaultWindowManager(); }
// FUNCTION: WON 0x007628b0
bool LobbyFn3628B0()
{
    if(gLobbyData.get()==NULL) return false;
    return gLobbyData->mSuspended;
}
}
