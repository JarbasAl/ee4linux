// The arithmetic of Grille's econ.c early_requirements (39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07): an epoch also needs
// new things of the current epoch, and the AI asks the research manager for them before the advance timer instead of
// waiting until it can pay. The ask is renewed three minutes later, or restarted without a new ask when the missing
// count drops. Decoration objects ("x ..." in the unit table) stop counting toward the epoch once per tech tree.
#ifndef AGEOFAI_EARLY_REQUIREMENTS_RULES_H
#define AGEOFAI_EARLY_REQUIREMENTS_RULES_H

namespace AgeOfAI {
namespace EarlyRequirementsRules {

const unsigned long kRenewMs = 180000;
const unsigned long kNearAttemptMs = 600000;
const int kAsk = 1;
const int kProgress = 2;
const int kRenew = 3;

inline bool IsDecorationName(const char* inName)
{
    return inName != NULL && inName[0] == 'x' && inName[1] == ' ';
}

// 0: wait. kProgress: restart the wait, no new ask. kAsk: first ask (no request out). kRenew: ask again while a
// request has been out for at least three minutes. requestOut is the game's epoch-research-requested byte; askedAt is
// zero before the first ask and askedMissing the gap at that ask. attemptTime is the game's next-epoch timer. Outside
// prefarm an attempt more than ten minutes away is only asked about when it is near.
inline int AskDecision(unsigned long inNow, unsigned long inAskedAt, int inAskedMissing, int inMissing,
                       unsigned long inAttemptTime, bool inPrefarm, bool inRequestOut)
{
    if (inMissing < 1)
        return 0;
    if (inRequestOut) {
        if (inAskedAt != 0 && inNow >= inAskedAt && inNow - inAskedAt < kRenewMs)
            return 0;
        if (inAskedAt != 0 && inMissing < inAskedMissing)
            return kProgress;
    }
    if (!inPrefarm && inAttemptTime > inNow && inAttemptTime - inNow > kNearAttemptMs)
        return 0;
    return inRequestOut ? kRenew : kAsk;
}

}  // namespace EarlyRequirementsRules
}  // namespace AgeOfAI

#endif
