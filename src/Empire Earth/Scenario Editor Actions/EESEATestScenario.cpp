// C:\EEX\Empire Earth\Scenario Editor Actions\EESEATestScenario.cpp (file name inferred: no __FILE__; RTTI class
// EESEATestScenario). The editor's "test scenario" action: 0x1aaaad..0x1aaad4 (ctor, Virtual4), after EESEATechTree.cpp (0x1aaa78), before
// EESEUITerrainColorLB's slot 0x1aaad4. Virtual8 is the shared empty slot (0x1720c6) and the scalar deleting destructor
// EEAction's (0x1e1b69): both ICF-folded. Unit flags: the exe module set.
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

#include "ee/EEAction.h"
#include "ee/EEUIManager.h"
#include "ee/EECivilization.h"
#include "ee/EEUIFScenarioEditor.h"
#include "ee/EESEATestScenario.h"

// FUNCTION: EXE 0x5aaaad
// Creates the test-scenario action
EESEATestScenario::EESEATestScenario()
{
}

// FUNCTION: EXE 0x5aaac1
// Leaves the scenario editor to test the scenario
void EESEATestScenario::Virtual4()
{
    gUIManager->mScenarioEditorForm->ExitEditor(true);
}

// ICF-FOLDED: EXE 0x5720c6
// Does nothing
// (folded into the shared empty slot)
void EESEATestScenario::Virtual8()
{
}
