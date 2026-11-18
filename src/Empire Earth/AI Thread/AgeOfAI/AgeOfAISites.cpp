// The mine search and the worker rebalance of an Age of AI player.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/sites.c (select_mine_near_drop,
// mine_crowds, find_mine_pick) and dll/ai/econ.c (econ_rebalance, movable_gatherer) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
// Also dll/ai/food.c (food_bush, food_pull). Not ported yet: the mine search's tile flag test, town_work (off by default),
// overseas-mine preference in other searches, the wood and field searches, and no_fields_wild (off by default). Ferry/no-citizen
// mine searches prefer an overseas mine when [patches] overseas_mines is on.
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
#include "AgeOfAISites.h"
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
#include "ee/EECPResourceCenter.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEAreaUtilities.h"
#include "AgeOfAI.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"
#include "AgeOfAIWorkers.h"

namespace AgeOfAI {

namespace {

const int kFarmEpoch = 2;

// the drop sites of a player, from its own units: the Town Center, Capitol and Settlement take everything, a Granary food only
struct Drop {
    long mX, mY;
    bool mFood;
};
struct DropCache {
    EEPlayer* mPlayer;
    unsigned long mTime;
    std::vector<Drop> mDrops;
} sDrops[16];

// the AI's own citizens aiming at a mine (their gather goal's site), counted every 2 seconds, a pick added at once
struct Aim {
    unsigned long mTime;
    std::vector<std::pair<EEUnit*, int> > mSites;
} sAim[16];

const SettingsAI& SettingsOf(EEComputerPlayer* inPlayer)
{
    return SettingsAIOf(inPlayer->mPlayerID, inPlayer->GetDifficulty());
}

bool IsCitizenUnit(EEUnit* inUnit)
{
    EEUnitClass* theClass = inUnit->mUnitClass;
    return theClass != NULL && (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kCitizen) != 0;
}

void DropsCached(EEPlayer* inPlayer)
{
    DropCache& theCache = sDrops[inPlayer->mPlayerID & 15];
    unsigned long theNow = gPhysics->mGameTime;
    if (theCache.mPlayer == inPlayer && theNow >= theCache.mTime && theNow - theCache.mTime <= 5000)   // cached 5 seconds
        return;
    theCache.mPlayer = inPlayer;
    theCache.mTime = theNow;
    theCache.mDrops.clear();
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL && theCache.mDrops.size() < 64; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
            continue;
        long theType = (long)theUnit->mUnitClass->mUnitType;
        bool theFood = theType == kUnitTypeGranary || theType == kUnitTypeGranaryImproved;
        if (!theFood && theType != kUnitTypeTownCenter && theType != kUnitTypeCapitol && theType != kUnitTypeSettlement)
            continue;
        Drop theDrop = {theUnit->mTileX, theUnit->mTileY, theFood};
        theCache.mDrops.push_back(theDrop);
    }
}

// the nearest cached drop site to a tile (food: granaries count too): its squared distance, 0x7FFFFFFF for none; outIndex its index
long DropNearest(unsigned inPlayer, long inX, long inY, bool inFood, int* outIndex)
{
    long theBest = 0x7FFFFFFF;
    const std::vector<Drop>& theDrops = sDrops[inPlayer & 15].mDrops;
    for (size_t i = 0; i < theDrops.size(); i++) {
        long theDX = theDrops[i].mX - inX, theDY = theDrops[i].mY - inY, theD2 = theDX * theDX + theDY * theDY;
        if ((theDrops[i].mFood && !inFood) || theD2 >= theBest)
            continue;
        theBest = theD2;
        if (outIndex != NULL)
            *outIndex = (int)i;
    }
    return theBest;
}

bool IsGathering(EEComplexUnit* inUnit, EEUnit** outSite)
{
    EEUGGather* theGoal = dynamic_cast<EEUGGather*>(inUnit->mGoalState);
    if (theGoal == NULL)
        return false;
    *outSite = theGoal->mResource;
    return true;
}

void AimCount(EEPlayer* inPlayer)
{
    Aim& theAim = sAim[inPlayer->mPlayerID & 15];
    unsigned long theNow = gPhysics->mGameTime;
    if (theAim.mTime != 0 && theNow >= theAim.mTime && theNow - theAim.mTime < 2000)
        return;
    theAim.mTime = theNow ? theNow : 1;
    theAim.mSites.clear();
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL || !IsCitizenUnit(theUnit))
            continue;
        EEUnit* theSite = NULL;
        if (!IsGathering(theUnit->mComplexUnit, &theSite) || theSite == NULL)
            continue;
        long theResource = (long)theSite->mResourceType;
        if (theResource < 4 || theResource > 6)
            continue;
        size_t k;
        for (k = 0; k < theAim.mSites.size() && theAim.mSites[k].first != theSite; k++) {}
        if (k == theAim.mSites.size()) {
            if (k == 512)
                continue;
            theAim.mSites.push_back(std::make_pair(theSite, 0));
        }
        theAim.mSites[k].second++;
    }
}

unsigned AimOf(unsigned inPlayer, EEUnit* inSite)
{
    const Aim& theAim = sAim[inPlayer & 15];
    for (size_t k = 0; k < theAim.mSites.size(); k++)
        if (theAim.mSites[k].first == inSite)
            return (unsigned)theAim.mSites[k].second;
    return 0;
}

