// The early game of an Age of AI player: the first towers of the town by its centre, and the citizens that lead raiders under
// them (AgeOfAIEarlyGeometry.h for the places).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/early.c
// (early_towers_tick, early_bait, early_core_spot) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. The mod's
// raiders_near and defence-layout direction are not ported yet: no raider memory, and the front is the map's centre.
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
#include "ee/EEUnitUtilities.h"
#include "ee/EEAreaUtilities.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEUGLocation.h"
#include "ee/EECPUnitManager.h"
#include "ee/EECPBuildManager.h"
#include "ee/EECPCivilizationManager.h"
#include "AgeOfAI.h"
#include "AgeOfAIEarlyGeometry.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

struct EarlyState {
    unsigned long mTowerTime, mBaitTime;
    struct { int mX, mY; unsigned long mTime; } mPending[Early::kCoreTowers];   // core spots handed out recently
    unsigned long mQueued, mPlaced, mBuilders, mBaits;
} sEarly[16];

// the guard towers, oldest first
const long kTowerLine[7] = {kUnitTypeGuardTowerPaleo, kUnitTypeGuardTowerCopper, kUnitTypeGuardTowerBronze,
                            kUnitTypeGuardTowerMiddle, kUnitTypeGuardTowerImperial, kUnitTypeGuardTowerWW1,
                            kUnitTypeGuardTowerDigital};

long Epoch(EEPlayer* inPlayer)
{
    return inPlayer->mTechnologies != NULL ? inPlayer->mTechnologies->mCurrentEpoch : -1;
}

bool CanBuildClass(EEPlayer* inPlayer, long inClass)
{
    if (inClass < 0 || inClass >= 871 || gUnitClassTable[inClass] == NULL || inPlayer->mTechnologies == NULL)
        return false;
    EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[inClass]->mTechnology);
    return theNode != NULL && theNode->GetIsAvailable();
}

// the best tower the player can build now; 0: none
long NewestTower(EEPlayer* inPlayer)
{
    for (int i = 6; i >= 0; i--)
        if (CanBuildClass(inPlayer, kTowerLine[i]))
            return kTowerLine[i];
    return 0;
}

bool TowerQueued(EECPBuildManager* inBuild)
{
    for (int i = 0; i < 7; i++)
        if (inBuild->IsQueued(kTowerLine[i]))
            return true;
    return false;
}

bool IsTower(EEUnit* inUnit)
{
    EEUnitClass* theClass = inUnit->mUnitClass;
    return theClass != NULL && (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kTower) != 0;
}

bool IsFinished(EEUnit* inUnit)
{
    return inUnit->mComplexUnit != NULL && !inUnit->mComplexUnit->IsFoundation() && inUnit->mComplexUnit->mIsBuilt;
}

// the player's towers within inRadius tiles (a square) of (inX, inY): finished, and all (foundations too)
void TowersAt(EEPlayer* inPlayer, long inX, long inY, long inRadius, int& outFinished, int& outAll)
{
    outFinished = outAll = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || !IsTower(theUnit))
            continue;
        if (labs(theUnit->mTileX - inX) > inRadius || labs(theUnit->mTileY - inY) > inRadius)
            continue;
        outAll++;
        outFinished += IsFinished(theUnit) ? 1 : 0;
    }
}

bool PlacedOK(EEPlayer* inPlayer, long inType, long inX, long inY)
{
    long theMap = gWorld.mMap != NULL ? (long)gWorld.mMap->mSize : 0;
    long theSize = gUnitClassTable[inType]->mTileSize < 1 ? 1 : gUnitClassTable[inType]->mTileSize;
    if (inX < 2 || inY < 2 || inX >= theMap - 2 || inY >= theMap - 2)
        return false;
    for (long x = 0; x < theSize; x++)
        for (long y = 0; y < theSize; y++)
            if (!EECanPlaceOnTile(inPlayer, inType, inX + x, inY + y, false, 0))
                return false;
    return true;
}

