// C:\EEX\Empire Earth\UI Events\EEUIESSEOpen.cpp (file name inferred: no __FILE__; class prefix EEUIESSE). The
// form that lists the human players with a status bar each (the multiplayer start screen): its open event alone,
// 0x1e1b8d..0x1e1e48 (the MGOE unit's EEAction ??_G ends at 0x1e1b8d; EEUIEMainScreenEvents.cpp starts at 0x1e1e48).
// Its ??_G (0x271cd8) follows its creator in EEUIManager. Unit flags: the exe's module set.
#include <windows.h>
#include <mbstring.h>
#include <algorithm>
#include <list>

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
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UIStatusBar.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"
#include "lle/UEncryption.h"

#include "ee/EmpireEarth.h"
#include "ee/EECommunications.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIManager.h"
#include "ee/EEGameOptions.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEUIEMainScreenEvents.h"

// FUNCTION: EXE 0x5e1b8d
// Fills the loading screen: decrypted registered-to and copyright lines and, with several humans, a name and
// progress bar per human or observer (observers white); notes the local player's bar index
long EEUIESSEOpen::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    EEGameOptionsPlayer thePlayer;
    GERectangle theRectangle;
    UString theEncryptedTitle("B7B2265DB7B2265D2C11B5F7AC114576B5A95DCC84F90596C4116DB6D6B115C626B327BEB7B274D5");
    UString theEncryptedText("B7B227FDB7B227FD1CC12D1714F15D2EFC610D9EB419BD94242B2506FCD147BEA49B253E24C9BD96AC19859696B327BEB7B25FB5");
    UString theTitle;
    UString theText;
    UWideString theTitleText;
    UWideString theTextText;
    unsigned long theCount;
    unsigned char i;
    UIBackground* theBackground;
    gWorld.GetLobbyOptions().mLock->ReadLock();
    UEDecryptUString(theEncryptedTitle, theTitle);
    UEDecryptUString(theEncryptedText, theText);
    theTitleText = theTitle;
    theTextText = theText;
    theTitleText += gU530BC8;
    static_cast<UITextLabel*>(inForm->GetControl(kSplashRegisteredTo))->SetText(theTitleText);
    static_cast<UITextLabel*>(inForm->GetControl(kSplashCopyRight))->SetText(theTextText);
    if (gCommunications->GetNumHumanPlayers() > 1) {
        theCount = 0;
        for (i = 0; i < 16; i++) {
            thePlayer = gWorld.GetLobbyOptions().mPlayers[i];
            if (thePlayer.mIsLocal == true)
                gUserInterface->mLocalPlayer = theCount;
            if (thePlayer.mType != 1 && thePlayer.mType != 2)
                continue;
            inForm->GetControl(theCount + 4)->SetDisplayMode(kDisplayModeShown);
            static_cast<UITextLabel*>(inForm->GetControl(theCount + 4))->SetText(thePlayer.mName);
            inForm->GetControl(theCount + 0x14)->SetDisplayMode(kDisplayModeShown);
            static_cast<UIStatusBar*>(inForm->GetControl(theCount + 0x14))->SetStatusBarOpacity(1.0f);
            if (thePlayer.mType == 2)
                (static_cast<UIStatusBar*>(inForm->GetControl(theCount + 0x14)))->SetStatusBarColor(GEColor(1.0f, 1.0f, 1.0f));
            else
                (static_cast<UIStatusBar*>(inForm->GetControl(theCount + 0x14)))->SetStatusBarColor(EEGetPlayerColor(thePlayer.mColor));
            static_cast<UIStatusBar*>(inForm->GetControl(theCount + 0x14))->SetCurrentRangeValue(0);
            theCount++;
        }
        theBackground = inForm->GetBackgroundByID(0x84);
        theRectangle = theBackground->GetRectangle();
        theRectangle.SetBottom(inForm->GetControl(theCount + 3)->GetRectangle().GetBottom() + 3.0f);
        theBackground->SetRectangle(theRectangle);
        theBackground->SetVisibility(true);
    } else {
        theBackground = inForm->GetBackgroundByID(0x84);
        theBackground->SetVisibility(false);
    }
    gWorld.GetLobbyOptions().mLock->ReadUnlock();
    return kErrorOK;
}
