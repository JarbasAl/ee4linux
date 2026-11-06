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
void AddEntityOp::Init()
{
	mFlags = DIR_UF_UNIQUEDISPLAYNAME;
	mLengthFieldSize = 4;
	mLifespan = 0;
	mIsService = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AddEntityOp::AddEntityOp(ServerContext *theDirContext, bool isService) : ServerRequestOp(theDirContext)
{
	Init();
	mIsService = isService;
}
   
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AddEntityOp::AddEntityOp(const IPAddr &theAddr, bool isService) : ServerRequestOp(theAddr)
{
	Init();
	mIsService = isService;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AddServiceOp::SetNetAddrPort(unsigned short thePort)
{
	WriteBuffer theBuf(0,true);
	theBuf.AppendShort( htons(thePort) );

	SetNetAddr(theBuf.ToByteBuffer(true));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus AddEntityOp::Virtual10()
{
	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendByte(5);						// Small message
	aMsg.AppendShort(2);					// Directory Server
	aMsg.AppendShort(mIsService?215:214);	// AddServiceEx : AddDirectoryEx
	aMsg.AppendByte(mFlags);				// Entity Flags
	aMsg.AppendWString(mPath,2);				// Directory path
	aMsg.AppendWString(mName,2);				// Entity name

	if(mIsService)
		aMsg.AppendBuffer(mNetAddr,1);

	aMsg.AppendWString(mDisplayName,2);		// Entity display name
	aMsg.AppendLong(mLifespan);				// Entity lifespan
	aMsg.AppendShort(mDataObjects.size());	// num data objects
	
	DirDataObjectList::iterator anItr = mDataObjects.begin();
	while(anItr!=mDataObjects.end())
	{
		DirDataObject &aDataObject = *anItr;
		aMsg.AppendString(aDataObject.mDataType,1);
		aMsg.AppendBuffer(aDataObject.mData,2);
		++anItr;
	}

	aMsg.AppendShort(0); // num permissions
	mRequest = aMsg.ToByteBuffer(true);
	return (WONStatus)100059;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus AddEntityOp::Virtual14()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadShort();
	unsigned short aMessageType = aMsg.ReadShort();

	if(aHeaderType!=5 || aServiceType!=2 || aMessageType!=1)
		return InvalidReplyHeader();


	short aStatus = aMsg.ReadShort();
	if(aStatus>=0 && mIsService)
	{
		if((mFlags & DIR_UF_SERVRETURNADDR)!=0)
	{
			unsigned char aLen = aMsg.ReadByte();
			mNetAddr = new ByteBuffer(aMsg.ReadBytes(aLen),aLen,false);
		}
	}

	return (WONStatus)aStatus;
}
