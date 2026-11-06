// Complete one-time authentication data cleanup, native-backed SDK methods.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006e0510
void AuthContext::ClearCDKeyCommunityJoins()
{
    AutoCrit lock(mDataCrit);
    mCDKeyCommunityJoinMap.clear();
}
// FUNCTION: WON 0x006e05e0
void AuthContext::ClearUserData()
{
    AutoCrit lock(mDataCrit);
    mSetCommunityUserDataMap.clear();
}
// FUNCTION: WON 0x006e06b0
void AuthContext::ClearOneTimeData()
{
    ClearUserData();
    ClearCDKeyCommunityJoins();
}
}