void AimAdd(unsigned inPlayer, EEUnit* inSite)
{
    Aim& theAim = sAim[inPlayer & 15];
    size_t k;
    for (k = 0; k < theAim.mSites.size() && theAim.mSites[k].first != inSite; k++) {}
    if (k == theAim.mSites.size()) {
        if (k == 512)
            return;
        theAim.mSites.push_back(std::make_pair(inSite, 0));
    }
    theAim.mSites[k].second++;
}

// no known enemy on the straight way from the drop site to the mine (the build manager's threat test), skipping our own base
bool RouteSafe(EECPBuildManager* inBuild, long inFromX, long inFromY, long inToX, long inToY)
{
    long theDX = inToX - inFromX, theDY = inToY - inFromY;
    int theLength = (int)sqrtf((float)(theDX * theDX + theDY * theDY));
    for (int t = 3; t < theLength; t += 2)
        if (inBuild->HasEnemyNear(inFromX + theDX * t / theLength, inFromY + theDY * t / theLength))
            return false;
    return true;
}

bool ExploredBy(EEUnit* inSite, EEPlayer* inPlayer)
{
    return inSite->mTile != NULL && inSite->mTile->GetVisibility(inPlayer->mPlayerID) != 0;
}

// The free mine of a kind nearest to one of the player's drop sites (mine_choice): the original's checks (active zone,
// explored, the gatherer cap, no enemy near, the unit's continent) and no known enemy on the way from that drop site.
// inNotContinent: a continent whose mines do not count, 0xFFFF none. NULL when none: the original search then runs.
EEUnit* SelectMineNearDrop(EECPResourceManager* inResources, long inType, EEComplexUnit* inUnit, long inNotContinent)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    EECPBuildManager* theBuild = inResources->mManager->mBuildManager;
    if (theBuild == NULL)
        return NULL;
    unsigned thePI = thePlayer->mPlayerID & 15;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    DropsCached(thePlayer);
    if (theSettings.mine_crowds)
        AimCount(thePlayer);
    unsigned long theCap = thePlayer->mMiningLimit;
    long theRegion = inUnit != NULL ? (long)gContinentManager.GetContinentID(inUnit) : 0;
    // LOCAL MINES ([ai_base] local_mines): a mine whose drop site is far from the citizen is another town's, and the
    // game's own assignment sent citizens a long way for it; one its own town can reach comes first
    const bool theLocal = theSettings.local_mines && inUnit != NULL;
    EEUnit* theBest = NULL;
    long theBestD = 0x7FFFFFFF;
    int theBestLocal = 0;
    for (unsigned long k = 0; k < thePlayer->GetNumResourceCenters(); k++) {
        EECPResourceCenter* theZone = thePlayer->GetResourceCenter(k);
        if (theZone == NULL || !theZone->mIsActive)
            continue;
        EEUnit* theSite = theZone->FindResourceUnit((unsigned long)inType);
        if (theSite == NULL || theSite->mResourceAmount <= 0)
            continue;
        int theIndex = -1;
        long theD = DropNearest(thePI, theSite->mTileX, theSite->mTileY, false, &theIndex);   // nearest drop site for all resources
        if (theIndex < 0)
            continue;
        const Drop& theDrop = sDrops[thePI].mDrops[theIndex];
        int theIsLocal = 0;
        if (theLocal) {
            theIsLocal = SitesRules::MineLocal(theDrop.mX, theDrop.mY, inUnit->mTileX, inUnit->mTileY);
            if (theIsLocal < 0)
                continue;
        }
        if (theIsLocal < theBestLocal || (theIsLocal == theBestLocal && theD >= theBestD))
            continue;
        if (!ExploredBy(theSite, thePlayer))
            continue;
        if ((unsigned long)gWorld.GetResourceUsers(theSite) >= theCap)
            continue;
        if (theSettings.mine_crowds && AimOf(thePI, theSite) >= theCap)   // the citizens on their way or waiting there count
            continue;
        long theMineRegion = (long)gContinentManager.GetContinentID(theSite);
        if (inUnit != NULL && theMineRegion != theRegion)
            continue;
        if (inNotContinent >= 0 && theMineRegion == inNotContinent)
            continue;
        if (theBuild->HasEnemyNear(theSite->mTileX, theSite->mTileY))
            continue;
        if (!RouteSafe(theBuild, theDrop.mX, theDrop.mY, theSite->mTileX, theSite->mTileY) || SiteDanger(thePlayer, theSite, false, (int)inType))
            continue;
        theBest = theSite;
        theBestD = theD;
        theBestLocal = theIsLocal;
    }
    if (theBest != NULL && theSettings.mine_crowds)
        AimAdd(thePI, theBest);
    return theBest;
}

}

