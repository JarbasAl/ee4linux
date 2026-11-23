// The economy of an Age of AI player: the income rates and the worker plan before farms (AgeOfAIWorkers.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/econ.c
// (rates_update, budget_reserve, econ_targets, follow_held, prod_sync) and dll/ai/food.c (food_plan_needs) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07,
// GPL-3.0. From the Copper Age on the game's own targets stand, apart from the food pile of late_targets food_to_metals
// (off in the shipped file) and the worker split of recover_economy. The other late rules are not ported, nor are the
// military's reserves (navy, landing, expansion), the field wood and the pull of gatherers to food.
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
#include "AgeOfAI/AgeOfAIEarlyRequirementsRules.h"
#include "AgeOfAI/AgeOfAIPopulationRules.h"
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
#include "ee/EECPResourceCenter.h"
#include "ee/EEResourceType.h"
#include "AgeOfAI.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"
#include "AgeOfAIWorkers.h"

namespace AgeOfAI {

namespace {

const int kFarmEpoch = 2;       // farms and granaries come with the Copper Age
const int kPacingTimer = 1;     // epoch_pacing choice: classic, timer, rush, follow, independent
const int kPacingFollow = 3;

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
    return SettingsAIOf(inPlayer->mPlayerID, inPlayer->GetDifficulty());
}

// The highest epoch a human player has reached, or -1 with no human in the game.
long HighestHumanEpoch()
{
    long theHighest = -1;
    for (unsigned long p = 0; p < gWorld.mNumPlayers && p < 16; p++) {
        EEPlayer* theOther = gWorld.mPlayers[p];
        if (theOther == NULL || theOther->mHumanPlayer == NULL || theOther->mTechnologies == NULL)
            continue;
        long theOtherEpoch = theOther->mTechnologies->mCurrentEpoch;
        if (theOtherEpoch > theHighest)
            theHighest = theOtherEpoch;
    }
    return theHighest;
}

// epoch_pacing=follow: the AI keeps level with the highest human epoch. While no human is ahead it holds its next
// epoch's attempt a minute off and reserves nothing for it, but still asks for its requirements. With no human in
// the game the timer alone counts.
bool FollowHeld(EEComputerPlayer* inPlayer)
{
    if (SettingsOf(inPlayer).epoch_pacing != kPacingFollow || inPlayer->mTechnologies == NULL)
        return false;
    long theHighest = HighestHumanEpoch();
    return theHighest >= 0 && theHighest <= Epoch(inPlayer);
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
    if (FollowHeld(inPlayer))
        return;
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

// The game pulls an AI's next-epoch attempt forward when a human reaches an epoch. With timer pacing it goes without
// that catch-up: the timer alone counts. Every other pacing keeps the catch-up, and so does the game's own AI.
bool EpochCatchUpSkipped(EEComputerPlayer* inPlayer)
{
    return EngineOf(inPlayer->mPlayerID) == kEngineAgeOfAI
        && SettingsOf(inPlayer).epoch_pacing == kPacingTimer;
}

// Upstream econ.c catch_up_ais, on in the shipped file. The game itself calls the epoch catch-up only when a human
// reaches an epoch. Every two game seconds this rule also checks the other computer players. The first sighting of a
// player only records its epoch; a later rise pulls this AI's next attempt forward through its own CatchUpEpoch, which
// keeps the timer-pacing and Atomic-WW1 rules. Humans stay with the game's own call.
struct CatchUpAIsState {
    long mSeen[16];
    unsigned long mLast;
};
static CatchUpAIsState sCatchUpAIs[16];
static CatchUpAIsCounts sCatchUpAIsCounts[16];

void CatchUpAIsTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI || inPlayer->mTechnologies == NULL
        || !SettingsOf(inPlayer).catch_up_ais)
        return;
    const unsigned long theIndex = inPlayer->mPlayerID & 15;
    CatchUpAIsState& theState = sCatchUpAIs[theIndex];
    if (theState.mLast == 0 || inNow < theState.mLast)
        std::fill(theState.mSeen, theState.mSeen + 16, -1);   // the first look of a game, or a new game
    if (theState.mLast != 0 && inNow >= theState.mLast && inNow - theState.mLast < 2000)
        return;
    theState.mLast = inNow != 0 ? inNow : 1;
    CatchUpAIsCounts& theCounts = sCatchUpAIsCounts[theIndex];
    theCounts.mChecks++;
    for (unsigned long p = 1; p < gWorld.GetNumPlayers() && p < 16; p++) {
        EEPlayer* theOther = gWorld.GetPlayer(p);
        if (theOther == NULL || theOther == inPlayer || theOther->IsHuman() || theOther->mTechnologies == NULL)
            continue;
        long theEpoch = theOther->mTechnologies->mCurrentEpoch;
        long theSeen = theState.mSeen[p];
        theState.mSeen[p] = theEpoch;
        if (theSeen < 0 || theEpoch <= theSeen || inPlayer->mTechnologies->mCurrentEpoch >= theEpoch)
            continue;
        theCounts.mCalls++;
        inPlayer->CatchUpEpoch(theOther);
    }
}

