// Original-backed routing group-list server request, complete f8 owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006c5ff0
RoutingGetGroupListReqOp::RoutingGetGroupListReqOp(const IPAddr& inAddress,unsigned char inFlags)
 : ServerRequestOp(inAddress),mOp(new RoutingGetGroupListOp)
{
    mLengthFieldSize=2;
    mOp->mFlags=inFlags;
}
// FUNCTION: WON 0x006c6150
WONStatus RoutingGetGroupListReqOp::Virtual10()
{
    WriteBuffer aRequest(mLengthFieldSize,true);
    aRequest.AppendByte(3);
    aRequest.AppendByte(7);
    aRequest.AppendByte(26);
    aRequest.AppendShort((unsigned char)mOp->mFlags);
    mRequest=aRequest.ToByteBuffer(true);
    return 100059;
}
// FUNCTION: WON 0x006c6270
WONStatus RoutingGetGroupListReqOp::Virtual14()
{
    ReadBuffer aMessage(mResponse->data(),mResponse->length());
    unsigned char aHeader=aMessage.ReadByte();
    unsigned char aService=aMessage.ReadByte();
    unsigned char aType=aMessage.ReadByte();
    if(aHeader!=3 || aService!=7 || aType!=25)
        return InvalidReplyHeader();
    WONStatus aStatus=aMessage.ReadShort();
    if(aStatus!=0)
        return aStatus;
    if(!mOp->ParseGroupListObj(aMessage.mData+aMessage.mL00,aMessage.mL08-aMessage.mL00))
        return 100012;
    return 0;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c60c0
// (public: virtual void * __thiscall WONAPI::RoutingGetGroupListReqOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGetGroupListReqOp@WONAPI)
// TEMPLATE: WON 0x006c60e0
// (public: virtual __thiscall WONAPI::RoutingGetGroupListReqOp::~RoutingGetGroupListReqOp(void)    [??1RoutingGetGroupListReqOp@WONAPI@@UAE@XZ])
