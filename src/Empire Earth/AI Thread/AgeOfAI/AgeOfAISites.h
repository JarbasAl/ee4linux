// The rules of the mine search (SelectMineNearDrop in AgeOfAISites.cpp) that need no game types: which mine is another
// town's, and which one is the citizen's own.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/sites.c
// (select_mine_near_drop: LOCAL_MINE_TILES, LOCAL_MINE_FAR) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_SITES_H
#define AGEOFAI_SITES_H

namespace AgeOfAI {
namespace SitesRules {

const long kLocalMineTiles = 40;   // a mine whose drop site is this near the citizen is its own town's
const long kLocalMineFar = 60;     // and past this, the mine is another town's: refused while local_mines is on

// The class of the mine next to inDropX/Y for a citizen at inAtX/Y, by the squared-free distances upstream compares:
// -1 another town's mine (refused), 0 a mine out of reach of the citizen's town, 1 its own town's (taken first).
inline int MineLocal(long inDropX, long inDropY, long inAtX, long inAtY)
{
    long theDX = inDropX - inAtX, theDY = inDropY - inAtY;
    long theD2 = theDX * theDX + theDY * theDY;
    if (theD2 > kLocalMineFar * kLocalMineFar)
        return -1;
    return theD2 <= kLocalMineTiles * kLocalMineTiles ? 1 : 0;
}

}
}

#endif
