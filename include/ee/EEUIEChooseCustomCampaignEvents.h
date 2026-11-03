// The custom campaign form's (0x22) events (RECONSTRUCTION; header and file names inferred, from the class prefix
// EEUIECCCE). ProcessEvents: EEUIEChooseCustomCampaignEvents.cpp (0x1d3a77..0x1d4165); Cancel's is folded into
// EEUIECSSEBack's (0x1d2aa1), FormClose's into the shared `return 0` (0x27cf38). Declaration order SelectCampaign, Ok,
// Cancel: the order of their ??_G after FormOpen. Requires lle/UIEvent.h.
#ifndef EE_EEUIECHOOSECUSTOMCAMPAIGNEVENTS_H
#define EE_EEUIECHOOSECUSTOMCAMPAIGNEVENTS_H

class EEUIECCCEFormOpen : public UIEvent {            // vtable 0x44691c
public:
    virtual ~EEUIECCCEFormOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECCCESelectCampaign : public UIEvent {      // vtable 0x4462d4
public:
    virtual ~EEUIECCCESelectCampaign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECCCEOk : public UIEvent {                  // vtable 0x4462c0
public:
    virtual ~EEUIECCCEOk() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECCCECancel : public UIEvent {              // vtable 0x4462ac
public:
    virtual ~EEUIECCCECancel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECCCEFormClose : public UIEvent {           // vtable 0x446908
public:
    virtual ~EEUIECCCEFormClose() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
