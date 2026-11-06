// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK account-from-product-key protocol.
// Missing unit proposed existing DBProxy/account family profile.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAccountFromCDKey.h"




using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetAccountFromCDKeyOp::GetAccountFromCDKeyOp(ServerContext* theContext, std::string theCDKey, unsigned long theCommunity)
	: DBProxyOp(theContext),
	  mCDKey(theCDKey),
	  mCommunity(theCommunity),
	  mUserName(L""),
	  mPassword(L""),
	  mEmail(""),
	  mLastLogin("")
{
	mProfileData = new ProfileData();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAccountFromCDKeyOp::Virtual18()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAccountFromCDKeyOp::Virtual04()
{
	mMessageType=1002;
	mSubMessageType=MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);

	// Append the CDKey (string)
	requestData.AppendString(mCDKey,2);

	// Append the community (unsigned long)
	requestData.AppendLong(mCommunity);
	
	// Append the profile data
	mProfileData->PackRequestData(requestData,true);

	// Copy buffer into the class
	mRequestData=requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetAccountFromCDKeyOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack:
	// [WString] UserName
	// [WString] Password
	// [String ] Email

	ReadBuffer theData(mReplyData->data(), mReplyData->length());

	try
	{
		theData.ReadWString(mUserName,2);
		theData.ReadString (mEmail,2);
		theData.ReadWString(mPassword,2);
		theData.ReadString (mLastLogin,2);
		theData.ReadString (mActivated,2);

		// Unpack the profile data (custom actions)
		mProfileData->UnpackGetData(theData); 
	}
	catch (ReadBufferException&)
	{
	}

	// Everything checked out
	return (WONStatus)0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
