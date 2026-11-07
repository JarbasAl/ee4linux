// Actual complete peer-authentication client protocol and original typed owners.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAuthPeerData.h"
namespace WONAPI {
// FUNCTION: WON 0x006e5c30
PeerAuthClient::PeerAuthClient()
{
    Reset();
    mUseAuth2=true;
    mUseClientSecret=false;
    mClientKeySize=8;
}
// FUNCTION: WON 0x006e5cb0
void PeerAuthClient::Reset()
{
    mState=STATE_NOT_STARTED;
}
// FUNCTION: WON 0x6e5cc0
WONStatus PeerAuthClient::HandleChallenge1(ReadBuffer &theChallenge, ByteBufferPtr &challenge2)
{
	unsigned short aSecretLenWithLen = theChallenge.ReadShort();
	ByteBufferPtr aSecretBuf = mPeerData->GetPrivateKey().Decrypt(theChallenge.ReadBytes(aSecretLenWithLen),aSecretLenWithLen);
	if(aSecretBuf.get()==NULL)
		return 100025 /* WS_PeerAuthClient_Challenge1DecryptFailure */;
		
	unsigned short aSecretLen = aSecretBuf->data()[0] | (aSecretBuf->data()[1]<<8);
	if(aSecretLen>aSecretBuf->length()-2)
		return 100026 /* WS_PeerAuthClient_Challenge1InvalidSecretLen */;
		
	mSecretA.Create(mClientKeySize);		
	if(!mSecretB.SetKey(aSecretBuf->data()+2, aSecretLen))
		return 100027 /* WS_PeerAuthClient_Challenge1InvalidSecretKey */;

	unsigned short aCertLen = theChallenge.ReadShort();

	if(mUseAuth2)
		mServerCertificate = new Auth2Certificate(theChallenge.ReadBytes(aCertLen),aCertLen);
	else
		mServerCertificate = new AuthCertificate(theChallenge.ReadBytes(aCertLen),aCertLen);

	if(!mServerCertificate->IsValid())
		return 100028 /* WS_PeerAuthClient_Challenge1CertificateUnpackFailure */;

	if(!mPeerData->Verify(mServerCertificate.get()))
		return 100029 /* WS_PeerAuthClient_Challenge1CertificateVerifyFailure */;

	return GetChallenge2(challenge2);
}
	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x6e5fb0
WONStatus PeerAuthClient::GetChallenge2(ByteBufferPtr &challenge2)
{
	mState = STATE_AWAITING_COMPLETE;

	WriteBuffer aMsg(mLengthFieldSize,true);
		
	aMsg.AppendLong(203);								// Auth1 Peer To Peer
	aMsg.AppendLong(52);								// Auth1 Challenge 2

	WriteBuffer anEncryptBuf(0,true);
	anEncryptBuf.AppendShort(mSecretB.GetKeyLen());
	anEncryptBuf.AppendBytes(mSecretB.GetKey(), mSecretB.GetKeyLen());
	anEncryptBuf.AppendBytes(mSecretA.GetKey(), mSecretA.GetKeyLen());

	ByteBufferPtr anEncrypt = mServerCertificate->GetPubKey().Encrypt(anEncryptBuf.mData,anEncryptBuf.mLength);
	if(anEncrypt.get()==NULL)
		return 100030 /* WS_PeerAuthClient_Challenge2EncryptFailure */;

	aMsg.AppendShort(anEncrypt->length());
	aMsg.AppendBytes(anEncrypt->data(),anEncrypt->length());
	challenge2 = aMsg.ToByteBuffer(true);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x6e61d0
WONStatus PeerAuthClient::HandleComplete(ReadBuffer &theComplete)
{
	short aStatus = theComplete.ReadShort();
	if(aStatus<0)
	{		
		unsigned short aNumErrors = theComplete.ReadShort();
		for(int i=0; i<aNumErrors; i++)
		{
			std::string anError;
			theComplete.ReadString(anError);
		}
		
		return (WONStatus)aStatus;
	}
		
	unsigned short aLen = theComplete.ReadShort();
	ByteBufferPtr aDecrypt = mPeerData->GetPrivateKey().Decrypt(theComplete.ReadBytes(aLen),aLen);
	if(aDecrypt.get()==NULL)
		return 100031 /* WS_PeerAuthClient_CompleteDecryptFailure */;

	if(aDecrypt->length()<2)
		return 100032 /* WS_PeerAuthClient_CompleteInvalidSecretLen */;
	
	aLen = (aDecrypt->data()[0] | (aDecrypt->data()[1]<<8));
	if(aLen>aDecrypt->length()-2 || aLen!=mSecretA.GetKeyLen() || memcmp(mSecretA.GetKey(),aDecrypt->data()+2,aLen)!=0)
		return 100033 /* WS_PeerAuthClient_CompleteInvalidSecretKey */;	

	unsigned short aSessionId = 0;
	if(mAuthType==AUTH_TYPE_SESSION)
		aSessionId = theComplete.ReadShort();

	if(mAuthType!=AUTH_TYPE_PERSISTENT_NOCRYPT)
		mSession = new AuthSession(mAuthType, aSessionId, mUseClientSecret?mSecretA:mSecretB, mLengthFieldSize);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x6e6730
ByteBufferPtr PeerAuthClient::Start(const AuthPeerData *theData, AuthType theType, unsigned char theLengthFieldSize)
{
	mState = STATE_AWAITING_CHALLENGE;

	mPeerData = theData;
	mAuthType = theType;
	mLengthFieldSize = theLengthFieldSize;

	bool sessioned = (mAuthType==AUTH_TYPE_SESSION);
	bool encrypted = (sessioned || mAuthType==AUTH_TYPE_PERSISTENT);

	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendLong(203);								// Auth1 Peer To Peer
	aMsg.AppendLong(50);								// Auth1 Request


	aMsg.AppendByte(sessioned ? 2 : 1);					// 1 = persistent, 2 = sessioned
	aMsg.AppendByte(encrypted ? 1 : 0);					// Encrypt Blowfish

	int anEncryptFlags = 0;
	if(!sessioned) anEncryptFlags |= 0x0001;
	if(mUseClientSecret) anEncryptFlags |= 0x8000;
	aMsg.AppendShort(anEncryptFlags);					// 0x0000 None
														// 0x0001 Don't Use sequence numbers
														// 0x0002 Allow clear messages
														// 0x8000 Use client specified session key

	const AuthCertificateBase *aCert = mPeerData->GetCertificate2();
	if(aCert==NULL || !mUseAuth2)
	{
		mUseAuth2 = false;
		aCert = mPeerData->GetCertificate();
	}

	unsigned short aCertLen = aCert->GetRawLen();
	aMsg.AppendShort(aCertLen);
	aMsg.AppendBytes(aCert->GetRaw(),aCertLen);
	return aMsg.ToByteBuffer(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x6e68d0
WONStatus PeerAuthClient::HandleRecvMsg(const void *inMsg, unsigned long inMsgLen, ByteBufferPtr &outMsg)
{
	outMsg = NULL;

	try
	{
		ReadBuffer aMsg(inMsg,inMsgLen);
		DWORD aServiceType = aMsg.ReadLong();
		DWORD aMessageType = aMsg.ReadLong();

		if(aServiceType!=203) // Auth 1 peer to peer
			return 100034 /* WS_PeerAuthClient_InvalidServiceType */;

		if(aMessageType==53)
			return HandleComplete(aMsg);
		else if(aMessageType==51)
		{
			if(mState!=STATE_AWAITING_CHALLENGE)
				return 100035 /* WS_PeerAuthClient_UnexpectedChallenge */;
			
			return HandleChallenge1(aMsg, outMsg);
		}
		else
			return 100036 /* WS_PeerAuthClient_InvalidMessage */;
	}
	catch(ReadBufferException&)
	{
		return 100037 /* WS_PeerAuthClient_MsgUnpackFailure */;
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e6450
// (public: __thiscall WONAPI::Blowfish::Blowfish(class WONAPI::Blowfish const &))
