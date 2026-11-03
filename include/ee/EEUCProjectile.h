// EEUCProjectile.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCProjectile.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCPROJECTILE_H
#define EE_EEUCPROJECTILE_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnitClass;

// vtable 0x448ed0; bases EEUCProjectile > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x236348; dtor 0x2305ee
class EEUCProjectile : public EEUnitClass {
public:
    virtual ~EEUCProjectile() {}
    EEUCProjectile();                       // inline
    EEUCProjectile(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x219388 (ICF: EEUCBomb)
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x2197d3 (ICF: EEUCBomb)
    virtual void AddToRegion(EEUnit*);     // +0x80 0x156dd8 trivial: ret 4 (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual void RemoveFromRegion(EEUnit*);  // +0x94 0x156dd8 trivial: ret 4 (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x2199a5 (ICF: EEUCBomb)
    virtual bool LaunchProjectile(EEComplexUnit* inUnit, const GE3DPoint& inLocation);  // +0xac 0x219850
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x219b4a
    virtual void UpdateEffects(EEComplexUnit* inUnit);  // +0xbc 0x219ae9 (ICF: EEUCBomb)
    // +0xe0 0x219143 NEW (ICF: EEUCBomb): the projectile reached the ground (an attack-ground goal) (provisional name)
    virtual bool CheckGroundImpact(EEComplexUnit* inUnit);
    // +0xe4 0x2192d0 NEW (ICF: EEUCBomb): it is below the ground (provisional name)
    virtual bool CheckWaterImpact(EEComplexUnit* inUnit);
};

#endif
