// STUCK-TRANSPORT WATCHDOG ([ai_base] transport_watchdog, off in the shipped file). A transport that holds one script
// state on one tile with a goal or cargo aboard for long enough is freed: a load goal waiting for passengers that
// are not coming lets them go after 30 seconds, and a loaded transport idle with its unload goal is set to
// PrepareToMove after 20 seconds, at most three times. Upstream scans every transport on the map once a second and
// watches only the AI players' transports. This port scans the players' unit lists from the AI update once a second.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/ai/transports.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. The pure rule is
// AgeOfAITransportWatchdogRules.h.
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
#include "ee/EEUGLoadContainer.h"
#include "ee/EEUGUnloadTransport.h"
#include "AgeOfAI/AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAITransportWatchdogRules.h"

namespace AgeOfAI {
namespace {

using namespace AgeOfAI::TransportWatchdogRules;

const int kWatched = 128;

struct Watch {
    EEComplexUnit* mUnit;
    unsigned long mState;
    long mX, mY;
    unsigned long mSince, mLastSeen;
    int mKicks;
};
Watch sWatches[kWatched];
int sCount;
unsigned long sLast = 0;   // one pass for all players every kScanEveryMs
TransportWatchdogCounts sCounts[16];

Watch* FindWatch(EEComplexUnit* inUnit)
{
    for (int i = 0; i < sCount; i++)
        if (sWatches[i].mUnit == inUnit)
            return &sWatches[i];
    if (sCount >= kWatched)
        return NULL;
    Watch* theWatch = &sWatches[sCount++];
    memset(theWatch, 0, sizeof *theWatch);
    return theWatch;
}

// A passenger's relationship to the waiting transport's load goal (tw_passenger). The board goal's container is
// compared with the transport's complex unit: a goal for a garrison or another ship is another order.
int ClassifyPassenger(EEComplexUnit* inTransport, EEComplexUnit* inPassenger)
{
    if (inPassenger == NULL || inPassenger->mHitPoints <= 0 || inPassenger->mUnitClass == NULL || inPassenger->mTileX < 0)
        return kGone;
    EEUnitGoal* theGoal = inPassenger->mGoalState;
    if (theGoal == NULL)
        return kNoGoal;
    if (theGoal->GetKind() != kGoalLoadTransport)
        return kOtherOrder;
    if (static_cast<EEUGLoadContainer*>(theGoal)->mContainer != inTransport)
        return kOtherOrder;
    return PassengerState(inPassenger->mHitPoints, inPassenger->mTileX, true, true, true,
                          inPassenger->GetCommandType());
}

// The waiting load goal lets go of the passengers that are not coming (tw_dog's first step). A copy is made first:
// RemoveUnit erases from the goal's pending list.
void DropPending(EEComplexUnit* inTransport, TransportWatchdogCounts& ioCounts)
{
    EEUnitGoal* theGoal = inTransport->mGoalState;
    if (theGoal == NULL || theGoal->GetKind() != kGoalLoadTransport)
        return;
    EEUGLoadContainer* theLoad = static_cast<EEUGLoadContainer*>(theGoal);
    std::vector<EEComplexUnit*> theDrop;
    for (std::vector<EEComplexUnit*>::iterator it = theLoad->mUnits.begin(); it != theLoad->mUnits.end(); ++it) {
        int theState = ClassifyPassenger(inTransport, *it);
        if (ShouldDrop(theState) && theDrop.size() < 64)
            theDrop.push_back(*it);
    }
    for (size_t i = 0; i < theDrop.size(); i++)
        theLoad->RemoveUnit(theDrop[i]);
    ioCounts.mDrops += (unsigned long)theDrop.size();
}

}  // namespace

void TransportWatchdogTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || !SettingsAIOf(inPlayer->mPlayerID, kDifficultyMedium).transport_watchdog)
        return;
    if (inNow < sLast) {   // a new game
        memset(sCounts, 0, sizeof sCounts);
        sCount = 0;
        sLast = 0;
    }
    if (sLast != 0 && inNow - sLast < kScanEveryMs)
        return;
    sLast = inNow != 0 ? inNow : 1;

    for (int i = 0; i < sCount; i++)
        sWatches[i].mLastSeen = 0;

    for (unsigned long p = 0; p < gWorld.mNumPlayers && p < 16; p++) {
        EEPlayer* theOwner = gWorld.mPlayers[p];
        if (theOwner == NULL || theOwner->IsHuman())
            continue;
        TransportWatchdogCounts& theCounts = sCounts[p & 15];
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL;
             theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mComplexUnit == NULL
                || theUnit->mTileX < 0 || !IsTransport(theUnit->mUnitClass->mCategory))
                continue;
            EEComplexUnit* theComplex = theUnit->mComplexUnit;
            Watch* theWatch = FindWatch(theComplex);
            if (theWatch == NULL)
                continue;
            theWatch->mLastSeen = inNow;
            const unsigned long theState = theComplex->GetCommandType();
            const long theX = theUnit->mTileX, theY = theUnit->mTileY;
            if (theWatch->mUnit != theComplex) {
                theWatch->mUnit = theComplex;
                theWatch->mState = theState;
                theWatch->mX = theX;
                theWatch->mY = theY;
                theWatch->mSince = inNow;
                theWatch->mKicks = 0;
                continue;
            }
            theCounts.mChecks++;
            if (theWatch->mState != theState || theWatch->mX != theX || theWatch->mY != theY) {
                theWatch->mState = theState;
                theWatch->mX = theX;
                theWatch->mY = theY;
                theWatch->mSince = inNow;
                theWatch->mKicks = 0;
                continue;
            }
            EEUnitGoal* theGoal = theComplex->mGoalState;
            const bool theHasLoad = theGoal != NULL && theGoal->GetKind() == kGoalLoadTransport;
            const bool theHasUnload = theGoal != NULL && theGoal->GetKind() == kGoalUnloadTransport;
            const unsigned long theCargo = theComplex->GetNumCargoUnits();
            if (!theHasLoad && !theHasUnload && theCargo == 0) {
                theWatch->mSince = inNow;   // idle and empty: not a stall
                continue;
            }
            if (!IsStall(theState, theX, theY, theWatch->mState, theWatch->mX, theWatch->mY, theGoal != NULL, theCargo))
                continue;
            const unsigned long theElapsed = inNow >= theWatch->mSince ? inNow - theWatch->mSince : 0;
            if (ShouldDropPassengers(theState, theHasLoad, theElapsed))
                DropPending(theComplex, theCounts);
            if (ShouldKick(theState, theHasUnload, theCargo, theElapsed, theWatch->mKicks)) {
                theComplex->StartAction(kPrepareToMove);
                theWatch->mKicks++;
                theCounts.mKicks++;
            }
        }
    }

    int theKept = 0;
    for (int i = 0; i < sCount; i++)
        if (sWatches[i].mLastSeen == inNow)
            sWatches[theKept++] = sWatches[i];
    sCount = theKept;
}

TransportWatchdogCounts TransportWatchdogCountsOf(unsigned long inPlayerID)
{
    return sCounts[inPlayerID & 15];
}

}  // namespace AgeOfAI
