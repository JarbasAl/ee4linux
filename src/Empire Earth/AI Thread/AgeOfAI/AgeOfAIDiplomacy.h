// The small diplomacy rules of Age of AI, ported from Grille's dll/ai/diplo.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07. The whole diplomacy state engine is not ported: these helpers only decide
// when an AI answers a declaration of war with its own stance change.
#ifndef AGEOFAI_AGEOFAIDIPLOMACY_H
#define AGEOFAI_AGEOFAIDIPLOMACY_H

namespace AgeOfAI {
namespace DiplomacyRules {

// diplo.c diplo_tick: 3 to 6 game seconds, deterministic in the two player numbers. They are 1-based, as upstream's
// player indices are.
inline unsigned long AnswerWarDelay(unsigned long inPlayer, unsigned long inOther)
{
    return 3000 + ((inPlayer * 7 + inOther * 3) % 4) * 1000;
}

// Only a change from the other player's ally to enemy, seen after the first look, while this AI is still its ally.
inline bool AnswerWarTriggered(long inTheirStance, long inPreviousStance, long inMyStance)
{
    return inTheirStance != 0 && inPreviousStance == 0 && inMyStance == 0;
}

// diplo.c checks both stances again when the timer ends: peace again, an answer already given, or a player that is out
// cancels it. A game result of 2 is a defeated player.
inline bool AnswerWarDue(long inTheirStance, long inMyStance, unsigned long inGameResult)
{
    return inTheirStance != 0 && inMyStance == 0 && inGameResult != 2;
}

}  // namespace DiplomacyRules
}  // namespace AgeOfAI

#endif
