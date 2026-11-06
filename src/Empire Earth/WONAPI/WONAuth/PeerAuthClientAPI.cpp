// Actual complete peer-authentication client protocol and original typed owners.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAuthPeerData.h"
namespace WONAPI {
// FUNCTION: WON 0x006e5c30
PeerAuthClient::PeerAuthClient()
{
    Reset();
    mUseAuth2=true;
    mUseClientSecret=false;
    mClientKeySize=8;
}
// FUNCTION: WON 0x006e5cb0
void PeerAuthClient::Reset()
{
    mState=STATE_NOT_STARTED;
}
}
