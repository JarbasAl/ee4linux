// FALL BACK, hospital bit ([ai_base] fall_back=hospital, shipped on). Grille's cover.c sends an expensive land unit
// below fall_back_hp of its hit points to the nearest own Hospital on its land, and holds it there until it heals to
// 90%. The game's own hospital regeneration does the healing; this rule only walks the unit there and back. The other
// fall_back bits (tower_cover, fleet_cover, give_up) need the fight estimate, landing and attack-group layers this port
// does not have, so only the hospital bit is ported.
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
#include "AgeOfAIHospitalRules.h"

namespace AgeOfAI {

namespace {

const unsigned long kHospitalEvery = 2000;
const int kHospitalHoldN = 64;

struct HospitalHold {
    EEUnit* mUnit;
    unsigned long mUnitID;
    long mX, mY;
    unsigned long mSince, mAt;
};
struct HospitalState {
    unsigned long mLast;
    HospitalHold mHold[kHospitalHoldN];
    int mNHold;
};
static HospitalState sHospital[16];
static HospitalCounts sHospitalCounts[16];

void HospitalMoveTo(EEComplexUnit* inUnit, float inX, float inY)
{
    EEUGLocation* theGoal = new EEUGLocation(GE2DPoint(inX, inY), true, true, false, 0.0f, false, 0);
    inUnit->SetGoal(theGoal, false, 0);
    inUnit->StartAction(kActionPrepareToMove);
    inUnit->mUnitClass->ActionChanged(inUnit);
}

bool HospitalFinished(EEUnit* inUnit)
{
    return inUnit->mComplexUnit != NULL && !inUnit->mComplexUnit->IsFoundation() && inUnit->mComplexUnit->mIsBuilt;
}

unsigned short HospitalLand(long inX, long inY)
{
    return inX < 0 || inY < 0 ? 0 : gContinentManager.GetContinent(EETileCoordinate(inX, inY), 2);
}

long HospitalValue(EEUnit* inUnit)
{
    if (inUnit->mUnitClass == NULL)
        return 0;
    EETechTreeNode* theNode = inUnit->GetTechTreeNode();
    if (theNode == NULL)
        return 0;
    long theValue = 0;
    for (long r = kResourceFood; r <= kResourceIron; r++) {
        long theCost = theNode->GetCost(r, inUnit->mUnitClass->GetUnitType(), 1.0f);
        if (theCost > 0)
            theValue += theCost;
    }
    return theValue;
}

bool Held(EEUnit* inUnit, const HospitalState& inState)
{
    for (int k = 0; k < inState.mNHold; k++)
        if (inState.mHold[k].mUnitID == inUnit->mUnitID)
            return true;
    return false;
}

}  // namespace

void HospitalTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    const SettingsAI& theSettings = SettingsAIOf(inPlayer->mPlayerID, AgeOfAI::kDifficultyMedium);
    if (!theSettings.fall_back.hospital)
        return;
    unsigned idx = inPlayer->mPlayerID & 15;
    HospitalState& theState = sHospital[idx];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
        std::memset(&sHospitalCounts[idx], 0, sizeof(sHospitalCounts[idx]));
    }
    if (theState.mLast != 0 && inNow >= theState.mLast && inNow - theState.mLast < kHospitalEvery)
        return;
    theState.mLast = inNow != 0 ? inNow : 1;
    sHospitalCounts[idx].mChecks++;

    long theHX[16], theHY[16];
    int theNH = 0;
    for (EEUnit* u = inPlayer->mFirstUnit; u != NULL && theNH < 16; u = static_cast<EEUnit*>(u->mNext)) {
        if (u->mHitPoints <= 0 || u->mUnitClass == NULL || u->mUnitClass->GetUnitType() != kUnitTypeHospital
            || !HospitalFinished(u))
            continue;
        theHX[theNH] = u->mTileX;
        theHY[theNH] = u->mTileY;
        theNH++;
    }
    sHospitalCounts[idx].mHospitals += theNH;

    for (int k = theState.mNHold - 1; k >= 0; k--) {
        HospitalHold& theHold = theState.mHold[k];
        EEUnit* theUnit = theHold.mUnit;
        bool theGo = theUnit == NULL || theUnit->mHitPoints <= 0 || theUnit->mUnitID != theHold.mUnitID
            || theUnit->mPlayer != inPlayer;
        if (!theGo) {
            bool theNear = false;
            for (int j = 0; j < theNH; j++) {
                long theDX = theHX[j] - theHold.mX, theDY = theHY[j] - theHold.mY;
                if (theDX * theDX + theDY * theDY <= 9)
                    theNear = true;
            }
            long theMax = theUnit->mStats != NULL ? theUnit->mStats->mMaxHitPoints : 0;
            if (theNear && theMax > 0 && theUnit->mHitPoints * 10 >= theMax * 9)
                sHospitalCounts[idx].mDone++;
            theGo = HospitalRules::HospitalRelease(theUnit->mHitPoints, theMax, theNear, inNow, theHold.mSince);
        }
        if (theGo) {
            theState.mHold[k] = theState.mHold[--theState.mNHold];
            continue;
        }
        EEUnitGoal* theGoal = theUnit->mComplexUnit != NULL ? theUnit->mComplexUnit->mGoalState : NULL;
        EEUGLocation* theLocation = dynamic_cast<EEUGLocation*>(theGoal);
        if (theLocation != NULL && std::fabs(theLocation->mLocation.GetX() - theHold.mX) < 2
            && std::fabs(theLocation->mLocation.GetY() - theHold.mY) < 2)
            continue;
        HospitalMoveTo(theUnit->mComplexUnit, (float)theHold.mX + 0.5f, (float)theHold.mY + 0.5f);
        sHospitalCounts[idx].mHolds++;
    }

    if (theNH == 0)
        return;
    long theLimit = (long)theSettings.fall_back_dist * theSettings.fall_back_dist;
    for (EEUnit* u = inPlayer->mFirstUnit; u != NULL; u = static_cast<EEUnit*>(u->mNext)) {
        if (u->mHitPoints <= 0 || u->mUnitClass == NULL || u->mUnitClass->mCategory != kUnitCategoryLand
            || u->mUnitClass->mAttack <= 0 || u->mTileX < 0 || u->mComplexUnit == NULL)
            continue;
        if (Held(u, theState))
            continue;
        long theMax = u->mStats != NULL ? u->mStats->mMaxHitPoints : 0;
        if (!HospitalRules::NeedsHospital(u->mHitPoints, theMax, theSettings.fall_back_hp))
            continue;
        if (HospitalValue(u) < theSettings.fall_back_value)
            continue;
        unsigned short theLand = HospitalLand(u->mTileX, u->mTileY);
        if (theLand == 0)
            continue;
        int theBest = -1;
        long theBestD = theLimit + 1;
        for (int j = 0; j < theNH; j++) {
            long theDX = theHX[j] - u->mTileX, theDY = theHY[j] - u->mTileY;
            long theD = theDX * theDX + theDY * theDY;
            if (theD < theBestD && HospitalLand(theHX[j], theHY[j]) == theLand
                && !RaidersNear(inPlayer, theHX[j], theHY[j], 1)) {
                theBestD = theD;
                theBest = j;
            }
        }
        if (theBest < 0 || theState.mNHold >= kHospitalHoldN)
            continue;
        long theX = 0, theY = 0;
        HospitalRules::HospitalTarget(u->mTileX, u->mTileY, theHX[theBest], theHY[theBest], theX, theY);
        HospitalHold& theHold = theState.mHold[theState.mNHold++];
        theHold.mUnit = u;
        theHold.mUnitID = u->mUnitID;
        theHold.mX = theX;
        theHold.mY = theY;
        theHold.mSince = inNow != 0 ? inNow : 1;
        theHold.mAt = 0;
        HospitalMoveTo(u->mComplexUnit, (float)theX + 0.5f, (float)theY + 0.5f);
        sHospitalCounts[idx].mSent++;
    }
}

HospitalCounts HospitalCountsOf(unsigned long inPlayerID)
{
    return inPlayerID < 16 ? sHospitalCounts[inPlayerID] : HospitalCounts();
}

}  // namespace AgeOfAI
