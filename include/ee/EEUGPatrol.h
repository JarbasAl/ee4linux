// EEUGPatrol.h (RECONSTRUCTION; file name inferred: one header per goal class like ee/EEUGExplore.h; split out of
// ee/EEUnitGoal.h, no __FILE__ evidence either way).
#ifndef EE_EEUGPATROL_H
#define EE_EEUGPATROL_H

#include "ee/EEUnitGoal.h"

class EEComplexUnit;
class EEPPatrolGroup;
class EEUnit;
class FSFile;
class GE3DPoint;
class UWideString;

// Patrolling (RTTI EEUGPatrol > EEUnitGoal; vtable 0x447920; ctor 0x227e2b: EEUnitGoal(9, inArg), a new patrol
// group; sizeof 0x2c: `new 0x2c` in EEMessage.cpp's patrol Executes). Names are provisional
class EEUGPatrol : public EEUnitGoal {
public:
    EEUGPatrol(bool inArg);                 // 0x227e2b
    // 0x227e89: an existing patrol group and the unit to start at (both referenced; EEMPatrol)
    EEUGPatrol(EEPPatrolGroup* inGroup, EEComplexUnit* inTarget, bool inArg);
    virtual ~EEUGPatrol();                  // +0x00 0x227ec3 (??_G 0x227e6d)
    virtual EEUnitGoal* Clone();            // +0x10 0x227f73
    virtual void DumpOutOfSynch(FILE* inFile);   // +0x14 0x227eed
    virtual long GetOutOfSynchChecksum();   // +0x18 0x227f48
    virtual void GetDebugLine(UWideString& ioText);  // +0x1c 0x227fd2
    virtual void GetDebugText(UWideString& ioText);  // +0x20 0x228044
    virtual void GetGoalLocation(GE3DPoint& outLocation);  // +0x28 0x22812a (its current point's position)
    virtual long GetStartAction();          // +0x30 0x227e67 (0x1f54)
    virtual void Step(EEComplexUnit* inUnit);        // +0x40 0x228139: on to the next point
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x44 0x228208
    virtual long RepairPointers(unsigned long inFirst, unsigned long inSecond);  // +0x50 0x228265 (repairs the pointers)
    virtual void Start(EEComplexUnit* inUnit);       // +0x54 0x2282d9: starts at the point nearest inUnit
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x58 0x228325
    virtual bool EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0x64 0x2283ac (the base's)
    virtual void ReplaceUnit(EEComplexUnit* inOld, EEComplexUnit* inNew, EEComplexUnit*);  // +0x6c (ICF-folded 0x2907a3: empty)
    virtual bool Suits(EEUnit* inUnit);       // +0x70 (ICF-folded 0x220a18: true)
    // Provisional name: drops the old group's reference, keeps one on inGroup (Load passes 0; ICF-folded into
    // EEPlayer.cpp's identical 0x163659)
    void SetPatrolGroup(EEPPatrolGroup* inGroup);

    EEPPatrolGroup* mPatrolGroup;           // +0x20 (= new EEPPatrolGroup; its id between Load and Virtual50)
    EEComplexUnit* mTarget;                 // +0x24 "Current Patrol Point" (referenced; its unit id until Virtual50)
    bool mIsGoingForward;                   // +0x28 "Going Forward Flag" (= true)

    static const UString sData;             // "EEUGPatrol-Data" (init chain 0x227de8)
};

#endif
