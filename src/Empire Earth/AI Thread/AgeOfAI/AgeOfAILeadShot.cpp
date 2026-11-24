// Aiming ahead of a moving target, in the game (AgeOfAILead.h): the flight time comes from the game's own arc solver.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/aim.c
// (hook_aim, flight_time) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include <windows.h>
#include <unistd.h>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/FSPath.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/SESoundManager.h"
#include "lle/Input System.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/NEMessage.h"
#include "lle/NESynchronousCommunications.h"
#include "lle/GELine.h"
#include "lle/UITextLabel.h"
#include "lle/UIListBox.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UILBString.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"
#include "lle/UIEditBox.h"
#include "lle/UICheckBox.h"
#include "lle/UIRadioButton.h"
#include "lle/UIScrollBar.h"
#include "lle/UIFileListBox.h"
#include "lle/UIComboBox.h"
#include "lle/UIBlinkingButton.h"
#include "lle/UIBitmap.h"
#include "lle/NESynchronousServer.h"
#include "lle/FSRestorable.h"
#include "ee/EmpireEarth.h"
#include "AgeOfAI/AgeOfAI.h"
#include "ee/EEAction.h"
#include "ee/EEARemovePlayer.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIManager.h"
#include "ee/EEServer.h"
#include "ee/EEWonNet.h"
#include "ee/EEGameOptions.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEGamePreferences.h"
#include "ee/EESMMediaStrings.h"
#include "ee/EEPSaveGameHeader.h"
#include "ee/EESaveGameFiles.h"
#include "ee/EEGameVariant.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EERandomMapGenerators.h"
#include "ee/EEUICPing.h"
#include "ee/EEUIELobbyEvents.h"
#include "ee/EEUIEPostGameSummaryEvents.h"
#include "ee/EEStartingResources.h"
#include "ee/EEStringIDs.h"
#include "ee/EETechTreeTechID.h"
#include "ee/EEPlayer.h"
#include "ee/EEPhysics.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPlane.h"
#include "lle/GE3DLine.h"
#include "lle/GETransformation.h"
#include "lle/GEPhysicalModel.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFile.h"
#include "lle/U2DSparseArray.h"
#include "lle/U2DPolygon.h"
#include "lle/USingleton.h"
#include "ee/EEComputerPlayer.h"
#include "ee/EEUnit.h"
#include "platform/PlatSim.h"


#include "ee/EEMap.h"
#include "ee/EEUnit.h"
#include "ee/EEUnitClassIDs.h"
#include "ee/EEUnitUtilities.h"
#include "ee/EEAreaUtilities.h"
#include "AgeOfAI.h"
#include "AgeOfAILead.h"
#include "AgeOfAILob.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

ProjectileCounts sCounts;
std::map<unsigned long, Lob::State> sLob;   // by unit id; not saved: a loaded game's unit looks again

// A missile's aim, tracked by its unit id (stage.c aim.c g_msl). The target's position between the missile's steps gives
// its velocity; a target change or a gap (the missile's climb) starts the measurement over. Not saved: a loaded missile
// measures again.
struct MissileTrack {
    EEUnit* mTarget = NULL;
    unsigned long mFirst = 0, mLast = 0;
    float mX = 0.0f, mY = 0.0f, mZ = 0.0f;   // the target's position at the last step
    float mVX = 0.0f, mVY = 0.0f, mVZ = 0.0f;
    int mNV = 0;                              // velocity samples; aiming waits for the first
};
std::map<unsigned long, MissileTrack> sMissile;

float TileHeight(float inX, float inY)
{
    EEMapTile* theTile = gWorld.mMap->GetTile((long)floorf(inX), (long)floorf(inY));
    return theTile != NULL ? theTile->GetHeight() : 0.0f;
}

// The seconds a projectile of the shooter flies to inTo, by the arc the launch will take (the game's own solver); < 0
// when unknown. Out of the arc's reach the game launches at 45 degrees and the shot comes down short, after its longest
// range, v^2 / g.
float FlightTime(EEComplexUnit* inShooter, const GE3DPoint& inFrom, const GE3DPoint& inTo)
{
    EEUnitClassUpgrade* theStats = inShooter->mStats;
    EEUnitClass* theProjectile = inShooter->mUnitClass->mProjectileClass;
    float theSpeed = theStats->mLaunchSpeed, theGravity = theProjectile->GetAcceleration();
    if (theSpeed <= 0.001f || theGravity <= 0.001f)
        return -1.0f;
    float thePitch = 0.0f;
    float theDX = inTo.GetX() - inFrom.GetX(), theDY = inTo.GetY() - inFrom.GetY();
    float theDistance = sqrtf(theDX * theDX + theDY * theDY);
    float theCos;
    if (EEGetBallisticPitch(inShooter, theStats, &inFrom, &inTo, &thePitch)) {
        theCos = UMCos(thePitch);
    } else {
        theCos = 0.70710678f;
        if (theDistance > theSpeed * theSpeed / theGravity)
            theDistance = theSpeed * theSpeed / theGravity;
    }
    if (theCos < 0.05f)
        return -1.0f;
    return theDistance / (theSpeed * theCos);
}

}

