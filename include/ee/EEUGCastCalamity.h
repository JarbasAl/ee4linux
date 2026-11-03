// EEUGCastCalamity.h (RECONSTRUCTION; file name inferred: one header per goal class like ee/EEUGExplore.h; split out of
// ee/EEUnitGoal.h, no __FILE__ evidence either way).
#ifndef EE_EEUGCASTCALAMITY_H
#define EE_EEUGCASTCALAMITY_H

#include "ee/EEUnitGoal.h"

class EEComplexUnit;
class EEUnit;
class FSFile;
class GE3DPoint;
class UWideString;

// Casting a calamity (RTTI EEUGCastCalamity > EEUnitGoal; vtable 0x44748c; sizeof 0x34; EEUGCastCalamity.cpp
// 0x220986..0x2213dd). Member names from its out-of-synch dump 0x220c39 (provisional name)
class EEUGCastCalamity : public EEUnitGoal {
public:
    // 0x2209c9: both units referenced (provisional names)
    EEUGCastCalamity(EEComplexUnit* inSource, EEUnit* inTarget, long inTileX, long inTileY, long inCalamityType,
                     bool inArg6);
    EEUGCastCalamity(bool inArg);          // 0x220a39 (EEUnitGoal::CreateGoal)
    virtual ~EEUGCastCalamity();            // +0x00 0x220a6d (??_G 0x220a1d)
    virtual void GetDestination(EEComplexUnit* inUnit, GE3DPoint& outLocation);  // +0x08 0x220c15
    virtual EEUnitGoal* Clone();            // +0x10 0x220d3f
    virtual void DumpOutOfSynch(FILE* inFile);   // +0x14 0x220c39
    virtual long GetOutOfSynchChecksum();   // +0x18 0x220ca5
    virtual void GetDebugLine(UWideString& ioText);  // +0x1c 0x220d78
    virtual void GetDebugText(UWideString& ioText);  // +0x20 0x220dd6
    virtual void GetGoalLocation(GE3DPoint& outLocation);  // +0x28 0x220ee6 (the target's position, else the tile's)
    virtual long GetStartAction();          // +0x30 0x220a12 (0x1389)
    virtual void SetOwner(EEComplexUnit* inSource);    // +0x3c 0x220f94 (a new source, referenced)
    virtual void Step(EEComplexUnit* inUnit);        // +0x40 0x220f12
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x44 0x220fb5
    virtual long RepairPointers(unsigned long inFirst, unsigned long inSecond);  // +0x50 0x22102d (repairs the pointers)
    virtual void Start(EEComplexUnit* inUnit);       // +0x54 (ICF-folded 0x157269: empty)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x58 0x221099
    virtual bool EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0x64 0x221133
    virtual void ReplaceUnit(EEComplexUnit* inOld, EEComplexUnit* inNew, EEComplexUnit*);  // +0x6c 0x2213b7
    virtual bool Suits(EEUnit* inUnit);       // +0x70 0x220a18 (true; EEUGLocation's and others' fold into it)
    // 0x220a96: the calamity class's Virtual30 on the tile (Virtual40's event 0x7ef) (provisional name)
    void Prepare(EEComplexUnit* inUnit);
    // 0x220ae6: casts the calamity (Virtual40's event 0x7e1) (provisional name)
    void Cast(EEComplexUnit* inUnit);
    // 0x220cda: where the calamity goes (the target's position, the source's for two types, else the tile's; at
    // least the source's height) (provisional name)
    void GetTargetLocation(GE3DPoint& outLocation);
    // 0x220e6e: whether the calamity class accepts the cast (Virtual64's event 0x3a9f) (provisional name)
    bool CanCast(EEComplexUnit* inUnit);

    EEComplexUnit* mSource;                 // +0x20 "Source Unit" (referenced)
    EEUnit* mTarget;                        // +0x24 "Target Unit" (referenced)
    long mTileX;                            // +0x28 "Tile X" (saved)
    long mTileY;                            // +0x2c "Tile Y" (saved)
    long mCalamityType;                     // +0x30 "Calamity Type": a tech tree id (saved)

    static const UString sData;             // "EEUGCastCalamity-Data" (init chain 0x220986)
};

#endif
