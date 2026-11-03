// C:\EEX\Empire Earth\UI Events\EEUIEMultiplayerScreenEvents.cpp (file name inferred: no __FILE__; class prefix
// EEUIEMS). The multiplayer screen's open event: 0x1e2144..0x1e2292 (the init chain of gMultiplayerString first).
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
#include "lle/GELine.h"
#include "lle/UITextLabel.h"
#include "lle/UIEditBox.h"
#include "lle/UIListBox.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UILBString.h"
#include "lle/UIComboBox.h"
#include "lle/UIFileComboBox.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"

#include "ee/EmpireEarth.h"
#include "ee/EEUIEMainScreenEvents.h"
#include "ee/EEUIManager.h"

UString gMultiplayerString("");             // 0x562fd8 (init chain 0x1e2144; read by 0x133498 and 0x27793a)

// FUNCTION: EXE 0x5e2176
// Sets up the multiplayer form by hiding all its tabs, buttons, boxes and labels
long EEUIEMSOpenMultiplayerScreen::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    inForm->GetControl(kMultiplayerMainMenuButton)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerHostNetworkGameTab)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerJoinNetworkGameTab)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerJoinByIPTab)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerPlayOnSierraTab)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerShowIPButton)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerCreateSierraAccountButton)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerSierraEmailMyPasswordButton)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerJoinHostButton)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerAvailGamesListBox)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerHostGameNameEditBox)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerSierraPasswordEditBox)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerPlayerNameEditBox)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerPlayerNameLabel)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerSierraUserIDEditBox)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerMultiPlayerOOSWarningLabel)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerMultiplayerTitleLabel)->SetDisplayMode(kDisplayModeHidden);
    inForm->GetControl(kMultiplayerAvailGamesLabel)->SetDisplayMode(kDisplayModeHidden);
    return kErrorOK;
}

// SYNTHETIC: EXE 0x5e2144
// SYNTHETIC: EXE 0x5e214e
// SYNTHETIC: EXE 0x5e215f
// SYNTHETIC: EXE 0x5e216b
