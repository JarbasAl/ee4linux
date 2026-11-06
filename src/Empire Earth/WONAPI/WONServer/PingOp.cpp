// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK ping family; no Native/primary/physical claim.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Missing unit: proposed existing server-operation family profile.
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONPing.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PingOp::PingOp(const IPAddr &theAddr, unsigned char theLengthFieldSize, bool extended) 
	: ServerRequestOp(theAddr), mExtended(extended) 
{
	mLengthFieldSize = theLengthFieldSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WONStatus PingOp::Virtual10()
{
	WriteBuffer aBuf(mLengthFieldSize,true);
	aBuf.AppendByte(3);					// mini message
	aBuf.AppendByte(1);					// common service
	aBuf.AppendByte(5);					// ping message
	aBuf.AppendLong(GetTickCount());	// start tick
	aBuf.AppendBool(mExtended);			// extended ping flag
	mRequest = aBuf.ToByteBuffer(true);
	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WONStatus PingOp::Virtual14()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned char aServiceType = aMsg.ReadByte();
	unsigned char aMessageType = aMsg.ReadByte();
	if(aHeaderType!=3 || aServiceType!=1 || aMessageType!=6)
		return InvalidReplyHeader();

	// Read startTick and determine lag
	mLag = GetTickCount() - aMsg.ReadLong();

	if(aMsg.Available()>0)
	{
		aMsg.ReadString(mAppName);
		aMsg.ReadString(mLogicalName);
		mVersion = aMsg.ReadLong();
		aMsg.ReadString(mImage);
		mPID = aMsg.ReadLong();
		aMsg.ReadString(mPorts);
		mIsAuth = aMsg.ReadBool();
		if (aMsg.Available()>0)
		{
			aMsg.ReadString(mRegData);
			aMsg.ReadString(mServData);
		}
	}

	return (WONStatus)0;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PingOp::Virtual18()
{
	mAppName.erase();
	mLogicalName.erase();
	mVersion = 0;
	mImage.erase();
	mPID = 0;
	mPorts.erase();
	mIsAuth = false;
	mRegData.erase();
	mServData.erase();
}
