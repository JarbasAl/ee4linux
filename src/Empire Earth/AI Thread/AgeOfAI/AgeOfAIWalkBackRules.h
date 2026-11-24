// REGROUP, the walk_back bit of Grille's Age of AI army_mass (dll/ai/mass.c ms_tether and ms_subgroups at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). A soldier chasing a target away from its subgroup's pack walks back to
// the pack. The arithmetic is free of game types so it can be tested on its own. The rally, to_pack, tether and
// wait_stronger bits are not ported.
#ifndef AGEOFAI_AGEOAIWALKBACK_RULES_H
#define AGEOFAI_AGEOAIWALKBACK_RULES_H

namespace AgeOfAI {
namespace WalkBackRules {

// ms_subgroups measures the members from the base: within 25 tiles none are away; away members past 30 tiles from the
// base make the subgroup out. All distances are squared tiles.
const long kBaseSafe = 25;
const long kOutBase = 30;

// ms_tether skips a soldier already walking to within 4 tiles of the pack on either axis (upstream goal_near).
const long kGoalNear = 4;

// ms_tether treats the combat states as the fighting phase; 3004 is a retreat to town but still in that phase.
const unsigned long kStateFightFirst = 3000;
const unsigned long kStateFightLast = 4000;

// A subgroup is out past forming when it is not idle, waiting for units, gathering, preparing travel or loading cargo,
// and its away members lie past kOutBase from the base. A marching subgroup with no away members is out too, but
// REGROUP only acts on packs of away members.
inline bool OutPastForming(unsigned long inState, long inAwayDist2)
{
    if (inState == 0 || inState == 1000 || inState == 1001 || inState == 1004 || inState == 8000)
        return false;
    return inAwayDist2 > kOutBase * kOutBase;
}

inline bool IsFightingState(unsigned long inState)
{
    return inState >= kStateFightFirst && inState < kStateFightLast;
}

// ms_tether's na >= 2: a pack needs at least two away members.
inline bool HasPack(int inAwayCount)
{
    return inAwayCount >= 2;
}

// The three skips of ms_tether and the walk-back test. inTetherPlus4 is army_mass_tether + 4, 16 with the shipped
// tether of 12. inGoalDX/inGoalDY are the current location goal's distances from the pack centre; a soldier with no
// location goal passes a large value. A soldier within kBaseSafe of the base, nearer the pack than inTetherPlus4, or
// already walking to within kGoalNear of the pack on both axes stays put.
inline bool ShouldWalkBack(long inBaseDist2, long inPackDist2, long inTetherPlus4, long inGoalDX, long inGoalDY)
{
    if (inBaseDist2 <= kBaseSafe * kBaseSafe)
        return false;
    if (inPackDist2 < inTetherPlus4 * inTetherPlus4)
        return false;
    if (inGoalDX > -kGoalNear && inGoalDX < kGoalNear && inGoalDY > -kGoalNear && inGoalDY < kGoalNear)
        return false;
    return true;
}

}  // namespace WalkBackRules
}  // namespace AgeOfAI

#endif
