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
GetPatchServerModuleListOp::GetPatchServerModuleListOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(1)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetPatchServerModuleListOp::GetPatchServerModuleListOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(1)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Exceptions handled by ServerRequestOp
WONStatus GetPatchServerModuleListOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	if (mMessageReplyType != 7)
		return InvalidReplyHeader();
	
	// Do extended unpack
	ReadBuffer aReadBuf(mReplyData->data(), mReplyData->length());
	unsigned short numModules = aReadBuf.ReadShort();

	// Read in each module
	for (int i=0; i < numModules; i++)
	{
		unsigned long moduleID = aReadBuf.ReadLong();
		mModuleList.push_back(moduleID);
	}

	// Finished
	return (WONStatus)0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetPatchServerModuleListOp::Virtual04()
{
	mMessageType=6;
	mSubMessageType=mMsgType;

	// Call base class implementation
	DBProxyOp::Virtual04();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetPatchServerModuleListOp::Init()
{
	Virtual18();
	ServerRequestOp::mLengthFieldSize = 4;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GetPatchServerModuleListOp::IsModuleAvailable(unsigned short theMsgType, unsigned short theSubMsgType)
{
	std::list<unsigned long>::const_iterator anItr = mModuleList.begin();
	for (; anItr != mModuleList.end(); ++anItr)
	{
		// Search for this message type and sub message type
		if (theMsgType == LOWORD(*anItr) && theSubMsgType == HIWORD(*anItr))
			return true;
	}

	return false;
}
