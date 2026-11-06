// Complete original socket operation dispatch and guarded state APIs.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006b2320
WONStatus ConnectOp::Virtual0C() { return mSocket->Fn2B0E60(mAddr); }
// FUNCTION: WON 0x006b2330
WONStatus ConnectOp::Virtual10()
{
    AsyncSocket* socket=mSocket.get();
    return socket->Fn2B1220(TimeLeft());
}
// FUNCTION: WON 0x006b4ee0
WONStatus RecvBytesOp::Virtual0C() { return Fn2B5000(); }
// FUNCTION: WON 0x006b4ef0
WONStatus RecvBytesOp::Virtual10() { return Fn2B50C0(); }
}

#include "ee/wonapi/WONAPICore.h"
namespace WONAPI {
// FUNCTION: WON 0x006b0310
WONStatus AcceptOp::Virtual10()
{
    mSocket->WaitForAccept(TimeLeft());
    return mSocket->Fn2B1060(mAcceptedSocket);
}
// FUNCTION: WON 0x006b60c0
void SocketOp::Virtual08()
{
    if(mB44)
        WONAPICore::RemoveFromSocketThread(this);
}
}
