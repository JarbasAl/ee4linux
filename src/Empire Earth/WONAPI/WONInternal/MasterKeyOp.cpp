// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK complete key-server request/response algorithms.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Missing-unit proposed DBProxy-family profile; no Native/primary/physical attribution.
#include "ee/wonapi/WONMasterKeyOp.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InsertMasterKeyOp::InsertMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mSequenceNumber(0),
	  mIsBeta(false),
	  mActivateTime(0),
	  mDeactivateTime(0),
	  mKeyLength(0),
	  mCDKeyFlags(0x0001)
{
	memset(mKeyP, '\0', 20);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertMasterKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);
	requestData.AppendShort(mSequenceNumber);
	requestData.AppendBool(mIsBeta);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mDeactivateTime);
	requestData.AppendByte(mKeyLength);
	requestData.AppendBytes(mKeyP, mKeyLength);
	requestData.AppendLong(mCDKeyFlags);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus InsertMasterKeyOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack: None

	// Everything checked out
	return (WONStatus)0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateMasterKeyOp::UpdateMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mSequenceNumber(0),
	  mIsBeta(false),
	  mActivateTime(0),
	  mDeactivateTime(0),
	  mCDKeyFlags(0x0001)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateMasterKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);
	requestData.AppendShort(mSequenceNumber);
	requestData.AppendBool(mIsBeta);
	requestData.AppendLong(mActivateTime);
	requestData.AppendLong(mDeactivateTime);
	requestData.AppendLong(mCDKeyFlags);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UpdateMasterKeyOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack: None

	// Everything checked out
	return (WONStatus)0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DeleteMasterKeyOp::DeleteMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mSequenceNumber(0)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DeleteMasterKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);
	requestData.AppendShort(mSequenceNumber);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus DeleteMasterKeyOp::Virtual14()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::Virtual14();
	if (result != (WONStatus)0)
		return result;
	
	// Make sure this is the expected reply type
	if (mSubMessageReplyType != mSubMessageType+1)
		return InvalidReplyHeader();

	// Extended unpack: None

	// Everything checked out
	return (WONStatus)0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
QueryMasterKeyOp::QueryMasterKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryMasterKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus QueryMasterKeyOp::Virtual14()
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
		MasterKey aKey;
		mMasterKeyList.clear();
		short aCount = theData.ReadShort();
		for(short i = 0; i < aCount; i++)
		{
			aKey.mSequenceNumber = theData.ReadShort();
			aKey.mActivateDate = theData.ReadLong();
			aKey.mDeactivateDate = theData.ReadLong();
			aKey.mIsBeta = (theData.ReadByte() == 0) ? false : true;
			aKey.mCDKeyFlags = theData.ReadLong();
			mMasterKeyList.push_back(aKey);
		}
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
AuthCheckKeyOp::AuthCheckKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AuthCheckKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName, 1);
	requestData.AppendString(mProductName, 1);
	requestData.AppendShort(mKeyList.size());

	for(KEY_LIST_CITR anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr, 1);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus AuthCheckKeyOp::Virtual14()
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
		mKeyStatusList.clear();
		short aCount = theData.ReadShort();
		for(short i = 0; i < aCount; i++)
			mKeyStatusList.push_back(theData.ReadShort());
	}
	catch (ReadBufferException&)
	{
		return (WONStatus)100006;
	}

	// Everything checked out
	return (WONStatus)0;
}