CatchUpAIsCounts CatchUpAIsCountsOf(unsigned long inPlayerID)
{
    return sCatchUpAIsCounts[inPlayerID & 15];
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
// pending after five minutes is asked again. With follow, while no human is ahead, the attempt is kept a minute off;
// once a human is ahead it moves up like rush. With timer it ignores the game's catch-up (EpochCatchUpSkipped).
void TickPacing(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sPending[16];
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    long theEpoch = Epoch(inPlayer);
    const int kPacingRush = 2;
    bool theRush = theSettings.epoch_pacing == kPacingRush;
    bool theFollow = theSettings.epoch_pacing == kPacingFollow;
    long theHighest = theFollow ? HighestHumanEpoch() : -1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theFollow && theHighest >= 0 && theHighest <= theEpoch) {
        if (theCivilization != NULL && theCivilization->mNextEpochAdvanceTime < inNow + 60000)
            theCivilization->mNextEpochAdvanceTime = inNow + 60000;
        return;
    }
    if (!((theSettings.copper_plan != 0 && theEpoch < kFarmEpoch) || theRush || (theFollow && theHighest >= 0)))
        return;
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

// EARLY REQUIREMENTS (econ.c early_requirements, off in the shipped file): the game asks for the new things the next
// epoch needs only at its advance timer, which can be ten minutes after the AI could pay. Ask as soon as the epoch is
// reached with the Copper plan (prefarm), or within ten minutes of the timer otherwise, then renew after three minutes
// without progress. The game's own AttemptEpochAdvance then sees its request-out byte set and leaves the ask to us.
// The budget's requirement-unit branch is not ported (the port has no training budget). EEAIGPG calls this each tick.
struct EarlyReqState {
    EarlyReqCounts mCounts;
    unsigned long mAskedAt = 0;
    int mAskedMissing = 0;
    EETechTree* mFixedTree = NULL;   // the decoration fix runs once per tech tree, as upstream's g_deco_done
};
EarlyReqState sEarlyReq[16];

void EarlyRequirementsTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || inPlayer->mTechnologies == NULL)
        return;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (!theSettings.early_requirements)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    EETechTree* theTree = inPlayer->mTechnologies;
    EarlyReqState& theState = sEarlyReq[idx];
    theState.mCounts.mChecks++;

    // Decoration objects ("x ..." statues and flags the editor places) count as epoch requirements upstream. The
    // request then falls back to one the AI can never place, so drop their techs from the count once per tree.
    if (theState.mFixedTree != theTree) {
        theState.mFixedTree = theTree;
        for (int t = 0; t < 768; t++) {   // upstream's 0x300 table bound
            EEUnitClass* theClass = gUnitClassTable[t];
            if (theClass == NULL || !EarlyRequirementsRules::IsDecorationName(theClass->GetName()))
                continue;
            long theTechID = theClass->mTechnology;
            EETechTreeNode* theNode = theTechID > 0 ? theTree->Get(theTechID) : NULL;
            if (theNode == NULL || theNode->mData == NULL || !theNode->mData->mCountsTowardEpoch)
                continue;
            theNode->mData->mCountsTowardEpoch = false;
            theState.mCounts.mDecoration++;
        }
    }

    long theEpoch = Epoch(inPlayer);
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    EECPResearchManager* theResearch = theCivilization != NULL ? theCivilization->mResearchManager : NULL;
    if (theResearch == NULL || theCivilization == NULL || theEpoch < 0 || theEpoch >= 13)
        return;
    EETechTreeEpoch& theNext = theTree->mEpochs[theEpoch + 1];
    int theMissing = (int)(theNext.GetRequiredSOL() - theNext.GetCurrentSOL());
    if (theMissing < 1) {   // upstream clears its ask timestamp when the gap is filled
        theState.mAskedAt = 0;
        return;
    }
    bool thePrefarm = theSettings.copper_plan != 0 && theEpoch < kFarmEpoch;
    int theDecision = EarlyRequirementsRules::AskDecision(inNow, theState.mAskedAt, theState.mAskedMissing, theMissing,
                                                          theCivilization->mNextEpochAdvanceTime, thePrefarm,
                                                          theCivilization->mEpochResearchRequested);
    if (theDecision == EarlyRequirementsRules::kProgress) {
        theState.mAskedAt = inNow != 0 ? inNow : 1;
        theState.mAskedMissing = theMissing;
        theState.mCounts.mProgress++;
        return;
    }
    if (theDecision == 0)
        return;
    theResearch->ResearchCheapest(theMissing);
    theCivilization->mEpochResearchRequested = true;
    theState.mAskedAt = inNow != 0 ? inNow : 1;
    theState.mAskedMissing = theMissing;
    if (theDecision == EarlyRequirementsRules::kAsk)
        theState.mCounts.mAsks++;
    else
        theState.mCounts.mRenews++;
}

EarlyReqCounts EarlyReqCountsOf(unsigned long inPlayerID)
{
    return sEarlyReq[inPlayerID & 15].mCounts;
}

