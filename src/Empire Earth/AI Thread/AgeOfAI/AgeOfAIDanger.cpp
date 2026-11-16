// What is dangerous for an Age of AI player's citizens: the enemy defences it has seen, the raiders in reach, the places where its
// citizens were hurt, and so which sites its searches refuse.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/attack.c (threat_refresh,
// route_covered, raiders_scan, hurt_note, site_danger_res, the damage memory of citizens_safety) and dll/ai/cover.c (cv_mem_raider)
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Not ported yet: raid_held (defence.c), walled_out and ship_reach_exact (off by default),
// the danger map for routes, and the rest of citizens_safety (the citizens sent back, the strays, the flight, the shelter).
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

void AttackUnit(EEUnit* inUnit, EEUnit* inTarget)
{
    if (inUnit->mHitPoints <= 0 || inUnit->mTileX == -10000 || inUnit->mComplexUnit == NULL)
        return;
    EEUGAttackUnit* theGoal = new EEUGAttackUnit(inUnit->mComplexUnit, inTarget, true, false);
    inUnit->mComplexUnit->SetGoal(theGoal, false, 0);
    inUnit->mComplexUnit->StartAction(kActionPrepareToMove);
    inUnit->mUnitClass->ActionChanged(inUnit->mComplexUnit);
}

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

}
