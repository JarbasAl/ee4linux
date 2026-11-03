// C:\EEX\Empire Earth\Units\EEUCNaval.cpp (file name inferred: no __FILE__; the class name). The warships' unit class:
// 0x217c6d..0x218158 (after EEUCMissile.cpp; EEUCNavalBuilding.cpp follows), slots +0x0c, +0x74, +0x78, +0x88, +0xa0
// and +0xb8 of ee/EEUCNaval.h plus the helper UpdateResourceUnit. The ctor is inline (EEUnitClass.cpp's factory). Unit flags:
// the exe's module set.
// Virtual0C 0x217c6d forwards (inUnit, ioPath, inArg) to the cdecl 0x23df77 (declared here).
#include <windows.h>
#include <stdio.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GETerrainMesh.h"
#include "lle/GEBoundingBox.h"
#include "lle/GEModel.h"
#include "lle/SESoundManager.h"

#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPhysics.h"
#include "ee/EEPhysicsModel.h"
#include "ee/EEUnitAction.h"
#include "ee/EEUnitGoal.h"
#include "ee/EEMedia.h"
#include "ee/EESoundObject.h"
#include "ee/EEGameSound.h"
#include "ee/EEEffectManager.h"
#include "ee/EEUCNaval.h"

// 0x23df77 (cdecl, void; in EEFOFindNewTargetNearOldTarget.cpp's range): the naval path smoothing the naval classes'
// Virtual0C forward to; no header declares it yet (provisional name)
void EEAdjustNavalPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inArg);

// FUNCTION: EXE 0x617c6d
// Smooths a ship's path with the naval path smoothing (0x23df77)
void EEUCNaval::AdjustPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inSmooth)
{
    EEAdjustNavalPath(inUnit, ioPath, inSmooth);
}

// FUNCTION: EXE 0x617c84
// Starts a ship sinking: the class's death effect and sound, then the sinking state; other actions go to the base
void EEUCNaval::ActionChanged(EEComplexUnit* inUnit)
{
    if ((inUnit->mCommand ? inUnit->mCommand->mType : 0) != kActionDying) {
        this->HandleActionChanged(inUnit);
        return;
    }
    if (this->mDeathEffect != NULL)
        gEffectManager->CreateUnitEffect(inUnit, this->mDeathEffect, NULL, 0, 0, 0);
    EESoundObject* theSound = inUnit->mStats->mDeathSound;
    if (theSound != NULL) {
        float theVolume;
        float thePan;
        float thePitch;
        this->GetSoundParameters(inUnit, theSound, &theVolume, &thePan, &thePitch);
        gSoundManager->Play(theSound->mSampleID, theVolume, thePan, thePitch, theSound->mLooping, NULL);
    }
    inUnit->SetCurrentPhysicsAction(2);
    inUnit->mAngularVelocity = GEOrientation(0.0f, 0.0f, 0.0f);
    inUnit->SetVelocity(0.0f);
    inUnit->SetAnimation(4, -1, 0);
    inUnit->DropPath();
}

// FUNCTION: EXE 0x617d6a
// Sets up a new ship at sea level (z 0), with the class's resource type and amount
void EEUCNaval::UnitCreated(EEComplexUnit* inUnit)
{
    GETransformation theTransformation(inUnit->mPhysicalModel.GetLocation());
    EEUnitClass::UnitCreated(inUnit);
    theTransformation.SetX(theTransformation.GetX());
    theTransformation.SetZ(0.0f);
    inUnit->SetTransformation(theTransformation, true);
    inUnit->SetResourceType(this->mResourceType);
    if (inUnit->mResourceType != kResourceNone)
        inUnit->SetResourceAmount(this->mResourceAmount);
}

// FUNCTION: EXE 0x617dcf
// Returns whether the ship may stop: already sinking, a slow request, or at the path's end tile with its speed reached
bool EEUCNaval::HasArrived(EEComplexUnit* inUnit, float inValue)
{
    GE3DPoint thePosition;
    bool theResult = false;
    if (inUnit->mCollisionState == 8 || inValue <= 0.05f)
        theResult = true;
    else {
        unsigned long theCount = inUnit->mWaypoints.size();
        inUnit->GetNextWaypointPoint(thePosition);
        thePosition.SetZ(0.0f);
        if (inUnit->GetTileX() == UMFloor(thePosition.GetX()) && inUnit->GetTileY() == UMFloor(thePosition.GetY())
            && (theCount > 1 || inUnit->mCollisionRadius >= 2.0f
                || (inUnit->GetQueueGoals(0).size() > 1 && inUnit->mGoalState != NULL && inUnit->mGoalState->mKind == kGoalLocation))) {
            if (inUnit->mCollisionRadius < inValue)
                theResult = false;
            else
                theResult = true;
        }
    }
    return theResult;
}

