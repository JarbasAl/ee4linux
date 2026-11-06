// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Missing unit: proposed existing server-operation family profile.
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK authenticated contest request/reply.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONContestCallOp.h"
using namespace WONAPI;

void ContestCallOp::Init()
{
	mProcNum = 0;
	mLengthFieldSize = 2;
	mUseAuth2 = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContestCallOp::ContestCallOp(ServerContext *theContestContext) : ServerRequestOp(theContestContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContestCallOp::ContestCallOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ContestCallOp::Virtual04()
{
	if(mAuthType==AUTH_TYPE_NONE)
	{
		Fn29F220((WONStatus)100056);
		return;
	}

	ServerRequestOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ContestCallOp::Virtual10()
{
	WriteBuffer aBuf(mLengthFieldSize,true);

	aBuf.AppendByte(3);			// mini message
	aBuf.AppendByte(4);		// Contest Service
	aBuf.AppendByte(1);		// ContestDBCall

	aBuf.AppendLong(mAuthContext->GetPeerData()->GetCertificate()->GetUserId()); // WONUserSeq
	aBuf.AppendShort(mProcNum); // Contest DB Procedure number

	for(int i=0; i<16; i++) // 16 Byte GUID (not needed)
		aBuf.AppendByte(0);

	aBuf.AppendBuffer(mRequestData);
	mRequest = aBuf.ToByteBuffer(true);
	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ContestCallOp::Virtual14()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadByte();
	unsigned short aMessageType = aMsg.ReadByte();
	if(aHeaderType!=3 || aServiceType!=4 || aMessageType!=2)
		return InvalidReplyHeader();

	short aStatus = aMsg.ReadShort();
	mReplyData = new ByteBuffer(aMsg.data() + aMsg.pos(), aMsg.Available(),false);

	return (WONStatus)aStatus;
}

