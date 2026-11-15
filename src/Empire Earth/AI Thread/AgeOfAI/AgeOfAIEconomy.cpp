// The economy of an Age of AI player: the income rates and the worker plan before farms (AgeOfAIWorkers.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/econ.c
// (rates_update, budget_reserve, econ_targets) and dll/ai/food.c (food_plan_needs) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07,
// GPL-3.0. From the Copper Age on the game's own targets stand (the late rules are not ported yet), and so do the military's
// reserves (navy, landing, expansion), the field wood and the pull of gatherers to food.
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
#include "ee/EECPUnitManager.h"
#include "ee/EECPBuildManager.h"
#include "ee/EEEvent.h"
#include "ee/EECPResearchManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "AgeOfAI.h"
#include "AgeOfAISettings.h"
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

int Pct(float inShare)
{
    float theX = inShare * 100.0f;
    return (int)(theX < 0 ? theX - 0.5f : theX + 0.5f);
}

long Workforce(EECPResourceManager* inResources)
{
    long theCount = 0;
    for (int t = 0; t < 7; t++)
        theCount += (long)inResources->mGatherers[t].size();
    return theCount;
}

// Whether a tech's event raises how fast citizens gather (is_gather_tech in econ.c): an effect of kind 2 on the citizen
// classes or on all, for an attribute between 0x1a and 0x21 but 0x1e
bool IsGatherTech(EETechTreeNode* inNode)
{
    EEEvent* theEvent = EEEvent::Get(inNode->mData->mResearchedEvent);
    if (theEvent == NULL)
        return false;
    for (unsigned long k = 0; k < theEvent->GetNumEffects() && k < 64; k++) {
        if (theEvent->GetEffect(k) >= 2000)
            continue;
        EEEventEffect* theEffect = EEEvent::GetEventEffect(theEvent->GetEffect(k));
        long theClass = theEffect->mUnitClass;
        long theAttribute = theEffect->mAttribute;
        if (theEffect->mKind == 2 && (theClass == 0x20 || theClass == 0x1e0 || theClass == -1) && theAttribute >= 0x1a
            && theAttribute <= 0x21 && theAttribute != 0x1e)
            return true;
    }
    return false;
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

// GATHERING RESEARCH (gather_research in econ.c, [ai_base] gather_research): every 15 s one available, unresearched
// candidate with a gathering effect is scheduled through the research manager and taken out of the candidates
void GatherResearch(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (!SettingsOf(inPlayer).gather_research || (inNow >= sLast[idx] && inNow - sLast[idx] < 15000))
        return;
    sLast[idx] = inNow;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mResearchManager == NULL || theCivilization->mResourceManager == NULL
        || Workforce(theCivilization->mResourceManager) < 10)
        return;
    EECPResearchManager* theResearch = theCivilization->mResearchManager;
    std::vector<long>& theCandidates = theResearch->mCandidates;
    for (size_t q = 0; theCandidates.size() < 500 && q < theCandidates.size(); q++) {
        long theID = theCandidates[q];
        EETechTreeNode* theNode = inPlayer->mTechnologies->Get(theID);
        if (theNode == NULL || theNode->mIsResearched || !theNode->mIsAvailable || !IsGatherTech(theNode))
            continue;
        theCandidates.erase(theCandidates.begin() + q);
        theResearch->Research(theID);
        return;
    }
}

// PAID UPGRADES (epoch_upgrades in econ.c, [ai_base] paid_upgrades): the game researches the kind-7 techs of an epoch (the
// unit and building line upgrades) for a computer player on the spot, without cost, building or time. An Age of AI
// player schedules each with its research manager instead: it is researched at its building and paid like a human's.
// The army plan's UPGRADE bit (a line it does not use yet waits) is not ported; it is off by default.
bool EpochUpgrades(EEComputerPlayer* inPlayer, EETechTreeEpoch* inEpoch)
{
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (!SettingsOf(inPlayer).paid_upgrades || theCivilization == NULL || theCivilization->mResearchManager == NULL)
        return false;
    for (std::map<long, EETechTreeNode*>::iterator theNode = inEpoch->mNodes.begin(); theNode != inEpoch->mNodes.end(); ++theNode) {
        EETechTreeNode* theTech = theNode->second;
        if (theTech->mData->mKind == 7 && theTech->mData->mResearchedWithEpoch && !theTech->mIsResearched)
            theCivilization->mResearchManager->Research(theTech->mData->mID);
    }
    return true;
}

