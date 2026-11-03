// EEUCSpaceship.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCSpaceship.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCSPACESHIP_H
#define EE_EEUCSPACESHIP_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnitClass;

// vtable 0x447ed4; bases EEUCSpaceship > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23645a; dtor 0x2305ee
class EEUCSpaceship : public EEUnitClass {
public:
    virtual ~EEUCSpaceship() {}
    EEUCSpaceship();                        // inline
    EEUCSpaceship(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual bool CanAttackMove();           // +0x10 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual float GetHeightAt(EEComplexUnit* inUnit, const GE3DPoint& inLocation); // +0x24 0x21a219
    virtual void GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);  // +0x2c 0x21a404 (ICF: EEUCHelicopter, EEUCHelicopterTransport)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x21a436
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x21a51a (ICF: EEUCHelicopter)
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x21aaa9 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x21aaf5 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x21a59e
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x21a5dd
    virtual long Animate(EEComplexUnit*);   // +0xc0 0x20afe9 trivial: push dword ptr [esp + 4]; call 0x638ce7; ret 4 (ICF: EEUCAirAnimal, EEUCAirplane...)
};

// 0x242110 (cdecl): the space version of the sound volume/pan/pitch (EEUCSpaceship::GetSoundParameters) (provisional
// name)
void EEGetDopplerPitch(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);

#endif
