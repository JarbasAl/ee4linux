// C:\EEX\Empire Earth\Formations\EEFormation.cpp (file and folder names inferred: no __FILE__). The formation base: 0xddcc6..0xdde43 (Create, Arrange).
// Unit flags: the exe's module set.
#include <windows.h>
#include <mbstring.h>
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

#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPlayer.h"

#include "ee/EEFormation.h"
#include "ee/EEFCircle.h"
#include "ee/EEFInwardCircle.h"
#include "ee/EEFOutwardCircle.h"
#include "ee/EEFLine.h"
#include "ee/EEFLongLine.h"
#include "ee/EEFLooseLine.h"
#include "ee/EEFShortLine.h"
#include "ee/EEFStaggeredLine.h"
#include "ee/EEFWedge.h"
#include "ee/EEFDeepWedge.h"
#include "ee/EEFShallowWedge.h"

// FUNCTION: EXE 0x4ddcc6
// Creates the formation of a type: 0-3 short, long, loose, staggered line; 4-5 deep, shallow wedge;
// 6-7 inward, outward circle; NULL for any other type
EEFormation* EEFormation::Create(long inType)
{
    EEFormation* theFormation = NULL;
    switch (inType) {
    case 0:
        theFormation = new EEFShortLine;
        break;
    case 1:
        theFormation = new EEFLongLine;
        break;
    case 2:
        theFormation = new EEFLooseLine;
        break;
    case 3:
        theFormation = new EEFStaggeredLine;
        break;
    case 4:
        theFormation = new EEFDeepWedge;
        break;
    case 5:
        theFormation = new EEFShallowWedge;
        break;
    case 6:
        theFormation = new EEFInwardCircle;
        break;
    case 7:
        theFormation = new EEFOutwardCircle;
        break;
    }
    return theFormation;
}

// FUNCTION: EXE 0x4dddd5
// Collects the group's units and places them into formation slots
void EEFormation::Arrange(EEGroup* inGroup, std::vector<EEFormationSlot>& outSlots)
{
    std::vector<EEComplexUnit*> theUnits;
    theUnits.resize(inGroup->GetNumUnits());
    for (long i = inGroup->GetNumUnits() - 1; i >= 0; i--)
        theUnits[i] = inGroup->GetUnit(i);
    this->Place(theUnits, outSlots);
}
