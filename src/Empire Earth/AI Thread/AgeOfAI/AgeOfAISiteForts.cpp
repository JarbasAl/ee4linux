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
#include "AgeOfAI/AgeOfAISiteFortsRules.h"

namespace AgeOfAI {

namespace {

using namespace FortRules;

struct Fort {
    long mX, mY;
    int mTowers;
    unsigned long mTime;
};

struct FortState {
    Fort mForts[kMaxForts];
    int mCount;
    unsigned long mLast;
};

FortState sForts[16];
SiteFortCounts sFortCounts[16];

// A mine with a tile of sea within a ship's reach, upstream's sea test (map_area with the water layer).
bool SeaNear(long inX, long inY)
{
    for (long dx = -kSeaRadius; dx <= kSeaRadius; dx++)
        for (long dy = -kSeaRadius; dy <= kSeaRadius; dy++)
            if (InSeaReach(dx, dy)
                && gContinentManager.GetContinent(EETileCoordinate(inX + dx, inY + dy), 1) != 0)
                return true;
    return false;
}

struct Work {
    EEUnit* mSite;
    long mWorkers;
};

// The mine the player's citizens work the most, with sea or a recent hurt for the exposure bonus. As upstream,
// this counts the citizens whose current goal gathers at each mine, whether walking there or working it.
bool PickMine(EEComputerPlayer* inPlayer, const FortState& inState, long& outX, long& outY,
              bool& outSea, bool& outHurt, unsigned long& outSites)
{
    unsigned thePI = inPlayer->mPlayerID & 15;
    Work theWork[64];
    int theSites = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mComplexUnit == NULL)
            continue;
        EEUGGather* theGoal = dynamic_cast<EEUGGather*>(theUnit->mComplexUnit->mGoalState);
        EEUnit* theSite = theGoal != NULL ? theGoal->mResource : NULL;
        if (theSite == NULL || theSite->mResourceType < kResourceStone || theSite->mResourceType > kResourceIron
            || theSite->GetResourceAmount() <= 0)
            continue;
        int k = 0;
        while (k < theSites && theWork[k].mSite != theSite) k++;
        if (k == theSites) {
            if (theSites == 64)
                continue;
            theWork[theSites].mSite = theSite;
            theWork[theSites].mWorkers = 0;
            theSites++;
        }
        theWork[k].mWorkers++;
    }
    for (int k = 0; k < theSites; k++)
        if (theWork[k].mWorkers < kMinWorkers)
            theWork[k].mSite = NULL;
    int theCandidates = 0;
    for (int k = 0; k < theSites; k++)
        if (theWork[k].mSite != NULL)
            theCandidates++;
    outSites += (unsigned long)theCandidates;

    long theBest = 0, theBX = 0, theBY = 0;
    bool theBSea = false, theBHurt = false;
    for (int k = 0; k < theSites; k++) {
        EEUnit* theSite = theWork[k].mSite;
        if (theSite == NULL)
            continue;
        long theSize = theSite->mUnitClass != NULL ? theSite->mUnitClass->GetTileSize() : 1;
        long theX = MineCenter(theSite->mTileX, theSize), theY = MineCenter(theSite->mTileY, theSize);
        bool theTaken = false;
        for (int f = 0; f < inState.mCount && !theTaken; f++)
            theTaken = FortTaken(inState.mForts[f].mX, inState.mForts[f].mY, theX, theY);
        if (theTaken)
            continue;
        bool theSea = SeaNear(theX, theY);
        bool theHurt = HurtNearAt(thePI, theX, theY) > 0;
        long theScore = Score(theWork[k].mWorkers, theSea || theHurt);
        if (theScore <= theBest)
            continue;
        theBest = theScore;
        theBX = theX;
        theBY = theY;
        theBSea = theSea;
        theBHurt = theHurt;
    }
    if (theBest == 0)
        return false;
    outX = theBX;
    outY = theBY;
    outSea = theBSea;
    outHurt = theBHurt;
    return true;
}

