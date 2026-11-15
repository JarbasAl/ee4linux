// The rules of counter-siege and the citizen militia (AgeOfAIDanger.cpp) that need no game types: when the citizens
// fight, and when the soldiers go.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/defence.c
// (counter_siege, militia_answer) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_DEFENCE_H
#define AGEOFAI_DEFENCE_H

namespace AgeOfAI {
namespace DefenceRules {

const int kMilitiaMostEnemies = 6;    // more enemies than this near the target: the citizens stay away
const int kMilitiaPerEnemy = 3;       // citizens sent per enemy at most
const float kMilitiaStand = 3.0f;     // the citizens' strength over the enemies' they need
const int kCounterSiegeMost = 5;      // soldiers on one siege threat at most, those already on it counted

// The citizens' strength over the enemies' (hit points times attack, summed). No enemy strength: they win.
inline float PowerRatio(float inCitizenPower, float inEnemyPower)
{
    return inEnemyPower > 0 ? inCitizenPower / inEnemyPower : 1e6f;
}

// Early militia: the citizens lost in the fight, from the fight estimate (the enemy's damage per second in contact,
// the time the citizens need to win, their count and their hit points). The citizens go only below 1.
inline float CitizensLost(float inContactDps, float inTime, long inCount, float inHitPoints)
{
    return inContactDps * inTime * (float)inCount / inHitPoints;
}

// militia_estimate: citizens caught by soldiers fight on only when the soldiers attack, are more than a tenth faster
// than the fastest citizen, and the citizens do at least inMinShare of the work (their time over the enemy's).
inline bool CaughtFight(bool inAttacking, float inFastest, float inCitizenSpeed, float inShare, float inMinShare)
{
    return inAttacking && inFastest > inCitizenSpeed * 1.1f && inShare >= inMinShare;
}

// Counter-siege: the soldiers go when the free ones and those already on the threat are at least twice the enemy
// soldiers near it, plus 3.
inline bool SoldiersGo(int inAvailable, int inAlready, int inGuards)
{
    return inAvailable + inAlready >= 2 * inGuards + 3;
}

}
}

#endif
