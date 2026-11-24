// ANSWER WAR ([ai_base] answer_war, default 1). Grille's diplo.c answers a player that turns hostile while the AI still
// sees it as an ally: a few seconds later the AI sets the same enemy stance. The rest of diplo.c - opinion, persona,
// history, tribute, peace and surrender - needs a diplomacy state engine this port does not have. This pass keeps only
// the stance answer and its first-look rule, so an AI does not answer a war that was already under way when it started.
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
#include "ee/EECPAttackCommander.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackSubGroup.h"
#include "AgeOfAI/AgeOfAIStats.h"
#include "AgeOfAI.h"
#include "AgeOfAIDefence.h"
#include "AgeOfAIHome.h"
#include "AgeOfAITowers.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"
#include "AgeOfAIDiplomacy.h"

// The reduced answer-war rules of Age of AI. The whole diplomacy state engine (opinion, persona, history, tribute,
// peace, surrender) is not ported: this pass keeps only the stance answer and its first-look rule.

namespace AgeOfAI {

namespace {

struct AnswerWarState {
    unsigned long mLast;
    bool mSeen;
    long mPrev[16];
    unsigned long mDue[16];
};
static AnswerWarState sAnswerWar[16];
static AnswerWarCounts sAnswerWarCounts[16];

}  // namespace

void AnswerWarTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    AnswerWarState& theState = sAnswerWar[idx];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
        std::memset(&sAnswerWarCounts[idx], 0, sizeof(sAnswerWarCounts[idx]));
    }
    const bool theFirst = !theState.mSeen;
    const unsigned long theAI = inPlayer->mPlayerID;
    const SettingsAI& theSettings = SettingsAIOf(theAI, AgeOfAI::kDifficultyMedium);

    for (unsigned long j = 1; j < gWorld.GetNumPlayers() && j < 16; j++) {
        EEPlayer* theOther = gWorld.GetPlayer(j);
        if (theOther == NULL || j == theAI)
            continue;
        const long theTheir = theOther->GetEnemy(theAI);
        const long theMine = inPlayer->GetEnemy(j);
        if (theFirst) {
            theState.mPrev[j] = theTheir;
            continue;
        }
        sAnswerWarCounts[idx].mChecks++;

        if (theSettings.answer_war
            && DiplomacyRules::AnswerWarTriggered(theTheir, theState.mPrev[j], theMine)) {
            sAnswerWarCounts[idx].mTransitions++;
            sAnswerWarCounts[idx].mScheduled++;
            theState.mDue[j] = inNow + DiplomacyRules::AnswerWarDelay(theAI, j);
        }
        theState.mPrev[j] = theTheir;

        if (theState.mDue[j] == 0 || inNow < theState.mDue[j])
            continue;
        theState.mDue[j] = 0;
        if (!DiplomacyRules::AnswerWarDue(theTheir, theMine, theOther->GetGameResult())) {
            sAnswerWarCounts[idx].mCancelled++;
            continue;
        }
        gWorld.SetDiplomaticStance(theAI, j, 1);
        sAnswerWarCounts[idx].mAnswers++;
    }

    if (theFirst)
        theState.mSeen = true;
    theState.mLast = inNow != 0 ? inNow : 1;
}

AnswerWarCounts AnswerWarCountsOf(unsigned long inPlayerID)
{
    return inPlayerID < 16 ? sAnswerWarCounts[inPlayerID] : AnswerWarCounts();
}

}  // namespace AgeOfAI
