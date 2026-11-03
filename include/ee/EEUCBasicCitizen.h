// EEUCBasicCitizen.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes:
// EEUCBasicCitizen.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: -.
#ifndef EE_EEUCBASICCITIZEN_H
#define EE_EEUCBASICCITIZEN_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnitClass;
class EETile;

// vtable 0x4499c4; bases EEUCBasicCitizen > EEUnitClass > UReferenceCounter; sizeof ?; ctor ?; ??_G 0x236308; dtor 0x2305ee
class EEUCBasicCitizen : public EEUnitClass {
public:
    void UpdateCitizen(EEComplexUnit* inUnit);  // 0x20d361 (EEUCLandCitizen::Update's last call) (provisional name)
    EEUCBasicCitizen();                     // inline
    EEUCBasicCitizen(EEUnitClassRecord* inRecord, int inIndex);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1; out of line 0x2362e3)
    virtual ~EEUCBasicCitizen() {}          // +0x00 dtor 0x2305ee
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x20bcf7 (ICF: EEUCLandCitizen)
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x20da7a trivial: push dword ptr [esp + 4]; call 0x63511e; ret 4 (ICF: EEUCBasicPriest, EEUCLandCitizen...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x20d10e (ICF: EEUCLandCitizen)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x20d361
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x20d336 (ICF: EEUCBasicPriest, EEUCLandCitizen...)
    // 0x20c631: the resource of kind inKind for inUnit to gather next (inResource first; EEUGGather's 0xfa6 handler)
    // (provisional name; types inferred)
    EEUnit* FindNextResource(EEComplexUnit* inUnit, EEUnit* inResource, unsigned long inKind, std::vector<EEUnit*>* ioUnits);
    // 0x20c6b3: the storage for inUnit (EEUGGather::FindStorage passes command == 0xfa7) (provisional name)
    EEUnit* FindStorage(EEComplexUnit* inUnit, bool inArg);
    bool HasGatherPoint(EEComplexUnit* inUnit, EEUnit* inResource);  // 0x20d4f4 (ret 8; EEUGGather::EvaluateCondition 0xfba; provisional name)
    // 0x20c932 (ret 0x18; this unread): where inUnit stands to work the footprint at (inX, inY)
    void FindGatherPoint(EEComplexUnit* inUnit, unsigned long inX, unsigned long inY, long inSizeX, long inSizeY,
                  GE3DPoint& outLocation);
    bool IsTileHeld(EETile* inTile);        // 0x20d2dc (ret 4; this unread): a blocking unit on inTile
    bool FindWoodSite(EEComplexUnit* inUnit, long inUnitType, U2DPoint<float>& outLocation);  // 0x20be58 (ret 0xc; EECPResourceManager::BuildSettlementFor) (provisional name)
    // 0x20c026 (EEUGGather::SearchNavalResource through dynamic_cast<EEUCNavalCitizen*>) (provisional name)
    EEUnit* FindResourceOfKind(EEComplexUnit* inUnit, EEUnit* inResource, EEUnit* inArg3, long inRange, unsigned long inKind,
                     std::vector<EEUnit*>* ioUnits, bool inArg7);
};

#endif
