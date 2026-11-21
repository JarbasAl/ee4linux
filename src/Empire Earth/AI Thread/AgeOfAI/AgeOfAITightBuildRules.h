// The deterministic rules of the packed-building search, ported from Grille's dll/ai/packing.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07. Kept separate from the game-facing search so the arithmetic is testable.
#ifndef AGEOFAI_AGEOFAITIGHTBUILDRULES_H
#define AGEOFAI_AGEOFAITIGHTBUILDRULES_H

#include "ee/EEUnitClassIDs.h"

namespace AgeOfAI {
namespace TightBuildRules {

enum {
    kRadius = 32,         // tiles from the base looked at
    kMoreRings = 8,       // once a spot is found, this many rings further out
    kMaxSize = 8,         // the biggest footprint looked at
    kTouchWeight = 8      // a touching tile is worth this many tiles of distance from the base
};

// packing.c tb_type: the production buildings this search places. Airports stay with the game's own search, whose
// strip reaches beyond the footprint towards the middle of the map.
inline bool IsPackedType(long inType)
{
    return inType == kUnitTypeBarracks || inType == kUnitTypeArcheryRange || inType == kUnitTypeStable
        || inType == kUnitTypeSiegeFactory || inType == kUnitTypeTankFactory || inType == kUnitTypeCyberFactory;
}

// packing.c tb_kin: the buildings a packed spot may touch.
inline bool IsKinType(long inType)
{
    return IsPackedType(inType) || inType == kUnitTypeHouse || inType == kUnitTypeHospital
        || inType == kUnitTypeTemple || inType == kUnitTypeUniversity;
}

// packing.c tb_spot: distance counts against a spot, a touching tile counts for eight.
inline long SpotScore(long inTouch, long inRing)
{
    return inTouch * kTouchWeight - inRing;
}

// packing.c tb_ring_ok: the walkable tiles round the footprint must form one unbroken arc. A ring wholly walkable is
// one arc too; upstream skips the arc count in that case. outWalkable lets the caller apply the s+2 exit rule.
inline bool RingWalkable(const bool* inRing, long inCount, long inMinWalkable, long& outWalkable)
{
    outWalkable = 0;
    long theArcs = 0;
    for (long i = 0; i < inCount; i++) {
        if (!inRing[i])
            continue;
        outWalkable++;
        if (!inRing[(i + 1) % inCount])
            theArcs++;
    }
    if (outWalkable < inMinWalkable)
        return false;
    return outWalkable == inCount || theArcs == 1;
}

}  // namespace TightBuildRules
}  // namespace AgeOfAI

#endif
