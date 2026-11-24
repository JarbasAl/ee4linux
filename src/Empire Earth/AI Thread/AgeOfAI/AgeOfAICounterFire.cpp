// Counter-fire ([ai_base] counter_fire, upstream's default on, this port's off): enemy units that shoot at the player's units or buildings from
// beyond the victim's reach (further than its range + 1, with a range of 5 tiles at least) are shooters. Shooters within
// 6 tiles of each other are one target. Against them count the enemy fighters seen within 10 tiles. The player's
// soldiers within 24 tiles (on the shooter's land, able to hurt it, not inside anything, not busy with an enemy within 4
// tiles, not sent already) join nearest first, those already on a shooter before them, until the fight estimate says
// they win by 1.5, at most 60. Short of that nobody goes. Each gets an attack on one of the shooters and is committed for
// 45 seconds. EEComputerPlayer::Update calls it every tick; it acts every 2 game seconds.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/counterfire.c
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Upstream calls it at the top of counter_siege (defence.c); the
// skirmish spotter (skirmish.c) is not ported, so no soldier is held back as one. The chat log lines are not ported.
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
#include "ee/EECPBuildManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "AgeOfAI.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"
#include "ee/EEAIContinentManager.h"
#include "AgeOfAI/AgeOfAIStats.h"
#include "AgeOfAI/AgeOfAICounterFire.h"

extern EEToHitTable gToHitTable;           // 0x5318e8

