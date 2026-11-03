// EEUGLoadGarrison.h (RECONSTRUCTION; file name inferred). Garrisoning units in a building (RTTI EEUGLoadGarrison >
// EEUGLoadContainer > EEUnitGoal; vtable 0x447754; sizeof 0x74: its new-site size; EEUGLoadGarrison.cpp
// 0x226a43..0x2271c7). Member names from its out-of-synch dump 0x226d22 (provisional name).
// Requires what ee/EEUGLoadContainer.h requires.
#ifndef EE_EEUGLOADGARRISON_H
#define EE_EEUGLOADGARRISON_H

#include "ee/EEUGLoadContainer.h"

class EEUGLoadGarrison : public EEUGLoadContainer {
public:
    // 0x226a86 (its Virtual10 copies through it): the destination is inContainerTile's point
    EEUGLoadGarrison(EEComplexUnit* inContainer, const UReferenceContainer<EEComplexUnit*>& inUnits,
                     const U2DPoint<long>& inContainerTile, const U2DPoint<long>& inUnitTile,
                     float inMinimumDistanceSquared, long inCost, long inUpgrade, bool inIsValid, bool inArg);
    // 0x226b0e: then Initialize()
    EEUGLoadGarrison(EEComplexUnit* inContainer, std::vector<EEComplexUnit*>& inUnits, bool inArg);
    EEUGLoadGarrison(bool inArg);           // 0x226b37 (EEUnitGoal::CreateGoal)
    virtual ~EEUGLoadGarrison();            // +0x00 0x226b51 (??_G 0x226af2)
    virtual EEUnitGoal* Clone();            // +0x10 0x226cd6
    virtual void DumpOutOfSynch(FILE* inFile);   // +0x14 0x226d22
    virtual long GetOutOfSynchChecksum();   // +0x18 0x226d6a
    virtual void GetDebugLine(UWideString& ioText);  // +0x1c 0x226d85
    virtual void GetDebugText(UWideString& ioText);  // +0x20 0x226d97
    virtual void Step(EEComplexUnit* inUnit);        // +0x40 0x226df6
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x44 0x226e25
    virtual void Start(EEComplexUnit* inUnit);       // +0x54 0x226f8c
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x58 0x226fb0
    virtual bool EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0x64 0x227003
    virtual void ReplaceUnit(EEComplexUnit* inOld, EEComplexUnit* inNew, EEComplexUnit*);  // +0x6c 0x2270e0
    virtual long GetUsedCapacity();         // +0x78 0x226adb (the container's load)
    virtual long GetCapacity();             // +0x7c 0x226ae5 (its class's capacity)
    virtual bool CanContain(EEComplexUnit* inUnit);  // +0x80 (ICF-folded 0xb3257: true)
    virtual long GetRemainingLoad();        // +0x84 0x226db8
    virtual long GetFreeCapacity();         // +0x88 0x226dc5
    virtual bool LoadUnit(EEComplexUnit* inUnit);    // +0x90 0x226e72 (garrisons inUnit)
    virtual void ComputeLoadDistance();     // +0x94 0x226b5c (the loading distance)
    virtual void ComputeRendezvous();       // +0x98 0x226ba4 (the destination and the rendezvous tiles)
    void ComputeUpgrade();                  // 0x226bf1 (provisional name)
    void Initialize();                      // 0x226dd8: Virtual98, ComputeLoadDistance, ComputeUpgrade (provisional name)
    void UpgradeContainer();                // 0x22719e (Virtual40's event 0x2ee7) (provisional name)

    long mUpgrade;                          // +0x68 "Garrison upgrade" (a gUnitClassTable index)
    long mCost;                             // +0x6c "Garrison Cost"
    bool mIsValid;                          // +0x70 "Is Valid"

    static const UString sData;             // "EEUGLoadGarrison-Data" (init chain 0x226a43; Load/Save pass it on)
};

#endif
