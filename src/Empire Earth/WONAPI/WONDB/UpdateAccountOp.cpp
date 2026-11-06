// New UpdateAccount operation family. Scratch source intent; no publication/CL lease.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006ce840
void UpdateAccountOp::Init()
{
    mLengthFieldSize=4;
    mProfileData=new ProfileData;
}

// FUNCTION: WON 0x006ce8f0
UpdateAccountOp::UpdateAccountOp(ServerContext* inContext,AuthContext* inAuth)
    : ServerRequestOp(inContext)
{
    mAuthType=AUTH_TYPE_SESSION;
    mAuthContext=inAuth;
    Init();
}

// FUNCTION: WON 0x006ceb30
WONStatus UpdateAccountOp::Virtual10()
{
    WriteBuffer aRequest(mLengthFieldSize,true);
    aRequest.AppendByte(5);
    aRequest.AppendShort(14);
    aRequest.AppendShort(1002);
    aRequest.AppendShort(4);
    aRequest.AppendWString(mNewPassword,2);
    aRequest.AppendString(mEmail,2);
    mProfileData->PackRequestData(aRequest,true);
    mRequest=aRequest.ToByteBuffer(true);
    return 100059;
}

// FUNCTION: WON 0x006cec80
WONStatus UpdateAccountOp::Virtual14()
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
    if (aMessage!=1002 || aSubMessage!=5)
        return InvalidReplyHeader();
    return 0;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: explicit empty SDK Reset override.
namespace WONAPI {
void UpdateAccountOp::Virtual18()
{
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cea10
// (public: virtual __thiscall WONAPI::UpdateAccountOp::~UpdateAccountOp(void)    [??1UpdateAccountOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ce9f0
// (public: virtual void * __thiscall WONAPI::UpdateAccountOp::`scalar deleting destructor'(unsigned int)    [??_GUpdateAccountOp@WONAPI@@UAEPAXI@Z])
