// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAPlaceGate.cpp (file name inferred: no __FILE__; RTTI class
// EESEAPlaceGate; its ctor/??_G are emitted by EESEUnit.cpp). The gate placement action: 0x1a9112..0x1a9650
// (Virtual4, the entry ctor COMDAT, Virtual0C, the vector<EESEAPlaceGateEntry> COMDATs). After EESEAPaintTerrain.cpp,
// before EESEAPlaceUnit.cpp.
#include <windows.h>
#include <mbstring.h>
#include <stdio.h>
#include <algorithm>
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
#include "lle/UIEditBox.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GETransformation.h"
#include "lle/GETerrainMesh.h"
#include <vector>
#include <deque>

#include "ee/EmpireEarth.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEMap.h"
#include "ee/EEPlayer.h"
#include "ee/EEHumanPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEAction.h"
#include "ee/EEUIManager.h"
#include "ee/EEUserInterface.h"
#include "ee/EEGraphics.h"
#include "ee/RTSGraphics.h"
#include "ee/EEMiniMap.h"
#include "ee/EEUIMiniMap.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEScenarioManager.h"
#include "ee/EEEnvironmentManager.h"
#include "ee/EEGameOptions.h"
#include "lle/UIButton.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EERandomMapClass.h"
#include "ee/EERandomMapGenerators.h"
#include "ee/EETileCoordinate.h"
#include "ee/EEGhostUnit.h"
#include "ee/EEUIFScenarioEditor.h"
#include "ee/EEUIFSEMainGroup.h"
#include "ee/EESEUnit.h"
#include "ee/EESEAPlaceUnit.h"

// FUNCTION: EXE 0x5a9112
// Builds each queued gate (EEBuildGate) and releases its unit media, then restarts placing the unit tab's
// selected unit class
void EESEAPlaceGate::Virtual4()
{
    EESEAPlaceGateEntry theEntry;
    while (this->mTiles.size() != 0) {
        theEntry = this->mTiles.back();
        EEPlayer* thePlayer = gWorld.mPlayers[theEntry.mPlayer];
        EEBuildGate(theEntry.mTile, theEntry.mPlayer, theEntry.mTechnology, theEntry.mDirection);
        thePlayer->UnloadUnitClassMedia(gTechTree.Get(theEntry.mTechnology)->mData->mUnitClass);
        this->mTiles.pop_back();
    }
    static_cast<EESEUnit*>(gUIManager->mScenarioEditorForm->mTabGroups[kEditorTabUnits])->SelectUnitType(GE2DPoint(0.0f, 0.0f));
}

// FUNCTION: EXE 0x5a91b8
// Creates an empty gate placement entry
EESEAPlaceGateEntry::EESEAPlaceGateEntry()
{
    this->mTile.mX = 0;
    this->mTile.mY = 0;
    this->mPlayer = 0;
    this->mTechnology = 0;
    this->mDirection = 0;
}

// FUNCTION: EXE 0x5a91cb
// If every ghost can be placed, loads the gate's media, queues the gate (tile, player, technology, direction)
// and posts the action; returns whether it did
bool EESEAPlaceGate::Place(const EETileCoordinate& inTile, long inPlayer, long inTechnology, long inDirection)
{
    EESEAPlaceGateEntry theEntry;
    bool thePlaced = true;
    gWorld.mGhostUnitsLock->Enter();
    unsigned long i = 0;
    while (true) {
        if (!(i < gWorld.GetNumGhostUnits()))
            break;
        thePlaced = gWorld.mGhostUnits[i]->mCanPlace;
        i++;
        if (!thePlaced)
            break;
    }
    if (thePlaced) {
        gWorld.mPlayers[inPlayer]->SetUnitClassMedia(gTechTree.Get(inTechnology)->mData->mUnitClass, 0x7f);
        theEntry.mTile = inTile;
        theEntry.mPlayer = inPlayer;
        theEntry.mTechnology = inTechnology;
        theEntry.mDirection = inDirection;
        this->mTiles.push_back(theEntry);
        gUserInterface->PostAction(this);
    }
    gWorld.mGhostUnitsLock->Leave();
    return thePlaced;
}

// FUNCTION: EXE 0x5a9299
// Appends a gate entry (vector<EESEAPlaceGateEntry>::push_back)

// FUNCTION: EXE 0x5a92c5
// Inserts one gate entry (vector<EESEAPlaceGateEntry>::insert(P, X))

// FUNCTION: EXE 0x5a931f
// Inserts copies of a gate entry, growing the storage if needed (vector<EESEAPlaceGateEntry>::insert(P, M, X))

// FUNCTION: EXE 0x5a95d6
// Copies a gate entry (EESEAPlaceGateEntry's implicit operator=; ICF-shared with
// pair<EEPathHistoryEntry, unsigned long>'s ctor)

// FUNCTION: EXE 0x5a95fb
// Copies a range of gate entries (std::copy)

// FUNCTION: EXE 0x5a9624
// Copy-constructs a range of gate entries into raw storage (vector<EESEAPlaceGateEntry>::_Ucopy)
