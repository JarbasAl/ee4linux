// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK complete key-server request/response algorithms.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Missing-unit proposed DBProxy-family profile; no Native/primary/physical attribution.
#include "ee/wonapi/WONKeySubscribeOp.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseKeySubscribeOp::BaseKeySubscribeOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BaseKeySubscribeOp::Pack(WriteBuffer& theBufferR)
{
	// Stores the message data
	theBufferR.AppendString(mProductName);
	theBufferR.AppendWString(mCommunityName,2);
	theBufferR.AppendShort(mKeyList.size());
	for(std::list<std::string>::iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		theBufferR.AppendString(*anItr, 1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus BaseKeySubscribeOp::Unpack(ReadBuffer& theBufferR)
{
	try
	{
		mStatusList.clear();
		unsigned short aListSize = theBufferR.ReadShort();
		for(unsigned short i = 0; i < aListSize; i++)
			mStatusList.push_back(theBufferR.ReadShort());
	}
	catch (ReadBufferException&)
	{
		return (WONStatus)100006;
	}

	// Everything checked out
	return (WONStatus)0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InsertKeySubscribeOp::InsertKeySubscribeOp(ServerContext* theContext)
	: BaseKeySubscribeOp(theContext),
	mAllocateDate(-1),
	mAllocateDuration(0),
	mActivateTime(-1),
	mActivateDuration(0)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertKeySubscribeOp::Virtual04()
{
	mMessageType = 1006;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	BaseKeySubscribeOp::Pack(requestData);

	requestData.AppendLong(mAllocateDate);
	requestData.AppendLong(mAllocateDuration);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mActivateDuration);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus InsertKeySubscribeOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack
	ReadBuffer theData(mReplyData->data(), mReplyData->length());
	return BaseKeySubscribeOp::Unpack(theData);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateKeySubscribeOp::UpdateKeySubscribeOp(ServerContext* theContext)
	: BaseKeySubscribeOp(theContext),
	mUpdateFields(0),
	mAllocateDate(-1),
	mAllocateDuration(0),
	mActivateTime(-1),
	mActivateDuration(0),
	mActivateUserSeq(0)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateKeySubscribeOp::Virtual04()
{
	mMessageType = 1006;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	BaseKeySubscribeOp::Pack(requestData);

	requestData.AppendShort(mUpdateFields);
	requestData.AppendLong(mAllocateDate);
	requestData.AppendLong(mAllocateDuration);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mActivateDuration);
	requestData.AppendLong(mActivateUserSeq);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UpdateKeySubscribeOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack
	ReadBuffer theData(mReplyData->data(), mReplyData->length());
	return BaseKeySubscribeOp::Unpack(theData);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RemoveKeySubscribeOp::RemoveKeySubscribeOp(ServerContext* theContext)
	: BaseKeySubscribeOp(theContext)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveKeySubscribeOp::Virtual04()
{
	mMessageType = 1006;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	BaseKeySubscribeOp::Pack(requestData);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RemoveKeySubscribeOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack
	ReadBuffer theData(mReplyData->data(), mReplyData->length());
	return BaseKeySubscribeOp::Unpack(theData);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
QueryKeySubscribeOp::QueryKeySubscribeOp(ServerContext* theContext)
	: DBProxyOp(theContext)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryKeySubscribeOp::Virtual04()
{
	mMessageType = 1006;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendString(mProductName);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr, 1);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus QueryKeySubscribeOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack
	ReadBuffer theData(mReplyData->data(), mReplyData->length());
	try
	{
		KEY_DATA aKey;
		mKeyDataList.clear();
		short aListSize = theData.ReadShort();
		for(short i = 0; i < aListSize; i++)
		{
			aKey.mStatus = (WONStatus)theData.ReadShort();
			aKey.mAllocateDate = theData.ReadLong();
			aKey.mAllocateDuration = theData.ReadLong();
			aKey.mActivateTime = theData.ReadLong();
			aKey.mActivateDuration = theData.ReadLong();
			aKey.mActivateUserSeq = theData.ReadLong();
			mKeyDataList.push_back(aKey);
		}
	}
	catch (ReadBufferException&)
	{
		return (WONStatus)100006;
	}

	// Everything checked out
	return (WONStatus)0;
}
