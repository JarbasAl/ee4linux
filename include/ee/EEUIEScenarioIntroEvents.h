// The scenario introduction form's events (class prefix EEUIESIE; ProcessEvents 0x1e76c1..0x1e853a)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIESCENARIOINTROEVENTS_H
#define EE_EEUIESCENARIOINTROEVENTS_H

// vtable 0x44b7d8; ??_G 0x275205; ProcessEvent 0x1e76c1; sizeof 0xc; ctor inline; new at 0x273ee7
class EEUIESIEBack : public UIEvent {
public:
    virtual ~EEUIESIEBack() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446a50; ??_G 0x1e83fc; ProcessEvent 0x1e7741; sizeof 0xc; ctor inline; new at 0x1e7d05, 0x1e8009, 0x273f0c
class EEUIESIEBriefing : public UIEvent {
public:
    virtual ~EEUIESIEBriefing() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b080; ??_G 0x275230; ProcessEvent 0x1e786a; sizeof 0xc; ctor inline; new at 0x271b0b
class EEUIESIEClose : public UIEvent {
public:
    virtual ~EEUIESIEClose() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b7c4; ??_G 0x27525b; ProcessEvent 0x1e78ea; sizeof 0xc; ctor inline; new at 0x273f31
class EEUIESIEHints : public UIEvent {
public:
    virtual ~EEUIESIEHints() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b7b0; ??_G 0x275286; ProcessEvent 0x1e79ba; sizeof 0xc; ctor inline; new at 0x273f56
class EEUIESIEHistory : public UIEvent {
public:
    virtual ~EEUIESIEHistory() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b79c; ??_G 0x2752b1; ProcessEvent 0x1e7a95; sizeof 0xc; ctor inline; new at 0x273f7b
class EEUIESIEMap : public UIEvent {
public:
    virtual ~EEUIESIEMap() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446a78; ??_G 0x1e8427; ProcessEvent 0x1e7ae7; sizeof 0xc; ctor inline; new at 0x1e7fe3, 0x273f9d
class EEUIESIEMovie : public UIEvent {
public:
    virtual ~EEUIESIEMovie() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446a3c; ??_G 0x1e8452; ProcessEvent 0x1e7b92; sizeof 0xc; ctor inline; new at 0x1e7b18
class EEUIESIESoundOver : public UIEvent {
public:
    virtual ~EEUIESIESoundOver() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b094; ??_G 0x275332; ProcessEvent 0x1e7d65; sizeof 0xc; ctor inline; new at 0x271ae6
class EEUIESIEOpen : public UIEvent {
public:
    virtual ~EEUIESIEOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446a64; ??_G 0x1e847d; ProcessEvent 0x1e8147; sizeof 0xc; ctor inline; new at 0x1e80c4, 0x273fbb
class EEUIESIEStart : public UIEvent {
public:
    virtual ~EEUIESIEStart() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
