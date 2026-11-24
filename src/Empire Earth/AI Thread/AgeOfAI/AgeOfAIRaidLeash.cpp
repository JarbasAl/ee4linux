// RAID LEASH ([ai_base] raid_leash, off in the shipped file). Upstream's force attacking a spot keeps its members
// with it until they reach the destination, and lets a member that chases farther than raid_leash tiles past that spot
// go home and wait for the force. The engine's attack-move follows the target, so without the leash a member can
// chase across the map. This port reads the attack groups' location tasks once a game second and calls a member home
// when it is past the limit. Its task tile stands in for upstream's raid destination, and the subgroup's rally point
// stands in for the force's home.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/ai/armies.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. The pure rule is AgeOfAIRaidLeashRules.h.
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
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEUnitBehavior.h"
#include "ee/EECPCombatManager.h"
#include "ee/EEUnitGoal.h"
#include "ee/EECPAttackCommander.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackSubGroup.h"
#include "ee/EECPAttackSubGroupUnit.h"
#include "ee/EECPAGTask.h"
#include "ee/EECPILocation.h"
#include "ee/EEUnitAction.h"
#include "AgeOfAI/AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAIRaidLeashRules.h"

namespace AgeOfAI {
namespace {

using namespace AgeOfAI::RaidLeashRules;

const unsigned long kEveryMs = 1000;
RaidLeashCounts sCounts[16];
unsigned long sLast[16];

bool IsOnMap(long inX, long inY)
{
    return inX >= 0 && inY >= 0 && inX != -10000 && inY != -10000;
}

}  // namespace

void RaidLeashTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL)
        return;
    const long theLeash = SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium).raid_leash;
    if (theLeash <= 0)
        return;
    const unsigned theIndex = inPlayer->mPlayerID & 15;
    if (inNow < sLast[theIndex]) {   // a new game
        memset(sCounts, 0, sizeof sCounts);
        memset(sLast, 0, sizeof sLast);
    }
    if (sLast[theIndex] != 0 && inNow - sLast[theIndex] < kEveryMs)
        return;
    sLast[theIndex] = inNow != 0 ? inNow : 1;

    EECPCombatManager* theCombat = inPlayer->mCombatManager;
    if (theCombat == NULL || theCombat->mAttackCommander == NULL)
        return;
    RaidLeashCounts& theCounts = sCounts[inPlayer->mPlayerID & 15];
    EECPAttackCommander* theCommander = theCombat->mAttackCommander;
    for (size_t g = 0; g < theCommander->mGroups.size(); g++) {
        EECPAttackGroup* theGroup = theCommander->mGroups[g];
        if (theGroup == NULL || theGroup->IsSpent())
            continue;
        for (size_t s = 0; s < theGroup->mSubGroups.size(); s++) {
            EECPAttackSubGroup* theSubGroup = theGroup->mSubGroups[s];
            if (theSubGroup == NULL || !theSubGroup->mTask.mIsActive || theSubGroup->mTask.mPlayer == NULL
                || !IsRaidTask(theSubGroup->mTask.mType))
                continue;
            const EETileLocation& theDestination = theSubGroup->mTask.mLocation.GetTile();
            const EETileLocation theHome = theSubGroup->GetRallyPoint(true);
            if (!IsOnMap(theDestination.GetX(), theDestination.GetY()) || !IsOnMap(theHome.GetX(), theHome.GetY()))
                continue;
            theCounts.mGroups++;
            for (size_t k = 0; k < theSubGroup->mUnits.size(); k++) {
                EECPAttackSubGroupUnit* theRecord = theSubGroup->mUnits[k];
                if (theRecord == NULL)
                    continue;
                EEComplexUnit* theUnit = theRecord->GetUnit();
                if (theUnit == NULL || theUnit->mHitPoints <= 0
                    || !IsOnMap(theUnit->mTileX, theUnit->mTileY))
                    continue;
                theCounts.mChecks++;
                const long theDX = theUnit->mTileX - theDestination.GetX();
                const long theDY = theUnit->mTileY - theDestination.GetY();
                if (!ShouldCall(true, true, !theRecord->mIsOutOfFight, theDX, theDY, theLeash))
                    continue;
                theRecord->mIsOutOfFight = true;
                const GE3DPoint thePoint(theHome.GetX() + 0.5f, theHome.GetY() + 0.5f, 0.0f);
                EEUGLocation* theGoal = new EEUGLocation(&thePoint, true, false, false, 0.0f, false, false);
                if (!theSubGroup->GiveGoal(theUnit, theGoal, false, kActionPrepareToMove) && theGoal != NULL)
                    delete theGoal;
                theCounts.mCalled++;
            }
        }
    }
}

RaidLeashCounts RaidLeashCountsOf(unsigned long inPlayerID)
{
    return sCounts[inPlayerID & 15];
}

}  // namespace AgeOfAI
