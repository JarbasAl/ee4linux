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
#include "AgeOfAIRoles.h"
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

// The military account and the income of a player since its first training request (g_bud in econ.c): the army spends
// its account, the citizens the rest of the stock above the reserve
struct Budget {
    unsigned long mLast;
    bool mInit;
    float mMil[7];
    long mIncome[7], mMilSpent[7];
} sBudget[16];
float sEpochAccount[16][7];   // copper_plan next_epoch: the next epoch's money, out of the citizens' reach
long sEpochAccountEpoch[16];
unsigned long sSaveSince[16];   // the game time a refused soldier began to wait for its price, 0: none
bool sRecovering[16], sHadEconomy[16];
bool sHomeGuardCall;   // set around the home guard's order: early_soldiers lets it train in the first epoch

// econ.c budget_reserve: what a unit order leaves of the stock for the next epoch. With the copper plan, before the Copper
// Age, its cost; else the next epoch's cost faded in over the reserve window before the planned attempt. The pacing
// follow (nothing held while no human is ahead) is not ported.
void BudgetReserveFull(EEComputerPlayer* inPlayer, long outReserve[7])
{
    for (int r = 0; r < 7; r++)
        outReserve[r] = 0;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (inPlayer->mTechnologies == NULL || theCivilization == NULL)
        return;
    long theNext = Epoch(inPlayer) + 1;
    if (theNext > 13 || theNext > inPlayer->mTechnologies->mEndEpoch || inPlayer->mTechnologies->Get(theNext) == NULL)
        return;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (theSettings.copper_plan != 0 && theNext <= kFarmEpoch) {
        BudgetReserve(inPlayer, theSettings.copper_plan, outReserve);
        return;
    }
    long theWindow = (long)theSettings.reserve_window_min * 60000;
    long theRemaining = (long)(theCivilization->mNextEpochAdvanceTime - gPhysics->mGameTime);
    float theFraction = theRemaining <= 0 ? 1.0f : theRemaining >= theWindow ? 0.0f : 1.0f - (float)theRemaining / (float)theWindow;
    for (int r = 2; r < 7; r++) {
        long theCost = EpochCost(inPlayer, theNext, r);
        outReserve[r] = theCost > 0 ? (long)((float)theCost * theFraction) : 0;
    }
}

// econ.c spend_reserve (reserve_plan, the default): from the Copper Age on, only what the income cannot bring in again
// before the attempt, 80% of it
void SpendReserve(EEComputerPlayer* inPlayer, long outReserve[7])
{
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (!theSettings.reserve_plan) {
        BudgetReserveFull(inPlayer, outReserve);
        return;
    }
    for (int r = 0; r < 7; r++)
        outReserve[r] = 0;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (inPlayer->mTechnologies == NULL || theCivilization == NULL)
        return;
    long theNext = Epoch(inPlayer) + 1;
    if (theNext > 13 || theNext > inPlayer->mTechnologies->mEndEpoch || inPlayer->mTechnologies->Get(theNext) == NULL)
        return;
    if (theSettings.copper_plan != 0 && theNext <= kFarmEpoch) {
        BudgetReserveFull(inPlayer, outReserve);
        return;
    }
    long theRemaining = (long)(theCivilization->mNextEpochAdvanceTime - gPhysics->mGameTime);
    if (theRemaining > 10 * 60000)
        return;
    float theUntil = theRemaining > 0 ? (float)theRemaining / 60000.0f : 0;
    for (int r = 2; r < 7; r++)
        outReserve[r] = EpochCost(inPlayer, theNext, r);
    const Rates& theRates = sRates[inPlayer->mPlayerID & 15];
    float theTime = 0;
    for (int r = 2; r < 7; r++) {
        long theMiss = outReserve[r] - inPlayer->mResourceCaps[r];
        if (theMiss <= 0)
            continue;
        float theNeed = theRates.mRate[r] > 1.0f ? (float)theMiss / theRates.mRate[r] : 10.0f;
        if (theNeed > theTime)
            theTime = theNeed;
    }
    if (theTime > 10)
        theTime = 10;
    float theHorizon = theUntil > theTime ? theUntil : theTime;
    for (int r = 2; r < 7; r++) {
        long theKeep = outReserve[r] - (long)(0.8f * theRates.mRate[r] * theHorizon);
        outReserve[r] = theKeep < 0 ? 0 : theKeep;
    }
}

