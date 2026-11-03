// C:\EEX\Empire Earth\UI Events\EEUIEWonNetServerSelectEvents.h (RECONSTRUCTION; header and file names inferred,
// from the class prefix EEUIEWNSS). The WON server list form's events.
// ProcessEvents: EEUIEWonNetServerSelectEvents.cpp (0x1eb7fb..0x1ebc13); Cancel's is identical to EEUIEWNARBack's and
// EEUIEWNMUACancel's and folded into one copy (0x1eb7e8). Declaration order Cancel, Ok: the order of their ??_G
// after OpenScreen. Requires lle/UIEvent.h.
#ifndef EE_EEUIEWONNETSERVERSELECTEVENTS_H
#define EE_EEUIEWONNETSERVERSELECTEVENTS_H

class EEUIEWNSSCancel : public UIEvent {                // vtable 0x446cd4; button 3
public:
    virtual ~EEUIEWNSSCancel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNSSOk : public UIEvent {                    // vtable 0x446cc0; button 2, the server list 1
public:
    virtual ~EEUIEWNSSOk() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNSSOpenScreen : public UIEvent {            // vtable 0x44ba48; created by the UI's event factory
public:
    virtual ~EEUIEWNSSOpenScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
