// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EETreeClumps.cpp (file name inferred; RTTI
// .?AVEETreeClumps@@ proves the class)
// Unit flags (hypothesis, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "ee/EERandomMap.h"
#include "EEClumps.h"
#include "EETreeClumps.h"
#include "CMU Map Generator.h"
#include "Unit12.h"
#include "ee/EERandomMapIDs.h"


// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x1000a6a9
// SYNTHETIC: DEFAULT 0x1000a840
// SYNTHETIC: DEFAULT 0x1000a86c
// SYNTHETIC: DEFAULT 0x1000a8bc
// SYNTHETIC: DEFAULT 0x1000ab5a
// SYNTHETIC: DEFAULT 0x1000ab81
// SYNTHETIC: DEFAULT 0x1000a3e0
// SYNTHETIC: DEFAULT 0x1000a3ea
// SYNTHETIC: DEFAULT 0x1000a3fb
// SYNTHETIC: DEFAULT 0x1000a407
// SYNTHETIC: DEFAULT 0x1000a412
// SYNTHETIC: DEFAULT 0x1000a41c
// SYNTHETIC: DEFAULT 0x1000a42d
// SYNTHETIC: DEFAULT 0x1000a439
// SYNTHETIC: DEFAULT 0x1000a444
// SYNTHETIC: DEFAULT 0x1000a44e
// SYNTHETIC: DEFAULT 0x1000a45f
// SYNTHETIC: DEFAULT 0x1000a46b
// SYNTHETIC: DEFAULT 0x1000a476
// SYNTHETIC: DEFAULT 0x1000a480
// SYNTHETIC: DEFAULT 0x1000a491
// SYNTHETIC: DEFAULT 0x1000a49d
// SYNTHETIC: DEFAULT 0x1000a4a8
// SYNTHETIC: DEFAULT 0x1000a4b2
// SYNTHETIC: DEFAULT 0x1000a4c3
// SYNTHETIC: DEFAULT 0x1000a4cf
// SYNTHETIC: DEFAULT 0x1000a4da
// SYNTHETIC: DEFAULT 0x1000a4e4
// SYNTHETIC: DEFAULT 0x1000a4f5
// SYNTHETIC: DEFAULT 0x1000a501
// SYNTHETIC: DEFAULT 0x1000a50c
// SYNTHETIC: DEFAULT 0x1000a516
// SYNTHETIC: DEFAULT 0x1000a527
// SYNTHETIC: DEFAULT 0x1000a533
// SYNTHETIC: DEFAULT 0x1000abaf
// SYNTHETIC: DEFAULT 0x1000abb9

// FUNCTION: DEFAULT 0x1000a53e
// Creates a forest grower: keeps the forest-free radius around every non-neutral start position and
// picks one random tree type (planet maps pick per tile from the terrain band instead)
EETreeClumps::EETreeClumps(EERandomMap* inMap, EECellGrid* inClaimed, float inSpread, unsigned long inClumpCount,
                           unsigned long inTargetSize, std::vector<unsigned long>* inDistances, long inType,
                           bool inFlag)
    : EEClumps(inMap, inClaimed, inSpread, inClumpCount, inTargetSize, inType, inDistances),
      mTiles(&inMap->mTiles), mFlatTiles(&inMap->mFlatTiles), mObjects(&inMap->mObjects), mHeights(&inMap->mHeights)
{
    unsigned long thePlayerCount = inMap->mPlayers.size();
    this->mResourceToEdgeDistance = inMap->mResourceToEdgeDistance;
    this->mResourceToWaterDistance = inMap->mResourceToWaterDistance;
    this->mLandElevationThreshold = inMap->mLandElevationThreshold;
    this->mFlag = inFlag;
    this->mMinimumDistanceSquared = inMap->mForestFreeRadius * inMap->mForestFreeRadius;
    for (unsigned long i = 0; i < thePlayerCount; i++)
        if (!inMap->mPlayers[i].mIsNeutral)
            this->mCentersToAvoid.push_back(inMap->mPlayers[i].mPosition);
    if ((inMap->mMapCharacteristics & kRMMapPlanetsMap) == kRMMapPlanetsMap)
        this->mTreeType = -1;
    else
        this->mTreeType = inMap->mTreeTypes[inMap->mRandomIntegers.Generate(0, inMap->mTreeTypes.size() - 1)];
}

// FUNCTION: DEFAULT 0x1000a6c5
// Destroys the forest grower, freeing its list of centres to avoid
EETreeClumps::~EETreeClumps()
{
}

// FUNCTION: DEFAULT 0x1000a6e2
// Returns whether a tree may grow on a tile: allowed, outside every forest-free radius, placeable
bool EETreeClumps::IsValidLocation(long inX, long inY)
{
    GE2DPoint thePoint((float)inX, (float)inY);
    return CMUIsCellSet(*this->GetAllowed(), inX, inY) && this->IsFarFromCenters(thePoint) && CanPlaceResource(this->GetMap(), kRMObjectTree, inX, inY);
}

// FUNCTION: DEFAULT 0x1000a73e
// Returns whether a point is at least the forest-free radius away from every player start
bool EETreeClumps::IsFarFromCenters(const GE2DPoint& inPoint)
{
    unsigned long theCount = this->mCentersToAvoid.size();
    bool theResult = true;
    for (unsigned long i = 0; i < theCount && theResult; i++)
        theResult = inPoint.DistanceToSquared(this->mCentersToAvoid[i]) >= this->mMinimumDistanceSquared;
    return theResult;
}

// FUNCTION: DEFAULT 0x1000a798
// Plants a tree object of the forest's type (or the tile's band type) on a tile and claims the tile
void EETreeClumps::Claim(long inX, long inY, bool inMark)
{
    EERMObject theObject;
    long theType = this->mTreeType;
    if (theType == -1)
        theType = this->GetMap()->mTreeTypes[((*this->mTiles)[inX][inY] + this->GetMap()->mTerrainBandOffset) % this->GetMap()->mNumTerrainBands];
    theObject.mLocation.SetX((float)inX);
    theObject.mLocation.SetY((float)inY);
    theObject.mKind = kRMObjectTree;
    theObject.mType = theType;
    this->mObjects->push_back(theObject);
    EEClumps::Claim(inX, inY, inMark);
}
