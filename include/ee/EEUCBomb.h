// EEUCBomb.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes: EEUCBomb.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCBOMB_H
#define EE_EEUCBOMB_H

#include "ee/EEUCProjectile.h"

class EEUCProjectile;

// vtable 0x448de4; bases EEUCBomb > EEUCProjectile > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x236348; dtor 0x2305ee
class EEUCBomb : public EEUCProjectile {
public:
    EEUCBomb();                             // inline
    EEUCBomb(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCBomb() {}                  // +0x00 dtor 0x2305ee
    virtual bool LaunchProjectile(EEComplexUnit* inUnit, const GE3DPoint& inLocation);  // +0xac 0x20e2cb
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x20e446
};

#endif
