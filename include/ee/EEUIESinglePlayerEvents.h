// The single-player screen's events (class prefix EEUIESPE; ProcessEvents 0x1e8a24..0x1e8e7d)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIESINGLEPLAYEREVENTS_H
#define EE_EEUIESINGLEPLAYEREVENTS_H

// vtable 0x4468cc; ??_G 0x1e1f5f; ProcessEvent 0x1e8a24; sizeof 0xc; ctor inline; new at 0x1e1f05
class EEUIESPEOpen : public UIEvent {
public:
    virtual ~EEUIESPEOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b814; ??_G 0x2742fb; ProcessEvent 0x1e8a41; sizeof 0xc; ctor inline; new at 0x2742db
class EEUIESPECampaign : public UIEvent {
public:
    virtual ~EEUIESPECampaign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b828; ??_G 0x274326; ProcessEvent 0x1e8b10; sizeof 0xc; ctor inline; new at 0x2742bd
class EEUIESPECivCampaign : public UIEvent {
public:
    virtual ~EEUIESPECivCampaign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b65c; ??_G 0x273a5e; ProcessEvent 0x1e8b44; sizeof 0xc; ctor inline; new at 0x2738c0, 0x274281
class EEUIESPELoadGame : public UIEvent {
public:
    virtual ~EEUIESPELoadGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b850; ??_G 0x27437c; ProcessEvent 0x1e8b68; sizeof 0xc; ctor inline; new at 0x27425f
class EEUIESPEMainMenu : public UIEvent {
public:
    virtual ~EEUIESPEMainMenu() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b864; ??_G 0x2743a7; ProcessEvent 0x1e8b86; sizeof 0xc; ctor inline; new at 0x27423d
class EEUIESPERandomMap : public UIEvent {
public:
    virtual ~EEUIESPERandomMap() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
