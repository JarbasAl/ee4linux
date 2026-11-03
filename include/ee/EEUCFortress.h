// EEUCFortress.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCFortress.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCFORTRESS_H
#define EE_EEUCFORTRESS_H

#include "ee/EEUCLandTransport.h"

class EEUCLandTransport;

// vtable 0x44860c; bases EEUCFortress > EEUCLandTransport > EEUCTransport > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x23647c; dtor 0x2305ee
class EEUCFortress : public EEUCLandTransport {
public:
    EEUCFortress();                         // inline
    EEUCFortress(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCFortress() {}              // +0x00 dtor 0x2305ee
    // +0x14/+0x1c 0x2101da (ret 8: the cargo's Virtual6C counts into the player's mPopulationCount; EEUnitClass's
    // one-argument declarations of these slots are OPEN)
    virtual void CargoRemoved(EEComplexUnit* inUnit, EEComplexUnit* inCargo);
    virtual void CargoUnloaded(EEComplexUnit* inUnit, EEComplexUnit* inCargo);  // +0x1c ICF-folded into Virtual14's body 0x2101da
    virtual void CargoAdded(EEComplexUnit* inUnit, EEComplexUnit* inCargo, bool inArg);  // +0x18 0x2101b5
    virtual bool IsLandBuilding();          // +0x54 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x21030a
    virtual void AddToRegion(EEUnit*);     // +0x80 0x21033d (ICF: EEUCBuilding, EEUCBuildingPriest...)
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x20f2ae (ICF: EEUCBuilding, EEUCBuildingPriest...)
    virtual void RemoveFromRegion(EEUnit*);  // +0x94 0x21034a (ICF: EEUCBuilding, EEUCBuildingPriest...)
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x20fb78 (ICF: EEUCBuilding, EEUCBuildingPriest...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x210357
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x210465
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x157269 trivial: ret 4 (ICF: EEAECalamityImmunity, EEAEConversionImmunity...)
    virtual void FindRendezvousTiles(std::vector<EEComplexUnit*>* inUnits, EEComplexUnit* inTransport, U2DPoint<long>* outTransportTile, U2DPoint<long>* outUnitTile); // +0xe4 0x21013b
    virtual void FindUnloadTiles(EEComplexUnit* inTransport, U2DPoint<long>* inDestination, U2DPoint<long>* outUnloadTile, U2DPoint<long>* outFirstCargoTile, U2DPoint<long>* outTile); // +0xe8 0x210153
    virtual long GetUnloadFailedTextID() { return 0x7c39; }  // +0xec inline (COMDAT 0x23640e in EEUnitClass.cpp's range)
    virtual bool HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo);   // +0xf0 0x2101f8 (a unit and its cargo: EEComplexUnit 0x1fcd3f passes this and the cargo)
    virtual bool CanUnloadAt(EEComplexUnit* inTransport, long inX, long inY); // +0xf8 0x210235
    virtual void UnloadAt(EEComplexUnit* inTransport, long inX, long inY);   // +0x104 0x210391
};

// cdecl callees (provisional names)
void EEAddBuildingRegion(EEUnit* inUnit);  // 0x24c8bc (Virtual80)
void EERemoveBuildingRegion(EEUnit* inUnit);  // 0x24d779 (ComputeLoadDistance)
extern unsigned short gFortressUnloadCost;   // 0x4c8fa4 (100)

#endif
