// The outdated units' attack ([ai_base] outdated_attack, on by default). A unit type's tech has a last epoch: the game offers
// the type up to there, and a line upgrade turns a line's units into the next type. Units of a type whose last epoch is behind
// the player's are ones nothing upgrades again. Every 5 game minutes, when 8 or more such land units (class Land: soldiers and
// siege, not citizens) are in none of the player's attack subgroups and free for the combat manager, they attack as one of the
// game's own attack groups, as the scenario effect "Create AI Military Group" makes one. The target is the enemy building
// nearest their middle, a tower or fortress only when no other is left. EEComputerPlayer::Update calls it every tick.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/army.c
// (outdated_attack, ou_members, ou_outdated) and dll/ai/attack.c (ag_launch) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07,
// GPL-3.0. Not ported: the list of the units sent and ou_sortie, which only stage.c, cover.c and mass.c read (they keep a
// sortie's units from their staging, falling back and rallying); those files are not ported, so the units fight to the end
// as upstream intends. The log lines are not ported.
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
#include <algorithm>
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
#include "ee/EEUnitClassIDs.h"
#include "ee/EECPILocation.h"
#include "ee/EECPAGTask.h"
#include "ee/EECPUnitRequirement.h"
#include "ee/EECPCombatStrategy.h"
#include "ee/EECPCombatManager.h"
#include "ee/EECPAttackCommander.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackSubGroup.h"
#include "ee/EECPAttackSubGroupUnit.h"
#include "AgeOfAI.h"
#include "AgeOfAIArmy.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

OutdatedCounts sCounts[16];

bool IsCitizenClass(EEUnitClass* inClass)
{
    return (Roles::RolesOf(inClass->mCategory, inClass->mFamily, inClass->mRange) & Roles::kCitizen) != 0;
}

// The units in the player's attack subgroups. The combat manager's free test does not see them: right after a launch its
// units are all free again.
std::vector<EEUnit*> GroupMembers(EECPCombatManager* inCombat)
{
    std::vector<EEUnit*> theMembers;
    EECPAttackCommander* theCommander = inCombat->mAttackCommander;
    if (theCommander == NULL)
        return theMembers;
    for (size_t k = 0; k < theCommander->mGroups.size() && k < (size_t)ArmyRules::kGroupsMost; k++) {
        EECPAttackGroup* theGroup = theCommander->mGroups[k];
        if (theGroup == NULL)
            continue;
        for (EECPAttackSubGroup* theSubGroup : theGroup->mSubGroups) {
            if (theSubGroup == NULL)
                continue;
            for (EECPAttackSubGroupUnit* theRecord : theSubGroup->mUnits)
                if (theMembers.size() < (size_t)ArmyRules::kMembersMost && theRecord != NULL && theRecord->mUnit != NULL)
                    theMembers.push_back(theRecord->mUnit);
        }
    }
    return theMembers;
}

// A group of the game's own against inTarget, as the scenario effect "Create AI Military Group" makes one: task kind
// "attack the unit", the combat mode, active; strategy kind 4 (attack), no favourites. outOk: the combat manager took the
// task.
EECPAttackGroup* LaunchGroup(EEComputerPlayer* inPlayer, std::vector<long>& inUnitIDs, EEComplexUnit* inTarget, bool& outOk)
{
    EECPCombatManager* theCombat = inPlayer->mCombatManager;
    outOk = false;
    if (theCombat == NULL || theCombat->mAttackCommander == NULL || inUnitIDs.empty())
        return NULL;
    EECPAGTask theTask;
    EECPILocation theLocation;
    EECPCombatStrategy theStrategy;
    theTask.SetUnit(inTarget);
    theLocation.mTile = EETileLocation(inTarget->mTileX, inTarget->mTileY);
    theTask.mLocation = theLocation;
    theTask.mPlayer = gWorld.GetPlayer(inTarget->mPlayer->mPlayerID & 15);
    theTask.mType = kAGTaskUnit;
    theTask.mMode = kAGModeCombat;
    theTask.mIsActive = true;
    theStrategy.Reset();
    theStrategy.mKind = 4;
    theStrategy.mAddFavorites = false;
    outOk = theCombat->IsValidTask(theTask, theStrategy, inUnitIDs);
    return outOk ? theCombat->mAttackCommander->CreateGroupFromStrategy(theTask, theStrategy, inUnitIDs) : NULL;
}

}