// FUNCTION: EXE 0x617eb2
// Answers dying done: true once the ship's transformed bounding box lies wholly below the water line
long EEUCNaval::EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent)
{
    GE3DPoint thePosition;
    GEBoundingBox theBox;
    GEBoundingBox theTransformedBox;
    EEComplexUnit* theUnit = inUnit;
    const GEPhysicalModel& thePhysicalModel = theUnit->mPhysicalModel;
    GEModel* theModel = thePhysicalModel.GetModel();
    long theResult = 0;
    if (inEvent != kTriggerDyingAnimationFinished)
        theResult = EEUnitClass::EvaluateCondition(theUnit, inEvent);
    else {
        theUnit->GetPosition(thePosition);
        GETransformation theTransformation(false);
        thePhysicalModel.GetLocation().Apply(theModel->GetInternalTransform(thePhysicalModel.GetFrame()),
                                             theTransformation);
        float theScale = thePhysicalModel.GetScale();
        theTransformation.Scale(theScale, theScale, theScale);
        theTransformation.SetX(thePosition.GetX());
        theTransformation.SetY(thePosition.GetY());
        theTransformation.SetZ(thePosition.GetZ());
        theBox = theModel->GetBoundingBox(theUnit->mPhysicalModel.GetFrame());
        theBox.Transform(theTransformation, theTransformedBox);
        if (theTransformedBox.GetTopRightBack().GetZ() < 0.0f)
            theResult = 1;
    }
    return theResult;
}

// FUNCTION: EXE 0x617fb5
// Updates a fish resource: while its resources rot it loses one every 4 s; a rotting unit counts its rotting time down
void EEUCNaval::UpdateResourceUnit(EEComplexUnit* inUnit)
{
    inUnit->UpdateOrbitals();
    switch (inUnit->mCommand ? inUnit->mCommand->mType : 0) {
    case kActionResourcesRotting: {
        long theTime = gPhysics->mTickTimeMs + inUnit->mRottingAwayTime;
        if (theTime > 4000) {
            long theValue = inUnit->mResourceAmount - theTime / 4000;
            if (theValue < 0)
                theValue = 0;
            inUnit->SetResourceAmount(theValue);
            theTime %= 4000;
        }
        inUnit->SetRottingAwayTime(theTime);
        break;
    }
    case kActionUnitRotting: {
        long theTime = inUnit->mRottingAwayTime - gPhysics->mTickTimeMs;
        if (theTime < 0)
            theTime = 0;
        inUnit->SetRottingAwayTime(theTime);
        break;
    }
    }
}

// FUNCTION: EXE 0x618055
// Updates a ship each tick (fish as a resource): physics, turrets, effects, timers, animation, firing on attack frames
void EEUCNaval::Update(EEComplexUnit* inUnit)
{
    long theFrame = inUnit->mPhysicalModel.GetFrame();
    if (this->mUnitType == kUnitTypeResFish) {
        this->UpdateResourceUnit(inUnit);
        return;
    }
    inUnit->SetLastHitPoints(inUnit->mHitPoints);
    if (this->mPhysicsModel != NULL && inUnit->GetTileX() != -10000) {
        this->mPhysicsModel->Step(inUnit);
        inUnit->UpdateTurrets();
    }
    inUnit->UpdateOrbitals();
    this->UpdateEffects(inUnit);
    inUnit->UpdateReloadTimer();
    inUnit->UpdateTargetSearchTimer();
    this->UpdateCamouflageTimer(inUnit);
    long theAttack = this->Animate(inUnit);
    this->RefreshSound(inUnit);
    switch (inUnit->mCommand ? inUnit->mCommand->mType : 0) {
    case kActionAttackEnemyUnit:
    case kActionAttackLocation:
    case kActionWaitForReload:
        this->FireOnAttackFrame(inUnit, theAttack, theFrame);
        break;
    case kActionUnitRotting: {
        long theTime = inUnit->mRottingAwayTime - gPhysics->mTickTimeMs;
        if (theTime < 0)
            theTime = 0;
        inUnit->SetRottingAwayTime(theTime);
        break;
    }
    }
    this->Fn157269(inUnit);
}
