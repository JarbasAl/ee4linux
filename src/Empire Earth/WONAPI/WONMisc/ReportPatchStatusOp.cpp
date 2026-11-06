// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: missing SDK patch-server request/reply implementation.
// Missing unit uses proposed existing DBProxy/patch-server family profile.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONPatchProtocol.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReportPatchStatusOp::ReportPatchStatusOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(19)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ReportPatchStatusOp::Virtual14()
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
void ReportPatchStatusOp::Virtual04()
{
	mMessageType=1004;
	mSubMessageType=mMsgType;

	// Pack the message data
	WriteBuffer requestData(0,true);
	requestData.AppendString(mProductName);
	requestData.AppendString(mConfigName);
	requestData.AppendString(mFromVersion);
	requestData.AppendString(mToVersion);
	requestData.AppendString(mNetAddress);		// patch url
	requestData.AppendByte(mPatchStatus);

	// Pack and call base class implementation
	mRequestData=requestData.ToByteBuffer(true);

	if(mUDPSocket.get()==NULL) // just do TCP ServerRequest
	{
		DBProxyOp::Virtual04();
		return;
	}

	// Do UDP server request

	IPAddr anAddr = GetAddr();
	if(!anAddr.IsValid())
	{
		Fn29F220((WONStatus)100055);
		return;
	}

	Virtual18();
	unsigned char aLengthFieldSize = mLengthFieldSize;
	mLengthFieldSize=0;
	Virtual10();
	mLengthFieldSize=aLengthFieldSize;
	SmartPtr<SendBytesToOp> anOp = new SendBytesToOp(mRequest, anAddr, mUDPSocket);
	anOp->Fn29F500(mRunMode,mTimeout);
	Fn29F220((WONStatus)0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReportPatchStatusOp::Init()
{
	Virtual18();
	ServerRequestOp::mLengthFieldSize = 4;

	mProductName	= "";
	mConfigName		= "";
	mFromVersion	= "";
	mToVersion		= "";
	mNetAddress		= "";		// patch url
	mPatchStatus	= 0;
}

