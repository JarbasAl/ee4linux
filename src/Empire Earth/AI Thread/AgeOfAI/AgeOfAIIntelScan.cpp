// An Age of AI player's view of the game for its memory (AgeOfAIIntel.h): reads every unit into a Scan, keeps a memory
// per player, and takes a scan every kScanEvery game ms.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/intel.c
// (intel_unit, intel_tick) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
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
#include "AgeOfAI.h"
#include "AgeOfAIIntel.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

Intel::Memory sMemories[16];

Intel::Kind KindOf(EEUnitClass* inClass)
{
    if (Roles::IsBuildingCategory(inClass->mCategory))
        return Intel::kBuilding;
    return Roles::IsSoldier(inClass->mCategory, inClass->mFamily) ? Intel::kSoldier : Intel::kOther;
}

// The attack a unit of the memory is judged by: a building's when it shoots at the land (range from 1 to 30), a
// soldier's per hit point
float AttackOf(EEUnit* inUnit, Intel::Kind inKind)
{
    float theAttack = inUnit->mStats != NULL ? inUnit->mStats->mAttack : (float)inUnit->mUnitClass->mAttack;
    if (!(theAttack > 0.0f))
        return 0.0f;
    if (inKind == Intel::kBuilding) {
        float theRange = inUnit->mStats != NULL ? inUnit->mStats->mRange : inUnit->mUnitClass->mRange;
        return theRange > 1.0f && theRange <= 30.0f ? theAttack : 0.0f;
    }
    return theAttack;
}

bool IsUsable(EEUnit* inUnit)
{
    return inUnit->mHitPoints > 0 && inUnit->mTileX >= 0 && inUnit->mUnitClass != NULL;
}

Intel::Scan ScanFor(EEPlayer* inViewer, unsigned inNow, bool inRevealAttackers)
{
    Intel::Scan theScan;
    theScan.mNow = inNow;
    std::vector<unsigned long> theHitting;                        // the enemies that hit the viewer's units
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(i);
        if (thePlayer == NULL)
            continue;
        bool theEnemy = !inViewer->IsAlly(thePlayer);
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (!IsUsable(theUnit))
                continue;
            unsigned long theCategory = theUnit->mUnitClass->mCategory;
            if (theCategory == kUnitCategoryAmbient || theCategory == kUnitCategoryResource || theCategory == kUnitCategoryProjectile)
                continue;
            theScan.mLive.push_back(theUnit->mUnitID);
            if (thePlayer == inViewer && theUnit->mComplexUnit != NULL) {
                EEComplexUnit* theAttacker = theUnit->mComplexUnit->mLastAttacker;
                if (theAttacker != NULL && theAttacker->mPlayer != NULL && !inViewer->IsAlly(theAttacker->mPlayer)
                    && theAttacker->mHitPoints > 0 && theAttacker->mTileX >= 0)
                    theHitting.push_back(theAttacker->mUnitID);
            }
            if (!theEnemy) {
                Intel::Eye theEye = {(int)theUnit->mTileX, (int)theUnit->mTileY};
                theScan.mEyes.push_back(theEye);
                continue;
            }
            Intel::Sighting theSighting = {};
            theSighting.mUnit = theUnit->mUnitID;
            theSighting.mOwner = (int)i;
            theSighting.mType = (int)theUnit->mUnitClass->mUnitType;
            theSighting.mKind = KindOf(theUnit->mUnitClass);
            theSighting.mX = (int)theUnit->mTileX;
            theSighting.mY = (int)theUnit->mTileY;
            theSighting.mHitPoints = (int)theUnit->mHitPoints;
            theSighting.mAttack = AttackOf(theUnit, theSighting.mKind);
            theSighting.mCount = theUnit->mTile != NULL ? (unsigned)theUnit->mTile->GetVisibility(inViewer->mPlayerID) : 0;
            theScan.mEnemies.push_back(theSighting);
        }
    }
    std::sort(theScan.mLive.begin(), theScan.mLive.end());
    std::sort(theHitting.begin(), theHitting.end());
    for (Intel::Sighting& theSighting : theScan.mEnemies)
        theSighting.mRevealed = inRevealAttackers
            && std::binary_search(theHitting.begin(), theHitting.end(), theSighting.mUnit);
    return theScan;
}

}

const Intel::Memory& MemoryOf(unsigned long inPlayerID)
{
    return sMemories[inPlayerID & 15];
}

void TickIntel(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    unsigned long theDifficulty = inPlayer->GetDifficulty() > 2 ? 1 : inPlayer->GetDifficulty();
    if (!gSettingsAI[theDifficulty].honest_vision)
        return;
    Intel::Memory& theMemory = sMemories[inPlayer->mPlayerID & 15];
    if (!theMemory.Due((unsigned)inNow))
        return;
    theMemory.Update(ScanFor(inPlayer, (unsigned)inNow, gSettingsAI[theDifficulty].reveal_attackers != 0));
}

}