ProjectileCounts& Projectiles()
{
    return sCounts;
}

bool LeadTargetsOn(unsigned long inPlayerID)
{
    return GeneralOf(inPlayerID).lead_targets != 0;
}

// A shot of a plain projectile (an arrow, a stone, a cannon ball; not a missile, which steers, nor a bomb) at a unit
// that moves: the aim moves to where the target will be when the projectile comes down.
void LeadShot(EEComplexUnit* inShooter, EEUnit* inTarget, GE3DPoint& ioAim)
{
    EEUnitClass* theProjectile = inShooter->mUnitClass->mProjectileClass;
    if (theProjectile == NULL || theProjectile->mCategory != kUnitCategoryProjectile)
        return;
    sCounts.mFired++;
    if (!LeadTargetsOn(inShooter->mPlayer->mPlayerID) || inTarget == NULL || inTarget == inShooter || inTarget->mComplexUnit == NULL)
        return;
    EEComplexUnit* theTarget = inTarget->mComplexUnit;
    float theSpeed = theTarget->mVelocity;
    if (!(theSpeed > 0.01f && theSpeed < 50.0f))
        return;
    // along its facing: the local y axis of its orientation
    GEVector theForward(0.0f, 1.0f, 0.0f), theFacing;
    theTarget->mPhysicalModel.GetLocation().Apply(theForward, theFacing);
    float theLength = sqrtf(theFacing.GetX() * theFacing.GetX() + theFacing.GetY() * theFacing.GetY());
    if (!(theLength > 0.01f))
        return;
    float theVX = theSpeed * theFacing.GetX() / theLength, theVY = theSpeed * theFacing.GetY() / theLength;
    const GETransformation& theShooter = inShooter->mPhysicalModel.GetLocation();
    GE3DPoint theFrom(theShooter.GetX(), theShooter.GetY(), theShooter.GetZ() + 0.5f);   // about the launch point
    const GETransformation& theLocation = theTarget->mPhysicalModel.GetLocation();
    float theTileX = (float)inTarget->mTileX + 0.5f, theTileY = (float)inTarget->mTileY + 0.5f;
    bool theCentred = ioAim.GetX() == theTileX && ioAim.GetY() == theTileY;
    float theGroundBefore = TileHeight(ioAim.GetX(), ioAim.GetY());
    GE3DPoint theAim = ioAim;
    float theFirst = FlightTime(inShooter, theFrom, theAim);
    if (theFirst < 0.0f)
        return;
    float theX, theY;
    struct Flight {
        EEComplexUnit* mShooter; const GE3DPoint* mFrom; GE3DPoint mAim; float mGroundBefore;
        float operator()(float inX, float inY)
        {
            mAim.SetX(inX);
            mAim.SetY(inY);
            return FlightTime(mShooter, *mFrom, mAim);
        }
    } theFlight = {inShooter, &theFrom, ioAim, theGroundBefore};
    Lead::LeadAim(theCentred ? theTileX : theLocation.GetX(), theCentred ? theTileY : theLocation.GetY(), ioAim.GetX(), ioAim.GetY(),
                  theCentred, theVX, theVY, theFirst, theFlight, theX, theY);
    if (theX == ioAim.GetX() && theY == ioAim.GetY())
        return;
    ioAim.SetZ(ioAim.GetZ() + TileHeight(theX, theY) - theGroundBefore);   // the ground under the new aim
    ioAim.SetX(theX);
    ioAim.SetY(theY);
    sCounts.mLed++;
}

bool LeadMissilesOn(unsigned long inPlayerID)
{
    return GeneralOf(inPlayerID).lead_missiles != 0;
}

