// The units-before-buildings rule of Grille's Age of AI (dll/engine/unitsfirst.c units_first at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). A unit that picked a building by itself turns on the nearest enemy
// unit close enough to shoot between its shots; a unit that was ordered to attack keeps its target. The arithmetic
// and the category tests are free of game types so they can be tested on their own.
#ifndef AGEOFAI_AGEOAIUNITSFIRST_RULES_H
#define AGEOFAI_AGEOAIUNITSFIRST_RULES_H

namespace AgeOfAI {
namespace UnitsFirstRules {

// The upstream look states (EEUnitAction.h). The port reaches kFace from the attack goal's Step; kWaitReload is
// kept for parity with the upstream rule, but this engine has no hook for that state.
enum { kFace = 2012, kWaitReload = 2021 };

// The reach past the weapon's own range, and the cadence of the look, in game milliseconds.
const float kReach = 4.0f;
const long kEveryMs = 500;

// EEUnitCategory values of things a unit can be attacking when it picks its own target (the building families of
// units_first.c's uf_building).
enum {
    kBuilding = 2,
    kNavalBuilding = 0x15,
    kGate = 0x19,
    kFortress = 0x1d,
    kBuildingPriest = 0x1e,
    kSpaceBuilding = 0x20,
};

// EEUnitCategory values of things worth turning on: walkers, sailors and hoverers, not passing aircraft, shots,
// animals or resources (uf_unit).
enum {
    kNaval = 4,
    kLand = 5,
    kLandCitizen = 6,
    kHelicopter = 7,
    kSubmergedNaval = 0xa,
    kLandPriest = 0xd,
    kLandTransport = 0x10,
    kHelicopterTransport = 0x11,
    kNavalTransport = 0x12,
    kNavalCitizen = 0x16,
    kLandMilitaryHero = 0x17,
    kHelicopterPriest = 0x1a,
};

// Categories the rule refuses, named so a test can name them too (EEUnitCategory).
enum {
    kResource = 1,
    kAirplane = 3,
    kProjectile = 9,
    kSeaAnimal = 0x1b,
};

inline bool IsBuilding(unsigned long inCategory)
{
    switch (inCategory) {
    case kBuilding:
    case kNavalBuilding:
    case kGate:
    case kFortress:
    case kBuildingPriest:
    case kSpaceBuilding:
        return true;
    default:
        return false;
    }
}

inline bool IsSwitchTarget(unsigned long inCategory)
{
    switch (inCategory) {
    case kNaval:
    case kLand:
    case kLandCitizen:
    case kHelicopter:
    case kSubmergedNaval:
    case kLandPriest:
    case kLandTransport:
    case kHelicopterTransport:
    case kNavalTransport:
    case kNavalCitizen:
    case kLandMilitaryHero:
    case kHelicopterPriest:
        return true;
    default:
        return false;
    }
}

// How far from the attacker a candidate unit may be (the weapon's range plus the four-tile reach).
inline float Reach(float inRange)
{
    return inRange + kReach;
}

// Whether a unit that picked a building for itself may look for another target. An ordered goal, a dead unit or
// a target that is not a building keeps its target.
inline bool ShouldSwitch(long inState, bool inOrdered, long inHitPoints, unsigned long inTargetCategory)
{
    return inHitPoints > 0 && !inOrdered && (inState == kFace || inState == kWaitReload) && IsBuilding(inTargetCategory);
}

// Whether a candidate replaces the current best target: seen, hurt-able, within the squared reach and nearer.
// inBestDist2 below zero means there is no best yet.
inline bool Better(long inDist2, long inBestDist2, float inReachSq, bool inSeen, bool inHittable)
{
    return inSeen && inHittable && (float)inDist2 <= inReachSq && (inBestDist2 < 0 || inDist2 < inBestDist2);
}

}  // namespace UnitsFirstRules
}  // namespace AgeOfAI

#endif
