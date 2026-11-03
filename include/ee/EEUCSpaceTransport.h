// EEUCSpaceTransport.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCSpaceTransport.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCSPACETRANSPORT_H
#define EE_EEUCSPACETRANSPORT_H

#include "ee/EEUCTransport.h"

class EEUCTransport;

// vtable 0x447dc8; bases EEUCSpaceTransport > EEUCTransport > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23647c; dtor 0x2305ee
class EEUCSpaceTransport : public EEUCTransport {
public:
    EEUCSpaceTransport();                   // inline
    EEUCSpaceTransport(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual float GetHeightAt(EEComplexUnit* inUnit, const GE3DPoint& inLocation); // +0x24 0x21a80d
    virtual void GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);  // +0x2c 0x21a81d
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x21aa74
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x21aaa9 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x21aaf5 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x21ab3b
    // +0xe4..+0x104: EEUCTransport's signatures
    virtual void FindRendezvousTiles(std::vector<EEComplexUnit*>* inUnits, EEComplexUnit* inTransport, U2DPoint<long>* outTransportTile, U2DPoint<long>* outUnitTile); // +0xe4 0x21a6e0 (ICF: EEUCNavalTransport)
    virtual void FindUnloadTiles(EEComplexUnit* inTransport, U2DPoint<long>* inDestination, U2DPoint<long>* outUnloadTile, U2DPoint<long>* outFirstCargoTile, U2DPoint<long>* outTile); // +0xe8 0x218a5f (ICF: EEUCNavalTransport)
    virtual bool HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo);   // +0xf0 0x2135bc trivial: mov al, 1; ret 8 (ICF: EECCCloaking, EEUCHelicopterTransport...)
    virtual bool CanCarry(EEComplexUnit* inUnit1, EEComplexUnit* inUnit2);  // +0xf4 0x21a838 (ICF: EEUCNavalTransport)
    virtual bool CanUnloadAt(EEComplexUnit* inTransport, long inX, long inY); // +0xf8 0x21a8aa
    virtual bool FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation); // +0xfc 0x218d33 (ICF: EEUCFortress, EEUCHelicopterTransport...)
    // the spaceship's bodies as non-virtual helpers (the slots +0x24/+0x2c/+0x78/+0xa0 call them; each is ICF-folded
    // into EEUCSpaceship's identical slot 0x21a219/0x21a404/0x21a51a/0x21a59e) (provisional names);
    // defined in EEUCSpaceTransport.cpp as copies of those bodies (owner decision)
    float SpaceshipVirtual24(EEComplexUnit* inUnit, const GE3DPoint& inLocation);
    void SpaceshipVirtual2C(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);
    void SpaceshipVirtual78(EEComplexUnit* inUnit);
    long SpaceshipVirtualA0(EEComplexUnit* inUnit, unsigned long inEvent);
    // members OPEN: a base's sizeof is unknown (no new site), so this class's own members cannot be placed yet
};

#endif
