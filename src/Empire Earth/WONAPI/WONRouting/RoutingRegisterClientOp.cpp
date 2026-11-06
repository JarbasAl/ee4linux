// Original-backed routing registration family.
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
// FUNCTION: WON 0x006c8450
RoutingRegisterClientOp::RoutingRegisterClientOp(RoutingConnection* inConnection)
    : RoutingOp(inConnection)
{
    mReconnectId = 0;
    mAsyncMessageFlags = 0;
    mRegisterFlags = 0;
    mClientFlags = 0;
    mClientId = 0;
}

// FUNCTION: WON 0x006c8680
RoutingRegisterClientOp::RoutingRegisterClientOp(RoutingConnection* inConnection,
    const std::wstring& inName,const std::wstring& inPassword,
    unsigned long inReconnectId,unsigned long inAsyncFlags,
    unsigned long inRegisterFlags,unsigned long inClientFlags)
    : RoutingOp(inConnection),mClientName(inName),mServerPassword(inPassword),
      mReconnectId(inReconnectId),mAsyncMessageFlags(inAsyncFlags),
      mRegisterFlags(inRegisterFlags),mClientFlags(inClientFlags)
{
    // Native overloaded constructor leaves mClientId and reply counters untouched.
}

// FUNCTION: WON 0x006c8540
RoutingOpType RoutingRegisterClientOp::GetType() const { return (RoutingOpType)42; }

// FUNCTION: WON 0x006c87a0
void RoutingRegisterClientOp::Virtual0C()
{
    mReplyCount = 0;
    mNumRepliesNeeded = 1;
    if (mRegisterFlags & 4) mNumRepliesNeeded = 2;
    InitSendMsg(52);
    mSendMsg.AppendWString(mClientName, 2);
    mSendMsg.AppendWString(mServerPassword, 2);
    mSendMsg.AppendLong(mReconnectId);
    mSendMsg.AppendLong(mAsyncMessageFlags);
    mSendMsg.AppendLong(mRegisterFlags);
    mSendMsg.AppendLong(mClientFlags);
    SendMsg();
    AddOp();
}

// FUNCTION: WON 0x006c8840
WONStatus RoutingRegisterClientOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    WONStatus status;
    if (inMsgType != 23 && inMsgType != 74) {
        if (inMsgType == 51) {
            status = inMsg.ReadShort();
            if (status == 0) {
                mClientId = inMsg.ReadShort();
                inMsg.ReadWString(mClientName, 2);
                mReconnectId = inMsg.ReadLong();
            }
            ++mReplyCount;
        } else {
            return (WONStatus)100104;
        }
    } else {
        if (!(mRegisterFlags & 0x14))
            return (WONStatus)100104;
        SmartPtr<RoutingGetClientListOp> op = new RoutingGetClientListOp(mConnection.get());
        status = op->Virtual10(inMsgType, inMsg);
        mClientMap = op->mClientMap;
        ++mReplyCount;
    }
    if (status == 0 && mReplyCount != mNumRepliesNeeded) return (WONStatus)100105;
    return status;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c8570
// (public: virtual __thiscall WONAPI::RoutingRegisterClientOp::~RoutingRegisterClientOp(void)    [??1RoutingRegisterClientOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c8550
// (public: virtual void * __thiscall WONAPI::RoutingRegisterClientOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingRegisterClientOp@WONAPI@@)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c96c0
// (protected: static struct std::_Tree<unsigned short,struct std::pair<unsigned short const ,class WONAPI::SmartPtr<struct WONAPI::RoutingClientInfo> >,s)
