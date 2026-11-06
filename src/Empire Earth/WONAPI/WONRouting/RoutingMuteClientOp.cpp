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
// FUNCTION: WON 0x006c78c0
RoutingMuteClientOp::RoutingMuteClientOp(RoutingConnection* inConnection)
    : RoutingOp(inConnection), mGroupId(0xffff), mIsMuted(true),
      mMuteTime(0), mMuteByWONId(true), mWONIdIsCDKeyId(false),
      mWONId(0), mClientId(0)
{
}
// FUNCTION: WON 0x006c7a10
void RoutingMuteClientOp::Virtual0C()
{
    InitSendMsg(46);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendBool(mIsMuted);
    mSendMsg.AppendLong(mMuteTime);
    mSendMsg.AppendWString(mMuteComment, 2);
    if (mMuteByWONId) {
        mSendMsg.AppendByte(mWONIdIsCDKeyId ? 3 : 2);
        mSendMsg.AppendLong(mWONId);
    } else {
        mSendMsg.AppendByte(1);
        mSendMsg.AppendShort(mClientId);
    }
    SendMsg();
    AddOp();
}
// Same physical generic reply2c8e90; separate method identity, zero additional primary credit.
WONStatus RoutingMuteClientOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native immediate type accessor; support only.
// FUNCTION: WON 0x006c7930
RoutingOpType RoutingMuteClientOp::GetType() const { return 38; }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c7940
// (public: virtual void * __thiscall WONAPI::RoutingMuteClientOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingMuteClientOp@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x006c7960
// (public: virtual __thiscall WONAPI::RoutingMuteClientOp::~RoutingMuteClientOp(void)    [??1RoutingMuteClientOp@WONAPI@@UAE@XZ])
