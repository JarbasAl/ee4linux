// C:\EEX\Empire Earth\UI Events\EEUIEChatEvents.cpp (file name inferred: no __FILE__; class prefix EEUIECE). The
// in-game chat form's events: 0x1d2b02..0x1d321b. The message ctors the events create are out-of-line COMDATs placed
// after their first user (EEMSelectPlayerChatType 0x1d2b36, EEMCheatCode 0x1d3162, EEMInGameChat 0x1d3196); the ??_G
// of the events OpenChat creates are emitted between the following functions. Unit flags: the exe's module set.
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
#include "lle/UIRadioButton.h"
#include "lle/UIEditBox.h"

#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPlayer.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "ee/EECheatCodes.h"
#include "ee/EmpireEarth.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIFMainGame.h"
#include "ee/EEUIManager.h"
#include "ee/EEUIEChatEvents.h"

// FUNCTION: EXE 0x5d2b02
// Posts the chosen chat recipients (0 all, 1 allies, 2 enemies, 3 custom) as the player's chat type
long EEUIECEChangeSendMessageType::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    gCommunications->ProcessLocalUserInput(new EEMSelectPlayerChatType(this->mType), kSlotNone);
    return kErrorOK;
}

// FUNCTION: EXE 0x5d2b36
// Creates the message that sets the player's chat recipients type
// (EEMSelectPlayerChatType's inline ctor, emitted out of line after its first user)

// FUNCTION: EXE 0x5d2b63
// Moves the chat recipients radio to the next choice (Tab: all, enemies, custom, allies) or the previous (Shift+Tab)
long EEUIECEChangeUserChatType::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    if (this->mBackwards == true) {
        if (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllyRadioButton)))->Check();
        else if (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllyRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatCustomRadioButton)))->Check();
        else if (static_cast<UIRadioButton*>(inForm->GetControl(kChatEnemyRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllRadioButton)))->Check();
        else if (static_cast<UIRadioButton*>(inForm->GetControl(kChatCustomRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatEnemyRadioButton)))->Check();
    } else {
        if (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatEnemyRadioButton)))->Check();
        else if (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllyRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllRadioButton)))->Check();
        else if (static_cast<UIRadioButton*>(inForm->GetControl(kChatEnemyRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatCustomRadioButton)))->Check();
        else if (static_cast<UIRadioButton*>(inForm->GetControl(kChatCustomRadioButton))->GetIsChecked())
            (static_cast<UIRadioButton*>(inForm->GetControl(kChatAllyRadioButton)))->Check();
    }
    return kErrorOK;
}

// FUNCTION: EXE 0x5d2bff
// Removes the chat form's hot keys when it closes
long EEUIECECloseChat::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    inForm->RemoveAllHotKeys();
    return kErrorOK;
}

// FUNCTION: EXE 0x5d2c0e
// Sets up the chat form: Enter sends, Escape closes, Tab/Shift+Tab cycle the recipients; checks the player's chat type
// (all, with the other choices hidden, for observers) and focuses the text box
long EEUIECEOpenChat::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    UIHotKey theHotKey;
    EEPlayer* thePlayer = ::gWorld.mPlayers[::gWorld.mCurrentPlayer];
    UIEvent* theEvent = new EEUIECESendMessage;
    theHotKey.SetKey(DIK_RETURN);
    theHotKey.SetJustPressedEvent(theEvent);
    inForm->GetControl(kChatTextEditBox)->SetTriggerEvent(kControlTriggerEnterPressed, theEvent);
    theEvent->Dereference();
    inForm->AddHotKey(theHotKey, 0);
    inForm->AddHotKey(DIK_ESCAPE, kKeyNone, kKeyNone, kKeyNone, new EEUIECEEscape, NULL, NULL, NULL, 0, false);
    inForm->AddHotKey(DIK_TAB, kKeyNone, kKeyNone, kKeyNone, new EEUIECEChangeUserChatType(false), NULL, NULL, NULL, 0, false);
    inForm->AddHotKey(DIK_TAB, kKeyShift, kKeyNone, kKeyNone, new EEUIECEChangeUserChatType(true), NULL, NULL, NULL, 0, false);
    if (thePlayer != NULL) {
        if (thePlayer->mObserverMode) {
            static_cast<UIRadioButton*>(inForm->GetControl(kChatAllRadioButton))->Check();
            inForm->GetControl(kChatAllRadioButton)->SetDisplayMode(kDisplayModeHidden);
            inForm->GetControl(kChatAllyRadioButton)->SetDisplayMode(kDisplayModeHidden);
            inForm->GetControl(kChatEnemyRadioButton)->SetDisplayMode(kDisplayModeHidden);
            inForm->GetControl(kChatCustomRadioButton)->SetDisplayMode(kDisplayModeHidden);
        } else {
            switch (thePlayer->mChatType) {
            case 0:
                static_cast<UIRadioButton*>(inForm->GetControl(kChatAllRadioButton))->Check();
                break;
            case 1:
                static_cast<UIRadioButton*>(inForm->GetControl(kChatAllyRadioButton))->Check();
                break;
            case 2:
                static_cast<UIRadioButton*>(inForm->GetControl(kChatEnemyRadioButton))->Check();
                break;
            case 3:
                static_cast<UIRadioButton*>(inForm->GetControl(kChatCustomRadioButton))->Check();
                break;
            }
        }
    }
    inForm->GetControl(kChatTextEditBox)->TabbedTo();
    theEvent = new EEUIECEChangeSendMessageType(0);
    inForm->GetControl(kChatAllRadioButton)->SetTriggerEvent(kControlTriggerChecked, theEvent);
    theEvent->Dereference();
    theEvent = new EEUIECEChangeSendMessageType(1);
    inForm->GetControl(kChatAllyRadioButton)->SetTriggerEvent(kControlTriggerChecked, theEvent);
    theEvent->Dereference();
    theEvent = new EEUIECEChangeSendMessageType(2);
    inForm->GetControl(kChatEnemyRadioButton)->SetTriggerEvent(kControlTriggerChecked, theEvent);
    theEvent->Dereference();
    theEvent = new EEUIECEChangeSendMessageType(3);
    inForm->GetControl(kChatCustomRadioButton)->SetTriggerEvent(kControlTriggerChecked, theEvent);
    theEvent->Dereference();
    theEvent = new EEUIECEChatKeyboardFocus;
    inForm->SetFormKeyboardCapturedEvent(theEvent);
    theEvent->Dereference();
    return kErrorOK;
}

