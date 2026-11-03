// The campaign editor's events (class prefix EEUIECE; ProcessEvents 0x1cff19..0x1d1766, the first UI Events unit)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIECAMPAIGNEDITOREVENTS_H
#define EE_EEUIECAMPAIGNEDITOREVENTS_H

// vtable 0x44aff4; ??_G 0x271eb1; ProcessEvent 0x1cff19; sizeof 0xc; ctor inline; new at 0x271c0b
class EEUIECEOpen : public UIEvent {
public:
    virtual ~EEUIECEOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b33c; ??_G 0x2728ad; ProcessEvent 0x1d0095; sizeof 0xc; ctor inline; new at 0x2726eb
class EEUIECEAddScenario : public UIEvent {
public:
    virtual ~EEUIECEAddScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b2b0; ??_G 0x2728d8; ProcessEvent 0x1d00f6; sizeof 0xc; ctor inline; new at 0x2727df
class EEUIECERemoveScenario : public UIEvent {
public:
    virtual ~EEUIECERemoveScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b2d8; ??_G 0x272903; ProcessEvent 0x1d0157; sizeof 0xc; ctor inline; new at 0x272799
class EEUIECEMoveScenarioUp : public UIEvent {
public:
    virtual ~EEUIECEMoveScenarioUp() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b2c4; ??_G 0x27292e; ProcessEvent 0x1d01bb; sizeof 0xc; ctor inline; new at 0x2727bb
class EEUIECEMoveScenarioDown : public UIEvent {
public:
    virtual ~EEUIECEMoveScenarioDown() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b300; ??_G 0x272984; ProcessEvent 0x1d021d; sizeof 0xc; ctor inline; new at 0x272751
class EEUIECECampaignSave : public UIEvent {
public:
    virtual ~EEUIECECampaignSave() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446184; ??_G 0x1d031f; ProcessEvent 0x1d02a8; sizeof 0x10; ctor inline; new at 0x1d0269
class EEUIECECampaignSaveContinued : public UIEvent {
public:
    EEUIECECampaignSaveContinued(UIForm* inForm) : mForm(inForm) {}   // posted by SaveCampaign 0x1d022c (init list: stored before the vptr)
    virtual ~EEUIECECampaignSaveContinued() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    UIForm* mForm;              // +0x0c the editor form
};

// vtable 0x44b2ec; ??_G 0x2729af; ProcessEvent 0x1d034a; sizeof 0xc; ctor inline; new at 0x272775
class EEUIECECampaignSelected : public UIEvent {
public:
    virtual ~EEUIECECampaignSelected() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b274; ??_G 0x272a05; ProcessEvent 0x1d0633; sizeof 0xc; ctor inline; new at 0x27283a
class EEUIECEPasswordEntered : public UIEvent {
public:
    virtual ~EEUIECEPasswordEntered() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b328; ??_G 0x272a30; ProcessEvent 0x1d0684; sizeof 0xc; ctor inline; new at 0x27270d
class EEUIECECampaignDelete : public UIEvent {
public:
    virtual ~EEUIECECampaignDelete() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b260; ??_G 0x2729da; ProcessEvent 0x1d0722; sizeof 0xc; ctor inline; new at 0x272857
class EEUIECECampaignNew : public UIEvent {
public:
    virtual ~EEUIECECampaignNew() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b29c; ??_G 0x272a5b; ProcessEvent 0x1d07b5; sizeof 0xc; ctor inline; new at 0x272800
class EEUIECEMainScreen : public UIEvent {
public:
    virtual ~EEUIECEMainScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b288; ??_G 0x272a86; ProcessEvent 0x1d085d; sizeof 0xc; ctor inline; new at 0x27281d
class EEUIECEScenarioEditorOpen : public UIEvent {
public:
    virtual ~EEUIECEScenarioEditorOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
