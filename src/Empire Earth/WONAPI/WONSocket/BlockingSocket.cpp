// Original-backed blocking operation family; source only, no runtime authority.
// Const send APIs use the finite reviewed legacy-operation constructor boundary bridge.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006b1c10
WONStatus BlockingSocket::Connect(const IPAddr& inAddress,unsigned long inTimeout)
{
    SmartPtr<ConnectOp> op=new ConnectOp(inAddress,this);
    op->RunBlock(inTimeout);
    return op->GetStatus();
}

// FUNCTION: WON 0x006b1f10
WONStatus BlockingSocket::RecvMsg(ByteBufferPtr& outMessage,unsigned long inTimeout)
{
    SmartPtr<RecvMsgOp> op=new RecvMsgOp(this);
    if (op->RunBlock(inTimeout)) outMessage=op->GetMsg().get();
    return op->GetStatus();
}

// FUNCTION: WON 0x006b2040
WONStatus BlockingSocket::RecvCRMsg(ByteBufferPtr& outMessage,unsigned long inTimeout)
{
    SmartPtr<RecvCRMsgOp> op=new RecvCRMsgOp(this);
    if (op->RunBlock(inTimeout)) outMessage=op->GetMsg().get();
    return op->GetStatus();
}
// Genuine SDK const payload APIs adapted only at existing legacy ctor boundary.
// Native2b1d00/228, operation-run/status adapter, support0.
// FUNCTION: WON 0x006b1d00
WONStatus BlockingSocket::SendBytes(const ByteBuffer* bytes,unsigned long timeout)
{
    SmartPtr<SendBytesOp> op=new SendBytesOp(const_cast<ByteBuffer*>(bytes),this);
    op->RunBlock(timeout);
    return op->GetStatus();
}

// Native2b1e30/219, same accepted SendMsgOp const ownership bridge, support0.
// FUNCTION: WON 0x006b1e30
WONStatus BlockingSocket::SendMsg(const ByteBuffer* message,unsigned long timeout)
{
    SmartPtr<SendMsgOp> op=new SendMsgOp(const_cast<ByteBuffer*>(message),this);
    op->RunBlock(timeout);
    return op->GetStatus();
}

} // namespace WONAPI

namespace WONAPI {
// FUNCTION: WON 0x006b1bd0
BlockingSocket::BlockingSocket(int type) : QueueSocket(type) {}
// FUNCTION: WON 0x006aab50
AsyncSocket* BlockingSocket::Duplicate()
{
    return new BlockingSocket(mType);
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing datagram operation runners.
namespace WONAPI {
WONStatus BlockingSocket::SendBytesTo(const ByteBuffer *theBytes, const IPAddr &theAddr, DWORD theTimeout)
{
	SmartPtr<SendBytesToOp> anOp = new SendBytesToOp(const_cast<ByteBuffer*>(theBytes),theAddr,this);
	anOp->RunBlock(theTimeout);
	return anOp->GetStatus();
}

WONStatus BlockingSocket::RecvBytesFrom(ByteBufferPtr &theBytes, unsigned short theMaxBytes, IPAddr &theAddr, DWORD theTimeout)
{
	SmartPtr<RecvBytesFromOp> anOp = new RecvBytesFromOp(theMaxBytes,this);
	if(anOp->RunBlock(theTimeout))
	{
		theAddr = anOp->GetAddr();
		theBytes = anOp->GetBytes();
	}
	
	return anOp->GetStatus();
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine blocking byte receive prerequisite.
namespace WONAPI {
WONStatus BlockingSocket::RecvBytes(ByteBufferPtr &theBytes, DWORD theNumBytes, DWORD theTimeout)
{
	SmartPtr<RecvBytesOp> anOp = new RecvBytesOp(theNumBytes,this);
	if(anOp->RunBlock(theTimeout))
		theBytes = anOp->GetBytes();

	return anOp->GetStatus();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006aa960
// (public: virtual void * __thiscall WONAPI::SendBytesToOp::`scalar deleting destructor'(unsigned int)    [??_GSendBytesToOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006aa980
// (public: virtual __thiscall WONAPI::SendBytesToOp::~SendBytesToOp(void)    [??1SendBytesToOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ab470
// (public: virtual __thiscall WONAPI::RecvMsgOp::~RecvMsgOp(void)    [??1RecvMsgOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ab5a0
// (public: virtual __thiscall WONAPI::RecvBytesOp::~RecvBytesOp(void)    [??1RecvBytesOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006abad0
// (public: virtual __thiscall WONAPI::SendBytesOp::~SendBytesOp(void)    [??1SendBytesOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006abb80
// (public: virtual __thiscall WONAPI::SendMsgOp::~SendMsgOp(void)    [??1SendMsgOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006b1df0
// (public: class WONAPI::SmartPtr<class WONAPI::ByteBuffer> __thiscall WONAPI::RecvCRMsgOp::GetMsg(void)const     [?GetMsg@RecvCRMsgOp@WONAPI@@QBE?AV?$Sm)
// TEMPLATE: WON 0x006b2180
// (public: virtual class WONAPI::AsyncOp * __thiscall WONAPI::RecvCRMsgOp::Duplicate(void)    [?Duplicate@RecvCRMsgOp@WONAPI@@UAEPAVAsyncOp@2@XZ])
// TEMPLATE: WON 0x006b2280
// (public: virtual __thiscall WONAPI::RecvCRMsgOp::~RecvCRMsgOp(void)    [??1RecvCRMsgOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b1bf0
// (public: virtual void * __thiscall WONAPI::BlockingSocket::`scalar deleting destructor'(unsigned int)    [??_GBlockingSocket@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b2260
// (public: virtual void * __thiscall WONAPI::RecvCRMsgOp::`scalar deleting destructor'(unsigned int)    [??_GRecvCRMsgOp@WONAPI@@UAEPAXI@Z])