// FORAGE NEAR ([ai_base] food_plan forage_near, off in the shipped file): the food search takes the free bush nearest to a
// drop site that takes food, within the reach of the pass - two times the player's drop-off distance for the pass that
// names a citizen, six for the one that does not - with the game's own checks. Upstream's food_bush and food_near_pick.
// Called from EECPResourceManager::FindFarm, the game's own first-bush-in-centre-order search.
EEUnit* FindForage(EECPResourceManager* inResources, bool inNear, EEComplexUnit* inUnit)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    EECPBuildManager* theBuild = inResources->mManager->mBuildManager;
    if (theBuild == NULL || !SettingsOf(thePlayer).food_plan.forage_near)
        return NULL;
    long theBaseX = theBuild->mTownCenterTile.mX, theBaseY = theBuild->mTownCenterTile.mY;
    if (theBaseX == -10000 || theBaseY == -10000)
        return NULL;
    unsigned thePI = thePlayer->mPlayerID & 15;
    DropsCached(thePlayer);
    unsigned long theCap = thePlayer->mMiningLimit;
    long theReach2 = SitesRules::ForageReach2(thePlayer->GetDropOffDistance(), inNear);
    long theRegion = inUnit != NULL ? (long)gContinentManager.GetContinentID(inUnit) : 0;
    EEUnit* theBest = NULL;
    long theBestD = 0x7FFFFFFF;
    for (unsigned long k = 0; k < thePlayer->GetNumResourceCenters(); k++) {
        EECPResourceCenter* theZone = thePlayer->GetResourceCenter(k);
        if (theZone == NULL || !theZone->mIsActive)
            continue;
        for (size_t z = 0; z < theZone->mUnits.size() && z < 4096; z++) {
            EEUnit* theSite = theZone->mUnits[z];
            if (theSite == NULL || theSite->mHitPoints <= 0 || theSite->mResourceAmount <= 0 || theSite->mUnitClass == NULL)
                continue;
            if ((long)theSite->mResourceType != kResourceFood || (long)theSite->mUnitClass->mUnitType != kUnitTypeResForageBush)
                continue;
            long theDX = theSite->mTileX - theBaseX, theDY = theSite->mTileY - theBaseY;
            if (theDX * theDX + theDY * theDY >= theReach2)
                continue;
            int theIndex = -1;
            long theD = DropNearest(thePI, theSite->mTileX, theSite->mTileY, true, &theIndex);   // a drop site that takes food
            if (theIndex < 0 || theD >= theBestD)
                continue;
            if (!ExploredBy(theSite, thePlayer))
                continue;
            if ((unsigned long)gWorld.GetResourceUsers(theSite) >= theCap)
                continue;
            if (inUnit != NULL && (long)gContinentManager.GetContinentID(theSite) != theRegion)
                continue;
            if (theBuild->HasEnemyNear(theSite->mTileX, theSite->mTileY))
                continue;
            if (SiteDanger(thePlayer, theSite, true, 2))
                continue;
            theBest = theSite;
            theBestD = theD;
        }
    }
    return theBest;
}

// The mine search of an Age of AI player (EECPResourceManager::FindResourceTarget): with mine_choice the free mine nearest to a
// drop site, else the game's own search; a mine the AI's citizens already fill (the game counts only those working) is refused
EEUnit* FindMine(EECPResourceManager* inResources, long inType, EEComplexUnit* inUnit, bool inBackwards)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    EECPBuildManager* theBuild = inResources->mManager->mBuildManager;
    const bool theMineResource = inType >= 4 && inType < 7;
    // OVERSEAS MINES: a search with no citizen (the ferry/island-expansion pass) wants a mine on another continent than the
    // base, not a home-island mine that would strand citizens on a useless crossing (upstream hook_find_mine, mine_site 5)
    long theOverseasHome = -1;
    if (gSettings.overseas_mines && theMineResource && inUnit == NULL && theBuild != NULL) {
        long theBaseX = theBuild->mTownCenterTile.mX, theBaseY = theBuild->mTownCenterTile.mY;
        if (theBaseX != -10000 && theBaseY != -10000)
            theOverseasHome = (long)gContinentManager.GetContinentID(theBaseX, theBaseY);
    }
    EEUnit* theResult = NULL;
    if (theSettings.mine_choice && theMineResource)
        theResult = SelectMineNearDrop(inResources, inType, inUnit, theOverseasHome >= 0 ? theOverseasHome : -1);
    if (theResult == NULL)
        theResult = inResources->FindResourceTargetGame(inType, inUnit, inBackwards);
    if (theResult != NULL && SiteDanger(thePlayer, theResult, inType == 2, (int)inType))   // the searches refuse a dangerous site
        theResult = NULL;
    if (theOverseasHome >= 0 && theResult != NULL && (long)gContinentManager.GetContinentID(theResult) == theOverseasHome)
        theResult = NULL;   // overseas_mines: no home-island mine for a crossing
    if (theResult != NULL && inUnit != NULL && theMineResource && theSettings.local_mines) {
        unsigned thePI = thePlayer->mPlayerID & 15;
        DropsCached(thePlayer);
        int theIndex = -1;
        if (DropNearest(thePI, theResult->mTileX, theResult->mTileY, false, &theIndex) == 0x7FFFFFFF)
            theResult = NULL;
        else {
            const Drop& theDrop = sDrops[thePI].mDrops[theIndex];
            long theDX = theDrop.mX - inUnit->mTileX, theDY = theDrop.mY - inUnit->mTileY;
            if (theDX * theDX + theDY * theDY > SitesRules::kLocalMineFar * SitesRules::kLocalMineFar)   // the game's pick is another town's mine: refused
                theResult = NULL;
        }
    }
    if (theResult != NULL && theSettings.mine_crowds && theMineResource) {
        unsigned thePI = thePlayer->mPlayerID & 15;
        AimCount(thePlayer);
        if (AimOf(thePI, theResult) >= thePlayer->mMiningLimit)
            return NULL;
        AimAdd(thePI, theResult);
    }
    return theResult;
}

