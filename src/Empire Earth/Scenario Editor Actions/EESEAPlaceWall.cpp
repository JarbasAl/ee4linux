// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAPlaceWall.cpp (file name inferred: no __FILE__; RTTI class
// EESEAPlaceWall; its ctor/??_G are emitted by EESEUnit.cpp). The wall placement action: 0x1aa0d1..0x1aa70e (Virtual4,
// the entry ctor, Virtual0C, the vector<EESEAPlaceWallEntry> COMDATs). After EESEAPlaceUnit.cpp, before
// EESEAQuickSaveLoad.cpp.
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
#include "ee/EESEAPlaceUnit.h"

// FUNCTION: EXE 0x5aa0d1
// Lays out each queued wall line for its player (EEPlanWall) and releases the wall's unit media
void EESEAPlaceWall::Virtual4()
{
    std::vector<EEUnit*> theWalls;
    EESEAPlaceWallEntry theEntry;
    while (this->mTiles.size() != 0) {
        theEntry = this->mTiles.back();
        EEPlayer* thePlayer = gWorld.mPlayers[theEntry.mPlayer];
        EEUnitClass* theClass = gUnitClassTable[thePlayer->mTechnologies->GetData(theEntry.mTechnology)->mUnitClass];
        EEPlanWall(theEntry.mStart, theEntry.mEnd, theClass, thePlayer, 0, 1, theWalls);
        thePlayer->UnloadUnitClassMedia(gTechTree.GetData(theEntry.mTechnology)->mUnitClass);
        this->mTiles.pop_back();
    }
}

// FUNCTION: EXE 0x5aa1a9
// Creates an empty wall placement entry
EESEAPlaceWallEntry::EESEAPlaceWallEntry()
    : mPlayer(0), mTechnology(0)
{
}

// FUNCTION: EXE 0x5aa1c7
// Loads the wall's media, queues a wall from the start to the end point and posts the action; returns true
bool EESEAPlaceWall::Place(const GE2DPoint& inStart, const GE2DPoint& inEnd, long inPlayer, long inTechnology)
{
    EESEAPlaceWallEntry theEntry;
    gWorld.mPlayers[inPlayer]->SetUnitClassMedia(gTechTree.GetData(inTechnology)->mUnitClass, 0x7f);
    theEntry.mStart = inStart;
    theEntry.mEnd = inEnd;
    theEntry.mPlayer = inPlayer;
    theEntry.mTechnology = inTechnology;
    this->mTiles.push_back(theEntry);
    gUserInterface->PostAction(this);
    return true;
}

// FUNCTION: EXE 0x5aa242
// Appends a wall entry (vector<EESEAPlaceWallEntry>::push_back)

// FUNCTION: EXE 0x5aa26e
// Removes the last wall entry (vector<EESEAPlaceWallEntry>::pop_back)

// FUNCTION: EXE 0x5aa2a2
// Inserts one wall entry (vector<EESEAPlaceWallEntry>::insert)

// FUNCTION: EXE 0x5aa2fc
// Removes a range of wall entries (vector<EESEAPlaceWallEntry>::erase)

// FUNCTION: EXE 0x5aa360
// Inserts copies of a wall entry, growing the storage if needed (vector<EESEAPlaceWallEntry>::insert(P, M, X))

// FUNCTION: EXE 0x5aa633
// Copy-constructs copies of a wall entry into raw storage (vector<EESEAPlaceWallEntry>::_Ufill)

// FUNCTION: EXE 0x5aa65e
// Copies a range of wall entries (std::copy)

// FUNCTION: EXE 0x5aa687
// Copy-constructs a range of wall entries into raw storage (vector<EESEAPlaceWallEntry>::_Ucopy)

// FUNCTION: EXE 0x5aa6b3
// Fills a range of wall entries with one value (std::fill)

// FUNCTION: EXE 0x5aa6d0
// Copies a range of wall entries backwards (std::copy_backward)

// FUNCTION: EXE 0x5aa6fc
// Copy-constructs one wall entry in place (std::_Construct)