// the core spot of slot inSlot as a ring around the geometry's tile, or false
bool CoreSpot(EEComputerPlayer* inPlayer, long inType, long inBaseX, long inBaseY, int& outX, int& outY)
{
    EarlyState& theState = sEarly[inPlayer->mPlayerID & 15];
    unsigned long theNow = gPhysics->mGameTime;
    float theDX, theDY;
    Early::FrontToCentre((int)inBaseX, (int)inBaseY, gWorld.mMap != NULL ? (int)gWorld.mMap->mSize : 0, theDX, theDY);
    float theSumX = 0, theSumY = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL)
            continue;
        EEUnitClass* theClass = theUnit->mUnitClass;
        if (!(Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kCitizen))
            continue;
        if (labs(theUnit->mTileX - inBaseX) > 15 || labs(theUnit->mTileY - inBaseY) > 15)
            continue;
        theSumX += (float)(theUnit->mTileX - inBaseX);
        theSumY += (float)(theUnit->mTileY - inBaseY);
    }
    for (int k = 0; k < Early::kCoreTowers; k++) {
        int theX, theY, theFinished, theAll;
        Early::CoreSlot((int)inBaseX, (int)inBaseY, theDX, theDY, k, theSumX, theSumY, theX, theY);
        TowersAt(inPlayer, theX, theY, 2, theFinished, theAll);
        if (theAll != 0)
            continue;
        if (theState.mPending[k].mTime != 0 && theNow >= theState.mPending[k].mTime && theNow - theState.mPending[k].mTime < 90000)
            continue;
        for (int r = 0; r <= 2; r++)
            for (int ax = -r; ax <= r; ax++)
                for (int ay = -r; ay <= r; ay++) {
                    if ((abs(ax) != r && abs(ay) != r) || !PlacedOK(inPlayer, inType, theX + ax, theY + ay))
                        continue;
                    outX = theX + ax;
                    outY = theY + ay;
                    theState.mPending[k].mX = outX;
                    theState.mPending[k].mY = outY;
                    theState.mPending[k].mTime = theNow ? theNow : 1;
                    theState.mPlaced++;
                    return true;
                }
    }
    return false;
}

long CountBuilders(EECPUnitManager* inUnits, EEUnit* inTower)
{
    std::vector<EEComplexUnit*> theCitizens;
    inUnits->GetCitizens(theCitizens);
    long theCount = 0;
    for (size_t i = 0; i < theCitizens.size(); i++) {
        EEUGBuildRepair* theGoal = dynamic_cast<EEUGBuildRepair*>(theCitizens[i]->mGoalState);
        if (theGoal != NULL && theGoal->mTarget == inTower)
            theCount++;
    }
    return theCount;
}

// the first towers of the town: two by the town centre in the first two epochs, built by more hands
void EarlyTowers(EEComputerPlayer* inPlayer, const SettingsAI& inSettings, unsigned long inNow)
{
    EarlyState& theState = sEarly[inPlayer->mPlayerID & 15];
    long theEpoch = Epoch(inPlayer);
    if (!inSettings.early_towers || theEpoch > 1 || (theState.mTowerTime != 0 && inNow >= theState.mTowerTime && inNow - theState.mTowerTime < 10000))
        return;
    theState.mTowerTime = inNow ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL)
        return;
    EECPBuildManager* theBuild = theCivilization->mBuildManager;
    long theBaseX = theBuild->mTownCenterTile.mX, theBaseY = theBuild->mTownCenterTile.mY;
    if (theBaseX == -10000)
        return;
    long theType = NewestTower(inPlayer);
    int theFinished, theAll, theX, theY;
    TowersAt(inPlayer, theBaseX, theBaseY, Early::kCoreDistance + 3, theFinished, theAll);
    if (theType != 0 && theAll < Early::kCoreTowers && !TowerQueued(theBuild) && theEpoch >= 0 && theEpoch < 14) {
        EETechTreeNode* theNode = inPlayer->mTechnologies->Get(gUnitClassTable[theType]->mTechnology);
        long theCost = theNode != NULL ? theNode->GetCost(kResourceStone, theType, 1.0f) : 0;
        if (inPlayer->mResourceCaps[kResourceStone] >= theCost && CoreSpot(inPlayer, theType, theBaseX, theBaseY, theX, theY)) {
            theBuild->QueueBuildingAt(theType, theX, theY);
            theState.mQueued++;
        }
    }
    // two builders for every unfinished tower near the town (the game's own and the defence layout's too)
    EEUnit* theTodo[8];
    int theCount = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL && theCount < 8; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || !IsTower(theUnit) || IsFinished(theUnit))
            continue;
        if (labs(theUnit->mTileX - theBaseX) > 20 || labs(theUnit->mTileY - theBaseY) > 20)
            continue;
        theTodo[theCount++] = theUnit;
    }
    EECPUnitManager* theUnits = theCivilization->mUnitManager;
    for (int i = 0; i < theCount && theUnits != NULL; i++)
        for (int n = (int)CountBuilders(theUnits, theTodo[i]); n < 2; n++) {
            EEComplexUnit* theWorker = theUnits->FindBuilderForTarget(theTodo[i], theTodo[i]->mUnitClass->mUnitType);
            if (theWorker == NULL)
                break;
            EEUGBuildRepair* theGoal = new EEUGBuildRepair(inPlayer, theTodo[i], false);
            theWorker->ClearGoalQueue(0);
            theWorker->SetGoal(theGoal, 0, 0);
            theWorker->StartAction(theGoal->GetStartAction());
            theWorker->mUnitClass->ActionChanged(theWorker);
            theState.mBuilders++;
        }
}

