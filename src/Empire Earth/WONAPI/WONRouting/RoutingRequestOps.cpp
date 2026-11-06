// Native routing request serializers; borrowed receivers, complete wire behavior.
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
// FUNCTION: WON 0x6c4540
void RoutingCancelJoinGroupOp::Virtual0C()
{
    InitSendMsg(5);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendBool(mLeaveIfAlreadyInGroup);
    SendMsg();
    AddOp();
}


WONStatus RoutingCancelJoinGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006bd7a0
RoutingOpType RoutingCancelJoinGroupOp::GetType() const { return 5; }

// FUNCTION: WON 0x6c7620
void RoutingLeaveGroupOp::Virtual0C()
{
    InitSendMsg(43);
    mSendMsg.AppendShort(mGroupId);
    SendMsg();
    AddOp();
}


WONStatus RoutingLeaveGroupOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006bd940
RoutingOpType RoutingLeaveGroupOp::GetType() const { return 35; }

// FUNCTION: WON 0x6c8c60
void RoutingSetGroupFlagsOp::Virtual0C()
{
    InitSendMsg(61);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendLong(mGroupFlagMask);
    mSendMsg.AppendLong(mGroupFlags);
    mSendMsg.AppendLong(mAsyncFlagMask);
    mSendMsg.AppendLong(mAsyncFlags);
    SendMsg();
    AddOp();
}


WONStatus RoutingSetGroupFlagsOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006bdd20
RoutingOpType RoutingSetGroupFlagsOp::GetType() const { return 52; }

// FUNCTION: WON 0x6c8b20
void RoutingSendServerAlertOp::Virtual0C()
{
    InitSendMsg(56);
    mSendMsg.AppendWString(mAlertText, 2);
    mSendMsg.AppendShort((short)mRecipientList.size());
    std::list<unsigned short>::const_iterator i;
    i=mRecipientList.begin();
    while(true) {
        if(!(i != mRecipientList.end())) break;
        mSendMsg.AppendShort(*i);
        ++i;
    }
    SendMsg();
    AddOp();
}


WONStatus RoutingSendServerAlertOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006bdef0
RoutingOpType RoutingSendServerAlertOp::GetType() const { return 47; }

// FUNCTION: WON 0x6c8cc0
void RoutingStartServerShutdownOp::Virtual0C()
{
    InitSendMsg(65);
    mSendMsg.AppendWString(mAlertText, 2);
    mSendMsg.AppendLong(mSecondsUntilShutdown);
    SendMsg();
    AddOp();
}


WONStatus RoutingStartServerShutdownOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006be120
RoutingOpType RoutingStartServerShutdownOp::GetType() const { return 56; }


// FUNCTION: WON 0x006c4170
void RoutingAbortServerShutdownOp::Virtual0C()
{
    InitSendMsg(1);
    SendMsg();
    AddOp();
}


WONStatus RoutingAbortServerShutdownOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006be2e0
RoutingOpType RoutingAbortServerShutdownOp::GetType() const { return 1; }

// FUNCTION: WON 0x6c8c10
void RoutingSetClientFlagsOp::Virtual0C()
{
    InitSendMsg(60);
    mSendMsg.AppendLong(mAsyncMessageFlagMask);
    mSendMsg.AppendLong(mAsyncMessageFlags);
    mSendMsg.AppendLong(mClientFlagMask);
    mSendMsg.AppendLong(mClientFlags);
    SendMsg();
    AddOp();
}


WONStatus RoutingSetClientFlagsOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006be490
RoutingOpType RoutingSetClientFlagsOp::GetType() const { return 51; }

// FUNCTION: WON 0x6c8a20
void RoutingSendChatOp::Virtual0C()
{
    InitSendMsg(54);
    mSendMsg.AppendShort(mFlags);
    mSendMsg.AppendWString(mText, 2);
    mSendMsg.AppendShort((short)mRecipients.size());
    for (std::list<unsigned short>::const_iterator i = mRecipients.begin(); i != mRecipients.end(); ++i)
        mSendMsg.AppendShort(*i);
    SendMsg();
    if (mFlags & 1) AddOp();
}


WONStatus RoutingSendChatOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006be7f0
RoutingOpType RoutingSendChatOp::GetType() const { return 44; }

// FUNCTION: WON 0x6c8aa0
void RoutingSendDataOp::Virtual0C()
{
    InitSendMsg(55);
    mSendMsg.AppendByte(mFlags);
    mSendMsg.AppendBuffer(mData.get(), 2);
    mSendMsg.AppendShort((short)mRecipients.size());
    for (std::list<unsigned short>::const_iterator i = mRecipients.begin(); i != mRecipients.end(); ++i)
        mSendMsg.AppendShort(*i);
    SendMsg();
    if (mFlags & 1) AddOp();
}


WONStatus RoutingSendDataOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006bebb0
RoutingOpType RoutingSendDataOp::GetType() const { return 46; }