// One tower of the fort's next corner, one order per tick. No room at a corner moves to the next, as upstream.
void BuildTower(EEComputerPlayer* inPlayer, Fort& ioFort, SiteFortCounts& ioCounts)
{
    EECPCivilizationManager* theCiv = inPlayer->mCivilizationManager;
    if (theCiv == NULL || theCiv->mBuildManager == NULL || inPlayer->mTechnologies == NULL)
        return;
    EECPBuildManager* theBuild = theCiv->mBuildManager;
    long theType = NewestTowerOf(inPlayer);
    long theEpoch = inPlayer->mTechnologies->mCurrentEpoch;
    EETechTreeNode* theNode = theType > 0 && theType < 871 && gUnitClassTable[theType] != NULL
                              ? inPlayer->mTechnologies->Get(gUnitClassTable[theType]->mTechnology) : NULL;
    long theCost = theNode != NULL ? theNode->GetCost(kResourceStone, theType, 1.0f) : 0;
    if (theType == 0 || TowerIsQueued(theBuild) || theEpoch < 0 || theEpoch >= 14
        || inPlayer->mResourceCaps[kResourceStone] < theCost + 100) {
        ioCounts.mWait++;
        return;
    }
    long theCX, theCY;
    Corner(ioFort.mX, ioFort.mY, ioFort.mTowers, theCX, theCY);
    long theOffsets[9][2];
    int theN = Ring(theOffsets);
    for (int k = 0; k < theN; k++) {
        long theX = theCX + theOffsets[k][0], theY = theCY + theOffsets[k][1];
        if (!CanPlace(inPlayer, theType, theX, theY))
            continue;
        theBuild->QueueBuildingAt(theType, theX, theY);
        ioFort.mTowers++;
        ioCounts.mTowers++;
        return;
    }
    ioFort.mTowers++;
    ioCounts.mSkipped++;
}

}  // namespace

SiteFortCounts SiteFortCountsOf(unsigned long inPlayerID)
{
    return sFortCounts[inPlayerID & 15];
}

bool SiteFortsBusy(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium).site_forts <= 0)
        return false;
    const FortState& theState = sForts[inPlayer->mPlayerID & 15];
    for (int f = 0; f < theState.mCount; f++) {
        const Fort& theFort = theState.mForts[f];
        if (theFort.mTime != 0 && inNow >= theFort.mTime && inNow - theFort.mTime < kBusyMs
            && theFort.mTowers < kTowers)
            return true;
    }
    return false;
}

// SITE FORTS ([ai_base] site_forts, off in the shipped file): a fort round the most-worked mine, with a corner
// tower at each of its four corners. Every twenty game seconds one tower goes into the build queue, and a new fort
// is picked only when none is being built. Upstream's wall runs and the side left open towards the base are not
// ported; their places in the state and the rule arithmetic are in AgeOfAISiteFortsRules.h.
void SiteFortsTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium);
    if (theSettings.site_forts <= 0)
        return;
    unsigned thePI = inPlayer->mPlayerID & 15;
    FortState& theState = sForts[thePI];
    SiteFortCounts& theCounts = sFortCounts[thePI];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
        std::memset(&theCounts, 0, sizeof(theCounts));
    }
    if (theState.mLast != 0 && inNow >= theState.mLast && inNow - theState.mLast < kTickMs)
        return;
    theState.mLast = inNow != 0 ? inNow : 1;
    theCounts.mChecks++;

    EECPCivilizationManager* theCiv = inPlayer->mCivilizationManager;
    if (theCiv == NULL || theCiv->mBuildManager == NULL)
        return;
    // Upstream needs a standing town centre before it looks for a mine; the fort itself needs no base point here.
    long theBaseX = theCiv->mBuildManager->mTownCenterTile.mX;
    long theBaseY = theCiv->mBuildManager->mTownCenterTile.mY;
    if (theBaseX < 0 || theBaseY < 0 || theBaseX == -10000 || theBaseY == -10000)
        return;

    for (int f = 0; f < theState.mCount; f++) {
        Fort& theFort = theState.mForts[f];
        if (theFort.mTowers >= kTowers || theFort.mTime == 0 || inNow < theFort.mTime || inNow - theFort.mTime >= kBusyMs)
            continue;
        theCounts.mBusy++;
        BuildTower(inPlayer, theFort, theCounts);
        return;
    }
    if (theState.mCount >= theSettings.site_forts || theState.mCount >= kMaxForts)
        return;

    long theX = 0, theY = 0;
    bool theSea = false, theHurt = false;
    if (!PickMine(inPlayer, theState, theX, theY, theSea, theHurt, theCounts.mSites))
        return;
    Fort& theFort = theState.mForts[theState.mCount++];
    theFort.mX = theX;
    theFort.mY = theY;
    theFort.mTowers = 0;
    theFort.mTime = inNow != 0 ? inNow : 1;
    theCounts.mPicked++;
    if (theSea || theHurt)
        theCounts.mExposed++;
}

}  // namespace AgeOfAI
