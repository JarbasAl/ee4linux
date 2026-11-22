// FALL BACK, tower-cover bit ([ai_base] fall_back=tower_cover, shipped on). Grille's cover.c judges the fight at a
// land unit with enemies near; when the estimate is below fall_back_ratio the unit walks to the nearest own defence
// building no known enemy defence covers, then to a Hospital, then to the base, and rests there. This is the BREAK bit
// only: END (give an attack up) and SEA (fleets under towers or into a dock) stay unported, as upstream's END is off by
// default. The fight estimate and the remembered enemies are the ones already ported for the lost-fight rule.
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
#include "ee/EEUnitClassIDs.h"
#include "ee/EECPBuildManager.h"
#include "ee/EECPUnitManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGLoadGarrison.h"
#include "ee/EEResourceType.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEUnitBehavior.h"
#include "ee/EECPCombatManager.h"
#include "ee/EECPAttackCommander.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackSubGroup.h"
#include "AgeOfAI/AgeOfAIStats.h"
#include "AgeOfAI/AgeOfAIDefence.h"
#include "AgeOfAI/AgeOfAIRoles.h"
#include "AgeOfAI/AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAIFightMemoryRules.h"
#include "AgeOfAI/AgeOfAICoverRules.h"
#include "AgeOfAI/AgeOfAICoverGiveUpRules.h"

namespace AgeOfAI {

namespace {

const unsigned long kCoverEvery = 2000;
const int kMaxHold = 64;

struct CoverHold {
    EEUnit* mUnit;
    unsigned long mUnitID;
    long mX, mY;
    unsigned long mSince, mAt;
    bool mGaveUp;                 // cover.c kind 3: it broke off from one attack group too often
    EECPAttackGroup* mGroup;      // the group it gave up on; NULL unless mGaveUp
};
struct CoverState {
    unsigned long mLast;
    CoverHold mHold[kMaxHold];
    int mNHold;
};
struct FightCache {
    long mCX, mCY;
    float mRatio;
};
// cover.c struct cv_brk: one record per unit, counting break-offs from the attack group it is in.
const int kBreaksMost = 256;
struct BreakRecord {
    EEUnit* mUnit;
    EECPAttackGroup* mGroup;
    unsigned long mUnitID;
    int mCount;
    unsigned long mTime;
};
static CoverState sCover[16];
static CoverCounts sCoverCounts[16];
static BreakRecord sBreaks[16][kBreaksMost];

bool Held(EEUnit* inUnit, const CoverState& inState)
{
    for (int k = 0; k < inState.mNHold; k++)
        if (inState.mHold[k].mUnitID == inUnit->mUnitID)
            return true;
    return false;
}

bool Finished(EEUnit* inUnit)
{
    return inUnit->mComplexUnit != NULL && !inUnit->mComplexUnit->IsFoundation() && inUnit->mComplexUnit->mIsBuilt;
}

unsigned short Land(long inX, long inY)
{
    return inX < 0 || inY < 0 ? 0 : gContinentManager.GetContinent(EETileCoordinate(inX, inY), 2);
}

// cover.c cv_mil: soldiers and ships, including siege, but not citizens, transports or buildings. Roles::IsSoldier
// leaves siege out, so this follows the class and gun instead.
bool Fighter(EEUnit* inUnit)
{
    if (inUnit == NULL || inUnit->mHitPoints <= 0 || inUnit->mUnitClass == NULL
        || inUnit->mTileX < 0 || inUnit->mComplexUnit == NULL || inUnit->mUnitClass->mAttack <= 0.0f
        || inUnit->mUnitClass->mFamily == kUnitFamilyCitizen)
        return false;
    unsigned long theCategory = inUnit->mUnitClass->mCategory;
    return theCategory == kUnitCategoryLand || theCategory == kUnitCategoryLand8
           || theCategory == kUnitCategoryLand15 || theCategory == kUnitCategoryLandMilitaryHero
           || theCategory == kUnitCategoryNaval || theCategory == kUnitCategorySubmergedNaval;
}

bool FighterType(EEUnitClass* inClass)
{
    if (inClass == NULL || inClass->mAttack <= 0.0f || inClass->mFamily == kUnitFamilyCitizen)
        return false;
    unsigned long theCategory = inClass->mCategory;
    return theCategory == kUnitCategoryLand || theCategory == kUnitCategoryLand8
           || theCategory == kUnitCategoryLand15 || theCategory == kUnitCategoryLandMilitaryHero
           || theCategory == kUnitCategoryNaval || theCategory == kUnitCategorySubmergedNaval;
}

// cover.c defence_stats on a finished building with a ground gun
bool Defence(EEUnit* inUnit, float& outRange, float& outAttack)
{
    outRange = 0.0f;
    outAttack = 0.0f;
    if (inUnit == NULL || inUnit->mHitPoints <= 0 || inUnit->mUnitClass == NULL || !Finished(inUnit))
        return false;
    unsigned long theCategory = inUnit->mUnitClass->mCategory;
    if ((theCategory != kUnitCategoryBuilding && theCategory != kUnitCategoryFortress)
        || !DefenceStats(inUnit, outRange, outAttack) || outAttack <= 0.0f)
        return false;
    return true;
}

// cover.c cv_foe_near with seen = 1: a fighter or a defence the player sees in the unit's cell or one around it
bool EnemyNear(EEPlayer* inPlayer, long inX, long inY)
{
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* theOwner = gWorld.GetPlayer(i);
        if (theOwner == NULL || theOwner->mObserverMode || !IsEnemyOf(inPlayer, theOwner))
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (!SeenBy(theUnit, inPlayer)
                || !CoverRules::EnemyNear(inX, inY, theUnit->mTileX, theUnit->mTileY))
                continue;
            if (Fighter(theUnit))
                return true;
            float theRange = 0.0f, theAttack = 0.0f;
            if (Defence(theUnit, theRange, theAttack))
                return true;
        }
    }
    return false;
}

// cover.c cv_enemy_def: a known enemy defence whose reach covers a tile
bool EnemyDefenceCovers(EEPlayer* inPlayer, long inX, long inY)
{
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* theOwner = gWorld.GetPlayer(i);
        if (theOwner == NULL || theOwner->mObserverMode || !IsEnemyOf(inPlayer, theOwner))
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (!SeenBy(theUnit, inPlayer))
                continue;
            float theRange = 0.0f, theAttack = 0.0f;
            if (!Defence(theUnit, theRange, theAttack))
                continue;
            long theDX = theUnit->mTileX - inX, theDY = theUnit->mTileY - inY;
            if ((float)(theDX * theDX + theDY * theDY) <= (theRange + 1.0f) * (theRange + 1.0f))
                return true;
        }
    }
    return false;
}

// cover.c cv_local with honest = 1: the fighters within ten tiles, the defences whose reach covers the spot, and the
// remembered enemies of fight_memory. The local estimate is cached per four-tile cell as in cv_behave.
float LocalFight(EEPlayer* inPlayer, long inX, long inY, const Intel::Memory* inMemory,
                 unsigned long inNow, unsigned long inLast)
{
    Stats::Side theOwn, theFoe;
    Stats::Clear(theOwn);
    Stats::Clear(theFoe);
    const long kRadius2 = (long)CoverRules::kLocalRadius * (long)CoverRules::kLocalRadius;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* theOwner = gWorld.GetPlayer(i);
        if (theOwner == NULL || theOwner->mObserverMode)
            continue;
        bool theEnemy = IsEnemyOf(inPlayer, theOwner);
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theEnemy && !SeenBy(theUnit, inPlayer))
                continue;
            long theDX = theUnit->mTileX - inX, theDY = theUnit->mTileY - inY;
            float theRange = 0.0f, theAttack = 0.0f;
            if (Defence(theUnit, theRange, theAttack)) {
                if ((float)(theDX * theDX + theDY * theDY) > (theRange + 1.0f) * (theRange + 1.0f))
                    continue;
            } else if (!Fighter(theUnit) || theDX * theDX + theDY * theDY > kRadius2) {
                continue;
            }
            Stats::Add(theEnemy ? theFoe : theOwn, theUnit);
        }
    }
    if (inMemory != NULL) {
        for (size_t k = 0; k < inMemory->Records().size(); k++) {
            const Intel::Record& theRecord = inMemory->Records()[k];
            if (theRecord.mKind != Intel::kSoldier || theRecord.mX < 0 || theRecord.mOwner <= 0
                || theRecord.mOwner >= gWorld.GetNumPlayers()
                || !FightMemoryRules::Fresh(inNow, theRecord.mTime, inLast))
                continue;
            long theDX = theRecord.mX - inX, theDY = theRecord.mY - inY;
            if (theDX * theDX + theDY * theDY > kRadius2)
                continue;
            EEPlayer* theOwner = gWorld.GetPlayer(theRecord.mOwner);
            if (theOwner == NULL || theOwner->mObserverMode || !IsEnemyOf(inPlayer, theOwner)
                || theRecord.mType < 0 || theRecord.mType >= 871)
                continue;
            if (!FighterType(gUnitClassTable[theRecord.mType]))
                continue;
            Stats::AddRecord(theFoe, theRecord.mType, theRecord.mOwner, theRecord.mHitPoints,
                             theRecord.mX, theRecord.mY);
        }
    }
    return Stats::Ratio(theOwn, theFoe);
}

