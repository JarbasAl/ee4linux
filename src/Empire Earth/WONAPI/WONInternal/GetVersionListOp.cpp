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
GetVersionListOp::GetVersionListOp(const std::string& theProductName, ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(9),
	  mProductName(theProductName),
	  mStateFilter(VersionState_BadState)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetVersionListOp::GetVersionListOp(const std::string& theProductName, const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(9),
	  mProductName(theProductName)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Exceptions handled by ServerRequestOp
WONStatus GetVersionListOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	if (mSubMessageReplyType != mMsgType+1)
		return InvalidReplyHeader();
	
	// Do extended unpack
	ReadBuffer aReadBuf(mReplyData->data(), mReplyData->length());
	unsigned short numVersions = aReadBuf.ReadShort();

	// Read in each version
	for (int i=0; i < numVersions; i++)
	{
		SmartPtr<VersionData> versionData = new VersionData;

		unsigned short versionDataSize = aReadBuf.ReadShort(); // record size for expandability

		versionData->ReadFromBuffer(ReadBuffer(aReadBuf.ReadBytes(versionDataSize), versionDataSize));
		mVersionDataList.push_back(versionData);
	}

	// Finished
	return (WONStatus)0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetVersionListOp::Virtual04()
{
	mMessageType=1004;
	mSubMessageType=mMsgType;

	// Pack the message data
	WriteBuffer requestData(0,true);
	requestData.AppendString(mProductName);
	requestData.AppendString(mConfigName);
	requestData.AppendByte(mStateFilter);
	

	// Pack and call base class implementation
	mRequestData=requestData.ToByteBuffer(true);
	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetVersionListOp::Init()
{
	Virtual18();
	ServerRequestOp::mLengthFieldSize = 4;

	mConfigName = "%";	// % means get every config
	mStateFilter = VersionState_BadState;

}
