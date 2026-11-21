// The deterministic part of Grille's coast_towers rule, from dll/ai/defence.c (coast_spot) at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Kept free of game types so the arithmetic is testable.
// AgeOfAICoast.cpp reads the work sites and places the tower through the build queue. Upstream's navy_islands
// precondition (every enemy across water) is left to the caller's coastal-work test; the map's own sea decides.
#ifndef AGEOFAI_COAST_RULES_H
#define AGEOFAI_COAST_RULES_H

#include <cmath>

namespace AgeOfAI {
namespace CoastRules {

const long kSea = 9;    // CT_SEA: a work site counts as coastal with ocean this close
const long kCell = 6;   // CT_CELL: work sites share a cell of this size
const long kCover = 6;  // CT_COVER: a tower this close already covers the cell
const long kIn = 2;     // CT_IN: stand this far in from the shore toward the work site

// coast_spot groups work sites by their tile divided by CT_CELL. The cell's representative is the first site seen.
inline long CellKey(long inTile)
{
    return inTile / kCell;
}

// coast_spot counts standing and pending towers within CT_COVER on each axis of the work site.
inline bool TowerCovers(long inWX, long inWY, long inTX, long inTY)
{
    return std::labs(inTX - inWX) <= kCover && std::labs(inTY - inWY) <= kCover;
}

// coast_spot's nearest-sea search stays inside a CT_SEA square; the distance bound keeps the loop small.
inline bool WithinSea(long inDX, long inDY)
{
    return inDX * inDX + inDY * inDY <= kSea * kSea;
}

// coast_spot moves the point from the work site toward the nearest sea, stopping CT_IN tiles short of it. With the
// sea already within CT_IN the site itself is used.
inline void Inset(long inWX, long inWY, long inSX, long inSY, long& outX, long& outY)
{
    float theDX = (float)(inSX - inWX), theDY = (float)(inSY - inWY);
    float theLen = std::sqrt(theDX * theDX + theDY * theDY);
    float theF = theLen > (float)kIn ? (theLen - (float)kIn) / theLen : 0.0f;
    outX = inWX + (long)std::lround(theDX * theF);
    outY = inWY + (long)std::lround(theDY * theF);
}

// coast_spot tries the centre, then the ring at radius 1, then radius 2. A tile is on the ring when either axis is
// exactly that far out; the centre (radius 0) is the single tile at the inset point.
inline bool OnRing(long inAX, long inAY, long inRadius)
{
    return inRadius == 0 ? (inAX == 0 && inAY == 0)
                         : (std::labs(inAX) == inRadius || std::labs(inAY) == inRadius);
}

}
}

#endif
