// Native routing behavior; source-only reconstruction.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x6c7bc0
void RoutingOp::InitSendMsg(unsigned char inType)
{
    mSendMsg.Reset();
    mSendMsg.AppendByte(3);
    mSendMsg.AppendByte(mIsDerivedServerOp ? 8 : 7);
    mSendMsg.AppendByte(inType);
}

// FUNCTION: WON 0x6c7c00
void RoutingOp::SendMsg()
{
    mConnection->Fn2B4740(new SendMsgOp(mSendMsg.ToByteBuffer(true).get()),-1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006c7ab0
RoutingOp::RoutingOp() : mSendMsg(0,true)
{
    mIsDerivedServerOp=false;
}
// FUNCTION: WON 0x006c7b40
RoutingOp::RoutingOp(RoutingConnection* connection)
    :mSendMsg(connection->GetLengthFieldSize(),true),mConnection(connection)
{
    mIsDerivedServerOp=false;
}

// FUNCTION: WON 0x006c7d20
void RoutingOp::Virtual04()
{
    if(mRunMode==OP_MODE_BLOCK) {
        Fn29F220((WONStatus)100009);
        return;
    }
    Virtual0C();
}

void RoutingOp::Virtual0C() {}
// FUNCTION: WON 0x006bcd30
WONStatus RoutingOp::Virtual10(unsigned char,ReadBuffer&) { return (WONStatus)100104; }
// FUNCTION: WON 0x006c7b10
RoutingOpType RoutingOp::GetType() const { return 0; }
// FUNCTION: WON 0x006c7d10
void RoutingOp::AddOp() { mConnection->AddOp(this); }
// FUNCTION: WON 0x006c4640
RoutingClientInfoPtr RoutingOp::GetNewClientInfo() { return mConnection->Virtual1C(); }
// FUNCTION: WON 0x006c4660
RoutingMemberInfoPtr RoutingOp::GetNewMemberInfo() { return mConnection->Virtual20(); }
}

namespace WONAPI {
// Actual2c5bb0 forwarding helper30B, support0.
// FUNCTION: WON 0x006c5bb0
RoutingGroupInfoPtr RoutingOp::GetNewGroupInfo() { return mConnection->Virtual24(); }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c7b20
// (public: virtual void * __thiscall WONAPI::RoutingOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingOp@WONAPI@@UAEPAXI@Z])