// MINE CROWDS: the citizens beyond the cap at a mine go to the nearest free one of the same kind, every 10 seconds (the
// farthest from the mine first, 20 at most)
void MineCrowds(EECPResourceManager* inResources)
{
    static unsigned long sLast[16];
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    unsigned thePI = thePlayer->mPlayerID & 15;
    unsigned long theNow = gPhysics->mGameTime;
    if (!theSettings.mine_crowds || !theSettings.mine_choice)
        return;
    if (sLast[thePI] != 0 && theNow >= sLast[thePI] && theNow - sLast[thePI] < 10000)
        return;
    sLast[thePI] = theNow ? theNow : 1;
    sAim[thePI].mTime = 0;
    AimCount(thePlayer);
    unsigned long theCap = thePlayer->mMiningLimit;
    int theMoved = 0;
    Aim& theAim = sAim[thePI];
    for (size_t k = 0; k < theAim.mSites.size() && theMoved < 20; k++) {
        if ((unsigned long)theAim.mSites[k].second <= theCap)
            continue;
        EEUnit* theSite = theAim.mSites[k].first;
        long theType = (long)theSite->mResourceType;
        int theOver = theAim.mSites[k].second - (int)theCap;
        std::vector<std::pair<EEComplexUnit*, long> > theCitizens;
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL && theCitizens.size() < 256; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL || !IsCitizenUnit(theUnit))
                continue;
            EEUnit* theGoalSite = NULL;
            if (!IsGathering(theUnit->mComplexUnit, &theGoalSite) || theGoalSite != theSite)
                continue;
            long theDX = theUnit->mTileX - theSite->mTileX, theDY = theUnit->mTileY - theSite->mTileY;
            theCitizens.push_back(std::make_pair(theUnit->mComplexUnit, theDX * theDX + theDY * theDY));
        }
        while (theOver > 0 && !theCitizens.empty() && theMoved < 20) {   // the farthest from the mine first
            size_t theFar = 0;
            for (size_t j = 1; j < theCitizens.size(); j++)
                if (theCitizens[j].second > theCitizens[theFar].second)
                    theFar = j;
            EEComplexUnit* theUnit = theCitizens[theFar].first;
            theCitizens[theFar] = theCitizens.back();
            theCitizens.pop_back();
            EEUnit* theTo = SelectMineNearDrop(inResources, theType, theUnit, -1);
            if (theTo == NULL || theTo == theSite)   // no free mine of the kind: they wait
                break;
            inResources->SendToGather(theUnit, theTo, NULL);
            theOver--;
            theMoved++;
        }
    }
}

namespace {

// the newest gatherer of a resource that is gathering (not frozen, held or busy), or NULL
EEComplexUnit* MovableGatherer(EECPResourceManager* inResources, int inResource)
{
    std::deque<EEComplexUnit*>& theList = inResources->mGatherers[inResource];
    for (size_t k = theList.size(); k-- > 0;) {   // the newest first
        EEComplexUnit* theUnit = theList[k];
        if (theUnit == NULL || theUnit->mHitPoints <= 0 || theUnit->GetIsDisabled() || theUnit->mIsDetachedFromAI
            || theUnit->mCitizenBuildQueue.size() != 0)
            continue;
        EEUnit* theSite;
        if (IsGathering(theUnit, &theSite))
            return theUnit;
    }
    return NULL;
}

}

// WORKER REBALANCE (after every assignment pass, 10 seconds): the game's assignment only hands out idle citizens and never takes a
// gatherer back, so workers stay where an old target sent them. Move gathering citizens from the resource with the largest surplus
// to a stone, gold or iron mine that is short (wood too from the Copper Age on), with the mine search of the AI.
void Rebalance(EECPResourceManager* inResources)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    MineCrowds(inResources);   // the citizens beyond a mine's cap to free mines
    if (!theSettings.rebalance_workers)
        return;
    long theEpoch = thePlayer->mTechnologies != NULL ? thePlayer->mTechnologies->mCurrentEpoch : 0;
    int theMoves = 0, theDry = 0, theMost = (theSettings.mine_fill.fill_slots || theSettings.citizen_plan.mine_slots) ? 4 : 2;
    int theFirst = theSettings.late_targets.surplus_to_wood && theEpoch >= kFarmEpoch ? 3 : 4;
    for (int r = theFirst; r < 7 && theMoves < theMost; r++) {
        while (theMoves < theMost && (long)inResources->mWantedGatherers[r] - (long)inResources->mGatherers[r].size() >= 2) {
            long theCount[7];
            unsigned long theTarget[7];
            for (int t = 0; t < 7; t++) {
                theCount[t] = (long)inResources->mGatherers[t].size();
                theTarget[t] = inResources->mWantedGatherers[t];
            }
            int theDonor = Workers::Donor(theCount, theTarget, theDry, r);
            if (theDonor < 0)
                break;
            EEComplexUnit* theUnit = MovableGatherer(inResources, theDonor);
            if (theUnit == NULL) {
                theDry |= 1 << theDonor;
                continue;
            }
            EEUnit* theSite = r == 3 ? inResources->FindForestTarget(r, theUnit) : FindMine(inResources, r, theUnit, true);
            if (theSite == NULL)
                break;   // no free, safe mine of this resource
            size_t theBefore = inResources->mGatherers[r].size();
            inResources->SendToGather(theUnit, theSite, NULL);
            if (inResources->mGatherers[r].size() == theBefore) {
                theDry |= 1 << theDonor;
                continue;
            }
            theMoves++;
        }
    }
}

