// The campaign scenario selection form's (0x21) events (RECONSTRUCTION; header and file names inferred, from the
// class prefix EEUIECSSE). ProcessEvents: EEUIECampaignSelectScreenEvents.cpp (0x1d1766..); FormClose's is the
// ICF-shared `return 0` at 0x27cf38. Requires lle/UIEvent.h.
#ifndef EE_EEUIECAMPAIGNSELECTSCREENEVENTS_H
#define EE_EEUIECAMPAIGNSELECTSCREENEVENTS_H

class EEUIECSSEFormOpen : public UIEvent {            // vtable 0x4462fc
public:
    virtual ~EEUIECSSEFormOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // 0x1d1766
};

class EEUIECSSEFormClose : public UIEvent {           // vtable 0x4462e8
public:
    virtual ~EEUIECSSEFormClose() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // ICF-folded into 0x27cf38
};

// ---- the form's controls' events, ProcessEvent order ----
// vtable 0x4461f4; ??_G 0x1d1911; ProcessEvent 0x1d2445; sizeof 0xc; ctor inline; new at 0x1d17b7
class EEUIECSSESelectCampaign : public UIEvent {
public:
    virtual ~EEUIECSSESelectCampaign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4461e0; ??_G 0x1d193c; ProcessEvent 0x1d24cc; sizeof 0xc; ctor inline; new at 0x1d1800
class EEUIECSSESelectScenario : public UIEvent {
public:
    virtual ~EEUIECSSESelectScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4461cc; ??_G 0x1d1967; ProcessEvent 0x1d26ef; sizeof 0xc; ctor inline; new at 0x1d183e
class EEUIECSSESelectDifficultyLevel : public UIEvent {
public:
    virtual ~EEUIECSSESelectDifficultyLevel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4461b8; ??_G 0x1d1992; ProcessEvent 0x1d274a; sizeof 0xc; ctor inline; new at 0x1d187c
class EEUIECSSEStartScenario : public UIEvent {
public:
    virtual ~EEUIECSSEStartScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4461a4; ??_G 0x1d19bd; ProcessEvent 0x1d2aa1 (ICF-shared with EEUIECCCECancel, EEUIESLECancelSave); sizeof 0xc; ctor inline; new at 0x1d18bf
class EEUIECSSEBack : public UIEvent {
public:
    virtual ~EEUIECSSEBack() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// 0x562ee0 "default": the player whose campaign progress the form shows (defined at the end of the unit: its init
// chain 0x1d2ab3 follows Back's ProcessEvent; provisional name)
extern UString gCampaignDefaultUser;
#endif
