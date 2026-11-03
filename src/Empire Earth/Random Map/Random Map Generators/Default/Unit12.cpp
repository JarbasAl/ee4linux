// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\Unit12.cpp (file name inferred: resource and
// object placement around the players; called by CMUGenerateResources 0x307e, CMUCreateTrees 0x1d93, EETreeClumps
// and the script reader)
// Unit flags (calibrated, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <math.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "ee/EERandomMap.h"
#include "CMU Map Generator.h"
#include "Unit11.h"
#include "Unit12.h"
#include "ee/EERandomMapIDs.h"

// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x10015c98
// SYNTHETIC: DEFAULT 0x10015ca2
// SYNTHETIC: DEFAULT 0x10015cb3
// SYNTHETIC: DEFAULT 0x10015cbf
// SYNTHETIC: DEFAULT 0x10015cca
// SYNTHETIC: DEFAULT 0x10015cd4
// SYNTHETIC: DEFAULT 0x10015ce5
// SYNTHETIC: DEFAULT 0x10015cf1
// SYNTHETIC: DEFAULT 0x10015cfc
// SYNTHETIC: DEFAULT 0x10015d06
// SYNTHETIC: DEFAULT 0x10015d17
// SYNTHETIC: DEFAULT 0x10015d23
// SYNTHETIC: DEFAULT 0x10015d2e
// SYNTHETIC: DEFAULT 0x10015d38
// SYNTHETIC: DEFAULT 0x10015d49
// SYNTHETIC: DEFAULT 0x10015d55
// SYNTHETIC: DEFAULT 0x10015d60
// SYNTHETIC: DEFAULT 0x10015d6a
// SYNTHETIC: DEFAULT 0x10015d7b
// SYNTHETIC: DEFAULT 0x10015d87
// SYNTHETIC: DEFAULT 0x10015d92
// SYNTHETIC: DEFAULT 0x10015d9c
// SYNTHETIC: DEFAULT 0x10015dad
// SYNTHETIC: DEFAULT 0x10015db9
// SYNTHETIC: DEFAULT 0x10015dc4
// SYNTHETIC: DEFAULT 0x10015dce
// SYNTHETIC: DEFAULT 0x10015ddf
// SYNTHETIC: DEFAULT 0x10015deb
// SYNTHETIC: DEFAULT 0x10015df6
// SYNTHETIC: DEFAULT 0x10015e00
// SYNTHETIC: DEFAULT 0x10015e18
// SYNTHETIC: DEFAULT 0x10015e24
// SYNTHETIC: DEFAULT 0x1001811a
// SYNTHETIC: DEFAULT 0x10018add
// SYNTHETIC: DEFAULT 0x10018ae7

std::vector<long> gPlayerOrder;             // 0x1c930: players in placement order (BuildPlayerOrder)
unsigned long gPlayerOrderIndex;            // 0x1c9b0: next entry of gPlayerOrder
const float kFootprintOffset = 2.0f;        // 0x19330: unit-local .rdata (not a shared __real constant)

// FUNCTION: DEFAULT 0x10015e2e
// Places a herd's animals on random valid, unclaimed, gentle tiles in the square around a point
void PlaceResourcesNear(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind,
                        unsigned long inX, unsigned long inY, unsigned long inRadius, unsigned long inPlayer)
{
    std::vector<GE2DPoint> theCandidates;
    unsigned long theSize = ioMap->mGridSize;
    unsigned long theLeft = inX - inRadius;
    unsigned long theRight = inX + inRadius;
    unsigned long theTop = inY - inRadius;
    unsigned long theBottom = inY + inRadius;
    if (theLeft >= theSize)
        theLeft = 0;
    if (theTop >= theSize)
        theTop = 0;
    if (theRight >= theSize)
        theRight = theSize - 1;
    if (theBottom >= theSize)
        theBottom = theSize - 1;
    for (unsigned long i = theLeft; i <= theRight; i++)
        for (unsigned long j = theTop; j <= theBottom; j++)
            if (ioMap->mValidTiles[i][j] && !ioMap->mClaimedTiles[i][j] && !IsSteep(ioMap, i, j))
                theCandidates.push_back(GE2DPoint(i + 0.5f, j + 0.5f));
    unsigned long theCount = min(theCandidates.size(), inKind.mArea);
    for (unsigned long k = 0; k < theCount; k++) {
        long theChoice = ioMap->mRandomIntegers.Generate(0, theCandidates.size() - 1);
        PlaceResource(ioMap, ioSites, inKind, theCandidates[theChoice], inPlayer);
        theCandidates.erase(theCandidates.begin() + theChoice);
    }
}

