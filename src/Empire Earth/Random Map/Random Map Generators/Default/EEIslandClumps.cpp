// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EEIslandClumps.cpp (file name inferred; RTTI
// .?AVEEIslandClumps@@ proves the class)
// Unit flags (hypothesis, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "ee/EERandomMap.h"
#include "EEClumps.h"
#include "EELandClumps.h"
#include "EEIslandClumps.h"

// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x100093f8
// SYNTHETIC: DEFAULT 0x10009257
// SYNTHETIC: DEFAULT 0x10009261
// SYNTHETIC: DEFAULT 0x10009272
// SYNTHETIC: DEFAULT 0x1000927e
// SYNTHETIC: DEFAULT 0x10009289
// SYNTHETIC: DEFAULT 0x10009293
// SYNTHETIC: DEFAULT 0x100092a4
// SYNTHETIC: DEFAULT 0x100092b0
// SYNTHETIC: DEFAULT 0x100092bb
// SYNTHETIC: DEFAULT 0x100092c5
// SYNTHETIC: DEFAULT 0x100092d6
// SYNTHETIC: DEFAULT 0x100092e2
// SYNTHETIC: DEFAULT 0x100092ed
// SYNTHETIC: DEFAULT 0x100092f7
// SYNTHETIC: DEFAULT 0x10009308
// SYNTHETIC: DEFAULT 0x10009314
// SYNTHETIC: DEFAULT 0x1000931f
// SYNTHETIC: DEFAULT 0x10009329
// SYNTHETIC: DEFAULT 0x1000933a
// SYNTHETIC: DEFAULT 0x10009346
// SYNTHETIC: DEFAULT 0x10009351
// SYNTHETIC: DEFAULT 0x1000935b
// SYNTHETIC: DEFAULT 0x1000936c
// SYNTHETIC: DEFAULT 0x10009378
// SYNTHETIC: DEFAULT 0x10009383
// SYNTHETIC: DEFAULT 0x1000938d
// SYNTHETIC: DEFAULT 0x1000939e
// SYNTHETIC: DEFAULT 0x100093aa
// SYNTHETIC: DEFAULT 0x100096bf
// SYNTHETIC: DEFAULT 0x100096c9

// FUNCTION: DEFAULT 0x100093b5
// Creates a land grower for one player that keeps the map's river width away from other players
EEIslandClumps::EEIslandClumps(EERandomMap* inMap, EECellGrid* inClaimed, std::vector<unsigned long>* inDistances,
                               unsigned long inClumpCount, float inSpread, unsigned long inTargetSize,
                               long inPlayer, bool inFlag)
    : EELandClumps(inMap, inClaimed, inDistances, inClumpCount, inSpread, inTargetSize, inPlayer, inFlag)
{
    this->mRiverWidth = inMap->mRiverWidth;
}

// FUNCTION: DEFAULT 0x10009414
// Destroys the island grower (members only)
EEIslandClumps::~EEIslandClumps()
{
}

// FUNCTION: DEFAULT 0x1000941f
// Returns whether a tile is valid land for this player and not too close to another player's clumps
bool EEIslandClumps::IsValidLocation(long inX, long inY)
{
    return EELandClumps::IsValidLocation(inX, inY) && !this->IsNearOther(inX, inY);
}

// FUNCTION: DEFAULT 0x10009451
// Returns whether a tile lies within a random 3/4..1 river width of a clump circle of another player
// (of another team when the map uses team growth)
bool EEIslandClumps::IsNearOther(unsigned long inX, unsigned long inY)
{
    GE2DPoint thePoint((float)inX, (float)inY);
    unsigned long theMinimum = 1;
    unsigned long i;
    unsigned long j = 0;
    unsigned long theCount;
    if (this->mRiverWidth > 1)
        theMinimum = this->mRiverWidth * 3 / 4;
    bool theFound = false;
    float theDistance = (float)this->mMap->mRandomIntegers.Generate(theMinimum, this->mRiverWidth);
    if (!this->mMap->mTeamGrowth) {
        theCount = this->mMap->mPlayers.size();
        for (i = 0; i < theCount; i++)
            if (i != this->mType) {
                while (j < this->mMap->mClumpCircles[i].size() && !theFound) {
                    float theRange = theDistance + this->mMap->mClumpCircles[i][j].mRadius;
                    theFound = thePoint.DistanceToSquared(this->mMap->mClumpCircles[i][j].mCenter) <= theRange * theRange;
                    j++;
                }
                j = 0;
            }
    } else {
        theCount = this->mMap->mPlayers.size();
        for (i = 0; i < theCount; i++)
            if (this->mMap->mPlayers[i].mTeam != this->mMap->mPlayers[this->mType].mTeam)
                for (j = 0; j < this->mMap->mClumpCircles[i].size() && !theFound; j++) {
                    float theRange = theDistance + this->mMap->mClumpCircles[i][j].mRadius;
                    theFound = thePoint.DistanceToSquared(this->mMap->mClumpCircles[i][j].mCenter) <= theRange * theRange;
                }
    }
    return theFound;
}
