// C:\EEX\Empire Earth\UI Events\EEUIECivilizationBuilderEvents.h (RECONSTRUCTION; header and file names inferred).
// The civilization builder form's control events (RTTI EEUIECB*), created by EEUIManager.cpp's
// factory 0x272ade (ids 0x33..0x7b); their ??_G bodies are ICF-folded (0x1e60e7) and their ProcessEvent bodies live in
// the civilization builder's event unit (not written yet). Requires lle/UIEvent.h.
#ifndef EE_EEUIECIVILIZATIONBUILDEREVENTS_H
#define EE_EEUIECIVILIZATIONBUILDEREVENTS_H

class EEUIECBAcceptChangeName : public UIEvent {
public:
    virtual ~EEUIECBAcceptChangeName() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBAcceptChanges : public UIEvent {
public:
    virtual ~EEUIECBAcceptChanges() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBAcceptLoadCivilization : public UIEvent {
public:
    virtual ~EEUIECBAcceptLoadCivilization() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBAcceptSaveCivilization : public UIEvent {
public:
    virtual ~EEUIECBAcceptSaveCivilization() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBCancelChangeName : public UIEvent {
public:
    virtual ~EEUIECBCancelChangeName() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBCancelChanges : public UIEvent {
public:
    virtual ~EEUIECBCancelChanges() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBCancelLoadCivDialog : public UIEvent {
public:
    virtual ~EEUIECBCancelLoadCivDialog() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBCancelSaveCivDialog : public UIEvent {
public:
    virtual ~EEUIECBCancelSaveCivDialog() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBDisplayDeleteCivDialog : public UIEvent {
public:
    virtual ~EEUIECBDisplayDeleteCivDialog() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBPotentialUpgradeSelected : public UIEvent {
public:
    virtual ~EEUIECBPotentialUpgradeSelected() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBPurchasedUpgradeSelected : public UIEvent {
public:
    virtual ~EEUIECBPurchasedUpgradeSelected() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBSaveNameSelected : public UIEvent {
public:
    virtual ~EEUIECBSaveNameSelected() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBShowChangeNameDialog : public UIEvent {
public:
    virtual ~EEUIECBShowChangeNameDialog() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBShowLoadCivDialog : public UIEvent {
public:
    virtual ~EEUIECBShowLoadCivDialog() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBShowSaveCivDialog : public UIEvent {
public:
    virtual ~EEUIECBShowSaveCivDialog() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBUpdateControls : public UIEvent {
public:
    virtual ~EEUIECBUpdateControls() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBUpgradePurchaseAttempt : public UIEvent {
public:
    virtual ~EEUIECBUpgradePurchaseAttempt() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIECBUpgradeReturned : public UIEvent {
public:
    virtual ~EEUIECBUpgradeReturned() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
