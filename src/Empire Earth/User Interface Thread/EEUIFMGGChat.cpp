// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGChat.cpp (file name inferred: no __FILE__; RTTI class
// EEUIFMGGChat). The in-game form's chat group: 0x267b77..0x267bb6 (ctor, ??_G, dtor; the empty slots are ICF-folded
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
#include "ee/EEUIFMGGChat.h"

// FUNCTION: EXE 0x667b77
// Creates the main game form's chat panel group (no controls of its own to refresh)
EEUIFMGGChat::EEUIFMGGChat(EEUIFMainGame* inOwner)
    : EEUIFMGGroup(inOwner)
{
}

// SYNTHETIC: EXE 0x667b8f
// (the scalar deleting destructor, emitted with the vtable; slot EEUIFMGGChat+0x0)

// FUNCTION: EXE 0x667bab
// Destroys the chat panel group
EEUIFMGGChat::~EEUIFMGGChat()
{
}

void EEUIFMGGChat::Refresh()
{
}

void EEUIFMGGChat::Virtual08()
{
}

void EEUIFMGGChat::Select()
{
}

bool EEUIFMGGChat::WantsRefresh()
{
    return false;
}
