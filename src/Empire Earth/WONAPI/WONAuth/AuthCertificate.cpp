// Original-backed AuthBase owned raw bytes, unpack dispatch and expiry.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAuthCertificate.h"
namespace WONAPI {
// FUNCTION: WON 0x006dcfb0
AuthCertificate::~AuthCertificate() {}
// FUNCTION: WON 0x006dd120
bool AuthCertificate::UnpackHook(ReadBuffer& input)
{
    if (!AuthBase::UnpackHook(input))
        return false;
    mUserId = input.ReadLong();
    mCommunityId = input.ReadLong();
    mTrustLevel = input.ReadShort();
    unsigned short keyLength = input.ReadShort();
    if (!mPubKey.SetPublicKey(input.ReadBytes(keyLength), keyLength))
        return false;
    return true;
}
// FUNCTION: WON 0x006dd190
bool AuthCertificate::VerifyPermissions(DWORD userId, DWORD community, unsigned short minTrust) const
{
    if (userId != 0 && mUserId != userId)
        return false;
    if (community != 0 && mCommunityId != community)
        return false;
    return mTrustLevel >= minTrust;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006dd100
// (protected: virtual void * __thiscall WONAPI::AuthCertificate::`scalar deleting destructor'(unsigned int)    [??_GAuthCertificate@WONAPI@@MAEPAXI@Z])