long UnitCost(EEComputerPlayer* inPlayer, long inClass, int inResource)
{
    if (inClass < 0 || inClass >= 871 || gUnitClassTable[inClass] == NULL)
        return 0;
    EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[inClass]->mTechnology);
    return theNode != NULL ? inPlayer->ScaleCost(theNode->mData->mCosts[inResource]) : 0;
}

bool IsCitizenClass(long inClass)
{
    EEUnitClass* theClass = inClass >= 0 && inClass < 871 ? gUnitClassTable[inClass] : NULL;
    return theClass != NULL && (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kCitizen) != 0;
}

// the requests the budget may replace: every military unit but the ships, the siege weapons and rams, and the citizens
bool Substitutable(long inClass)
{
    EEUnitClass* theClass = inClass >= 0 && inClass < 871 ? gUnitClassTable[inClass] : NULL;
    if (theClass == NULL)
        return false;
    unsigned long theCategory = theClass->mCategory;
    if (theCategory == kUnitCategoryNaval || theCategory == kUnitCategorySubmergedNaval || theCategory == kUnitCategoryNavalTransport
        || theCategory == kUnitCategoryNavalBuilding || theCategory == kUnitCategoryNavalCitizen)
        return false;
    return (Roles::RolesOf(theCategory, theClass->mFamily, theClass->mRange) & (Roles::kSiege | Roles::kRam | Roles::kCitizen)) == 0;
}

bool LandSoldier(long inClass)
{
    EEUnitClass* theClass = inClass >= 0 && inClass < 871 ? gUnitClassTable[inClass] : NULL;
    if (theClass == NULL || theClass->mCategory != kUnitCategoryLand || inClass == kUnitTypePriest || inClass == kUnitTypeProphet)
        return false;
    return (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & (Roles::kSiege | Roles::kRam | Roles::kCitizen)) == 0;
}

long SoldierScore(long inClass)
{
    EEUnitClass* theClass = gUnitClassTable[inClass];
    return theClass->mHitPoints * theClass->mAttack;
}

// econ.c substitute_soldier: the strongest researched land soldier (hit points times attack) that the stock pays for and the
// game can train now; candidates refreshed every 30 s. With early_army the food-free ranged soldiers first.
long SubstituteSoldier(EEComputerPlayer* inPlayer, const long inVisible[7], long inWant, long& outCount, bool inSpareFood)
{
    static struct Candidates {
        EEPlayer* mPlayer;
        unsigned long mTime;
        int mCount;
        long mClass[48];
    } sCandidates[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    unsigned long theNow = gPhysics->mGameTime;
    Candidates& theCandidates = sCandidates[idx];
    if (theCandidates.mPlayer != inPlayer || theNow < theCandidates.mTime || theNow - theCandidates.mTime > 30000) {
        theCandidates.mPlayer = inPlayer;
        theCandidates.mTime = theNow;
        theCandidates.mCount = 0;
        for (long t = 0; t < 724; t++) {
            if (!LandSoldier(t) || SoldierScore(t) <= 0)
                continue;
            EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[t]->mTechnology);
            if (theNode == NULL || !theNode->mIsResearched)
                continue;
            int n = theCandidates.mCount, k = n < 48 ? n : 47;
            while (k > 0 && SoldierScore(theCandidates.mClass[k - 1]) < SoldierScore(t)) {
                if (k < 48)
                    theCandidates.mClass[k] = theCandidates.mClass[k - 1];
                k--;
            }
            if (k < 48)
                theCandidates.mClass[k] = t;
            if (n < 48)
                theCandidates.mCount++;
        }
    }
    long theBest = 0, theBestCount = 0;
    float theBestScore = 0;
    for (int i = 0; i < theCandidates.mCount; i++) {
        long theClass = theCandidates.mClass[i], theCount = inWant;
        bool thePaid = false;
        for (int r = 2; r < 7; r++) {
            long theCost = UnitCost(inPlayer, theClass, r);
            if (theCost > 0) {
                thePaid = true;
                if (inVisible[r] / theCost < theCount)
                    theCount = inVisible[r] / theCost;
            }
        }
        if (!thePaid || theCount <= 0 || !inPlayer->CanProduceUnitType(theClass, 0x20, NULL))
            continue;
        if (!inSpareFood) {
            outCount = theCount;
            return theClass;
        }
        float theScore = (float)SoldierScore(theClass) * (gUnitClassTable[theClass]->mRange > 2.0f ? 2.0f : 1.0f)
            * (UnitCost(inPlayer, theClass, 2) > 0 ? 0.25f : 1.0f);
        if (theScore > theBestScore) {
            theBestScore = theScore;
            theBest = theClass;
            theBestCount = theCount;
        }
    }
    if (theBest != 0)
        outCount = theBestCount;
    return theBest;
}

