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
// FUNCTION: WON 0x0075e620
void LobbyFn35E620()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && data->mNetworkHandle!=WONAPI::IPAddr::GetLocalHost()) {
        data->mNetworkHandle=WONAPI::IPAddr::GetLocalHost();
        data->mGlobalDirty=true;
    }
}
// FUNCTION: WON 0x0075dea0
void LobbyFn35DEA0(const GUIString& column,const GUIString& value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) {
        data->mGameOptions[column].Add(value);
        data->mUserDirty=true;
    }
}
// FUNCTION: WON 0x0075ded0
void LobbyFn35DED0(const GUIString& column,const GUIString& value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data==NULL) return;
    LobbyOptionSet& options=data->mGameOptions[column];
    if(options.Erase(value)) data->mUserDirty=true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007670a0
// (public: class WONAPI::LobbyOptionSet & __thiscall std::map<class WONAPI::GUIString,class WONAPI::LobbyOptionSet,struct WONAPI::GUIStringLessNoCase,cla)
