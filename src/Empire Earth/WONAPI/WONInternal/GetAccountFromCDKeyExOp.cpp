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
GetAccountFromCDKeyExOp::GetAccountFromCDKeyExOp(ServerContext* theContext, std::string theCDKey, unsigned long theCommunity)
	: GetAccountFromCDKeyOp(theContext,theCDKey,theCommunity),
	  mActionFlag(RESET)
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAccountFromCDKeyExOp::Virtual04()
{
	mMessageType=1002;
	mSubMessageType=MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);

	// Append the CDKey (string)
	requestData.AppendString(mCDKey,2);

	// Append the community (unsigned long)
	requestData.AppendLong(mCommunity);
	
	// Append the Reset or Ban flag
	requestData.AppendByte(mActionFlag);

	// Append the profile data
	mProfileData->PackRequestData(requestData,true);

	// Copy buffer into the class
	mRequestData=requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* GetAccountFromCDKeyExOp::GetActionString(ACTIONFLAG actionFlag)
{
	switch(actionFlag)
	{
	case RESET:
		return "RESET";
	case BAN:
		return "BAN";
	default:
		return "Unknown Action";
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