// econ.c recovering (recover_economy): after the Copper Age a workforce under min_workforce gives the citizens the whole stock
// and the army nothing, unless enemy soldiers are in the base (diplo.c is not ported: never)
bool Recovering(EEComputerPlayer* inPlayer, long inWorkforce)
{
    unsigned idx = inPlayer->mPlayerID & 15;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    if (!theSettings.recover_economy || Epoch(inPlayer) < kFarmEpoch)
        return false;
    bool theOn = sRecovering[idx] ? inWorkforce < theSettings.min_workforce + 5 : inWorkforce < theSettings.min_workforce;
    if (inWorkforce >= theSettings.min_workforce + 5)
        sHadEconomy[idx] = true;
    if (!sHadEconomy[idx] && gPhysics->mGameTime < 1800000)
        theOn = false;
    sRecovering[idx] = theOn;
    return theOn;
}

// a unit class whose first training counts for the next epoch (early_requirements)
bool RequirementUnit(EEComputerPlayer* inPlayer, long inClass)
{
    if (!SettingsOf(inPlayer).early_requirements || inClass < 0 || inClass >= 871 || gUnitClassTable[inClass] == NULL)
        return false;
    long theNext = Epoch(inPlayer) + 1;
    if (theNext > 13 || theNext > inPlayer->mTechnologies->mEndEpoch)
        return false;
    EETechTreeEpoch* theEpoch = static_cast<EETechTreeEpoch*>(inPlayer->mTechnologies->Get(theNext));
    if (theEpoch == NULL || theEpoch->mRequiredSOL - theEpoch->mCurrentSOL < 1)
        return false;
    EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[inClass]->mTechnology);
    return theNode != NULL && !theNode->mIsResearched && theNode->mData->mCountsTowardEpoch;
}

// the units a finished producer of the class takes before it holds depth units (the one in the making and its queue); -1:
// no producer known, so the game decides
long QueueRoom(EEComputerPlayer* inPlayer, EECPUnitManager* inUnits, long inClass, long inDepth)
{
    EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[inClass]->mTechnology);
    if (theNode == NULL)
        return -1;
    long theRoom = 0;
    bool theAny = false;
    const std::vector<unsigned long>& theMakers = theNode->mData->mUnitClasses;
    for (size_t m = 0; m < theMakers.size() && m < 16; m++) {
        if (m > 0 && theMakers[m - 1] == theMakers[m])
            continue;
        std::deque<EEComplexUnit*>* theUnits = inUnits->GetUnits((long)theMakers[m]);
        for (size_t j = 0; theUnits != NULL && j < theUnits->size() && j < 256; j++) {
            EEComplexUnit* theBuilding = (*theUnits)[j];
            if (theBuilding == NULL || !theBuilding->mIsBuilt)
                continue;
            long theCurrent = theBuilding->mCurrentResearch;
            long theQueued = (theCurrent > 13 && theCurrent != 100000 ? 1 : 0) + (long)theBuilding->mBuildQueue.size();
            theAny = true;
            if (theQueued < inDepth)
                theRoom += inDepth - theQueued;
        }
    }
    return theAny ? theRoom : -1;
}

}

