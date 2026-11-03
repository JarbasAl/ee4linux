// C:\EEX\Empire Earth\AI Thread\EECPASGStats.h (RECONSTRUCTION; file name inferred). The
// statistics of an attack sub group's units (RTTI EECPASGStats, no base; vtable 0x43b9e4: the destructor only; sizeof
// 0x48: EECPAttackSubGroup +0x84 .. the EECPILocation at +0xcc). EECPAttackSubGroup's ctor 0x5adb2 stores the vptr and
// calls Clear (an inline ctor calling an out-of-line member: Compute 0x511e2 calls Clear too); its dtor 0x5aff8 only
// stores the vptr (an empty inline dtor); ??_G 0x51189 is emitted next to the out-of-line members (0x51189..0x512e9).
// Field names by offset (provisional names); the per-unit sums come from Add 0x51239.
#ifndef EE_EECPASGSTATS_H
#define EE_EECPASGSTATS_H

class EEComplexUnit;
class EECPAttackSubGroupUnit;

class EECPASGStats {
public:
    EECPASGStats() { this->Clear(); }
    virtual ~EECPASGStats() {}              // +0x00 (??_G 0x51189)

    void Clear();                           // 0x511a6 (fldz into the floats, 0 into every long)
    // 0x511e2: Clear, mUnitCount = the units' number, then Add(unit->mUnit) for each from the back (provisional name)
    void Compute(std::vector<EECPAttackSubGroupUnit*>& inUnits);   // non-const: operator[] folds (0x451223)
    void Add(EEComplexUnit* inUnit);        // 0x51239 (sums of the unit's and its class's values)

    float mTotalAttack;                     // +0x04 (+= the unit's +0x34 object's +0xd8)
    float mTotalRange;                      // +0x08 (+= its +0x9c)
    float mTotalSpeed;                      // +0x0c (+= its +0xac)
    long mTotalInfluence;                   // +0x10 (+= the unit class's +0x244)
    long mNumAttacking;                     // +0x14 (counts units whose +0x1f0 object's kind byte is 2, 3 or 0xb)
    long mTotalHitPoints;                   // +0x18 (+= the unit's +0x3c)
    long mNumCitizens;                      // +0x1c (a count by EEUnitClass::mUnitType)
    long mNumPriests;                       // +0x20 (a count by EEUnitClass::mUnitType)
    long mL24;                              // +0x24
    long mNumProphets;                      // +0x28 (a count by EEUnitClass::mUnitType)
    long mTotalMaxHitPoints;                // +0x2c (+= the unit's +0x34 object's +0x144)
    long mUnitCount;                        // +0x30 (Compute: the vector's size)
    long mL34;                              // +0x34
    long mL38;                              // +0x38
    long mNumCalamityUnits;                 // +0x3c (counts unit classes with the +0x31d flag)
    long mL40;                              // +0x40
    long mL44;                              // +0x44
};

#endif
