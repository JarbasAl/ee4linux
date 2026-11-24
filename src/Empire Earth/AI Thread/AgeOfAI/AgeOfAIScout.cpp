// SCOUTING UNSEEN LAND ([ai_base] towns=scout, off in the shipped file). Grille's towns.c sends its fastest idle
// soldier to the unseen cell of the home continent nearest a town, behind the front its known enemy buildings hold,
// and turns it back when an enemy soldier comes in sight. The port has no town planner, so the near tests use the
// base and the standing towns instead of the planned ones; the look is the intel memory's six-tile cell. The
// arithmetic is in AgeOfAIScoutRules.h. Upstream's scout watch runs every tick; this port checks every two seconds.
#include <windows.h>
#include <unistd.h>
#include <algorithm>
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
#include "ee/EECPBuildManager.h"
#include "ee/EECPUnitManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGLoadGarrison.h"
#include "ee/EEResourceType.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEUnitBehavior.h"
#include "ee/EECPCombatManager.h"
#include "ee/EECPAttackCommander.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackSubGroup.h"
#include "AgeOfAI/AgeOfAIStats.h"
#include "AgeOfAI.h"
#include "AgeOfAIDefence.h"
#include "AgeOfAIHome.h"
#include "AgeOfAITowers.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"
#include "AgeOfAIHospitalRules.h"
#include "AgeOfAI/AgeOfAIRoles.h"
#include "AgeOfAI/AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAIIntel.h"
#include "AgeOfAI/AgeOfAIScoutRules.h"

