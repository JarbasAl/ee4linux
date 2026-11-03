// The in-game options menu's events (class prefix EEUIEMGOE; ProcessEvents 0x1e0e1b..0x1e1b8d)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Declaration order: the order of the ??_G block VC6 emits after OpenOptionsScreen (0x1e193a..0x1e1b3e),
// so CloseOptions sits between GameBriefing and GameSettings. EEUIEMGOEOpenOptionsScreen is created by the main game
// form (0x25ffc5) and EEUIEMGDisplayScenarioIntro by the UI manager. Requires lle/UIEvent.h.
#ifndef EE_EEUIEMAINGAMEOPTIONSEVENTS_H
#define EE_EEUIEMAINGAMEOPTIONSEVENTS_H

// vtable 0x446890; ??_G 0x1e193a; ProcessEvent 0x1e0e1b; sizeof 0xc; ctor inline; new at 0x1e1575
class EEUIEMGOEReturnToGame : public UIEvent {
public:
    virtual ~EEUIEMGOEReturnToGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44687c; ??_G 0x1e1965; ProcessEvent 0x1e0e2b; sizeof 0xc; ctor inline; new at 0x1e15b4
class EEUIEMGOEResign : public UIEvent {
public:
    virtual ~EEUIEMGOEResign() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4467dc; ??_G 0x1e1990; ProcessEvent 0x1e0ed9; sizeof 0xc; ctor inline; new at 0x1e17f8
class EEUIEMGOERestartGame : public UIEvent {
public:
    virtual ~EEUIEMGOERestartGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4467c8; ??_G 0x1e19bb; ProcessEvent 0x1e0f38; sizeof 0xc; ctor inline; new at 0x1e183e
class EEUIEMGOEReturnToEditor : public UIEvent {
public:
    virtual ~EEUIEMGOEReturnToEditor() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446804; ??_G 0x1e19e6; ProcessEvent 0x1e0fec; sizeof 0xc; ctor inline; new at 0x1e178a
class EEUIEMGOELoadSavedGame : public UIEvent {
public:
    virtual ~EEUIEMGOELoadSavedGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446818; ??_G 0x1e1a11; ProcessEvent 0x1e108a; sizeof 0xc; ctor inline; new at 0x1e16cc
class EEUIEMGOESaveGame : public UIEvent {
public:
    virtual ~EEUIEMGOESaveGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446868; ??_G 0x1e1a3c; ProcessEvent 0x1e1167; sizeof 0xc; ctor inline; new at 0x1e15e9
class EEUIEMGOEPauseGame : public UIEvent {
public:
    virtual ~EEUIEMGOEPauseGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446854; ??_G 0x1e1a67; ProcessEvent 0x1e11ef; sizeof 0xc; ctor inline; new at 0x1e161e
class EEUIEMGOEPreviousMessages : public UIEvent {
public:
    virtual ~EEUIEMGOEPreviousMessages() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4467f0; ??_G 0x1e1a92; ProcessEvent 0x1e120e; sizeof 0x10; ctor inline; new at 0x1e17bf
class EEUIEMGOELoadScenario : public UIEvent {
public:
    EEUIEMGOELoadScenario() : mPosted(false) {}   // byte +0x0c cleared before the vptr store (init list)
    virtual ~EEUIEMGOELoadScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    bool mPosted;               // +0x0c the first run re-posts the event and sets it (provisional name)
};

// vtable 0x44b4cc; ??_G 0x273430; ProcessEvent 0x1e136e (ICF-shared with EEUIEMGOEGameBriefing) trivial: call 0x67a470; xor eax, eax; ret 8; sizeof 0xc; ctor inline; new at 0x2730e1
class EEUIEMGDisplayScenarioIntro : public UIEvent {
public:
    virtual ~EEUIEMGDisplayScenarioIntro() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x446840; ??_G 0x1e1abd; ProcessEvent 0x1e136e (ICF-shared with EEUIEMGDisplayScenarioIntro) trivial: call 0x67a470; xor eax, eax; ret 8; sizeof 0xc; ctor inline; new at 0x1e1653
class EEUIEMGOEGameBriefing : public UIEvent {
public:
    virtual ~EEUIEMGOEGameBriefing() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4468a4; ??_G 0x1e1ae8; ProcessEvent 0x27cf38 (ICF-shared with EEPPlayerEvent, EEUGAttackGround, EEUGExplore...) trivial: xor eax, eax; ret 8; sizeof 0xc; ctor inline; new at 0x1e1544
class EEUIEMGOECloseOptions : public UIEvent {
public:
    virtual ~EEUIEMGOECloseOptions() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44682c; ??_G 0x1e1b13; ProcessEvent 0x1e1378; sizeof 0xc; ctor inline; new at 0x1e1688
class EEUIEMGOEGameSettings : public UIEvent {
public:
    virtual ~EEUIEMGOEGameSettings() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4467b4; ??_G 0x1e1b3e; ProcessEvent 0x1e1407; sizeof 0xc; ctor inline; new at 0x1e18b1
class EEUIEMGOEExitGame : public UIEvent {
public:
    virtual ~EEUIEMGOEExitGame() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44a624; ??_G 0x260026; ProcessEvent 0x1e14cb; sizeof 0xc; ctor inline; new at 0x25ffc5
class EEUIEMGOEOpenOptionsScreen : public UIEvent {
public:
    virtual ~EEUIEMGOEOpenOptionsScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
