// Original-backed protocol family; pending first ordinary full-COFF review.
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

// FUNCTION: WON 0x006c4a80
RoutingCreateGroupOp::RoutingCreateGroupOp(RoutingConnection* inConnection)
    : RoutingOp(inConnection)
{
    mGroupId = 0xffff;
    mParentGroupId = 0xffff;
    mMaxPlayers = 0;
    mJoinFlags = 0;
    mGroupFlags = 0;
    mAsyncFlags = 0;
    mLeaveGroupOnCancel = false;
}

// FUNCTION: WON 0x006c4c20
void RoutingCreateGroupOp::Virtual0C()
{
    InitSendMsg(13);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendWString(mGroupName, 2);
    mSendMsg.AppendWString(mGroupPassword, 2);
    mSendMsg.AppendShort(mMaxPlayers);
    mSendMsg.AppendByte(mJoinFlags);
    mSendMsg.AppendLong(mGroupFlags);
    mSendMsg.AppendLong(mAsyncFlags);
    mSendMsg.AppendShort(mParentGroupId);
    SendMsg();
    AddOp();
}

// FUNCTION: WON 0x006c4d90
WONStatus RoutingCreateGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType == 41) {
        if (mJoinFlags == 0)
            return (WONStatus)100104;
        short status = inMsg.ReadShort();
        CheckLeaveGroup(status);
        return status;
    }
    if (inMsgType != 12)
        return (WONStatus)100104;
    WONStatus status = inMsg.ReadShort();
    if (status == 0) {
        mGroupId = inMsg.ReadShort();
        inMsg.ReadWString(mGroupName, 2);
        CheckLeaveGroup((short)status);
        if (mJoinFlags != 0)
            return (WONStatus)100105;
    }
    // Target2c4e24 compares the reader positions unsigned. Current canonical
    // member spellings are signed; retain the actual bounds comparison explicitly.
    if ((unsigned long)inMsg.mL00 < (unsigned long)inMsg.mL08)
        mParentGroupId = inMsg.ReadShort();
    return status;
}

// Whole-family deferred body, nativeLeaveGroupOp68 constructor/fulltype gate.
// FUNCTION: WON 0x006c4cb0
void RoutingCreateGroupOp::CheckLeaveGroup(short inStatus)
{
    if (inStatus == 0 && mStatus != (WONStatus)100000 && mLeaveGroupOnCancel) {
        mLeaveGroupOnCancel = false;
        SmartPtr<RoutingLeaveGroupOp> op = new RoutingLeaveGroupOp(mConnection.get(),mGroupId);
        op->Run(-1);
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c4b40
// (public: virtual __thiscall WONAPI::RoutingCreateGroupOp::~RoutingCreateGroupOp(void)    [??1RoutingCreateGroupOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c4b10
// (public: virtual long __thiscall WONAPI::RoutingCreateGroupOp::GetType(void)const     [?GetType@RoutingCreateGroupOp@WONAPI@@UBEJXZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c4b20
// (public: virtual void * __thiscall WONAPI::RoutingCreateGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingCreateGroupOp@WONAPI@@UAEPAX)
