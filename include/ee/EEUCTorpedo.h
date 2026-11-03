// EEUCTorpedo.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCTorpedo.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCTORPEDO_H
#define EE_EEUCTORPEDO_H

#include "ee/EEUCMissile.h"

class EEUCMissile;

// vtable 0x448b20; bases EEUCTorpedo > EEUCMissile > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x2363ac; dtor 0x2305ee
class EEUCTorpedo : public EEUCMissile {
public:
    EEUCTorpedo();                          // inline
    EEUCTorpedo(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x21b1f7
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x21b232
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x20d336 (ICF: EEUCBasicCitizen, EEUCBasicPriest...)
    virtual bool CheckGroundImpact(EEComplexUnit* inUnit);  // +0xe0 0x21b163
    virtual bool CheckWaterImpact(EEComplexUnit* inUnit);  // +0xe4 0x29059d trivial: xor al, al; ret 4 (ICF: AnimationComponent@WONAPI, Button@WONAPI...)
};

#endif