// FUNCTION: EXE 0x5d2f6f
// Destroys the send-chat event and frees it when asked (scalar deleting destructor)
// (??_G of EEUIECESendMessage; then the COMDATs of the classes OpenChat creates, in their declaration order)

// FUNCTION: EXE 0x5d2f9a
// Moves the keyboard focus to the chat text box when nothing on the form holds the keyboard
// (EEUIECEChatKeyboardFocus::ProcessEvent, inline in the header)

// FUNCTION: EXE 0x5d2fb4
// Destroys the chat keyboard-focus event and frees it when asked (scalar deleting destructor)
// (??_G of EEUIECEChatKeyboardFocus)

// FUNCTION: EXE 0x5d2fdf
// Destroys the chat-recipients radio event and frees it when asked (scalar deleting destructor)
// (??_G of EEUIECEChangeSendMessageType)

// FUNCTION: EXE 0x5d300a
// Closes the chat form (Escape)
// (EEUIECEEscape::ProcessEvent, inline in the header)

// FUNCTION: EXE 0x5d3023
// Destroys the chat Escape event and frees it when asked (scalar deleting destructor)
// (??_G of EEUIECEEscape)

// FUNCTION: EXE 0x5d304e
// Destroys the Tab chat-recipients event and frees it when asked (scalar deleting destructor)
// (??_G of EEUIECEChangeUserChatType)

// FUNCTION: EXE 0x5d3079
// Posts the typed chat line (first any cheat code in it, when cheats are enabled) and closes the chat form
long EEUIECESendMessage::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    UWideString theText(static_cast<UIEditBox*>(inForm->GetControl(kChatTextEditBox))->GetText(0));
    long theCode = 0;
    long theParameter = 0;
    if (theText.GetLength() > 0) {
        if (::gWorld.GetCheatsEnabled()) {
            gCheatCodes->ParseCheatCode(theText, theCode, theParameter);
            if (theCode != 0)
                gCommunications->ProcessLocalUserInput(new EEMCheatCode(theCode, theParameter), kSlotNone);
        }
        gCommunications->ProcessLocalUserInput(new EEMInGameChat(theText), kSlotNone);
    }
    return gUserInterface->CloseForm(gUIManager->mMainGameForm);
}

// FUNCTION: EXE 0x5d3162
// Creates the message that applies a cheat code with its parameter
// (EEMCheatCode's inline ctor, out of line)

// FUNCTION: EXE 0x5d3196
// Creates the in-game chat message carrying a text line
// (EEMInGameChat's inline ctor, out of line)

// FUNCTION: EXE 0x5d31df
// Closes the chat form when the mouse is pressed outside it; returns the close result (0 otherwise)
long EEUIECECancel::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    long theResult = 0;
    if (!inForm->GetRectangle().Contains(inForm->GetMouseLocation()))
        theResult = gUserInterface->CloseForm(gUIManager->mMainGameForm);
    return theResult;
}