// Upstream econ.c recovering and the worker split of econ_targets. The other two recovery effects - spending the
// whole stock and refusing soldier training while the economy is low - need the port's missing budget and training
// layers. The two latches follow upstream: reaching min_workforce + 5 records a working economy at any time; without
// that, recovery starts only from minute 30, so a late-epoch start is not read as a collapse.
namespace {
struct RecoverState {
    RecoverCounts mCounts;
    bool mOn = false, mHadEconomy = false;
};
RecoverState sRecover[16];

void RecoverTargets(EECPResourceManager* inResources, const SettingsAI& inSettings, unsigned long inNow,
                    unsigned inIndex)
{
    RecoverState& theState = sRecover[inIndex];
    RecoverCounts& theCounts = theState.mCounts;
    theCounts.mChecks++;
    long theWorkforce = Workforce(inResources);
    bool theOn = theState.mOn ? theWorkforce < inSettings.min_workforce + 5
                              : theWorkforce < inSettings.min_workforce;
    if (theWorkforce >= inSettings.min_workforce + 5)
        theState.mHadEconomy = true;
    if (!theState.mHadEconomy && inNow < 1800000)
        theOn = false;
    if (theOn != theState.mOn) {
        theState.mOn = theOn;
        if (theOn)
            theCounts.mStarts++;
        else
            theCounts.mEnds++;
    }
    if (!theOn)
        return;
    long theTarget[7], theTotal = 0;
    for (int r = 0; r < 7; r++)
        theTarget[r] = (long)inResources->mWantedGatherers[r];
    for (int r = 2; r < 7; r++)
        theTotal += theTarget[r];
    long theWant = theTotal * 2 / 3;
    while (theTarget[2] < theWant) {
        int theFrom = -1;
        for (int r = 3; r < 7; r++)
            if (theTarget[r] > 1 && (theFrom < 0 || theTarget[r] > theTarget[theFrom]))
                theFrom = r;
        if (theFrom < 0)
            break;
        theTarget[theFrom]--;
        theTarget[2]++;
        theCounts.mMoves++;
    }
    for (int r = 2; r < 7; r++)
        inResources->mWantedGatherers[r] = (unsigned long)theTarget[r];
}
}

RecoverCounts RecoverCountsOf(unsigned long inPlayerID)
{
    if (inPlayerID >= 16)
        return RecoverCounts();
    RecoverCounts theCounts = sRecover[inPlayerID].mCounts;
    theCounts.mOn = sRecover[inPlayerID].mOn ? 1 : 0;
    theCounts.mHadEconomy = sRecover[inPlayerID].mHadEconomy ? 1 : 0;
    return theCounts;
}

// The gatherers wanted of each resource, after the game's own count (EECPResourceManager::Update, every 5 seconds):
// before the Copper Age the plan of AgeOfAIWorkers.h replaces it; after it only the recovery split changes the
// game's targets here.
void EconTargets(EECPResourceManager* inResources, unsigned long inNow)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    int theCopperPlan = theSettings.copper_plan;
    unsigned idx = thePlayer->mPlayerID & 15;
    if (Epoch(thePlayer) >= kFarmEpoch) {
        if (theSettings.recover_economy && theCopperPlan != 0 && theSettings.late_targets.wood_gap)
            RecoverTargets(inResources, theSettings, inNow, idx);
        return;
    }
    if (theCopperPlan == 0)
        return;
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

// The FEED half of upstream's cz_targets: the gatherers food needs to pay the finished citizen producers. This is the
// target change only. Upstream also pulls the workers, raises the citizen goal and fills the mine slots from the same
// feed number; those halves need layers the port does not have. Wood keeps its floor of four and each metal the
// gatherers already on it, so a target never takes a worker off a mine.
namespace {
struct FeedTargetsState {
    FeedTargetsCounts mCounts;
};
FeedTargetsState sFeedTargets[16];

long FinishedCitizenProducers(EEPlayer* inPlayer)
{
    if (inPlayer == NULL)
        return 0;
    long theCount = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mUnitClass == NULL)
            continue;
        long theType = (long)theUnit->mUnitClass->mUnitType;
        if (theType != kUnitTypeTownCenter && theType != kUnitTypeCapitol)
            continue;
        EEComplexUnit* theComplex = theUnit->mComplexUnit;
        if (theComplex != NULL && theComplex->mIsBuilt && !theComplex->IsFoundation())
            theCount++;
    }
    return theCount;
}
}  // namespace

FeedTargetsCounts FeedTargetsCountsOf(unsigned long inPlayerID)
{
    return sFeedTargets[inPlayerID & 15].mCounts;
}

void FeedTargets(EECPResourceManager* inResources, unsigned long inNow)
{
    (void)inNow;
    EEComputerPlayer* thePlayer = inResources->mManager != NULL ? inResources->mManager->mPlayer : NULL;
    if (thePlayer == NULL || EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI
        || Epoch(thePlayer) < kFarmEpoch || !SettingsOf(thePlayer).citizen_plan.food_pays)
        return;
    EECPUnitManager* theUnits = thePlayer->mCivilizationManager != NULL ? thePlayer->mCivilizationManager->mUnitManager : NULL;
    if (theUnits == NULL)
        return;
    unsigned idx = thePlayer->mPlayerID & 15;
    long theMissing = theUnits->GetWantedCitizens() - (long)theUnits->GetNumWorkers();
    long theProducers = FinishedCitizenProducers(thePlayer);
    float thePerWorker = sRates[idx].mTime != 0 ? sRates[idx].mPerWorker[2] : kDefaultPerWorker[2];
    long theFeed = FeedTargetsRules::Feed(theMissing, theProducers, thePerWorker, Workers::kFoodPerCitizen,
                                          gSettings.citizen_train_ms);
    long theFloor[7] = {0, 0, 0, FeedTargetsRules::kWoodFloor, 0, 0, 0};
    for (int r = 4; r < 7; r++) {
        long theGatherers = (long)inResources->mGatherers[r].size();
        theFloor[r] = theGatherers > 2 ? theGatherers : 2;
    }
    long theFromWood = 0, theFromMetals = 0;
    long theMoved = FeedTargetsRules::RaiseFood(inResources->mWantedGatherers, theFloor, theFeed,
                                                &theFromWood, &theFromMetals);
    FeedTargetsCounts& theCounts = sFeedTargets[idx].mCounts;
    theCounts.mChecks++;
    theCounts.mMissing = (unsigned long)(theMissing > 0 ? theMissing : 0);
    theCounts.mProducers = (unsigned long)theProducers;
    theCounts.mFeed = (unsigned long)theFeed;
    theCounts.mMoved += (unsigned long)theMoved;
    theCounts.mFromWood += (unsigned long)theFromWood;
    theCounts.mFromMetals += (unsigned long)theFromMetals;
}

