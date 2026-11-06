// Complete actual peer authentication server wire protocol; true original owners.
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
// FUNCTION: WON 0x006e7d30
void PeerAuthServer::Start(const AuthPeerData* data,unsigned char lengthFieldSize)
{
    mPeerData=data;
    mLengthFieldSize=lengthFieldSize;
    mState=STATE_AWAITING_REQUEST;
}

// FUNCTION: WON 0x006e7dd0
WONStatus PeerAuthServer::GetChallenge1(ByteBufferPtr& challenge)
{
    WriteBuffer msg(mLengthFieldSize,true);
    msg.AppendLong(203);
    msg.AppendLong(51);
    mSecretB.Create(8);
    WriteBuffer secret(0,true);
    secret.AppendShort(mSecretB.GetKeyLen());
    secret.AppendBytes(mSecretB.GetKey(),mSecretB.GetKeyLen());

    ByteBufferPtr encrypted=mClientCertificate->GetPubKey().Encrypt(secret.mData,secret.mLength);
    if(encrypted.get()==NULL)
        return 100049; // WS_PeerAuthServer_FailedToEncryptWithClientPubKey

    msg.AppendShort(encrypted->length());
    msg.AppendBytes(encrypted->data(),encrypted->length());
    if(mUseAuth2)
        msg.AppendBuffer(mPeerData->GetCertificate2()->GetRawBuf().get(),2);
    else
        msg.AppendBuffer(mPeerData->GetCertificate()->GetRawBuf().get(),2);
    challenge=msg.ToByteBuffer(true);
    return 0;
}

// FUNCTION: WON 0x006e80a0
WONStatus PeerAuthServer::HandleRequest(ReadBuffer& request,ByteBufferPtr& challenge)
{
    unsigned char authMode=request.ReadByte();
    unsigned char encryptType=request.ReadByte();
    request.ReadShort(); // encryption flags are consumed even though this version ignores them
    if(authMode!=1)
        return 100044; // WS_PeerAuthServer_InvalidAuthMode
    if(encryptType!=0 && encryptType!=1)
        return 100045; // WS_PeerAuthServer_InvalidEncryptType
    bool encrypted=encryptType==1;
    mAuthType=encrypted?AUTH_TYPE_PERSISTENT:AUTH_TYPE_PERSISTENT_NOCRYPT;

    unsigned short len=request.ReadShort();
    if(mUseAuth2)
        mClientCertificate=new Auth2Certificate(request.ReadBytes(len),len);
    else
        mClientCertificate=new AuthCertificate(request.ReadBytes(len),len);

    if(!mClientCertificate->IsValid())
        return 100046; // WS_PeerAuthServer_InvalidClientCertificate
    if(mClientCertificate->IsExpired(mPeerData->GetAuthDelta()))
        return 100047; // WS_PeerAuthServer_ExpiredClientCertificate
    if(!mPeerData->Verify(mClientCertificate.get()))
        return 100048; // WS_PeerAuthServer_FailedToVerifyClientCertificate
    return GetChallenge1(challenge);
}

// FUNCTION: WON 0x006e8300
WONStatus PeerAuthServer::GetComplete(WONStatus status,ByteBufferPtr& complete)
{
    mState=STATE_NOT_STARTED;
    WriteBuffer msg(mLengthFieldSize,true);
    msg.AppendLong(203);
    msg.AppendLong(53);
    if(status!=0)
    {
        msg.AppendShort(-4); // WS_CommServ_InvalidParameters
        msg.AppendShort(1);
        msg.AppendString(WONStatusToString(status));
    }
    else
    {
        msg.AppendShort(0);
        WriteBuffer secret(0,true);
        secret.AppendShort(mSecretA.GetKeyLen());
        secret.AppendBytes(mSecretA.GetKey(),mSecretA.GetKeyLen());
        ByteBufferPtr encrypted=mClientCertificate->GetPubKey().Encrypt(secret.mData,secret.mLength);
        if(encrypted.get()==NULL)
            return 100049; // WS_PeerAuthServer_FailedToEncryptWithClientPubKey
        msg.AppendShort(encrypted->length());
        msg.AppendBytes(encrypted->data(),encrypted->length());
        mSession=new AuthSession(mAuthType,0,mSecretB,mLengthFieldSize);
    }
    complete=msg.ToByteBuffer(true);
    return 0;
}

// FUNCTION: WON 0x006e8630
WONStatus PeerAuthServer::HandleChallenge2(ReadBuffer& challenge,ByteBufferPtr& complete)
{
    unsigned short encryptedLen=challenge.ReadShort();
    ByteBufferPtr decrypted=mPeerData->GetPrivateKey().Decrypt(challenge.ReadBytes(encryptedLen),encryptedLen);
    if(decrypted.get()==NULL)
        return 100050; // WS_PeerAuthServer_FailedToDecryptWithPrivateKey
    ReadBuffer msg(decrypted->data(),decrypted->length());
    unsigned short secretBLen=msg.ReadShort();
    if(secretBLen!=mSecretB.GetKeyLen() || memcmp(mSecretB.GetKey(),msg.ReadBytes(secretBLen),secretBLen)!=0)
        return 100052; // WS_PeerAuthServer_InvalidSecretB
    if(!mSecretA.SetKey(msg.mData+msg.mL00,msg.mL08-msg.mL00))
        return 100051; // WS_PeerAuthServer_InvalidSecretA
    return GetComplete(0,complete);
}

// FUNCTION: WON 0x006e87c0
WONStatus PeerAuthServer::HandleRecvMsg(const void* data,unsigned long len,ByteBufferPtr& output)
{
    output=NULL;
    if(mState==STATE_NOT_STARTED)
        return 100038; // WS_PeerAuthServer_NotStarted
    WONStatus status;
    try
    {
        ReadBuffer msg(data,len);
        DWORD serviceType=msg.ReadLong();
        DWORD messageType=msg.ReadLong();
        if(serviceType!=203)
            return 100039; // WS_PeerAuthServer_InvalidServiceType
        if(messageType==50)
        {
            if(mState!=STATE_AWAITING_REQUEST)
                return 100040; // WS_PeerAuthServer_UnexpectedRequest
            status=HandleRequest(msg,output);
            mState=STATE_AWAITING_CHALLENGE2;
        }
        else if(messageType==52)
        {
            if(mState!=STATE_AWAITING_CHALLENGE2)
                return 100041; // WS_PeerAuthServer_UnexpectedChallenge
            status=HandleChallenge2(msg,output);
        }
        else
            return 100042; // WS_PeerAuthServer_InvalidMessage
    }
    catch(ReadBufferException&)
    {
        status=100043; // WS_PeerAuthServer_MsgUnpackFailure
    }
    if(status!=0)
        GetComplete(status,output);
    return status;
}

// SUPPORT: EXE 0x006e7cc0 (106B genuine SDK constructor, zero primary quota)
PeerAuthServer::PeerAuthServer()
{
    mState=STATE_NOT_STARTED;
    mUseAuth2=true;
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006e7cc0
// (public: __thiscall WONAPI::PeerAuthServer::PeerAuthServer(void)    [??0PeerAuthServer@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006e8060
// (public: class WONAPI::SmartPtr<class WONAPI::ByteBuffer> __thiscall WONAPI::AuthBase::GetRawBuf(void)const     [?GetRawBuf@AuthBase@WONAPI@@QBE?AV?$Sm)
