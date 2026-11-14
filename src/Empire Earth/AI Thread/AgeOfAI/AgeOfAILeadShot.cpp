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
#include "AgeOfAI.h"
#include "AgeOfAILead.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

ProjectileCounts sCounts;

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

bool LeadTargetsOn()
{
    EngineOf(1);                     // reads ageofai.ini once
    return gSettings.lead_targets != 0;
}

// A shot of a plain projectile (an arrow, a stone, a cannon ball; not a missile, which steers, nor a bomb) at a unit
// that moves: the aim moves to where the target will be when the projectile comes down.
void LeadShot(EEComplexUnit* inShooter, EEUnit* inTarget, GE3DPoint& ioAim)
{
    EEUnitClass* theProjectile = inShooter->mUnitClass->mProjectileClass;
    if (theProjectile == NULL || theProjectile->mCategory != kUnitCategoryProjectile)
        return;
    sCounts.mFired++;
    if (!LeadTargetsOn() || inTarget == NULL || inTarget == inShooter || inTarget->mComplexUnit == NULL)
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

}