// The next epoch's cost held back near its scheduled attempt (econ.c budget_reserve, the plain path). The port's
// BudgetReserve stops at the Copper Age and is the copper-plan variant; the fill pass needs the timer-scaled cost of the
// next epoch even later in the game. With epoch_pacing=follow nothing is held while the AI may advance at any time.
void MineReserve(EEComputerPlayer* inPlayer, unsigned long inNow, long outReserve[7])
{
    for (int r = 0; r < 7; r++)
        outReserve[r] = 0;
    if (FollowHeld(inPlayer))
        return;
    long theNext = Epoch(inPlayer) + 1;
    if (theNext > 13 || theNext < 0 || inPlayer->mCivilizationManager == NULL)
        return;
    long theRemaining = (long)inPlayer->mCivilizationManager->mNextEpochAdvanceTime - (long)inNow;
    float theWindow = (float)SettingsOf(inPlayer).reserve_window_min * 60000.0f;
    float theFraction = theRemaining <= 0 ? 1.0f : theRemaining >= theWindow ? 0.0f
                        : 1.0f - (float)theRemaining / theWindow;
    for (int r = 2; r < 7; r++) {
        long theCost = EpochCost(inPlayer, theNext, r);
        outReserve[r] = theCost > 0 ? (long)(theCost * theFraction) : 0;
    }
}

// FILL THE MINES (mines.c mine_fill, [ai_base] mine_fill fill_slots and mines_first, both off in the shipped file): from
// the Copper Age on, once the game has run three minutes and the workforce reaches 30, turn spare wood- and
// food-gatherers toward metal mines that have a free slot near a drop site (the home continent's alone while the
// population cap is under 120). A metal already piling up above its epoch reserve gets no workers. mines_first also
// draws below a fifth of the workers on wood and a third on food. The added workers change the wanted-gatherer targets;
// the game's own assignment then moves the citizens.
void MineFill(EECPResourceManager* inResources, unsigned long inNow)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    if (!theSettings.mine_fill.fill_slots)
        return;
    unsigned idx = thePlayer->mPlayerID & 15;
    if (Epoch(thePlayer) < kFarmEpoch || inNow < 180000 || Workforce(inResources) < 30)
        return;

    long theSlots[7], theHomeSlots[7];
    MineSlots(thePlayer, inNow, theSlots, theHomeSlots);

    long theStock[7], theDemand[7], theReserve[7];
    for (int r = 0; r < 7; r++) {
        theStock[r] = thePlayer->mResourceCaps[r];
        theDemand[r] = inResources->mNeeds[r];
    }
    MineReserve(thePlayer, inNow, theReserve);

    bool theLow = thePlayer->mPopulationCap < 120;
    long theWant[7] = {0};
    long theGap = 0;
    for (int r = 4; r < 7; r++) {
        theWant[r] = theLow ? theHomeSlots[r] : theSlots[r];
        if (theStock[r] - theReserve[r] > 1500)
            theWant[r] = 0;
        if ((long)inResources->mWantedGatherers[r] < theWant[r])
            theGap += theWant[r] - (long)inResources->mWantedGatherers[r];
    }
    if (theGap <= 0)
        return;

    long theTotal = 0, theSum = 0;
    for (int r = 0; r < 7; r++)
        theTotal += (long)inResources->mGatherers[r].size();
    for (int r = 2; r < 7; r++)
        theSum += (long)inResources->mWantedGatherers[r];
    long theFree = theSum < theTotal ? theTotal - theSum : 0;
    long theWoodNeed = theDemand[3] > theReserve[3] ? theDemand[3] : theReserve[3];
    if (theWoodNeed > 3000)
        theWoodNeed = 3000;
    long theWoodSpare = theStock[3] - theWoodNeed - 500;
    long theFoodSpare = theStock[2] - theReserve[2] - 1000;
    long theWoodFloor = 4, theFoodFloor = (long)inResources->mWantedGatherers[2] * 3 / 5;
    long theTaken = theFree < theGap ? theFree : theGap;

    if (theWoodSpare > 0) {
        float thePerWorker = sRates[idx].mPerWorker[3];
        long theKeep = (long)((float)(theWoodNeed > theStock[3] ? theWoodNeed - theStock[3] : 0)
                              / (2.0f * (thePerWorker > 2.0f ? thePerWorker : 2.0f))) + 2;
        if (theKeep > theWoodFloor)
            theWoodFloor = theKeep;
        while (theTaken < theGap && (long)inResources->mWantedGatherers[3] > theWoodFloor) {
            inResources->mWantedGatherers[3]--;
            theTaken++;
        }
    }
    if (theFoodSpare > 0)
        while (theTaken < theGap && (long)inResources->mWantedGatherers[2] > theFoodFloor) {
            inResources->mWantedGatherers[2]--;
            theTaken++;
        }
    if (theSettings.mine_fill.mines_first) {
        long theWoodFirst = theTotal / 5 > 4 ? theTotal / 5 : 4;
        long theFoodFirst = theTotal / 3 > 6 ? theTotal / 3 : 6;
        while (theTaken < theGap && ((long)inResources->mWantedGatherers[3] > theWoodFirst
                                     || (long)inResources->mWantedGatherers[2] > theFoodFirst)) {
            if ((long)inResources->mWantedGatherers[3] - theWoodFirst >= (long)inResources->mWantedGatherers[2] - theFoodFirst)
                inResources->mWantedGatherers[3]--;
            else
                inResources->mWantedGatherers[2]--;
            theTaken++;
        }
    }
    for (int r = 4; r < 7 && theTaken > 0; r++) {
        long theNeed = theWant[r] - (long)inResources->mWantedGatherers[r];
        if (theNeed <= 0)
            continue;
        if (theNeed > theTaken)
            theNeed = theTaken;
        inResources->mWantedGatherers[r] += theNeed;
        theTaken -= theNeed;
    }
}

