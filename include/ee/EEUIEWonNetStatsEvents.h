// C:\EEX\Empire Earth\UI Events\EEUIEWonNetStatsEvents.h (RECONSTRUCTION; header and file names inferred, from the
// class prefix EEUIEWNS). The WON statistics form's events. Open's ProcessEvent:
// EEUIEWonNetStatsEvents.cpp (0x1ebc13); Cancel's (0x2547cf) and both ??_G (0x271daf, 0x274cf9) are elsewhere.
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEWONNETSTATSEVENTS_H
#define EE_EEUIEWONNETSTATSEVENTS_H

class EEUIEWNSCancel : public UIEvent {                 // vtable 0x44b8a0
public:
    virtual ~EEUIEWNSCancel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEWNSOpen : public UIEvent {                   // vtable 0x44b0a8; created by the UI's event factory
public:
    virtual ~EEUIEWNSOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
