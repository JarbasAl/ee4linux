// EEUCLandCitizen.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCLandCitizen.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCLANDCITIZEN_H
#define EE_EEUCLANDCITIZEN_H

#include "ee/EEUCBasicCitizen.h"

class EEUCBasicCitizen;

// vtable 0x449184; bases EEUCLandCitizen > EEUCBasicCitizen > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x236308; dtor 0x2305ee
class EEUCLandCitizen : public EEUCBasicCitizen {
public:
    EEUCLandCitizen();                      // inline
    EEUCLandCitizen(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCLandCitizen() {}           // +0x00 dtor 0x2305ee
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x216c48
    // members OPEN: a base's sizeof is unknown (no new site), so this class's own members cannot be placed yet
};

#endif
