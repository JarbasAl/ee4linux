// EEUnitTrigger.h (RECONSTRUCTION; file name inferred). A trigger of a unit AI script (Unit AI Scripts\*.tai,
// parsed by EEUnitAction.cpp 0x22c8bd): `anyof(A, B)` / `allof(A, B)` over trigger ids, then `true(Action)` /
// `false(Action)`. Layout from the ctor 0x22cd15, the parser's stores and Evaluate 0x22b9a0.
// Names are provisional. Requires the engine declarations of: UString.
#ifndef EE_EEUNITTRIGGER_H
#define EE_EEUNITTRIGGER_H

class EEComplexUnit;
class EEUnitAction;

// vtable 0x447aa4; sizeof 0x40; ctor 0x22cd15; ??_G 0x22a801; dtor 0x22a7e6
class EEUnitTrigger {
public:
    EEUnitTrigger();                        // 0x22cd15
    virtual ~EEUnitTrigger();               // +0x00 dtor 0x22a7e6

    void AddTrigger(long inTrigger);        // 0x22cd64
    long Evaluate(EEComplexUnit* inUnit, EEUnitAction*& outAction);   // 0x22b9a0 (0: no action, 1: outAction, 2: done)
    void Link(std::map<long, EEUnitAction*>*& ioActions);             // 0x22b854
    // the trigger count (out of line: ICF-folded into EECalamityManager::GetCalamityClassCount 0x138e4b)
    unsigned long GetTriggerCount() const { return this->mTriggerNames.size(); }

    static long GetTriggerID(const char* inName);       // 0x22b7e8
    static UString& GetTriggerName(unsigned long inTrigger);     // 0x22b837
    static void Register(const char* inName, long inTrigger);   // 0x22a81d
    static void RegisterAll();                          // 0x22a8e5

    std::vector<UString> mTriggerNames;     // +0x004
    std::vector<long> mTriggers;            // +0x014
    EEUnitAction* mTrueAction;              // +0x024
    EEUnitAction* mFalseAction;             // +0x028
    long mTrueActionID;                     // +0x02c
    long mFalseActionID;                    // +0x030
    float mTrueWeight;                      // +0x034 (100.0f once set by the parser)
    float mFalseWeight;                     // +0x038
    bool mAnyOf;                            // +0x03c
    bool mAllOf;                            // +0x03d

    static std::map<UString, long> sTriggerIDs;     // 0x563658
    static std::map<unsigned long, UString> sTriggerNames;   // 0x563648
};

#endif
