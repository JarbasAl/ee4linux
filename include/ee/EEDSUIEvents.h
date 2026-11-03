// C:\EEX\Empire Earth\UI Events\EEDSUIEvents.h (RECONSTRUCTION; header and source file names inferred).
// The debug sound panel's control events (RTTI EEDSUI*), created by EEUIManager.cpp's factory 0x2724eb (ids
// 0x6b..0x75); ProcessEvent bodies elsewhere (e.g. 0x2546ae). Requires lle/UIEvent.h.
#ifndef EE_EEDSUIEVENTS_H
#define EE_EEDSUIEVENTS_H

class EEDSUIAdjustAttackSoundOne : public UIEvent {
public:
    virtual ~EEDSUIAdjustAttackSoundOne() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustAttackSoundTwo : public UIEvent {
public:
    virtual ~EEDSUIAdjustAttackSoundTwo() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustCommandSoundOne : public UIEvent {
public:
    virtual ~EEDSUIAdjustCommandSoundOne() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustCommandSoundTwo : public UIEvent {
public:
    virtual ~EEDSUIAdjustCommandSoundTwo() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustCreatedSound : public UIEvent {
public:
    virtual ~EEDSUIAdjustCreatedSound() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustDyingSound : public UIEvent {
public:
    virtual ~EEDSUIAdjustDyingSound() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustSelectedSoundOne : public UIEvent {
public:
    virtual ~EEDSUIAdjustSelectedSoundOne() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustSelectedSoundTwo : public UIEvent {
public:
    virtual ~EEDSUIAdjustSelectedSoundTwo() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustWalkSoundOne : public UIEvent {
public:
    virtual ~EEDSUIAdjustWalkSoundOne() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIAdjustWalkSoundTwo : public UIEvent {
public:
    virtual ~EEDSUIAdjustWalkSoundTwo() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEDSUIHideDebugSoundPanel : public UIEvent {
public:
    virtual ~EEDSUIHideDebugSoundPanel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
