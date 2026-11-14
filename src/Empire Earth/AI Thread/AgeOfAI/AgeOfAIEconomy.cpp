// The economy of an Age of AI player: the income rates and the worker plan before farms (AgeOfAIWorkers.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/econ.c
// (rates_update, budget_reserve, econ_targets) and dll/ai/food.c (food_plan_needs) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07,
// GPL-3.0. From the Copper Age on the game's own targets stand (the late rules are not ported yet), and so do the military's
// reserves (navy, landing, expansion), the field wood and the pull of gatherers to food.
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
#include "ee/EECPUnitManager.h"
#include "ee/EECPBuildManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "AgeOfAI.h"
#include "AgeOfAISettings.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAIWorkers.h"

namespace AgeOfAI {

namespace {

const int kFarmEpoch = 2;   // farms and granaries come with the Copper Age

// The income of each resource since the game began, and its rate: per minute in total and per worker-minute (with 2 or
// more workers on it; the defaults until then), measured every 30 seconds and smoothed over about 2 minutes
struct Rates {
    unsigned long mTime;
    long mLast[7];
    float mRate[7], mPerWorker[7];
} sRates[16];
long sIncome[16][7];
const float kDefaultPerWorker[7] = {0, 0, 10, 10, 5, 8, 6};

long Epoch(EEPlayer* inPlayer)
{
    return inPlayer->mTechnologies != NULL ? inPlayer->mTechnologies->mCurrentEpoch : -1;
}

const SettingsAI& SettingsOf(EEComputerPlayer* inPlayer)
{
    return gSettingsAI[inPlayer->GetDifficulty() > 2 ? 1 : inPlayer->GetDifficulty()];
}

long EpochCost(EEPlayer* inPlayer, long inEpoch, long inResource)
{
    if (inEpoch < 0 || inEpoch > 13 || inPlayer->mTechnologies == NULL)
        return 0;
    EETechTreeNode* theNode = inPlayer->mTechnologies->Get(inEpoch);
    long theCost = theNode != NULL ? theNode->GetCost(inResource, 0x20, 1.0f) : 0;
    return theCost > 0 ? theCost : 0;
}

// What the next epoch costs and the player holds back (the copper plan): the Copper Age's cost, or with next_epoch only the
// next epoch's; nothing from the Copper Age on (not ported: the late rules)
void BudgetReserve(EEComputerPlayer* inPlayer, int inCopperPlan, long outReserve[7])
{
    for (int r = 0; r < 7; r++)
        outReserve[r] = 0;
    long theNext = Epoch(inPlayer) + 1;
    if (theNext > 13 || theNext > kFarmEpoch)
        return;
    for (int r = 2; r < 7; r++)
        for (long e = theNext; e <= (inCopperPlan == 2 ? theNext : kFarmEpoch); e++)
            outReserve[r] += EpochCost(inPlayer, e, r);
}

long Workforce(EECPResourceManager* inResources)
{
    long theCount = 0;
    for (int t = 0; t < 7; t++)
        theCount += (long)inResources->mGatherers[t].size();
    return theCount;
}

// The wood of the next three queued buildings up to the player's epoch: a building it cannot pay for is no demand
long QueuedWood(EEComputerPlayer* inPlayer, EECPBuildManager* inBuild)
{
    long theWood = 0;
    int theCount = 0;
    for (long e = 0; e <= Epoch(inPlayer) && e < 14 && theCount < 3; e++) {
        const EECPBuildQueue& theQueue = inBuild->mQueues[e];
        for (EECPBuildQueue::const_iterator it = theQueue.begin(); it != theQueue.end() && theCount < 3; ++it) {
            if (it->first >= 871 || gUnitClassTable[it->first] == NULL)
                continue;
            EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[it->first]->mTechnology);
            long theCost = theNode != NULL ? theNode->GetCost(kResourceWood, (long)it->first, 1.0f) : 0;
            if (theCost > 0) {
                theWood += theCost;
                theCount++;
            }
        }
    }
    return theWood;
}

}

