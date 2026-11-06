// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK directory executable implementation.
// Missing-unit proposed family profile: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// No FUNCTION/Native address or primary/physical byte credit is claimed.
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONDirExtended.h"
using namespace std;
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RenewEntityOp::Init()
{
	mLengthFieldSize = 4;
	mLifespan = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenewEntityOp::RenewEntityOp(ServerContext *theDirContext, bool isService) : ServerRequestOp(theDirContext)
{
	mIsService = isService;
	Init();
}
   
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RenewEntityOp::RenewEntityOp(const IPAddr &theAddr, bool isService) : ServerRequestOp(theAddr)
{
	mIsService = isService;
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RenewEntityOp::Virtual10()
{
	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendByte(5);						// Small message
	aMsg.AppendShort(2);					// Directory Server
	aMsg.AppendShort(mIsService?205:204);	// RenewService : RenewDir
	aMsg.AppendWString(mPath,2);				// Directory path

	if(mIsService)
	{
		aMsg.AppendWString(mName,2);				// Entity name
		aMsg.AppendBuffer(mNetAddr,1);
	}

	aMsg.AppendLong(mLifespan);

	mRequest = aMsg.ToByteBuffer(true);
	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RenewEntityOp::Virtual14()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadShort();
	unsigned short aMessageType = aMsg.ReadShort();

	if(aHeaderType!=5 || aServiceType!=2 || aMessageType!=1)
		return InvalidReplyHeader();


	short aStatus = aMsg.ReadShort();
	return (WONStatus)aStatus;
}
