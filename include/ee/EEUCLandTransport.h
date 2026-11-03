// EEUCLandTransport.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCLandTransport.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCLANDTRANSPORT_H
#define EE_EEUCLANDTRANSPORT_H

#include "ee/EEUCTransport.h"

class EEUCTransport;

// vtable 0x448930; bases EEUCLandTransport > EEUCTransport > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23647c; dtor 0x2305ee
class EEUCLandTransport : public EEUCTransport {
public:
    EEUCLandTransport();                    // inline
    EEUCLandTransport(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    EEUCLandTransport(EEUnitClassRecord* inRecord, int inIndex);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual void BadlyHurt(EEUnit* inUnit);   // +0x8c 0x216fea (ICF: EEUCFortress)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x217070
    virtual void FindRendezvousTiles(std::vector<EEComplexUnit*>* inUnits, EEComplexUnit* inTransport,
                           U2DPoint<long>* outTransportTile, U2DPoint<long>* outUnitTile);   // +0xe4 0x216de3
    virtual bool HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo);   // +0xf0 0x2135bc trivial: mov al, 1; ret 8 (ICF: EECCCloaking, EEUCHelicopterTransport...)
    virtual bool CanCarry(EEComplexUnit* inUnit1, EEComplexUnit* inUnit2);  // +0xf4 0x216f17 (ICF: EEUCFortress)
    virtual bool CanUnloadAt(EEComplexUnit* inTransport, long inX, long inY);  // +0xf8 0x216f8e (same continent, then the base's)
    virtual bool FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation); // +0xfc 0x218d33 (ICF: EEUCFortress, EEUCHelicopterTransport...)
    virtual void UnloadAt(EEComplexUnit* inTransport, long inX, long inY);   // +0x104 0x218e14 (forwards to EEUCTransport's; ICF: EEUCNavalTransport)
    // members OPEN: a base's sizeof is unknown (no new site), so this class's own members cannot be placed yet
};

#endif
