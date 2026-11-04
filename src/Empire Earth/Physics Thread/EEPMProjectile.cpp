// C:\EEX\Empire Earth\Physics Thread\EEPMProjectile.cpp (file name inferred: no __FILE__; RTTI EEPMProjectile; one unit per
// physics-model class, alphabetical link order). Range 0x15d015..0x15d44f; split out of a single EEPMLand.cpp
// (each unit ends with its own inline-COMDAT cluster).
#include <windows.h>
#include <vector>
#include <math.h>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/U2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFileSpec.h"

#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEMap.h"
#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEPhysics.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EEPhysicsModel.h"

// NEAR: frame/float slot order; the original calls vector<long>::operator[] out of line (0x11e2a5) for the first lookup
// FUNCTION: EXE 0x55d015
// Places the launched projectile on its ballistic arc from the launch point for its flight time so far,
// nose along the arc; a meteor flies straight at its top speed, without gravity
// Fields: start point (+0x10c), yaw/pitch (+0x188/+0x18c), the launcher class's launch speed (else its own) for
// +0x290 ms of flight under the class's gravity (+0x13c); a type 0x325 flies straight at its speed
void EEPMProjectile::Step0(EEComplexUnit* inUnit)
{
    GE3DPoint thePosition = inUnit->mPhysicalModel.GetLocation().GetPosition();
    GE3DPoint theStart = inUnit->mProjectileStart;
    GEVector theDirection(0.0f, 0.0f, 0.0f);
    GEVector theMove(0.0f, 0.0f, 0.0f);
    long theTileX;
    long theTileY;
    float theTime = inUnit->mFlightTime * 0.001f;
    float theGravity = inUnit->mUnitClass->mAcceleration;
    float thePitch = inUnit->mOrientation.mPitch;
    float theYaw = inUnit->mOrientation.mYaw;
    float theCos = UMCos(thePitch);
    float theSin = UMCos(1.5707964f - thePitch);
    inUnit->mInitialContactLocation = thePosition;
    float theSpeed;
    if (inUnit->mUnitClass->mUnitType == kUnitTypeMeteor) {
        theSpeed = inUnit->GetMaxVelocity();
        theGravity = 0.0f;
    } else {
        theSpeed = inUnit->mPlayer->mUnitClassUpgrades[inUnit->mLauncherClass->mUnitType]->mLaunchSpeed;
        if (!(theSpeed > 0.001f))
            theSpeed = inUnit->mPlayer->mUnitClassUpgrades[inUnit->mUnitClass->mUnitType]->mLaunchSpeed;
    }
    float theHorizontal = theSpeed * theCos;
    theDirection.mY = theHorizontal * theTime;
    float theVertical = theSpeed * theSin;
    theDirection.mZ = theVertical * theTime - (theTime * theTime) * theGravity * 0.5f;
    GETransformation theTransformation(false);
    theTransformation.SetOrientationYPR(theYaw, 0.0f, 0.0f);
    theTransformation.SetPosition(0.0f, 0.0f, 0.0f);
    theTransformation.SetElement(3, 0, 0.0f);
    theTransformation.SetElement(3, 1, 0.0f);
    theTransformation.SetElement(3, 2, 0.0f);
    theTransformation.SetElement(3, 3, 1.0f);
    theTransformation.Apply(theDirection, theMove);
    theStart += theMove;
    theTransformation.SetPosition(theStart.GetX(), theStart.GetY(), theStart.GetZ());
    theDirection.mY = theHorizontal;
    theDirection.mZ = theVertical - theTime * theGravity;
    theDirection.Normalize();
    theTransformation.SetOrientationYPR(theYaw, UMArcCos(-theDirection.mZ) - 1.5707964f, 0.0f);
    EEUpdateUnitTile(&theTransformation, inUnit, &theTileX, &theTileY);
}

// NEAR: frame layout (the original keeps the step in the dead inUnit slot; 0x94 vs our 0xa0)
// FUNCTION: EXE 0x55d259
// Moves the projectile one tick along its heading and pulls its nose down by its class's gravity; only
// while it is at or below height 0 and above the ground point
// Class gravity +0x13c; the height-0 test is the original's (bytes 0x55d2e4: `fcomp 0.0`, `ja` returns)
void EEPMProjectile::Step2(EEComplexUnit* inUnit)
{
    GETransformation theTransformation(inUnit->mPhysicalModel.GetLocation());
    GE3DPoint thePosition = theTransformation.GetPosition();
    GE3DPoint theGround;
    GEVector theForward(0.0f, 1.0f, 0.0f);
    GEVector theDirection;
    GEVector theMove;
    long theTileX;
    long theTileY;
    float theGravity = inUnit->mUnitClass->mAcceleration;
    float theStep = gPhysics->mTickSeconds;
    float theSpeed = inUnit->mVelocity;
    if (thePosition.GetZ() > 0.0f)
        return;
    ::gWorld.mMap->GetGroundPoint(thePosition, theGround);
    if (thePosition.GetZ() <= theGround.GetZ())
        return;
    theTransformation.Apply(theForward, theDirection);
    float thePitch = UMArcCos(-theDirection.GetZ()) - 1.5707964f;
    float theYaw = UMArcTan2(-theDirection.GetX(), theDirection.GetY());
    theForward = GEVector(0.0f, theSpeed / UMCos(thePitch) * 0.1f, 0.0f);
    theTransformation.Apply(theForward, theDirection);
    theMove = GEVector(theDirection.GetX() * theStep, theDirection.GetY() * theStep, theDirection.GetZ() * theStep);
    thePosition += theMove;
    theTransformation.SetPosition(thePosition);
    theDirection.mZ = theDirection.GetZ() - theStep * theGravity * 0.2f;
    theDirection.Normalize();
    float theDelta = UMArcCos(-theDirection.GetZ()) - 1.5707964f - thePitch;
    theTransformation.SetOrientationYPR(theYaw, thePitch + theDelta, 0.0f);
    EEUpdateUnitTile(&theTransformation, inUnit, &theTileX, &theTileY);
}