// seen by the player: its count above the owner's floor, or hitting one of its units; the game's view with honest_vision off
bool SeenBy(EEUnit* inUnit, EEPlayer* inViewer, const SettingsAI& inSettings)
{
    unsigned long theCount = inUnit->mTile != NULL ? inUnit->mTile->GetSightCount(inViewer->mPlayerID) : 0;
    if (!inSettings.honest_vision)
        return theCount != 0;
    const Intel::Memory& theMemory = MemoryOf(inViewer->mPlayerID);
    unsigned long theOwner = inUnit->mPlayer != NULL ? inUnit->mPlayer->mPlayerID : 0;
    return theCount > theMemory.Floor((int)theOwner) || theMemory.IsRevealed(inUnit->mUnitID);
}

// A citizen with an enemy soldier within 5 tiles (or one attacking it), outside its own towers' reach, walks to the nearest
// finished tower within 25 tiles and stands 1.5 tiles behind it from the enemy, so the raiders that follow come under it.
// Up to the Bronze Age, every 2 seconds.
void TowerBait(EEComputerPlayer* inPlayer, const SettingsAI& inSettings, unsigned long inNow)
{
    EarlyState& theState = sEarly[inPlayer->mPlayerID & 15];
    if (!inSettings.tower_bait || Epoch(inPlayer) > 3 || (theState.mBaitTime != 0 && inNow >= theState.mBaitTime && inNow - theState.mBaitTime < 2000))
        return;
    theState.mBaitTime = inNow ? inNow : 1;
    EECPCivilizationManager* theCivilization = inPlayer->mCivilizationManager;
    if (theCivilization == NULL || theCivilization->mBuildManager == NULL)
        return;
    long theBaseX = theCivilization->mBuildManager->mTownCenterTile.mX, theBaseY = theCivilization->mBuildManager->mTownCenterTile.mY;
    if (theBaseX == -10000)
        return;
    struct Cover { long mX, mY; float mRange; } theCover[24];
    struct Enemy { long mX, mY; EEUnit* mTarget; } theEnemies[64];
    int theCovers = 0, theEnemyCount = 0;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(i);
        if (thePlayer == NULL)
            continue;
        bool theOwn = thePlayer == inPlayer, theEnemy = !inPlayer->IsAlly(thePlayer);
        if (!theOwn && !theEnemy)
            continue;
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || labs(theUnit->mTileX - theBaseX) > 36 || labs(theUnit->mTileY - theBaseY) > 36)
                continue;
            EEUnitClass* theClass = theUnit->mUnitClass;
            if (theOwn) {
                float theRange = theUnit->mStats != NULL ? theUnit->mStats->mRange : theClass->mRange;
                float theAttack = theUnit->mStats != NULL ? theUnit->mStats->mAttack : (float)theClass->mAttack;
                if (theCovers < 24 && Roles::IsBuildingCategory(theClass->mCategory) && theAttack > 0 && theRange > 1.0f && theRange <= 30.0f && IsFinished(theUnit)) {
                    theCover[theCovers].mX = theUnit->mTileX;
                    theCover[theCovers].mY = theUnit->mTileY;
                    theCover[theCovers++].mRange = theRange;
                }
            } else if (theEnemyCount < 64 && Roles::IsSoldier(theClass->mCategory, theClass->mFamily) && SeenBy(theUnit, inPlayer, inSettings)) {
                EEUnitGoal* theGoal = theUnit->mComplexUnit != NULL ? theUnit->mComplexUnit->mGoalState : NULL;
                theEnemies[theEnemyCount].mX = theUnit->mTileX;
                theEnemies[theEnemyCount].mY = theUnit->mTileY;
                theEnemies[theEnemyCount++].mTarget = theGoal != NULL && theGoal->mKind == kGoalAttackUnit ? static_cast<EEUGAttackUnit*>(theGoal)->mTarget : NULL;
            }
        }
    }
    if (theCovers == 0 || theEnemyCount == 0)
        return;
    unsigned long theSent = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        EEUnitClass* theClass = theUnit->mUnitClass;
        if (theUnit->mHitPoints <= 0 || theClass == NULL || theUnit->mComplexUnit == NULL || theUnit->mTileX == -10000 ||
            !(Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kCitizen) ||
            labs(theUnit->mTileX - theBaseX) > 32 || labs(theUnit->mTileY - theBaseY) > 32)
            continue;
        int theHit = -1;
        for (int k = 0; k < theEnemyCount && theHit < 0; k++)
            if (theEnemies[k].mTarget == theUnit || (labs(theEnemies[k].mX - theUnit->mTileX) <= 5 && labs(theEnemies[k].mY - theUnit->mTileY) <= 5))
                theHit = k;
        if (theHit < 0)
            continue;
        int theBest = -1;
        float theBestDistance = 25.0f * 25.0f;
        for (int c = 0; c < theCovers; c++) {
            float theDX = (float)(theCover[c].mX - theUnit->mTileX), theDY = (float)(theCover[c].mY - theUnit->mTileY);
            float theDistance = theDX * theDX + theDY * theDY;
            if (theDistance <= (theCover[c].mRange - 0.5f) * (theCover[c].mRange - 0.5f)) {   // covered already: the tower fights
                theBest = -2;
                break;
            }
            if (theDistance < theBestDistance) {
                theBestDistance = theDistance;
                theBest = c;
            }
        }
        if (theBest < 0)
            continue;
        EEUnitGoal* theGoal = theUnit->mComplexUnit->mGoalState;
        if (theGoal != NULL && theGoal->mKind == kGoalAttackUnit)   // the militia's: a fight it wins
            continue;
        float theX, theY;
        Early::BaitSpot((int)theCover[theBest].mX, (int)theCover[theBest].mY, (int)theEnemies[theHit].mX, (int)theEnemies[theHit].mY, theX, theY);
        if (theGoal != NULL && theGoal->mKind == kGoalLocation &&
            fabsf(static_cast<EEUGLocation*>(theGoal)->mLocation.GetX() - theX) < 2 && fabsf(static_cast<EEUGLocation*>(theGoal)->mLocation.GetY() - theY) < 2)
            continue;   // on its way there
        EEUGLocation* theLocation = new EEUGLocation(GE2DPoint(theX, theY), true, true, false, 0.0f, false, 0);
        theUnit->mComplexUnit->SetGoal(theLocation, false, 0);
        theUnit->mComplexUnit->StartAction(kActionPrepareToMove);
        theUnit->mUnitClass->ActionChanged(theUnit->mComplexUnit);
        theSent++;
    }
    theState.mBaits += theSent;
}

}

EarlyCounts EarlyCountsOf(unsigned long inPlayerID)
{
    const EarlyState& theState = sEarly[inPlayerID & 15];
    EarlyCounts theCounts = {theState.mQueued, theState.mPlaced, theState.mBuilders, theState.mBaits};
    return theCounts;
}

// The first towers of the town and the bait under them, every update of an Age of AI player (docs/ageofai.md "The early game").
void TickEarly(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    unsigned long theDifficulty = inPlayer->GetDifficulty() > 2 ? 1 : inPlayer->GetDifficulty();
    const SettingsAI& theSettings = gSettingsAI[theDifficulty];
    EarlyTowers(inPlayer, theSettings, inNow);
    TowerBait(inPlayer, theSettings, inNow);
}

}
