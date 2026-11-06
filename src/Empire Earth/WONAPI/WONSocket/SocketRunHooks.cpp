// Actual socket operation start/async/blocking dispatch; no new owner or helper.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAPICore.h"
namespace WONAPI {
// FUNCTION: WON 0x006b6070
void SocketOp::Virtual04()
{
    if(mSocket.get()==NULL)
        Fn29F220(100022); //actual invalid-socket status; native continues to Start
    WONStatus status=Virtual0C();
    if(status!=100001) //actual WS_TimedOut signals pending socket work
    {
        Fn29F220(status);
        return;
    }
    if(mRunMode==1)
        WONAPICore::AddToSocketThread(this);
    else
    {
        status=Virtual10();
        Fn29F220(status);
    }
}
}

// Genuine SDK SocketOp.cpp static definition; actual native565f08 logical zero byte.
namespace WONAPI {
bool SocketOp::mRunAsyncImmediatelyDef;
}
