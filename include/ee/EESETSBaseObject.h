// C:\EEX\Empire Earth\Scenario Editor\EESETSBaseObject.h (RECONSTRUCTION; file name inferred). The base of the
// trigger screen's five sub-screens (RTTI EESETSBaseObject, a root class; vtable 0x443858: slot 0 the ??_G
// 0x17da35 of an inline empty dtor, 14 pure slots, the shared empty slot 0x1720c6 at
// +0x08/+0x3c/+0x44 and the `ret 4` slot 0x156dd8 at +0x40). Created by EESETriggerScreen's factory 0x17c1e5:
// 0 EESETSTrigger, 1 EESETSCondition, 2 EESETSEffect, 3 EESETSObject, 4 EESETSArea (EESETriggerScreen::mScreens).
// The inline ctor stores +0x04 and +0x08 before the vptr (an init list; EESETSArea 0x17ccf3, EESETSObject 0x18efb7).
// Slot names by offset; signatures from `ret N` and the bodies of the overrides (provisional name).
#ifndef EE_EESETSBASEOBJECT_H
#define EE_EESETSBASEOBJECT_H

class EESETSBaseObject {
public:
    EESETSBaseObject() : mSelection(0), mHasSelection(false) {}
    virtual ~EESETSBaseObject() {}          // +0x00
    virtual void Sort() = 0;                // +0x04
    virtual void Update() {}                // +0x08 (0x1720c6)
    virtual void MoveDown() = 0;            // +0x0c
    virtual void Delete() = 0;              // +0x10 (EESETSCondition: the scenario manager's bool 0x1b0f3f)
    virtual void Deactivate() = 0;          // +0x14
    virtual void Duplicate() = 0;           // +0x18
    virtual void GetNames(std::vector<UWideString>& outNames) = 0;  // +0x1c (the manager fills the names: 0x1b3656/0x1b3726)
    virtual void Activate() = 0;            // +0x20
    virtual void Store() = 0;               // +0x24 (EESETSTrigger's index steps call it first)
    virtual void Hide() = 0;                // +0x28
    virtual void Load() = 0;                // +0x2c (EESETSTrigger's index steps call it last)
    virtual void MoveUp() = 0;              // +0x30
    virtual void New() = 0;                 // +0x34
    virtual void Show() = 0;                // +0x38
    virtual void UpdateKeys() {}            // +0x3c (0x1720c6)
    virtual void MouseMove(const GE2DPoint& inLocation) {}   // +0x40 (0x156dd8; EESETSArea: the mouse location)
    virtual void RefreshControls() {}       // +0x44 (0x1720c6)

    long mSelection;                        // +0x04
    bool mHasSelection;                     // +0x08
};

#endif
