// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK patch/version DBProxy protocol unit.
// Missing unit proposed existing DBProxy/patch-server family profile.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONPatchAdmin.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AddVersionOp::AddVersionOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(3),
	  mProductName("")
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AddVersionOp::AddVersionOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(3),
	  mProductName("")
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus AddVersionOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	if (mSubMessageReplyType != mMsgType+1)
		return InvalidReplyHeader();
	
	// Do extended unpack



	// Finished
	return (WONStatus)0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AddVersionOp::Virtual04()
{
	mMessageType=1004;
	mSubMessageType=mMsgType;

	// Pack the message data
	WriteBuffer requestData(0,true);
	requestData.AppendString(mProductName);

	// Append the version data (don't include state);
	mVersionData->WriteToBuffer(requestData, VersionData::WriteFlag_AppendDescripURL);
	
	// Pack and call base class implementation
	mRequestData=requestData.ToByteBuffer(true);
	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AddVersionOp::Init()
{
	Virtual18();
	ServerRequestOp::mLengthFieldSize = 4;
	mVersionData = new VersionData;
}
