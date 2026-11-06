// Routing administrative request behavior; native complete operation records.
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
// FUNCTION: WON 0x006c4320
RoutingBanClientOp::RoutingBanClientOp(RoutingConnection* inConnection)
    : RoutingOp(inConnection), mGroupId(0xffff), mIsBanned(true),
      mBanTime(0), mBanByWONId(true), mWONIdIsCDKeyId(false),
      mWONId(0), mClientId(0)
{
}
// FUNCTION: WON 0x006c4470
void RoutingBanClientOp::Virtual0C()
{
    InitSendMsg(3);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendBool(mIsBanned);
    mSendMsg.AppendLong(mBanTime);
    mSendMsg.AppendWString(mBanComment, 2);
    if (mBanByWONId) {
        mSendMsg.AppendByte(mWONIdIsCDKeyId ? 3 : 2);
        mSendMsg.AppendLong(mWONId);
    } else {
        mSendMsg.AppendByte(1);
        mSendMsg.AppendShort(mClientId);
    }
    SendMsg();
    AddOp();
}
// FUNCTION: WON 0x006c8e90
WONStatus RoutingBanClientOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native immediate type accessor; support only.
// FUNCTION: WON 0x006c4390
RoutingOpType RoutingBanClientOp::GetType() const { return 3; }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c43a0
// (public: virtual void * __thiscall WONAPI::RoutingBanClientOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingBanClientOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006c43c0
// (public: virtual __thiscall WONAPI::RoutingBanClientOp::~RoutingBanClientOp(void)    [??1RoutingBanClientOp@WONAPI@@UAE@XZ])