// An income of the player's, from EEComputerPlayer::AddResource: counted, and from the army's share into its account
void OnIncome(EEComputerPlayer* inPlayer, long inResource, long inAmount)
{
    if (inAmount <= 0 || inResource < 2 || inResource >= 7)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    sIncome[idx][inResource] += inAmount;
    Budget& theBudget = sBudget[idx];
    if (!theBudget.mInit)
        return;
    theBudget.mIncome[inResource] += inAmount;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    long theEpoch = Epoch(inPlayer);
    float theShare = theEpoch >= kFarmEpoch ? theSettings.military_income_share_farms : theSettings.military_income_share;
    if (theSettings.early_army && theEpoch < kFarmEpoch) {
        if (theEpoch == 0 && !theSettings.early_soldiers)
            theShare = 0;
        else if (inResource == 2)
            theShare = theBudget.mMil[2] >= 300 ? 0 : theSettings.early_army_food_share;
    }
    theBudget.mMil[inResource] += (float)inAmount * theShare;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theSettings.copper_plan == 2 && theEpoch < kFarmEpoch && theCivilization != NULL && theCivilization->mResourceManager != NULL
        && Workforce(theCivilization->mResourceManager) >= theSettings.min_workforce) {
        if (sEpochAccountEpoch[idx] != theEpoch + 1) {
            memset(sEpochAccount[idx], 0, sizeof sEpochAccount[idx]);
            sEpochAccountEpoch[idx] = theEpoch + 1;
        }
        float theMissing = (float)EpochCost(inPlayer, theEpoch + 1, inResource) - sEpochAccount[idx][inResource];
        float theAdd = (float)inAmount * theSettings.epoch_share;
        if (theMissing > 0)
            sEpochAccount[idx][inResource] += theAdd < theMissing ? theAdd : theMissing;
    }
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

