// The places of the early game of an Age of AI player: where the first towers of a town stand and where a citizen walks
// to lead raiders under a tower. Tiles are the game's tile numbers. No game types.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/early.c
// (early_slot, early_bait) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_EARLYGEOMETRY_H
#define AGEOFAI_EARLYGEOMETRY_H

namespace AgeOfAI {
namespace Early {

const int kCoreTowers = 2;   // the towers of the town's core
const int kCoreDistance = 4; // tiles from the base

// The unit vector from a base towards the map's centre: the front when no defence plan names one. (1, 0) when the base
// is within a tile of the centre.
void FrontToCentre(int inBaseX, int inBaseY, int inMapSize, float& outDX, float& outDY);

// The tile of core tower slot inSlot: 0 towards the front (inDX, inDY, a unit vector), 1 across the front on the side
// of the citizens (inSumX, inSumY: the sum of their offsets from the base). kCoreDistance tiles out.
void CoreSlot(int inBaseX, int inBaseY, float inDX, float inDY, int inSlot, float inSumX, float inSumY, int& outX,
              int& outY);

// Where a citizen walks to lead an enemy under a tower: 1.5 tiles past the tower's centre, away from the enemy
// (the tower and the enemy as tiles). The tower's centre when the two stand on one tile.
void BaitSpot(int inTowerX, int inTowerY, int inEnemyX, int inEnemyY, float& outX, float& outY);

}
}

#endif
