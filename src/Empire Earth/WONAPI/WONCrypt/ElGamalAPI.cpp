
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONElGamal.h"
namespace WONAPI {
// FUNCTION: WON 0x007d7820
void ElGamal::Invalidate()
{
    mIsPrivate=false;
    mIsPublic=false;
}
// FUNCTION: WON 0x007d7830
bool ElGamal::IsPrivate() const
{
    return mIsPrivate;
}
// FUNCTION: WON 0x007d7840
bool ElGamal::IsPublic() const
{
    return mIsPublic;
}
}
