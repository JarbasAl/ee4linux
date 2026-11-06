// Complete original-backed certificate storage/virtual contracts. Constructors remain original-owned;
// this packet implements bounded unpack/check methods and actual owned destruction only.
#ifndef EE_WONAPI_WONAUTHCERTIFICATE_H
#define EE_WONAPI_WONAUTHCERTIFICATE_H
#include <map>
#include <string>
#include "ee/wonapi/WONAuthBase.h"
#include "ee/wonapi/WONElGamal.h"
namespace WONAPI {
class AuthCertificateBase : public AuthBase {
protected:
    unsigned long mUserId; //1c
    ElGamal mPubKey; //20..1c8, actual native alignment
public:
    AuthCertificateBase(const void* data = NULL, unsigned long len = 0)
        :AuthBase(data,len),mUserId(0) {} // inline: AuthPeerData 0x6e1b60 expands it; 0x6e5760 is a refused copy
    const ElGamal& GetPubKey() const { return mPubKey; } //genuine SDK/native20 borrowed
    unsigned long GetUserId() const { return mUserId; } //genuine SDK existing1c scalar accessor; Source-only support
    virtual bool VerifyPermissions(DWORD, DWORD, unsigned short) const; //actual derived slot8, base fallback remains external
    // Native implicit destructor2dd3d0: six BigInteger+Random owners then AuthBase; support0.
};
class AuthCertificate : public AuthCertificateBase {
    unsigned long mCommunityId; //1c8
    unsigned short mTrustLevel; //1cc; natural extent1d0
protected:
    virtual ~AuthCertificate(); //2dcfb0, real own-vptr store then complete member/base cleanup; support0
    virtual bool UnpackHook(ReadBuffer&); //2dd120
public:
    AuthCertificate(const void* data = NULL, unsigned long len = 0)
        :AuthCertificateBase(data,len),mCommunityId(0),mTrustLevel(0) { Unpack(); } //genuine SDK inline, native2e8214..2e8241; support0
    time_t ComputeAuthDelta() const { return GetIssueTime() - time(NULL); } //genuine SDK inline; native GetCert reply shares issue-time minus current-time block
    virtual bool VerifyPermissions(DWORD, DWORD, unsigned short) const; //2dd190 discoverygap
};
typedef std::map<DWORD, unsigned short> CommunityTrustMap;
typedef std::map<std::wstring, std::wstring> NicknameMap;
typedef std::map<DWORD, ByteBufferPtr> UserDataMap;
typedef std::map<DWORD, DWORD> KeyIdMap;
class Auth2Certificate : public AuthCertificateBase {
    std::wstring mUserName; //1c8..1d8
    CommunityTrustMap mCommunityTrustMap; //1d8..1e4
    NicknameMap mNicknameMap; //1e4..1f0
    UserDataMap mUserDataMap; //1f0..1fc
    KeyIdMap mKeyIdMap; //1fc..208, complete extent208
protected:
    virtual ~Auth2Certificate(); //2dd1d0, complete owner cleanup, support0
    virtual bool UnpackHook(ReadBuffer&); //2dd6b0 discoverygap; code918, table16 excluded
public:
    Auth2Certificate(const void* data = NULL, unsigned long len = 0)
        :AuthCertificateBase(data,len) { Unpack(); } //genuine SDK inline, native2e8171..2e81e8 full real owners; support0
    time_t ComputeAuthDelta() const { return GetIssueTime() - time(NULL); } //genuine SDK inline; complete certificate owner, borrowed timestamp only
    DWORD GetFirstCommunity() const; //2ddb50/25 actual cached native count/head/firstkey getter, call-only0quota
    std::wstring GetNickname(const std::wstring&) const;
    ByteBufferPtr GetUserData(DWORD) const;
    DWORD GetKeyId(DWORD) const;
    std::wstring GetFirstNickname() const;
    unsigned short GetFirstCommunityTrust() const;
    ByteBufferPtr GetFirstUserData() const;
    virtual bool VerifyPermissions(DWORD, DWORD, unsigned short) const; //2dda60
};
}
#endif