// FUNCTION: DEFAULT 0x10016035
// Adds one herd animal near a point with a random facing, claims a 4x4 square and a path to the
// owner's nearest site, and records the point as a site
void PlaceResource(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind,
                   const GE2DPoint& inPosition, unsigned long inPlayer)
{
    GE2DPoint theNearest;
    unsigned long theX = UMFloorUnsigned(inPosition.mX - kFootprintOffset);
    unsigned long theY = UMFloorUnsigned(inPosition.mY - kFootprintOffset);
    EERMFeature theFeature;
    theFeature.mAngle = ioMap->mRandom.GenerateUnit() * 6.2831855f - 3.1415927f;
    theFeature.mPosition.mX = (ioMap->mRandom.GenerateUnit() - 0.5f) * 0.75f + inPosition.mX;
    theFeature.mPosition.mY = (ioMap->mRandom.Generate() - 0.5f) * 0.75f + inPosition.mY;
    theFeature.mUnitType = inKind.mUnitType;
    theFeature.mIndex = inPlayer;
    theFeature.mIsWorldUnit = true;
    theFeature.mInRing = false;
    theFeature.mIsHerd = true;
    ioMap->mFeatures.push_back(theFeature);
    MarkSquare(ioMap->mClaimedTiles, theX, theY, 4);
    theNearest = ioSites[inPlayer][0];
    for (unsigned long i = 0; i < ioSites[inPlayer].size(); i++)
        if (inPosition.DistanceToSquared(ioSites[inPlayer][i]) <= inPosition.DistanceToSquared(theNearest))
            theNearest = ioSites[inPlayer][i];
    MarkLine(ioMap->mClaimedTiles, inPosition, theNearest, 0.75f);
    ioSites[inPlayer].push_back(inPosition);
}

// FUNCTION: DEFAULT 0x100162a1
// Returns whether the heights of a 2x2 tile block differ by more than one elevation step
bool IsSteep(EERandomMap* inMap, unsigned long inX, unsigned long inY)
{
    if (!CMUIsCellSet(inMap->mValidTiles, inX, inY) || !CMUIsCellSet(inMap->mValidTiles, inX, inY + 1) ||
        !CMUIsCellSet(inMap->mValidTiles, inX + 1, inY) || !CMUIsCellSet(inMap->mValidTiles, inX + 1, inY + 1))
        return false;
    float theMaximum = inMap->mHeights[inX][inY];
    float theMinimum = theMaximum;
    float theHeight = inMap->mHeights[inX + 1][inY];
    if (theHeight < theMinimum)
        theMinimum = theHeight;
    else if (theHeight > theMaximum)
        theMaximum = theHeight;
    theHeight = inMap->mHeights[inX][inY + 1];
    if (theHeight < theMinimum)
        theMinimum = theHeight;
    else if (theHeight > theMaximum)
        theMaximum = theHeight;
    theHeight = inMap->mHeights[inX + 1][inY + 1];
    if (theHeight < theMinimum)
        theMinimum = theHeight;
    else if (theHeight > theMaximum)
        theMaximum = theHeight;
    if (theMaximum - theMinimum > inMap->mElevationUnit)
        return true;
    return false;
}

// FUNCTION: DEFAULT 0x10016461
// Returns whether a resource (or tree) fits on a tile: not a start position, unclaimed, within the
// elevation range, away from map edge and water, on flat ground (trees: a band with a tree type)
bool CanPlaceResource(EERandomMap* inMap, long inKind, unsigned long inX, unsigned long inY)
{
    bool theFree = true;
    for (unsigned long i = 0; i < inMap->mPlayers.size() && theFree; i++)
        if (UMFloorUnsigned(inMap->mPlayers[i].mPosition.mX) == inX &&
            UMFloorUnsigned(inMap->mPlayers[i].mPosition.mY) == inY)
            theFree = false;
        else
            theFree = true;
    if (!theFree)
        return false;
    if (inKind == kRMObjectTree) {
        if (!inMap->mClaimedTiles[inX][inY] && inMap->mHeights[inX][inY] <= inMap->mMaxTreeElevation &&
            inMap->mHeights[inX][inY] >= inMap->mLandElevationThreshold && !IsSquareOpen(inMap->mValidTiles, inX, inY, inMap->mResourceToEdgeDistance) &&
            !IsAnyMaskedTileClear(inMap->mValidTiles, inMap->mTiles, inX, inY, inMap->mResourceToWaterDistance) &&
            CountSteepCells(inMap, inX, inY, 1) == 0 && inMap->mTiles[inX][inY] < 0x8000) {
            if ((inMap->mMapCharacteristics & kRMMapPlanetsMap) != kRMMapPlanetsMap)
                return true;
            if ((inMap->mTerrainBandOffset + inMap->mTiles[inX][inY]) % inMap->mNumTerrainBands < inMap->mTreeTypes.size())
                return true;
        }
    } else if (!inMap->mClaimedTiles[inX][inY] && inMap->mHeights[inX][inY] <= inMap->mMaxResourceElevation &&
               inMap->mHeights[inX][inY] >= inMap->mLandElevationThreshold && !IsSquareOpen(inMap->mValidTiles, inX, inY, inMap->mResourceToEdgeDistance) &&
               !IsAnyMaskedTileClear(inMap->mValidTiles, inMap->mTiles, inX, inY, inMap->mResourceToWaterDistance) &&
               CountSteepCells(inMap, inX, inY, 1) == 0)
        return true;
    return false;
}

