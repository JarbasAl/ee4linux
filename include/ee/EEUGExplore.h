// EEUGExplore.h (RECONSTRUCTION; file name inferred). Exploring (RTTI EEUGExplore > EEUnitGoal; vtable 0x44750c;
// ctor 0x221420: EEUnitGoal(10, inArg); sizeof 0x4c: `new 0x4c` in EEMExplore::Execute). Member names from
// its out-of-synch dump 0x2214ab; provisional names. Requires lle/U2DSparseArray.h
// (U2DSparseArrayPointContainer) and ee/EEUnitGoal.h.
#ifndef EE_EEUGEXPLORE_H
#define EE_EEUGEXPLORE_H

#include "lle/U2DPoint.h"               // mTile by value
#include "ee/EEUnitGoal.h"
#include "ee/EEUnitAction.h"

class GE2DPoint;

class EEUGExplore : public EEUnitGoal {
public:
    EEUGExplore(bool inArg);                // 0x221420
    virtual ~EEUGExplore();                 // +0x00 0x22147e (??_G 0x221462)
    virtual EEUnitGoal* Clone();            // +0x10 0x221534
    virtual void DumpOutOfSynch(FILE* inFile);   // +0x14 0x2214ab
    virtual long GetOutOfSynchChecksum();   // +0x18 0x221513
    virtual void GetDebugText(UWideString& ioText);  // +0x20 0x221b05 ("Exploring")
    virtual void GetGoalLocation(GE3DPoint& outLocation);  // +0x28 0x221b35 (the tile's location)
    virtual void Step(EEComplexUnit* inUnit);        // +0x40 0x221b4d
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x44 0x221bfc
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x58 0x221cbd
    virtual bool EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0x64 0x221d67
    virtual void ReplaceUnit(EEComplexUnit* inOld, EEComplexUnit* inNew, EEComplexUnit*);  // +0x6c (ICF 0x2907a3: empty)
    virtual bool Suits(EEUnit* inUnit);       // +0x70 (ICF 0xb3257: true)
    virtual void AppendDebugLabel(UWideString& ioText);  // +0x78 0x221af3 ("Explore:") NEW
    virtual void GetTileCenter(GE3DPoint& outLocation);  // +0x7c 0x221b17 (the tile's centre) NEW
    // inline: emitted after their first user, the ctor (0x221454, 0x221458, 0x22145c), before the ??_G
    virtual bool GetNoUnexploredAreas() { return this->mNoUnexploredAreas; }  // +0x80 NEW
    virtual bool GetNoNonVisibleAreas() { return this->mNoNonVisibleAreas; }  // +0x84 NEW
    virtual long GetStartAction() { return kActionFindUnexploredArea; }  // +0x30 (the explore action)
    // 0x22155e: the next tile to explore within the search radius (Virtual40) (provisional name)
    bool FindNextTile(EEComplexUnit* inUnit); // original221ae9 tracked AL, ret4

    U2DSparseArrayPointContainer mBadTiles; // +0x20 "Bad Tile Array" (not saved: Save writes a zero count)
    U2DPoint<long> mTile;                   // +0x3c "Tile Location" (saved)
    unsigned long mSearchRadius;            // +0x44 "Search Radius" (saved)
    bool mNoUnexploredAreas;                // +0x48 "No UnExplored Areas flag" (saved)
    bool mNoNonVisibleAreas;                // +0x49 "No NonVisible Areas flag" (saved)

    static const UString sData;             // "EEUGExplore-Data" (init chain 0x2213dd)
};

#endif
