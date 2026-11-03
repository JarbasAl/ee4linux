// EEUCLand.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes: EEUCLand.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCLAND_H
#define EE_EEUCLAND_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnitClass;

// vtable 0x449268; bases EEUCLand > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x236414; dtor 0x2305ee
class EEUCLand : public EEUnitClass {
public:
    EEUCLand();                             // inline
    EEUCLand(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1; out of line 0x2362c3)
    virtual ~EEUCLand() {}                  // +0x00 dtor 0x2305ee
    virtual bool CanAttackMove();           // +0x10 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x2138c0 (ICF: EEUCLandMilitaryHero)
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x213866 (ICF: EEUCLandMilitaryHero)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x213abf
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x213a94 (ICF: EEUCLandMilitaryHero)
    // +0xe0 0x21393c NEW (ICF: EEUCLandMilitaryHero): the paratroopers' plane (class 0x2e1) flies in for inUnit
    virtual void CallParatroopPlane(EEComplexUnit* inUnit);
};

#endif
