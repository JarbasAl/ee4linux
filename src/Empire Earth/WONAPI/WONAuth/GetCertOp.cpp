// Original authentication request operation; external protocol helpers retain their real contracts.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
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
// FUNCTION: WON 0x006e3c30
GetCertOp::GetCertOp(AuthContext* context)
    :ServerRequestOp(context->GetServerContext().get()),mAuthContext(context)
{
    mCreateAccount=false;
}
// FUNCTION: WON 0x006e3d90
GetCertOp::~GetCertOp()
{
}

// FUNCTION: WON 0x006e3f40
void GetCertOp::Virtual04()
{
    if(!mAuthContext->GetVerifierKey().IsPublic())
    {
        Fn29F220(100070); // WS_GetCert_NeedVerifierKey
        return;
    }

    ConstSmartPtr<AuthPeerData> peerData=mAuthContext->GetPeerData();
    mKeyBlock=peerData->GetPubKeyBlock();
    mAuthDelta=peerData->GetAuthDelta();
    mAuthContext->LoadSecretList();
    ServerRequestOp::Virtual04();
}

// FUNCTION: WON 0x006e4270
WONStatus GetCertOp::SendPubKeyRequest()
{
    mState=GETTING_PUB_KEYS;
    WriteBuffer msg(4,true);
    msg.AppendByte(5);
    msg.AppendShort(12);
    msg.AppendShort(1);
    mRequest=msg.ToByteBuffer(true);
    return 100059; // WS_ServerReq_Recv
}

// FUNCTION: WON 0x006e4380
WONStatus GetCertOp::SendLoginRequest()
{
    WriteBuffer msg(4,true);
    msg.AppendByte(5);
    msg.AppendShort(12);
    msg.AppendShort(3);
    msg.AppendShort(mKeyBlock->GetId());

    const ElGamal& pubKey=mKeyBlock->GetCurrentKey();
    mSessionKey.Create(8);
    ByteBufferPtr encryptedSessionKey=pubKey.Encrypt(mSessionKey.GetKey(),mSessionKey.GetKeyLen());
    if(encryptedSessionKey.get()==NULL)
        return 100065; // WS_GetCert_PubKeyEncryptFailure

    msg.AppendShort(encryptedSessionKey->length());
    msg.AppendBytes(encryptedSessionKey->data(),encryptedSessionKey->length());

    WriteBuffer loginData(0,true);
    loginData.AppendBytes(mSessionKey.GetKey(),2);
    loginData.AppendShort(mKeyBlock->GetId());
    int flags=0x0002|0x0004|0x0008;
    if(mCreateAccount)
        flags|=0x0001;
    loginData.AppendLong(flags);
    loginData.AppendWString(mAuthContext->GetUserName(),2);
    loginData.AppendWString(mAuthContext->GetPassword(),2);
    loginData.AppendWString(mNewPassword,2);
    mAuthContext->AppendCommunityData(loginData);

    ByteBufferPtr encryptedLoginData=mSessionKey.Encrypt(loginData.mData,loginData.mLength);
    msg.AppendShort(encryptedLoginData->length());
    msg.AppendBytes(encryptedLoginData->data(),encryptedLoginData->length());
    mRequest=msg.ToByteBuffer(true);
    mState=GETTING_CHALLENGE;
    return 100059; // WS_ServerReq_Recv
}

// FUNCTION: WON 0x006e46a0
WONStatus GetCertOp::SendLoginConfirm()
{
    WriteBuffer msg(4,true);
    msg.AppendByte(5);
    msg.AppendShort(12);
    msg.AppendShort(5);

    WriteBuffer dataBlock(0,true);
    mAuthContext->AppendHashes(dataBlock,mChallengeSeed);
    ByteBufferPtr encrypted=mSessionKey.Encrypt(dataBlock.mData,dataBlock.mLength);
    msg.AppendShort(encrypted->length());
    msg.AppendBytes(encrypted->data(),encrypted->length());
    mRequest=msg.ToByteBuffer(true);
    return 100059; // WS_ServerReq_Recv
}
namespace {
enum {
    WS_GetCert_DecryptFailure=100074,
    WS_GetCert_InvalidCertificate=100072,
    WS_GetCert_InvalidPrivateKey=100075,
    WS_GetCert_InvalidPubKeyBlock=100073,
    WS_GetCert_InvalidSecretConfirm=100076,
    WS_GetCert_MissingCertificate=100077,
    WS_GetCert_UnexpectedLoginChallenge=100069,
    WS_ServerReq_Send=100058,
    WS_ServerReq_TryNextServer=100057,
    WS_Success=0
};
}

