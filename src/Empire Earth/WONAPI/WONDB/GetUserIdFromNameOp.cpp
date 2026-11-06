// Native DB proxy user-name lookup operation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
extern unsigned short gEmptyWideString[]; // 0x8e60bc, an empty wide string (zero .bss; defined in EEWonNet.cpp)
namespace WONAPI {
// FUNCTION: WON 0x006cd3a0
GetUserIdFromNameOp::GetUserIdFromNameOp(ServerContext* inContext)
    : DBProxyOp(inContext),mUserName(gEmptyWideString),mUserId(0)
{
}
// FUNCTION: WON 0x006cd4d0
GetUserIdFromNameOp::~GetUserIdFromNameOp()
{
}
// FUNCTION: WON 0x006cd600
void GetUserIdFromNameOp::Virtual04()
{
    mMessageType=1002;
    mSubMessageType=16;
    WriteBuffer request(0,true);
    request.AppendWString(mUserName,2);
    mRequestData=request.ToByteBuffer(true);
    ServerRequestOp::Virtual04();
}
// FUNCTION: WON 0x006cd700
WONStatus GetUserIdFromNameOp::Virtual14()
{
    WONStatus result=DBProxyOp::Virtual14();
    if (result!=0) return result;
    if (mSubMessageReplyType!=17) return InvalidReplyHeader();
    ReadBuffer reply(mReplyData->data(),mReplyData->length());
    mUserId=reply.ReadLong();
    return 0;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cd4b0
// (protected: virtual void * __thiscall WONAPI::GetUserIdFromNameOp::`scalar deleting destructor'(unsigned int)    [??_GGetUserIdFromNameOp@WONAPI@@MAEPA)