// WOOD DROP (wood_drops in mines.c, [ai_base] wood_drop): nothing in the game puts a drop site by the forest its woodcutters
// work. Every minute, while no Settlement is going up and the player can build one: the woodcutters whose tree lies 15 or
// more tiles from a drop site, the tree with most of them within 8 tiles; with 8 or more there a Settlement is laid at the
// placeable spot nearest the middle of their trees (2 to 12 tiles), no enemy building or soldiers near. Not ported: the
// reserve of the Settlement's cost that wood_reserve holds back from training (the training budget is not ported), and the
// build manager's placement bookkeeping of econ.c hook_place (the retries of dropped placements).
void WoodDrops(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (!SettingsOf(inPlayer).wood_drop || (sLast[idx] != 0 && inNow >= sLast[idx] && inNow - sLast[idx] < 60000))
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mResourceManager == NULL || theCivilization->mBuildManager == NULL)
        return;
    DropsCached(inPlayer);
    if (sDrops[idx].mDrops.empty())
        return;
    struct Spot { long mX, mY; };
    std::vector<Spot> theForests;
    std::deque<EEComplexUnit*>& theWoodcutters = theCivilization->mResourceManager->mGatherers[3];
    for (size_t k = 0; k < theWoodcutters.size() && theForests.size() < 1024; k++) {
        EEUnit* theSite = NULL;
        if (theWoodcutters[k] == NULL || !IsGathering(theWoodcutters[k], &theSite) || theSite == NULL)
            continue;
        if (DropNearest(idx, theSite->mTileX, theSite->mTileY, false, NULL) >= 15 * 15) {
            Spot theSpot = {theSite->mTileX, theSite->mTileY};
            theForests.push_back(theSpot);
        }
    }
    int theBest = -1, theCount = 0;
    for (size_t i = 0; i < theForests.size(); i++) {
        int theNear = 0;
        for (size_t j = 0; j < theForests.size(); j++) {
            long theDX = theForests[i].mX - theForests[j].mX, theDY = theForests[i].mY - theForests[j].mY;
            theNear += theDX * theDX + theDY * theDY <= 8 * 8;
        }
        if (theNear > theCount) {
            theCount = theNear;
            theBest = (int)i;
        }
    }
    if (theCount < 8)
        return;
    long theSumX = 0, theSumY = 0;
    for (size_t j = 0; j < theForests.size(); j++) {
        long theDX = theForests[theBest].mX - theForests[j].mX, theDY = theForests[theBest].mY - theForests[j].mY;
        if (theDX * theDX + theDY * theDY <= 8 * 8) {
            theSumX += theForests[j].mX;
            theSumY += theForests[j].mY;
        }
    }
    long theMidX = theSumX / theCount, theMidY = theSumY / theCount;
    long theEpoch = inPlayer->mTechnologies != NULL ? inPlayer->mTechnologies->mCurrentEpoch : -1;
    if (theEpoch < 0 || theEpoch >= 14 || gUnitClassTable[kUnitTypeSettlement] == NULL || theCivilization->mBuildManager->HasEnemyNear(theMidX, theMidY))
        return;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext))   // one going up: wait for it
        if (theUnit->mHitPoints > 0 && theUnit->mUnitClass != NULL && (long)theUnit->mUnitClass->mUnitType == kUnitTypeSettlement
            && (theUnit->mComplexUnit == NULL || theUnit->mComplexUnit->IsFoundation() || !theUnit->mComplexUnit->mIsBuilt))
            return;
    if (!inPlayer->CanProduceUnitType(kUnitTypeSettlement, 0x20, NULL))
        return;
    for (int r = 2; r <= 12; r++)
        for (int ax = -r; ax <= r; ax++)
            for (int ay = -r; ay <= r; ay++) {
                if (abs(ax) != r && abs(ay) != r)
                    continue;
                long theX = theMidX + ax, theY = theMidY + ay;
                if (!CanPlace(inPlayer, kUnitTypeSettlement, theX, theY) || RaidersNear(inPlayer, theX, theY, 12))
                    continue;
                theCivilization->mBuildManager->StartBuilding(EECPBuildRequest(kUnitTypeSettlement, U2DPoint<long>(theX, theY)));
                return;
            }
}


