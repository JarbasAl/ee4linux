// The raid leash of Grille's Age of AI (dll/ai/armies.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). A member of a force attacking a spot that chases further than the leash
// is no longer part of the force: it goes home and waits there. The task test and the distance arithmetic are free of
// game types so they can be tested on their own.
#ifndef AGEOFAI_AGEOAIRAIDLEASH_RULES_H
#define AGEOFAI_AGEOAIRAIDLEASH_RULES_H

namespace AgeOfAI {
namespace RaidLeashRules {

// The arrival radius of a force at its destination (armies.c AR_DONE). The leash is measured from that radius.
const long kDone = 8;

// Attack group task types (EECPAGTask.h). A location task stands in for upstream's raid mission (a->out == 2): the
// force attacks a spot, not a particular enemy unit. A returning group is left alone.
enum {
    kTaskNone = 0,
    kTaskLocation = 1,
    kTaskTerritory = 2,
    kTaskUnit = 7,
    kTaskReturn = 8,
    kTaskAttackLocation = 10,
    kTaskDefendWonder = 11,
    kTaskAttackWonder = 12
};

inline bool IsRaidTask(long inType)
{
    return inType == kTaskLocation || inType == kTaskAttackLocation;
}

// Upstream compares ivec2_dist2(tile, dest) with (AR_DONE + raid_leash)^2. Equal is still inside the leash.
inline bool BeyondLeash(long inDX, long inDY, long inLeash)
{
    const long theLimit = kDone + inLeash;
    return inDX * inDX + inDY * inDY > theLimit * theLimit;
}

// A member is called home only when it is in a raid, alive, still part of the force, and past the leash.
inline bool ShouldCall(bool inRaidTask, bool inAlive, bool inPartOfForce, long inDX, long inDY, long inLeash)
{
    return inRaidTask && inAlive && inPartOfForce && BeyondLeash(inDX, inDY, inLeash);
}

}  // namespace RaidLeashRules
}  // namespace AgeOfAI

#endif