// FUNCTION: WON 0x6c4510
void RoutingBecomeModeratorOp::Virtual0C()
{
    InitSendMsg(4);
    mSendMsg.AppendBool(mModeratorOn);
    SendMsg();
    AddOp();
}


WONStatus RoutingBecomeModeratorOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006bf4e0
RoutingOpType RoutingBecomeModeratorOp::GetType() const { return 4; }

// FUNCTION: WON 0x6c42d0
void RoutingAcceptClientOp::Virtual0C()
{
    InitSendMsg(2);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendShort(mClientId);
    mSendMsg.AppendBool(mAccepted);
    mSendMsg.AppendWString(mAcceptComment, 2);
    SendMsg();
    AddOp();
}


WONStatus RoutingAcceptClientOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006c41f0
RoutingOpType RoutingAcceptClientOp::GetType() const { return 2; }

// FUNCTION: WON 0x6c4a20
void RoutingCreateDataObjectOp::Virtual0C()
{
    InitSendMsg(11);
    mSendMsg.AppendByte(mFlags);
    mSendMsg.AppendShort(mLinkId);
    mSendMsg.AppendString(mDataType, 1);
    mSendMsg.AppendWString(mDataName, 1);
    mSendMsg.AppendBuffer(mData.get(), 2);
    SendMsg();
    AddOp();
}


WONStatus RoutingCreateDataObjectOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006c48e0
RoutingOpType RoutingCreateDataObjectOp::GetType() const { return 11; }

// FUNCTION: WON 0x6c5210
void RoutingDeleteDataObjectOp::Virtual0C()
{
    InitSendMsg(17);
    mSendMsg.AppendShort(mLinkId);
    mSendMsg.AppendString(mDataType, 1);
    mSendMsg.AppendWString(mDataName, 1);
    SendMsg();
    AddOp();
}


WONStatus RoutingDeleteDataObjectOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006c5100
RoutingOpType RoutingDeleteDataObjectOp::GetType() const { return 16; }

// FUNCTION: WON 0x6c70f0
void RoutingInviteClientOp::Virtual0C()
{
    InitSendMsg(40);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendShort(mClientId);
    mSendMsg.AppendBool(mInvited);
    mSendMsg.AppendWString(mInviteComment, 2);
    SendMsg();
    AddOp();
}


WONStatus RoutingInviteClientOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006c7010
RoutingOpType RoutingInviteClientOp::GetType() const { return 33; }

// FUNCTION: WON 0x6c7840
void RoutingModifyDataObjectOp::Virtual0C()
{
    InitSendMsg(45);
    mSendMsg.AppendShort(mLinkId);
    mSendMsg.AppendString(mDataType, 1);
    mSendMsg.AppendWString(mDataName, 1);
    mSendMsg.AppendShort(mOffset);
    mSendMsg.AppendBool(mIsInsert);
    mSendMsg.AppendBuffer(mData.get(), 2);
    SendMsg();
    AddOp();
}


WONStatus RoutingModifyDataObjectOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006c7700
RoutingOpType RoutingModifyDataObjectOp::GetType() const { return 37; }

// FUNCTION: WON 0x6c8090
void RoutingReadDataObjectOp::Virtual0C()
{
    mDataObjects.clear();
    InitSendMsg(mSubscribe ? 67 : 50);
    mSendMsg.AppendShort(mLinkId);
    mSendMsg.AppendString(mDataType, 1);
    mSendMsg.AppendByte(mFlag);
    SendMsg();
    AddOp();
}

// Native type accessor, support only.
// FUNCTION: WON 0x006c7ed0
RoutingOpType RoutingReadDataObjectOp::GetType() const { return 41; }

// FUNCTION: WON 0x6c8e40
void RoutingUnsubscribeDataObjectOp::Virtual0C()
{
    InitSendMsg(68);
    mSendMsg.AppendShort(mLinkId);
    mSendMsg.AppendString(mDataType, 1);
    mSendMsg.AppendByte(mFlags);
    SendMsg();
    AddOp();
}


WONStatus RoutingUnsubscribeDataObjectOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 66) return (WONStatus)100104;
    return inMsg.ReadShort();
}
// Native type accessor, support only.
// FUNCTION: WON 0x006c8d60
RoutingOpType RoutingUnsubscribeDataObjectOp::GetType() const { return 57; }

