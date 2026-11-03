// C:\EEX\Empire Earth\Scenario Editor\EEUIFScenarioEditorEvents.h (RECONSTRUCTION; file name inferred). The scenario
// editor form's own UI events (each > UIEvent, sizeof 0xc, inline ctor; newed by the form's ctor 0x19d615). Their
// ProcessEvent bodies open EEUIFScenarioEditor.cpp (0x19cc39..); the ??_Gs sit in the form's
// later range. Requires lle/UIEvent.h.
#ifndef EE_EEUIFSCENARIOEDITOREVENTS_H
#define EE_EEUIFSCENARIOEDITOREVENTS_H

// the player combo box's selection changed: the unit tab re-picks, the player tab refreshes its lists
class EEUIENewPlayer : public UIEvent {
public:
    virtual ~EEUIENewPlayer() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x19cc39
};

// the camera back to its home rotation (yaw 45 degrees, pitch -35)
class EEUIEReturnToHomeRotation : public UIEvent {
public:
    virtual ~EEUIEReturnToHomeRotation() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x19cc5e
};

// saves only the scenario's triggers to "ScenarioTriggers.scn"
class EEUIESaveScenarioTriggersOnly : public UIEvent {
public:
    virtual ~EEUIESaveScenarioTriggersOnly() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x19ccc0
};

// loads them back
class EEUIELoadScenarioTriggersOnly : public UIEvent {
public:
    virtual ~EEUIELoadScenarioTriggersOnly() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x19d582
};

#endif
