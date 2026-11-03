// The campaign choice form's events (class prefix EEUIEPCE; ProcessEvents 0x1e2292..0x1e295f)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEPLAYCAMPAIGNEVENTS_H
#define EE_EEUIEPLAYCAMPAIGNEVENTS_H

// vtable 0x44b60c; ??_G 0x27395c; ProcessEvent 0x1e244a; sizeof 0xc; ctor inline; new at 0x27393c
class EEUIEPCEBackToSinglePlayer : public UIEvent {
public:
    virtual ~EEUIEPCEBackToSinglePlayer() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b620; ??_G 0x273987; ProcessEvent 0x1e2464; sizeof 0xc; ctor inline; new at 0x27391e
class EEUIEPCECampaign1 : public UIEvent {
public:
    virtual ~EEUIEPCECampaign1() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b634; ??_G 0x2739b2; ProcessEvent 0x1e2599; sizeof 0xc; ctor inline; new at 0x273900
class EEUIEPCECampaign2 : public UIEvent {
public:
    virtual ~EEUIEPCECampaign2() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b648; ??_G 0x2739dd; ProcessEvent 0x1e25b7; sizeof 0xc; ctor inline; new at 0x2738e2
class EEUIEPCECampaign3 : public UIEvent {
public:
    virtual ~EEUIEPCECampaign3() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4468f4; ??_G 0x1e2755; ProcessEvent 0x1e25d5; sizeof 0xc; ctor inline; new at 0x1e26a4, 0x1e26e4, 0x1e2719
class EEUIEPCEMouseOver : public UIEvent {
public:
    virtual ~EEUIEPCEMouseOver() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446aa0; ??_G 0x1e8aa1; ProcessEvent 0x1e264f; sizeof 0xc; ctor inline; new at 0x1e8a5f
class EEUIEPCEFormOpen : public UIEvent {
public:
    virtual ~EEUIEPCEFormOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b670; ??_G 0x273a08; ProcessEvent 0x1e2866; sizeof 0xc; ctor inline; new at 0x27389e
class EEUIEPCECustomCampaign : public UIEvent {
public:
    virtual ~EEUIEPCECustomCampaign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b684; ??_G 0x273a33; ProcessEvent 0x1e2930; sizeof 0xc; ctor inline; new at 0x27387c
class EEUIEPCELearningCampaign : public UIEvent {
public:
    virtual ~EEUIEPCELearningCampaign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
