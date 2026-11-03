// C:\EEX\Empire Earth\Units\EEUCSpaceBuilding.cpp (file name inferred: no __FILE__; the class name). The space
// building's unit class: 0x219fe3..0x21a219 (after EEUCSeaAnimal.cpp; EEUCSpaceship.cpp follows). Unit flags: the
// exe's module set. Its ctor is inline. Virtual74/78/88 are also EEUCNavalBuilding's (ICF: these copies are kept);
// VirtualA0/E0 are EEUCNavalBuilding.cpp's (ICF).
#include <windows.h>
#include <stdio.h>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GETerrainMesh.h"

#include "ee/EEPhysics.h"
#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEMap.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPhysicsModel.h"
#include "ee/EEUCBuilding.h"
#include "ee/EEUCSpaceBuilding.h"

// FUNCTION: EXE 0x619fe3
// Puts a space building's location at the class's cruise altitude
void EEUCSpaceBuilding::GroundLocation(GE3DPoint& ioLocation)
{
    ioLocation.SetZ(this->mCruiseAltitude);
}

// FUNCTION: EXE 0x619ff3
// Returns the tile under a location along the camera line, at the cruise altitude
void EEUCSpaceBuilding::LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY)
{
    GE3DPoint thePoint;
    GE3DLine theLine(::gWorld.mLine);
    theLine.SetStart(inLocation);
    float theT;
    float theZ = this->mCruiseAltitude;
    theLine.SolveForZ(theZ, theT, thePoint);
    outX = UMFloor(thePoint.GetX());
    outY = UMFloor(thePoint.GetY());
}

// FUNCTION: EXE 0x61a0a0
// Creates a space building at a placement, at the cruise altitude
EEUnit* EEUCSpaceBuilding::CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation,
                                        EEUnit* inCreator)
{
    GETransformation theTransformation(inTransformation);
    theTransformation.SetX(theTransformation.GetX());
    theTransformation.SetZ(this->mCruiseAltitude);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// FUNCTION: EXE 0x61a0dd
// Creates a space building on a tile, at the cruise altitude
EEUnit* EEUCSpaceBuilding::CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation;
    theTransformation.SetX((float)inX);
    theTransformation.SetY((float)inY);
    theTransformation.SetZ(this->mCruiseAltitude);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// FUNCTION: EXE 0x61a11d
// Handles a new action as a building does
void EEUCSpaceBuilding::ActionChanged(EEComplexUnit* inUnit)
{
    EEUCBuilding::ActionChanged(inUnit);
}

// FUNCTION: EXE 0x61a129
// Sets up a new space building as a building, already built
void EEUCSpaceBuilding::UnitCreated(EEComplexUnit* inUnit)
{
    EEUCBuilding::UnitCreated(inUnit);
    inUnit->SetBuilt(1, 0, 1);
}

// FUNCTION: EXE 0x61a144
// Returns whether the space building may stop: already sinking, a slow request, or at its next waypoint's tile
bool EEUCSpaceBuilding::HasArrived(EEComplexUnit* inUnit, float inValue)
{
    GE3DPoint theLocation;
    bool theResult = false;
    if (inUnit->mCollisionState == 8 || inValue <= 0.05f)
        theResult = true;
    else {
        inUnit->GetNextWaypointPoint(theLocation);
        if (inUnit->GetTileX() == UMFloor(theLocation.GetX()) && inUnit->GetTileY() == UMFloor(theLocation.GetY()))
            theResult = true;
    }
    return theResult;
}

// FUNCTION: EXE 0x61a1cb
// Steps the space building's physics and sound, regenerates it, then updates it as a building
void EEUCSpaceBuilding::Update(EEComplexUnit* inUnit)
{
    if (this->mPhysicsModel != NULL && inUnit->mTileX != -10000)
        this->mPhysicsModel->Step(inUnit);
    this->RefreshSound(inUnit);
    if (inUnit->mStats != NULL && inUnit->mStats->mRegeneration > 0)
        this->Regenerate(inUnit);
    EEUCBuilding::Update(inUnit);
}
