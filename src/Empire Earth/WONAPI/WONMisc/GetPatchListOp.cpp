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
GetPatchListOp::GetPatchListOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(17)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetPatchListOp::GetPatchListOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(17)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetPatchListOp::~GetPatchListOp() // Safety Lock -- Enforces Smart Ptr Use
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetPatchListOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	if (mSubMessageReplyType != mMsgType+1)
		return InvalidReplyHeader();
	
	// Do extended unpack
	ReadBuffer readBuf(mReplyData->data(), mReplyData->length());

	unsigned short numPatches = readBuf.ReadShort();
	for (int patchNum=0; patchNum<numPatches; ++patchNum)
	{
		unsigned short patchDataSize = readBuf.ReadShort();

		ReadBuffer patchData(readBuf.ReadBytes(patchDataSize), patchDataSize);

		SmartPtr<PatchData> thePatchData = new PatchData;
		thePatchData->ReadFromBuffer(patchData, true);
		mPatchDataList.push_back(thePatchData);
	}

	// Finished
	return (WONStatus)0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetPatchListOp::Virtual04()
{
	mMessageType=1004;
	mSubMessageType=mMsgType;

	// Pack the message data
	WriteBuffer requestData(0,true);
	requestData.AppendString(mProductName);
	requestData.AppendString(mConfigName);
	requestData.AppendString(mFromVersion);
	requestData.AppendString(mToVersion);
	requestData.AppendByte(mStateFilter);
	

	// Pack and call base class implementation
	mRequestData=requestData.ToByteBuffer(true);
	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetPatchListOp::Init()
{
	Virtual18();
	ServerRequestOp::mLengthFieldSize = 4;

	mProductName	= "";
	mConfigName		= "";
	mFromVersion	= "";
	mToVersion		= "";
	mStateFilter	= 2;
}