// LATE FOOD TARGET (econ.c late_targets bit 8 food_to_metals, off in the shipped file): from the Copper Age on, while food
// lies far above the epoch reserve, lower the farmers' wanted count a little at a time and give those workers to the
// metals by their demand beyond stock. A food stock under the reserve + 500 restores the target up to the fields + 2;
// no more than half the fields stand idle. Not ported from the same setting: the wood-floor pass (bits 1 and 2) and the
// army-refusal metal pull (bit 16), which need the unported training budget and refusal counters.
void LateFoodTargets(EECPResourceManager* inResources, unsigned long inNow)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    if (!theSettings.late_targets.food_to_metals || Epoch(thePlayer) < kFarmEpoch)
        return;
    unsigned idx = thePlayer->mPlayerID & 15;
    long theTotal = 0;
    for (int r = 2; r < 7; r++)
        theTotal += (long)inResources->mWantedGatherers[r];
    if (theTotal < 6)
        return;

    static long sCap[16];
    static unsigned long sCapTime[16];
    if (inNow < sCapTime[idx])   // a new game
        sCap[idx] = 0;
    long theFields = 0;
    EECPCivilizationManager* theCivilization = thePlayer->mCivilizationManager;
    if (theCivilization != NULL && theCivilization->mUnitManager != NULL)
        theFields = (long)theCivilization->mUnitManager->GetCount(kUnitTypeFarm);
    long theReserve[7];
    MineReserve(thePlayer, inNow, theReserve);
    long theSpare = thePlayer->mResourceCaps[2] - theReserve[2];
    long theCap = sCap[idx] != 0 ? sCap[idx] : (long)inResources->mWantedGatherers[2];
    long theTop = theFields + 2 > 6 ? theFields + 2 : 6;
    if (inNow < sCapTime[idx] || inNow - sCapTime[idx] >= 10000) {
        sCapTime[idx] = inNow != 0 ? inNow : 1;
        theCap += theSpare > 4000 ? -2 : theSpare < 2000 ? 6 : 0;
    }
    if (theSpare < 500)
        theCap = theTop;
    long theLow = theFields / 2 > 6 ? theFields / 2 : 6;
    theCap = theCap < theLow ? theLow : theCap > theTop ? theTop : theCap;
    sCap[idx] = theCap;
    long theExtra = (long)inResources->mWantedGatherers[2] - theCap;
    if (theExtra <= 0)
        return;

    float theWeight[7] = {0};
    float theSum = 0;
    for (int r = 4; r < 7; r++) {
        long theGap = inResources->mNeeds[r] - thePlayer->mResourceCaps[r];
        theWeight[r] = (float)(theGap < 0 ? 0 : theGap > 3000 ? 3000 : theGap) + 100.0f;
        theSum += theWeight[r];
    }
    if (theSum <= 0)
        return;
    long theGiven = 0;
    for (int r = 4; r < 7; r++) {
        long theCount = (long)((float)theExtra * theWeight[r] / theSum);
        inResources->mWantedGatherers[r] += theCount;
        theGiven += theCount;
    }
    for (int r = 5; theGiven < theExtra; r = r == 6 ? 5 : r + 1) {
        inResources->mWantedGatherers[r]++;
        theGiven++;
    }
    inResources->mWantedGatherers[2] = (unsigned long)theCap;
}

// MORE GRANARIES (econ.c more_granaries, [ai_base] more_granaries): a granary feeds eight fields. The game's build
// list places one, but on the random maps the plan never adds another when the food target outgrows the fields. Once a
// minute, when no field waits to be built or worked and no granary is going up, ask for one if the food target is far
// enough above the food workers, or, with farm_first, while the first two are still missing. Upstream also tries its
// own granary spot 50 seconds after an unproductive request; that fallback needs the port's missing placement and
// raider searches, so it is left out and the ordinary request is repeated. The engine gate, the setting and the
// counters live here; EECPResourceManager::Update calls this after its gatherer targets are set.
static unsigned long sGranNext[16];
static GranaryCounts sGran[16];