// FUNCTION: DEFAULT 0x10016837
// Counts the steep 2x2 blocks in the square around a tile
unsigned long CountSteepCells(EERandomMap* inMap, unsigned long inX, unsigned long inY, unsigned long inRadius)
{
    unsigned long theSize = inMap->mValidTiles.size();
    unsigned long theCount;   // assigned after inY's last read: it takes inY's slot (needed for exactness)
    unsigned long theLeft = inX - inRadius;
    unsigned long theRight = inX + inRadius;
    unsigned long theTop = inY - inRadius;
    unsigned long theBottom = inY + inRadius;
    theCount = 0;
    if (theLeft >= theSize)
        theLeft = 0;
    if (theTop >= theSize)
        theTop = 0;
    if (theRight >= theSize)
        theRight = theSize - 1;
    if (theBottom >= theSize)
        theBottom = theSize - 1;
    for (unsigned long i = theLeft; i <= theRight; i++)
        for (unsigned long j = theTop; j <= theBottom; j++)
            if (IsSteep(inMap, i, j))
                theCount++;
    return theCount;
}

// FUNCTION: DEFAULT 0x100168cb
// Empties the neutral placement order and restarts it
void ResetPlayerOrder()
{
    gPlayerOrder.clear();
    gPlayerOrderIndex = 0;
}

// FUNCTION: DEFAULT 0x100168dd
// Places every herd slot from the script: per-player slots around each player, the others on
// neutral land; random slots pick a herd kind from the random herd set
void PlaceResources(EERandomMap* ioMap, EERMSiteLists& ioSites)
{
    for (unsigned long i = 0; i < ioMap->mResourceSlots.size(); i++) {
        EERMResourceSlot theSlot = ioMap->mResourceSlots[i];
        long theId;
        unsigned long k;
        if (theSlot.mPerPlayer) {
            for (unsigned long p = 0; p < ioMap->mPlayers.size(); p++)
                if (!ioMap->mPlayers[p].mIsNeutral) {
                    if (theSlot.mRandomKind) {
                        theId = ioMap->mResourceKindIds[ioMap->mRandomIntegers.Generate(0,
                                                        ioMap->mResourceKindIds.size() - 1)];
                        for (k = 0; k < ioMap->mResourceKinds.size(); k++)
                            if (ioMap->mResourceKinds[k].mId == theId) {
                                PlaceResourceAroundPlayer(ioMap, ioSites, ioMap->mResourceKinds[k], p);
                                break;
                            }
                    } else {
                        for (k = 0; k < ioMap->mResourceKinds.size(); k++)
                            if (ioMap->mResourceKinds[k].mId == theSlot.mId) {
                                PlaceResourceAroundPlayer(ioMap, ioSites, ioMap->mResourceKinds[k], p);
                                break;
                            }
                    }
                }
        } else if (theSlot.mRandomKind) {
            theId = ioMap->mResourceKindIds[ioMap->mRandomIntegers.Generate(0, ioMap->mResourceKindIds.size() - 1)];
            for (k = 0; k < ioMap->mResourceKinds.size(); k++)
                if (ioMap->mResourceKinds[k].mId == theId) {
                    PlaceResourceKind(ioMap, ioSites, ioMap->mResourceKinds[k]);
                    break;
                }
        } else {
            for (k = 0; k < ioMap->mResourceKinds.size(); k++)
                if (ioMap->mResourceKinds[k].mId == theSlot.mId) {
                    PlaceResourceKind(ioMap, ioSites, ioMap->mResourceKinds[k]);
                    break;
                }
        }
    }
}

