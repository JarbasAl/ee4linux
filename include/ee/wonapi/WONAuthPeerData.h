// Complete actual AuthPeerData1c8 owner; constructors remain original-owned/carved.
#ifndef EE_WONAPI_WONAUTHPEERDATA_H
#define EE_WONAPI_WONAUTHPEERDATA_H
#include "ee/wonapi/WONAuthCertificate.h"
#include "ee/wonapi/WONAuthPubKeyBlock.h"
namespace WONAPI {
class AuthPeerData : public RefCount {
    ConstSmartPtr<AuthCertificate> mCertificate; //08
    ConstSmartPtr<Auth2Certificate> mCertificate2; //0c
    ConstSmartPtr<AuthPubKeyBlock> mKeyBlock; //10; natural alignment14..17
    ElGamal mPrivateKey; //18..1c0
    time_t mAuthDelta; //1c0; natural1c4..1c8
protected:
    virtual ~AuthPeerData(); //2e2140, actual own-vptr then complete owners, support0
public:
    AuthPeerData(const AuthPubKeyBlock* = NULL); //2e1b60 external constructor; no new allocation here
    AuthPeerData(const AuthCertificate*,const AuthPubKeyBlock*,const ElGamal&,time_t,const Auth2Certificate* = NULL); //2e1ee0 external
    const AuthPubKeyBlock* GetPubKeyBlock() const { return mKeyBlock.get(); } //genuine SDK borrowed accessor; native GetCert2e3faf reads10
    time_t GetAuthDelta() const { return mAuthDelta; } //genuine SDK accessor; native GetCert2e3ff2 reads1c0
    const AuthCertificate* GetCertificate() const { return mCertificate.get(); } //genuine SDK/native08 borrowed
    const ElGamal& GetPrivateKey() const { return mPrivateKey; } //genuine SDK/native18 borrowed
    bool Verify(const AuthBase*) const; //2e2290, support forwarding
    bool CertificateExpired(time_t = 0) const; //2e22a0, support forwarding
    bool Certificate2Expired(time_t = 0) const; //2e22c0, support forwarding
    bool IsValid() const; //2e22e0
    bool IsValid2() const;
    time_t GetExpireDelta() const; //2e2310
    const Auth2Certificate* GetCertificate2() const { return mCertificate2.get(); } //actual SDK borrowed accessor, nativefield0c
};
}
#endif
