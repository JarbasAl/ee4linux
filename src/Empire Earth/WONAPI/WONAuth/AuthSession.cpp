// Original-backed bounded cryptography implementation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <string.h>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include <time.h>
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONBlowfish.h"
namespace WONAPI {
// FUNCTION: WON 0x006e27b0
AuthSession::AuthSession(AuthType theType, unsigned short theId, Blowfish theKey, 
						 unsigned char theLengthFieldSize) : mAuthType(theType), mId(theId), 
						 mKey(theKey), mLengthFieldSize(theLengthFieldSize), mInSeq(0), mOutSeq(0) 
{
	mLastUseTime = time(NULL);
}

// FUNCTION: WON 0x006e2a80
AuthSession::~AuthSession()
{
}

// FUNCTION: WON 0x006e2b40
WONStatus AuthSession::Encrypt(ByteBufferPtr &theMsg)
{
	mLastUseTime = time(NULL);

	if(mAuthType==AUTH_TYPE_NONE || mAuthType==AUTH_TYPE_PERSISTENT_NOCRYPT)
		return 0; // WS_Success

	WriteBuffer aMsg(mLengthFieldSize, true);
	aMsg.AppendByte(12); // encrypted message

	int anOffset = mLengthFieldSize;
	if(anOffset==2 && theMsg->length()>=0xffff) // extended 2-byte length
		anOffset = 6;

	WriteBuffer aSeqBuf(0, true);
	const char *aBuf = theMsg->data() + anOffset;
	unsigned long aLen = theMsg->length() - anOffset;

	if(mAuthType==AUTH_TYPE_SESSION)
	{
		aMsg.AppendShort(mId);
		aSeqBuf.AppendShort(++mOutSeq);
		aSeqBuf.AppendBytes(aBuf,aLen);
		aBuf = aSeqBuf.mData;
		aLen = aSeqBuf.mLength;
	}

	ByteBufferPtr anEncrypt = mKey.Encrypt(aBuf,aLen);
	if(anEncrypt.get()==NULL)
		return 100082; // WS_AuthSession_EncryptFailure
	
	aMsg.AppendBytes(anEncrypt->data(),anEncrypt->length());
	theMsg = aMsg.ToByteBuffer(true);
	return 0; // WS_Success
}

// FUNCTION: WON 0x006e2d60
WONStatus AuthSession::Decrypt(ByteBufferPtr &theMsg)
{
	mLastUseTime = time(NULL);

	try
	{
		if(mAuthType==AUTH_TYPE_NONE || mAuthType==AUTH_TYPE_PERSISTENT_NOCRYPT)
			return 0; // WS_Success

		ReadBuffer anIn(theMsg->data(),theMsg->length());
		WriteBuffer anOut(0, true);
		unsigned char headerType = anIn.ReadByte();
		switch (headerType)
		{
			case 2:							break;	//WONMsg::EncryptedService:
			case 4:	anOut.AppendByte(3);	break;	//WONMsg::MiniEncryptedService:
			case 6:	anOut.AppendByte(5);	break;	//WONMsg::SmallEncryptedService:
			case 8:	anOut.AppendByte(7);	break;	//WONMsg::LargeEncryptedService:
			case 12:						break;	//WONMsg::HeaderEncryptedService:

			default:
				return 0; // WS_Success
		}

		bool sessioned = mAuthType==AUTH_TYPE_SESSION;

		if(sessioned)
		{
			unsigned short aSessionId = anIn.ReadShort();
			if(aSessionId!=mId)
				return 100083; // WS_AuthSession_DecryptSessionIdMismatch
		}

		ByteBufferPtr aDecrypt = mKey.Decrypt(anIn.mData + anIn.mL00, anIn.mL08 - anIn.mL00);
		if(aDecrypt.get()==NULL)
			return 100084; // WS_AuthSession_DecryptFailure

		if(sessioned)
		{
			if(aDecrypt->length()<2)
				return 100085; // WS_AuthSession_DecryptBadLen

			if(++mInSeq!=*(const unsigned short*)aDecrypt->data()) // sequence mismatch
				return 100086; // WS_AuthSession_DecryptInvalidSequenceNum

			anOut.AppendBytes(aDecrypt->data()+2,aDecrypt->length()-2);
		}
		else
			anOut.AppendBytes(aDecrypt->data(),aDecrypt->length());

		theMsg = anOut.ToByteBuffer(true);
		return 0; // WS_Success
	}
	catch(ReadBufferException&)
	{
		return 100087; // WS_AuthSession_DecryptUnpackFailure
	}
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e2a60
// (protected: virtual void * __thiscall WONAPI::AuthSession::`scalar deleting destructor'(unsigned int)    [??_GAuthSession@WONAPI@@MAEPAXI@Z])
