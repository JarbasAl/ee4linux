// ANSWERING FIRE ([general] answer_fire, upstream's game value off in the shipped file). An idle fighter shot from beyond
// its reply range goes for the enemy whose attack goal aims at it. The enemy must be alive on the map within 24 tiles,
// seen by the fighter's player, hurt-able by its weapon and on the same land or water area. The answer is the fighter's
// own goal, so an order is never kept waiting behind it. Upstream runs the scan at the game's safe point for every
// player's units (all) or the computer players' units (computers). This port scans the players the setting names from
// the AI update, once every 500 game milliseconds; an Age of AI computer player must be present to drive it.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/engine/unitsfirst.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. The pure rule is
// AgeOfAIAnswerFireRules.h.
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
#include "ee/EECPUnitManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGLoadGarrison.h"
#include "ee/EEResourceType.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEUnitBehavior.h"
#include "ee/EECPCombatManager.h"
#include "AgeOfAI/AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAIAnswerFireRules.h"

extern EEToHitTable gToHitTable;   // 0x5318e8

namespace AgeOfAI {
namespace {

using namespace AgeOfAI::AnswerFireRules;

AnswerFireCounts sCounts[16];
unsigned long sLast = 0;   // one pass for all players every kEveryMs

// The connected area at a tile; zero off the map. A helicopter needs none (af_reachable).
unsigned short AreaOf(long inX, long inY, long inKind)
{
    if (inX < 0 || inY < 0 || inKind == kFlyingArea)
        return 0;
    return gContinentManager.GetContinent(EETileCoordinate(inX, inY), inKind);
}

// The nearest enemy whose attack goal aims at the defender and that the defender may answer (answer_fire_tick's
// checks on the attacker e). The search stays inside the tile reach.
EEUnit* PickAttacker(EEUnit* inDefender, unsigned long inPlayerID, unsigned long inCategory)
{
    if (inDefender->mUnitClass == NULL)
        return NULL;
    const long theKind = AreaKind(inCategory);
    const unsigned short theArea = AreaOf(inDefender->mTileX, inDefender->mTileY, theKind);
    GE2DPoint theCenter((float)inDefender->mTileX, (float)inDefender->mTileY);
    std::vector<EEUnit*> theNear;
    EEGetUnitsInRadius(theCenter, (float)kReach, theNear);
    EEUnit* theBest = NULL;
    long theBestD2 = -1;
    for (size_t i = 0; i < theNear.size(); i++) {
        EEUnit* theEnemy = theNear[i];
        if (theEnemy == NULL || theEnemy == inDefender || theEnemy->mHitPoints <= 0 || theEnemy->mTileX < 0
            || theEnemy->mUnitClass == NULL || theEnemy->mPlayer == NULL || theEnemy->mComplexUnit == NULL)
            continue;
        if (!IsEnemyOf(inDefender->mPlayer, theEnemy->mPlayer))
            continue;
        EEUnitGoal* theGoal = theEnemy->mComplexUnit->mGoalState;
        if (theGoal == NULL || theGoal->GetKind() != kGoalAttackUnit)
            continue;
        if (static_cast<EEUGAttackUnit*>(theGoal)->GetTarget() != inDefender)
            continue;
        const long theDX = theEnemy->mTileX - inDefender->mTileX;
        const long theDY = theEnemy->mTileY - inDefender->mTileY;
        if (theBestD2 >= 0 && theDX * theDX + theDY * theDY >= theBestD2)
            continue;
        if (!CanAnswer(theDX, theDY, theEnemy->mHitPoints, theEnemy->mTileX,
                       theEnemy->mTile != NULL && theEnemy->mTile->GetSightCount(inPlayerID) != 0,
                       gToHitTable.GetAttackFactor(inDefender->mUnitClass->mAmmoType, theEnemy->mUnitClass,
                                                   inPlayerID, theEnemy->mPlayer->mPlayerID) > 0.0f,
                       Reachable(inCategory, theArea,
                                 AreaOf(theEnemy->mTileX, theEnemy->mTileY, theKind))))
            continue;
        theBest = theEnemy;
        theBestD2 = theDX * theDX + theDY * theDY;
    }
    return theBest;
}

}  // namespace

void AnswerFireTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL)
        return;
    const long theMode = GeneralOf(inPlayer->mPlayerID).answer_fire;
    if (theMode == kGame)
        return;
    if (inNow < sLast) {                               // a new game
        memset(sCounts, 0, sizeof sCounts);
        sLast = 0;
    }
    if (sLast != 0 && inNow - sLast < kEveryMs)
        return;
    sLast = inNow != 0 ? inNow : 1;

    for (unsigned long p = 0; p < gWorld.mNumPlayers && p < 16; p++) {
        EEPlayer* theOwner = gWorld.mPlayers[p];
        if (theOwner == NULL || !AppliesTo(theMode, theOwner->IsHuman()))
            continue;
        AnswerFireCounts& theCounts = sCounts[p & 15];
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL;
             theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL
                || theUnit->mComplexUnit->mGoalState != NULL)
                continue;
            const unsigned long theCommand = theUnit->mComplexUnit->GetCommandType();
            if (!ShouldAnswer((long)theCommand, false, theUnit->mHitPoints, theUnit->mUnitClass->mCategory,
                              (float)theUnit->mUnitClass->mAttack))
                continue;
            theCounts.mChecks++;
            EEUnit* theEnemy = PickAttacker(theUnit, theOwner->mPlayerID, theUnit->mUnitClass->mCategory);
            if (theEnemy == NULL)
                continue;
            EEUGAttackUnit* theGoal = new EEUGAttackUnit(theUnit->mComplexUnit, theEnemy, false, false);
            theUnit->mComplexUnit->SetGoal(theGoal, false, 0);
            theUnit->mComplexUnit->StartAction(kPrepareToMove);
            theUnit->mUnitClass->ActionChanged(theUnit->mComplexUnit);
            theCounts.mSent++;
        }
    }
}

AnswerFireCounts AnswerFireCountsOf(unsigned long inPlayerID)
{
    return sCounts[inPlayerID & 15];
}

}  // namespace AgeOfAI
