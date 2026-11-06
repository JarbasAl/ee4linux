// Original-backed AuthBase owned raw bytes, unpack dispatch and expiry.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAuthBase.h"
#include "ee/wonapi/WONElGamal.h"
namespace WONAPI {
// FUNCTION: WON 0x006dcce0
AuthBase::AuthBase(const void* data, unsigned long length)
{
    mIsValid = false;
    if (data != NULL && length != 0)
        mRawBuf = new ByteBuffer(data, length, false);
}
// FUNCTION: WON 0x006dcdd0
AuthBase::~AuthBase() {}
// Primary hot112+shared normal tail20; generatedcatch/restore12 excluded.
// FUNCTION: WON 0x006dce40
bool AuthBase::Unpack()
{
    bool valid = false;
    if (mRawBuf.get() != NULL) {
        try {
            ReadBuffer input(mRawBuf->data(), mRawBuf->length());
            valid = UnpackHook(input);
            if (valid) {
                mDataLen = static_cast<unsigned short>(input.mL00);
                if (static_cast<unsigned long>(input.mL08) <= mDataLen)
                    valid = false;
            }
        }
        catch (ReadBufferException&) {}
    }
    mIsValid = valid;
    return mIsValid;
}
// FUNCTION: WON 0x006dcf40
bool AuthBase::IsExpired(time_t extra) const
{
    if (!mIsValid)
        return true;
    return mExpireTime <= time(NULL) + extra;
}
// FUNCTION: WON 0x006dcf80
bool AuthBase::UnpackHook(ReadBuffer& input)
{
    mFamily = input.ReadShort();
    mIssueTime = input.ReadLong();
    mExpireTime = input.ReadLong();
    return true;
}
// FUNCTION: WON 0x006dced0
bool AuthBase::Verify(const ElGamal& key) const
{
    return key.Verify(GetData(), GetDataLen(), GetSig(), GetSigLen());
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006dcdb0
// (protected: virtual void * __thiscall WONAPI::AuthBase::`scalar deleting destructor'(unsigned int)    [??_GAuthBase@WONAPI@@MAEPAXI@Z])
