// The deterministic rules of the fall_back hospital bit, ported from Grille's dll/ai/cover.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07. Kept separate from the game-facing pass so the arithmetic is testable.
#ifndef AGEOFAI_AGEOFAIHOSPITALRULES_H
#define AGEOFAI_AGEOFAIHOSPITALRULES_H

#include <cmath>

namespace AgeOfAI {
namespace HospitalRules {

// cover.c:690. The setting is a fraction here (40% is 0.40), as upstream's setting_pct turns it into 40.
inline bool NeedsHospital(long inHP, long inMaxHP, float inFraction)
{
    return inMaxHP > 0 && inHP > 0 && (float)inHP < (float)inMaxHP * inFraction;
}

// cover.c:696-703. The unit stops two tiles from the hospital, on the side it came from.
inline void HospitalTarget(long inUX, long inUY, long inHX, long inHY, long& outX, long& outY)
{
    float theDX = (float)(inUX - inHX), theDY = (float)(inUY - inHY);
    float theLen = std::sqrt(theDX * theDX + theDY * theDY);
    outX = inHX + (theLen >= 1.0f ? (long)std::lround(2.0f * theDX / theLen) : 2);
    outY = inHY + (theLen >= 1.0f ? (long)std::lround(2.0f * theDY / theLen) : 0);
}

// cover.c:657-664. A unit leaves the hospital when it has healed to 90%, when no hospital is near its stop any more,
// or after four minutes. A game result that ends earlier also clears the hold in the game-facing pass.
inline bool HospitalRelease(long inHP, long inMaxHP, bool inHospitalNear, unsigned long inNow,
                            unsigned long inSince)
{
    if (!inHospitalNear)
        return true;
    if (inMaxHP > 0 && inHP * 10 >= inMaxHP * 9)
        return true;
    return inNow >= inSince && inNow - inSince > 240000;
}

}  // namespace HospitalRules
}  // namespace AgeOfAI

#endif