// LOST UPGRADES (upgrades_retry in econ.c, paid_upgrades): the research manager hands each scheduled tech to a building
// once and forgets it, and a building that leaves the map loses its queue. Every 30 s each available, unresearched line
// upgrade of the player's epochs or earlier that is in no research list and no building's queue or production is scheduled
// again, when the stock pays its price (a refused schedule adds the price to the resource demand). Not ported: the
// tech that the game marked as not queueable, the army plan's UPGRADE bit, and the reserve of the price of four lines in use.
void UpgradesRetry(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (inNow < sLast[idx])
        sLast[idx] = 0;
    if (!SettingsOf(inPlayer).paid_upgrades || (sLast[idx] != 0 && inNow - sLast[idx] < 30000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mResearchManager == NULL || inPlayer->mTechnologies == NULL)
        return;
    EECPResearchManager* theResearch = theCivilization->mResearchManager;
    std::vector<bool> theHeld(15000, false);
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mComplexUnit == NULL)
            continue;
        EEComplexUnit* theBuilding = theUnit->mComplexUnit;
        if (theBuilding->mCurrentResearch >= 0 && theBuilding->mCurrentResearch < 15000)
            theHeld[theBuilding->mCurrentResearch] = true;
        for (size_t q = 0; q < theBuilding->mBuildQueue.size(); q++)
            if (theBuilding->mBuildQueue[q].mTechnology >= 0 && theBuilding->mBuildQueue[q].mTechnology < 15000)
                theHeld[theBuilding->mBuildQueue[q].mTechnology] = true;
    }
    for (long theID = 1000; theID < 15000; theID++) {
        EETechTreeNode* theTech = inPlayer->mTechnologies->Get(theID);
        if (theTech == NULL || theTech->mData == NULL || theTech->mData->mKind != 7 || theTech->mIsResearched || !theTech->mIsAvailable
            || theTech->mEpoch > inPlayer->mTechnologies->mCurrentEpoch || !theTech->mData->mResearchedWithEpoch || theHeld[theID])
            continue;
        bool theListed = false;
        for (int e = 0; e < 14 && !theListed; e++)
            theListed = std::find(theResearch->mEpochIDs[e].begin(), theResearch->mEpochIDs[e].end(), theID) != theResearch->mEpochIDs[e].end();
        bool thePays = true;
        for (int r = 2; r < 7; r++)
            if (inPlayer->mResourceCaps[r] < theTech->mData->mCosts[r])
                thePays = false;
        if (!theListed && thePays)
            theResearch->Research(theID);
    }
}

// THE PLAYER'S OWN SETTINGS (settings_apply in settings.c), once per player and game, on its first update: the price it pays, whether
// it waits for the humans' epoch (every pacing but classic does not), the epoch timer's range, the attack interval, the
// attack group's size and the chances of the attacks. The first attack's range, the citizen share, the attention activities
// are not ported (their fields are not mapped; their defaults change nothing).
void ApplySettings(EEComputerPlayer* inPlayer)
{
    static EEComputerPlayer* sDone[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (sDone[idx] == inPlayer)
        return;
    sDone[idx] = inPlayer;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    int theDifficulty = (int)inPlayer->GetDifficulty();
    const int kPacingClassic = 0;
    int theCost = Pct(theSettings.cost_percent);
    if (theCost > 0) {
        inPlayer->SetIsCheating(theCost != 100);
        if (theDifficulty == 0)
            inPlayer->mUnitCostReductionEasy = theCost;
        else if (theDifficulty == 2)
            inPlayer->mUnitCostReductionHard = theCost;
        else
            inPlayer->mUnitCostMedium = (float)theCost;
    }
    inPlayer->mIgnoreOthers = theSettings.epoch_pacing != kPacingClassic;
    long theEpochMin = theSettings.epoch_min, theEpochMax = theSettings.epoch_max < theEpochMin ? theEpochMin : theSettings.epoch_max;
    if (theEpochMin > 0) {
        inPlayer->mEpochFrequencyMin = theEpochMin;
        inPlayer->mEpochFrequencyMax = theEpochMax;
        EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
        unsigned long theNow = gPhysics->mGameTime;
        if (theCivilization != NULL) {
            unsigned long theTime = theCivilization->mNextEpochAdvanceTime;
            unsigned long theLow = theNow + (unsigned long)theEpochMin * 60000ul, theHigh = theNow + (unsigned long)theEpochMax * 60000ul;
            if (theTime < theLow || theTime > theHigh) {
                long theSpan = theEpochMax - theEpochMin + 1;
                theCivilization->mNextEpochAdvanceTime = theLow + (unsigned long)((idx * 7) % theSpan) * 60000ul;
            }
        }
    }
    long theAttackMin = theSettings.attack_interval_min, theAttackMax = theSettings.attack_interval_max < theAttackMin ? theAttackMin : theSettings.attack_interval_max;
    if (theAttackMin > 0) {
        inPlayer->mAttackFrequencyMin = theAttackMin;
        inPlayer->mAttackFrequencyMax = theAttackMax;
    }
    int theGroup = Pct(theSettings.attack_group_percent);
    if (theGroup > 0) {
        if (theDifficulty == 0)
            inPlayer->mAttackGroupModifierEasy = theGroup;
        else if (theDifficulty == 2)
            inPlayer->mAttackGroupModifierHard = theGroup;
        else
            inPlayer->mAttackGroupModifierMedium = theGroup;
    }
    if (Pct(theSettings.attack_on_contact) >= 0)
        inPlayer->mAttackEnemyChance = Pct(theSettings.attack_on_contact);
    if (Pct(theSettings.attack_for_resources) >= 0)
        inPlayer->mAttackToClaimResourceChance = Pct(theSettings.attack_for_resources);
    if (Pct(theSettings.attack_revenge) >= 0)
        inPlayer->mAttackForRevengeChance = Pct(theSettings.attack_revenge);
    if (theSettings.last_stand_buildings >= 0)
        inPlayer->mBonzaiAttackLevel = theSettings.last_stand_buildings;
}

}
