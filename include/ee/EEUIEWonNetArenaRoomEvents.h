// C:\EEX\Empire Earth\UI Events\EEUIEWonNetArenaRoomEvents.h (RECONSTRUCTION; header and file names inferred,
// from the class prefix EEUIEWNAR). The WON room list form's events.
// ProcessEvents: EEUIEWonNetArenaRoomEvents.cpp (0x1eafcb..0x1eb5e2); Back's is identical to EEUIEWNMUACancel's and
// EEUIEWNSSCancel's and folded into one copy (0x1eb7e8). Declaration order ActivateForm, Back, DeactivateForm,
// DoubleClickRoom, ClickUpdateAccount, JoinRoom: the order of the ??_G VC6 emits after OpenScreen (UpdateForm's
// comes after ActivateForm, its first user). Requires lle/UIEvent.h.
#ifndef EE_EEUIEWONNETARENAROOMEVENTS_H
#define EE_EEUIEWONNETARENAROOMEVENTS_H

class EEUIEWNARUpdateForm : public UIEvent {            // vtable 0x446bf8; the form's update event while active
public:
    virtual ~EEUIEWNARUpdateForm() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNARActivateForm : public UIEvent {          // vtable 0x446c48
public:
    virtual ~EEUIEWNARActivateForm() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNARBack : public UIEvent {                  // vtable 0x446c84; button 8
public:
    virtual ~EEUIEWNARBack() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNARDeactivateForm : public UIEvent {        // vtable 0x446c34
public:
    virtual ~EEUIEWNARDeactivateForm() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNARDoubleClickRoom : public UIEvent {       // vtable 0x446c5c; the rooms box (control 6)
public:
    virtual ~EEUIEWNARDoubleClickRoom() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNARClickUpdateAccount : public UIEvent {    // vtable 0x446c20; button 9: opens form 0x25
public:
    virtual ~EEUIEWNARClickUpdateAccount() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNARJoinRoom : public UIEvent {              // vtable 0x446c70; button 7
public:
    virtual ~EEUIEWNARJoinRoom() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNAROpenScreen : public UIEvent {            // vtable 0x43e4b4; created by the UI's event factory
public:
    virtual ~EEUIEWNAROpenScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
