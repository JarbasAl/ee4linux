// The deterministic surrender decision of Grille's diplo.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07. Kept free
// of game types so the arithmetic is testable. The rest of upstream's surrender rule (who destroyed most of the
// player, the block split, the army-power exception) needs a diplomacy/power layer this port does not have.
#ifndef AGEOFAI_AGEOFAISURRENDER_RULES_H
#define AGEOFAI_AGEOFAISURRENDER_RULES_H

namespace AgeOfAI {
namespace SurrenderRules {

enum {
    kNoTrainMs = 60000,    // no citizens this long with no way to train one
    kAnyNoCitizenMs = 180000  // no citizens this long even when a town centre could train them
};

// An AI with no citizens cannot rebuild. It gives up after a minute when it has no finished town centre or cannot pay
// for a citizen (kCanTrain false), or after three minutes in any case. At peace with everyone only the first rule
// applies: an AI that can still train its own people is not broken just because it has none at the moment.
inline bool ShouldResign(unsigned long inNoCitizenMs, bool inCanTrain, bool inAtWar)
{
    if (!inAtWar)
        return !inCanTrain && inNoCitizenMs >= kNoTrainMs;
    return (!inCanTrain && inNoCitizenMs >= kNoTrainMs) || inNoCitizenMs >= kAnyNoCitizenMs;
}

}  // namespace SurrenderRules
}  // namespace AgeOfAI

#endif
