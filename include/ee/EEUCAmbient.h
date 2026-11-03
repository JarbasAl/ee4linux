// EEUCAmbient.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCAmbient.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: GE3DPoint.
#ifndef EE_EEUCAMBIENT_H
#define EE_EEUCAMBIENT_H

#include "ee/EEUnit.h"

class EEUnitClass;

// vtable 0x4496ec; bases EEUCAmbient > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x236165; dtor 0x2305ee
class EEUCAmbient : public EEUnitClass {
public:
    EEUCAmbient();                          // inline
    EEUCAmbient(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCAmbient() {}               // +0x00 dtor 0x2305ee
    virtual void GroundLocation(GE3DPoint& ioLocation); // +0x04 0x20b9fc
    virtual void LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY); // +0x08 0x20ba14
    virtual bool IsPickable();              // +0x60 0x27c0b4 trivial: xor al, al; ret (ICF: EEAECloaking, EEAEEconomy...)
    virtual bool IsSelectable(EEUnit* inUnit); // +0x64 (a unit: EEPlayer::SelectUnit 0x16a53b) 0x20bac9
    virtual void SimpleUnitCreated(EEUnit* inUnit);  // +0x7c 0x20bae4 trivial: push dword ptr [esp + 4]; call 0x6351ba; ret 4
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x20baf0
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x20bbd0 (ICF: EEUCResource)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x20bc8e
};

#endif
