// C:\EEX\Empire Earth\AI Thread\EECPASGUPCTargetIsUnitSetScreenBase.h (RECONSTRUCTION; file name inferred). The prophet
// calamity analyzers' target screens: a predicate over a sub group target (RTTI EECPASGUPCTargetIsUnitSetScreenBase, no
// base; vtable 0x4394d8; sizeof 0x10), and its five kinds (each overrides +0x04 only; their inline ctors call the base
// ctor out of line, then store the own vptr: 0x954f). The analyzers build
// them on the stack (EECPASGUProphetCalamityAnalyzerBase::PickTarget 0x91ef: `(mProphet, this, true)`). The base ctor
// 0x908a is out of line (first called from 0x65b0, before its home unit 0x908a..); ??_G 0x70d6 stores the own vptr only
// (a user-declared empty inline dtor). Names are provisional.
#ifndef EE_EECPASGUPCTARGETISUNITSETSCREENBASE_H
#define EE_EECPASGUPCTARGETISUNITSETSCREENBASE_H

class EECPASGUProphet;
class EECPASGUProphetCalamityAnalyzerBase;
class EECPASTargetUnit;

class EECPASGUPCTargetIsUnitSetScreenBase {
public:
    // 0x908a: +0x04, +0x08, +0x0c from the arguments, then the vptr (an init list)
    EECPASGUPCTargetIsUnitSetScreenBase(EECPASGUProphet* inProphet, EECPASGUProphetCalamityAnalyzerBase* inAnalyzer,
                                        bool inUseUnitSet);
    virtual ~EECPASGUPCTargetIsUnitSetScreenBase() {}
    // +0x04 0x90aa: !mUseUnitSet or no unit set -> true; else the analyzer's unit set Contains the target's unit type
    virtual bool Passes(EECPASTargetUnit* inTarget);

    EECPASGUProphet* mProphet;              // +0x04
    EECPASGUProphetCalamityAnalyzerBase* mAnalyzer;   // +0x08 (its +0x14 EEUnitSet*)
    bool mUseUnitSet;                       // +0x0c
};

// vtable 0x439748; +0x04 0x9661
class EECPASGUPCTargetIsUnitSetScreenOnlyBuildings : public EECPASGUPCTargetIsUnitSetScreenBase {
public:
    EECPASGUPCTargetIsUnitSetScreenOnlyBuildings(EECPASGUProphet* inProphet,
                                                 EECPASGUProphetCalamityAnalyzerBase* inAnalyzer, bool inUseUnitSet)
        : EECPASGUPCTargetIsUnitSetScreenBase(inProphet, inAnalyzer, inUseUnitSet) {}
    virtual bool Passes(EECPASTargetUnit* inTarget);
};

// vtable 0x4394cc; +0x04 0x7063
class EECPASGUPCTargetIsUnitSetScreenOnlySplashUnits : public EECPASGUPCTargetIsUnitSetScreenBase {
public:
    EECPASGUPCTargetIsUnitSetScreenOnlySplashUnits(EECPASGUProphet* inProphet,
                                                   EECPASGUProphetCalamityAnalyzerBase* inAnalyzer, bool inUseUnitSet)
        : EECPASGUPCTargetIsUnitSetScreenBase(inProphet, inAnalyzer, inUseUnitSet) {}
    virtual bool Passes(EECPASTargetUnit* inTarget);
};

// vtable 0x43975c; +0x04 0x9d18
class EECPASGUPCTargetIsUnitSetScreenOnlyWonders : public EECPASGUPCTargetIsUnitSetScreenBase {
public:
    EECPASGUPCTargetIsUnitSetScreenOnlyWonders(EECPASGUProphet* inProphet,
                                               EECPASGUProphetCalamityAnalyzerBase* inAnalyzer, bool inUseUnitSet)
        : EECPASGUPCTargetIsUnitSetScreenBase(inProphet, inAnalyzer, inUseUnitSet) {}
    virtual bool Passes(EECPASTargetUnit* inTarget);
};

// vtable 0x4394c0; +0x04 0x7089
class EECPASGUPCTargetIsUnitSetScreenRangedOrSpecialUnits : public EECPASGUPCTargetIsUnitSetScreenBase {
public:
    EECPASGUPCTargetIsUnitSetScreenRangedOrSpecialUnits(EECPASGUProphet* inProphet,
                                                        EECPASGUProphetCalamityAnalyzerBase* inAnalyzer,
                                                        bool inUseUnitSet)
        : EECPASGUPCTargetIsUnitSetScreenBase(inProphet, inAnalyzer, inUseUnitSet) {}
    virtual bool Passes(EECPASTargetUnit* inTarget);
};

// vtable 0x439770; +0x04 0xa4a0
class EECPASGUPCTargetIsUnitSetScreenRangedUnits : public EECPASGUPCTargetIsUnitSetScreenBase {
public:
    EECPASGUPCTargetIsUnitSetScreenRangedUnits(EECPASGUProphet* inProphet,
                                               EECPASGUProphetCalamityAnalyzerBase* inAnalyzer, bool inUseUnitSet)
        : EECPASGUPCTargetIsUnitSetScreenBase(inProphet, inAnalyzer, inUseUnitSet) {}
    virtual bool Passes(EECPASTargetUnit* inTarget);
};

#endif