namespace {

FoodCounts sFood[16];

// food_bush (food.c): the free forage bush nearest to a drop site that takes food, within inMaxD2 of the base; NULL: none.
// The checks of the game's own search: explored, under the gatherer cap, the unit's land, no enemy near, no known danger.
EEUnit* FoodBush(EECPResourceManager* inResources, EEComplexUnit* inUnit, long inMaxD2)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    EECPBuildManager* theBuild = inResources->mManager->mBuildManager;
    if (theBuild == NULL)
        return NULL;
    long theBaseX = theBuild->mTownCenterTile.mX, theBaseY = theBuild->mTownCenterTile.mY;
    if (theBaseX == -10000 || theBaseY == -10000)
        return NULL;
    unsigned thePI = thePlayer->mPlayerID & 15;
    DropsCached(thePlayer);
    unsigned long theCap = (unsigned long)gWorldTable.GetLongValue(0x16);
    unsigned short theLand = inUnit != NULL ? gContinentManager.GetContinent(EETileCoordinate(inUnit->mTileX, inUnit->mTileY), 2) : 0;
    EEUnit* theBest = NULL;
    long theBestD = 0x7FFFFFFF;
    for (unsigned long z = 0; z < thePlayer->GetNumResourceCenters() && z < 256; z++) {
        EECPResourceCenter* theZone = thePlayer->GetResourceCenter(z);
        if (theZone == NULL)
            continue;
        for (size_t k = 0; k < theZone->mUnits.size() && k < 4096; k++) {
            EEUnit* theSite = theZone->mUnits[k];
            if (theSite == NULL || theSite->mHitPoints <= 0 || theSite->mUnitClass == NULL || theSite->mResourceType != 2
                || theSite->mUnitClass->mUnitType != kUnitTypeResForageBush || theSite->mResourceAmount <= 0)
                continue;
            if (theSite->mTile != NULL && theSite->mTile->mTerrainType == 1)
                continue;
            long theDX = theSite->mTileX - theBaseX, theDY = theSite->mTileY - theBaseY;
            if (theDX * theDX + theDY * theDY >= inMaxD2)
                continue;
            long theD = DropNearest(thePI, theSite->mTileX, theSite->mTileY, true, NULL);
            if (theD >= theBestD)
                continue;
            if (!ExploredBy(theSite, thePlayer))
                continue;
            if (gWorld.GetResourceUsers(theSite) >= theCap)
                continue;
            if (inUnit != NULL && gContinentManager.GetContinent(EETileCoordinate(theSite->mTileX, theSite->mTileY), 2) != theLand)
                continue;
            if (theBuild->HasEnemyNear(theSite->mTileX, theSite->mTileY) || SiteDanger(thePlayer, theSite, true, 2))
                continue;
            theBest = theSite;
            theBestD = theD;
        }
    }
    return theBest;
}

}


// HUNT NEAR (food_plan hunt_near, off in the shipped file; before the assignment): idle citizens hunt animals near a
// drop site that takes hunters (Town Center, Capitol, Settlement), within [ai_base] food_hunt_range tiles, before farms.
// The animal is the free one nearest to a drop site: food, an animal, gatherers below 3, the citizen's land, no enemy
// near, not a dangerous site. Upstream's food_hunt and food_animal (food.c). Upstream leaves tiger and wolf alone; this
// port does not resolve their type names, so the site's danger and the enemy-near checks stand in for that guard.
void FoodHunt(EECPResourceManager* inResources)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsOf(thePlayer);
    if (!theSettings.food_plan.hunt_near)
        return;
    long theEpoch = thePlayer->mTechnologies != NULL ? thePlayer->mTechnologies->mCurrentEpoch : -1;
    if (theEpoch >= kFarmEpoch)
        return;
    if (inResources->mWantedGatherers[kResourceFood] <= inResources->mGatherers[kResourceFood].size())
        return;

    EECPBuildManager* theBuild = inResources->mManager->mBuildManager;
    if (theBuild == NULL)
        return;
    unsigned thePI = thePlayer->mPlayerID & 15;
    DropsCached(thePlayer);
    const std::vector<Drop>& theDrops = sDrops[thePI].mDrops;
    long theRange = gSettingsAI[thePlayer->GetDifficulty()].food_hunt_range;
    if (theRange <= 0)
        theRange = 15;

    // the idle citizens, as AssignGatherers collects them: no build queue, on the map, with no goal or a waiting gather
    std::vector<EEComplexUnit*> theIdle;
    for (int j = 0; j < 7; j++) {
        std::deque<EEComplexUnit*>& theList = inResources->mGatherers[j];
        for (std::deque<EEComplexUnit*>::iterator it = theList.begin(); it != theList.end(); ++it) {
            EEComplexUnit* theUnit = *it;
            if (theUnit == NULL || theUnit->GetIsDisabled() || theUnit->mIsDetachedFromAI)
                continue;
            if (theUnit->GetUnitClass() == NULL || theUnit->GetUnitClass()->GetCategory() == kUnitCategoryNavalCitizen)
                continue;
            EEUnitGoal* theGoal = theUnit->GetLoadGoal();
            EEUGGather* theGather = dynamic_cast<EEUGGather*>(theGoal);
            if (theUnit->mCitizenBuildQueue.size() == 0 && theUnit->GetHitPoints() > 0 && theUnit->GetTileX() != -10000
                && theUnit->GetTileY() != -10000
                && (theGoal == NULL || (theGather != NULL && theGather->mWaiting && theUnit->mCurrentPhysicsAction != 0))) {
                // a citizen (not every complex unit that can gather)
                EEUnitClass* theClass = theUnit->GetUnitClass();
                if (theClass != NULL && IsCitizenUnit(theUnit))
                    theIdle.push_back(theUnit);
            }
        }
    }
    if (theIdle.empty())
        return;

    for (size_t n = 0; n < theIdle.size(); n++) {
        unsigned long theWanted = inResources->mWantedGatherers[kResourceFood];
        unsigned long theHave = inResources->mGatherers[kResourceFood].size();
        if (theWanted <= theHave)
            break;
        EEComplexUnit* theUnit = theIdle[n];
        if (theUnit == NULL)
            continue;
        // the unit may already have been moved by an earlier iteration (defensive)
        EEUGGather* theGoal = dynamic_cast<EEUGGather*>(theUnit->GetLoadGoal());
        if (!(theUnit->GetLoadGoal() == NULL || (theGoal != NULL && theGoal->mWaiting)))
            continue;

        long theRegion = (long)gContinentManager.GetContinentID(theUnit);
        EEUnit* theBest = NULL;
        long theBestD = theRange * theRange + 1;
        std::vector<EEUnit*> theNear;
        for (size_t i = 0; i < theDrops.size(); i++) {
            const Drop& theDrop = theDrops[i];
            if (theDrop.mFood)   // granaries do not take hunters
                continue;
            theNear.clear();
            GE2DPoint theCenter((float)theDrop.mX, (float)theDrop.mY);
            EEGetUnitsInRadius(theCenter, (float)theRange, theNear);
            for (size_t u = 0; u < theNear.size(); u++) {
                EEUnit* theAnimal = theNear[u];
                if (theAnimal == NULL || theAnimal->mHitPoints <= 0 || theAnimal->mUnitClass == NULL)
                    continue;
                if ((long)theAnimal->mResourceType != kResourceFood || theAnimal->mResourceAmount <= 0)
                    continue;
                if (theAnimal->mUnitClass->mFamily != kUnitFamilyAnimal)
                    continue;
                // predators are not excluded by type name in this port; the danger and enemy-near checks guard the citizen
                long theAX = theAnimal->mTileX, theAY = theAnimal->mTileY;
                long theDX = theAX - theDrop.mX, theDY = theAY - theDrop.mY;
                long theD2 = theDX * theDX + theDY * theDY;
                if (theD2 >= theBestD)
                    continue;
                if ((unsigned long)gWorld.GetResourceUsers(theAnimal) >= 3)
                    continue;
                if ((long)gContinentManager.GetContinentID(theAnimal) != theRegion)
                    continue;
                if (theBuild->HasEnemyNear(theAX, theAY))
                    continue;
                if (SiteDanger(thePlayer, theAnimal, true, 2))
                    continue;
                theBest = theAnimal;
                theBestD = theD2;
            }
        }
        if (theBest == NULL)
            continue;
        unsigned long theBefore = inResources->mGatherers[kResourceFood].size();
        inResources->SendToGather(theUnit, theBest, NULL);
        if (inResources->mGatherers[kResourceFood].size() == theBefore)
            continue;
    }
}