// FUNCTION: WON 0x006e4050
void GetCertOp::Virtual08()
{
	// Give the pub key block to the AuthContext if we managed to at least get that
	if(mStatus!=0 && mKeyBlock->IsValid() && !mAuthContext->GetPeerData()->GetPubKeyBlock()->IsValid())
		mAuthContext->SetPeerData(new AuthPeerData(mKeyBlock));

	ServerRequestOp::Virtual08();
}

// FUNCTION: WON 0x006e4130
void GetCertOp::Virtual18()
{
	mState = GETTING_PUB_KEYS;
	mChallengeSeed.erase();
	mErrorList.clear();
	mCommunityStatusList.clear();
}

// FUNCTION: WON 0x006e4850
WONStatus GetCertOp::Virtual10()
{	
 	if(!mKeyBlock->IsValid()) 
		return SendPubKeyRequest();
	else if(mState==GETTING_CHALLENGE)
		return SendLoginConfirm();
	else
		return SendLoginRequest();
}

// FUNCTION: WON 0x006e4880
WONStatus GetCertOp::HandleLoginReply(ReadBuffer &theMsg)
{

	ConstSmartPtr<AuthCertificate> aCert1;
	ConstSmartPtr<Auth2Certificate> aCert2;
	ElGamal aPrivKey;
	time_t anAuthDelta = 0;

	WONStatus aStatus = (WONStatus)theMsg.ReadShort();
	if(aStatus<0) // Failure status
	{
		if(!mChallengeSeed.empty())
			mAuthContext->RemoveSecret(mChallengeSeed.data());

		if(aStatus==-1) // unknown auth error
		{
			SetLastServerError(aStatus);
			return WS_ServerReq_TryNextServer;
		}

//		return aStatus;
	}
	
	unsigned char anErrorCount = theMsg.ReadByte();
	unsigned long i;
	const char *data;
	for(i=0; i<anErrorCount; i++)
	{
		std::string anErrorString;
		theMsg.ReadString(anErrorString);
		mErrorList.push_back(anErrorString);
	}
	
	unsigned char aNumClearEntries = theMsg.ReadByte();

	for(i=0; i<aNumClearEntries; i++)
	{
		unsigned char aType = theMsg.ReadByte();
		unsigned short aLen = theMsg.ReadShort();
		
		data = (const char*)theMsg.ReadBytes(aLen);
		switch(aType)
		{
			case 1:	
			{
				aCert1 = new AuthCertificate(data,aLen);
				if(!aCert1->IsValid() || !mKeyBlock->Verify(aCert1)) 
				{
					SetLastServerError(WS_GetCert_InvalidCertificate);
					return WS_ServerReq_TryNextServer; 
				}
				else
					anAuthDelta = aCert1->ComputeAuthDelta();
			}
			break;
			
			case 3: 
			{
				ConstSmartPtr<AuthPubKeyBlock> aKeyBlock = new AuthPubKeyBlock(data,aLen);
				if(!aKeyBlock->IsValidAndVerified(mAuthContext->GetVerifierKey()))
				{
					SetLastServerError(WS_GetCert_InvalidPubKeyBlock);
					return WS_ServerReq_TryNextServer;
				}
				else
					mKeyBlock = aKeyBlock;	
			}
			break;

			case 6:
			{
				aCert2 = new Auth2Certificate(data,aLen);
				if(!aCert2->IsValid() || !mKeyBlock->Verify(aCert2))
				{
					SetLastServerError(WS_GetCert_InvalidCertificate);
					return WS_ServerReq_TryNextServer; 
				}
				else
					anAuthDelta = aCert2->ComputeAuthDelta();
			}
			break;

			case 7:
			{
				ReadBuffer aSubMsg(data,aLen);
				mCommunityStatusList.push_back(COMMUNITY_STATUS());
				COMMUNITY_STATUS &aCommunityStatus = mCommunityStatusList.back();

				aCommunityStatus.mStatus = aSubMsg.ReadShort();
				aCommunityStatus.mCommunityID = aSubMsg.ReadLong();
				aSubMsg.ReadWString(aCommunityStatus.mCommunityName,2);
			}
			break;
		}
	}
	
	if(aStatus<0)
		return aStatus;

	// Commit secrets and clean up secret file using auth server clock
	time_t anAuthTime = 0, aCertDuration;
	if(aCert2.get()!=NULL)
	{
		anAuthTime = aCert1->GetIssueTime();
		aCertDuration = aCert1->GetExpireTime() - aCert1->GetIssueTime();
	}
	else if(aCert1.get()!=NULL)
	{
		anAuthTime = aCert2->GetIssueTime();
		aCertDuration = aCert2->GetExpireTime() - aCert2->GetIssueTime();
	}

	mAuthContext->CommitSecrets(anAuthTime, aCertDuration);

	
	int aNumLeft = theMsg.length() - theMsg.mL00;

	ByteBufferPtr aDecryptBuf = mSessionKey.Decrypt(theMsg.ReadBytes(aNumLeft),aNumLeft);
	if(aDecryptBuf.get()==NULL)
	{
		SetLastServerError(WS_GetCert_DecryptFailure);
		return WS_ServerReq_TryNextServer;
	}
	
	ReadBuffer aDecrypt(aDecryptBuf->data(),aDecryptBuf->length());		
	unsigned char aNumCryptEntries = aDecrypt.ReadByte();
	
	for(i=0; i<aNumCryptEntries; i++)
	{
		unsigned char aType = aDecrypt.ReadByte();
		unsigned short aLen = aDecrypt.ReadShort();
		data = (const char*)aDecrypt.ReadBytes(aLen);
		switch(aType)
		{
			case 2: 
				if(!aPrivKey.SetPrivateKey(data,aLen))
				{
					SetLastServerError(WS_GetCert_InvalidPrivateKey);
					return WS_ServerReq_TryNextServer;
				}
				break;

			case 4:
				if((aLen!=mSessionKey.GetKeyLen()+2) || memcmp(data+2,mSessionKey.GetKey(),aLen-2)!=0)
				{
					SetLastServerError(WS_GetCert_InvalidSecretConfirm);
					return WS_ServerReq_TryNextServer;
				}
				break;

			case 5: 
				//ReadNickNameInfo(aDecryptIn);
				break;
		}
	}

	if(aCert1.get()==NULL || aCert2.get()==NULL)
	{
		SetLastServerError(WS_GetCert_MissingCertificate);
		return WS_ServerReq_TryNextServer;
	}

	mPeerData = new AuthPeerData(aCert1, mKeyBlock, aPrivKey, anAuthDelta, aCert2);
	if(!mNewPassword.empty())
		mAuthContext->SetPassword(mNewPassword);

	mAuthContext->SetPeerData(mPeerData.get());	
	mAuthContext->ClearOneTimeData();
	return WS_Success;
}

