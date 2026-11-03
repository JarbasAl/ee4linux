// The main screen's events (class prefixes EEUIESSE/MSE/MS; ProcessEvents 0x1e1b8d..0x1e2292)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEMAINSCREENEVENTS_H
#define EE_EEUIEMAINSCREENEVENTS_H

// vtable 0x44b044; ??_G 0x271cd8; ProcessEvent 0x1e1b8d; sizeof 0xc; ctor inline; new at 0x271b7a
class EEUIESSEOpen : public UIEvent {
public:
    virtual ~EEUIESSEOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b5f8; ??_G 0x2736b3; ProcessEvent 0x1e1e48; sizeof 0xc; ctor inline; new at 0x27351c
class EEUIEMSEGameSettings : public UIEvent {
public:
    virtual ~EEUIEMSEGameSettings() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b5e4; ??_G 0x2736de; ProcessEvent 0x1e1ed8; sizeof 0xc; ctor inline; new at 0x273541
class EEUIEMSEExitGame : public UIEvent {
public:
    virtual ~EEUIEMSEExitGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b5bc; ??_G 0x273709; ProcessEvent 0x1e1ee8; sizeof 0xc; ctor inline; new at 0x2735b0
class EEUIEMSESinglePlayer : public UIEvent {
public:
    virtual ~EEUIEMSESinglePlayer() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x441eec; ??_G 0x12d57a; ProcessEvent 0x1e1f8a; sizeof 0xc; ctor inline; new at 0x12d4bf, 0x12d641, 0x273566
class EEUIEMSEMultiPlayer : public UIEvent {
public:
    virtual ~EEUIEMSEMultiPlayer() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44b0f8; ??_G 0x271d03; ProcessEvent 0x1e2035; sizeof 0xc; ctor inline; new at 0x271a2d
class EEUIEMSEOpen : public UIEvent {
public:
    virtual ~EEUIEMSEOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4468e0; ??_G 0x1e200a; ProcessEvent 0x1e2176; sizeof 0xc; ctor inline; new at 0x1e1fb9
class EEUIEMSOpenMultiplayerScreen : public UIEvent {
public:
    virtual ~EEUIEMSOpenMultiplayerScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