// A homing missile (EEPMMissile::Home) steers to where its moving target will be when it gets there, as upstream's
// hook_missile_aim. The target's velocity comes from its position changes between steps, averaged over the last two; a
// target faster than 20 tiles a second looks like a jump (teleport, new unit) and starts the measurement over. The
// intercept time solves |r + v T| = s T for the missile's top speed s, clamped to 10 s; with no solution the game's own
// pursuit stays. A target that stands still is left alone. is_vanilla is not ported: as with LeadShot the setting reads
// the missile owner's [general] rows, so every player needs the same setting in a multiplayer game.
void LeadMissile(EEComplexUnit* inMissile, EEUnit* inTarget, GE3DPoint& ioAim)
{
    if (!LeadMissilesOn(inMissile->mPlayer->mPlayerID) || inTarget == NULL || inTarget == inMissile)
        return;
    unsigned long theNow = gPhysics->mGameTime;
    float theSpeed = inMissile->mStats->GetMaxVelocity();
    GE3DPoint thePos;
    inTarget->GetPosition(thePos);
    GE3DPoint theMissile;
    inMissile->GetPosition(theMissile);
    float theRX = ioAim.GetX() - theMissile.GetX(), theRY = ioAim.GetY() - theMissile.GetY(), theRZ = ioAim.GetZ() - theMissile.GetZ();
    float theDist2 = theRX * theRX + theRY * theRY + theRZ * theRZ;
    float theDist = sqrtf(theDist2);

    if (sMissile.size() > 8192)
        sMissile.clear();
    MissileTrack& theTrack = sMissile[inMissile->mUnitID];
    if (theTrack.mTarget != inTarget) {
        theTrack.mTarget = inTarget;
        theTrack.mNV = 0;
        theTrack.mFirst = theNow;
    } else if ((long)(theNow - theTrack.mLast) > 200) {
        theTrack.mNV = 0;   // the climb, or any gap, starts over
    } else if (theNow > theTrack.mLast) {
        float theDT = (float)(theNow - theTrack.mLast) * 0.001f;
        float theVX = (thePos.GetX() - theTrack.mX) / theDT;
        float theVY = (thePos.GetY() - theTrack.mY) / theDT;
        float theVZ = (thePos.GetZ() - theTrack.mZ) / theDT;
        if (theVX * theVX + theVY * theVY + theVZ * theVZ < 400.0f) {   // under 20 tiles a second
            if (theTrack.mNV)
                theTrack.mVX = (theTrack.mVX + theVX) * 0.5f, theTrack.mVY = (theTrack.mVY + theVY) * 0.5f,
                theTrack.mVZ = (theTrack.mVZ + theVZ) * 0.5f;
            else
                theTrack.mVX = theVX, theTrack.mVY = theVY, theTrack.mVZ = theVZ;
            theTrack.mNV++;
        } else
            theTrack.mNV = 0;
    }
    theTrack.mTarget = inTarget;
    theTrack.mLast = theNow;
    theTrack.mX = thePos.GetX(), theTrack.mY = thePos.GetY(), theTrack.mZ = thePos.GetZ();
    if (!theTrack.mNV || !(theSpeed > 0.01f))
        return;

    float theVX = theTrack.mVX, theVY = theTrack.mVY, theVZ = theTrack.mVZ;
    float theV2 = theVX * theVX + theVY * theVY + theVZ * theVZ;
    if (theV2 < 0.0025f)   // standing: under 0.05 tiles a second
        return;
    float theA = theV2 - theSpeed * theSpeed;
    float theB = 2.0f * (theRX * theVX + theRY * theVY + theRZ * theVZ);
    float theT = -1.0f;
    if (fabsf(theA) < 1e-4f) {
        if (theB < 0.0f)
            theT = -theDist2 / theB;
    } else {
        float theDisc = theB * theB - 4.0f * theA * theDist2;
        if (theDisc < 0.0f)
            return;
        float theQ = sqrtf(theDisc);
        float theT1 = (-theB - theQ) / (2.0f * theA), theT2 = (-theB + theQ) / (2.0f * theA);
        theT = theT1 > 0.0f && (theT1 < theT2 || theT2 <= 0.0f) ? theT1 : theT2;
    }
    if (!(theT > 0.0f))
        return;
    if (theT > 10.0f)
        theT = 10.0f;
    ioAim.SetX(ioAim.GetX() + theVX * theT);
    ioAim.SetY(ioAim.GetY() + theVY * theT);
    ioAim.SetZ(ioAim.GetZ() + theVZ * theT);
    sCounts.mMissilesLed++;
}

bool LobEachShotOn(unsigned long inPlayerID)
{
    return GeneralOf(inPlayerID).lob_each_shot != 0;
}

// Before a shot of a unit that may lob over walls: it looks along the line to its target and fires high only when a wall
// stands in it (AgeOfAILob.h).
void LobBeforeShot(EEComplexUnit* inShooter, EEUnit* inTarget)
{
    EEUnit* theBlocker = NULL;
    long theX = 0, theY = 0;
    bool theBlocked = EEIsLineToUnitBlocked(inShooter, inTarget, &theBlocker, &theX, &theY) && theBlocker != NULL && theBlocker != inTarget;
    if (sLob.size() > 8192)
        sLob.clear();
    bool theHigh = Lob::Decide(sLob[inShooter->mUnitID], inShooter->mIsFiringHigh, theBlocked);
    inShooter->SetIsFiringHigh(theHigh);
}

}