void MoreGranaries(EECPResourceManager* inResources, unsigned long inNow)
{
    EEComputerPlayer* thePlayer = inResources->mManager != NULL ? inResources->mManager->mPlayer : NULL;
    if (thePlayer == NULL || EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsAIOf(thePlayer->mPlayerID, kDifficultyMedium);
    if (!theSettings.more_granaries || Epoch(thePlayer) < kFarmEpoch || gUnitClassTable[kUnitTypeGranary] == NULL)
        return;
    unsigned idx = thePlayer->mPlayerID & 15;
    if (inNow < sGranNext[idx] && sGranNext[idx] - inNow <= 60000)
        return;
    sGranNext[idx] = inNow + 60000;

    std::vector<EEComplexUnit*> theFarms;
    inResources->GetUnfinishedFarms(16, theFarms);
    if (!theFarms.empty())
        return;
    inResources->GetIdleFarms(16, theFarms);
    if (!theFarms.empty())
        return;

    GranaryCounts& theCounts = sGran[idx];
    theCounts.mChecks++;
    EECPCivilizationManager* theCivilization = thePlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mUnitManager == NULL || theCivilization->mBuildManager == NULL)
        return;

    long theGranaries = 0;
    bool theBuilding = false;
    std::deque<EEComplexUnit*>* theList = theCivilization->mUnitManager->GetUnits(kUnitTypeGranary);
    if (theList != NULL) {
        for (std::deque<EEComplexUnit*>::iterator it = theList->begin(); it != theList->end(); ++it) {
            EEComplexUnit* theGranary = *it;
            if (theGranary == NULL || theGranary->mHitPoints <= 0)
                continue;
            if (!theGranary->mIsBuilt) {
                theBuilding = true;
                break;
            }
            theGranaries++;
        }
    }
    if (theBuilding)
        return;

    long theWant = (long)inResources->mWantedGatherers[2];
    long theHave = (long)inResources->mGatherers[2].size();
    bool theFarmFirst = theSettings.farm_first && theGranaries < 2;
    if (!theFarmFirst && theWant < theHave + (theGranaries != 0 ? 6 : 2))
        return;
    long theCap = 1 + theWant / 8;
    if (theFarmFirst && theCap < 2)
        theCap = 2;
    if (theCap > 8)
        theCap = 8;
    if (theGranaries >= theCap || theCivilization->mBuildManager->IsQueued(kUnitTypeGranary))
        return;

    theCivilization->mBuildManager->QueueBuilding(kUnitTypeGranary, true, false);
    theCounts.mRequested++;
}

GranaryCounts GranaryCountsOf(unsigned long inPlayerID)
{
    return sGran[inPlayerID & 15];
}

// FISHING BOAT (navy.c hook_fish_type, [ai_base] fishing_boat): the game's pick by epoch alone reaches the Modern
// boat at the World War I age and then asks for a boat it cannot train, so fishing stops. Walk the class table for the
// newest boat the player can build now; an upgraded line's old class has no available tech and drops out. With no boat
// available, keep the game's pick. The caller gates the engine; this gate is the setting.
static FishingCounts sFishing[16];

long PickFishingBoat(EEPlayer* inPlayer, long inGameClass)
{
    FishingCounts& theCounts = sFishing[inPlayer->mPlayerID & 15];
    theCounts.mCalls++;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium);
    if (!theSettings.fishing_boat || inPlayer->mTechnologies == NULL) {
        theCounts.mLastClass = inGameClass;
        return inGameClass;
    }
    long theBest = 0, theBestTech = 0;
    for (long c = 1; c < 871; c++) {
        EEUnitClass* theClass = gUnitClassTable[c];
        if (theClass == NULL || !(Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kFishingBoat))
            continue;
        long theTech = theClass->mTechnology;
        if (theTech <= theBestTech)
            continue;
        EETechTreeNode* theNode = inPlayer->mTechnologies->Get(theTech);
        if (theNode == NULL || !theNode->GetIsAvailable())
            continue;
        theBestTech = theTech;
        theBest = c;
    }
    if (theBest == 0) {
        theCounts.mLastClass = inGameClass;
        return inGameClass;
    }
    if (theBest != inGameClass)
        theCounts.mChanged++;
    theCounts.mPicked++;
    theCounts.mLastClass = theBest;
    return theBest;
}

FishingCounts FishingCountsOf(unsigned long inPlayerID)
{
    return sFishing[inPlayerID & 15];
}

// PRODUCTION SYNC (econ.c prod_sync, [ai_base] production_sync, off in the shipped file): a loaded game keeps the
// buildings' queues but not the unit manager's in-production count, so the AI counts room it does not have and trains
// past its population cap. Every 10 s set that count to the units queued or in the making in its buildings.
void ProductionSync(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mUnitManager == NULL || inPlayer->mTechnologies == NULL)
        return;

    if (!SettingsOf(inPlayer).production_sync)
        return;
    static unsigned long sLast[16];
    if (sLast[idx] != 0 && inNow >= sLast[idx] && inNow - sLast[idx] < 10000)
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    long theQueued = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mComplexUnit == NULL)
            continue;
        EEComplexUnit* theBuilding = theUnit->mComplexUnit;
        for (int q = -1; q < (int)theBuilding->mBuildQueue.size(); q++) {
            long theTech = q < 0 ? theBuilding->mCurrentResearch : theBuilding->mBuildQueue[q].mTechnology;
            if (theTech <= 13 || theTech == 100000)
                continue;
            EETechTreeNode* theNode = inPlayer->mTechnologies->Get(theTech);
            if (theNode != NULL && theNode->mData != NULL && theNode->mData->mMakesUnit)
                theQueued++;
        }
    }
    long theWas = theCivilization->mUnitManager->mNumInProduction;
    if (theQueued == theWas)
        return;
    theCivilization->mUnitManager->mNumInProduction = theQueued;
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

