// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAQuickSaveScenario.cpp (file name inferred: no __FILE__; RTTI class
// EESEAQuickSaveScenario). The editor's "quick save scenario" action: 0x1aa723..0x1aa74f (Virtual4 only: the ctor
// 0x1af8ad and ??_G 0x1af8cd are inline, emitted by their first user). Before EESEARotateUnits' Virtual4. Unit
// flags: the exe module set.
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
#include "lle/FSMemoryFile.h"

#include "ee/EmpireEarth.h"
#include "ee/EEAction.h"
#include "ee/EEUIManager.h"
#include "ee/EEUserInterface.h"
#include "ee/EECivilization.h"
#include "ee/EEUIFScenarioEditor.h"
#include "ee/EEAQuickSaveLoad.h"
#include "ee/EESEAQuickSaveScenario.h"

// FUNCTION: EXE 0x5aa723
// Stores the current tab's settings and posts the quick save action in save mode, written to disk
void EESEAQuickSaveScenario::Virtual4()
{
    gUIManager->mScenarioEditorForm->RefreshCurrentGroup();
    EEAQuickSaveLoad* theAction = this->mQuickSaveLoad;
    theAction->mIsLoad = false;
    theAction->mWriteToFile = true;
    gUserInterface->PostAction(theAction);
}

// ICF-FOLDED: EXE 0x5720c6
// Does nothing
// (folded into the shared empty slot)
void EESEAQuickSaveScenario::Virtual8()
{
}
