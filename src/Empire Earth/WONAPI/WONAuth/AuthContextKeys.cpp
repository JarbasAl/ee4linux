// Actual AuthContext community key mutation, existing complete mapped owners.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006e0380
void AuthContext::SetCDKey(const std::wstring& community,const CDKey& key)
{
    AutoCrit lock(mDataCrit);
    mCommunityMap[community].mCDKey=key;
}
}
