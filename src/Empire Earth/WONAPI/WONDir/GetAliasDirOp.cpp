// SOURCE_ONLY_NO_NATIVE_ID: complete same-SDK alias directory request/response sequence.
// Missing-unit proposed family profile: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONDirAlias.h"


//#include "DirEntityReplyParser.h"

using namespace std;
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::Init()
{
	mLengthFieldSize   = 4;
	mNumFailedRequests = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GetAliasDirOp::GetAliasDirOp(ServerContext *theDirContext) : ServerRequestOp(theDirContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetAliasDirOp::GetAliasDirOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::Virtual18()
{
	GetEntityRequestList::const_iterator anItr = mRequestList.begin();
	for (;anItr != mRequestList.end(); ++anItr)
		(*anItr)->Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::AddRequest(GetEntityRequest* theRequest)
{
	mRequestList.push_back(theRequest);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetAliasDirOp::AddService(const std::wstring& theService)
{
	GetEntityRequestPtr pRequest = new GetEntityRequest;
	pRequest->SetPath(theService);
	pRequest->SetFlags(mFlags);
	pRequest->SetDataTypes(mDataTypes);

	AddRequest(pRequest);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus GetAliasDirOp::Virtual10()
{
	// Once per op
	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendByte(5);						// Small Header
	aMsg.AppendShort(2);					// DirServer
	aMsg.AppendShort(114);					// AliasGetEntity

	aMsg.AppendShort(0);					// MaxEntitiesPerReply
	aMsg.AppendString(mProductName);
	aMsg.AppendByte(mRequestList.size());	// Number of entities requests

	// Once per request
	GetEntityRequestList::const_iterator anItr = mRequestList.begin();
	for (;anItr != mRequestList.end(); ++anItr)
	{	
		(*anItr)->Pack(&aMsg, GetEntityRequest::Pack_GetMultiDirOp);
	}
	mRequest = aMsg.ToByteBuffer(true);

	// Prepare the iterator for Recv
	mCurRequest = mRequestList.begin();

	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Parse the dir entities (One MultiEntityReply for each Request)
WONStatus GetAliasDirOp::Virtual14()
{
	// Was the request list empty?
	if (mCurRequest == mRequestList.end())
	{
		// Construct a new request to parse out the status result
		GetEntityRequestPtr aRequest = new GetEntityRequest;
		return aRequest->ParseMultiEntityReply(mResponse->data(),mResponse->length());
	}

	(*mCurRequest)->ParseMultiEntityReply(mResponse->data(), mResponse->length());
		
	// Does this request have pending data?
	if ((*mCurRequest)->GetStatus() != (WONStatus)100059)
	{
		if ((*mCurRequest)->GetStatus() != (WONStatus)0)
			++mNumFailedRequests;
		++mCurRequest;
	}

	// Are all requests complete?
	if (mCurRequest == mRequestList.end())
	{
		if (mNumFailedRequests == 0)
			return (WONStatus)0;
		else if (mNumFailedRequests != mRequestList.size())
			return (WONStatus)-1223;
		else
			return (WONStatus)-1224;
	}
	else
		return (WONStatus)100059;

}
