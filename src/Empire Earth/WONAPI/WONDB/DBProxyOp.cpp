// New native DBProxy operation source; original grouping is provisional.

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// Direct-address form of the constructor; nothing in this build calls it.
DBProxyOp::DBProxyOp(const IPAddr& theAddr)
	: ServerRequestOp(theAddr),
	  mMessageType(0),
	  mMessageReplyType(0),
	  mSubMessageType(0),
	  mSubMessageReplyType(0),
      mReceivedData(0,true),
	  mReplyErrorString("")
{

}
// FUNCTION: WON 0x006cca10
DBProxyOp::DBProxyOp(ServerContext* inContext)
    : ServerRequestOp(inContext), mMessageType(0), mMessageReplyType(0),
      mSubMessageType(0), mSubMessageReplyType(0), mReceivedData(0,true),
      mReplyErrorString("")
{
}
// FUNCTION: WON 0x006ccc00
WONStatus DBProxyOp::Virtual10()
{
    WriteBuffer aRequest(mLengthFieldSize,true);
    aRequest.AppendByte(5);
    aRequest.AppendShort(14);
    aRequest.AppendShort(mMessageType);
    aRequest.AppendShort(mSubMessageType);
    aRequest.AppendBuffer(mRequestData.get(),0);
    mRequest=aRequest.ToByteBuffer(true);
    return 100059;
}
// FUNCTION: WON 0x006ccd30
WONStatus DBProxyOp::Virtual14()
{
    ReadBuffer aReply(mResponse->data(),mResponse->length());
    unsigned char aHeader=aReply.ReadByte();
    unsigned short aService=aReply.ReadShort();
    mMessageReplyType=aReply.ReadShort();
    mSubMessageReplyType=aReply.ReadShort();
    WONStatus aStatus=aReply.ReadShort();
    aReply.ReadString(mReplyErrorString,2);
    unsigned char aFlags=aReply.ReadByte();
    if (aHeader!=5 || aService!=14)
        return InvalidReplyHeader();
    if (aReply.mL00<aReply.length())
        mReceivedData.AppendBytes(aReply.mData+aReply.mL00,aReply.length()-aReply.mL00);
    if ((aFlags&0x80)!=0) {
        mReplyData=mReceivedData.ToByteBuffer(true);
        return aStatus;
    }
    return 100059;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK reset forwarding; closes declared virtual provider.
namespace WONAPI {
void DBProxyOp::Virtual18()
{
	// Clear all received data
	//mReplyData->Release();

	ServerRequestOp::Virtual18();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ccb20
// (public: virtual __thiscall WONAPI::DBProxyOp::~DBProxyOp(void)    [??1DBProxyOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ccb00
// (public: virtual void * __thiscall WONAPI::DBProxyOp::`scalar deleting destructor'(unsigned int)    [??_GDBProxyOp@WONAPI@@UAEPAXI@Z])