// THE TRAINING BUDGET (train_budget, train_call and hook_train_units in econ.c): the game's training of a request (the unit
// manager's StartProduction) goes through the budget first. A request the budget does not allow is refused and comes again;
// a soldier it refuses but the game can train is saved for, and one it cannot pay is replaced by the strongest soldier it
// can; a request is cut to what keeps each finished producer of the class at train_queue units.
// The reserve is the epoch's (spend_reserve): the other rows of priority.c hold nothing yet (their files are not ported:
// landing, outposts, fortresses, navy, the special orders, the fields' wood, the training reserve for forests, the upgrades).
// Not ported: the fleet's mix and accounts (navy.c), metal_plan's value choice and workforce_plan's thrift (metal.c,
// citizens.c), citizen_plan FIRST, the idle-citizen stop (idle_citizen_slack is off), the room for the mod's own orders (they
// are not ported), diplo.c's base_threatened.
bool TrainUnits(EECPUnitManager* inUnits, EECPUMRequest& inRequest, long& outMade, bool inModOrder)
{
    EEComputerPlayer* thePlayer = inUnits->mPlayer;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    unsigned idx = thePlayer->mPlayerID & 15;
    EECPCivilizationManager* theCivilization = thePlayer->mCivilizationManager;
    long theClass = (long)inRequest.mUnitClass;
    if (!theSettings.budget || theCivilization == NULL || theCivilization->mResourceManager == NULL || thePlayer->mTechnologies == NULL
        || theClass < 0 || theClass >= 871 || gUnitClassTable[theClass] == NULL)
        return inUnits->StartProductionGame(inRequest, outMade);
    long* theStock = thePlayer->mResourceCaps;
    long theBefore[7];
    for (int r = 0; r < 7; r++)
        theBefore[r] = theStock[r];
    unsigned long theNow = gPhysics->mGameTime;
    Budget& theBudget = sBudget[idx];
    if (theNow < theBudget.mLast)
        theBudget.mInit = false;
    theBudget.mLast = theNow;
    long theReserve[7];
    SpendReserve(thePlayer, theReserve);
    if (!theBudget.mInit) {
        memset(&theBudget, 0, sizeof theBudget);
        theBudget.mInit = true;
        theBudget.mLast = theNow;
        for (int r = 2; r < 7; r++)
            theBudget.mMil[r] = theStock[r] > theReserve[r] ? (float)(theStock[r] - theReserve[r]) * theSettings.military_start_share : 0;
    }
    bool theCitizen = IsCitizenClass(theClass);
    bool theMilitary = !theCitizen;
    bool theRequirement = theMilitary && RequirementUnit(thePlayer, theClass);
    long theEpoch = Epoch(thePlayer);
    long theWorkforce = Workforce(theCivilization->mResourceManager);
    int theCopper = theSettings.copper_plan;
    bool theEarly = theSettings.early_army && theEpoch < kFarmEpoch;
    bool theNoArmy = theEarly && theMilitary && !theRequirement && theEpoch == 0 && !(theSettings.early_soldiers && sHomeGuardCall);
    bool theRecover = Recovering(thePlayer, theWorkforce);
    long theVisible[7] = {0, 0, 0, 0, 0, 0, 0};
    for (int r = 2; r < 7; r++) {
        long theSeen = theRequirement ? theStock[r] : theStock[r] - theReserve[r];
        if (theCitizen && theCopper == 1 && theEpoch < kFarmEpoch) {
            if (r != 2 || theWorkforce < 10)
                theSeen = theStock[r];
            else if (theWorkforce < theSettings.min_workforce
                     && theStock[2] + (long)((float)WildFood(thePlayer) * theSettings.wild_food_share) >= theReserve[2] + 500)
                theSeen = theStock[2];
        }
        if (theCitizen && theCopper == 2 && theEpoch < kFarmEpoch) {
            long theAccount = sEpochAccountEpoch[idx] == theEpoch + 1 ? (long)sEpochAccount[idx][r] : 0;
            theSeen = theStock[r] - theAccount;
            if (r == 2 && (long)((float)WildFood(thePlayer) * theSettings.wild_food_share) < EpochCost(thePlayer, theEpoch + 1, 2) - theAccount + 500)
                theSeen = theStock[2] - EpochCost(thePlayer, theEpoch + 1, 2);
        }
        if (theCitizen && theEarly && theBudget.mMil[r] > 0)
            theSeen -= (long)theBudget.mMil[r];
        if (theMilitary && !theRequirement) {
            long theAllowed = (long)theBudget.mMil[r];
            long theExcess = theStock[r] - theReserve[r] - (r >= 4 ? theSettings.hoard_cap_mineral : theSettings.hoard_cap);
            if (theExcess > theAllowed)
                theAllowed = theExcess;
            if (theEarly)
                theSeen = theStock[r];
            if (theSeen > theAllowed)
                theSeen = theAllowed;
        }
        if (theRecover && theCitizen)
            theSeen = theStock[r];
        if (theRecover && theMilitary && !theRequirement)
            theSeen = 0;
        if (theNoArmy)
            theSeen = 0;
        if (theSeen < 0)
            theSeen = 0;
        if (theSeen > theStock[r])
            theSeen = theStock[r];
        theVisible[r] = theSeen;
    }
    long theWant = inRequest.mCount > 0 ? (long)inRequest.mCount : 1;
    long theAfford = theWant;
    for (int r = 2; r < 7; r++) {
        long theCost = UnitCost(thePlayer, theClass, r);
        if (theCost > 0 && theVisible[r] / theCost < theAfford)
            theAfford = theVisible[r] / theCost;
    }
    bool theBlocked = theAfford <= 0;
    long theSubstitute = 0;
    if (!theNoArmy) {
        bool theSubstitutable = theMilitary && theSettings.unit_substitution && Substitutable(theClass);
        bool theTrainable = theSubstitutable && thePlayer->CanProduceUnitType(theClass, 0x20, NULL);
        int theSub = theSubstitutable && (theBlocked || !theTrainable) ? 1 : 0;
        bool theSpareFood = theSettings.early_army && theEpoch >= 1 && (theEpoch < kFarmEpoch || theWorkforce < theSettings.early_army_workers)
            && theSubstitutable;
        if (theSpareFood && UnitCost(thePlayer, theClass, 2) > 0)
            theSub = 2;
        if (theSub == 1 && theTrainable) {   // the budget refused a unit it can train: save for it first
            if (!sSaveSince[idx] || theNow < sSaveSince[idx])
                sSaveSince[idx] = theNow ? theNow : 1;
            long theWait = theSettings.substitute_wait;
            theSub = !theWait || theNow - sSaveSince[idx] >= (unsigned long)theWait * 1000u;
        }
        if (theSub) {
            long theGot = 0;
            long theAlternative = SubstituteSoldier(thePlayer, theVisible, theWant, theGot, theSpareFood);
            if (theAlternative != 0) {
                theSubstitute = theAlternative;
                theAfford = theGot;
                theBlocked = false;
            }
        }
    }
    bool theResult = false;
    outMade = 0;
    if (!theBlocked) {
        EECPUMRequest theRequest = inRequest;
        if (theSubstitute != 0)
            theRequest.mUnitClass = (unsigned long)theSubstitute;
        if (theAfford > 0 && (theAfford < (long)theRequest.mCount || theSubstitute != 0))
            theRequest.mCount = (unsigned long)theAfford;
        if (inModOrder && (theSettings.train_room >> idx & 1)) {   // the mod's own order: no more than the room under the population cap
            long theRoomLeft = thePlayer->GetPopulationLimit(0) - inUnits->mNumInProduction - thePlayer->GetTotalPopulation(true, false);
            if (theRoomLeft <= 0)
                return false;
            if (theRoomLeft < (long)theRequest.mCount)
                theRequest.mCount = (unsigned long)theRoomLeft;
        }
        long theDepth = theCitizen ? 0 : theSettings.train_queue;
        long theRoom = theDepth > 0 ? QueueRoom(thePlayer, inUnits, (long)theRequest.mUnitClass, theDepth) : -1;
        long theAsked = theRequest.mCount > 0 ? (long)theRequest.mCount : 1;
        if (theRoom >= 0 && theRoom < theAsked) {
            if (theRoom > 0) {
                theRequest.mCount = (unsigned long)theRoom;
                theResult = inUnits->StartProductionGame(theRequest, outMade);
            }
        } else
            theResult = inUnits->StartProductionGame(theRequest, outMade);
    }
    bool theSpent = false;
    for (int r = 2; r < 7; r++) {
        long theCost = theBefore[r] - theStock[r];
        theSpent = theSpent || theCost > 0;
        if (theMilitary && theCost > 0) {
            theBudget.mMil[r] -= (float)theCost;
            theBudget.mMilSpent[r] += theCost;
        }
    }
    if (theMilitary && theSpent && theSubstitute == 0)
        sSaveSince[idx] = 0;
    return theResult;
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

// HOME GUARD (home_guard in defence.c, [ai_base] home_guard): the unit manager trains only what is ordered, mostly the next
// attack's group. Every 10 s, with fewer than max(6 + home_guard x epoch, home_guard_share x the military room) own soldiers
// within 25 tiles of the base, up to 5 more of the strongest land soldier the game can train go through the budget.
// The room is the population limit x (1 - citizen share). Not ported: the chat line and the fight estimate.
void HomeGuard(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    const SettingsAI& theSettings = SettingsOf(inPlayer);
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (!theSettings.home_guard || (sLast[idx] != 0 && inNow >= sLast[idx] && inNow - sLast[idx] < 10000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    if (theCivilization == NULL || theCivilization->mUnitManager == NULL || theCivilization->mBuildManager == NULL
        || inPlayer->mTechnologies == NULL)
        return;
    long theBaseX = theCivilization->mBuildManager->mTownCenterTile.mX, theBaseY = theCivilization->mBuildManager->mTownCenterTile.mY;
    if (theBaseX == -10000)
        return;
    long theEpoch = Epoch(inPlayer);
    long theTarget = 6 + (long)theSettings.home_guard * theEpoch;
    long theRoom = (long)((float)inPlayer->GetPopulationLimit(0) * (1.0f - inPlayer->mCitizenPercentage) * theSettings.home_guard_share);
    if (theRoom > theTarget)
        theTarget = theRoom;
    if (theSettings.early_army && theEpoch == 0) {
        theTarget = inNow >= 120000 ? theSettings.early_soldiers : 0;
        if (!theTarget)
            return;
    }
    long theHome = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL
            || !Roles::IsSoldier(theUnit->mUnitClass->mCategory, theUnit->mUnitClass->mFamily))
            continue;
        long theDX = theUnit->mTileX - theBaseX, theDY = theUnit->mTileY - theBaseY;
        theHome += theDX * theDX + theDY * theDY <= 625;
    }
    if (theHome >= theTarget)
        return;
    long theBig[7] = {0, 0, 1 << 20, 1 << 20, 1 << 20, 1 << 20, 1 << 20}, theGot = 0;
    long theType = SubstituteSoldier(inPlayer, theBig, 1, theGot, false);
    if (theType == 0)
        return;
    EECPUMRequest theRequest;
    theRequest.mUnitClass = (unsigned long)theType;
    theRequest.mCount = (unsigned long)(theTarget - theHome < 5 ? theTarget - theHome : 5);
    theRequest.mTownID = 0;
    theRequest.mContinent = 0;
    long theMade = 0;
    sHomeGuardCall = true;
    TrainUnits(theCivilization->mUnitManager, theRequest, theMade, true);
    sHomeGuardCall = false;
}

}
