// C:\EEX\Empire Earth\Scenario Editor Actions\EESEATechTree.cpp (file name inferred: no __FILE__; RTTI class
// EESEATechTree). The editor's tech tree action: 0x1aaa78..0x1aaaad (Virtual4 only; the ctor is inline at its users,
// ??_G EEAction's 0x1e1b69). After EESEASaveScenario.cpp (its inline EEUIESLSaveScreen dtor ends at 0x1aaa78),
// before EESEATestScenario's ctor. Unit flags: the exe module set.
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
#include "lle/GEText.h"
#include "lle/UISlider.h"
#include "lle/UIScrollBar.h"
#include "lle/UIListBox.h"
#include "lle/UILBString.h"

#include "ee/EEAction.h"
#include "ee/EEUIManager.h"
#include "ee/EECivilization.h"
#include "ee/EEUIFScenarioEditor.h"
#include "ee/EESEUITechListBox.h"
#include "ee/EESEATechTree.h"

// FUNCTION: EXE 0x5aaa78
// Moves the tech lines mFirst..mLast across the unit-types tab's tech lists, from the left or the right list
void EESEATechTree::Virtual4()
{
    EEUIFScenarioEditor* theForm = gUIManager->mScenarioEditorForm;
    EESEUITechListBox* theListBox;
    if (this->mIsLeftList)
        theListBox = static_cast<EESEUITechListBox*>(theForm->GetControlById(kSERBUnitTypesTechList));
    else
        theListBox = static_cast<EESEUITechListBox*>(theForm->GetControlById(kSERBUnitTypesOtherTechList));
    theListBox->Fill(this->mFirst, this->mLast);
}

// ICF-FOLDED: EXE 0x5720c6
// Does nothing
// (folded into the shared empty slot)
void EESEATechTree::Virtual8()
{
}
