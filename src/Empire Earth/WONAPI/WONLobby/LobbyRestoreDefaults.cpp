// Restore user defaults and retain the original dirty-state calculation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
// Existing original support provider, defined in LobbyPersistentData.cpp at35c900.
extern LobbyPersistentData* GetLobbyPersistentData();
// FUNCTION: WON 0x0075e6a0
void LobbyFn35E6A0()
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if(aData != NULL) {
        int anOldAdapter = aData->mNetworkHandle;
        aData->Fn35C720();
        aData->mUserDirty = true;
        aData->mGlobalDirty = aData->mB6C != aData->mB5C ||
            aData->mB6D != aData->mB5D ||
            aData->mNetworkHandle != anOldAdapter;
    }
}
}