// FUNCTION: DEFAULT 0x10016bb7
// Places one herd on the next neutral land mass in placement order, at a random site good enough
void PlaceResourceKind(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind)
{
    unsigned long theRadius = UMFloatToLong((float)sqrt(inKind.mArea) * 0.5f);
    unsigned long theSize = theRadius + 1;
    if (gPlayerOrder.empty())
        BuildPlayerOrder(ioMap);
    if (gPlayerOrder.empty())
        return;
    long thePlayer = gPlayerOrder[gPlayerOrderIndex % gPlayerOrder.size()];
    gPlayerOrderIndex++;
    for (unsigned long i = 0; i < ioMap->mMaxLoopCount; i++) {
        long theIndex = ioMap->mRandomIntegers.Generate(0, ioMap->mPlayers[thePlayer].mTileCount - 1);
        GE2DPoint thePoint = ioMap->mPlayers[thePlayer].mTiles[theIndex];
        unsigned long theX = UMFloorUnsigned(thePoint.mX);
        unsigned long theY = UMFloorUnsigned(thePoint.mY);
        if (IsGoodResourceSite(ioMap, inKind, theSize, theX, theY)) {
            PlaceResourcesNear(ioMap, ioSites, inKind, theX, theY, theSize, thePlayer);
            break;
        }
    }
}

// FUNCTION: DEFAULT 0x10016d67
// Returns whether a herd fits around a tile: not a start position, all tiles valid and unclaimed,
// within the elevation range, away from edge and water, with enough gentle tiles for its area
bool IsGoodResourceSite(EERandomMap* inMap, const EERMResourceKind& inKind, unsigned long inRadius,
                        unsigned long inX, unsigned long inY)
{
    bool theResult = true;
    for (unsigned long i = 0; i < inMap->mPlayers.size() && theResult; i++)
        if (UMFloorUnsigned(inMap->mPlayers[i].mPosition.mX) == inX &&
            UMFloorUnsigned(inMap->mPlayers[i].mPosition.mY) == inY)
            theResult = false;
        else
            theResult = true;
    if (theResult) {
        unsigned long theSquare = max(inMap->mResourceToEdgeDistance, inRadius);
        unsigned long theClear = max(inMap->mResourceToWaterDistance, inRadius);
        if (AreAllMasked(inMap->mValidTiles, inMap->mClaimedTiles, inX, inY, inRadius) &&
            inMap->mHeights[inX][inY] <= inMap->mMaxResourceElevation && inMap->mHeights[inX][inY] >= inMap->mLandElevationThreshold &&
            !IsSquareOpen(inMap->mValidTiles, inX, inY, theSquare) &&
            !IsAnyMaskedTileClear(inMap->mValidTiles, inMap->mTiles, inX, inY, theClear))
            theResult = inRadius * inRadius * 4 - CountSteepCells(inMap, inX, inY, inRadius) >= inKind.mArea;
        else
            theResult = false;
    }
    return theResult;
}

// FUNCTION: DEFAULT 0x10016f09
// Places one herd at a random angle and distance (within the kind's range) from a player's start
void PlaceResourceAroundPlayer(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind,
                               unsigned long inPlayer)
{
    float theCenterX = ioMap->mPlayers[inPlayer].mPosition.mX;
    float theCenterY = ioMap->mPlayers[inPlayer].mPosition.mY;
    unsigned long theRadius = UMFloatToLong((float)sqrt(inKind.mArea) * 0.5f);
    unsigned long theSize = theRadius + 1;
    for (unsigned long i = 0; i < ioMap->mMaxLoopCount; i++) {
        float theAngle = ioMap->mRandom.Generate() * 6.2831855f - 3.1415927f;
        float theDistance = ioMap->mRandom.Generate() * (inKind.mMaximum - inKind.mMinimum) + inKind.mMinimum;
        float theX = UMCos(theAngle) * theDistance + theCenterX;
        float theY = UMCos(1.5707964f - theAngle) * theDistance + theCenterY;
        if (!(theX >= 0.0f) || !(theY >= 0.0f))
            continue;
        unsigned long theCellX = UMFloorUnsigned(theX);
        unsigned long theCellY = UMFloorUnsigned(theY);
        if (IsGoodResourceSite(ioMap, inKind, theSize, theCellX, theCellY)) {
            PlaceResourcesNear(ioMap, ioSites, inKind, theCellX, theCellY, theSize, inPlayer);
            break;
        }
    }
}

// FUNCTION: DEFAULT 0x100170a8
// Places the moons' resources: each moon gets its resource count of gold or iron on usable tiles
void PlacePlayerObjects(EERandomMap* ioMap)
{
    for (std::vector<EERMPlayer>::iterator i = ioMap->mPlayers.begin(); i != ioMap->mPlayers.end(); i++)
        if (i->mIsMoon) {
            unsigned long theCount = ioMap->mMoonResources;
            unsigned long thePlaced = 0;
            for (unsigned long theTries = 0; thePlaced < theCount && theTries < theCount * 64; theTries++) {
                long theIndex = ioMap->mRandomIntegers.Generate(0, i->mTileCount - 1);
                GE2DPoint thePoint = i->mTiles[theIndex];
                unsigned long theX = UMFloorUnsigned(thePoint.mX);
                unsigned long theY = UMFloorUnsigned(thePoint.mY);
                if (IsSquareOpen(ioMap->mValidTiles, theX, theY, 1) ||
                    IsAnyMaskedTileClear(ioMap->mValidTiles, ioMap->mTiles, theX, theY, 1) ||
                    ioMap->mClaimedTiles[theX][theY] == 1)
                    continue;
                EERMObject theObject;
                theObject.mLocation = thePoint;
                theObject.mKind = ioMap->mRandomIntegers.URandom::Generate() >= 0.5f ? 6 : 1;
                ioMap->mObjects.push_back(theObject);
                MarkSquare(ioMap->mClaimedTiles, theX, theY, 4);
                thePlaced++;
            }
        }
}

