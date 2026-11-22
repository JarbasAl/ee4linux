// The give-up part of Grille's Age of AI fall_back (dll/ai/cover.c END block at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). A soldier that breaks off from the same attack group again and again
// stops returning to the lost fight: after fall_back_breaks break-offs it stays at cover while that group lasts.
// The arithmetic is free of game types so it can be tested on its own. The attack call-off (cv_end) is not ported.
#ifndef AGEOFAI_AGEOAICOVERGIVEUP_RULES_H
#define AGEOFAI_AGEOAICOVERGIVEUP_RULES_H

namespace AgeOfAI {
namespace CoverGiveUpRules {

// The give_up field of [ai_base] fall_back (rows.h FALL_BACK_FIELDS), the eighth bit of the flags word. It is off in
// the shipped ini, so the tally and the longer hold do nothing until a profile asks for them.
const unsigned long kGiveUpBit = 8;

// cover.c cv_tally: a record older than ten minutes starts a new tally, and so does a break-off from another group.
const unsigned long kResetMs = 600000;

// cover.c cv_behave kind 3: a gave-up soldier stays back until its attack is over, at most five minutes. Upstream
// releases it when it is no longer in the group it gave up on, or when this time is up.
const unsigned long kHoldMaxMs = 300000;

// True when a repeated break-off must start a fresh tally (a new game also counts as a reset).
inline bool ResetTally(unsigned long inNow, unsigned long inTime)
{
    return inNow < inTime || inNow - inTime > kResetMs;
}

// cover.c: kind 3 when the unit is in a group and its tally reaches fall_back_breaks. A non-positive break count
// disables the rule, as the setting row's lower limit does.
inline bool GaveUp(int inCount, long inBreaks)
{
    return inBreaks > 0 && inCount >= inBreaks;
}

// cover.c kind 3 release: the attack is over when the unit is not with that group any more, or the cap is reached.
inline bool GiveUpDone(bool inSameGroup, unsigned long inNow, unsigned long inSince)
{
    return !inSameGroup || inNow < inSince || inNow - inSince > kHoldMaxMs;
}

}  // namespace CoverGiveUpRules
}  // namespace AgeOfAI

#endif
