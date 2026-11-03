// EEUCTransport.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCTransport.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCTRANSPORT_H
#define EE_EEUCTRANSPORT_H

#include "ee/EEUnit.h"

class EEUnitClass;
template <class T> class U2DPoint;

// vtable 0x4498b8; bases EEUCTransport > EEUnitClass > UReferenceCounter; sizeof ?; ctor ?; ??_G 0x23647c; dtor 0x2305ee
class EEUCTransport : public EEUnitClass {
public:
    virtual ~EEUCTransport() {}
    EEUCTransport();                        // inline
    EEUCTransport(EEUnitClassRecord* inRecord, int inIndex);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1; out of line 0x23629e)
    // +0x00: the implicit dtor (??_G 0x23647c calls EEUnitClass's 0x2305ee)
    virtual bool FliesOver() { return this->mDomains & kMovementAir; }  // +0x5c inline (COMDAT 0x236292 in EEUnitClass.cpp's range; ICF: EEUCFortress...)
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x21c7d1 (ICF: EEUCLandTransport, EEUCNavalTransport)
    virtual void BadlyHurt(EEUnit* inUnit);   // +0x8c 0x157269 trivial: ret 4 (ICF: EEAECalamityImmunity, EEAEConversionImmunity...)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x21c92c (ICF: EEUCSpaceTransport)
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x20d336 (ICF: EEUCBasicCitizen, EEUCBasicPriest...)
    // +0xe0 0x21b27a NEW (ICF: EEUCFortress, EEUCHelicopterTransport...): the units' gathering tile near inFrom (by
    // value: the out-of-line U2DPoint<long> copy ctor 0x4d28b; EEUCSpaceTransport::FindRendezvousTiles) (provisional
    // name)
    virtual long FindGatheringTile(std::vector<EEComplexUnit*>* inUnits, U2DPoint<long> inFrom, U2DPoint<long>* outTile);
    // +0xe4 0x21b33f: the rendezvous tiles of inTransport and inUnits (EEUGLoadTransport::ComputeRendezvous 0x227340)
    virtual void FindRendezvousTiles(std::vector<EEComplexUnit*>* inUnits, EEComplexUnit* inTransport, U2DPoint<long>* outTransportTile, U2DPoint<long>* outUnitTile);
    // 0x21b613: whether inTransport can load inUnit (EEUGLoadTransport::EvaluateCondition) (provisional name)
    void GetRingTiles(long inX, long inY, std::vector<U2DPoint<long> >& outTiles); // original ret12
    bool CanLoad(EEComplexUnit* inTransport, EEComplexUnit* inUnit);
    // 0x21c255: six stack args, ret24 and x87 score; independent transport callers.
    float FindBestShoreTile(EEComplexUnit* inTransport, const U2DPoint<long>& inFrom,
                  const U2DPoint<long>& inDestination, const U2DPoint<long>& inTarget,
                  bool inRestrictTerrain, U2DPoint<long>* outTile);
    // +0xe8 0x21b361 (ICF: EEUCAirAnimal, EEUCAirplane...): the unload tiles for inDestination (EEUGUnloadTransport 0x228739)
    virtual void FindUnloadTiles(EEComplexUnit* inTransport, U2DPoint<long>* inDestination, U2DPoint<long>* outUnloadTile, U2DPoint<long>* outFirstCargoTile, U2DPoint<long>* outTile);
    virtual long GetUnloadFailedTextID() { return 0x7c38; }  // +0xec inline (COMDAT 0x23628c in EEUnitClass.cpp's range)
    virtual bool HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo) = 0;   // +0xf0 pure NEW (EEComplexUnit 0x1fcd3f: this and the cargo)
    virtual bool CanCarry(EEComplexUnit* inUnit1, EEComplexUnit* inUnit2);  // +0xf4 0x21b5ab NEW (ICF: EEUCHelicopterTransport)
    virtual bool CanUnloadAt(EEComplexUnit* inTransport, long inX, long inY); // +0xf8 0x21c75c (can it unload there; EEUGUnloadTransport 0x2286cf)
    virtual bool FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation); // +0xfc 0x21c610 NEW
    // +0x100 0x21c806 NEW: drops from ioUnits (into outRejected) the units inTransport cannot load (provisional name)
    virtual void FilterLoadable(std::vector<EEComplexUnit*>* ioUnits, EEComplexUnit* inTransport, std::vector<EEComplexUnit*>* outRejected);  // (ICF: EEUCFortress, EEUCHelicopterTransport...)
    virtual void UnloadAt(EEComplexUnit* inTransport, long inX, long inY);   // +0x104 0x21c8f7 (unloads at (inX, inY); ICF: EEUCHelicopterTransport, EEUCSpaceTransport)
    // 0x21b83f: whether inTransport can unload inCargo near *inDestination (EEUGUnloadTransport 0x228a44) (provisional
    // name)
    bool CanUnloadNear(EEComplexUnit* inTransport, EEComplexUnit* inCargo, U2DPoint<long>* ioUnloadTile, U2DPoint<long>* ioFirstCargoTile, U2DPoint<long>* inDestination);
    // 0x21a995 (in EEUCSpaceTransport.cpp's range; ecx unused): whether continent inTo can be reached from inFrom
    // (inFrom's neighbour ids, not kind 3, through gContinentManager's flags) (provisional name)
    bool ContinentsConnect(unsigned short inFrom, unsigned short inTo);
    // 0x21baac: the first cargo tile next to *inUnloadTile (the nearest land tile on its continent; else the unload
    // tile itself) (provisional name; EEUCNavalTransport::FindUnloadTiles)
    void FindLandingTile(EEComplexUnit* inTransport, U2DPoint<long>* inUnloadTile, U2DPoint<long>* inTarget, U2DPoint<long>* outTile);
    // 0x21b38e: the transport's and the first unit's meeting tiles (provisional name)
    void FindMeetingTiles(EEComplexUnit* inTransport, EEComplexUnit* inUnit, U2DPoint<long>* ioTransportTile, U2DPoint<long>* ioUnitTile);
    // 0x21be21: the unload tile for the transport at inFrom towards inDestination (provisional name)
    void FindShoreTile(EEComplexUnit* inTransport, const EETileCoordinate& inFrom, U2DPoint<long>* inDestination, U2DPoint<long>* inTarget, bool inArg, U2DPoint<long>* outTile);
};

// 0x23e530 (cdecl; whether a unit of type inUnitType of inPlayer is blocked on inTile): ee/EEUnitUtilities.h
// 0x249727 (cdecl): the tiles of the line (inX1, inY1)..(inX2, inY2) into outTiles (provisional name)
void EEGetTileLine(long inX1, long inY1, long inX2, long inY2, std::vector<U2DPoint<long> >* outTiles, bool inEnds, bool inThin);  // bools: 0x649727 tests them with `cmp byte`

#endif