// FUNCTION: DEFAULT 0x1001725d
// Places one resource kind for every player, then on the neutral land masses in placement order
void PlaceObjects(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind)
{
    unsigned long theCount = ioMap->mPlayers.size();
    for (unsigned long i = 0; i < theCount; i++)
        if (!ioMap->mPlayers[i].mIsNeutral)
            PlaceObjectsForPlayer(ioMap, ioSites, inKind, i);
    PlaceObjectsByOrder(ioMap, ioSites, inKind);
}

// FUNCTION: DEFAULT 0x100172c9
// Places a player's share of one resource kind on its land (fish on shore water), preferring well
// spaced sites, else the candidate whose distance is closest to the kind's spacing limits
void PlaceObjectsForPlayer(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, unsigned long inPlayer)
{
    std::vector<long> theUnused;
    GE2DPoint thePosition = ioMap->mPlayers[inPlayer].mPosition;
    GE2DPoint theCandidate;
    GE2DPoint theBest;
    float theBestError = 0.0f;
    long theLast = ioMap->mPlayers[inPlayer].mTileCount - 1;
    unsigned long theTries = ioMap->mMaxLoopCount;
    if (ioMap->mPlayers[inPlayer].mTileCount != 0)
        for (long n = ioMap->mResourcePerPlayer[inKind]; n != 0; n--) {
            bool theDone = false;
            bool theFound = false;
            for (unsigned long t = 0; t < theTries && !theDone; t++) {
                unsigned long theX;
                unsigned long theY;
                bool theUsable;
                if (inKind == kRMObjectFish) {
                    theCandidate = ioMap->mWaterPoints[ioMap->mRandomIntegers.Generate(0, ioMap->mWaterPoints.size() - 1)];
                    theX = UMFloorUnsigned(theCandidate.mX);
                    theY = UMFloorUnsigned(theCandidate.mY);
                    theUsable = IsShoreSite(ioMap, theX, theY);
                } else {
                    theCandidate = ioMap->mPlayers[inPlayer].mTiles[ioMap->mRandomIntegers.Generate(0, theLast)];
                    theX = UMFloorUnsigned(theCandidate.mX);
                    theY = UMFloorUnsigned(theCandidate.mY);
                    theUsable = CanPlaceResource(ioMap, inKind, theX, theY);
                }
                if (!theUsable)
                    continue;
                float theDistance = theCandidate.DistanceToSquared(thePosition);
                if (!IsWellSpaced(ioMap, ioSites, inKind, theX, theY, inPlayer)) {
                    float theTarget = (float)ioMap->mKindSpacing[inKind][0];
                    if (theTarget <= theDistance)
                        theTarget = (float)ioMap->mKindSpacing[inKind][1];
                    float theError = theDistance - theTarget;
                    if (theError < 0.0f)
                        theError = -theError;
                    if (!theFound) {
                        theBest = theCandidate;
                        theFound = true;
                        theBestError = theError;
                    } else if (theError < theBestError) {
                        theBest = theCandidate;
                        theBestError = theError;
                    }
                } else {
                    PlaceObject(ioMap, ioSites, inKind, theCandidate, inPlayer);
                    theDone = true;
                }
            }
            if (theFound && !theDone)
                PlaceObject(ioMap, ioSites, inKind, theBest, inPlayer);
        }
}

// FUNCTION: DEFAULT 0x1001762d
// Places one resource object at a position for a player
void PlaceObject(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, const GE2DPoint& inPosition,
                 unsigned long inPlayer)
{
    PlaceObjectAt(ioMap, ioSites, inKind, inPosition, inPlayer);
}

