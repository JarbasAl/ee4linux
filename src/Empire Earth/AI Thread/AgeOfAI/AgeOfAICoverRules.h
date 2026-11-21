// The deterministic part of Grille's fall_back BREAK bit, from dll/ai/cover.c (cv_foe_near, cv_behave, cv_cover) at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Kept free of game types so the arithmetic is testable.
// The port leaves out END and SEA: it holds a unit at cover, but it neither gives an attack up nor moves fleets.
#ifndef AGEOFAI_COVER_RULES_H
#define AGEOFAI_COVER_RULES_H

#include <cmath>

namespace AgeOfAI {
namespace CoverRules {

const int kCell = 8;                   // CV_CELL: enemy fighters in the unit's cell or one around it
const int kLocalRadius = 10;           // cv_local(pl, p, 10, 1): the fight judged at the unit
const int kCoverOffset = 2;            // cv_cover: the stop two tiles on the base side of the cover
const unsigned long kHoldAtMs = 20000; // held this long after arriving at cover
const unsigned long kHoldMaxMs = 90000;// held at most this long

// cv_foe_near: any enemy fighter in the unit's own 8-tile cell or one of the eight around it. Tiles outside the
// map are not cells upstream; the caller keeps the unit on the map.
inline bool EnemyNear(long inUX, long inUY, long inEX, long inEY)
{
    long theDX = inUX / kCell - inEX / kCell;
    long theDY = inUY / kCell - inEY / kCell;
    return theDX >= -1 && theDX <= 1 && theDY >= -1 && theDY <= 1;
}

// cv_behave: break off when the local estimate is below fall_back_ratio (a fraction here, percent upstream).
inline bool LostFight(float inEstimate, float inRatio)
{
    return inEstimate < inRatio;
}

// cv_behave: a unit already within four tiles of the cover fights there instead of falling back.
inline bool AtCover(long inDX, long inDY)
{
    return inDX * inDX + inDY * inDY <= 16;
}

// cv_cover: stand two tiles from the cover on the side the base lies. With no base, stand on the cover itself.
inline void CoverTarget(long inUX, long inUY, long inCX, long inCY, bool inBase,
                        long inBX, long inBY, long& outX, long& outY)
{
    (void)inUX;
    (void)inUY;
    outX = inCX;
    outY = inCY;
    if (!inBase)
        return;
    float theDX = (float)(inBX - inCX), theDY = (float)(inBY - inCY);
    float theLen = theDX * theDX + theDY * theDY;
    if (theLen < 1.0f)
        return;
    theLen = std::sqrt(theLen);
    outX = inCX + (long)std::lround(theDX / theLen * (float)kCoverOffset);
    outY = inCY + (long)std::lround(theDY / theLen * (float)kCoverOffset);
}

// cv_behave's release: twenty seconds after arrival, or ninety seconds at most after the order was given.
inline bool HoldDone(bool inAt, unsigned long inNow, unsigned long inAtTime, unsigned long inSince)
{
    if (inNow < inSince)
        return false;
    if (inNow - inSince > kHoldMaxMs)
        return true;
    return inAt && inNow >= inAtTime && inNow - inAtTime >= kHoldAtMs;
}

}
}

#endif
