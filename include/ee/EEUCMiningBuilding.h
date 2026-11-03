// EEUCMiningBuilding.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCMiningBuilding.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: GETransformation.
#ifndef EE_EEUCMININGBUILDING_H
#define EE_EEUCMININGBUILDING_H

#include "ee/EEUCBuilding.h"

class EEComplexUnit;
class EEUCBuilding;
class EEUnit;

// vtable 0x44843c; bases EEUCMiningBuilding > EEUCBuilding > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23626a; dtor 0x2305ee
class EEUCMiningBuilding : public EEUCBuilding {
public:
    EEUCMiningBuilding();                   // inline
    EEUCMiningBuilding(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual EEUnit* CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator); // +0x30 0x2171b6
    virtual EEUnit* CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator); // +0x34 0x217085
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x217231
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x21726d
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x2172a5
};

#endif
