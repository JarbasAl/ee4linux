// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGOptions.cpp (file name inferred: no __FILE__; RTTI class
// EEUIFMGGOptions). The in-game form's options group: 0x267c34..0x267c73 (ctor, ??_G, dtor; the empty slots are ICF-folded
// elsewhere). Unit flags: the exe module set.
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
#include "ee/EEUnit.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "ee/EEUIFMainGame.h"
#include "ee/EEUIFMGMainGroup.h"
#include "ee/EEUIFMGGOptions.h"

// FUNCTION: EXE 0x667c34
// Creates the main game form's options panel group (no controls of its own to refresh)
EEUIFMGGOptions::EEUIFMGGOptions(EEUIFMainGame* inOwner)
    : EEUIFMGGroup(inOwner)
{
}

// SYNTHETIC: EXE 0x667c4c
// (the scalar deleting destructor, emitted with the vtable; slot EEUIFMGGOptions+0x0)

// FUNCTION: EXE 0x667c68
// Destroys the options panel group
EEUIFMGGOptions::~EEUIFMGGOptions()
{
}

void EEUIFMGGOptions::Refresh()
{
}

void EEUIFMGGOptions::Virtual08()
{
}

void EEUIFMGGOptions::Select()
{
}

bool EEUIFMGGOptions::WantsRefresh()
{
    return false;
}