// LINE UPGRADES IN USE (the army plan's used_lines bit, army.c ap_owned/ap_upg_line): a line upgrade waits until the
// player has or the game asks for a unit of the old or the new type. The unit types of the player's units (0..767) and
// the game's 14 request lists make up the set, refreshed at most every 5 s.
bool sOwnedLines[16][768];
unsigned long sOwnedLinesTime[16];

void OwnedLines(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    unsigned thePI = inPlayer->mPlayerID & 15;
    if (sOwnedLinesTime[thePI] != 0 && inNow >= sOwnedLinesTime[thePI] && inNow - sOwnedLinesTime[thePI] < 5000)
        return;
    sOwnedLinesTime[thePI] = inNow != 0 ? inNow : 1;
    memset(sOwnedLines[thePI], 0, sizeof(sOwnedLines[thePI]));
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
            continue;
        long theType = (long)theUnit->mUnitClass->mUnitType;
        if (theType >= 0 && theType < 768)
            sOwnedLines[thePI][theType] = true;
    }
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    EECPUnitManager* theUnits = theCivilization != NULL ? theCivilization->mUnitManager : NULL;
    if (theUnits == NULL)
        return;
    for (int e = 0; e < 14; e++) {
        const std::deque<EECPUMRequest>& theRequests = theUnits->mRequests[e];
        for (std::deque<EECPUMRequest>::const_iterator it = theRequests.begin(); it != theRequests.end(); ++it) {
            long theType = (long)it->mUnitClass;
            if (it->mCount > 0 && theType >= 0 && theType < 768)
                sOwnedLines[thePI][theType] = true;
        }
    }
}

// Whether a tech's line upgrade should wait: it has a kind-0x13 effect whose old or new type is neither owned nor asked
// for (ap_upgrade_waits in army.c). False for any other tech, so the gate leaves them alone.
bool LineUpgradeWaits(EEComputerPlayer* inPlayer, EETechTreeNode* inNode, unsigned long inNow)
{
    EEEvent* theEvent = EEEvent::Get(inNode->mData->mResearchedEvent);
    if (theEvent == NULL)
        return false;
    unsigned thePI = inPlayer->mPlayerID & 15;
    bool theLine = false, theUsed = false;
    for (unsigned long k = 0; k < theEvent->GetNumEffects() && k < 64; k++) {
        long theEffectID = theEvent->GetEffect(k);
        if (theEffectID < 0 || theEffectID >= 2000)
            continue;
        EEEventEffect* theEffect = EEEvent::GetEventEffect((unsigned long)theEffectID);
        if (theEffect == NULL || theEffect->mKind != 0x13)
            continue;
        if (!theLine) {
            theLine = true;
            OwnedLines(inPlayer, inNow);
        }
        long theOld = theEffect->mUnitClass, theNew = theEffect->mNewUnitClass;
        if ((theOld >= 0 && theOld < 768 && sOwnedLines[thePI][theOld])
            || (theNew >= 0 && theNew < 768 && sOwnedLines[thePI][theNew]))
            theUsed = true;
    }
    return theLine && !theUsed;
}

// PAID UPGRADES (epoch_upgrades in econ.c, [ai_base] paid_upgrades): the game researches the kind-7 techs of an epoch (the
// unit and building line upgrades) for a computer player on the spot, without cost, building or time. An Age of AI
// player schedules each with its research manager instead: it is researched at its building and paid like a human's.
// With the army plan's used_lines bit a line the player does not use yet waits for a later retry.
bool EpochUpgrades(EEComputerPlayer* inPlayer, EETechTreeEpoch* inEpoch)
{
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (!theSettings.paid_upgrades || theCivilization == NULL || theCivilization->mResearchManager == NULL)
        return false;
    const bool theWaitLines = theSettings.army_plan.used_lines != 0;
    const unsigned long theNow = gPhysics->mGameTime;
    for (std::map<long, EETechTreeNode*>::iterator theNode = inEpoch->mNodes.begin(); theNode != inEpoch->mNodes.end(); ++theNode) {
        EETechTreeNode* theTech = theNode->second;
        if (theTech->mData->mKind == 7 && theTech->mData->mResearchedWithEpoch && !theTech->mIsResearched) {
            if (theWaitLines && LineUpgradeWaits(inPlayer, theTech, theNow))
                continue;
            theCivilization->mResearchManager->Research(theTech->mData->mID);
        }
    }
    return true;
}

