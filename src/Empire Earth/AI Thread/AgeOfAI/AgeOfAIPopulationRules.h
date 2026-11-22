// The population-cap rule of Grille's Age of AI (dll/ai/econ.c hook_pop_cap at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). After the game has worked out an AI's cap from the human populations,
// pop_follow_human=0 gives that AI its own game limit instead; 1 keeps the game's weighted cap. The rule is inert
// while the AI ignores the other players, when the game already uses its own limit. Free of game types so the
// arithmetic is testable.
#ifndef AGEOFAI_AGEOAIPOPULATION_RULES_H
#define AGEOFAI_AGEOAIPOPULATION_RULES_H

namespace AgeOfAI {
namespace PopulationRules {

// The cap after the pop_follow_human rule. inWeighted is the game's cap from the human populations; inGameLimit is
// the player's own limit from EEPlayer::GetPopulationLimit; inFollowHuman is the difficulty's pop_follow_human.
// A nonzero setting keeps the weighted cap; zero uses the game limit.
inline long Cap(long inWeighted, long inGameLimit, long inFollowHuman)
{
    return inFollowHuman != 0 ? inWeighted : inGameLimit;
}

}  // namespace PopulationRules
}  // namespace AgeOfAI

#endif
