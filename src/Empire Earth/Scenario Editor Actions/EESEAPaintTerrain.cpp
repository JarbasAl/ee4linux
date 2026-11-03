// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAPaintTerrain.cpp (file name inferred: no __FILE__; RTTI class
// EESEAPaintTerrain). The terrain painting action: 0x1a8d3e..0x1a9112 (PlantTerrainUnits, Virtual4, SetArea, the deque
// _Growmap/iterator COMDATs). After EESEAPaintMoutains.cpp, before EESEAPlaceGate.cpp.
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
#include "lle/URandom.h"
#include "ee/EEAreaUtilities.h"
#include "ee/EESEAPaint.h"
#include "ee/EESEAPaintTerrain.h"

// FUNCTION: EXE 0x5a8d3e
// Scatters the terrain's ambient units (gaia) over the area's tiles at random, by its ambient chance, where they fit
void EESEAPaintTerrain::PlantTerrainUnits(long inLeft, long inRight, long inTop, long inBottom)
{
    URandomIntegers theRandom;
    EEMap* theMap = gWorld.mMap;
    EETerrainTable* theTerrain = EETerrainTable::Get(this->mTerrain);
    long x;
    long y;
    if (!(theTerrain->mAmbientUnitChance > 0))
        return;
    theRandom.SetSeed(TMGetTime());
    for (y = inTop; y >= inBottom; y--) {
        for (x = inLeft; x <= inRight; x++) {
            if (theMap->GetTile(x, y) == NULL || !(theRandom.Generate(1, 100) < theTerrain->mAmbientUnitChance))
                continue;
            long theUnitType = theTerrain->GetUnitClass(theRandom.Generate(0, theTerrain->GetNumUnitClasses() - 1));
            EEUnitClass* theClass = gUnitClassTable[theUnitType];
            EEPlayer* thePlayer = gWorld.mPlayers[0];
            if (EECanPlaceOnTile(thePlayer, theUnitType, x, y, 0, 0))
                theClass->CreateUnit(x, y, thePlayer->mPlayerID, false, NULL);
        }
    }
}

// FUNCTION: EXE 0x5a8e6c
// Paints each queued area with the terrain and texture (unless the terrain only adds ambient units), scatters its
// ambient units, fixes cliffs and redraws the minimap
void EESEAPaintTerrain::Virtual4()
{
    EEMap* theMap = gWorld.mMap;
    EETerrainTable* theTerrain = EETerrainTable::Get(this->mTerrain);
    EESEAPaintRect theRect;

    gB529066 = false;
    gCameraFollowing = false;
    gB529068 = false;
    while (this->mPendingAreas.size() != 0) {
        theRect = this->mPendingAreas.back();
        this->mPendingAreas.pop_back();
        if (!theTerrain->mIsAmbientsOnly)
            theMap->PaintTerrainRect(theRect.mLeft, theRect.mRight, theRect.mTop, theRect.mBottom, this->mTerrain, this->mTexture);
        this->PlantTerrainUnits(theRect.mLeft, theRect.mRight, theRect.mTop, theRect.mBottom);
        theMap->UpdateCliffs(theRect.mLeft - 2, theRect.mRight + 3, theRect.mBottom - 2, theRect.mTop + 3);
        gMiniMap->RedrawRect(theRect.mLeft, theRect.mRight, theRect.mTop, theRect.mBottom);
    }
    gMiniMap->Refresh();
}

// FUNCTION: EXE 0x5a8f45
// Queues the area if any of its tiles is on the map; returns whether it queued
bool EESEAPaintTerrain::SetArea(long inLeft, long inTop, long inRight, long inBottom)
{
    EEMap* theMap = gWorld.mMap;
    EESEAPaintRect theRect;
    bool theChanged = false;
    long x;
    long y;
    for (y = inTop; y >= inBottom; y--) {
        for (x = inLeft; x <= inRight; x++) {
            if (theMap->GetTile(x, y) != NULL)
                theChanged = true;
        }
    }
    if (theChanged) {
        theRect.mLeft = inLeft;
        theRect.mTop = inTop;
        theRect.mRight = inRight;
        theRect.mBottom = inBottom;
        this->mPendingAreas.push_back(theRect);
    }
    return theChanged;
}

// FUNCTION: EXE 0x5a8fea
// Grows the block map of the paint area queue (deque<EESEAPaintRect>::_Growmap)

// FUNCTION: EXE 0x5a90ea
// Returns the paint area an iterator points at (deque<EESEAPaintRect>::iterator::operator*)

// Does nothing (Virtual8: the shared empty slot +0x08, ICF-folded into 0xe123d; no marker of its own)
void EESEAPaintTerrain::Virtual8()
{
}
