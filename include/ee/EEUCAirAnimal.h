// EEUCAirAnimal.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCAirAnimal.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: GE3DPoint.
#ifndef EE_EEUCAIRANIMAL_H
#define EE_EEUCAIRANIMAL_H

#include "ee/EEUCAirplane.h"

class EEComplexUnit;
class EEUCAirplane;

// vtable 0x4480a0; bases EEUCAirAnimal > EEUCAirplane > EEUCLandable > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23649e; dtor 0x2305ee
class EEUCAirAnimal : public EEUCAirplane {
public:
    EEUCAirAnimal();                        // inline
    EEUCAirAnimal(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCAirAnimal() {}             // +0x00 dtor 0x2305ee
    virtual void GroundLocation(GE3DPoint& ioLocation); // +0x04 0x157269 trivial: ret 4 (ICF: EEAECalamityImmunity, EEAEConversionImmunity...)
    virtual bool CanAttackMove();           // +0x10 0xac9f3 trivial: xor al, al; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual float GetHeightAt(EEComplexUnit* inUnit, const GE3DPoint& inLocation); // +0x24 0x20a3c0
    virtual void GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);  // +0x2c 0x20a691
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x20a8de
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x20ad1c
    virtual bool HasArrived(EEComplexUnit* inUnit, float inValue);  // +0x88 0x20ad46
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x20ade9
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x20aff5
    bool ReturnToAirport(EEComplexUnit* inUnit);  // 0x20a279 (Virtual74's command 1000: onto its airport) (provisional name)
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x157269 trivial: ret 4 (ICF: EEAECalamityImmunity, EEAEConversionImmunity...)
    // +0xc0 0x20afe9: EEUnitClass::Animate (the kept copy of an override ICF-folded with the airplane, landable,
    // helicopter, spaceship, sea animal and submerged naval classes' ones; the first in link order)
    virtual long Animate(EEComplexUnit* inUnit);
};

#endif