namespace AgeOfAI {

namespace {

const unsigned long kEveryMs = 2000;
const unsigned long kHoldMs = 45000;
const long kCluster = 6;
const long kEscort = 10;
const long kFrom = 24;
const size_t kCommitted = 512;   // committed units per player
const size_t kShooters = 128;
const size_t kOwn = 2048;
const size_t kFoes = 4096;

struct Commitment {
    EEUnit* mUnit;
    unsigned long mID;
    unsigned long mUntil;
};
std::vector<Commitment> sCommitted[16];   // sorted by address
CounterFireCounts sCounts[16];
unsigned long sLast[16];

bool ByAddress(const Commitment& inA, const Commitment& inB)
{
    return std::less<EEUnit*>()(inA.mUnit, inB.mUnit);
}

long Dist2(EEUnit* inA, long inX, long inY)
{
    long theDX = inA->mTileX - inX, theDY = inA->mTileY - inY;
    return theDX * theDX + theDY * theDY;
}

EEUnit* AttackTarget(EEUnit* inUnit)
{
    EEUGAttackUnit* theGoal = inUnit->mComplexUnit != NULL ? dynamic_cast<EEUGAttackUnit*>(inUnit->mComplexUnit->mGoalState) : NULL;
    return theGoal != NULL ? theGoal->mTarget : NULL;
}

// area_id(p, 2) (forces.c): the land the tile is on, 0 none
unsigned short LandOf(long inX, long inY)
{
    return inX < 0 || inY < 0 ? 0 : gContinentManager.GetContinent(EETileCoordinate(inX, inY), 2);
}

bool IsSoldier(EEUnitClass* inClass)
{
    return Roles::IsSoldier(inClass->mCategory, inClass->mFamily, inClass->mRange);
}

// st_counts (stats.c): soldiers, citizens and buildings that shoot
bool Counts(EEUnitClass* inClass)
{
    if (Roles::RolesOf(inClass->mCategory, inClass->mFamily, inClass->mRange) & Roles::kCitizen)
        return true;
    if (IsSoldier(inClass))
        return true;
    return (inClass->mCategory == kUnitCategoryBuilding || inClass->mCategory == kUnitCategoryFortress) && inClass->mAttack > 0;
}

struct Scan {
    std::vector<EEUnit*> mShooters;   // enemy shooters out of their victims' reach
    std::vector<EEUnit*> mOwn;        // own soldiers
    std::vector<EEUnit*> mFoes;       // enemy fighters seen
};

void ScanUnit(EEPlayer* inPlayer, EEUnit* inUnit, Scan& ioScan)
{
    EEUnitClass* theClass = inUnit->mUnitClass;
    EEPlayer* theOwner = inUnit->mPlayer;
    if (theClass == NULL || theOwner == NULL || inUnit->mHitPoints <= 0 || inUnit->mTileX < 0)
        return;
    if (theOwner == inPlayer) {
        bool theInside = inUnit->mComplexUnit != NULL && inUnit->mComplexUnit->mContainer != NULL;
        if (IsSoldier(theClass) && !theInside && ioScan.mOwn.size() < kOwn)
            ioScan.mOwn.push_back(inUnit);
        return;
    }
    if (inPlayer->IsAlly(theOwner) || !Counts(theClass) || !SeenBy(inUnit, inPlayer))
        return;
    if (ioScan.mFoes.size() < kFoes)
        ioScan.mFoes.push_back(inUnit);
    EEUnit* theVictim = AttackTarget(inUnit);
    if (theVictim == NULL || theVictim->mPlayer != inPlayer || theVictim->mHitPoints <= 0 || ioScan.mShooters.size() >= kShooters)
        return;
    if (CounterFireRules::IsShooter(Stats::Range(inUnit), Stats::Range(theVictim), Dist2(inUnit, theVictim->mTileX, theVictim->mTileY)))
        ioScan.mShooters.push_back(inUnit);
}

Stats::Side sOwn, sFoe;

}

bool CounterFireUnit(EEPlayer* inPlayer, EEUnit* inUnit, unsigned long inNow)
{
    if (inPlayer == NULL || inUnit == NULL)
        return false;
    const std::vector<Commitment>& theList = sCommitted[inPlayer->mPlayerID & 15];
    Commitment theKey = {inUnit, 0, 0};
    std::vector<Commitment>::const_iterator theFound = std::lower_bound(theList.begin(), theList.end(), theKey, ByAddress);
    return theFound != theList.end() && theFound->mUnit == inUnit && theFound->mID == inUnit->mUnitID && inNow < theFound->mUntil
           && theFound->mUntil - inNow <= kHoldMs;
}

void CounterFire(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    unsigned idx = inPlayer->mPlayerID & 15;
    if (inNow < sLast[idx]) {                                  // a new game
        sCommitted[idx].clear();
        sCounts[idx] = CounterFireCounts();
    }
    if (!SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium).counter_fire ||   // upstream reads it from [ai_base]
        (inNow >= sLast[idx] && inNow - sLast[idx] < kEveryMs))
        return;
    sLast[idx] = inNow;
    std::vector<Commitment>& theList = sCommitted[idx];
    size_t theKeep = 0;                                        // the commitments still running
    for (size_t k = 0; k < theList.size(); k++)
        if (inNow < theList[k].mUntil && theList[k].mUntil - inNow <= kHoldMs)
            theList[theKeep++] = theList[k];
    theList.resize(theKeep);

