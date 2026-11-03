// EEFOFindNewTargetNearOldTarget.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names).
// Classes: EEFOFindNewTargetNearOldTarget.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEFOFINDNEWTARGETNEAROLDTARGET_H
#define EE_EEFOFINDNEWTARGETNEAROLDTARGET_H

#include "ee/EEFOFindNewTargetScreenBase.h"

class EEFOFindNewTargetScreenBase;

// vtable 0x449ba8; bases EEFOFindNewTargetNearOldTarget > EEFOFindNewTargetScreenBase; sizeof ?; ctor ?; ??_G 0x239ec5 (dtor inline)
class EEFOFindNewTargetNearOldTarget : public EEFOFindNewTargetScreenBase {
public:
    // inline (0x241c2d builds one on the stack: both members stored before the vptr, the init list)
    EEFOFindNewTargetNearOldTarget(EEComplexUnit* inTarget, float inDistanceSquared)
        : mTarget(inTarget), mDistanceSquared(inDistanceSquared) {}
    virtual ~EEFOFindNewTargetNearOldTarget() {} // +0x00 (inline: ??_G 0x239ec5)
    virtual bool Virtual04(EEComplexUnit* inUnit);   // +0x04 0x241e21 (alive and nearer to mTarget than mDistanceSquared)
    virtual bool Virtual08(EEComplexUnit* inUnit);   // +0x08 0x241e21 (the same body, ICF)
    EEComplexUnit* mTarget;                 // +0x04 (provisional names)
    float mDistanceSquared;                 // +0x08 (25.0f from 0x241c2d)
};

#endif
