// C:\EEX\Empire Earth\Scenario Editor\EESETSEffectType.h (RECONSTRUCTION; file name inferred). The base of the
// trigger-effect editors (RTTI EESETSEffectType, a root class; vtable 0x443cc4, seven slots). Slot evidence: +0x00 the
// scalar deleting destructor 0x1864b3 (the derived dtors store both vptrs and call nothing: the dtor is inline and
// empty); +0x04 and +0x18 are 0xac9c8 (`ret 8`: an inline empty virtual taking two
// arguments); +0x08, +0x0c, +0x10, +0x14 are 0x29d172 (_purecall). The derived overrides
// return with ret 8 at +0x04/+0x08/+0x10/+0x18 and ret 4 at +0x0c/+0x14; no caller reads eax.
// Arguments (EESETSEffect, 0x182a3b..0x182d69): the effect is gWorld.mScenarioManager->GetEffect(mIndex + 1)
// (EEScenarioManager::GetEffect, COMDAT 0x182d82), the controls are EESETSEffect's +0x2c (ee/EESETSEffectControls.h).
// Slot use there: +0x04 (ChangeEffectAction 0x182a3b), +0x0c (controls) on the old type and +0x14 (controls) on the
// new one when the type changes, +0x10 (effect, controls) after it (0x182a69, 0x182c55), +0x18 (effect, controls)
// 0x182d40. Slot names by offset (provisional name).
// The types (EESETSEffect::mTypes, its ctor 0x182629): [0] EESETSETNoEffect, [1] EESETSETObject, [2] EESETSETPlayer,
// [3] EESETSETTrigger, [4] EESETSETMedia, [5] EESETSETGame, [6] EESETSETAI (ee/EESETSET<Name>.h, one header per family).
#ifndef EE_EESETSEFFECTTYPE_H
#define EE_EESETSEFFECTTYPE_H

class EESETSEffectControls;
class EESMEffect;

class EESETSEffectType
{
public:
    virtual ~EESETSEffectType() {}                                                              // +0x00 (??_G 0x1864b3)
    virtual void ChangeAction(EESMEffect* inEffect, EESETSEffectControls* inControls) {}        // +0x04 (0xac9c8)
    virtual void Store(EESMEffect* inEffect, EESETSEffectControls* inControls) = 0;             // +0x08
    virtual void VirtualC(EESETSEffectControls* inControls) = 0;                                // +0x0c
    virtual void Load(EESMEffect* inEffect, EESETSEffectControls* inControls) = 0;              // +0x10
    virtual void Show(EESETSEffectControls* inControls) = 0;                                    // +0x14
    virtual void UpdateName(EESMEffect* inEffect, EESETSEffectControls* inControls) {}          // +0x18 (0xac9c8)
};

#endif