    static Scan sScan;
    sScan.mShooters.clear();
    sScan.mOwn.clear();
    sScan.mFoes.clear();
    for (unsigned long p = 0; p < gWorld.GetNumPlayers() && p < 16; p++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(p);
        if (thePlayer == NULL)
            continue;
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext))
            ScanUnit(inPlayer, theUnit, sScan);
    }

    std::vector<Commitment> theNew;
    std::vector<bool> theDone(sScan.mShooters.size(), false);
    for (size_t i = 0; i < sScan.mShooters.size(); i++) {
        if (theDone[i])
            continue;
        EEUnit* theShooter = sScan.mShooters[i];
        long theX = theShooter->mTileX, theY = theShooter->mTileY;
        EEUnit* theCluster[16];
        int theClusterN = 0;
        for (size_t j = i; j < sScan.mShooters.size(); j++)
            if (!theDone[j] && Dist2(sScan.mShooters[j], theX, theY) <= kCluster * kCluster) {
                theDone[j] = true;
                if (theClusterN < 16)
                    theCluster[theClusterN++] = sScan.mShooters[j];
            }
        Stats::Clear(sFoe);
        Stats::Clear(sOwn);
        for (EEUnit* theFoe : sScan.mFoes)
            if (Dist2(theFoe, theX, theY) <= kEscort * kEscort)
                Stats::Add(sFoe, theFoe);
        unsigned short theLand = LandOf(theX, theY);
        unsigned long theShooterOwner = theShooter->mPlayer->GetPlayerID();
        // candidates nearest first; those already on a shooter of it count from the start
        struct Candidate { EEUnit* mUnit; long mD2; };
        std::vector<Candidate> theCandidates;
        int theAlready = 0;
        for (EEUnit*& theOwn : sScan.mOwn) {
            if (theOwn == NULL)
                continue;
            EEUnit* theTarget = AttackTarget(theOwn);
            bool theOn = false;
            for (int j = 0; j < theClusterN && theTarget != NULL && !theOn; j++)
                theOn = theTarget == theCluster[j];
            if (theOn) {
                Stats::Add(sOwn, theOwn);
                theAlready++;
                theOwn = NULL;
                continue;
            }
            long theD2 = Dist2(theOwn, theX, theY);
            if (theD2 > kFrom * kFrom || CounterFireUnit(inPlayer, theOwn, inNow))
                continue;
            if (theTarget != NULL && labs(theTarget->mTileX - theOwn->mTileX) <= 4 && labs(theTarget->mTileY - theOwn->mTileY) <= 4)
                continue;                                      // busy close by
            EEUnitClass* theClass = theOwn->mUnitClass;
            if (theClass == NULL || theClass->FindAction(kActionInitialAttackState) == NULL)
                continue;
            if (gToHitTable.GetAttackFactor(theClass->mAmmoType, theShooter->mUnitClass, inPlayer->mPlayerID, theShooterOwner) <= 0.0f)
                continue;
            if (theClass->mCategory != kUnitCategoryHelicopter && (theLand == 0 || LandOf(theOwn->mTileX, theOwn->mTileY) != theLand))
                continue;
            Candidate theCandidate = {theOwn, theD2};
            theCandidates.push_back(theCandidate);
        }
        for (size_t a = 1; a < theCandidates.size(); a++)   // insertion sort by distance, as upstream (stable)
            for (size_t b = a; b > 0 && theCandidates[b].mD2 < theCandidates[b - 1].mD2; b--)
                std::swap(theCandidates[b], theCandidates[b - 1]);
        int theAdded = 0;
        int theTake = CounterFireRules::Take((int)theCandidates.size(), theAlready, Stats::Ratio(sOwn, sFoe), [&](int inTaken) {
            for (; theAdded < inTaken; theAdded++)
                Stats::Add(sOwn, theCandidates[theAdded].mUnit);
            return Stats::Ratio(sOwn, sFoe);
        });
        if (theTake < 0) {
            sCounts[idx].mShort++;
            continue;
        }
        if (theTake == 0)
            continue;                                          // enough on it already
        int theSent = 0;
        for (int k = 0; k < theTake; k++) {
            EEUnit* theUnit = theCandidates[k].mUnit;
            if (!AttackUnit(theUnit, theCluster[k % theClusterN]))
                continue;
            theSent++;
            if (theList.size() + theNew.size() < kCommitted) {
                Commitment theCommitment = {theUnit, theUnit->mUnitID, inNow + kHoldMs};
                theNew.push_back(theCommitment);
            }
            for (EEUnit*& theOwn : sScan.mOwn)
                if (theOwn == theUnit) {
                    theOwn = NULL;
                    break;
                }
        }
        sCounts[idx].mSent += (unsigned long)theSent;
        sCounts[idx].mSorties++;
    }
    if (!theNew.empty()) {
        theList.insert(theList.end(), theNew.begin(), theNew.end());
        std::sort(theList.begin(), theList.end(), ByAddress);
    }
}

CounterFireCounts CounterFireCountsOf(unsigned long inPlayerID)
{
    return sCounts[inPlayerID & 15];
}

}
