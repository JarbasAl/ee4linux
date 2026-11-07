// New native email account-request family; exact grouping provisional.
// Source-only reconstruction; full144 records/parents/Profile20 are canonical; runtime substitution remains unapproved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006ce090
RequestAccountInfoEmailOp::RequestAccountInfoEmailOp(ServerContext* inContext)
    : DBProxyOp(inContext),mMsgType(14)
{
    Init();
}
// FUNCTION: WON 0x006ce130
RequestAccountInfoEmailOp::~RequestAccountInfoEmailOp() {}
// FUNCTION: WON 0x006ce280
WONStatus RequestAccountInfoEmailOp::Virtual14()
{
    WONStatus status=DBProxyOp::Virtual14();
    if (status!=0) return status;
    if (mSubMessageReplyType!=mMsgType+1) return InvalidReplyHeader();
    return 0;
}
// FUNCTION: WON 0x006ce2b0
void RequestAccountInfoEmailOp::Virtual04()
{
    mMessageType=1002;
    mSubMessageType=mMsgType;
    WriteBuffer msg(0,true);
    const std::string& email=mEmail;
    msg.AppendString(email,2);
    mProfileData->PackRequestData(msg,true);
    mRequestData=msg.ToByteBuffer(true);
    ServerRequestOp::Virtual04();
}
// FUNCTION: WON 0x006ce3d0
void RequestAccountInfoEmailOp::Init()
{
    mLengthFieldSize=4;
    mProfileData=new ProfileData;
}
// FUNCTION: WON 0x006ce480
RequestPasswordEmailOp::RequestPasswordEmailOp(ServerContext* inContext)
    : DBProxyOp(inContext),mMsgType(8)
{
    Init();
}
// FUNCTION: WON 0x006ce520
RequestPasswordEmailOp::~RequestPasswordEmailOp() {}
// Shared actual physical44B response at2ce280. This second definition has zero

WONStatus RequestPasswordEmailOp::Virtual14()
{
    WONStatus status=DBProxyOp::Virtual14();
    if (status!=0) return status;
    if (mSubMessageReplyType!=mMsgType+1) return InvalidReplyHeader();
    return 0;
}
// FUNCTION: WON 0x006ce670
void RequestPasswordEmailOp::Virtual04()
{
    mMessageType=1002;
    mSubMessageType=mMsgType;
    WriteBuffer msg(0,true);
    const std::wstring& userName=mUserName;
    msg.AppendWString(userName,2);
    mProfileData->PackRequestData(msg,true);
    mRequestData=msg.ToByteBuffer(true);
    ServerRequestOp::Virtual04();
}
// FUNCTION: WON 0x006ce790
void RequestPasswordEmailOp::Init()
{
    mLengthFieldSize=4;
    mProfileData=new ProfileData;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ce110
// (protected: virtual void * __thiscall WONAPI::RequestAccountInfoEmailOp::`scalar deleting destructor'(unsigned int)    [??_GRequestAccountInfoEmailOp@W)
// TEMPLATE: WON 0x006ce500
// (protected: virtual void * __thiscall WONAPI::RequestPasswordEmailOp::`scalar deleting destructor'(unsigned int)    [??_GRequestPasswordEmailOp@WONAPI@)