namespace AgeOfAI {

namespace {

const unsigned long kScoutEvery = 2000;

struct ScoutState {
    unsigned long mLast;
    unsigned long mSentAt;
    EEUnit* mScout;
    unsigned long mScoutID;
};

ScoutState sScout[16];
ScoutCounts sScoutCounts[16];

void ScoutMoveTo(EEComplexUnit* inUnit, float inX, float inY)
{
    EEUGLocation* theGoal = new EEUGLocation(GE2DPoint(inX, inY), true, true, false, 0.0f, false, 0);
    inUnit->SetGoal(theGoal, false, 0);
    inUnit->StartAction(kActionPrepareToMove);
    inUnit->mUnitClass->ActionChanged(inUnit);
}

bool StandingTown(EEUnit* inUnit)
{
    if (inUnit == NULL || inUnit->mHitPoints <= 0 || inUnit->mUnitClass == NULL || inUnit->mComplexUnit == NULL)
        return false;
    long theType = (long)inUnit->mUnitClass->mUnitType;
    return (theType == kUnitTypeTownCenter || theType == kUnitTypeCapitol || theType == kUnitTypeSettlement)
           && !inUnit->mComplexUnit->IsFoundation() && inUnit->mComplexUnit->mIsBuilt;
}

float SpeedOf(EEUnit* inUnit)
{
    return inUnit != NULL && inUnit->mStats != NULL ? inUnit->mStats->mMaxVelocity : 0.0f;
}

unsigned short LandAt(long inX, long inY)
{
    return inX < 0 || inY < 0 ? 0 : gContinentManager.GetContinent(EETileCoordinate(inX, inY), 2);
}

}  // namespace

ScoutCounts ScoutCountsOf(unsigned long inPlayerID)
{
    return sScoutCounts[inPlayerID & 15];
}

void ScoutTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium);
    if (!theSettings.towns.scout)
        return;
    unsigned long theID = inPlayer->mPlayerID;
    unsigned thePI = theID & 15;
    ScoutState& theState = sScout[thePI];
    ScoutCounts& theCounts = sScoutCounts[thePI];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
        std::memset(&theCounts, 0, sizeof(theCounts));
    }
    if (theState.mLast != 0 && inNow >= theState.mLast && inNow - theState.mLast < kScoutEvery)
        return;
    theState.mLast = inNow != 0 ? inNow : 1;
    theCounts.mChecks++;

    EECPCivilizationManager* theCiv = inPlayer->mCivilizationManager;
    if (theCiv == NULL || theCiv->mBuildManager == NULL)
        return;
    long theBaseX = theCiv->mBuildManager->mTownCenterTile.mX;
    long theBaseY = theCiv->mBuildManager->mTownCenterTile.mY;
    if (theBaseX < 0 || theBaseY < 0 || theBaseX == -10000 || theBaseY == -10000)
        return;

    if (theState.mScout != NULL) {
        bool theGo = theState.mScout->mHitPoints <= 0 || theState.mScout->mUnitID != theState.mScoutID
                     || theState.mScout->mPlayer != inPlayer || theState.mScout->mComplexUnit == NULL;
        if (!theGo) {
            long theX = theState.mScout->mTileX, theY = theState.mScout->mTileY;
            if (RaidersKnown(inPlayer, theX, theY, (int)ScoutRules::kSight) || ThreatAt(theID, theX, theY) > 0.0f) {
                ScoutMoveTo(theState.mScout->mComplexUnit, (float)theBaseX + 0.5f, (float)theBaseY + 0.5f);
                theCounts.mBack++;
                theGo = true;
            }
        }
        if (theGo)
            theState.mScout = NULL;
    }
    if (theState.mScout != NULL || !ScoutRules::PauseOver(inNow, theState.mSentAt))
        return;

    std::vector<ScoutRules::Point> theOwn, theFoe;
    ScoutRules::Point theBase = { theBaseX, theBaseY };
    theOwn.push_back(theBase);
    unsigned short theLand = LandAt(theBaseX, theBaseY);
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL && theOwn.size() < 64; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (!StandingTown(theUnit) || theUnit->mTileX < 0)
            continue;
        ScoutRules::Point theTown = { theUnit->mTileX, theUnit->mTileY };
        theOwn.push_back(theTown);
    }
    const Intel::Memory& theMemory = MemoryOf(theID);
    const std::vector<Intel::Record>& theRecords = theMemory.Records();
    for (size_t k = 0; k < theRecords.size(); k++) {
        const Intel::Record& theRecord = theRecords[k];
        if (theRecord.mKind != Intel::kBuilding || theRecord.mX < 0 || theRecord.mOwner < 0)
            continue;
        EEPlayer* theOwner = gWorld.GetPlayer(theRecord.mOwner);
        if (theOwner == NULL || !IsEnemyOf(inPlayer, theOwner))
            continue;
        ScoutRules::Point theBuilding = { theRecord.mX, theRecord.mY };
        theFoe.push_back(theBuilding);
    }
    theCounts.mTowns += (unsigned long)theOwn.size();
    theCounts.mFoes += (unsigned long)theFoe.size();

    ScoutRules::Point theCell = { 0, 0 };
    long theDist = 0;
    long theSize = gWorld.mMap != NULL ? gWorld.mMap->GetSize() : 0;
    bool thePicked = ScoutRules::PickCell(theSize, theOwn.data(), (int)theOwn.size(),
        theFoe.data(), (int)theFoe.size(),
        [&](const ScoutRules::Point& inP) {
            bool theUnseen = theMemory.LookedAgo(inP.mX, inP.mY) == 0xFFFFFFFFu && LandAt(inP.mX, inP.mY) == theLand;
            if (theUnseen)
                theCounts.mCells++;
            return theUnseen;
        },
        [&](const ScoutRules::Point& inP) { return ThreatAt(theID, inP.mX, inP.mY) > 0.0f; },
        theCell, theDist);
    if (!thePicked)
        return;
    theCounts.mPicked++;

    EEUnit* theBest = NULL;
    float theFastest = 0.0f;
    long theReach = ScoutRules::kHome * ScoutRules::kHome;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mTileX < 0 || theUnit->mUnitClass == NULL
            || theUnit->mUnitClass->mCategory != kUnitCategoryLand || theUnit->mComplexUnit == NULL
            || theUnit->mComplexUnit->mGoalState != NULL
            || !Roles::IsSoldier(theUnit->mUnitClass->mCategory, theUnit->mUnitClass->mFamily, theUnit->mUnitClass->mRange))
            continue;
        long theDX = theUnit->mTileX - theBaseX, theDY = theUnit->mTileY - theBaseY;
        if (theDX * theDX + theDY * theDY > theReach)
            continue;
        float theSpeed = SpeedOf(theUnit);
        if (ScoutRules::Faster(theSpeed, theFastest)) {
            theFastest = theSpeed;
            theBest = theUnit;
        }
    }
    if (theBest == NULL)
        return;
    ScoutMoveTo(theBest->mComplexUnit, (float)theCell.mX + 0.5f, (float)theCell.mY + 0.5f);
    theState.mScout = theBest;
    theState.mScoutID = theBest->mUnitID;
    theState.mSentAt = inNow != 0 ? inNow : 1;
    theCounts.mSent++;
}

}
