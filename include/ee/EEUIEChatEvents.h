// The in-game chat form's events (RECONSTRUCTION; header and file names inferred, from the class prefix EEUIECE).
// Declaration order SendMessage, ChatKeyboardFocus, ChangeSendMessageType, Escape, ChangeUserChatType: the order
// in which VC6 emits their ??_G (and the two inline ProcessEvents) after EEUIECEOpenChat.
// ProcessEvent bodies and the ??_G copies: EEUIEChatEvents.cpp (0x1d2b02..0x1d321b). EEUIECECloseChat and
// EEUIECEOpenChat are created by the UI's event factory. Requires lle/UIEvent.h.
#ifndef EE_EEUIECHATEVENTS_H
#define EE_EEUIECHATEVENTS_H

class EEUIECESendMessage : public UIEvent {             // vtable 0x44627c
public:
    virtual ~EEUIECESendMessage() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECEChatKeyboardFocus : public UIEvent {       // vtable 0x44622c
public:
    virtual ~EEUIECEChatKeyboardFocus() {}
    // inline: emitted with the ??_G after EEUIECEOpenChat (0x1d2f9a)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity)
    {
        if (!inForm->GetIsKeyboardCaptured())
            inForm->SetTabStop(1);
        return 0;
    }
};

class EEUIECEChangeSendMessageType : public UIEvent {   // vtable 0x446240; the radio buttons' trigger
public:
    EEUIECEChangeSendMessageType(long inType) { this->mType = inType; }
    virtual ~EEUIECEChangeSendMessageType() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mType;                             // +0x0c 0..3 (EEMSelectPlayerChatType)
};

class EEUIECEEscape : public UIEvent {                  // vtable 0x446268
public:
    virtual ~EEUIECEEscape() {}
    // inline: emitted with the ??_G after EEUIECEOpenChat (0x1d300a)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity)
    {
        return gUserInterface->CloseForm(gUIManager->mMainGameForm);
    }
};

class EEUIECEChangeUserChatType : public UIEvent {      // vtable 0x446254; Tab / Shift+Tab
public:
    EEUIECEChangeUserChatType(bool inBackwards) { this->mBackwards = inBackwards; }
    virtual ~EEUIECEChangeUserChatType() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    bool mBackwards;                        // +0x0c
};

class EEUIECECloseChat : public UIEvent {
public:
    virtual ~EEUIECECloseChat() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECEOpenChat : public UIEvent {
public:
    virtual ~EEUIECEOpenChat() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECECancel : public UIEvent {
public:
    virtual ~EEUIECECancel() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