void OutdatedAttack(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    static unsigned long sLast[16];
    unsigned idx = inPlayer->mPlayerID & 15;
    if (!gSettingsAI[kDifficultyMedium].outdated_attack)   // upstream reads it once, from [ai_base]
        return;
    if (inNow < sLast[idx]) {   // a new game
        sLast[idx] = 0;
        sCounts[idx] = OutdatedCounts();
    }
    if (sLast[idx] != 0 && inNow - sLast[idx] < ArmyRules::kOutdatedEvery)
        return;
    EECPCombatManager* theCombat = inPlayer->mCombatManager;
    EETechTree* theTree = inPlayer->mTechnologies;
    long theEpoch = theTree != NULL ? theTree->mCurrentEpoch : -1;
    if (theCombat == NULL || theTree == NULL || theEpoch < 0)
        return;
    sLast[idx] = inNow != 0 ? inNow : 1;
    std::vector<EEUnit*> theMembers = GroupMembers(theCombat);
    std::vector<long> theIDs;
    long theSumX = 0, theSumY = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        EEUnitClass* theClass = theUnit->mUnitClass;
        if (theClass == NULL || theUnit->mHitPoints <= 0 || theUnit->mTileX < 0)
            continue;
        EETechTreeNode* theTech = theTree->Get(theClass->mTechnology);
        if (theTech == NULL || !ArmyRules::IsOutdated(theClass->mCategory, IsCitizenClass(theClass), theTech->mObsoleteEpoch, theEpoch))
            continue;
        if (std::find(theMembers.begin(), theMembers.end(), theUnit) != theMembers.end() || theUnit->mComplexUnit == NULL
            || !theCombat->IsMilitaryUnit(theUnit->mComplexUnit))
            continue;
        if (theIDs.size() == (size_t)ArmyRules::kOutdatedMost)
            continue;
        theIDs.push_back((long)theUnit->mUnitID);
        theSumX += theUnit->mTileX;
        theSumY += theUnit->mTileY;
    }
    if (theIDs.size() < (size_t)ArmyRules::kOutdatedLeast)
        return;
    long theN = (long)theIDs.size(), theMidX = theSumX / theN, theMidY = theSumY / theN;
    ArmyRules::TargetChoice<EEUnit> theChoice;
    for (unsigned long p = 0; p < gWorld.mNumPlayers && p < 16; p++) {
        EEPlayer* theOwner = gWorld.mPlayers[p];
        if (theOwner == NULL || !IsEnemyOf(inPlayer, theOwner) || theOwner->mObserverMode)   // mObserverMode: out of the game
            continue;
        for (EEUnit* theUnit = theOwner->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            EEUnitClass* theClass = theUnit->mUnitClass;
            if (theClass == NULL || theUnit->mHitPoints <= 0
                || (theClass->mCategory != kUnitCategoryBuilding && theClass->mCategory != kUnitCategoryFortress)
                || (Roles::RolesOf(theClass->mCategory, theClass->mFamily, theClass->mRange) & Roles::kWall))
                continue;
            float theRange, theAttack;
            long theDX = theUnit->mTileX - theMidX, theDY = theUnit->mTileY - theMidY;
            bool theDefence = theClass->mCategory == kUnitCategoryFortress || DefenceStats(theUnit, theRange, theAttack);
            theChoice.Offer(theUnit, theDX * theDX + theDY * theDY, theDefence);
        }
    }
    EEComplexUnit* theTarget = dynamic_cast<EEComplexUnit*>(theChoice.Chosen());
    if (theTarget == NULL)
        return;
    bool theOk = false;
    OutdatedCounts& theCounts = sCounts[idx];
    if (LaunchGroup(inPlayer, theIDs, theTarget, theOk) != NULL) {
        theCounts.mSorties++;
        theCounts.mSent += theIDs.size();
    } else if (theOk)
        theCounts.mNoGroup++;
    else
        theCounts.mRefused++;
}

OutdatedCounts OutdatedCountsOf(unsigned long inPlayerID)
{
    return sCounts[inPlayerID & 15];
}

}
