// Native AoC GetDirOp protocol behavior; legacy header migration pending Root.
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
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
// FUNCTION: WON 0x006f1190
void GetDirOp::Init()
{
    mLengthFieldSize=4;
    mGetEntityRequest=new GetEntityRequest;
}
// FUNCTION: WON 0x006f12d0
GetDirOp::GetDirOp(ServerContext* context) : ServerRequestOp(context)
{
    Init();
}
// FUNCTION: WON 0x006f13c0
void GetDirOp::Virtual18()
{
    mGetEntityRequest->Reset();
}
// FUNCTION: WON 0x006f13d0
WONStatus GetDirOp::Virtual10()
{
    WriteBuffer message(mLengthFieldSize,true);
    message.AppendByte(5);
    message.AppendShort(2);
    message.AppendShort(103);
    mGetEntityRequest->Pack(&message,GetEntityRequest::Pack_GetDirOp);
    mRequest=message.ToByteBuffer(true);
    return (WONStatus)100059;
}
// FUNCTION: WON 0x006f14e0
WONStatus GetDirOp::Virtual14()
{
    WONStatus status=mGetEntityRequest->ParseMultiEntityReply(mResponse->data(),mResponse->length());
    if(status==(WONStatus)100063) return InvalidReplyHeader();
    return status;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006f1330
// (public: virtual void * __thiscall WONAPI::GetDirOp::`scalar deleting destructor'(unsigned int)    [??_GGetDirOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006f1350
// (public: virtual __thiscall WONAPI::GetDirOp::~GetDirOp(void)    [??1GetDirOp@WONAPI@@UAE@XZ])
