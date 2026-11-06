// Original-backed public key block serialization and signature checks.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAuthPubKeyBlock.h"
namespace WONAPI {
// SUPPORT: EXE 0x006e2350 (empty authored dtor, actual list/ElGamal owner cleanup, quota0)
AuthPubKeyBlock::~AuthPubKeyBlock() {}
// FUNCTION: WON 0x006e2400
bool AuthPubKeyBlock::UnpackHook(ReadBuffer& input)
{
    if(!AuthBase::UnpackHook(input)) return false;
    mKeyBlockId=input.ReadShort();
    unsigned short count=input.ReadShort();
    if(count==0) return false;
    mKeyBlock.clear();
    for(int i=0;i<count;++i) {
        unsigned short keyLength=input.ReadShort();
        ElGamal key;
        if(!key.SetPublicKey(input.ReadBytes(keyLength),keyLength)) return false;
        mKeyBlock.push_back(key);
    }
    return true;
}
// FUNCTION: WON 0x006e26e0
bool AuthPubKeyBlock::Verify(const AuthBase* base) const
{
    if(!IsValid()) return false;
    if(!base->IsValid()) return false;
    for(KeyBlock::const_iterator key=mKeyBlock.begin();key!=mKeyBlock.end();++key)
        if(base->Verify(*key)) return true;
    return false;
}
// FUNCTION: WON 0x006e2740
bool AuthPubKeyBlock::IsValidAndVerified(const ElGamal& verifier) const
{
    return IsValid() && verifier.Verify(GetData(),GetDataLen(),GetSig(),GetSigLen());
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e2350
// (protected: virtual __thiscall WONAPI::AuthPubKeyBlock::~AuthPubKeyBlock(void)    [??1AuthPubKeyBlock@WONAPI@@MAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006eb150
// (protected: struct std::list<class WONAPI::ElGamal,class std::allocator<class WONAPI::ElGamal> >::_Node * __thiscall std::list<class WONAPI::ElGamal,cl)
