// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: missing SDK disconnect request, proposed routing family profile.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRoutingDisconnectClientOp.h"
namespace WONAPI {
void RoutingDisconnectClientOp::Virtual0C()
{
	InitSendMsg(20);
	SendMsg();
}
}
