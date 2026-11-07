// C:\EEX\Empire Earth\Units\EEUCNavalTransport.cpp (file name inferred: no __FILE__; the class name). The naval
// transport's unit class: 0x218a45..0x218e34 (after EEUCNavalPriest.cpp; EEUCPlanetSpaceship.cpp's +0x74 0x218e34
// follows). Unit flags: the exe's module set. Its ctor is inline (EEUnitClass.cpp's factory; vtable 0x448718); its
// slots +0x04, +0xe8, +0xf8, +0xfc, +0x88, +0x104, +0xb8 in source order (Virtual08/0C/30/34/A0/E4/F4 are other
// units' copies by ICF).
#include <windows.h>
#include <stdio.h>
#include <algorithm>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GETerrainMesh.h"
#include "lle/U2DPoint.h"
#include "lle/U2DRectangle.h"
#include "lle/U2DPolygon.h"
#include "lle/U2DSparseArray.h"

#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEMap.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEUnitGoal.h"
#include "ee/EEAIContinent.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEUCTransport.h"
#include "ee/EEUCNavalTransport.h"
#include "ee/EEAreaUtilities.h"
#include "ee/EEUnitUtilities.h"

// FUNCTION: EXE 0x618a45
// Raises a location below the water plane to z 0
void EEUCNavalTransport::GroundLocation(GE3DPoint& ioLocation)
{
    if (ioLocation.GetZ() < 0.0f)
        ioLocation.SetZ(0.0f);
}

// FUNCTION: EXE 0x618a5f
// Finds where the transport unloads for a destination: shore and landing tiles, then the destination when in range
// and reachable, else the last good tile on the line from the landing tile (the first one if none is good)
void EEUCNavalTransport::FindUnloadTiles(EEComplexUnit* inTransport, U2DPoint<long>* inDestination,
                                   U2DPoint<long>* outUnloadTile, U2DPoint<long>* outFirstCargoTile,
                                   U2DPoint<long>* outTile)
{
    std::vector<EEUnit*> theUnits;
    std::vector<U2DPoint<long> > theTiles;
    float theRadius = this->mUnloadRange;
    float theRange = theRadius * theRadius;
    EETileCoordinate theFrom = inTransport->GetTile();
    this->FindShoreTile(inTransport, theFrom, inDestination, inDestination, true, outUnloadTile);
    this->FindLandingTile(inTransport, outUnloadTile, inDestination, outFirstCargoTile);
    if (outUnloadTile->DistanceToSquaredF(*inDestination) < theRange
        && this->CanUnloadNear(NULL, inTransport->GetCargo(0), outUnloadTile, outFirstCargoTile, inDestination)) {
        if (this->CanUnloadNear(NULL, *(inTransport->mCargo.begin() + 0), outUnloadTile, outFirstCargoTile, inDestination))
            *outTile = *inDestination;
        else
            *outTile = *outFirstCargoTile;
    } else {
        EEGetTileLine(outFirstCargoTile->GetX(), outFirstCargoTile->GetY(), inDestination->GetX(),
                   inDestination->GetY(), &theTiles, 1, 0);
        std::vector<U2DPoint<long> >::iterator theTile = theTiles.begin();
        while (true) {
            if (theTile == theTiles.end())
                break;
            U2DPoint<long>* thePoint = &(*theTile);
            float theDistance = outUnloadTile->DistanceToSquaredF(*thePoint);
            EEMapTile* theMapTile = ::gWorld.mMap->GetTile(thePoint->GetX(), thePoint->GetY());
            if (thePoint == NULL || theDistance > theRange
                || EEGetBlockingUnits(inTransport->GetPlayer(), this->GetUnitType(), theMapTile, 0, theUnits)
                || !this->CanUnloadNear(NULL, inTransport->mCargo[0], outUnloadTile, outFirstCargoTile, thePoint))
                break;
            theTile++;
        }
        if (theTile != theTiles.begin() && theTiles.size() > 0)
            theTile--;
        if (theTile != theTiles.end())
            *outTile = *theTile;
        else
            *outTile = *outFirstCargoTile;
    }
}

// FUNCTION: EXE 0x618c48
// Returns whether the transport can unload at a tile: not deep water, cliff or space, both continents known, the base
// class agrees, and its land continent connects to the tile's (or its water body is flagged for it)
bool EEUCNavalTransport::CanUnloadAt(EEComplexUnit* inTransport, long inX, long inY)
{
    EEMapTile* theTile = ::gWorld.mMap->GetTile(inX, inY);
    bool theResult = false;
    if (theTile != NULL) {
        long theType = theTile->mTerrainType;
        if (theType == kTerrainDeepWater || theType == kTerrainCliff || theType == kTerrainSpace) {
            theResult = false;
        } else {
            EEAIContinent* theFrom = gContinentManager.GetContinentAt(inTransport->GetTileX(), inTransport->GetTileY());
            EEAIContinent* theTo = gContinentManager.GetContinentAt(inX, inY);
            if (theFrom == NULL || theTo == NULL) {
                theResult = false;
            } else {
                // the base class decides; a continent of another kind keeps its answer
                theResult = EEUCTransport::CanUnloadAt(inTransport, inX, inY);
                if (theResult) {
                    if (theFrom->mCategory == kContinentCategoryLand) {
                        unsigned short theToID = theTo->mID;
                        unsigned short theFromID = theFrom->mID;
                        theResult = this->ContinentsConnect(theFromID, theToID);
                    } else if (theFrom->mCategory == kContinentCategoryWater)
                        theResult = gContinentManager.mTileFlags[theFrom->mID * gContinentManager.mFlagsWidth + theTo->mID];
                }
            }
        }
    }
    return theResult;
}

// FUNCTION: EXE 0x618d33
// Finds where a unit boards the transport (the base transport's rule)
bool EEUCNavalTransport::FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation,
                                   GE3DPoint& outLocation)
{
    return EEUCTransport::FindBoardingLocation(inUnit, inTransport, inLocation, outLocation);
}

// FUNCTION: EXE 0x618d4b
// Returns whether the transport may stop: already sinking, at the path's end tile with its speed reached, or slow
bool EEUCNavalTransport::HasArrived(EEComplexUnit* inUnit, float inValue)
{
    GE3DPoint thePosition(0.0f, 0.0f, 0.0f);
    bool theResult = false;
    if (inUnit->mCollisionState != 8) {
        unsigned long theCount = inUnit->mWaypoints.size();
        inUnit->GetNextWaypointPoint(thePosition);
        if (inUnit->GetTileX() == UMFloor(thePosition.GetX()) && inUnit->GetTileY() == UMFloor(thePosition.GetY())
            && (theCount > 1
                || (inUnit->GetQueueGoals(0).size() > 1 && inUnit->mGoalState != NULL && inUnit->mGoalState->mKind == kGoalLocation))) {
            if (inUnit->mCollisionRadius < inValue)
                theResult = false;
            else
                theResult = true;
        } else if (inValue <= 0.05f) {
            theResult = true;
        }
    } else {
        theResult = true;
    }
    return theResult;
}

// FUNCTION: EXE 0x618e14
// Unloads the transport at a tile (the base transport's rule)
void EEUCNavalTransport::UnloadAt(EEComplexUnit* inTransport, long inX, long inY)
{
    EEUCTransport::UnloadAt(inTransport, inX, inY);
}

// FUNCTION: EXE 0x618e28
// Updates the transport as a base transport
void EEUCNavalTransport::Update(EEComplexUnit* inUnit)
{
    EEUCTransport::Update(inUnit);
}
