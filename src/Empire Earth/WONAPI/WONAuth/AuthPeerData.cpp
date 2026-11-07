// Original-backed AuthPeerData checks on complete true1c8 owners; no constructor allocation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAuthPeerData.h"
#include "LostDebug.h"
namespace WONAPI {
// FUNCTION: WON 0x006e1b60
AuthPeerData::AuthPeerData(const AuthPubKeyBlock* keyBlock)
    :mCertificate(new AuthCertificate),mCertificate2(new Auth2Certificate),
     mKeyBlock(keyBlock),mAuthDelta(0)
{
    // LOST-DEBUG: none: exact only.
    EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1);
    EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1);
    EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1);
    EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1); EE_LOST_TRACE(mAuthDelta+1);
    if(mKeyBlock.get()==NULL) mKeyBlock=new AuthPubKeyBlock;
}
// FUNCTION: WON 0x006e1ee0
AuthPeerData::AuthPeerData(const AuthCertificate* certificate,const AuthPubKeyBlock* keyBlock,
    const ElGamal& key,time_t delta,const Auth2Certificate* certificate2)
    :mCertificate(certificate),mCertificate2(certificate2),mKeyBlock(keyBlock),
     mPrivateKey(key),mAuthDelta(delta)
{
}

// FUNCTION: WON 0x006e2140
AuthPeerData::~AuthPeerData() {}
// FUNCTION: WON 0x006e2290
bool AuthPeerData::Verify(const AuthBase* base) const { return mKeyBlock->Verify(base); }
// FUNCTION: WON 0x006e22a0
bool AuthPeerData::CertificateExpired(time_t extra) const { return mCertificate->IsExpired(extra+mAuthDelta); }
// FUNCTION: WON 0x006e22c0
bool AuthPeerData::Certificate2Expired(time_t extra) const { return mCertificate2->IsExpired(extra+mAuthDelta); }
// FUNCTION: WON 0x006e22e0
bool AuthPeerData::IsValid() const
{
    return mKeyBlock->IsValid() && mCertificate->IsValid() && mPrivateKey.IsPrivate();
}
// FUNCTION: WON 0x006e2310
time_t AuthPeerData::GetExpireDelta() const
{
    if(CertificateExpired()) return 0;
    return mCertificate->GetExpireTime()-mAuthDelta-time(NULL);
}
}

namespace WONAPI {


bool AuthPeerData::IsValid2() const
{
	return (mKeyBlock->IsValid() && mCertificate2->IsValid() && mPrivateKey.IsPrivate());
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e1ea0
// (protected: virtual void * __thiscall WONAPI::AuthPubKeyBlock::`scalar deleting destructor'(unsigned int)    [??_GAuthPubKeyBlock@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006e1ec0
// (protected: virtual void * __thiscall WONAPI::AuthPeerData::`scalar deleting destructor'(unsigned int)    [??_GAuthPeerData@WONAPI@@MAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e1df0
// (public: __thiscall std::map<unsigned long,unsigned long,struct std::less<unsigned long>,class std::allocator<struct std::pair<unsigned long const ,uns)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e1dd0
// (public: virtual void * __thiscall WONAPI::AuthCertificateBase::`scalar deleting destructor'(unsigned int)    [??_GAuthCertificateBase@WONAPI@@UAEPAXI@)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e9850
// (public: __thiscall std::list<class WONAPI::ElGamal,class std::allocator<class WONAPI::ElGamal> >::~list<class WONAPI::ElGamal,class std::allocator<cla)

// Compiler-generated and header-inline functions this unit emits:
