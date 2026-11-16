// What is dangerous for an Age of AI player's citizens: the enemy defences it has seen, the raiders in reach, the places where its
// citizens were hurt, and so which sites its searches refuse.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/attack.c (threat_refresh,
// route_covered, raiders_scan, hurt_note, site_danger_res, the damage memory of citizens_safety) and dll/ai/cover.c (cv_mem_raider)
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Not ported yet: raid_held (defence.c), walled_out and ship_reach_exact (off by default),
// the danger map for routes, and the rest of citizens_safety (the citizens sent back, the strays, the flight, the shelter).
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
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEAIContinentManager.h"
#include "AgeOfAI/AgeOfAIStats.h"
#include "AgeOfAI.h"
#include "AgeOfAIDefence.h"
#include "AgeOfAIHome.h"
#include "AgeOfAITowers.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

const int kMaxDefences = 256;
const int kRaidMargin = 3;
const int kRaidMax = 15;
const int kRaidFar = 27;    // citizen_threats: artillery (12-14), mortars (10-11), ships (24) at their own reach
const int kAvoidMargin = 6; // tiles added to a tower's range
const int kHurtN = 64;
const unsigned long kHurtSeenMs = 20000;
const unsigned long kThreatMemory = 60000;   // threat_memory: a remembered soldier counts for the citizens a minute

struct Defence {
    long mX, mY;
    float mRange, mAttack;
};
struct Threats {
    Defence mDefences[kMaxDefences];
    int mCount;
    unsigned long mRefreshed;
} sThreat[16];

struct Hurt {
    unsigned mAt;
    struct { long mX, mY; unsigned long mTime; bool mSeen; } mSpots[kHurtN];
} sHurt[16];
struct CitizenHP {
    EEUnit* mUnit;
    long mHitPoints;
} sCitizenHP[16][512];

const SettingsAI& Cfg(unsigned long inPlayerID)
{
    return SettingsAIOf(inPlayerID, kDifficultyMedium);   // upstream reads these once, from [ai_base]
}

}

// A building with a ground attack (AA guns and missile towers only shoot at aircraft)
bool DefenceStats(EEUnit* inUnit, float& outRange, float& outAttack)
{
    EEUnitClass* theClass = inUnit->mUnitClass;
    if (theClass == NULL || (theClass->mCategory != kUnitCategoryBuilding && theClass->mCategory != kUnitCategoryFortress))
        return false;
    long theType = (long)theClass->mUnitType;
    if (theType == kUnitTypeAA10Gun88mm || theType == kUnitTypeAA13MissileTower)
        return false;
    float theRange = inUnit->mStats != NULL ? inUnit->mStats->mRange : theClass->mRange;
    float theAttack = inUnit->mStats != NULL ? inUnit->mStats->mAttack : (float)theClass->mAttack;
    if (!(theAttack > 0.0f) || !(theRange > 1.0f) || theRange > 30.0f)
        return false;
    outRange = theRange;
    outAttack = theAttack;
    return true;
}

bool IsEnemyOf(EEPlayer* inPlayer, EEPlayer* inOther)
{
    return inOther != NULL && !inPlayer->IsAlly(inOther);
}

namespace {

bool IsCitizen(EEUnit* inUnit)
{
    EEUnitClass* theClass = inUnit->mUnitClass;
    return theClass != NULL && (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kCitizen) != 0;
}

// The enemy defences this AI has seen (every 5 seconds): one per tile
void ThreatRefresh(EEPlayer* inPlayer, unsigned long inNow)
{
    unsigned thePI = inPlayer->mPlayerID & 15;
    Threats& theThreats = sThreat[thePI];
    if (theThreats.mRefreshed != 0 && inNow >= theThreats.mRefreshed && inNow - theThreats.mRefreshed < 5000)
        return;
    theThreats.mRefreshed = inNow ? inNow : 1;
    int theCount = 0;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers() && theCount < kMaxDefences; i++) {
        EEPlayer* theOwner = gWorld.GetPlayer(i);
        if (theOwner == NULL || !IsEnemyOf(inPlayer, theOwner))
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL && theCount < kMaxDefences; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            float theRange, theAttack;
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || !DefenceStats(theUnit, theRange, theAttack) || !SeenBy(theUnit, inPlayer))
                continue;
            int k;
            for (k = 0; k < theCount; k++)   // one defence per tile
                if (theThreats.mDefences[k].mX == theUnit->mTileX && theThreats.mDefences[k].mY == theUnit->mTileY)
                    break;
            if (k < theCount)
                continue;
            Defence& theDefence = theThreats.mDefences[theCount++];
            theDefence.mX = theUnit->mTileX;
            theDefence.mY = theUnit->mTileY;
            theDefence.mRange = theRange;
            theDefence.mAttack = theAttack;
        }
    }
    theThreats.mCount = theCount;
}

bool TowerCovers(unsigned inPlayer, float inX, float inY)
{
    const Threats& theThreats = sThreat[inPlayer & 15];
    for (int k = 0; k < theThreats.mCount; k++) {
        float theRange = theThreats.mDefences[k].mRange + 2.0f;
        float theDX = inX - (float)theThreats.mDefences[k].mX, theDY = inY - (float)theThreats.mDefences[k].mY;
        if (theDX * theDX + theDY * theDY <= theRange * theRange)
            return true;
    }
    return false;
}

// fx = -10000: the site only
bool RouteCovered(EEPlayer* inPlayer, long inFX, long inFY, long inTX, long inTY)
{
    unsigned thePI = inPlayer->mPlayerID & 15;
    ThreatRefresh(inPlayer, gPhysics->mGameTime);
    if (sThreat[thePI].mCount == 0)
        return false;
    if (TowerCovers(thePI, (float)inTX, (float)inTY))
        return true;
    if (inFX == -10000)
        return false;
    float theDX = (float)(inTX - inFX), theDY = (float)(inTY - inFY);
    int theN = (int)sqrtf(theDX * theDX + theDY * theDY);
    for (int t = 2; t < theN; t++)   // not the first tiles: the drop site itself may be in range (towers at the base)
        if (TowerCovers(thePI, (float)inFX + theDX * (float)t / (float)theN, (float)inFY + theDY * (float)t / (float)theN))
            return true;
    return false;
}

// a remembered enemy soldier whose range + RAID_MARGIN (at least r0, at most rmax) covers p (cover.c cv_mem_raider)
bool MemoryRaider(EEPlayer* inPlayer, long inX, long inY, int inR0, int inRMax)
{
    unsigned thePI = inPlayer->mPlayerID & 15;
    if (!Cfg(inPlayer->mPlayerID).honest_vision || !Cfg(inPlayer->mPlayerID).threat_memory)
        return false;
    const Intel::Memory& theMemory = MemoryOf(thePI);
    for (size_t k = 0; k < theMemory.Records().size(); k++) {
        const Intel::Record& theRecord = theMemory.Records()[k];
        if (theRecord.mKind != Intel::kSoldier || theRecord.mX < 0 || theRecord.mTime == theMemory.Last() || theMemory.Now() - theRecord.mTime > kThreatMemory
            || theRecord.mOwner <= 0)
            continue;
        if (labs(theRecord.mX - inX) > inRMax || labs(theRecord.mY - inY) > inRMax)
            continue;
        EEPlayer* theOwner = gWorld.GetPlayer((unsigned long)theRecord.mOwner);
        if (theOwner == NULL || !IsEnemyOf(inPlayer, theOwner) || theRecord.mType < 0 || theRecord.mType >= 871 || gUnitClassTable[theRecord.mType] == NULL
            || gUnitClassTable[theRecord.mType]->mAttack <= 0)
            continue;
        EEUnitClass* theClass = gUnitClassTable[theRecord.mType];
        EEUnitClassUpgrade* theStats = theOwner->mUnitClassUpgrades[theRecord.mType];
        float theRange = theStats != NULL ? theStats->mRange : theClass->mRange;
        int theReach = theRange > 0.0f && theRange < 30.0f ? (int)(theRange + 0.5f) + kRaidMargin : 0;
        if (theReach > inRMax)
            theReach = inRMax;
        if (theReach < inR0)
            theReach = inR0;
        if (labs(theRecord.mX - inX) <= theReach && labs(theRecord.mY - inY) <= theReach)
            return true;
    }
    return false;
}

}

// Enemy soldiers that this player sees within reach of a tile: each by its own reach (its range + RAID_MARGIN, at least r0, at
// most the box). The first raider found answers (attack.c raiders_scan without the kinds; walled_out and ship_reach_exact are off by default)
bool RaidersNear(EEPlayer* inPlayer, long inX, long inY, int inR0)
{
    int theMax = Cfg(inPlayer->mPlayerID).citizen_threats ? kRaidFar : kRaidMax;
    int theR = inR0 > theMax ? inR0 : theMax;   // the search box; each unit then by its own reach
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* theOwner = gWorld.GetPlayer(i);
        if (theOwner == NULL || !IsEnemyOf(inPlayer, theOwner))
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            EEUnitClass* theClass = theUnit->mUnitClass;
            if (theUnit->mHitPoints <= 0 || theClass == NULL || labs(theUnit->mTileX - inX) > theR || labs(theUnit->mTileY - inY) > theR)
                continue;
            unsigned long theCategory = theClass->mCategory;
            if ((theCategory != kUnitCategoryLand && theCategory != kUnitCategoryNaval && !(Cfg(inPlayer->mPlayerID).citizen_threats && theCategory == kUnitCategorySubmergedNaval))
                || IsCitizen(theUnit) || theClass->mAttack <= 0)
                continue;
            float theRange = theUnit->mStats != NULL ? theUnit->mStats->mRange : theClass->mRange;
            int theReach = theRange > 0.0f && theRange < 30.0f ? (int)(theRange + 0.5f) + kRaidMargin : 0;
            if (theReach > theMax)
                theReach = theMax;
            if (theReach < inR0)
                theReach = inR0;
            if (labs(theUnit->mTileX - inX) > theReach || labs(theUnit->mTileY - inY) > theReach)
                continue;
            if (SeenBy(theUnit, inPlayer))
                return true;
        }
    }
    return false;
}

// raiders_near or one it remembers there (threat_memory): where a citizen is sent
bool RaidersKnown(EEPlayer* inPlayer, long inX, long inY, int inR0)
{
    return RaidersNear(inPlayer, inX, inY, inR0) || MemoryRaider(inPlayer, inX, inY, inR0, Cfg(inPlayer->mPlayerID).citizen_threats ? kRaidFar : kRaidMax);
}

namespace {

bool HurtLive(unsigned inPI, const unsigned long inTime, bool inSeen, unsigned long inNow, unsigned long inMs)
{
    return inTime != 0 && inNow >= inTime && inNow - inTime <= (inSeen && Cfg(inPI).hurt_seen_short ? kHurtSeenMs : inMs);
}

// remembers a unit's hit points; a loss marks its spot (attack.c hurt_note)
void HurtNote(EEPlayer* inPlayer, unsigned inPI, EEUnit* inUnit, unsigned long inNow, bool inMark)
{
    long theHP = inUnit->mHitPoints;
    unsigned theHash = (unsigned)(((uintptr_t)inUnit >> 4) & 511);
    for (int k = 0; k < 512; k++) {
        CitizenHP& theEntry = sCitizenHP[inPI][(theHash + k) & 511];
        if (theEntry.mUnit == inUnit) {
            if (theHP < theEntry.mHitPoints && inMark) {
                Hurt& theHurt = sHurt[inPI];
                auto& theSpot = theHurt.mSpots[theHurt.mAt++ % kHurtN];
                theSpot.mX = inUnit->mTileX;
                theSpot.mY = inUnit->mTileY;
                theSpot.mTime = inNow ? inNow : 1;
                theSpot.mSeen = Cfg(inPlayer->mPlayerID).hurt_seen_short && RaidersNear(inPlayer, theSpot.mX, theSpot.mY, 6);
            }
            theEntry.mHitPoints = theHP;
            return;
        }
        if (theEntry.mUnit == NULL) {
            theEntry.mUnit = inUnit;
            theEntry.mHitPoints = theHP;
            return;
        }
    }
    memset(sCitizenHP[inPI], 0, sizeof sCitizenHP[inPI]);   // full (dead citizens' entries): start over
}

// citizens hurt within 6 tiles in the last 2 minutes
int HurtNear(unsigned inPI, long inX, long inY)
{
    unsigned long theNow = gPhysics->mGameTime;
    int theCount = 0;
    for (int k = 0; k < kHurtN; k++) {
        const auto& theSpot = sHurt[inPI].mSpots[k];
        if (HurtLive(inPI, theSpot.mTime, theSpot.mSeen, theNow, 120000) && labs(theSpot.mX - inX) <= 6 && labs(theSpot.mY - inY) <= 6)
            theCount++;
    }
    return theCount;
}

// res: 2 food, 3 wood, 4 stone, 5 gold, 6 iron, -1 unknown
bool HurtDanger(EEPlayer* inPlayer, long inX, long inY, int inResource)
{
    if (!Cfg(inPlayer->mPlayerID).citizen_threats)
        return false;
    return HurtNear(inPlayer->mPlayerID & 15, inX, inY) >= (inResource >= 4 ? 3 : 1);
}

// the nearest drop site of the player to a tile, from its own units: false when none
bool NearestDrop(EEPlayer* inPlayer, long inX, long inY, bool inFood, long& outX, long& outY)
{
    long theBest = 0x7FFFFFFF;
    bool theFound = false;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
            continue;
        long theType = (long)theUnit->mUnitClass->mUnitType;
        bool theGranary = theType == kUnitTypeGranary || theType == kUnitTypeGranaryImproved;
        if (!theGranary && theType != kUnitTypeTownCenter && theType != kUnitTypeCapitol && theType != kUnitTypeSettlement)
            continue;
        if (theGranary && !inFood)
            continue;
        long theDX = theUnit->mTileX - inX, theDY = theUnit->mTileY - inY, theD2 = theDX * theDX + theDY * theDY;
        if (theD2 < theBest) {
            theBest = theD2;
            outX = theUnit->mTileX;
            outY = theUnit->mTileY;
            theFound = true;
        }
    }
    return theFound;
}

}

// A site is dangerous when raiders or hurt citizens are near it, or the straight way from the nearest drop site to it crosses a known
// enemy defence's reach (the searches refuse such a site). raid_held (a town that holds against the raiders) is not ported yet.
bool SiteDanger(EEPlayer* inPlayer, EEUnit* inSite, bool inFood, int inResource)
{
    if (inPlayer == NULL || inSite == NULL || !Cfg(inPlayer->mPlayerID).avoid_towers_citizens)
        return false;
    long theX = inSite->mTileX, theY = inSite->mTileY;
    bool theRaided = RaidersNear(inPlayer, theX, theY, 6), theHurt = !theRaided && HurtDanger(inPlayer, theX, theY, inResource);
    if (theRaided || theHurt)
        return true;
    long theDX = -10000, theDY = 0;
    NearestDrop(inPlayer, theX, theY, inFood, theDX, theDY);
    return Cfg(inPlayer->mPlayerID).avoid_towers_citizens && RouteCovered(inPlayer, theDX, theDY, theX, theY);
}

namespace {

// An enemy building (not a wall) within r tiles of a tile: only goals at an enemy building are retargeted
bool BuildingAt(EEPlayer* inPlayer, long inX, long inY, int inR)
{
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* theOwner = gWorld.GetPlayer(i);
        if (theOwner == NULL || !IsEnemyOf(inPlayer, theOwner))
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mUnitClass == NULL || theUnit->mUnitClass->mCategory != kUnitCategoryBuilding
                || (Roles::RolesOf(theUnit->mUnitClass->mCategory, theUnit->mUnitClass->mFamily, theUnit->mUnitClass->mRange) & Roles::kWall))
                continue;
            long theDX = theUnit->mTileX - inX, theDY = theUnit->mTileY - inY;
            if (theDX * theDX + theDY * theDY <= (long)inR * inR)
                return true;
        }
    }
    return false;
}

void MoveTo(EEComplexUnit* inUnit, float inX, float inY)
{
    EEUGLocation* theGoal = new EEUGLocation(GE2DPoint(inX, inY), true, true, false, 0.0f, false, 0);
    inUnit->SetGoal(theGoal, false, 0);
    inUnit->StartAction(kActionPrepareToMove);
    inUnit->mUnitClass->ActionChanged(inUnit);
}

struct SafetyCounts {
    unsigned mSent, mHeld, mRefusedRepairs, mRefusedSites, mStrays;
} sSafety[16];

}

// every 4 seconds, before the citizen assignment (attack.c citizens_safety): the enemy defences seen; each citizen's hit points
// against the last look (the places where citizens were hurt); the strays of an attack sent home; and the gatherers and builders
// at a site near raiders or enemy towers, or where citizens were hurt, sent back to their drop site. Not hunters: the animals hurt
// them. Not ported yet: the flight of an overrun base (muster_rally of defence.c), the shelter in a fortress (bunker.c), raid_held.
void SafetyTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sNext[16];
    unsigned thePI = inPlayer->mPlayerID & 15;
    if (inNow < sNext[thePI] && sNext[thePI] - inNow <= 4000)
        return;
    sNext[thePI] = inNow + 4000;
    if (!Cfg(inPlayer->mPlayerID).avoid_towers_citizens)
        return;
    ThreatRefresh(inPlayer, inNow);
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mResourceManager == NULL)
        return;
    int theSent = 0, theStray = 0;
    for (int r = 0; r < 7; r++) {
        const std::deque<EEComplexUnit*> theList = theCivilization->mResourceManager->mGatherers[r];   // a copy: the orders below change the lists
        for (size_t k = 0; k < theList.size(); k++) {
            EEComplexUnit* theUnit = theList[k];
            if (theUnit == NULL || theUnit->mHitPoints <= 0)
                continue;
            EEUnitGoal* theGoal = theUnit->mGoalState;
            EEUGGather* theGather = dynamic_cast<EEUGGather*>(theGoal);
            EEUGBuildRepair* theBuild = dynamic_cast<EEUGBuildRepair*>(theGoal);
            if (Cfg(inPlayer->mPlayerID).citizen_threats) {   // not hunters: the animals hurt them (a farm is a building)
                EEUnit* theHuntSite = theGather != NULL ? theGather->mResource : NULL;
                HurtNote(inPlayer, thePI, theUnit, inNow, !(r == 2 && theHuntSite != NULL && theHuntSite->mUnitClass != NULL
                                                          && theHuntSite->mUnitClass->mCategory != kUnitCategoryBuilding));
            }
            EEUGLocation* theLocation = dynamic_cast<EEUGLocation*>(theGoal);
            if (r == 0 && theLocation != NULL) {   // a stray: an attack's leftover order, march to the target's Town Center
                long theDX = (long)theLocation->mLocation.GetX(), theDY = (long)theLocation->mLocation.GetY(), theNX, theNY;
                if (BuildingAt(inPlayer, theDX, theDY, 6)
                    && (!NearestDrop(inPlayer, theDX, theDY, false, theNX, theNY) || (theNX - theDX) * (theNX - theDX) + (theNY - theDY) * (theNY - theDY) > 225)
                    && NearestDrop(inPlayer, theUnit->mTileX, theUnit->mTileY, false, theNX, theNY)) {
                    MoveTo(theUnit, (float)theNX + 0.5f, (float)theNY + 0.5f);
                    theStray++;
                    continue;
                }
            }
            if (theGoal == NULL || (theBuild == NULL && theGather == NULL))
                continue;
            EEUnit* theSite = theBuild != NULL ? theBuild->mTarget : theGather->mResource;
            long theDropX, theDropY;
            if (theSite == NULL || !NearestDrop(inPlayer, theUnit->mTileX, theUnit->mTileY, r == 2, theDropX, theDropY))
                continue;
            bool theRaided = RaidersKnown(inPlayer, theSite->mTileX, theSite->mTileY, theBuild != NULL ? 8 : 6);
            // builders near known towers only: only of fields (walls and towers at the front: the wall manager sends them again and again)
            if (theBuild != NULL && !theRaided && theSite->mUnitClass->mUnitType != kUnitTypeFarm)
                continue;
            if (theBuild == NULL && !theRaided && HurtDanger(inPlayer, theSite->mTileX, theSite->mTileY, r))   // hurt there
                theRaided = true;
            if (!theRaided && (theBuild != NULL || !SiteDanger(inPlayer, theSite, r == 2, r)))
                continue;
            if (theRaided && labs(theDropX - theSite->mTileX) <= 3 && labs(theDropY - theSite->mTileY) <= 3)   // at home already
                continue;
            MoveTo(theUnit, (float)theDropX + 0.5f, (float)theDropY + 0.5f);
            theSent++;
        }
    }
    sSafety[thePI].mSent += (unsigned)theSent;
    sSafety[thePI].mStrays += (unsigned)theStray;
}

// REPAIRS UNDER ATTACK: the repair round sends a citizen to each damaged building, also to the wall or dock an army is hitting that
// moment. Its order is refused while enemy soldiers it knows stand within 8 tiles of the building (or their range + 3), and
// with repair_one_walker when another of its citizens already holds a build goal on the building (one at a time: a foundation
// far out drew a citizen every 31 s until one arrived). repair_safe_way (off by default) is not ported.
bool RefuseRepair(EEComputerPlayer* inPlayer, EEComplexUnit* inCitizen, EEUnit* inBuilding)
{
    if (EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI || inBuilding == NULL)
        return false;
    unsigned thePI = inPlayer->mPlayerID & 15;
    if (Cfg(inPlayer->mPlayerID).avoid_towers_citizens && RaidersKnown(inPlayer, inBuilding->mTileX, inBuilding->mTileY, 8)) {
        sSafety[thePI].mRefusedRepairs++;
        return true;
    }
    if (Cfg(inPlayer->mPlayerID).repair_one_walker)
        for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit == inCitizen || theUnit->mHitPoints <= 0 || theUnit->mComplexUnit == NULL)
                continue;
            EEUGBuildRepair* theGoal = dynamic_cast<EEUGBuildRepair*>(theUnit->mComplexUnit->mGoalState);
            if (theGoal != NULL && theGoal->mTarget == inBuilding)
                return true;
        }
    return false;
}

// IDLE CITIZENS HOME (citizens_home in econ.c, [ai_base] citizens_home): the game keeps one point per AI, the build manager's base, for
// its idle citizens, the overseas mine search and the ring of walls. Every 3 s: (1) a base at sea, or with none of the player's
// finished towns within 20 tiles, moves to the town with the most finished buildings within 20 tiles; (2) a citizen with no
// goal 15 or more tiles from its nearest finished town (idle on two passes in a row, between two jobs is not), or walking to
// within 3 tiles of the base while a town is 10 tiles nearer, is sent to the nearest finished town on its land with no enemy
// soldiers it sees within 12 tiles, none safe: it stays. Not ported: the exclusion of the citizens a landing, an outpost or a new
// town holds (landing.c, expand.c, towns.c), and (3) the removal of unfinished buildings stacked on one tile.
void CitizensHome(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    static std::vector<EEUnit*> sWasIdle[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (!Cfg(inPlayer->mPlayerID).citizens_home || (sLast[idx] != 0 && inNow >= sLast[idx] && inNow - sLast[idx] < 3000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL)
        return;
    EECPBuildManager* theBuild = theCivilization->mBuildManager;
    long theBaseX = theBuild->mTownCenterTile.mX, theBaseY = theBuild->mTownCenterTile.mY;
    if (theBaseX == -10000)
        return;
    struct Town {
        long mX, mY;
    };
    std::vector<Town> theTowns;
    bool theHome = false;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL && theTowns.size() < 64; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL)
            continue;
        long theType = (long)theUnit->mUnitClass->mUnitType;
        if ((theType != kUnitTypeTownCenter && theType != kUnitTypeCapitol && theType != kUnitTypeSettlement)
            || theUnit->mComplexUnit->IsFoundation() || !theUnit->mComplexUnit->mIsBuilt)
            continue;
        Town theTown = {theUnit->mTileX, theUnit->mTileY};
        theTowns.push_back(theTown);
        long theDX = theTown.mX - theBaseX, theDY = theTown.mY - theBaseY;
        if (theDX * theDX + theDY * theDY <= HomeRules::kTownReach * HomeRules::kTownReach)
            theHome = true;
    }
    if (theTowns.empty())
        return;
    bool theSea = theBaseX < 0 || theBaseY < 0
        || (gContinentManager.GetContinent(EETileCoordinate(theBaseX, theBaseY), 2) == 0
            && gContinentManager.GetContinent(EETileCoordinate(theBaseX, theBaseY), 1) != 0);
    if (!theHome || theSea) {
        std::vector<int> theCount(theTowns.size(), 0);
        for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL
                || !Roles::IsBuildingCategory(theUnit->mUnitClass->mCategory)
                || (Roles::RolesOf(theUnit->mUnitClass->mCategory, theUnit->mUnitClass->mFamily, theUnit->mUnitClass->mRange) & Roles::kWall)
                || theUnit->mComplexUnit->IsFoundation() || !theUnit->mComplexUnit->mIsBuilt)
                continue;
            for (size_t i = 0; i < theTowns.size(); i++) {
                long theDX = theTowns[i].mX - theUnit->mTileX, theDY = theTowns[i].mY - theUnit->mTileY;
                if (theDX * theDX + theDY * theDY <= HomeRules::kTownReach * HomeRules::kTownReach)
                    theCount[i]++;
            }
        }
        size_t theBest = 0;
        for (size_t i = 1; i < theTowns.size(); i++)
            if (theCount[i] > theCount[theBest])
                theBest = i;
        theBuild->mTownCenterTile.mX = theTowns[theBest].mX;
        theBuild->mTownCenterTile.mY = theTowns[theBest].mY;
        theBaseX = theTowns[theBest].mX;
        theBaseY = theTowns[theBest].mY;
    }
    std::vector<unsigned> theLand(theTowns.size());
    std::vector<int> theSafe(theTowns.size(), -1);
    for (size_t i = 0; i < theTowns.size(); i++)
        theLand[i] = gContinentManager.GetContinent(EETileCoordinate(theTowns[i].mX, theTowns[i].mY), 2);
    std::vector<EEUnit*> theNowIdle;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mTileX < 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL
            || !(Roles::RolesOf(theUnit->mUnitClass->mCategory, theUnit->mUnitClass->mFamily, theUnit->mUnitClass->mRange) & Roles::kCitizen))
            continue;
        EEUnitGoal* theGoal = theUnit->mComplexUnit->mGoalState;
        EEUGLocation* theLocation = dynamic_cast<EEUGLocation*>(theGoal);
        long theDestX = theUnit->mTileX, theDestY = theUnit->mTileY;
        int theWhy = 0;
        if (theGoal == NULL)
            theWhy = 1;
        else if (theLocation != NULL) {
            theDestX = (long)theLocation->mLocation.GetX();
            theDestY = (long)theLocation->mLocation.GetY();
            if (HomeRules::HeadingToBase(theDestX - theBaseX, theDestY - theBaseY))
                theWhy = 2;
        }
        if (theWhy == 0)
            continue;
        long theNearest = 0x7FFFFFFF;
        for (size_t i = 0; i < theTowns.size(); i++) {
            long theD = (theTowns[i].mX - theDestX) * (theTowns[i].mX - theDestX) + (theTowns[i].mY - theDestY) * (theTowns[i].mY - theDestY);
            if (theD < theNearest)
                theNearest = theD;
        }
        if (theWhy == 1 && !HomeRules::IdleFar(theNearest))
            continue;
        if (theWhy == 1) {
            if (theNowIdle.size() < 64)
                theNowIdle.push_back(theUnit);
            if (std::find(sWasIdle[idx].begin(), sWasIdle[idx].end(), theUnit) == sWasIdle[idx].end())
                continue;
        }
        unsigned theContinent = gContinentManager.GetContinent(EETileCoordinate(theUnit->mTileX, theUnit->mTileY), 2);
        long theTown = -1, theTownD = 0x7FFFFFFF;
        for (size_t i = 0; i < theTowns.size(); i++) {
            if (theLand[i] != theContinent)
                continue;
            if (theSafe[i] < 0)
                theSafe[i] = RaidersNear(inPlayer, theTowns[i].mX, theTowns[i].mY, 12) ? 0 : 1;
            long theD = (theTowns[i].mX - theUnit->mTileX) * (theTowns[i].mX - theUnit->mTileX)
                + (theTowns[i].mY - theUnit->mTileY) * (theTowns[i].mY - theUnit->mTileY);
            if (theSafe[i] && theD < theTownD) {
                theTownD = theD;
                theTown = (long)i;
            }
        }
        if (theTown < 0)
            continue;
        float theFromBase = sqrtf((float)((theUnit->mTileX - theBaseX) * (theUnit->mTileX - theBaseX)
                                          + (theUnit->mTileY - theBaseY) * (theUnit->mTileY - theBaseY)));
        if (theWhy == 2 && !HomeRules::TownCloser(sqrtf((float)theTownD), theFromBase))   // the base's town is as near
            continue;
        float theX, theY;
        HomeRules::Spot(theTowns[theTown].mX, theTowns[theTown].mY, theUnit->mTileX, theUnit->mTileY, theX, theY);
        MoveTo(theUnit->mComplexUnit, theX, theY);
    }
    sWasIdle[idx] = theNowIdle;
}

// MUSTER (muster in defence.c, [ai_base] muster): an enemy army in the base got the defenders one by one. Every 3 s: enemy
// soldiers within 22 tiles of the base (strength = hit points x attack) against the own soldiers within 35 and the own
// towers within 22. Only when the fight at home is hopeless - at least 8 enemies and the home side under half of their
// strength - every own soldier there goes to a rally point 18 tiles beyond the base, away from the enemy's centre; new
// ones join as they are trained. Once the soldiers at the rally point are 1.3 times the enemy in the base, each attacks the
// nearest enemy soldier. The muster ends when the enemy is gone (fewer than 3), when soldiers and towers can stand up to
// it again, or when the enemy follows to the rally point a third time (then it fights where it is); a new muster waits
// 30 s or 60 s. Not ported: the fight estimate (stats.c, off by default), the chat line, the exceptions of unit_goal for
// shore guns and garrisons (bunker.c, towns.c), and the citizens' flight to the rally point (SafetyTick).
namespace {

struct Muster {
    bool mOn;
    int mMoves;
    float mRX, mRY;
    unsigned long mSince, mCool, mCoolLength;
} sMuster[16];

float UnitPower(EEUnit* inUnit)
{
    float theAttack = inUnit->mStats != NULL ? inUnit->mStats->mAttack : (float)inUnit->mUnitClass->mAttack;
    return (float)inUnit->mHitPoints * (theAttack > 0 ? theAttack : 0);
}

}

bool AttackUnit(EEUnit* inUnit, EEUnit* inTarget)
{
    if (inUnit->mHitPoints <= 0 || inUnit->mTileX == -10000 || inUnit->mComplexUnit == NULL || inUnit->mComplexUnit->mIsDisabled)
        return false;
    EEUGAttackUnit* theGoal = new EEUGAttackUnit(inUnit->mComplexUnit, inTarget, true, false);
    inUnit->mComplexUnit->SetGoal(theGoal, false, 0);
    inUnit->mComplexUnit->StartAction(kActionPrepareToMove);
    inUnit->mUnitClass->ActionChanged(inUnit->mComplexUnit);
    return true;
}

void MusterTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (!Cfg(inPlayer->mPlayerID).muster || (sLast[idx] != 0 && inNow >= sLast[idx] && inNow - sLast[idx] < 3000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    Muster& theMuster = sMuster[idx];
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL || theCivilization->mBuildManager->mTownCenterTile.mX == -10000) {
        theMuster.mOn = false;
        return;
    }
    long theBaseX = theCivilization->mBuildManager->mTownCenterTile.mX, theBaseY = theCivilization->mBuildManager->mTownCenterTile.mY;
    std::vector<EEUnit*> theOwn, theEnemy;
    float theEnemyPower = 0, theOwnPower = 0, theTowerPower = 0, theEX = 0, theEY = 0;
    int theEnemies = 0;
    for (unsigned long p = 0; p < gWorld.mNumPlayers && p < 16; p++) {
        EEPlayer* thePlayer = gWorld.mPlayers[p];
        if (thePlayer == NULL)
            continue;
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
                continue;
            long theDX = theUnit->mTileX - theBaseX, theDY = theUnit->mTileY - theBaseY, theD2 = theDX * theDX + theDY * theDY;
            float theRange, theAttack;
            if (thePlayer == inPlayer && theD2 <= 22 * 22 && DefenceStats(theUnit, theRange, theAttack)) {
                theTowerPower += (float)theUnit->mHitPoints * theAttack;
                continue;
            }
            if (!Roles::IsSoldier(theUnit->mUnitClass->mCategory, theUnit->mUnitClass->mFamily, theUnit->mUnitClass->mRange))
                continue;
            if (thePlayer == inPlayer) {
                if (theD2 > 35 * 35)
                    continue;
                if (theOwn.size() < 256)
                    theOwn.push_back(theUnit);
                theOwnPower += UnitPower(theUnit);
            } else if (thePlayer->mPlayerID > 0 && IsEnemyOf(inPlayer, thePlayer) && theD2 <= 22 * 22) {
                float thePower = UnitPower(theUnit);
                theEnemyPower += thePower;
                theEX += (float)theUnit->mTileX * thePower;
                theEY += (float)theUnit->mTileY * thePower;
                theEnemies++;
                if (theEnemy.size() < 128)
                    theEnemy.push_back(theUnit);
            }
        }
    }
    float theStand = theEnemyPower > 0 ? (theOwnPower + theTowerPower) / theEnemyPower : 1e6f;
    struct Local {
        static void Rally(Muster& ioMuster, long inBX, long inBY, float inEnemyPower, float inEX, float inEY)
        {
            float theCX = inEnemyPower > 0 ? inEX / inEnemyPower : (float)inBX + 1, theCY = inEnemyPower > 0 ? inEY / inEnemyPower : (float)inBY;
            float theDX = (float)inBX - theCX, theDY = (float)inBY - theCY, theLength = sqrtf(theDX * theDX + theDY * theDY);
            if (theLength < 1.0f) {
                theDX = 1.0f;
                theDY = 0.0f;
                theLength = 1.0f;
            }
            float theSize = gWorld.mMap != NULL ? (float)gWorld.mMap->mSize : 0;
            float theRX = (float)inBX + theDX / theLength * 18.0f, theRY = (float)inBY + theDY / theLength * 18.0f;
            if (theSize > 8) {
                theRX = theRX < 3 ? 3 : theRX > theSize - 4 ? theSize - 4 : theRX;
                theRY = theRY < 3 ? 3 : theRY > theSize - 4 ? theSize - 4 : theRY;
            }
            ioMuster.mRX = theRX;
            ioMuster.mRY = theRY;
        }
        static EEUnit* Nearest(const std::vector<EEUnit*>& inEnemy, EEUnit* inUnit)
        {
            EEUnit* theBest = NULL;
            long theBestD = 0x7FFFFFFF;
            for (size_t k = 0; k < inEnemy.size(); k++) {
                if (inEnemy[k]->mHitPoints <= 0)
                    continue;
                long theDX = inEnemy[k]->mTileX - inUnit->mTileX, theDY = inEnemy[k]->mTileY - inUnit->mTileY, theD = theDX * theDX + theDY * theDY;
                if (theD < theBestD) {
                    theBestD = theD;
                    theBest = inEnemy[k];
                }
            }
            return theBest;
        }
    };
    if (!theMuster.mOn) {
        if (theEnemies < 8 || theStand >= 0.5f || (theMuster.mCool != 0 && inNow - theMuster.mCool < theMuster.mCoolLength))
            return;
        theMuster.mOn = true;
        theMuster.mMoves = 0;
        theMuster.mSince = inNow != 0 ? inNow : 1;
        Local::Rally(theMuster, theBaseX, theBaseY, theEnemyPower, theEX, theEY);
    } else {
        if (theEnemies < 3 || theStand >= 1.0f) {
            theMuster.mOn = false;
            theMuster.mCool = inNow != 0 ? inNow : 1;
            theMuster.mCoolLength = 30000;
            return;
        }
        float theRallyPower = 0;
        for (size_t k = 0; k < theOwn.size(); k++) {
            float theDX = (float)theOwn[k]->mTileX - theMuster.mRX, theDY = (float)theOwn[k]->mTileY - theMuster.mRY;
            if (theDX * theDX + theDY * theDY <= 100)
                theRallyPower += UnitPower(theOwn[k]);
        }
        float theGathered = theEnemyPower > 0 ? theRallyPower / theEnemyPower : 1e6f;
        if (theGathered >= 1.3f) {   // strike
            for (size_t k = 0; k < theOwn.size(); k++) {
                EEUnit* theTarget = Local::Nearest(theEnemy, theOwn[k]);
                if (theTarget != NULL)
                    AttackUnit(theOwn[k], theTarget);
            }
            theMuster.mOn = false;
            theMuster.mCool = inNow != 0 ? inNow : 1;
            theMuster.mCoolLength = 30000;
            return;
        }
        bool theNearRally = false;
        for (size_t k = 0; k < theEnemy.size() && !theNearRally; k++) {
            float theDX = (float)theEnemy[k]->mTileX - theMuster.mRX, theDY = (float)theEnemy[k]->mTileY - theMuster.mRY;
            theNearRally = theDX * theDX + theDY * theDY <= 100;
        }
        if (theNearRally && ++theMuster.mMoves >= 3) {   // it follows: no getting away - fight where it is
            theMuster.mOn = false;
            theMuster.mCool = inNow != 0 ? inNow : 1;
            theMuster.mCoolLength = 60000;
            for (size_t k = 0; k < theOwn.size(); k++) {
                EEUnit* theTarget = Local::Nearest(theEnemy, theOwn[k]);
                if (theTarget != NULL)
                    AttackUnit(theOwn[k], theTarget);
            }
            return;
        }
        if (theNearRally)
            Local::Rally(theMuster, theBaseX, theBaseY, theEnemyPower, theEX, theEY);
    }
    for (size_t k = 0; k < theOwn.size(); k++) {   // everyone who isn't there or on the way
        EEUnit* theUnit = theOwn[k];
        float theDX = (float)theUnit->mTileX - theMuster.mRX, theDY = (float)theUnit->mTileY - theMuster.mRY;
        if (theDX * theDX + theDY * theDY <= 36 || theUnit->mComplexUnit == NULL)
            continue;
        EEUGLocation* theLocation = dynamic_cast<EEUGLocation*>(theUnit->mComplexUnit->mGoalState);
        if (theLocation != NULL && fabsf(theLocation->mLocation.GetX() - theMuster.mRX) < 2 && fabsf(theLocation->mLocation.GetY() - theMuster.mRY) < 2)
            continue;
        if (theUnit->mHitPoints > 0 && theUnit->mTileX != -10000)
            MoveTo(theUnit->mComplexUnit, theMuster.mRX, theMuster.mRY);
    }
}


// COUNTER-SIEGE and CITIZEN MILITIA (counter_siege, militia and militia_answer in defence.c; [ai_base] counter_siege,
// citizen_militia, militia_estimate, militia_caught_share, early_militia). Every 2 s per AI, one scan of the map:
// - the citizen militia: enemy citizens building a defence within 12 tiles of the AI's buildings or citizens (any epoch),
//   and up to the Bronze Age enemy units attacking one of the AI's units within 10 tiles of its citizens, are attacked by
//   the AI's gathering or idle citizens within 10 tiles - at most 3 per enemy and against at most 6 enemies, and only when
//   the citizens' strength (hit points x attack) is 3 times that of the enemies within 6 tiles. With early_militia (on by
//   default) the citizens fight soldiers only next to their own soldiers or under their own towers, and only where the
//   fight estimate (AgeOfAIStats.h) has them win without the loss of a citizen;
// - counter-siege: an enemy siege weapon or ram the AI sees within its range + 4 tiles of one of its buildings, or an
//   enemy defence going up within 12 tiles of its buildings or citizens, is attacked by up to 5 of its soldiers within
//   20 tiles (nearest first; idle, marching or fighting something more than 4 tiles away) - the builders first, spread
//   over them - but only when those soldiers are at least 2 x + 3 the enemy soldiers within 8 tiles of it. Too few
//   soldiers against a defence going up: the citizens go at the builders instead.
// Not ported: counter_fire (counterfire.c, its own file), the chat lines, and the control pipe's defence switch.
namespace {

struct SiegeScan {
    int mBuildings;
    long mBX[256], mBY[256];
    int mSoldiers;
    EEUnit* mSoldier[128];
    int mSiege;
    EEUnit* mEnemySiege[16];
    int mEnemySoldiers;
    long mEX[256], mEY[256];
    int mCitizens;
    long mCX[128], mCY[128];
    EEUnit* mCitizen[128];
    int mEnemyUnits;
    EEUnit* mEnemyUnit[128];
    int mTowers;
    EEUnit* mEnemyTower[16];
};

DefenceCounts sDefence[16];

// is_enemy (players.c): the diplomatic stance towards the other player
bool IsEnemy(EEPlayer* inPlayer, EEPlayer* inOther)
{
    return inOther != NULL && inOther != inPlayer && inOther->mPlayerID < 16 && inPlayer->mEnemies[inOther->mPlayerID] != 0;
}

// is_soldier (roles.c): an army class that is no siege weapon, ram or citizen
bool IsSoldierUnit(EEUnit* inUnit)
{
    EEUnitClass* theClass = inUnit->mUnitClass;
    return Roles::IsSoldier(theClass->mCategory, theClass->mFamily, theClass->mRange);
}

// early_finished (early.c): the building is built
bool IsBuilt(EEUnit* inUnit)
{
    return inUnit->mComplexUnit != NULL && inUnit->mComplexUnit->mIsBuilt;
}

long CurrentEpoch(EEPlayer* inPlayer)
{
    return inPlayer->mTechnologies != NULL ? inPlayer->mTechnologies->mCurrentEpoch : -1;
}

bool IsBuilding(EEUnit* inUnit)
{
    return inUnit->mUnitClass->mCategory == kUnitCategoryBuilding || inUnit->mUnitClass->mCategory == kUnitCategoryFortress;
}

EEUnitGoal* GoalOf(EEUnit* inUnit)
{
    return inUnit->mComplexUnit != NULL ? inUnit->mComplexUnit->mGoalState : NULL;
}

EEUnit* AttackTarget(EEUnitGoal* inGoal)
{
    EEUGAttackUnit* theAttack = dynamic_cast<EEUGAttackUnit*>(inGoal);
    return theAttack != NULL ? theAttack->mTarget : NULL;
}

// field_site (econ.c): the outermost unit of a site's chain of parts, what a build goal holds
EEUnit* FieldSite(EEUnit* inSite)
{
    EEUnit* theLast = inSite;
    for (EEComplexUnit* thePart = inSite->mComplexUnit; thePart != NULL; thePart = thePart->mPreviousPart)
        theLast = thePart;
    return theLast;
}

long Box(long inA, long inB)
{
    return inA > inB ? inA - inB : inB - inA;
}

void ScanSiege(EEPlayer* inPlayer, SiegeScan& outScan)
{
    memset(&outScan, 0, sizeof(outScan));
    for (unsigned long p = 0; p < gWorld.GetNumPlayers() && p < 16; p++) {
        EEPlayer* theOwner = gWorld.GetPlayer(p);
        if (theOwner == NULL)
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
                continue;
            EEUnitClass* theClass = theUnit->mUnitClass;
            long theX = theUnit->mTileX, theY = theUnit->mTileY;
            unsigned theRoles = Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange);
            bool theCitizen = (theRoles & Roles::kCitizen) != 0;
            if (theOwner == inPlayer) {
                if (IsBuilding(theUnit) && outScan.mBuildings < 256) {
                    outScan.mBX[outScan.mBuildings] = theX;
                    outScan.mBY[outScan.mBuildings++] = theY;
                } else if (IsSoldierUnit(theUnit) && outScan.mSoldiers < 128) {
                    outScan.mSoldier[outScan.mSoldiers++] = theUnit;
                } else if (theCitizen && outScan.mCitizens < 128) {
                    outScan.mCitizen[outScan.mCitizens] = theUnit;
                    outScan.mCX[outScan.mCitizens] = theX;
                    outScan.mCY[outScan.mCitizens++] = theY;
                }
            } else if (IsEnemy(inPlayer, theOwner)) {
                float theRange, theAttack;
                if ((IsSoldierUnit(theUnit) || theCitizen) && outScan.mEnemyUnits < 128 && SeenBy(theUnit, inPlayer))
                    outScan.mEnemyUnit[outScan.mEnemyUnits++] = theUnit;
                if ((theRoles & (Roles::kSiege | Roles::kRam)) != 0 && outScan.mSiege < 16 && SeenBy(theUnit, inPlayer))
                    outScan.mEnemySiege[outScan.mSiege++] = theUnit;
                else if (IsBuilding(theUnit) && theUnit->mComplexUnit != NULL && !theUnit->mComplexUnit->mIsBuilt
                         && outScan.mTowers < 16 && DefenceStats(theUnit, theRange, theAttack) && SeenBy(theUnit, inPlayer))
                    outScan.mEnemyTower[outScan.mTowers++] = theUnit;
                else if (IsSoldierUnit(theUnit) && outScan.mEnemySoldiers < 256) {
                    outScan.mEX[outScan.mEnemySoldiers] = theX;
                    outScan.mEY[outScan.mEnemySoldiers++] = theY;
                }
            }
        }
    }
}

// early_cover (early.c): an own finished building that shoots reaches x, y (its range - 0.5)
bool EarlyCover(EEPlayer* inPlayer, long inX, long inY)
{
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        float theRange, theAttack;
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || Box(theUnit->mTileX, inX) > 16 || Box(theUnit->mTileY, inY) > 16
            || !DefenceStats(theUnit, theRange, theAttack) || !IsBuilt(theUnit))
            continue;
        long theDX = theUnit->mTileX - inX, theDY = theUnit->mTileY - inY;
        if ((float)(theDX * theDX + theDY * theDY) <= (theRange - 0.5f) * (theRange - 0.5f))
            return true;
    }
    return false;
}

Stats::Side sMilitiaOwn, sMilitiaFoe, sMilitiaSoldiers;

// militia_answer: the AI's citizens near inTarget attack it; returns how many were ordered
int MilitiaAnswer(EEPlayer* inPlayer, const SettingsAI& inSettings, SiegeScan& ioScan, EEUnit* inTarget, int& ioFree)
{
    const SettingsAI& theBase = Cfg(inPlayer->mPlayerID);
    unsigned thePI = inPlayer->mPlayerID & 15;
    long theX = inTarget->mTileX, theY = inTarget->mTileY;
    int theEnemies = 0, theOrdered = 0, theSoldiers = 0;
    float theEnemyPower = 0, theCitizenPower = 0, theFastest = 1e9f, theCitizenSpeed = 0;
    bool theEstimate = theBase.fight_estimate || theBase.militia_estimate || inSettings.early_militia;
    if (theEstimate) {
        Stats::Clear(sMilitiaOwn);
        Stats::Clear(sMilitiaFoe);
    }
    for (int k = 0; k < ioScan.mEnemyUnits; k++) {
        EEUnit* theEnemy = ioScan.mEnemyUnit[k];
        if (theEnemy->mHitPoints <= 0 || Box(theEnemy->mTileX, theX) > 6 || Box(theEnemy->mTileY, theY) > 6)
            continue;
        theEnemies++;
        theEnemyPower += UnitPower(theEnemy);
        if (theEstimate)
            Stats::Add(sMilitiaFoe, theEnemy);
        if (IsSoldierUnit(theEnemy)) {
            theSoldiers++;
            if (Stats::Speed(theEnemy) < theFastest)
                theFastest = Stats::Speed(theEnemy);
        }
    }
    if (theEnemies < 1) {   // a foundation: no fight, only its hit points
        theEnemies = 1;
        theEnemyPower = UnitPower(inTarget);
        if (theEstimate)
            Stats::Add(sMilitiaFoe, inTarget);
    }
    if (theEnemies > DefenceRules::kMilitiaMostEnemies)
        return 0;
    for (int c = 0, n = 0; c < ioScan.mCitizens && n < DefenceRules::kMilitiaPerEnemy * theEnemies; c++)
        if (ioScan.mCitizen[c] != NULL && Box(ioScan.mCX[c], theX) <= 10 && Box(ioScan.mCY[c], theY) <= 10) {
            theCitizenPower += UnitPower(ioScan.mCitizen[c]);
            n++;
            if (theEstimate)
                Stats::Add(sMilitiaOwn, ioScan.mCitizen[c]);
            if (Stats::Speed(ioScan.mCitizen[c]) > theCitizenSpeed)
                theCitizenSpeed = Stats::Speed(ioScan.mCitizen[c]);
        }
    // early militia: against soldiers, only next to own soldiers or under own towers, and only without a citizen lost
    if (inSettings.early_militia && theSoldiers) {
        bool theCover = EarlyCover(inPlayer, theX, theY);
        for (int k = 0; k < ioScan.mSoldiers && !theCover; k++)
            theCover = ioScan.mSoldier[k] != NULL && Box(ioScan.mSoldier[k]->mTileX, theX) <= 12 && Box(ioScan.mSoldier[k]->mTileY, theY) <= 12;
        Stats::Result theResult;
        if (theCover && sMilitiaOwn.mCount)
            Stats::Estimate(sMilitiaOwn, sMilitiaFoe, theResult);
        float theLost = theCover && sMilitiaOwn.mCount && theResult.mTime[0] < 1e9f
                        ? DefenceRules::CitizensLost(theResult.mContactDps[1], theResult.mTime[0], sMilitiaOwn.mCount, sMilitiaOwn.mHP)
                        : 99.0f;
        if (theLost >= 1.0f) {
            sDefence[thePI].mMilitiaRefused++;
            return 0;
        }
    }
    if (theBase.militia_estimate) {
        if (!sMilitiaOwn.mCount)
            return 0;
        // soldiers near that win it on their own do the job instead
        Stats::Clear(sMilitiaSoldiers);
        for (int k = 0; k < ioScan.mSoldiers; k++)
            if (ioScan.mSoldier[k] != NULL && Box(ioScan.mSoldier[k]->mTileX, theX) <= 12 && Box(ioScan.mSoldier[k]->mTileY, theY) <= 12)
                Stats::Add(sMilitiaSoldiers, ioScan.mSoldier[k]);
        if (sMilitiaSoldiers.mCount && Stats::Ratio(sMilitiaSoldiers, sMilitiaFoe) >= 1.0f)
            return 0;
        float theStand = Stats::Ratio(sMilitiaOwn, sMilitiaFoe);
        if (!theSoldiers) {   // builders, a foundation
            if (theStand < 1.0f)
                return 0;
        } else if (theStand < 1.5f) {   // caught: running is no way out - fight if the citizens still do some good
            Stats::Result theResult;
            Stats::Estimate(sMilitiaOwn, sMilitiaFoe, theResult);
            float theShare = theResult.mTime[0] < 1e9f ? theResult.mTime[1] / (theResult.mTime[0] > 0.1f ? theResult.mTime[0] : 0.1f) : 0;
            bool theAttacking = AttackTarget(GoalOf(inTarget)) != NULL;
            if (!DefenceRules::CaughtFight(theAttacking, theFastest, theCitizenSpeed, theShare, theBase.militia_caught_share))
                return 0;
            sDefence[thePI].mMilitiaCaught++;
        }
    } else {
        float theStand = theBase.fight_estimate ? Stats::Ratio(sMilitiaOwn, sMilitiaFoe)
                                                : DefenceRules::PowerRatio(theCitizenPower, theEnemyPower);
        if (theStand < DefenceRules::kMilitiaStand)
            return 0;   // citizens beat clubmen and builders, not real soldiers
    }
    for (int c = 0; c < ioScan.mCitizens && theOrdered < DefenceRules::kMilitiaPerEnemy * theEnemies; c++) {
        EEUnit* theCitizen = ioScan.mCitizen[c];
        if (theCitizen == NULL || Box(ioScan.mCX[c], theX) > 10 || Box(ioScan.mCY[c], theY) > 10)
            continue;
        EEUnitGoal* theGoal = GoalOf(theCitizen);
        if (dynamic_cast<EEUGAttackUnit*>(theGoal) != NULL) {   // already fighting
            ioScan.mCitizen[c] = NULL;
            continue;
        }
        if (theGoal != NULL && dynamic_cast<EEUGGather*>(theGoal) == NULL)
            continue;   // building, walking: leave it
        if (AttackUnit(theCitizen, inTarget))
            theOrdered++;
        ioScan.mCitizen[c] = NULL;   // one target per citizen per tick
    }
    ioFree -= theOrdered;
    return theOrdered;
}

void Militia(EEPlayer* inPlayer, const SettingsAI& inSettings, SiegeScan& ioScan)
{
    const SettingsAI& theBase = Cfg(inPlayer->mPlayerID);
    if (!theBase.citizen_militia || !ioScan.mCitizens || !ioScan.mEnemyUnits)
        return;
    bool theEarly = theBase.militia_estimate || CurrentEpoch(inPlayer) <= 3;
    int theFree = ioScan.mCitizens, theTotal = 0;
    for (int k = 0; k < ioScan.mEnemyUnits && theFree > 2; k++) {
        EEUnit* theEnemy = ioScan.mEnemyUnit[k];
        EEUnitGoal* theGoal = GoalOf(theEnemy);
        if (theGoal == NULL || theEnemy->mHitPoints <= 0)
            continue;
        long theX = theEnemy->mTileX, theY = theEnemy->mTileY;
        int theWhy = 0;
        EEUGBuildRepair* theBuild = dynamic_cast<EEUGBuildRepair*>(theGoal);
        if (theBuild != NULL) {
            EEUnit* theSite = theBuild->mTarget;
            float theRange, theAttack;
            if (theSite != NULL && theSite->mPlayer != inPlayer && theSite->mComplexUnit != NULL && !theSite->mComplexUnit->mIsBuilt
                && DefenceStats(theSite, theRange, theAttack))
                theWhy = 1;   // a tower going up
        } else if (theEarly) {
            EEUnit* theTarget = AttackTarget(theGoal);
            if (theTarget != NULL && theTarget->mPlayer == inPlayer)
                theWhy = 2;   // attacking one of ours
        }
        if (!theWhy)
            continue;
        bool theClose = false;
        for (int b = 0; theWhy == 1 && b < ioScan.mBuildings && !theClose; b++)
            theClose = Box(ioScan.mBX[b], theX) <= 12 && Box(ioScan.mBY[b], theY) <= 12;
        long theReach = theWhy == 1 ? 12 : 10;
        for (int c = 0; c < ioScan.mCitizens && !theClose; c++)
            theClose = Box(ioScan.mCX[c], theX) <= theReach && Box(ioScan.mCY[c], theY) <= theReach;
        if (theClose)
            theTotal += MilitiaAnswer(inPlayer, inSettings, ioScan, theEnemy, theFree);
    }
    sDefence[inPlayer->mPlayerID & 15].mMilitiaOrders += (unsigned long)theTotal;
}

}

DefenceCounts DefenceCountsOf(unsigned long inPlayerID)
{
    return sDefence[inPlayerID & 15];
}

void CounterSiege(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    static SiegeScan sScan;
    unsigned thePI = inPlayer->mPlayerID & 15;
    const SettingsAI& theBase = Cfg(inPlayer->mPlayerID);
    if (!theBase.counter_siege || (inNow >= sLast[thePI] && inNow - sLast[thePI] < 2000))
        return;
    sLast[thePI] = inNow;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, inPlayer->GetDifficulty());
    SiegeScan& s = sScan;
    ScanSiege(inPlayer, s);
    Militia(inPlayer, theSettings, s);
    for (int i = 0; i < s.mSiege + s.mTowers; i++) {
        bool theTower = i >= s.mSiege;   // an enemy defence going up near our buildings or citizens (counter-tower)
        EEUnit* theEnemy = theTower ? s.mEnemyTower[i - s.mSiege] : s.mEnemySiege[i];
        long theX = theEnemy->mTileX, theY = theEnemy->mTileY;
        float theReach = theTower ? 12.0f : Stats::Range(theEnemy) + 4.0f;
        bool theThreat = false;
        for (int b = 0; b < s.mBuildings && !theThreat; b++) {
            long theDX = s.mBX[b] - theX, theDY = s.mBY[b] - theY;
            theThreat = (float)(theDX * theDX + theDY * theDY) <= theReach * theReach;
        }
        for (int c = 0; theTower && c < s.mCitizens && !theThreat; c++) {
            long theDX = s.mCX[c] - theX, theDY = s.mCY[c] - theY;
            theThreat = (float)(theDX * theDX + theDY * theDY) <= theReach * theReach;
        }
        if (!theThreat)
            continue;
        // a tower going up: its builders first (the foundation is harmless without them)
        EEUnit* theBuilders[8];
        EEUnit* theSite = theTower ? FieldSite(theEnemy) : NULL;
        int theBuilderCount = 0;
        for (int k = 0; theTower && k < s.mEnemyUnits && theBuilderCount < 8; k++) {
            EEUGBuildRepair* theBuild = dynamic_cast<EEUGBuildRepair*>(GoalOf(s.mEnemyUnit[k]));
            if (theBuild != NULL && s.mEnemyUnit[k]->mHitPoints > 0 && (theBuild->mTarget == theSite || theBuild->mTarget == theEnemy))
                theBuilders[theBuilderCount++] = s.mEnemyUnit[k];
        }
        int theGuards = 0;
        for (int k = 0; k < s.mEnemySoldiers; k++)
            theGuards += Box(s.mEX[k], theX) <= 8 && Box(s.mEY[k], theY) <= 8;
        // responders: nearest first, up to 5
        int thePick[5], thePicked = 0, theAvailable = 0, theAlready = 0;
        for (int k = 0; k < s.mSoldiers; k++) {
            EEUnit* theUnit = s.mSoldier[k];
            if (theUnit == NULL)
                continue;   // already ordered this tick
            long theDX = theUnit->mTileX - theX, theDY = theUnit->mTileY - theY, theD2 = theDX * theDX + theDY * theDY;
            if (theD2 > 400)
                continue;
            EEUnitGoal* theGoal = GoalOf(theUnit);
            if (dynamic_cast<EEUGAttackUnit*>(theGoal) != NULL) {
                EEUnit* theTarget = AttackTarget(theGoal);
                bool theMine = theTarget == theEnemy;
                for (int b = 0; b < theBuilderCount && !theMine; b++)
                    theMine = theTarget == theBuilders[b];
                if (theMine) {
                    theAlready++;
                    s.mSoldier[k] = NULL;
                    continue;
                }
                if (theTarget != NULL && Box(theTarget->mTileX, theUnit->mTileX) <= 4 && Box(theTarget->mTileY, theUnit->mTileY) <= 4)
                    continue;   // busy fighting close by
            }
            theAvailable++;
            if (thePicked < 5) {
                thePick[thePicked++] = k;
            } else {   // keep the 5 nearest
                int theWorst = 0;
                long theFarthest = -1;
                for (int j = 0; j < 5; j++) {
                    EEUnit* theOther = s.mSoldier[thePick[j]];
                    long theVX = theOther->mTileX - theX, theVY = theOther->mTileY - theY;
                    if (theVX * theVX + theVY * theVY > theFarthest) {
                        theFarthest = theVX * theVX + theVY * theVY;
                        theWorst = j;
                    }
                }
                if (theD2 < theFarthest)
                    thePick[theWorst] = k;
            }
        }
        if (theAlready >= DefenceRules::kCounterSiegeMost)
            continue;
        if (!DefenceRules::SoldiersGo(theAvailable, theAlready, theGuards)) {
            int theFree = s.mCitizens, theCount = 0;   // a tower going up and too few soldiers: the citizens go at the builders
            for (int b = 0; theTower && theBase.citizen_militia && b < theBuilderCount && theFree > 2; b++)
                theCount += MilitiaAnswer(inPlayer, theSettings, s, theBuilders[b], theFree);
            if (theTower && theBase.citizen_militia && !theBuilderCount)
                theCount += MilitiaAnswer(inPlayer, theSettings, s, theEnemy, theFree);   // nobody building: the foundation
            if (theCount)
                sDefence[thePI].mMilitiaOrders += (unsigned long)theCount;
            else
                sDefence[thePI].mCounterSiegeSkipped++;
            continue;
        }
        int theOrdered = 0;
        for (int j = 0; j < thePicked && theAlready + theOrdered < DefenceRules::kCounterSiegeMost; j++) {
            EEUnit* theUnit = s.mSoldier[thePick[j]];
            EEUnit* theTarget = theBuilderCount ? theBuilders[(theAlready + theOrdered) % theBuilderCount] : theEnemy;   // spread over the builders
            if (AttackUnit(theUnit, theTarget))
                theOrdered++;
            s.mSoldier[thePick[j]] = NULL;   // one target per unit per tick
        }
        sDefence[thePI].mCounterSiegeOrders += (unsigned long)theOrdered;
    }
}


// THE DEFENCE LAYOUT (def_refresh, def_slot, defence_spot and the extra towers of defence_tick in defence.c): the player's base,
// its two nearest enemies seen (by their buildings; none seen: the map's centre) and, on the way to each, a front 10 tiles
// (defence_distance) out; tower slots across each front (0, -1, +1, then two more), by each drop site beyond 12 tiles from the
// base, and then the rest. Every 20 s (defence_layout, and defence_walls or enclose), with defence_towers and no tower queued, a tower of the line the player can build goes into the build
// queue at the first free slot when the stone pays it with 100 left, at most once a minute. Not ported: the fronts' walls and
// gates (the wall manager), the site forts and the enclosure (both off by default), the coast towers, and the slots the
// game's own tower requests take (hook_place).
namespace {

using TowerRules::Front;
struct Plan {
    EEPlayer* mPlayer;
    unsigned long mTime;
    long mBX, mBY;
    int mFronts, mOuts;
    Front mFront[2];
    long mOut[8][2];
    struct {
        long mX, mY;
        unsigned long mTime;
    } mPending[8];   // slots given out recently: the construction may not show yet
} sPlan[16];

void PlanRefresh(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    unsigned idx = inPlayer->mPlayerID & 15;
    Plan& thePlan = sPlan[idx];
    if (thePlan.mPlayer == inPlayer && inNow >= thePlan.mTime && inNow - thePlan.mTime < 30000)
        return;
    if (thePlan.mPlayer != inPlayer)
        memset(&thePlan, 0, sizeof thePlan);
    thePlan.mPlayer = inPlayer;
    thePlan.mTime = inNow;
    thePlan.mFronts = thePlan.mOuts = 0;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL || theCivilization->mBuildManager->mTownCenterTile.mX == -10000)
        return;
    thePlan.mBX = theCivilization->mBuildManager->mTownCenterTile.mX;
    thePlan.mBY = theCivilization->mBuildManager->mTownCenterTile.mY;
    long theD2[16], theX[16], theY[16];
    for (int i = 0; i < 16; i++)
        theD2[i] = 0x7FFFFFFF;
    for (unsigned long p = 0; p < gWorld.mNumPlayers && p < 16; p++) {
        EEPlayer* theOther = gWorld.mPlayers[p];
        if (theOther == NULL || theOther == inPlayer || !IsEnemyOf(inPlayer, theOther))
            continue;
        for (EEUnit* theUnit = theOther->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL
                || (theUnit->mUnitClass->mCategory != kUnitCategoryBuilding && theUnit->mUnitClass->mCategory != kUnitCategoryFortress)
                || !SeenBy(theUnit, inPlayer))
                continue;
            long theDX = theUnit->mTileX - thePlan.mBX, theDY = theUnit->mTileY - thePlan.mBY, theDist = theDX * theDX + theDY * theDY;
            unsigned thePI = theOther->mPlayerID & 15;
            if (theDist < theD2[thePI]) {
                theD2[thePI] = theDist;
                theX[thePI] = theUnit->mTileX;
                theY[thePI] = theUnit->mTileY;
            }
        }
    }
    int theFirst = -1;
    for (int k = 0; k < 2; k++) {   // the two nearest enemies
        int theBest = -1;
        for (int i = 0; i < 16; i++)
            if (theD2[i] != 0x7FFFFFFF && (theBest < 0 || theD2[i] < theD2[theBest]) && (k == 0 || i != theFirst))
                theBest = i;
        float theDX, theDY;
        if (theBest >= 0) {
            theDX = (float)(theX[theBest] - thePlan.mBX);
            theDY = (float)(theY[theBest] - thePlan.mBY);
        } else if (k == 0) {   // nothing seen yet: towards the map's centre
            long theSize = gWorld.mMap != NULL ? (long)gWorld.mMap->mSize : 0;
            theDX = (float)(theSize / 2 - thePlan.mBX);
            theDY = (float)(theSize / 2 - thePlan.mBY);
        } else
            break;
        float theLength = sqrtf(theDX * theDX + theDY * theDY);
        if (theLength < 4.0f)
            break;
        if (k == 0)
            theFirst = theBest;
        Front& theFront = thePlan.mFront[thePlan.mFronts];
        theFront.mDX = theDX / theLength;
        theFront.mDY = theDY / theLength;
        long theDistance = Cfg(inPlayer->mPlayerID).defence_distance;
        theFront.mFX = thePlan.mBX + (long)lroundf(theFront.mDX * (float)theDistance);
        theFront.mFY = thePlan.mBY + (long)lroundf(theFront.mDY * (float)theDistance);
        thePlan.mFronts++;
    }
    std::vector<std::pair<long, long> > theDrops;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL && theDrops.size() < 64; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
            continue;
        long theType = (long)theUnit->mUnitClass->mUnitType;
        if (theType == kUnitTypeTownCenter || theType == kUnitTypeCapitol || theType == kUnitTypeSettlement)
            theDrops.push_back(std::make_pair((long)theUnit->mTileX, (long)theUnit->mTileY));
    }
    for (size_t i = 0; i < theDrops.size() && thePlan.mOuts < 8; i++) {
        long theDX = theDrops[i].first - thePlan.mBX, theDY = theDrops[i].second - thePlan.mBY;
        if (theDX * theDX + theDY * theDY < 144 || !thePlan.mFronts)
            continue;
        thePlan.mOut[thePlan.mOuts][0] = theDrops[i].first + (long)lroundf(thePlan.mFront[0].mDX * 3.0f);
        thePlan.mOut[thePlan.mOuts][1] = theDrops[i].second + (long)lroundf(thePlan.mFront[0].mDY * 3.0f);
        thePlan.mOuts++;
    }
}

bool DefenceSpot(EEComputerPlayer* inPlayer, long inType, unsigned long inNow, long& outX, long& outY)
{
    unsigned idx = inPlayer->mPlayerID & 15;
    PlanRefresh(inPlayer, inNow);
    Plan& thePlan = sPlan[idx];
    ThreatRefresh(inPlayer, inNow);   // slots inside known enemy defences would cost the builders
    long theX, theY;
    for (int i = 0; TowerRules::Slot(thePlan.mFront, thePlan.mFronts, thePlan.mOut, thePlan.mOuts, i, theX, theY); i++) {
        int theFinished, theAll;
        TowerCounts(inPlayer, theX, theY, 2, theFinished, theAll);
        bool theTaken = theAll > 0;
        for (int t = 0; t < 8 && !theTaken; t++)
            theTaken = thePlan.mPending[t].mTime != 0 && inNow - thePlan.mPending[t].mTime < 90000 && labs(thePlan.mPending[t].mX - theX) <= 2
                && labs(thePlan.mPending[t].mY - theY) <= 2;
        if (theTaken)
            continue;
        for (int r = 0; r <= 2; r++)
            for (int ax = -r; ax <= r; ax++)
                for (int ay = -r; ay <= r; ay++) {
                    if ((abs(ax) != r && abs(ay) != r) || RouteCovered(inPlayer, thePlan.mBX, thePlan.mBY, theX + ax, theY + ay)
                        || RaidersNear(inPlayer, theX + ax, theY + ay, 10) || !CanPlace(inPlayer, inType, theX + ax, theY + ay))
                        continue;
                    outX = theX + ax;
                    outY = theY + ay;
                    int theOldest = 0;
                    for (int t = 1; t < 8; t++)
                        if (thePlan.mPending[t].mTime < thePlan.mPending[theOldest].mTime)
                            theOldest = t;
                    thePlan.mPending[theOldest].mX = outX;
                    thePlan.mPending[theOldest].mY = outY;
                    thePlan.mPending[theOldest].mTime = inNow != 0 ? inNow : 1;
                    return true;
                }
    }
    return false;
}

}

void DefenceTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16], sLastTower[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    const SettingsAI& theBase = Cfg(inPlayer->mPlayerID);
    if (!theBase.defence_layout || (!theBase.defence_walls && !theBase.enclose)
        || (sLast[idx] != 0 && inNow >= sLast[idx] && inNow - sLast[idx] < 20000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL || inPlayer->mTechnologies == NULL)
        return;
    PlanRefresh(inPlayer, inNow);
    if (!theBase.defence_towers || (sLastTower[idx] != 0 && inNow >= sLastTower[idx] && inNow - sLastTower[idx] < 60000))
        return;
    EECPBuildManager* theBuild = theCivilization->mBuildManager;
    long theType = NewestTowerOf(inPlayer);
    long theEpoch = inPlayer->mTechnologies->mCurrentEpoch;
    if (theType == 0 || TowerIsQueued(theBuild) || theEpoch < 0 || theEpoch >= 14)
        return;
    EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[theType]->mTechnology);
    long theCost = theNode != NULL ? theNode->GetCost(kResourceStone, theType, 1.0f) : 0;
    long theX, theY;
    if (inPlayer->mResourceCaps[kResourceStone] >= theCost + 100 && DefenceSpot(inPlayer, theType, inNow, theX, theY)) {
        theBuild->QueueBuildingAt(theType, theX, theY);
        sLastTower[idx] = inNow != 0 ? inNow : 1;
    }
}

}
