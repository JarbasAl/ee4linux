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
void GetServiceOp::Init()
{
	mLengthFieldSize = 4;

	mFlags = DIR_GF_DECOMPSERVICES | DIR_GF_ADDDISPLAYNAME | DIR_GF_ADDDATAOBJECTS | 
		DIR_GF_SERVADDNAME | DIR_GF_SERVADDNETADDR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GetServiceOp::GetServiceOp(ServerContext *theDirContext) : ServerRequestOp(theDirContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetServiceOp::GetServiceOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetServiceOp::Virtual18()
{
	mService = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetServiceOp::Virtual10()
{
	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendByte(5);						// Small Header
	aMsg.AppendShort(2);					// DirServer
	aMsg.AppendShort(105);					// GetServiceEx

	aMsg.AppendLong(mFlags);				// GetFlags
	aMsg.AppendWString(mPath,2);				// Directory Path
	aMsg.AppendWString(mName,2);				// Name of service
	aMsg.AppendBuffer(mNetAddr,1);			// Net address
	aMsg.AppendShort(mDataTypes.size());	// Number of data types requested
	
	DirDataTypeSet::iterator anItr = mDataTypes.begin();
	while(anItr!=mDataTypes.end())
	{
		aMsg.AppendString(*anItr,1);
		++anItr;
	}

	mRequest = aMsg.ToByteBuffer(true);
	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetServiceOp::Virtual14()
{
	WONStatus aStatus = DirEntityReplyParser::ParseSingleEntityReply(mResponse->data(),mResponse->length(),mService);
	if(aStatus==(WONStatus)100063)
		return InvalidReplyHeader();
	else
		return aStatus;
}
