// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK complete key-server request/response algorithms.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Missing-unit proposed DBProxy-family profile; no Native/primary/physical attribution.
#include "ee/wonapi/WONBannedKeyOp.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InsertBannedKeyOp::InsertBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName(""),
	  mBannedUntil(0)
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertBannedKeyOp::Virtual18()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InsertBannedKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;
	mStatusList.clear();

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);
	requestData.AppendLong(mBannedUntil);

	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::const_iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr,1);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus InsertBannedKeyOp::Virtual14()
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
		unsigned short aListSize = theData.ReadShort();
		for(unsigned short i = 0; i < aListSize; i++)
			mStatusList.push_back(theData.ReadShort());
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
QueryBannedKeyOp::QueryBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryBannedKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;
	mKeyList.clear();

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
WONStatus QueryBannedKeyOp::Virtual14()
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
		unsigned short aListSize = theData.ReadShort();
		BANNED_KEY_PAIR aBannedKey;
		for(unsigned short i = 0; i < aListSize; i++)
		{
			theData.ReadString(aBannedKey.first,1);
			aBannedKey.second = theData.ReadLong();
			mKeyList.push_back(aBannedKey);
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
QueryListBannedKeyOp::QueryListBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueryListBannedKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;
	mKeyDataList.clear();

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);

	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::const_iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr,1);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus QueryListBannedKeyOp::Virtual14()
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
		unsigned short aListSize = theData.ReadShort();
		BANNED_KEY_PAIR aBannedKey;
		for(unsigned short i = 0; i < aListSize; i++)
		{
			aBannedKey.first = theData.ReadLong();
			aBannedKey.second = theData.ReadShort();
			mKeyDataList.push_back(aBannedKey);
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
RemoveBannedKeyOp::RemoveBannedKeyOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mCommunityName(L""),
	  mProductName("")
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveBannedKeyOp::Virtual18()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveBannedKeyOp::Virtual04()
{
	mMessageType = 1002;
	mSubMessageType = MSGTYPE;
	mStatusList.clear();

	// Stores the message data
	WriteBuffer requestData(0,true);
	requestData.AppendWString(mCommunityName,2);
	requestData.AppendString(mProductName);

	requestData.AppendShort(mKeyList.size());
	for(std::list<std::string>::const_iterator anItr = mKeyList.begin(); anItr != mKeyList.end(); ++anItr)
		requestData.AppendString(*anItr,1);

	// Copy buffer into the class
	mRequestData = requestData.ToByteBuffer(true);

	DBProxyOp::Virtual04();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RemoveBannedKeyOp::Virtual14()
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
		unsigned short aListSize = theData.ReadShort();
		for(unsigned short i = 0; i < aListSize; i++)
			mStatusList.push_back(theData.ReadShort());
	}
	catch (ReadBufferException&)
	{
		return (WONStatus)100006;
	}

	// Everything checked out
	return (WONStatus)0;
}
