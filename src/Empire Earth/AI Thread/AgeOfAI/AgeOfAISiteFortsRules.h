// The arithmetic of the site-fort choice (AgeOfAISiteForts.cpp) that needs no game types: which mine gets a fort,
// how far forts keep apart, and where the four corner towers stand.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/defence.c
// (sf_pick, sf_corner, sf_build, site_forts_tick) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Upstream's
// wall runs and the side left open towards the base are not ported, so the rules stop at the corner towers.
#ifndef AGEOFAI_SITE_FORTS_H
#define AGEOFAI_SITE_FORTS_H

namespace AgeOfAI {
namespace FortRules {

const long kMaxForts = 6;             // forts per player, the setting's range and upstream's SF_MAX
const long kTowers = 4;               // one tower at each corner of the mine
const long kRadius = 4;               // tiles from the mine to a corner tower, upstream's SF_R
const long kMinWorkers = 2;           // gatherers needed at a mine before it gets a fort, upstream's SF_MIN
const long kExposedBonus = 4;         // extra score for sea in reach or recently hurt citizens, SF_EXPOSED
const long kSeaRadius = 9;            // tiles a ship can reach from the mine, upstream's CT_SEA
const unsigned long kTickMs = 20000;  // the defence tick that builds a fort, one tower at a time
const unsigned long kBusyMs = 1800000;// a fort's towers are ordered within 30 minutes of its pick

// A mine stands on more than one tile: its middle is the sprite's tile plus half its size, for sizes 2 to 7.
inline long MineCenter(long inTile, long inSize)
{
    return inTile + ((inSize > 1 && inSize < 8) ? inSize / 2 : 0);
}

// A tile within the ship's reach of the mine, by squared distance (the circle, not the square).
inline bool InSeaReach(long inDX, long inDY)
{
    return inDX * inDX + inDY * inDY <= kSeaRadius * kSeaRadius;
}

// The mine's score: its gatherers, plus the exposure bonus when a ship or a hurt citizen is near.
inline long Score(long inWorkers, bool inExposed)
{
    return inWorkers + (inExposed ? kExposedBonus : 0);
}

// A second fort cannot stand within twice the corner radius of one already picked, on either axis.
inline bool FortTaken(long inFortX, long inFortY, long inX, long inY)
{
    long theDX = inFortX > inX ? inFortX - inX : inX - inFortX;
    long theDY = inFortY > inY ? inFortY - inY : inY - inFortY;
    return theDX < 2 * kRadius && theDY < 2 * kRadius;
}

// Corner order 0 (-,-), 1 (+,-), 2 (+,+), 3 (-,+), as upstream's sf_corner.
inline void Corner(long inX, long inY, int inK, long& outX, long& outY)
{
    outX = inX + ((inK == 1 || inK == 2) ? kRadius : -kRadius);
    outY = inY + (inK >= 2 ? kRadius : -kRadius);
}

// The tiles tried for one corner: the corner itself, then its eight neighbours. Nine offsets, r=0 before r=1.
inline int Ring(long outOffsets[9][2])
{
    int theCount = 0;
    for (long r = 0; r <= 1; r++)
        for (long ax = -r; ax <= r; ax++)
            for (long ay = -r; ay <= r; ay++) {
                long theAX = ax < 0 ? -ax : ax, theAY = ay < 0 ? -ay : ay;
                if (theAX != r && theAY != r)
                    continue;
                outOffsets[theCount][0] = ax;
                outOffsets[theCount][1] = ay;
                theCount++;
            }
    return theCount;
}

}  // namespace FortRules
}  // namespace AgeOfAI

#endif
