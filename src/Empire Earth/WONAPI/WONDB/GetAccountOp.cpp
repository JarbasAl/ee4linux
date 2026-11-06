// New GetAccount operation family. Scratch source intent; no publication/CL lease.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006ccef0
void GetAccountOp::Init()
{
    mLengthFieldSize=4;
    mProfileData=new ProfileData;
}

// FUNCTION: WON 0x006ccfa0
GetAccountOp::GetAccountOp(ServerContext* inContext,AuthContext* inAuth)
    : ServerRequestOp(inContext)
{
    mAuthType=AUTH_TYPE_SESSION;
    mAuthContext=inAuth;
    Init();
}

// FUNCTION: WON 0x006cd190
WONStatus GetAccountOp::Virtual10()
{
    WriteBuffer aRequest(mLengthFieldSize,true);
    aRequest.AppendByte(5);
    aRequest.AppendShort(14);
    aRequest.AppendShort(1002);
    aRequest.AppendShort(6);
    mProfileData->PackRequestData(aRequest,false);
    mRequest=aRequest.ToByteBuffer(true);
    return 100059;
}

// FUNCTION: WON 0x006cd2b0
WONStatus GetAccountOp::Virtual14()
{
    ReadBuffer aReply(mResponse->data(),mResponse->length());
    unsigned char aHeader=aReply.ReadByte();
    unsigned short aService=aReply.ReadShort();
    unsigned short aMessage=aReply.ReadShort();
    unsigned short aSubMessage=aReply.ReadShort();
    WONStatus aStatus=aReply.ReadShort();
    aReply.ReadString(mErrorString,2);
    aReply.ReadByte();
    if (aHeader!=5 || aService!=14)
        return InvalidReplyHeader();
    if (aStatus!=0)
        return aStatus;
    if (aMessage!=1002 || aSubMessage!=7)
        return InvalidReplyHeader();
    aReply.ReadString(mEmail,1);
    mProfileData->UnpackGetData(aReply);
    return 0;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: explicit empty SDK Reset override.
namespace WONAPI {
void GetAccountOp::Virtual18()
{
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cd0a0
// (public: virtual __thiscall WONAPI::GetAccountOp::~GetAccountOp(void)    [??1GetAccountOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cd080
// (public: virtual void * __thiscall WONAPI::GetAccountOp::`scalar deleting destructor'(unsigned int)    [??_GGetAccountOp@WONAPI@@UAEPAXI@Z])
