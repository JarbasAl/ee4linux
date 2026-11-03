// EEUCHelicopter.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCHelicopter.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCHELICOPTER_H
#define EE_EEUCHELICOPTER_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnitClass;

// vtable 0x448fbc; bases EEUCHelicopter > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23638a; dtor 0x2305ee
class EEUCHelicopter : public EEUnitClass {
public:
    EEUCHelicopter();                       // inline
    EEUCHelicopter(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCHelicopter() {}            // +0x00 dtor 0x2305ee
    virtual bool CanAttackMove();           // +0x10 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual float GetHeightAt(EEComplexUnit* inUnit, const GE3DPoint& inLocation); // +0x24 0x212a6f
    virtual void GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);  // +0x2c 0x21a404 (ICF: EEUCHelicopterTransport, EEUCSpaceship)
    virtual bool FliesOver();               // +0x5c 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x212c5a
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x21a51a (ICF: EEUCSpaceship)
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x21aaa9 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x21aaf5 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x2137f8 (ICF: EEUCHelicopterTransport)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x212deb
    virtual long Animate(EEComplexUnit*);   // +0xc0 0x20afe9 trivial: push dword ptr [esp + 4]; call 0x638ce7; ret 4 (ICF: EEUCAirAnimal, EEUCAirplane...)
};

#endif
