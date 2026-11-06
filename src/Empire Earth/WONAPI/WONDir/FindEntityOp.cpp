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
void FindEntityOp::Init()
{
	mLengthFieldSize = 4;
	mIsService = true;

	mGetFlags = DIR_GF_DECOMPROOT | DIR_GF_DECOMPSERVICES | DIR_GF_DECOMPSUBDIRS | DIR_GF_ADDTYPE
		| DIR_GF_ADDDISPLAYNAME | DIR_GF_ADDDATAOBJECTS | DIR_GF_SERVADDNAME | DIR_GF_SERVADDNETADDR 
		| DIR_GF_DIRADDNAME;

	mMatchMode = DIR_FMM_EXACT;
	mFindFlags = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FindEntityOp::FindEntityOp(ServerContext *theDirContext, bool isService) : ServerRequestOp(theDirContext)
{
	Init();
	mIsService = isService;
}
   
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FindEntityOp::FindEntityOp(const IPAddr &theAddr, bool isService) : ServerRequestOp(theAddr)
{
	Init();
	mIsService = isService;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FindEntityOp::Virtual18()
{
	mService = NULL;
	mDirEntityList.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus FindEntityOp::Virtual10()
{
	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendByte(5);						// Small message
	aMsg.AppendShort(2);					// Directory Server
	aMsg.AppendShort(mIsService?109:107);	// FindServiceEx : FindDirectoryEx
	
	aMsg.AppendLong(mGetFlags);				
	aMsg.AppendByte(mMatchMode);
	aMsg.AppendByte(mFindFlags);
	aMsg.AppendShort(0);					// MaxEntitiesPerReply
	aMsg.AppendWString(mPath,2);				// Directory path
	aMsg.AppendWString(mName,2);				// Match name

	if(mIsService)
		aMsg.AppendBuffer(mNetAddr,1);		// Match NetAddress

	aMsg.AppendWString(mDisplayName,2);		// Match DisplayName

	aMsg.AppendShort(mFindDataObjects.size());	// num data objects
	
	DirDataObjectList::iterator aFindItr = mFindDataObjects.begin();
	while(aFindItr!=mFindDataObjects.end())
	{
		DirDataObject &aDataObject = *aFindItr;
		aMsg.AppendString(aDataObject.mDataType,1);
		aMsg.AppendBuffer(aDataObject.mData,2);
		
		++aFindItr;
	}

	aMsg.AppendShort(mGetDataTypes.size()); // Num Get DataObjects
	DirDataTypeSet::iterator aGetItr = mGetDataTypes.begin();
	while(aGetItr!=mGetDataTypes.end())
	{
		aMsg.AppendString(*aGetItr,1);
		++aGetItr;
	}

	mRequest = aMsg.ToByteBuffer(true);
	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus FindEntityOp::Virtual14()
{
	WONStatus aStatus;
	if(mIsService && !(mFindFlags & DIR_FF_MATCHALL))
	{
		aStatus = DirEntityReplyParser::ParseSingleEntityReply(mResponse->data(),mResponse->length(),mService);
		if(aStatus==(WONStatus)0)
			mDirEntityList.push_back(mService);
	}
	else
	{
		aStatus = DirEntityReplyParser::ParseMultiEntityReply(mResponse->data(),mResponse->length(),mDirEntityList);
		if(aStatus==(WONStatus)0 && mIsService && !mDirEntityList.empty())
			mService = mDirEntityList.front();
	}

	if(aStatus==(WONStatus)100063)
		return InvalidReplyHeader();
	else
		return aStatus;
}
