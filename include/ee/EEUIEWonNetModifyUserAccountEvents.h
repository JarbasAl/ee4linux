// C:\EEX\Empire Earth\UI Events\EEUIEWonNetModifyUserAccountEvents.h (RECONSTRUCTION; header and file names
// inferred, from the class prefix EEUIEWNMUA). The WON update-account
// form's (0x25) events. ProcessEvents: EEUIEWonNetModifyUserAccountEvents.cpp (0x1eb5e2..0x1eb7fb); Cancel's is
// the one copy ICF kept of the identical `return inForm->GetManager()->Pop();` bodies (also EEUIEWNARBack's and
// EEUIEWNSSCancel's). Declaration order Cancel, OpenScreen, Submit: the order of the ??_G after OpenScreen.
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEWONNETMODIFYUSERACCOUNTEVENTS_H
#define EE_EEUIEWONNETMODIFYUSERACCOUNTEVENTS_H

class EEUIEWNMUACancel : public UIEvent {               // vtable 0x446cac; button 9
public:
    virtual ~EEUIEWNMUACancel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNMUAOpenScreen : public UIEvent {           // vtable 0x446c0c; EEUIEWNARClickUpdateAccount sets it
public:
    virtual ~EEUIEWNMUAOpenScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNMUASubmit : public UIEvent {               // vtable 0x446c98; button 8
public:
    virtual ~EEUIEWNMUASubmit() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
