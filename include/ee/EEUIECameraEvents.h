// Mouse-move and camera events (EEUIEDisable/EnableMouseMove, EEUIERevolve/RotateCamera; ProcessEvents 0x1e6711..0x1e688c)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIECAMERAEVENTS_H
#define EE_EEUIECAMERAEVENTS_H

// vtable 0x445030; ??_G 0x19e226; ProcessEvent 0x1e6711; sizeof 0xc; ctor inline; new at 0x19de39, 0x263b64
class EEUIEDisableMouseMove : public UIEvent {
public:
    virtual ~EEUIEDisableMouseMove() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44501c; ??_G 0x19e251; ProcessEvent 0x1e672b; sizeof 0xc; ctor inline; new at 0x19de5f, 0x263b18
class EEUIEEnableMouseMove : public UIEvent {
public:
    virtual ~EEUIEEnableMouseMove() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x445008; ??_G 0x19e27c; ProcessEvent 0x1e6745; sizeof 0xc; ctor inline; new at 0x19de85
class EEUIERevolveCamera : public UIEvent {
public:
    virtual ~EEUIERevolveCamera() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x444ff4; ??_G 0x19e2a7; ProcessEvent 0x1e67dd; sizeof 0xc; ctor inline; new at 0x19deab, 0x263b3e
class EEUIERotateCamera : public UIEvent {
public:
    virtual ~EEUIERotateCamera() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