// WILD SETTLEMENT (food_plan wild_settlement, off in the shipped file; before farms): a Settlement by the richest wild
// food the town's drop sites are far from - every 2 minutes while none is queued or going up and the player can build
// one, at most 2 a game. Bushes and animals within 30 tiles of the base count as wild food; a spot 10 or more tiles
// from every food drop site with 1500 or more food within 6 tiles gets a Settlement placed 2-4 tiles from it. Upstream's
// food_camp (food.c). Upstream excludes tiger and wolf by type; this port does not resolve those type names.
void FoodCamp(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    static int sCount[16];
    unsigned theIdx = inPlayer->mPlayerID & 15;
    if (EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, inPlayer->GetDifficulty());
    if (!theSettings.food_plan.wild_settlement)
        return;
    long theEpoch = inPlayer->mTechnologies != NULL ? inPlayer->mTechnologies->mCurrentEpoch : -1;
    if (theEpoch >= kFarmEpoch)
        return;
    // a new game resets the per-player state (time goes backwards)
    if (inNow < sLast[theIdx])
        sLast[theIdx] = 0, sCount[theIdx] = 0;
    if (sCount[theIdx] >= 2 || (sLast[theIdx] != 0 && inNow - sLast[theIdx] < 120000))
        return;
    sLast[theIdx] = inNow != 0 ? inNow : 1;

    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL || theCivilization->mResourceManager == NULL)
        return;
    EECPBuildManager* theBuild = theCivilization->mBuildManager;
    long theBaseX = theBuild->mTownCenterTile.mX, theBaseY = theBuild->mTownCenterTile.mY;
    if (theBaseX == -10000)
        return;
    if (gUnitClassTable[kUnitTypeSettlement] == NULL)
        return;
    if (theBuild->IsQueued(kUnitTypeSettlement))
        return;
    // any Settlement going up: wait for it
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
            continue;
        if ((long)theUnit->mUnitClass->mUnitType != kUnitTypeSettlement)
            continue;
        if (theUnit->mComplexUnit == NULL || theUnit->mComplexUnit->IsFoundation() || !theUnit->mComplexUnit->mIsBuilt)
            return;
    }
    if (!inPlayer->CanProduceUnitType(kUnitTypeSettlement, 0x20, NULL))
        return;

    DropsCached(inPlayer);

    struct Food { long mX, mY, mAmount; };
    std::vector<Food> theFood;

    // animals within 30 tiles of the base
    {
        std::vector<EEUnit*> theNear;
        GE2DPoint theCenter((float)theBaseX, (float)theBaseY);
        EEGetUnitsInRadius(theCenter, 30.0f, theNear);
        for (size_t i = 0; i < theNear.size() && theFood.size() < 512; i++) {
            EEUnit* theAnimal = theNear[i];
            if (theAnimal == NULL || theAnimal->mHitPoints <= 0 || theAnimal->mUnitClass == NULL)
                continue;
            if ((long)theAnimal->mResourceType != kResourceFood || theAnimal->mResourceAmount <= 0)
                continue;
            if (theAnimal->mUnitClass->mFamily != kUnitFamilyAnimal)
                continue;
            Food theItem = {theAnimal->mTileX, theAnimal->mTileY, theAnimal->mResourceAmount};
            theFood.push_back(theItem);
        }
    }
    // forage bushes within 30 tiles of the base
    for (unsigned long z = 0; z < inPlayer->GetNumResourceCenters() && theFood.size() < 512; z++) {
        EECPResourceCenter* theZone = inPlayer->GetResourceCenter(z);
        if (theZone == NULL)
            continue;
        for (size_t k = 0; k < theZone->mUnits.size() && theFood.size() < 512; k++) {
            EEUnit* theSite = theZone->mUnits[k];
            if (theSite == NULL || theSite->mHitPoints <= 0 || theSite->mUnitClass == NULL || theSite->mResourceAmount <= 0)
                continue;
            if ((long)theSite->mResourceType != kResourceFood || theSite->mUnitClass->mUnitType != kUnitTypeResForageBush)
                continue;
            long theDX = theSite->mTileX - theBaseX, theDY = theSite->mTileY - theBaseY;
            if (theDX * theDX + theDY * theDY > 900)
                continue;
            if (!ExploredBy(theSite, inPlayer))
                continue;
            Food theItem = {theSite->mTileX, theSite->mTileY, theSite->mResourceAmount};
            theFood.push_back(theItem);
        }
    }

    // the richest wild food 10 or more tiles from every food drop site, with no enemy near
    int theBest = -1;
    long theBestFood = 1499;
    for (size_t i = 0; i < theFood.size(); i++) {
        if (DropNearest(theIdx, theFood[i].mX, theFood[i].mY, true, NULL) < 100)
            continue;
        if (theBuild->HasEnemyNear(theFood[i].mX, theFood[i].mY))
            continue;
        long theSum = 0;
        for (size_t j = 0; j < theFood.size(); j++) {
            long theDX = theFood[j].mX - theFood[i].mX, theDY = theFood[j].mY - theFood[i].mY;
            if (theDX * theDX + theDY * theDY <= 36)
                theSum += theFood[j].mAmount;
        }
        if (theSum > theBestFood) {
            theBestFood = theSum;
            theBest = (int)i;
        }
    }
    if (theBest < 0)
        return;

    // a placeable spot 2-4 tiles from it, without raiders near
    for (int r = 2; r <= 4; r++) {
        for (int ax = -r; ax <= r; ax++) {
            for (int ay = -r; ay <= r; ay++) {
                if (ax != -r && ax != r && ay != -r && ay != r)
                    continue;
                long theX = theFood[theBest].mX + ax, theY = theFood[theBest].mY + ay;
                if (!CanPlace(inPlayer, kUnitTypeSettlement, theX, theY) || RaidersNear(inPlayer, theX, theY, 12))
                    continue;
                theCivilization->mBuildManager->StartBuilding(EECPBuildRequest(kUnitTypeSettlement, U2DPoint<long>(theX, theY)));
                sCount[theIdx]++;
                return;
            }
        }
    }
}