// FUNCTION: WON 0x006c8130
WONStatus RoutingReadDataObjectOp::Virtual10(unsigned char inMsgType, ReadBuffer& inMsg)
{
    if (inMsgType != 49) return (WONStatus)100104;
    WONStatus status = inMsg.ReadShort();
    if (status == 0) ParseReplyExceptForStatus(inMsg);
    return status;
}
// FUNCTION: WON 0x006c8170
void RoutingReadDataObjectOp::ParseReplyExceptForStatus(ReadBuffer& inMsg)
{
    int count = (unsigned short)inMsg.ReadShort();
    for (int i = 0; i < count; ++i) {
        RoutingDataObjectPtr data = new RoutingDataObject;
        data->mLinkId = inMsg.ReadShort();
        inMsg.ReadString(data->mDataType, 1);
        inMsg.ReadWString(data->mName, 1);
        data->mData = inMsg.ReadBuf(2);
        mDataObjects.push_back(data);
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x006c4190
RoutingAcceptClientOp::RoutingAcceptClientOp(RoutingConnection* connection)
    : RoutingOp(connection),mGroupId(0),mClientId(0),mAccepted(false) {}
// FUNCTION: WON 0x006c6fb0
RoutingInviteClientOp::RoutingInviteClientOp(RoutingConnection* connection)
    : RoutingOp(connection),mGroupId(0),mClientId(0),mInvited(false) {}
}

namespace WONAPI {
// FUNCTION: WON 0x006c4870
RoutingCreateDataObjectOp::RoutingCreateDataObjectOp(RoutingConnection* connection)
    :RoutingOp(connection) { mLinkId=0; }
// FUNCTION: WON 0x006c5090
RoutingDeleteDataObjectOp::RoutingDeleteDataObjectOp(RoutingConnection* connection)
    :RoutingOp(connection) { mLinkId=0; }
// FUNCTION: WON 0x006c7690
RoutingModifyDataObjectOp::RoutingModifyDataObjectOp(RoutingConnection* connection)
    :RoutingOp(connection) { mLinkId=0; }
// FUNCTION: WON 0x006c8000
RoutingReadDataObjectOp::RoutingReadDataObjectOp(RoutingConnection* connection)
    :RoutingOp(connection) { mLinkId=0; mFlag=0; mSubscribe=false; }
// FUNCTION: WON 0x006c8d00
RoutingUnsubscribeDataObjectOp::RoutingUnsubscribeDataObjectOp(RoutingConnection* connection)
    :RoutingOp(connection) { mLinkId=0; mFlags=0; }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c8390
// (public: virtual __thiscall WONAPI::RoutingDataObject::~RoutingDataObject(void)    [??1RoutingDataObject@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006c8d90
// (public: virtual __thiscall WONAPI::RoutingUnsubscribeDataObjectOp::~RoutingUnsubscribeDataObjectOp(void)    [??1RoutingUnsubscribeDataObjectOp@WONAPI@)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c4200
// (public: virtual void * __thiscall WONAPI::RoutingAcceptClientOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingAcceptClientOp@WONAPI@@UAEP)
// TEMPLATE: WON 0x006c4220
// (public: virtual __thiscall WONAPI::RoutingAcceptClientOp::~RoutingAcceptClientOp(void)    [??1RoutingAcceptClientOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006c7020
// (public: virtual void * __thiscall WONAPI::RoutingInviteClientOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingInviteClientOp@WONAPI@@UAEP)
// TEMPLATE: WON 0x006c7040
// (public: virtual __thiscall WONAPI::RoutingInviteClientOp::~RoutingInviteClientOp(void)    [??1RoutingInviteClientOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c48f0
// (public: virtual void * __thiscall WONAPI::RoutingCreateDataObjectOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingCreateDataObjectOp@WONA)
// TEMPLATE: WON 0x006c4910
// (public: virtual __thiscall WONAPI::RoutingCreateDataObjectOp::~RoutingCreateDataObjectOp(void)    [??1RoutingCreateDataObjectOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006c5110
// (public: virtual void * __thiscall WONAPI::RoutingDeleteDataObjectOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingDeleteDataObjectOp@WONA)
// TEMPLATE: WON 0x006c5130
// (public: virtual __thiscall WONAPI::RoutingDeleteDataObjectOp::~RoutingDeleteDataObjectOp(void)    [??1RoutingDeleteDataObjectOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006c7710
// (public: virtual void * __thiscall WONAPI::RoutingModifyDataObjectOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingModifyDataObjectOp@WONA)
// TEMPLATE: WON 0x006c7730
// (public: virtual __thiscall WONAPI::RoutingModifyDataObjectOp::~RoutingModifyDataObjectOp(void)    [??1RoutingModifyDataObjectOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006c7ee0
// (public: virtual void * __thiscall WONAPI::RoutingReadDataObjectOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingReadDataObjectOp@WONAPI@@)
// TEMPLATE: WON 0x006c7f00
// (public: virtual __thiscall WONAPI::RoutingReadDataObjectOp::~RoutingReadDataObjectOp(void)    [??1RoutingReadDataObjectOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006c8d70
// (public: virtual void * __thiscall WONAPI::RoutingUnsubscribeDataObjectOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingUnsubscribeDataObj)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c8370
// (public: virtual void * __thiscall WONAPI::RoutingDataObject::`scalar deleting destructor'(unsigned int)    [??_GRoutingDataObject@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ca600
// (protected: void __thiscall std::list<class WONAPI::SmartPtr<struct WONAPI::RoutingDataObject>,class std::allocator<class WONAPI::SmartPtr<struct WONAP)