TrainingScope::TrainingScope(EEComputerPlayer* inPlayer, long inUnitClass) : mPlayer(inPlayer)
{
    for (int r = 0; r < 7; r++)
        this->mHidden[r] = 0;
    if (EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (theSettings.copper_plan == 0 || Epoch(inPlayer) >= kFarmEpoch || inPlayer->mCivilizationManager == NULL
        || inPlayer->mCivilizationManager->mResourceManager == NULL)
        return;
    long theStock[7], theReserve[7], theVisible[7];
    for (int r = 0; r < 7; r++)
        theStock[r] = inPlayer->mResourceCaps[r];
    BudgetReserve(inPlayer, theSettings.copper_plan, theReserve);
    bool theCitizen = false;
    if (inUnitClass >= 0 && inUnitClass < 871 && gUnitClassTable[inUnitClass] != NULL) {
        EEUnitClass* theClass = gUnitClassTable[inUnitClass];
        theCitizen = (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kCitizen) != 0;
    }
    Workers::Visible(theStock, theReserve, theCitizen, theSettings.copper_plan, Workforce(inPlayer->mCivilizationManager->mResourceManager),
                     theSettings.min_workforce, theVisible);
    for (int r = 2; r < 7; r++) {
        this->mHidden[r] = theStock[r] - theVisible[r];
        inPlayer->mResourceCaps[r] -= this->mHidden[r];
    }
}

TrainingScope::~TrainingScope()
{
    for (int r = 2; r < 7; r++)
        this->mPlayer->mResourceCaps[r] += this->mHidden[r];
}

// An income of the player's, from EEComputerPlayer::AddResource
void OnIncome(EEComputerPlayer* inPlayer, long inResource, long inAmount)
{
    if (inAmount > 0 && inResource >= 2 && inResource < 7)
        sIncome[inPlayer->mPlayerID & 15][inResource] += inAmount;
}

// The income rates, every 30 game seconds
void RatesUpdate(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    unsigned idx = inPlayer->mPlayerID & 15;
    Rates& theRates = sRates[idx];
    if (theRates.mTime != 0 && inNow >= theRates.mTime && inNow - theRates.mTime < 30000)
        return;
    if (theRates.mTime == 0 || inNow < theRates.mTime) {   // the first look, or a new game
        memset(&theRates, 0, sizeof theRates);
        memcpy(theRates.mPerWorker, kDefaultPerWorker, sizeof kDefaultPerWorker);
        for (int r = 0; r < 7; r++)
            theRates.mLast[r] = sIncome[idx][r];
        theRates.mTime = inNow ? inNow : 1;
        return;
    }
    EECPResourceManager* theResources = inPlayer->mCivilizationManager != NULL ? inPlayer->mCivilizationManager->mResourceManager : NULL;
    float theMinutes = (float)(inNow - theRates.mTime) / 60000.0f;
    for (int r = 2; r < 7; r++) {
        float theIncome = (float)(sIncome[idx][r] - theRates.mLast[r]);
        theRates.mLast[r] = sIncome[idx][r];
        theRates.mRate[r] = theRates.mRate[r] * 0.75f + 0.25f * theIncome / theMinutes;
        long theWorkers = theResources != NULL ? (long)theResources->mGatherers[r].size() : 0;
        if (theWorkers >= 2)
            theRates.mPerWorker[r] = theRates.mPerWorker[r] * 0.75f + 0.25f * theIncome / (theMinutes * (float)theWorkers);
    }
    theRates.mTime = inNow;
}

// The attempt at the next epoch, pulled forward: the game plans it by a timer, often long after the player could pay. Before
// the Copper Age with the copper plan, and in every epoch with epoch_pacing rush, the attempt moves to now as soon as the
// stock pays the next epoch (the research of the requirements the game then asks for first). A request for them still
// pending after five minutes is asked again. The follow pacing (waiting for the humans) and the timer pacing's catch-up
// are not ported.
void TickPacing(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sPending[16];
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    long theEpoch = Epoch(inPlayer);
    const int kPacingRush = 2;
    bool theRush = theSettings.epoch_pacing == kPacingRush;
    if (!((theSettings.copper_plan != 0 && theEpoch < kFarmEpoch) || theRush))
        return;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theEpoch < 0 || theEpoch >= 13 || inPlayer->mTechnologies->Get(theEpoch + 1) == NULL)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    if (theCivilization->mEpochResearchRequested) {
        if (sPending[idx] == 0 || inNow < sPending[idx])
            sPending[idx] = inNow ? inNow : 1;
        else if (inNow - sPending[idx] > 5 * 60000) {
            theCivilization->mEpochResearchRequested = false;
            sPending[idx] = 0;
        }
    } else
        sPending[idx] = 0;
    for (int r = 2; r < 7; r++)
        if (inPlayer->mResourceCaps[r] < EpochCost(inPlayer, theEpoch + 1, r))
            return;
    if (theCivilization->mNextEpochAdvanceTime > inNow + 5000)
        theCivilization->mNextEpochAdvanceTime = inNow;
}

// The gatherers wanted of each resource, after the game's own count (EECPResourceManager::Update, every 5 seconds):
// before the Copper Age the plan of AgeOfAIWorkers.h replaces it
void EconTargets(EECPResourceManager* inResources)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    int theCopperPlan = theSettings.copper_plan;
    if (theCopperPlan == 0 || Epoch(thePlayer) >= kFarmEpoch)
        return;
    unsigned idx = thePlayer->mPlayerID & 15;
    long theTotal = 0;
    for (int r = 2; r < 7; r++)
        theTotal += (long)inResources->mWantedGatherers[r];
    long theStock[7], theDemand[7], theReserve[7], theNeed[7];
    for (int r = 0; r < 7; r++) {
        theStock[r] = thePlayer->mResourceCaps[r];
        theDemand[r] = inResources->mNeeds[r];
    }
    BudgetReserve(thePlayer, theCopperPlan, theReserve);
    long theMore = 0, theCeiling = -1;
    if (theSettings.food_plan.count_trained) {
        long theWant = (long)(thePlayer->GetCitizenPercentage() * (float)thePlayer->mPopulationCap);
        if (theCopperPlan == 1 && theWant > theSettings.min_workforce)
            theWant = theSettings.min_workforce;
        theMore = theWant - Workforce(inResources);
        long theQueued = inResources->mManager->mBuildManager != NULL ? QueuedWood(thePlayer, inResources->mManager->mBuildManager) : 0;
        theCeiling = (theReserve[3] > theQueued ? theReserve[3] : theQueued) + 100;   // the fields come with farms
    }
    Workers::Needs(theDemand, theStock, theReserve, theSettings.mineral_worker_margin, EarlyTowerStone(thePlayer), theMore, theCeiling,
                   theNeed);
    unsigned long theTarget[7];
    for (int r = 0; r < 7; r++)
        theTarget[r] = inResources->mWantedGatherers[r];
    if (Workers::Split(theNeed, sRates[idx].mTime != 0 ? sRates[idx].mPerWorker : kDefaultPerWorker, (int)theTotal, theSettings.reserve_plan != 0,
                       theCopperPlan, theSettings.food_floor, theTarget))
        for (int r = 2; r < 7; r++)
            inResources->mWantedGatherers[r] = theTarget[r];
}

}
