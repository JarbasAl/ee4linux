// The answering-fire rule of Grille's Age of AI (dll/engine/unitsfirst.c answer_fire at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). An idle fighter shot from beyond its reply range goes for the enemy
// that aims at it, if the enemy is near, seen, hurt-able and on the same land or water area. The arithmetic and the
// category tests are free of game types so they can be tested on their own.
#ifndef AGEOFAI_AGEOAIANSWERFIRE_RULES_H
#define AGEOFAI_AGEOAIANSWERFIRE_RULES_H

namespace AgeOfAI {
namespace AnswerFireRules {

// The upstream setting's three values: the shipped game value, all players, computer players only.
enum { kGame = 0, kAll = 1, kComputers = 2 };

// The look's tile reach and cadence from unitsfirst.c (AF_REACH and UF_EVERY).
const long kReach = 24;
const long kEveryMs = 500;

// The script state that may answer fire (EEUnitAction.h kActionIdle = 5000), and the state an assigned attack goal
// starts in (kActionPrepareToMove = 8000, upstream's 0x1F40).
enum { kIdle = 5000, kPrepareToMove = 8000 };

// EEUnitCategory values of the upstream af_fighter list (CLS_LAND, CLS_LAND_MILITARY_HERO, CLS_NAVAL,
// CLS_SUBMERGED_NAVAL, CLS_HELICOPTER). Citizens, priests and transports are not fighters.
enum { kNaval = 4, kLand = 5, kHelicopter = 7, kSubmergedNaval = 0xa, kLandMilitaryHero = 0x17 };

// The area kind the upstream af_reachable asks the map for: land, water, flying (a helicopter flies over either).
enum { kFlyingArea = 0, kWaterArea = 1, kLandArea = 2 };

// Only these categories with a positive attack answer fire (af_fighter).
inline bool IsFighter(unsigned long inCategory, float inAttack)
{
    if (!(inAttack > 0.0f))
        return false;
    switch (inCategory) {
    case kNaval:
    case kLand:
    case kHelicopter:
    case kSubmergedNaval:
    case kLandMilitaryHero:
        return true;
    default:
        return false;
    }
}

// The connected area the unit must share with its attacker (af_reachable's kind).
inline long AreaKind(unsigned long inCategory)
{
    switch (inCategory) {
    case kLand:
    case kLandMilitaryHero:
        return kLandArea;
    case kHelicopter:
        return kFlyingArea;
    default:
        return kWaterArea;
    }
}

// Land and sea units need one nonzero shared area; a helicopter flies, so it always reaches its attacker.
inline bool Reachable(unsigned long inCategory, unsigned short inFromArea, unsigned short inToArea)
{
    if (AreaKind(inCategory) == kFlyingArea)
        return true;
    return inFromArea != 0 && inFromArea == inToArea;
}

// Whether the attacker is close enough to answer (AF_REACH, measured in whole tiles).
inline bool InReach(long inDX, long inDY)
{
    return inDX * inDX + inDY * inDY <= kReach * kReach;
}

// Whether a unit gets a goal at all: the game value does nothing; computers skips human players.
inline bool AppliesTo(long inMode, bool inHuman)
{
    return inMode != kGame && (inMode != kComputers || !inHuman);
}

// Whether a unit may answer fire: idle, without a goal, alive and a fighter (answer_fire_tick's defender checks).
inline bool ShouldAnswer(long inState, bool inHasGoal, long inHitPoints, unsigned long inCategory, float inAttack)
{
    return inState == kIdle && !inHasGoal && inHitPoints > 0 && IsFighter(inCategory, inAttack);
}

// Whether an aiming enemy may be answered: alive, on the map, seen, hurt-able, in reach and reachable.
inline bool CanAnswer(long inDX, long inDY, long inHitPoints, long inTileX, bool inSeen, bool inHittable, bool inReachable)
{
    return inHitPoints > 0 && inTileX >= 0 && inSeen && inHittable && inReachable && InReach(inDX, inDY);
}

}  // namespace AnswerFireRules
}  // namespace AgeOfAI

#endif