void MoveTo(EEUnit* inUnit, long inX, long inY)
{
    EEComplexUnit* theComplex = inUnit->mComplexUnit;
    if (theComplex == NULL)
        return;
    EEUGLocation* theGoal = new EEUGLocation(GE2DPoint((float)inX + 0.5f, (float)inY + 0.5f), true, true, false, 0.0f, false, 0);
    theComplex->SetGoal(theGoal, false, 0);
    theComplex->StartAction(kActionPrepareToMove);
    inUnit->mUnitClass->ActionChanged(theComplex);
}

// cover.c cv_group_of: the attack group a soldier is in, or NULL when it is in none. The engine's FindHolder searches
// the commander's current subgroups, which is the same membership the upstream hash table is rebuilt from each tick.
EECPAttackGroup* GroupOf(EEComputerPlayer* inPlayer, EEUnit* inUnit)
{
    EECPCombatManager* theCombat = inPlayer->mCombatManager;
    if (theCombat == NULL || theCombat->mAttackCommander == NULL)
        return NULL;
    EECPAttackSubGroup* theHolder = theCombat->mAttackCommander->FindHolder(inUnit);
    return theHolder != NULL ? theHolder->mGroup : NULL;
}

// cover.c cv_tally: count break-offs from one group. A different group or a record older than ten minutes starts
// again; a dead unit's id stops its old record from being reused. The oldest record is replaced when all are taken.
int TallyBreak(unsigned thePI, EEUnit* inUnit, EECPAttackGroup* inGroup, unsigned long inNow)
{
    BreakRecord* theRecords = sBreaks[thePI];
    int theOldest = 0;
    for (int k = 0; k < kBreaksMost; k++) {
        BreakRecord& theRecord = theRecords[k];
        if (theRecord.mUnit == inUnit && theRecord.mUnitID == inUnit->mUnitID) {
            if (theRecord.mGroup != inGroup || CoverGiveUpRules::ResetTally(inNow, theRecord.mTime)) {
                theRecord.mGroup = inGroup;
                theRecord.mCount = 0;
            }
            theRecord.mTime = inNow != 0 ? inNow : 1;
            return ++theRecord.mCount;
        }
        if (theRecord.mUnit == NULL) {
            if (theRecords[theOldest].mUnit != NULL)
                theOldest = k;
        } else if (theRecords[theOldest].mUnit != NULL && theRecord.mTime < theRecords[theOldest].mTime) {
            theOldest = k;
        }
    }
    BreakRecord& theRecord = theRecords[theOldest];
    theRecord.mUnit = inUnit;
    theRecord.mGroup = inGroup;
    theRecord.mUnitID = inUnit->mUnitID;
    theRecord.mCount = 1;
    theRecord.mTime = inNow != 0 ? inNow : 1;
    return 1;
}

}

void CoverTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium);
    if (!theSettings.fall_back.tower_cover)
        return;
    unsigned thePI = inPlayer->mPlayerID & 15;
    CoverState& theState = sCover[thePI];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
        std::memset(&sCoverCounts[thePI], 0, sizeof(sCoverCounts[thePI]));
        std::memset(&sBreaks[thePI], 0, sizeof(sBreaks[thePI]));
    }
    if (theState.mLast != 0 && inNow >= theState.mLast && inNow - theState.mLast < kCoverEvery)
        return;
    theState.mLast = inNow != 0 ? inNow : 1;
    CoverCounts& theCounts = sCoverCounts[thePI];
    theCounts.mChecks++;

    for (int k = theState.mNHold - 1; k >= 0; k--) {
        CoverHold& theHold = theState.mHold[k];
        EEUnit* theUnit = theHold.mUnit;
        bool theGo = theUnit == NULL || theUnit->mHitPoints <= 0 || theUnit->mUnitID != theHold.mUnitID
                     || theUnit->mPlayer != inPlayer;
        if (!theGo) {
            long theDX = theUnit->mTileX - theHold.mX, theDY = theUnit->mTileY - theHold.mY;
            if (theDX * theDX + theDY * theDY <= 9 && theHold.mAt == 0)
                theHold.mAt = inNow != 0 ? inNow : 1;
            if (theHold.mGaveUp) {
                if (CoverGiveUpRules::GiveUpDone(GroupOf(inPlayer, theUnit) == theHold.mGroup,
                                                 inNow, theHold.mSince)) {
                    theGo = true;
                    theCounts.mBack++;
                    theCounts.mGaveBack++;
                }
            } else if (CoverRules::HoldDone(theHold.mAt != 0, inNow, theHold.mAt, theHold.mSince)) {
                theGo = true;
                theCounts.mBack++;
            }
        }
        if (theGo) {
            theState.mHold[k] = theState.mHold[--theState.mNHold];
            continue;
        }
        EEUnitGoal* theGoal = theUnit->mComplexUnit != NULL ? theUnit->mComplexUnit->mGoalState : NULL;
        EEUGLocation* theLocation = dynamic_cast<EEUGLocation*>(theGoal);
        if (theLocation != NULL && std::fabs(theLocation->mLocation.GetX() - (float)theHold.mX) < 2.0f
            && std::fabs(theLocation->mLocation.GetY() - (float)theHold.mY) < 2.0f)
            continue;
        MoveTo(theUnit, theHold.mX, theHold.mY);
        theCounts.mHolds++;
    }

    long theDefX[64], theDefY[64], theHospX[16], theHospY[16];
    int theDefs = 0, theHosps = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mUnitClass == NULL || !Finished(theUnit))
            continue;
        if (theUnit->mUnitClass->GetUnitType() == kUnitTypeHospital && theHosps < 16) {
            theHospX[theHosps] = theUnit->mTileX;
            theHospY[theHosps] = theUnit->mTileY;
            theHosps++;
            continue;
        }
        float theRange = 0.0f, theAttack = 0.0f;
        if (Defence(theUnit, theRange, theAttack) && theDefs < 64) {
            theDefX[theDefs] = theUnit->mTileX;
            theDefY[theDefs] = theUnit->mTileY;
            theDefs++;
        }
    }
    EECPCivilizationManager* theCiv = inPlayer->mCivilizationManager;
    bool theBase = theCiv != NULL && theCiv->mBuildManager != NULL
                   && theCiv->mBuildManager->mTownCenterTile.mX != -10000;
    long theBaseX = theBase ? theCiv->mBuildManager->mTownCenterTile.mX : 0;
    long theBaseY = theBase ? theCiv->mBuildManager->mTownCenterTile.mY : 0;

    const Intel::Memory* theMemory = NULL;
    unsigned long theMemNow = 0, theMemLast = 0;
    if (theSettings.fight_memory && theSettings.honest_vision) {
        theMemory = &MemoryOf(thePI);
        theMemNow = theMemory->Now();
        theMemLast = theMemory->Last();
    }
    FightCache theCache[256];
    int theCached = 0;
    long theLimit = (long)theSettings.fall_back_dist * (long)theSettings.fall_back_dist;

    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (!Fighter(theUnit) || Held(theUnit, theState) || HospitalHeld(theUnit))
            continue;
        long theX = theUnit->mTileX, theY = theUnit->mTileY;
        if (!EnemyNear(inPlayer, theX, theY))
            continue;
        theCounts.mNear++;
        long theCX = theX / 4, theCY = theY / 4;
        int theHit = 0;
        while (theHit < theCached && (theCache[theHit].mCX != theCX || theCache[theHit].mCY != theCY))
            theHit++;
        if (theHit == theCached && theCached < (int)(sizeof(theCache) / sizeof(theCache[0]))) {
            theCache[theCached].mCX = theCX;
            theCache[theCached].mCY = theCY;
            theCache[theCached].mRatio = LocalFight(inPlayer, theX, theY, theMemory, theMemNow, theMemLast);
            theCached++;
            theCounts.mFights++;
        }
        float theEstimate = theHit < theCached ? theCache[theHit].mRatio : 1e6f;
        if (!CoverRules::LostFight(theEstimate, theSettings.fall_back_ratio))
            continue;
        unsigned short theLand = Land(theX, theY);
        if (theLand == 0)
            continue;
        int theBest = -1;
        long theBestD = theLimit + 1;
        for (int k = 0; k < theDefs; k++) {
            long theDX = theDefX[k] - theX, theDY = theDefY[k] - theY;
            long theD = theDX * theDX + theDY * theDY;
            if (theD < theBestD && Land(theDefX[k], theDefY[k]) == theLand
                && !EnemyDefenceCovers(inPlayer, theDefX[k], theDefY[k])) {
                theBestD = theD;
                theBest = k;
            }
        }
        long theTX = 0, theTY = 0;
        if (theBest >= 0) {
            CoverRules::CoverTarget(theX, theY, theDefX[theBest], theDefY[theBest], theBase, theBaseX, theBaseY,
                                    theTX, theTY);
        } else {
            theBestD = theLimit + 1;
            for (int k = 0; k < theHosps; k++) {
                long theDX = theHospX[k] - theX, theDY = theHospY[k] - theY;
                long theD = theDX * theDX + theDY * theDY;
                if (theD < theBestD && Land(theHospX[k], theHospY[k]) == theLand) {
                    theBestD = theD;
                    theBest = k;
                }
            }
            if (theBest >= 0) {
                theTX = theHospX[theBest];
                theTY = theHospY[theBest] + 2;
            } else if (theBase && Land(theBaseX, theBaseY) == theLand) {
                long theDX = theBaseX - theX, theDY = theBaseY - theY;
                if (theDX * theDX + theDY * theDY <= theLimit) {
                    theTX = theBaseX;
                    theTY = theBaseY;
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }
        if (CoverRules::AtCover(theTX - theX, theTY - theY) || theState.mNHold >= kMaxHold)
            continue;
        CoverHold& theHold = theState.mHold[theState.mNHold++];
        theHold.mUnit = theUnit;
        theHold.mUnitID = theUnit->mUnitID;
        theHold.mX = theTX;
        theHold.mY = theTY;
        theHold.mSince = inNow != 0 ? inNow : 1;
        theHold.mAt = 0;
        theHold.mGaveUp = false;
        theHold.mGroup = NULL;
        if (theSettings.fall_back.give_up) {
            EECPAttackGroup* theGroup = GroupOf(inPlayer, theUnit);
            if (theGroup != NULL
                && CoverGiveUpRules::GaveUp(TallyBreak(thePI, theUnit, theGroup, inNow),
                                            theSettings.fall_back_breaks)) {
                theHold.mGaveUp = true;
                theCounts.mGaveUp++;
            }
            theHold.mGroup = theGroup;
        }
        MoveTo(theUnit, theTX, theTY);
        theCounts.mSent++;
    }
}

bool CoverHeld(EEUnit* inUnit)
{
    if (inUnit == NULL)
        return false;
    for (unsigned thePI = 0; thePI < 16; thePI++)
        if (Held(inUnit, sCover[thePI]))
            return true;
    return false;
}

CoverCounts CoverCountsOf(unsigned long inPlayerID)
{
    return inPlayerID < 16 ? sCoverCounts[inPlayerID] : CoverCounts();
}

}  // namespace AgeOfAI
