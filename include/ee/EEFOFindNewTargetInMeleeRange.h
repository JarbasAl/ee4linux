// EEFOFindNewTargetInMeleeRange.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names).
// Classes: EEFOFindNewTargetInMeleeRange.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEFOFINDNEWTARGETINMELEERANGE_H
#define EE_EEFOFINDNEWTARGETINMELEERANGE_H

#include "ee/EEFOFindNewTargetScreenBase.h"

class EEFOFindNewTargetScreenBase;

class EEUnit;

// vtable 0x449aa8; bases EEFOFindNewTargetInMeleeRange > EEFOFindNewTargetScreenBase; sizeof 0x10; ctor 0x23795f; ??_G 0x239ec5 (dtor inline)
// (the ctor stores [+4]/[+8] from its two arguments, then [+0xc] = EEGetHitDistanceSquared(arg1, arg2, -10000.0f,
// -10000.0f); Virtual04/08 compare EEGetHitDistanceSquared(mUnit, inUnit, ..) against [+0xc])
class EEFOFindNewTargetInMeleeRange : public EEFOFindNewTargetScreenBase {
public:
    EEFOFindNewTargetInMeleeRange(EEComplexUnit* inUnit, EEUnit* inTarget);   // 0x23795f (out of line, EEUnitClass.cpp)
    virtual ~EEFOFindNewTargetInMeleeRange() {} // +0x00 (inline: ??_G 0x239ec5)
    virtual bool Virtual04(EEComplexUnit* inUnit);   // +0x04 0x23799d (alive and nearer than mRange)
    virtual bool Virtual08(EEComplexUnit* inUnit);   // +0x08 0x23799d (the same body, ICF)
    EEComplexUnit* mUnit;                   // +0x04
    EEUnit* mTarget;                        // +0x08
    float mRange;                           // +0x0c
};

#endif
