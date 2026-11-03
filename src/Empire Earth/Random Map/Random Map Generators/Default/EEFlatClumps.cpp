// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EEFlatClumps.cpp (file name inferred; RTTI
// .?AVEEFlatClumps@@ proves the class)
// Unit flags (hypothesis, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "ee/EERandomMap.h"
#include "EEClumps.h"
#include "EEFlatClumps.h"
#include "CMU Map Generator.h"
#include "Unit11.h"


// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x100090a3
// SYNTHETIC: DEFAULT 0x10008ef8
// SYNTHETIC: DEFAULT 0x10008f02
// SYNTHETIC: DEFAULT 0x10008f13
// SYNTHETIC: DEFAULT 0x10008f1f
// SYNTHETIC: DEFAULT 0x10008f2a
// SYNTHETIC: DEFAULT 0x10008f34
// SYNTHETIC: DEFAULT 0x10008f45
// SYNTHETIC: DEFAULT 0x10008f51
// SYNTHETIC: DEFAULT 0x10008f5c
// SYNTHETIC: DEFAULT 0x10008f66
// SYNTHETIC: DEFAULT 0x10008f77
// SYNTHETIC: DEFAULT 0x10008f83
// SYNTHETIC: DEFAULT 0x10008f8e
// SYNTHETIC: DEFAULT 0x10008f98
// SYNTHETIC: DEFAULT 0x10008fa9
// SYNTHETIC: DEFAULT 0x10008fb5
// SYNTHETIC: DEFAULT 0x10008fc0
// SYNTHETIC: DEFAULT 0x10008fca
// SYNTHETIC: DEFAULT 0x10008fdb
// SYNTHETIC: DEFAULT 0x10008fe7
// SYNTHETIC: DEFAULT 0x10008ff2
// SYNTHETIC: DEFAULT 0x10008ffc
// SYNTHETIC: DEFAULT 0x1000900d
// SYNTHETIC: DEFAULT 0x10009019
// SYNTHETIC: DEFAULT 0x10009024
// SYNTHETIC: DEFAULT 0x1000902e
// SYNTHETIC: DEFAULT 0x1000903f
// SYNTHETIC: DEFAULT 0x1000904b
// SYNTHETIC: DEFAULT 0x10009241
// SYNTHETIC: DEFAULT 0x1000924b

// FUNCTION: DEFAULT 0x10009056
// Creates a flat-area grower for one player that claims tiles in the map's flat-tile grid
EEFlatClumps::EEFlatClumps(EERandomMap* inMap, std::vector<unsigned long>* inDistances, unsigned long inClumpCount,
                           float inSpread, unsigned long inTargetSize, long inType, bool inFlag)
    : EEClumps(inMap, &inMap->mFlatTiles, inSpread, inClumpCount, inTargetSize, inType, inDistances),
      mPlayers(&inMap->mPlayers), mFlag(inFlag)
{
}

// FUNCTION: DEFAULT 0x100090bf
// Destroys the flat-area grower (members only)
EEFlatClumps::~EEFlatClumps()
{
}

// FUNCTION: DEFAULT 0x100090ca
// Starts a clump at a location; for a player's first clump also claims every valid free tile of the
// map's flat area around it. Returns whether the clump started
bool EEFlatClumps::AddLocation(const GE2DPoint& inLocation)
{
    EERandomMap* theMap = this->mMap;
    bool theResult = EEClumps::AddLocation(inLocation);
    if (theResult && this->mIndex == 0 && !this->mFlag) {
        long theLeft = UMFloor(theMap->mFlatArea.GetLeft() + inLocation.GetX());
        long theTop = UMFloor(theMap->mFlatArea.GetBottom() + inLocation.GetY());
        long theRight = UMFloor(theMap->mFlatArea.GetRight() + inLocation.GetX()) + 1;
        long theBottom = UMFloor(theMap->mFlatArea.GetTop() + inLocation.GetY()) + 1;
        for (long x = theLeft; x <= theRight; x++)
            for (long y = theTop; y <= theBottom; y++)
                if (CMUIsCellSet(theMap->mValidTiles, x, y) && this->IsFree(x, y))
                    this->Claim(x, y, true);
    }
    return theResult;
}

// FUNCTION: DEFAULT 0x100091f9
// Returns whether a tile is free and already belongs to this grower's player land (or neutral land)
bool EEFlatClumps::IsValidLocation(long inX, long inY)
{
    return EEClumps::IsValidLocation(inX, inY) && IsTileOfType(this->GetMap()->mTiles, inX, inY, this->mType, this->mFlag);
}
