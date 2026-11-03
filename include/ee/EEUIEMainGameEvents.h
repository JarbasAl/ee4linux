// The main game form's events (class prefix EEUIEMG; RECONSTRUCTION; file name inferred).
// EEUIEMainGameEvents.cpp sorts inside the User Interface Thread group (0x2609e5..). The classes are declared in the
// ProcessEvent order (which is also the main form's ??_G order). Requires lle/UIEvent.h.
#ifndef EE_EEUIEMAINGAMEEVENTS_H
#define EE_EEUIEMAINGAMEEVENTS_H

// vtable 0x44aa64; ??_G 0x2662ac; ProcessEvent 0x25fdb3; sizeof 0xc (the main game form creates it)
class EEUIEMGCancelUIMode : public UIEvent {
public:
    virtual ~EEUIEMGCancelUIMode() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x444f90; ??_G 0x19e1a5; ProcessEvent 0x25fe4d; sizeof 0x10. The main screen's Open adds two hot keys with
// it (0x1e2089: false, 0x1e20cd: true); the ctor stores +0x0c and +0x0d before the vptr (init list).
class EEUIEMGDumpLoadedMedia : public UIEvent {
public:
    EEUIEMGDumpLoadedMedia(bool inFlag, bool inDumpLeaks = true) : mFlag(inFlag), mDumpLeaks(inDumpLeaks) {}  // the scenario editor passes false (0x19e082, 0x19e0c0)
    virtual ~EEUIEMGDumpLoadedMedia() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    bool mFlag;                 // +0x0c (provisional name)
    bool mDumpLeaks;            // +0x0d (provisional name)
};

// vtable 0x44a920; ??_G 0x26443e; ProcessEvent 0x25ff64; sizeof 0xc (the main game form creates it)
class EEUIEMGDisplayChat : public UIEvent {
public:
    virtual ~EEUIEMGDisplayChat() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a880; ??_G 0x264469; ProcessEvent 0x25ffc0; sizeof 0xc (the main game form creates it)
class EEUIEMGDisplayOptions : public UIEvent {
public:
    virtual ~EEUIEMGDisplayOptions() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a970; ??_G 0x264494; ProcessEvent 0x260051; sizeof 0xc (the main game form creates it)
class EEUIEMGEnterShiftMode : public UIEvent {
public:
    virtual ~EEUIEMGEnterShiftMode() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a984; ??_G 0x2644bf; ProcessEvent 0x2600b7; sizeof 0xc (the main game form creates it)
class EEUIEMGExitShiftMode : public UIEvent {
public:
    virtual ~EEUIEMGExitShiftMode() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a8f8; ??_G 0x2644ea; ProcessEvent 0x2600f0; sizeof 0xc (the main game form creates it)
class EEUIEMGFlareMode : public UIEvent {
public:
    virtual ~EEUIEMGFlareMode() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a8bc; ??_G 0x264515; ProcessEvent 0x260215; sizeof 0xc (the main game form creates it)
class EEUIEMGEnterScenarioEditor : public UIEvent {
public:
    virtual ~EEUIEMGEnterScenarioEditor() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a894; ??_G 0x264540; ProcessEvent 0x26031d; sizeof 0x10 (the main game form creates it)
class EEUIEMGInGameLoadAndSave : public UIEvent {
public:
    EEUIEMGInGameLoadAndSave(long inMode) : mMode(inMode) {}   // init list: stored before the vptr
    virtual ~EEUIEMGInGameLoadAndSave() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mMode;                 // +0x0c (0, 1, 2: the three hot keys; provisional name)
};

// vtable 0x43e43c; ??_G 0xcc1ce (EEServer.cpp, its first user: the server posts it for a pause message);
// ProcessEvent 0x260566; sizeof 0xc
class EEUIEMGPauseGame : public UIEvent {
public:
    virtual ~EEUIEMGPauseGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a934; ??_G 0x26456b; ProcessEvent 0x2605ba; sizeof 0xc (the main game form creates it)
class EEUIEMGCancelUpdateState : public UIEvent {
public:
    virtual ~EEUIEMGCancelUpdateState() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a86c; ??_G 0x264596; ProcessEvent 0x26061c; sizeof 0xc (the main game form creates it)
class EEUIEMGToggleGameClock : public UIEvent {
public:
    virtual ~EEUIEMGToggleGameClock() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a95c; ??_G 0x2645c1; ProcessEvent 0x260650; sizeof 0x10 (the main game form creates it)
class EEUIEMGKillUnits : public UIEvent {
public:
    EEUIEMGKillUnits(bool inFlag) : mFlag(inFlag) {}
    virtual ~EEUIEMGKillUnits() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    bool mFlag;                 // +0x0c (provisional name)
};

// vtable 0x44a8e4; ??_G 0x2645ec; ProcessEvent 0x2606b2; sizeof 0x14 (the main game form creates it)
class EEUIEMGAttackPlayer : public UIEvent {
public:
    EEUIEMGAttackPlayer(bool inFlag) : mL0C(0), mFlag(inFlag) {}
    virtual ~EEUIEMGAttackPlayer() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mL0C;                  // +0x0c (= 0) (provisional name)
    bool mFlag;                 // +0x10 (provisional name)
};

// vtable 0x44a9d4; ??_G 0x264617; ProcessEvent 0x260a12; sizeof 0x10 (the main game form creates it)
class EEUIEMGUnitBehavior : public UIEvent {
public:
    EEUIEMGUnitBehavior(long inBehavior) { this->mBehavior = inBehavior; }  // body: stored after the vptr
    virtual ~EEUIEMGUnitBehavior() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mBehavior;             // +0x0c (0, 1, 2, 5) (provisional name)
};

// vtable 0x444fa4; ??_G 0x19e1d0; ProcessEvent 0x26072f; sizeof 0xc (the main game form creates it)
class EEUIEMGToggleShowHiddenUnits : public UIEvent {
public:
    virtual ~EEUIEMGToggleShowHiddenUnits() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a90c; ??_G 0x264642; ProcessEvent 0x2607e0; sizeof 0x10 (the main game form creates it)
class EEUIEMGFollowUnit : public UIEvent {
public:
    EEUIEMGFollowUnit(bool inFlag) : mFlag(inFlag) {}
    virtual ~EEUIEMGFollowUnit() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    bool mFlag;                 // +0x0c (= false) (provisional name)
};

// vtable 0x44a8a8; ??_G 0x26466d; ProcessEvent 0x26085e; sizeof 0xc (the main game form creates it)
class EEUIEMGTogglePerspective : public UIEvent {
public:
    virtual ~EEUIEMGTogglePerspective() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a948; ??_G 0x264698; ProcessEvent 0x2608f4; sizeof 0x10 (the main game form creates it)
class EEUIEMGChangeGameSpeed : public UIEvent {
public:
    EEUIEMGChangeGameSpeed(bool inFaster) : mFaster(inFaster) {}
    virtual ~EEUIEMGChangeGameSpeed() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    bool mFaster;               // +0x0c (true: key 0x4e, false: 0x4a) (provisional name)
};

// vtable 0x44a9ac; ??_G 0x2646c3; ProcessEvent 0x260c08; sizeof 0xc (the main game form creates it)
class EEUIEMGShowPreviousMessages : public UIEvent {
public:
    virtual ~EEUIEMGShowPreviousMessages() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
