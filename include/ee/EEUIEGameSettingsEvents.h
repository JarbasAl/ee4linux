// The game settings form's events (class prefix EEUIEGSE; ProcessEvents 0x1d6b94..0x1d99ff)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
// Checked by EEUIEGameSettingsEvents.cpp: the unit holds the ProcessEvents in this
// order (0x1d6bc0..0x1d791d) and the ??_G of the three classes it creates (InGameOk 0x1d6f51, OutOfGameOk 0x1d7119,
// VideoTab 0x1d865a, each right after its creator); EEUIManager's unit emits the others' ??_G 0x27180c..0x2719e5 in
// this declaration order. The empty inline virtual dtors give the 0x2b-byte ??_G (three vptr stores).
#ifndef EE_EEUIEGAMESETTINGSEVENTS_H
#define EE_EEUIEGAMESETTINGSEVENTS_H

// vtable 0x44af68; ??_G 0x27180c; ProcessEvent 0x1d6bc0; sizeof 0xc; ctor inline; new at 0x271660
class EEUIEGSEAudioTab : public UIEvent {
public:
    virtual ~EEUIEGSEAudioTab() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af40; ??_G 0x271837; ProcessEvent 0x1d6c0f; sizeof 0xc; ctor inline; new at 0x2716cf
class EEUIEGSECancel : public UIEvent {
public:
    virtual ~EEUIEGSECancel() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446448; ??_G 0x1d6f51; ProcessEvent 0x1d6cca; sizeof 0xc; ctor inline; new at 0x1d6e94
class EEUIEGSEInGameOk : public UIEvent {
public:
    virtual ~EEUIEGSEInGameOk() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4467a0; ??_G 0x1e13dc; ProcessEvent 0x1d6e34; sizeof 0xc; ctor inline; new at 0x1e137d
class EEUIEGSEInGameOpen : public UIEvent {
public:
    virtual ~EEUIEGSEInGameOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af7c; ??_G 0x271862; ProcessEvent 0x1d6f7c; sizeof 0xc; ctor inline; new at 0x27163b
class EEUIEGSEInterfaceTab : public UIEvent {
public:
    virtual ~EEUIEGSEInterfaceTab() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af04; ??_G 0x27188d; ProcessEvent 0x1d6fcb; sizeof 0xc; ctor inline; new at 0x27176b
class EEUIEGSEMusicQualityRadio : public UIEvent {
public:
    virtual ~EEUIEGSEMusicQualityRadio() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af2c; ??_G 0x2718b8; ProcessEvent 0x1d7020; sizeof 0xc; ctor inline; new at 0x2716f4
class EEUIEGSEMusicVolumeSlider : public UIEvent {
public:
    virtual ~EEUIEGSEMusicVolumeSlider() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44645c; ??_G 0x1d7119; ProcessEvent 0x1d7082; sizeof 0xc; ctor inline; new at 0x1d70cc
class EEUIEGSEOutOfGameOk : public UIEvent {
public:
    virtual ~EEUIEGSEOutOfGameOk() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4468b8; ??_G 0x1e1ead; ProcessEvent 0x1d70ac; sizeof 0xc; ctor inline; new at 0x1e1e4d
class EEUIEGSEOutOfGameOpen : public UIEvent {
public:
    virtual ~EEUIEGSEOutOfGameOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44afa4; ??_G 0x2718e3; ProcessEvent 0x1d7144; sizeof 0xc; ctor inline; new at 0x2715f1
class EEUIEGSERenderingSystemListBox : public UIEvent {
public:
    virtual ~EEUIEGSERenderingSystemListBox() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af54; ??_G 0x27190e; ProcessEvent 0x1d7689; sizeof 0xc; ctor inline; new at 0x2716aa
class EEUIEGSERestoreDefault : public UIEvent {
public:
    virtual ~EEUIEGSERestoreDefault() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44aef0; ??_G 0x271939; ProcessEvent 0x1d76d7; sizeof 0xc; ctor inline; new at 0x27178d
class EEUIEGSESoundVolumeSlider : public UIEvent {
public:
    virtual ~EEUIEGSESoundVolumeSlider() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af18; ??_G 0x271964; ProcessEvent 0x1d7709; sizeof 0xc; ctor inline; new at 0x271746
class EEUIEGSETestSoundVolume : public UIEvent {
public:
    virtual ~EEUIEGSETestSoundVolume() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44af90; ??_G 0x27198f; ProcessEvent 0x1d777b; sizeof 0xc; ctor inline; new at 0x271616
class EEUIEGSEVideoCardComboBox : public UIEvent {
public:
    virtual ~EEUIEGSEVideoCardComboBox() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446470; ??_G 0x1d865a; ProcessEvent 0x1d7841; sizeof 0xc; ctor inline; new at 0x1d8606, 0x271685
class EEUIEGSEVideoTab : public UIEvent {
public:
    virtual ~EEUIEGSEVideoTab() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44aedc; ??_G 0x2719ba; ProcessEvent 0x1d789c; sizeof 0xc; ctor inline; new at 0x2717ab
class EEUIEGSEGraphicsQuality : public UIEvent {
public:
    virtual ~EEUIEGSEGraphicsQuality() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44aec8; ??_G 0x2719e5; ProcessEvent 0x1d791d; sizeof 0xc; ctor inline; new at 0x2717e9
class EEUIEGSEAdvancedGraphicsSettingsChanged : public UIEvent {
public:
    virtual ~EEUIEGSEAdvancedGraphicsSettingsChanged() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
