// The rules of counter-fire (AgeOfAICounterFire.cpp) that need no game types: which enemy is a shooter, and how many of
// the player's soldiers go at it.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/counterfire.c
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_COUNTERFIRE_H
#define AGEOFAI_COUNTERFIRE_H

namespace AgeOfAI {
namespace CounterFireRules {

const float kMinRange = 5.0f;   // a shooter's range at least
const float kWin = 1.5f;        // the fight estimate's ratio the soldiers must reach
const int kMost = 60;           // soldiers on one target at most, those already on it counted

// An enemy that shoots at a victim from beyond the victim's reach: its own range is kMinRange at least, and the
// squared tile distance is above (the victim's range + 1) squared.
inline bool IsShooter(float inRange, float inVictimRange, long inDist2)
{
    float theReach = inVictimRange + 1.0f;
    return inRange >= kMinRange && (float)inDist2 > theReach * theReach;
}

// How many of inCandidates (nearest first) go. They join four at a time until the ratio reaches kWin, while those
// already on the target plus those taken stay under kMost. inRatio: the ratio with none taken; inRatioWith(n): the
// ratio with the first n taken. -1: short of kWin even so, and nobody goes; 0: those already on it are enough.
template <typename RatioWith>
int Take(int inCandidates, int inAlready, float inRatio, RatioWith inRatioWith)
{
    int theTake = 0;
    while (inRatio < kWin && theTake < inCandidates && inAlready + theTake < kMost) {
        theTake += theTake + 4 < inCandidates ? 4 : inCandidates - theTake;
        inRatio = inRatioWith(theTake);
    }
    return inRatio < kWin ? -1 : theTake;
}

}
}

#endif
