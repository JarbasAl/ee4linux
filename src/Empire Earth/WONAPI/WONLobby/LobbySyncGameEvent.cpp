// Complete native-backed missing lobby support. No primary quota.
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
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyModel.h"
namespace WONAPI {
// FUNCTION: WON 0x0075b0f0
SyncGameListEvent::SyncGameListEvent(LobbyGameList* games)
    : LobbyEvent(100057),mGames(games) {}
// FUNCTION: WON 0x0075b160
SyncGameListEvent::~SyncGameListEvent() {}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0075b140
// (public: virtual void * __thiscall WONAPI::SyncGameListEvent::`scalar deleting destructor'(unsigned int)    [??_GSyncGameListEvent@WONAPI@@UAEPAXI@Z])
