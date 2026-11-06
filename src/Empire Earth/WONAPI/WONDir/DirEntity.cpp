// Native directory entity behavior; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
// FUNCTION: WON 0x006f0190
DirEntity::DirEntity()
{
    mType = 0;
    mLifespan = 0;
    mCreated = 0;
    mTouched = 0;
    mCRC = 0;
    mCreateIP = 0;
    mTouchIP = 0;
    mCreateId = 0;
    mTouchId = 0;
    mReqUnique = false;
}
// FUNCTION: WON 0x006f0290
DirEntity::~DirEntity()
{
}
// FUNCTION: WON 0x006f0400
IPAddr DirEntity::GetNetAddrAsIP() const
{
    IPAddr result;
    if (mNetAddr.get() != NULL && mNetAddr->length() >= 6)
        result.SetSixByte(mNetAddr->data());
    return result;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f0270
// (protected: virtual void * __thiscall WONAPI::DirEntity::`scalar deleting destructor'(unsigned int)    [??_GDirEntity@WONAPI@@MAEPAXI@Z])