// LOST UPGRADES (upgrades_retry in econ.c, paid_upgrades): the research manager hands each scheduled tech to a building
// once and forgets it, and a building that leaves the map loses its queue. Every 30 s each available, unresearched line
// upgrade of the player's epochs or earlier that is in no research list and no building's queue or production is scheduled
// again, when the stock pays its price (a refused schedule adds the price to the resource demand). With the army plan's
// used_lines bit a line still out of use stays back. Not ported: the tech that the game marked as not queueable and the
// reserve of the price of four lines in use.
void UpgradesRetry(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (inNow < sLast[idx])
        sLast[idx] = 0;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (!theSettings.paid_upgrades || (sLast[idx] != 0 && inNow - sLast[idx] < 30000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    const bool theWaitLines = theSettings.army_plan.used_lines != 0;
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
        if (theWaitLines && LineUpgradeWaits(inPlayer, theTech, inNow))
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
// it waits for the humans' epoch (every pacing but classic does not), the epoch timer's range, the first attack's range,
// the attack interval, the attack group's size, the chances of the attacks and the citizens' share of the population cap.
// attention_activities is not ported: its field is not mapped in this tree.
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
    // The first attack comes from a range of minutes the game draws from these two fields while the player has not
    // started attacking (SetNextAttackTime). A timer the game already drew is drawn again inside the new range, with
    // upstream's spread by player index.
    long theFirstMin = theSettings.first_attack_min;
    long theFirstMax = theSettings.first_attack_max < theFirstMin ? theFirstMin : theSettings.first_attack_max;
    if (theFirstMin > 0) {
        inPlayer->mInitialAttackDelayMin = theFirstMin;
        inPlayer->mInitialAttackDelayMax = theFirstMax;
        if (!inPlayer->mIsStarted) {
            long theSpan = theFirstMax - theFirstMin + 1;
            inPlayer->mNextAttackTime = gPhysics->mGameTime
                + (unsigned long)((theFirstMin + (long)inPlayer->mPlayerID * 5 % theSpan) * 60000);
        }
    }
    if (theSettings.citizen_share > 0.0f && theSettings.citizen_share < 1.0f)
        inPlayer->mCitizenPercentage = theSettings.citizen_share;
}

// Upstream citizens.c cz_mine_cap, [ai_base] citizen_mine_cap on by default. The game asks for the population cap
// times the citizen share, so a player with few mines asks for more citizens than its mines can hold. From the Copper
// Age on, the cap refreshes every 30 game seconds: it counts the unique mine sites the player knows (stone, gold and
// iron), allows table 0x16 gatherers per mine times 1.5, and limits the result to half the population cap. The cap
// applies to the citizen count the unit manager asks for, not to the gatherer split. EESim reports the queries, the
// reductions, the last mine count and the last cap.
MineCapCounts sMineCap[16];

long MineCapWantedCitizens(EEComputerPlayer* inPlayer, long inCount)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return inCount;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium);
    if (!theSettings.citizen_mine_cap || Epoch(inPlayer) < kFarmEpoch)
        return inCount;
    unsigned thePI = inPlayer->mPlayerID & 15;
    MineCapCounts& theCounts = sMineCap[thePI];
    theCounts.mCalls++;
    unsigned long theNow = gPhysics != NULL ? gPhysics->mGameTime : 0;
    if (theCounts.mTime == 0 || theNow < theCounts.mTime || theNow - theCounts.mTime >= 30000) {
        theCounts.mTime = theNow ? theNow : 1;
        long theMines = 0;
        std::vector<EEUnit*> theSeen;
        for (unsigned long z = 0; z < inPlayer->GetNumResourceCenters() && z < 256; z++) {
            EECPResourceCenter* theZone = inPlayer->GetResourceCenter(z);
            if (theZone == NULL)
                continue;
            for (size_t k = 0; k < theZone->mUnits.size() && k < 4096 && theSeen.size() < 4096; k++) {
                EEUnit* theSite = theZone->mUnits[k];
                if (theSite == NULL || theSite->mHitPoints <= 0 || theSite->mResourceAmount <= 0)
                    continue;
                long theResource = (long)theSite->mResourceType;
                if (theResource < kResourceStone || theResource > kResourceIron)
                    continue;
                bool theKnown = false;
                for (size_t s = 0; s < theSeen.size(); s++)
                    if (theSeen[s] == theSite) { theKnown = true; break; }
                if (theKnown)
                    continue;
                theSeen.push_back(theSite);
                theMines++;
            }
        }
        long theCap = theMines * (long)gWorldTable.GetLongValue(0x16) * 3 / 2;
        long theHalf = inPlayer->GetPopulationLimit(0) / 2;
        if (theCap > theHalf)
            theCap = theHalf;
        theCounts.mMines = theMines;
        theCounts.mCap = theCap;
    }
    if (inCount > theCounts.mCap) {
        theCounts.mCapped++;
        return theCounts.mCap;
    }
    return inCount;
}

MineCapCounts MineCapCountsOf(unsigned long inPlayerID)
{
    return sMineCap[inPlayerID & 15];
}

// POPULATION CAP (econ.c hook_pop_cap, [ai_base] pop_follow_human): after the game works out an AI's cap from the
// human populations, an AI that does not follow the humans takes its own game limit instead. Called from
// EEComputerPlayer::WorkOutPopulation only while the game is following the humans; the other case already uses the
// limit. No cap is kept across games.
PopCapCounts sPopCap[16];

PopCapCounts PopCapCountsOf(unsigned long inPlayerID)
{
    return sPopCap[inPlayerID & 15];
}

void ApplyPopCap(EEComputerPlayer* inPlayer, long inGameLimit)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    PopCapCounts& theCounts = sPopCap[inPlayer->mPlayerID & 15];
    theCounts.mChecks++;
    const long theFollow = SettingsOf(inPlayer).pop_follow_human;
    const long theWeighted = inPlayer->mPopulationCap;
    const long theCap = PopulationRules::Cap(theWeighted, inGameLimit, theFollow);
    if (theFollow == 0) {
        inPlayer->mPopulationCap = theCap;
        theCounts.mOverrides++;
        theCounts.mWeighted = theWeighted;
        theCounts.mCap = theCap;
    }
}

}
