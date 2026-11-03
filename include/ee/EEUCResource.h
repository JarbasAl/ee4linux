// EEUCResource.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCResource.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCRESOURCE_H
#define EE_EEUCRESOURCE_H

#include "ee/EEUnit.h"

class EEUnitClass;

// vtable 0x449608; bases EEUCResource > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23618b; dtor 0x2305ee
class EEUCResource : public EEUnitClass {
public:
    virtual ~EEUCResource() {}
    EEUCResource();                         // inline
    EEUCResource(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual long Virtual70() { return 2; }   // +0x70 inline (COMDAT 0x236187 in EEUnitClass.cpp's range: the factory emits the vtable)
    virtual void SimpleUnitCreated(EEUnit* inUnit);  // +0x7c 0x219c49 (the unit's +0x38/+0x40 from the class)
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x219c7e (the unit enters every tile of its footprint)
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x20bbd0 (ICF: EEUCAmbient)
};

#endif
