// The deterministic part of Grille's towns scout, from dll/ai/towns.c (tn_scout, tn_front and tn_scout_watch) at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Kept free of game types so the arithmetic is testable.
// AgeOfAIScout.cpp reads the map, the player's memory and its units, then walks the scout with the game's goal.
// Upstream sends the scout only when no town site is left; the port has no town planner and sends it on the setting.
#ifndef AGEOFAI_SCOUT_RULES_H
#define AGEOFAI_SCOUT_RULES_H

#include <cmath>

namespace AgeOfAI {
namespace ScoutRules {

const long kCell = 8;        // TN_CELL: the map is scanned in cells this wide, starting at their middle
const long kSight = 12;      // TN_SCOUT_SIGHT: an enemy soldier this close sends the scout home
const unsigned long kPause = 90000;  // one scout at a time, then 90 game seconds before the next
const long kHome = 40;       // the scout is picked from the idle soldiers this near the base

struct Point {
    long mX, mY;
};

inline long Dist2(const Point& inA, const Point& inB)
{
    long theDX = inA.mX - inB.mX, theDY = inA.mY - inB.mY;
    return theDX * theDX + theDY * theDY;
}

// tn_front: the nearest own town's distance to any known enemy building, outDF: the cell's distance to the nearest
// one. No known enemy building gives a front of -1 and an unreachable distance, so no cell is held back.
inline long Front(const Point* inOwn, int inNOwn, const Point* inFoe, int inNFoe, const Point& inP, long& outDF)
{
    long theFront = -1;
    outDF = 0x7FFFFFFF;
    if (inNFoe <= 0)
        return theFront;
    for (int i = 0; i < inNOwn; i++) {
        long theNearest = 0x7FFFFFFF;
        for (int j = 0; j < inNFoe; j++) {
            long theD = Dist2(inOwn[i], inFoe[j]);
            if (theD < theNearest)
                theNearest = theD;
        }
        long theD = (long)std::sqrt((float)theNearest);
        if (theFront < 0 || theD < theFront)
            theFront = theD;
    }
    long theNearest = 0x7FFFFFFF;
    for (int j = 0; j < inNFoe; j++) {
        long theD = Dist2(inP, inFoe[j]);
        if (theD < theNearest)
            theNearest = theD;
    }
    outDF = (long)std::sqrt((float)theNearest);
    return theFront;
}

// tn_scout refuses a cell nearer to the enemy than its front town.
inline bool BeyondFront(long inFront, long inDF)
{
    return inFront < 0 || inDF >= inFront;
}

inline long NearestOwn2(const Point* inOwn, int inNOwn, const Point& inP)
{
    long theBest = 0x7FFFFFFF;
    for (int i = 0; i < inNOwn; i++) {
        long theD = Dist2(inP, inOwn[i]);
        if (theD < theBest)
            theBest = theD;
    }
    return theBest;
}

// The scan of tn_scout: the never-looked, unthreatened cell on the player's side of the front nearest to its towns.
// The caller supplies the look and threat tests; the own list includes the base as well as the standing towns.
template <class TUnseen, class TThreat>
bool PickCell(long inSize, const Point* inOwn, int inNOwn, const Point* inFoe, int inNFoe,
              TUnseen inUnseen, TThreat inThreat, Point& outCell, long& outDist2)
{
    if (inNOwn <= 0 || inSize <= 0)
        return false;
    long theBest = 0x7FFFFFFF;
    Point theCell = { 0, 0 };
    for (long y = kCell / 2; y < inSize; y += kCell) {
        for (long x = kCell / 2; x < inSize; x += kCell) {
            Point theP = { x, y };
            long theDF = 0;
            if (!BeyondFront(Front(inOwn, inNOwn, inFoe, inNFoe, theP, theDF), theDF)
                || !inUnseen(theP) || inThreat(theP))
                continue;
            long theD = NearestOwn2(inOwn, inNOwn, theP);
            if (theD < theBest) {
                theBest = theD;
                theCell = theP;
            }
        }
    }
    if (theBest == 0x7FFFFFFF)
        return false;
    outCell = theCell;
    outDist2 = theBest;
    return true;
}

// tn_scout picks the fastest idle soldier; the first one wins a tie, as upstream's strict speed test does.
inline bool Faster(float inSpeed, float inBest)
{
    return inSpeed > inBest;
}

// tn_scout_watch sends the scout home when an enemy soldier is in sight or a known defence covers it.
inline bool TurnBack(long inDX, long inDY, bool inDefence)
{
    return inDefence || inDX * inDX + inDY * inDY <= kSight * kSight;
}

inline bool PauseOver(unsigned long inNow, unsigned long inSince)
{
    return inSince == 0 || inNow < inSince || inNow - inSince >= kPause;
}

}
}

#endif
