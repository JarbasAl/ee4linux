// The town-work rule of Grille's Age of AI (dll/ai/sites.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). The game's assignment sends a citizen to whichever site fills an empire-wide
// shortfall, even when that site belongs to a town far away. This port keeps a citizen at a site its own town can reach.
// The arithmetic is free of game types so it can be tested on its own. The training-time homes half is not ported.
#ifndef AGEOFAI_AGEOAITOWNWORK_RULES_H
#define AGEOFAI_AGEOAITOWNWORK_RULES_H

namespace AgeOfAI {
namespace TownWorkRules {

// upstream TOWN_FAR = LOCAL_MINE_FAR (sites.c): a site is a town's own while its nearest drop site is within this many
// tiles of the citizen. The citizen's current tile stands in for the home upstream remembers from training.
const long kFar = 60;

// upstream town_local: compare the squared distance from the site's nearest drop site to the citizen's home.
inline bool IsFar(long inDropX, long inDropY, long inHomeX, long inHomeY)
{
    const long theDX = inDropX - inHomeX, theDY = inDropY - inHomeY;
    return theDX * theDX + theDY * theDY > kFar * kFar;
}

// upstream town_work_for: after the citizen's own resource, try the resource with the largest shortfall that has not
// been tried yet. A non-positive or already tried shortfall cannot be chosen. Returns -1 when nothing is left.
inline int BestShortfall(const long* inShort, unsigned long inTried, int inCount)
{
    int theBest = -1;
    long theMost = 0;
    for (int k = 0; k < inCount; k++)
        if (!(inTried & (1UL << k)) && inShort[k] > theMost) {
            theMost = inShort[k];
            theBest = k;
        }
    return theBest;
}

}  // namespace TownWorkRules
}  // namespace AgeOfAI

#endif
