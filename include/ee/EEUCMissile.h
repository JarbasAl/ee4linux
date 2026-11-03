// EEUCMissile.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCMissile.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCMISSILE_H
#define EE_EEUCMISSILE_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnitClass;

// vtable 0x448cf8; bases EEUCMissile > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x2363ac; dtor 0x2305ee
class EEUCMissile : public EEUnitClass {
public:
    virtual ~EEUCMissile() {}
    EEUCMissile();                          // inline
    EEUCMissile(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1; out of line 0x23636a)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x21765d (ICF: EEUCTorpedo)
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x217916 (ICF: EEUCTorpedo)
    virtual void AddToRegion(EEUnit*);     // +0x80 0x156dd8 trivial: ret 4 (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual void RemoveFromRegion(EEUnit*);  // +0x94 0x156dd8 trivial: ret 4 (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x217a37
    virtual bool LaunchProjectile(EEComplexUnit* inUnit, const GE3DPoint& inLocation);  // +0xac 0x21799b (ICF: EEUCTorpedo)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x217bb2
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x217b77
    virtual bool CheckGroundImpact(EEComplexUnit* inUnit);  // +0xe0 0x217337 NEW
    virtual bool CheckWaterImpact(EEComplexUnit* inUnit);  // +0xe4 0x2175cb NEW
};

#endif