// FUNCTION: DEFAULT 0x10017649
// Adds a resource object (space-map fish become stone, gold or iron; trees get a type), claims a 4x4
// square and, except for fish, a path to the player's nearest site, recording a new site
void PlaceObjectAt(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, const GE2DPoint& inPosition,
                   unsigned long inPlayer)
{
    GE2DPoint theNearest;
    unsigned long theX = UMFloorUnsigned(inPosition.mX - kFootprintOffset);
    unsigned long theY = UMFloorUnsigned(inPosition.mY - kFootprintOffset);
    EERMObject theObject;
    theObject.mLocation = inPosition;
    theObject.mKind = inKind;
    if (inKind == kRMObjectFish && ioMap->mIsSpaceMap)
        switch (ioMap->mRandomIntegers.Generate(0, 2)) {
        case 0:
            theObject.mKind = kRMObjectStone;
            break;
        case 1:
            theObject.mKind = kRMObjectGold;
            break;
        default:
            theObject.mKind = kRMObjectIron;
            break;
        }
    if (inKind == kRMObjectTree) {
        unsigned long theType;
        if ((ioMap->mMapCharacteristics & kRMMapPlanetsMap) == kRMMapPlanetsMap)
            theType = (ioMap->mTerrainBandOffset + ioMap->mTiles[UMFloorUnsigned(inPosition.mX)][UMFloorUnsigned(inPosition.mY)]) %
                      ioMap->mNumTerrainBands;
        else
            theType = ioMap->mRandomIntegers.Generate(0, ioMap->mTreeTypes.size() - 1);
        theObject.mType = ioMap->mTreeTypes[theType];
    }
    ioMap->mObjects.push_back(theObject);
    MarkSquare(ioMap->mClaimedTiles, theX, theY, 4);
    if (inKind != kRMObjectFish) {
        theNearest = ioSites[inPlayer][0];
        for (unsigned long i = 0; i < ioSites[inPlayer].size(); i++)
            if (inPosition.DistanceToSquared(ioSites[inPlayer][i]) <= inPosition.DistanceToSquared(theNearest))
                theNearest = ioSites[inPlayer][i];
        MarkLine(ioMap->mClaimedTiles, inPosition, theNearest, 0.75f);
        ioSites[inPlayer].push_back(inPosition);
    }
}

// FUNCTION: DEFAULT 0x10017975
// Returns whether a tile keeps the resource separation from every site and, for its own player,
// lies within the kind's near/far range of the start position
bool IsWellSpaced(EERandomMap* inMap, EERMSiteLists& inSites, unsigned long inKind, unsigned long inX,
                  unsigned long inY, unsigned long inPlayer)
{
    GE2DPoint thePoint((float)inX, (float)inY);
    float theMinimum = UMSquare(inMap->mResourceSeperation);
    bool theResult = true;
    float theNear = (float)inMap->mKindSpacing[inKind][0];
    float theFar = (float)inMap->mKindSpacing[inKind][1];
    for (unsigned long p = 0; p < inSites.size() && theResult; p++)
        for (unsigned long i = 0; i < inSites[p].size() && theResult; i++)
            if (i == 0) {
                if (thePoint.mX == inSites[p][0].mX && thePoint.mY == inSites[p][0].mY)
                    theResult = false;
                else {
                    theResult = true;
                    if (p == inPlayer && !inMap->mPlayers[p].mIsNeutral) {
                        float theDistance = thePoint.DistanceToSquared(inSites[p][0]);
                        if (theDistance < theNear || theDistance > theFar)
                            theResult = false;
                        else
                            theResult = true;
                    }
                }
            } else {
                theResult = true;
                if (thePoint.DistanceToSquared(inSites[p][i]) < theMinimum)
                    theResult = false;
            }
    return theResult;
}

// FUNCTION: DEFAULT 0x10017b77
// Returns whether a tile is unclaimed water two steps below the land threshold with valid neighbours
bool IsShoreSite(EERandomMap* inMap, unsigned long inX, unsigned long inY)
{
    unsigned long theSize = inMap->mGridSize;
    if (!inMap->mClaimedTiles[inX][inY] &&
        inMap->mHeights[inX][inY] < inMap->mLandElevationThreshold - (inMap->mElevationUnit + inMap->mElevationUnit) && inX + 1 < theSize &&
        inMap->mValidTiles[inX + 1][inY] && inY + 1 < theSize && inMap->mValidTiles[inX][inY + 1] &&
        inMap->mValidTiles[inX + 1][inY + 1] && !IsSquareOpen(inMap->mValidTiles, inX, inY, 1))
        return true;
    return false;
}

// FUNCTION: DEFAULT 0x10017cd9
// Places the neutral-land share of a resource kind, one neutral land mass after another in order
void PlaceObjectsByOrder(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind)
{
    if (gPlayerOrder.empty())
        BuildPlayerOrder(ioMap);
    if (!gPlayerOrder.empty())
        for (unsigned long i = 0; i < ioMap->mResourcePerNeutralLand[inKind]; i++) {
            PlaceObjectsNearPlayer(ioMap, ioSites, inKind, gPlayerOrder[gPlayerOrderIndex % gPlayerOrder.size()]);
            gPlayerOrderIndex++;
        }
}

