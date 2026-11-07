// Native account creation exchange through DBProxy service.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONElGamal.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006cbe70
void CreateAccountOp::Init()
{
    mLengthFieldSize=4;
    mProfileData=new ProfileData;
    mSymKey.Create(8);
}
// FUNCTION: WON 0x006cbf20
CreateAccountOp::CreateAccountOp(ServerContext* inContext)
    : ServerRequestOp(inContext)
{
    Init();
}
// FUNCTION: WON 0x006cc4e0
void CreateAccountOp::Virtual18()
{
    mState=State_GettingPubKey;
}
// FUNCTION: WON 0x006cc4f0
WONStatus CreateAccountOp::Virtual10()
{
    WriteBuffer request(mLengthFieldSize,true);
    if (mState==State_GettingPubKey) {
        request.AppendByte(5);
        request.AppendShort(14);
        request.AppendShort(2);
        request.AppendShort(1);
        mRequest=request.ToByteBuffer(true);
        return 100059;
    } else if (mState==State_SettingSessionKey) {
        request.AppendByte(5);
        request.AppendShort(14);
        request.AppendShort(4);
        request.AppendShort(1);
        ByteBufferPtr encryptedKey=mPubKey.Encrypt(mSymKey.GetKey(),mSymKey.GetKeyLen());
        request.AppendBuffer(encryptedKey.get(),2);
        mRequest=request.ToByteBuffer(true);
        mState=State_CreatingAccount;
        return 100058;
    } else {
        request.AppendByte(5);
        request.AppendShort(14);
        request.AppendShort(5);
        request.AppendShort(1);
        WriteBuffer account(0,true);
        account.AppendByte(5);
        account.AppendShort(14);
        account.AppendShort(1002);
        account.AppendShort(2);
        account.AppendWString(mUserName,2);
        account.AppendWString(mPassword,2);
        account.AppendString(mEmail,2);
        mProfileData->PackRequestData(account,true);
        ByteBufferPtr encrypted=mSymKey.Encrypt(account.data(),account.length());
        request.AppendBuffer(encrypted.get(),0);
        mRequest=request.ToByteBuffer(true);
        return 100059;
    }
}
// FUNCTION: WON 0x006cc8d0
WONStatus CreateAccountOp::Virtual14()
{
    ReadBuffer reply(mResponse->data(),mResponse->length());
    unsigned char header=reply.ReadByte();
    unsigned short service=reply.ReadShort();
    unsigned short message=reply.ReadShort();
    unsigned short subMessage=reply.ReadShort();
    WONStatus status=reply.ReadShort();
    reply.ReadString(mErrorString,2);
    reply.ReadByte();
    if (header!=5 || service!=14) return InvalidReplyHeader();
    if (status!=0) return status;
    if (mState==State_GettingPubKey) {
        if (message!=3) return InvalidReplyHeader();
        unsigned short keyLength=reply.ReadShort();
        if (!mPubKey.SetPublicKey(reply.ReadBytes(keyLength),keyLength)) {
            SetLastServerError(100011);
            return 100057;
        }
        mState=State_SettingSessionKey;
        return 100058;
    } else {
        if (message!=1002 || subMessage!=3) return InvalidReplyHeader();
        return status;
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cc010
// (public: __thiscall WONAPI::ElGamal::~ElGamal(void)    [??1ElGamal@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006cc140
// (public: __thiscall WONAPI::Random::~Random(void)    [??1Random@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006cc230
// (public: __thiscall WONAPI::Blowfish::~Blowfish(void)    [??1Blowfish@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006cc2e0
// (public: virtual __thiscall WONAPI::CreateAccountOp::~CreateAccountOp(void)    [??1CreateAccountOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cc210
// (public: virtual void * __thiscall WONAPI::CreateAccountOp::`scalar deleting destructor'(unsigned int)    [??_GCreateAccountOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ced50
// (private: void __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::_Tidy(bool)    )
