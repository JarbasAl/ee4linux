// SURRENDER ([ai_base] surrender, default 1; not in scenarios). Grille's diplo.c resigns an AI that has no citizens
// left and cannot rebuild: when it cannot train one, or after three minutes at war. This reduced pass keeps that
// decision and its timing. It does not choose whom to surrender to, weigh army power, split blocks or pay tribute:
// the game's own defeat result ends the AI, as upstream's deferred resign does.
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

// SURRENDER (diplo.c, [ai_base] surrender on by default). Upstream resigns an AI that has no citizens left and cannot
// rebuild: after a minute with no way to train one, or three minutes in any case while at war. This reduced pass keeps
// that decision and its timing. It does not choose whom to surrender to, weigh the army's power, split blocks, or pay
// tribute: the game's own defeat result (EEPlayer::SetGameResult(2)) ends the AI, as upstream's deferred resign does.
#include "AgeOfAI/AgeOfAISurrenderRules.h"

namespace AgeOfAI {

namespace {

struct SurrenderState {
    unsigned long mLast, mNoCitizenSince;
};
static SurrenderState sSurrender[16];
static SurrenderCounts sSurrenderCounts[16];

// A finished town centre can train another citizen; a foundation or an unfinished one cannot. Upstream also refuses
// when the stock cannot pay for a citizen; that resource test is left out here.
bool CanTrainCitizen(EEPlayer* inPlayer)
{
    if (inPlayer == NULL)
        return false;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mUnitClass == NULL || theUnit->mUnitClass->mUnitType != kUnitTypeTownCenter
            && theUnit->mUnitClass->mUnitType != kUnitTypeCapitol
            && theUnit->mUnitClass->mUnitType != kUnitTypeSettlement)
            continue;
        EEComplexUnit* theComplex = theUnit->mComplexUnit;
        if (theComplex != NULL && !theComplex->IsFoundation() && theComplex->mIsBuilt)
            return true;
    }
    return false;
}

bool HasWar(EEPlayer* inPlayer)
{
    if (inPlayer == NULL)
        return false;
    for (unsigned long j = 1; j < gWorld.GetNumPlayers() && j < 16; j++) {
        if (j == inPlayer->mPlayerID)
            continue;
        EEPlayer* theOther = gWorld.GetPlayer(j);
        if (theOther == NULL || theOther->GetGameResult() == 2)
            continue;
        if (inPlayer->GetEnemy(j) != 0)
            return true;
    }
    return false;
}

}  // namespace

void SurrenderTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    SurrenderState& theState = sSurrender[idx];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
        std::memset(&sSurrenderCounts[idx], 0, sizeof(sSurrenderCounts[idx]));
    }
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, AgeOfAI::kDifficultyMedium);
    // The AoA tick clears mIsScenarioGame when scenario_ai is set, so a normal game runs this rule. A real scenario
    // with scenario_ai off keeps the flag and stays out of it, as upstream's !scenario_game() requires.
    if (!theSettings.surrender || gWorld.mGameOptions.mIsScenarioGame || inPlayer->GetGameResult() == 2)
        return;

    long theCitizens = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mUnitClass == NULL)
            continue;
        unsigned long theCategory = theUnit->mUnitClass->mCategory;
        if (theCategory == kUnitCategoryLandCitizen || theCategory == kUnitCategoryNavalCitizen)
            theCitizens++;
    }
    sSurrenderCounts[idx].mChecks++;
    if (theCitizens > 0) {
        theState.mNoCitizenSince = 0;
        sSurrenderCounts[idx].mCitizens++;
        theState.mLast = inNow != 0 ? inNow : 1;
        return;
    }
    sSurrenderCounts[idx].mNoCitizen++;
    if (theState.mNoCitizenSince == 0)
        theState.mNoCitizenSince = inNow != 0 ? inNow : 1;
    unsigned long theMs = inNow >= theState.mNoCitizenSince ? inNow - theState.mNoCitizenSince : 0;
    const bool theCanTrain = CanTrainCitizen(inPlayer), theWar = HasWar(inPlayer);
    if (theCanTrain)
        sSurrenderCounts[idx].mCanTrain++;
    if (theWar)
        sSurrenderCounts[idx].mAtWar++;
    if (SurrenderRules::ShouldResign(theMs, theCanTrain, theWar)) {
        sSurrenderCounts[idx].mResigned++;
        inPlayer->SetGameResult(2);
    }
    theState.mLast = inNow != 0 ? inNow : 1;
}

SurrenderCounts SurrenderCountsOf(unsigned long inPlayerID)
{
    return inPlayerID < 16 ? sSurrenderCounts[inPlayerID] : SurrenderCounts();
}

}  // namespace AgeOfAI