// FUNCTION: DEFAULT 0x10017d7d
// Places one resource kind on a land mass (one for neutrals, the player share otherwise), preferring
// well spaced sites, else the candidate farthest from the start
void PlaceObjectsNearPlayer(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, unsigned long inPlayer)
{
    ioMap->mPlayers[inPlayer];
    unsigned long theTries = ioMap->mMaxLoopCount;
    GE2DPoint theCandidate;
    GE2DPoint theBest;
    if (!ioMap->mPlayers[inPlayer].mIsMoon)
        for (long n = GetObjectCount(ioMap, inKind, inPlayer); n != 0; n--) {
            bool theDone = false;
            bool theFound = false;
            for (unsigned long t = 0; t < theTries && !theDone; t++)
                if (inKind == kRMObjectFish) {
                    if (!ioMap->mWaterPoints.empty()) {
                        theCandidate = ioMap->mWaterPoints[ioMap->mRandomIntegers.Generate(0, ioMap->mWaterPoints.size() - 1)];
                        unsigned long theX = UMFloorUnsigned(theCandidate.mX);
                        unsigned long theY = UMFloorUnsigned(theCandidate.mY);
                        if (IsShoreSite(ioMap, theX, theY)) {
                            PlaceObject(ioMap, ioSites, kRMObjectFish, theCandidate, inPlayer);
                            theDone = true;
                        }
                    }
                } else if (ioMap->mPlayers[inPlayer].mTileCount != 0) {
                    theCandidate = ioMap->mPlayers[inPlayer].mTiles[ioMap->mRandomIntegers.Generate(0,
                                                                  ioMap->mPlayers[inPlayer].mTileCount - 1)];
                    unsigned long theX = UMFloorUnsigned(theCandidate.mX);
                    unsigned long theY = UMFloorUnsigned(theCandidate.mY);
                    if (CanPlaceResource(ioMap, inKind, theX, theY)) {
                        if (IsWellSpaced(ioMap, ioSites, inKind, theX, theY, inPlayer)) {
                            PlaceObject(ioMap, ioSites, inKind, theCandidate, inPlayer);
                            theDone = true;
                        } else if (!theFound) {
                            theBest = theCandidate;
                            theFound = true;
                        } else
                            theBest = GetFartherPoint(ioMap, inPlayer, theBest, theCandidate);
                    }
                }
            if (theFound && !theDone)
                PlaceObject(ioMap, ioSites, inKind, theBest, inPlayer);
        }
}

// FUNCTION: DEFAULT 0x10018081
// Returns whichever of two points is farther from a player's start position
const GE2DPoint& GetFartherPoint(EERandomMap* inMap, unsigned long inPlayer, const GE2DPoint& inA,
                                 const GE2DPoint& inB)
{
    static GE2DPoint sResult;
    GE2DPoint thePosition = inMap->mPlayers[inPlayer].mPosition;
    if (thePosition.DistanceToSquared(inA) > thePosition.DistanceToSquared(inB))
        sResult = inA;
    else
        sResult = inB;
    return sResult;
}

// FUNCTION: DEFAULT 0x1001811b
// Returns how many objects of a kind a land mass gets: 1 for neutrals, the per-player count otherwise
long GetObjectCount(EERandomMap* inMap, unsigned long inKind, unsigned long inPlayer)
{
    if (inMap->mPlayers[inPlayer].mIsNeutral)
        return 1;
    return inMap->mResourcePerPlayer[inKind];
}

