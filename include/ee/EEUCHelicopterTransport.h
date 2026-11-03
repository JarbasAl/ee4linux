// EEUCHelicopterTransport.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCHelicopterTransport.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCHELICOPTERTRANSPORT_H
#define EE_EEUCHELICOPTERTRANSPORT_H

#include "ee/EEUCTransport.h"

class EEComplexUnit;
class EEUCTransport;

// vtable 0x448824; bases EEUCHelicopterTransport > EEUCTransport > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23647c; dtor 0x2305ee
class EEUCHelicopterTransport : public EEUCTransport {
public:
    EEUCHelicopterTransport();              // inline
    EEUCHelicopterTransport(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCHelicopterTransport() {}   // +0x00 dtor 0x2305ee
    virtual bool CanAttackMove();           // +0x10 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual float GetHeightAt(EEComplexUnit* inUnit, const GE3DPoint& inLocation); // +0x24 0x2133e8
    virtual void GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch);  // +0x2c 0x21a404 (ICF: EEUCHelicopter, EEUCSpaceship)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x21361f
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x213774
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x21aaa9 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x21aaf5 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x2137f8 (ICF: EEUCHelicopter)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x213851
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x157269 trivial: ret 4 (ICF: EEAECalamityImmunity, EEAEConversionImmunity...)
    virtual long Animate(EEComplexUnit*);   // +0xc0 0x20afe9 trivial: push dword ptr [esp + 4]; call 0x638ce7; ret 4 (ICF: EEUCAirAnimal, EEUCAirplane...)
    // +0xe4 0x21339d (the base slot's signature: VirtualE0 from the transport's tile)
    virtual void FindRendezvousTiles(std::vector<EEComplexUnit*>* inUnits, EEComplexUnit* inTransport, U2DPoint<long>* outTransportTile, U2DPoint<long>* outUnitTile);
    virtual bool HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo);   // +0xf0 0x2135bc trivial: mov al, 1; ret 8 (ICF: EECCCloaking, EEUCLandTransport...; defined in EEUCHelicopterTransport.cpp)
    virtual bool CanUnloadAt(EEComplexUnit* inTransport, long inX, long inY); // +0xf8 0x2135c1 (the base slot's signature)
    virtual bool FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation); // +0xfc 0x218d33 (ICF: EEUCFortress, EEUCLandTransport...)
    // members OPEN: a base's sizeof is unknown (no new site), so this class's own members cannot be placed yet
};

#endif
