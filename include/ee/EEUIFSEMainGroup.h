// C:\EEX\Empire Earth\Scenario Editor\EEUIFSEMainGroup.h (RECONSTRUCTION; file name inferred). The base of
// the scenario editor's tab groups (RTTI EEUIFSEMainGroup, no base; vtable 0x442e14: slot 0 the scalar deleting dtor
// 0x1720c7, +0x0c `ret 4`, +0x24 pure, the rest the shared empty slot 0x1720c6). The editor holds them in
// EEUIFScenarioEditor::mTabGroups. Virtual names by slot; arguments of the empty slots unknown.
#ifndef EE_EEUIFSEMAINGROUP_H
#define EE_EEUIFSEMAINGROUP_H

class EEUIFSEMainGroup {
public:
    virtual ~EEUIFSEMainGroup() {}          // +0x00
    virtual void Update() {}                // +0x04
    virtual void Leave() {}                 // +0x08
    virtual void Draw(const GERectangle& inRectangle) {}        // +0x0c (EESETriggerScreen: the dirty rectangle)
    virtual void Disable() {}               // +0x10
    virtual void Load() {}                  // +0x14 (EEUIFScenarioEditor::ResumeCurrentGroup)
    virtual void Enter() {}                 // +0x18
    virtual void Store() {}                 // +0x1c (EEUIFScenarioEditor::RefreshCurrentGroup)
    virtual void Enable() {}                // +0x20
    virtual void MouseMove(const GE2DPoint& inLocation) = 0;   // +0x24 (EESETriggerScreen 0x17ca79: `ret 4`)
    virtual void UpdateKeys() {}            // +0x28
};

#endif