// FUNCTION: DEFAULT 0x10018143
// Places the script's PlaceResourceInRing objects around each player at a random distance in the
// rule's range, preferring well spaced sites
void PlaceSpacedObjects(EERandomMap* ioMap, EERMSiteLists& ioSites)
{
    GE2DPoint thePosition;
    GE2DPoint theCandidate;
    GE2DPoint theBest;
    unsigned long theCount = ioMap->mPlayers.size();
    for (unsigned long p = 0; p < theCount; p++)
        if (!ioMap->mPlayers[p].mIsNeutral) {
            thePosition = ioMap->mPlayers[p].mPosition;
            float theCenterX = ioMap->mPlayers[p].mPosition.mX;
            float theCenterY = ioMap->mPlayers[p].mPosition.mY;
            for (unsigned long k = 0; k < ioMap->mSpacingRules.size(); k++) {
                bool theDone = false;
                bool theFound = false;
                float theBestError = 0.0f;
                for (unsigned long t = 0; t < ioMap->mMaxLoopCount && !theDone; t++) {
                    float theAngle = ioMap->mRandom.GenerateUnit() * 6.2831855f - 3.1415927f;
                    float theFraction = ioMap->mRandom.GenerateUnit();
                    float theDistance = (ioMap->mSpacingRules[k].mMaximum - ioMap->mSpacingRules[k].mMinimum) *
                                        theFraction + ioMap->mSpacingRules[k].mMinimum;
                    theCandidate.mX = UMCos(theAngle) * theDistance + theCenterX;
                    theCandidate.mY = UMCos(1.5707964f - theAngle) * theDistance + theCenterY;
                    long theX = UMFloor(theCandidate.mX);
                    long theY = UMFloor(theCandidate.mY);
                    if (!CMUIsCellSet(ioMap->mValidTiles, theX, theY) ||
                        (!(ioMap->mSpacingRules[k].mKind != kRMObjectFish && CanPlaceResource(ioMap, ioMap->mSpacingRules[k].mKind,
                                                                                theX, theY) ||
                         ioMap->mSpacingRules[k].mKind == kRMObjectFish && IsShoreSite(ioMap, theX, theY))))
                        continue;
                    float theSquared = thePosition.DistanceToSquared(theCandidate);
                    if (!IsWellSpaced(ioMap, ioSites, ioMap->mSpacingRules[k].mKind, theX, theY, p)) {
                        long theTarget;
                        if (ioMap->mSpacingRules[k].mMinimum <= theSquared)
                            theTarget = ioMap->mSpacingRules[k].mMaximum;
                        else
                            theTarget = ioMap->mSpacingRules[k].mMinimum;
                        float theError = theSquared - theTarget;
                        if (theError < 0.0f)
                            theError = -theError;
                        if (!theFound) {
                            theBest = theCandidate;
                            theFound = true;
                            theBestError = theError;
                        } else if (theError < theBestError) {
                            theBest = theCandidate;
                            theBestError = theError;
                        }
                    } else {
                        PlaceObject(ioMap, ioSites, ioMap->mSpacingRules[k].mKind, theCandidate, p);
                        theDone = true;
                    }
                }
                if (theFound && !theDone)
                    PlaceObject(ioMap, ioSites, ioMap->mSpacingRules[k].mKind, theBest, p);
            }
        }
}

// FUNCTION: DEFAULT 0x100185ee
// Orders the neutral land masses for resource placement, alternating outer and inner ones and
// preferring (with some randomness) the one farthest from the last pick
void BuildPlayerOrder(EERandomMap* ioMap)
{
    std::vector<long> theSecond;
    std::vector<long> theFirst;
    gPlayerOrder.clear();
    unsigned long theCount = ioMap->mPlayers.size();
    for (unsigned long i = 0; i < theCount; i++)
        if (ioMap->mPlayers[i].mIsNeutral && ioMap->mPlayers[i].mTileCount != 0)
            (ioMap->mPlayers[i].mIsOuter ? theFirst : theSecond).push_back(i);
    if (theFirst.empty() && theSecond.empty())
        return;
    bool theFromFirst;
    long theIndex;
    if (theSecond.empty()) {
        theIndex = ioMap->mRandomIntegers.Generate(0, theFirst.size() - 1);
        gPlayerOrder.push_back(theFirst[theIndex]);
        theFromFirst = false;
        theFirst.erase(theFirst.begin() + theIndex);
    } else {
        theIndex = ioMap->mRandomIntegers.Generate(0, theSecond.size() - 1);
        gPlayerOrder.push_back(theSecond[theIndex]);
        theFromFirst = true;
        theSecond.erase(theSecond.begin() + theIndex);
    }
    while (!theFirst.empty() || !theSecond.empty()) {
        float theBest = -1.0f;
        unsigned long theChoice;
        unsigned long i;
        if (theFromFirst) {
            theChoice = theFirst.size();
            for (i = 0; i < theFirst.size(); i++) {
                float theDistance = ioMap->mPlayers[theFirst[i]].mPosition.DistanceToSquared(
                    ioMap->mPlayers[gPlayerOrder.back()].mPosition);
                if (theDistance > theBest) {
                    theBest = theDistance - ioMap->mRandom.GenerateUnit() * theDistance * 0.5f;
                    theChoice = i;
                }
            }
            if (theChoice < theFirst.size()) {
                gPlayerOrder.push_back(theFirst[theChoice]);
                theFirst.erase(theFirst.begin() + theChoice);
            }
            theFromFirst = false;
        } else {
            theChoice = theSecond.size();
            for (i = 0; i < theSecond.size(); i++) {
                float theDistance = ioMap->mPlayers[theSecond[i]].mPosition.DistanceToSquared(
                    ioMap->mPlayers[gPlayerOrder.back()].mPosition);
                if (theDistance > theBest) {
                    theBest = theDistance - ioMap->mRandom.GenerateUnit() * theDistance * 0.5f;
                    theChoice = i;
                }
            }
            if (theChoice < theSecond.size()) {
                gPlayerOrder.push_back(theSecond[theChoice]);
                theSecond.erase(theSecond.begin() + theChoice);
            }
            theFromFirst = true;
        }
    }
}
