// EEUGUnloadTransport.h (RECONSTRUCTION; file name inferred). Unloading a transport at a destination tile (RTTI
// EEUGUnloadTransport > EEUnitGoal; vtable 0x4479a0; sizeof 0x40 = its new-site size; EEUGUnloadTransport.cpp
// 0x2283bc..0x228c4c). Member names from its out-of-synch dump 0x2284ef (provisional name).
// Requires lle/LowLevelEngine.h.
#ifndef EE_EEUGUNLOADTRANSPORT_H
#define EE_EEUGUNLOADTRANSPORT_H

#include "lle/U2DPoint.h"
#include "ee/EEUnitGoal.h"

class EEUnit;
class EEUnitGoal;

// 0x4c8fa8 (.data, 498): the sound the unload goal plays for unit type 0x313 transports (provisional name)
extern long gSpaceUnloadSoundID;

class EEUGUnloadTransport : public EEUnitGoal {
public:
    // 0x2283ff: referenced; the range from the transport's class (provisional names)
    EEUGUnloadTransport(EEComplexUnit* inTransport, long inX, long inY, bool inArg);
    // inline (COMDAT 0x23c685 after its first user, EEUnitGoal::CreateGoal): the points are zeroed twice, by their
    // ctors and again in the body
    EEUGUnloadTransport(bool inArg)
        : EEUnitGoal(kGoalUnloadTransport, inArg), mUnloadRangeSquared(0.0f)
    {
        this->mTransport = NULL;
        this->mUnloadTile.SetXY(0, 0);
        this->mFirstCargoTile.SetXY(0, 0);
        this->mDestination.SetXY(0, 0);
    }
    virtual ~EEUGUnloadTransport();         // +0x00 0x228483 (??_G 0x228467)
    virtual void GetDestination(EEComplexUnit* inUnit, GE3DPoint& outLocation);  // +0x08 0x2284a0 (the unload tile)
    virtual EEUnitGoal* Clone();            // +0x10 0x2284b8
    virtual void DumpOutOfSynch(FILE* inFile);   // +0x14 0x2284ef
    virtual long GetOutOfSynchChecksum();   // +0x18 0x228562
    virtual void GetDebugLine(UWideString& ioText);  // +0x1c 0x2285be
    virtual void GetDebugText(UWideString& ioText);  // +0x20 0x22861d
    virtual void GetGoalLocation(GE3DPoint& outLocation);  // +0x28 0x22866b (the destination tile)
    virtual long GetMaxPathAttempts();      // +0x2c 0x228735 (9)
    virtual long GetStartAction();          // +0x30 (ICF-folded 0x227264: 0x1f40)
    virtual void SetOwner(EEComplexUnit* inUnit);    // +0x3c (ICF-folded 0x157269: empty)
    virtual void Step(EEComplexUnit* inUnit);        // +0x40 0x2287d6
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x44 0x2287fa
    virtual long RepairPointers(unsigned long inFirst, unsigned long inSecond);  // +0x50 0x228883 (repairs the transport)
    virtual void Start(EEComplexUnit* inUnit);       // +0x54 0x2288ea
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x58 0x228990
    virtual bool EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0x64 0x228a44
    virtual void ReplaceUnit(EEComplexUnit* inOld, EEComplexUnit* inNew, EEComplexUnit*);  // +0x6c 0x228b08
    virtual bool Suits(EEUnit* inUnit);       // +0x70 0x228b55
    bool IsInUnloadRange();                 // 0x228683 (provisional name)
    bool CanUnloadAtDestination();          // 0x2286cf (provisional name)
    void FindUnloadTiles();                 // 0x228739 (provisional name)
    void SetTransport(EEComplexUnit* inTransport);   // 0x228a19 (no vtable slot; provisional name)
    void Unload();                          // 0x228b68 (Virtual40's event 0x2ee1) (provisional name)

    EEComplexUnit* mTransport;              // +0x20 "Transport" (referenced)
    U2DPoint<long> mUnloadTile;             // +0x24 "Unload Tile"
    U2DPoint<long> mFirstCargoTile;         // +0x2c "First Cargo Tile"
    U2DPoint<long> mDestination;            // +0x34 "Transport Destination"
    float mUnloadRangeSquared;              // +0x3c "Unload Range Squared"

    static const UString sData;             // "EEUGUnloadTransport-Data" (init chain 0x2283bc)
};

#endif
