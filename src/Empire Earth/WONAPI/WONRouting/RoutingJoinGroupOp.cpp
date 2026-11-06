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
// FUNCTION: WON 0x006c7140
RoutingJoinGroupOp::RoutingJoinGroupOp(RoutingConnection* inConnection)
    : RoutingOp(inConnection)
{
    mGroupId = 0;
    mJoinFlags = 0;
    mMemberFlags = 0;
}

// FUNCTION: WON 0x006c7370
void RoutingJoinGroupOp::Virtual0C()
{
    InitSendMsg(42);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendWString(mGroupPassword, 2);
    mSendMsg.AppendWString(mJoinComment, 2);
    mSendMsg.AppendByte(mJoinFlags);
    mSendMsg.AppendByte(mMemberFlags);
    SendMsg();
    AddOp();
}

// Whole-family reply source intent retained until actual GetMembersOp78 lifetime
// gate. All map assignment and operation ownership are real typed C++ objects.
// FUNCTION: WON 0x006c73e0
WONStatus RoutingJoinGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType == 41) {
        WONStatus status = inMsg.ReadShort();
        unsigned short groupId = inMsg.ReadShort();
        if (groupId == mGroupId) {
            inMsg.ReadWString(mJoinComment, 2);
            if (status == (WONStatus)-2541) return (WONStatus)100105;
            if (status != 0) return status;
            return (mJoinFlags & 1) ? (WONStatus)100105 : (WONStatus)0;
        }
    } else if (inMsgType == 29) {
        SmartPtr<RoutingGetMembersOfGroupOp> op =
            new RoutingGetMembersOfGroupOp(mConnection.get());
        op->SetGroupId(mGroupId);
        WONStatus status = op->Virtual10(29, inMsg);
        if (status == 0) mMemberMap = op->GetMembers();
        return status;
    }
    return (WONStatus)100104;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c7250
// (public: virtual __thiscall WONAPI::RoutingJoinGroupOp::~RoutingJoinGroupOp(void)    [??1RoutingJoinGroupOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c7220
// (public: virtual long __thiscall WONAPI::RoutingJoinGroupOp::GetType(void)const     [?GetType@RoutingJoinGroupOp@WONAPI@@UBEJXZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c7230
// (public: virtual void * __thiscall WONAPI::RoutingJoinGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingJoinGroupOp@WONAPI@@UAEPAXI@Z])
