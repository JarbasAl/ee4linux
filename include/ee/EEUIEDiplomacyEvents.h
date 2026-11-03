// The diplomacy form's events (class prefix EEUIEDS; ProcessEvents 0x1d410f..0x1d6775)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Declaration order: the ??_G block 0x1d5a0e..0x1d5b3b after Open (their first user) runs
// AlliedCheckBox, CustomChatCheckBox, TributeLClick, TributeRClick, OKButton, ClearTribute, Cancel (??_G COMDATs
// follow their first user in declaration order), although Open creates CustomChatCheckBox first.
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEDIPLOMACYEVENTS_H
#define EE_EEUIEDIPLOMACYEVENTS_H

// vtable 0x44b008; ??_G 0x271e86; ProcessEvent 0x1d4165; sizeof 0xc; ctor inline; new at 0x271be9
class EEUIEDSOpen : public UIEvent {
public:
    virtual ~EEUIEDSOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446398; ??_G 0x1d5a0e; ProcessEvent 0x27cf38 (ICF-shared with EEPPlayerEvent, EEUGAttackGround, EEUGExplore...) trivial: xor eax, eax; ret 8; sizeof 0xc; ctor inline; new at 0x1d4284
class EEUIEDSAlliedCheckBox : public UIEvent {
public:
    virtual ~EEUIEDSAlliedCheckBox() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4463ac; ??_G 0x1d5a39; ProcessEvent 0x27cf38 (ICF-shared with EEPPlayerEvent, EEUGAttackGround, EEUGExplore...) trivial: xor eax, eax; ret 8; sizeof 0xc; ctor inline; new at 0x1d4237
class EEUIEDSCustomChatCheckBox : public UIEvent {
public:
    virtual ~EEUIEDSCustomChatCheckBox() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446384; ??_G 0x1d5a64; ProcessEvent 0x1d5fd1; sizeof 0xc; ctor inline; new at 0x1d42d4
class EEUIEDSTributeLClick : public UIEvent {
public:
    virtual ~EEUIEDSTributeLClick() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446370; ??_G 0x1d5a8f; ProcessEvent 0x1d6113; sizeof 0xc; ctor inline; new at 0x1d4324
class EEUIEDSTributeRClick : public UIEvent {
public:
    virtual ~EEUIEDSTributeRClick() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44635c; ??_G 0x1d5aba; ProcessEvent 0x1d622f; sizeof 0xc; ctor inline; new at 0x1d4371
class EEUIEDSOKButton : public UIEvent {
public:
    virtual ~EEUIEDSOKButton() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446348; ??_G 0x1d5ae5; ProcessEvent 0x1d65f8; sizeof 0xc; ctor inline; new at 0x1d43be
class EEUIEDSClearTribute : public UIEvent {
public:
    virtual ~EEUIEDSClearTribute() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446334; ??_G 0x1d5b10; ProcessEvent 0x1d674a; sizeof 0xc; ctor inline; new at 0x1d440e
class EEUIEDSCancel : public UIEvent {
public:
    virtual ~EEUIEDSCancel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