// FOOD PULL (food_plan back_to_food, on by default; after every assignment pass and the rebalance): while food is 2 or
// more gatherers short of its target, up to 4 gatherers move to the nearest free forage bush, each from the resource
// farthest above its target
void FoodPull(EECPResourceManager* inResources)
{
    EEComputerPlayer* thePlayer = inResources->mManager->mPlayer;
    if (EngineOf(thePlayer->mPlayerID) != kEngineAgeOfAI || !SettingsOf(thePlayer).food_plan.back_to_food)
        return;
    long theReach = thePlayer->GetDropOffDistance();
    int theMoves = 0, theDry = 0;
    while (theMoves < 4 && (long)inResources->mWantedGatherers[2] - (long)inResources->mGatherers[2].size() >= 2) {
        long theCount[7];
        unsigned long theTarget[7];
        for (int t = 0; t < 7; t++) {
            theCount[t] = (long)inResources->mGatherers[t].size();
            theTarget[t] = inResources->mWantedGatherers[t];
        }
        int theDonor = Workers::FoodDonor(theCount, theTarget, theDry);
        if (theDonor < 0)
            break;
        EEComplexUnit* theUnit = MovableGatherer(inResources, theDonor);
        if (theUnit == NULL) {
            theDry |= 1 << theDonor;
            continue;
        }
        EEUnit* theSite = FoodBush(inResources, theUnit, theReach * theReach * 36);
        if (theSite == NULL) {   // no free, safe bush for this donor: try another
            theDry |= 1 << theDonor;
            continue;
        }
        size_t theBefore = inResources->mGatherers[2].size();
        inResources->SendToGather(theUnit, theSite, NULL);
        if (inResources->mGatherers[2].size() == theBefore) {
            theDry |= 1 << theDonor;
            continue;
        }
        sFood[thePlayer->mPlayerID & 15].mPulled++;
        theMoves++;
    }
}

FoodCounts FoodCountsOf(unsigned long inPlayerID)
{
    return sFood[inPlayerID & 15];
}

}