// FUNCTION: WON 0x006e5800
WONStatus GetCertOp::HandleLoginChallenge(ReadBuffer &theMsg)
{
	unsigned short aLen = theMsg.ReadShort();
	ByteBufferPtr aDecrypt = mSessionKey.Decrypt(theMsg.ReadBytes(aLen),aLen);
	if(aDecrypt.get()==NULL)
	{
		SetLastServerError(WS_GetCert_DecryptFailure);
		return WS_ServerReq_TryNextServer;
	}

	ReadBuffer aBuf(aDecrypt->data(),aDecrypt->length());
	mChallengeSeed.assign((const unsigned char*)aBuf.ReadBytes(16),16);
	mAuthContext->AddSecret(mChallengeSeed.data());
	return WS_ServerReq_Send;
}

// FUNCTION: WON 0x006e5980
WONStatus GetCertOp::Virtual14()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadShort();
	unsigned short aMessageType = aMsg.ReadShort();

	if(aHeaderType!=5 || aServiceType!=12)
		return InvalidReplyHeader();

	if(mState==GETTING_PUB_KEYS)
	{
		if(aMessageType!=2) // GetPubKeysReply
			return InvalidReplyHeader();

		WONStatus aStatus = (WONStatus)aMsg.ReadShort();
		if(aStatus<0) // shouldn't get this
		{
			SetLastServerError(aStatus);
			return WS_ServerReq_TryNextServer;
		}

		unsigned short aLen = aMsg.ReadShort();
		ConstSmartPtr<AuthPubKeyBlock> aKeyBlock = new AuthPubKeyBlock(aMsg.ReadBytes(aLen),aLen);
		if(!aKeyBlock->IsValidAndVerified(mAuthContext->GetVerifierKey()))
		{
			SetLastServerError(WS_GetCert_InvalidPubKeyBlock);
			return WS_ServerReq_TryNextServer;
		}
		else
		{
			mKeyBlock = aKeyBlock;
			return WS_ServerReq_Send;
		}
	}

	
	
	if(aMessageType==6)
		return HandleLoginReply(aMsg);
	
	if(aMessageType!=4)
		return InvalidReplyHeader();

	if(mState!=GETTING_CHALLENGE)
	{
		SetLastServerError(WS_GetCert_UnexpectedLoginChallenge);
		return WS_ServerReq_TryNextServer;
	}

	return HandleLoginChallenge(aMsg);
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e3d70
// (protected: virtual void * __thiscall WONAPI::GetCertOp::`scalar deleting destructor'(unsigned int)    [??_GGetCertOp@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006e57c0
// (public: __thiscall WONAPI::GetCertOp::COMMUNITY_STATUS::~COMMUNITY_STATUS(void)    [??1COMMUNITY_STATUS@GetCertOp@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e5760
// (public: __thiscall WONAPI::AuthCertificateBase::AuthCertificateBase(void const *,unsigned long)    [??0AuthCertificateBase@WONAPI@@QAE@PBXK@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e9820
// (public: __thiscall std::list<class WONAPI::ElGamal,class std::allocator<class WONAPI::ElGamal> >::list<class WONAPI::ElGamal,class std::allocator<clas)
// TEMPLATE: WON 0x006ea0d0
// (public: __thiscall std::vector<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<class std:)
// TEMPLATE: WON 0x006ebdd0
// (public: class std::list<struct WONAPI::GetCertOp::COMMUNITY_STATUS,class std::allocator<struct WONAPI::GetCertOp::COMMUNITY_STATUS> >::iterator __this)
// TEMPLATE: WON 0x006ef690
// (protected: void __thiscall std::vector<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<cl)
// TEMPLATE: WON 0x006f00f0
// (public: void * __thiscall WONAPI::GetCertOp::COMMUNITY_STATUS::`scalar deleting destructor'(unsigned int)    [??_GCOMMUNITY_STATUS@GetCertOp@WONAPI@@Q)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ebd70
// (public: void __thiscall std::list<struct WONAPI::GetCertOp::COMMUNITY_STATUS,class std::allocator<struct WONAPI::GetCertOp::COMMUNITY_STATUS> >::_Inse)
// TEMPLATE: WON 0x006ef6e0
// (protected: void __thiscall std::list<struct WONAPI::GetCertOp::COMMUNITY_STATUS,class std::allocator<struct WONAPI::GetCertOp::COMMUNITY_STATUS> >::_I)
// TEMPLATE: WON 0x006efca0
// (void __cdecl std::_Construct(struct WONAPI::GetCertOp::COMMUNITY_STATUS *,struct WONAPI::GetCertOp::COMMUNITY_STATUS const &))
