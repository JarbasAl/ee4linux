// C:\EEX\Empire Earth\Calamities\EEContagionFunctor.h (RECONSTRUCTION; file name inferred). What a contagion
// helper applies to the units it reaches (vtable 0x43c65c; every virtual of the base is an inline default, emitted
// with the first functor unit 0xac7b9..). Slot names are provisional (by behaviour; unknown ones by slot offset):
//   +0x08 the unit (category, type) qualifies; +0x0c the unit set of EEUnitSetManager::Contains; +0x10 the spread
//   radius; +0x14 the spread chance per second; +0x18 the spread interval; +0x20 the helper's units (instead of
//   spreading, when +0x1c); +0x24 once per helper update; +0x28 a unit was infected; +0x2c a unit was cured; +0x30
//   once per infected unit and update.
// Requires <vector>.
#ifndef EE_EECONTAGIONFUNCTOR_H
#define EE_EECONTAGIONFUNCTOR_H

class EECalamity;
class EEGraphicEffect;
class EEUnit;

class EEContagionFunctor {
public:
    virtual ~EEContagionFunctor() {}        // +0x00 (??_G 0xac9f6)
    virtual bool InfectsAlliesOnly() { return true; }  // +0x04 (ICF-folded 0x18aa9): spreads to allies too when false
    virtual bool CanInfectType(long inCategory, long inUnitType) { return true; }   // +0x08 (0xac9ee)
    virtual unsigned long GetUnitSet() { return 0; }   // +0x0c (ICF-folded 0x1ad67e)
    virtual float GetRadius() { return 0.0f; }   // +0x10 (0xac7e2)
    virtual float GetChance() { return 0.0f; }   // +0x14 (0xac7e2)
    virtual unsigned long GetSpreadTime() { return 0; }   // +0x18 (ICF-folded 0x1ad67e)
    virtual bool SkipsSpread() { return false; }  // +0x1c (0xac9f3): the helper calls +0x20 instead of spreading
    virtual void UpdateUnits(std::vector<EEUnit*>* inUnits) {}  // +0x20 (ICF-folded 0x156dd8)
    virtual void Update(float inElapsedTime, EECalamity* inCalamity) {}   // +0x24 (0xac9c8)
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity) {}       // +0x28 (0xac9c8)
    virtual void Cure(EEUnit* inUnit, EECalamity* inCalamity) {}         // +0x2c (0xac9c8)
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity) = 0;   // +0x30

    bool CanInfect(EEUnit* inUnit);         // 0xb66f8 (EEContagionManager.cpp) (provisional name)
};

// The fire's functor (RTTI EEFireFunctor > EEContagionFunctor; vtable 0x43cfa8; EEFireFunctor.cpp 0xb6bd1..0xb6e58;
// sizeof 0x28)
class EEFireFunctor : public EEContagionFunctor {
public:
    // 0xb6bd1 (provisional names)
    EEFireFunctor(EEGraphicEffect* inEffect, long inBurn, float inRadius, float inChance, long inDamage, long inSpreadTime,
                  unsigned long inValue, long inMode);
    virtual ~EEFireFunctor();               // +0x00 0xb6c2f (??_G 0xb6c13)
    virtual bool CanInfectType(long inCategory, long inUnitType);   // +0x08 0xb6c4d (every functor's: ICF)
    virtual unsigned long GetUnitSet();     // +0x0c 0xb6c78
    virtual float GetRadius();              // +0x10 0xb6c49
    virtual float GetChance();              // +0x14 0xb6c70
    virtual unsigned long GetSpreadTime();  // +0x18 0xb6c74
    virtual void Update(float inElapsedTime, EECalamity* inCalamity);   // +0x24 0xb6c7c
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity);       // +0x28 0xb6d2c
    virtual void Cure(EEUnit* inUnit, EECalamity* inCalamity);         // +0x2c 0xb6dec
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity);   // +0x30 0xb6cad

    EEGraphicEffect* mEffect;               // +0x04 (referenced)
    float mRadius;                          // +0x08 (record +0x64)
    float mChance;                          // +0x0c (record +0x68)
    long mBurn;                             // +0x10 (record +0xb8: hundredths of the hit points burnt at once)
    long mDamage;                           // +0x14 (record +0xbc: per discrete update)
    unsigned long mSpreadTime;              // +0x18 (record +0xc0)
    unsigned long mUnitSet;                 // +0x1c
    long mMode;                             // +0x20
    unsigned long mUpdates;                 // +0x24 (the calamity's discrete updates of this helper update)
};

// The leprosy's functor (RTTI EELeprosyFunctor > EEContagionFunctor; vtable 0x43cfe0; EELeprosyFunctor.cpp
// 0xb6e58..0xb6f21; sizeof 0x1c). Its implicit dtor and ??_G are ICF-folded (0xac9e7, 0xac9cb).
class EELeprosyFunctor : public EEContagionFunctor {
public:
    // 0xb6e58 (provisional names)
    EELeprosyFunctor(long inDamage, unsigned long inSpreadTime, unsigned long inValue, long inMode);
    // +0x08 (ICF-folded 0xb6c4d, EEFireFunctor's: the unit set test, not the base's `true`)
    virtual bool CanInfectType(long inCategory, long inUnitType);
    virtual unsigned long GetUnitSet();     // +0x0c 0xb6e83
    virtual bool SkipsSpread();             // +0x1c (ICF-folded 0x60b1: true)
    virtual void UpdateUnits(std::vector<EEUnit*>* inUnits);  // +0x20 (ICF-folded 0x157269: empty)
    virtual void Update(float inElapsedTime, EECalamity* inCalamity);   // +0x24 0xb6eae
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity);       // +0x28 0xb6e87
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity);   // +0x30 0xb6edf

    long mL04;                              // +0x04 (not set by the ctor; provisional name)
    long mDamage;                           // +0x08
    unsigned long mSpreadTime;              // +0x0c
    unsigned long mUnitSet;                 // +0x10
    long mMode;                             // +0x14
    unsigned long mUpdates;                 // +0x18
};

// The plague's functor (RTTI EEPlagueFunctor > EEContagionFunctor; vtable 0x43d01c; EEPlagueFunctor.cpp
// 0xb6fb1..0xb70cc; sizeof 0x20: EECCPlague::Initialize's new). Implicit dtor (ICF-folded ??_G 0xac9cb).
class EEPlagueFunctor : public EEContagionFunctor {
public:
    // 0xb6fb1: stores its arguments at +0x04..+0x18 (provisional names)
    EEPlagueFunctor(float inRadius, float inChance, long inDamage, long inDamageTime, unsigned long inValue, long inMode);
    virtual bool InfectsAlliesOnly();       // +0x04 (ICF-folded 0x27c0b4: false)
    virtual bool CanInfectType(long inCategory, long inUnitType);   // +0x08 (ICF-folded 0xb6c4d)
    virtual unsigned long GetUnitSet();     // +0x0c 0xb6fee
    virtual float GetRadius();              // +0x10 0xb6fe4
    virtual float GetChance();              // +0x14 (ICF-folded 0xb6c49)
    virtual unsigned long GetSpreadTime();  // +0x18 0xb6fe8 (1000)
    virtual void Update(float inElapsedTime, EECalamity* inCalamity);   // +0x24 0xb6ff2
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity);       // +0x28 0xb706c
    virtual void Cure(EEUnit* inUnit, EECalamity* inCalamity);         // +0x2c 0xb70a9
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity);   // +0x30 0xb7023

    float mRadius;                          // +0x04
    float mChance;                          // +0x08
    long mDamage;                           // +0x0c
    unsigned long mDamageTime;              // +0x10
    unsigned long mUnitSet;                 // +0x14
    long mMode;                             // +0x18
    unsigned long mUpdates;                 // +0x1c
};

class EEAreaEffectFunctor : public EEContagionFunctor {   // sizeof 0x20 (the subclasses' fields start there)
public:
    // 0xac991: stores its arguments at +0x04..+0x18 in order and zeroes +0x1c (provisional names)
    EEAreaEffectFunctor(EEGraphicEffect* inEffect2, EEGraphicEffect* inEffect3, long inDamage, long inUpdateTime,
                        unsigned long inValue, long inMode);
    virtual ~EEAreaEffectFunctor();         // +0x00 (??_G 0xac9cb)
    // its overrides, all ICF-folded with the sibling functors' bodies (EEAreaEffectFunctor.cpp)
    virtual bool CanInfectType(long inCategory, long inUnitType);   // +0x08 (ICF 0xb6c4d)
    virtual unsigned long GetUnitSet();     // +0x0c (ICF 0xb6fee: mValue)
    virtual bool SkipsSpread();             // +0x1c (ICF 0x60b1: true)
    virtual void UpdateUnits(std::vector<EEUnit*>* inUnits);  // +0x20 (ICF 0x157269: empty)
    virtual void Update(float inElapsedTime, EECalamity* inCalamity);   // +0x24 (ICF 0xb6ff2)
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity);       // +0x28 0xaca13
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity);   // +0x30 0xacad4

    // members in ctor argument order (the ctor stores +0x04..+0x18 and zeroes +0x1c; provisional names, after
    // the sibling functors)
    EEGraphicEffect* mEffect2;              // +0x04
    EEGraphicEffect* mEffect3;              // +0x08
    long mDamage;                           // +0x0c
    long mUpdateTime;                       // +0x10
    unsigned long mValue;                   // +0x14
    long mMode;                             // +0x18
    unsigned long mUpdates;                 // +0x1c (= 0; its slot Apply updates it)
};

class EEAEExtraDamageFunctor : public EEAreaEffectFunctor {   // vtable 0x43c5b0; sizeof 0x24
public:
    EEAEExtraDamageFunctor(unsigned long inValue, long inMode, bool inFlag);   // 0xac7b9 (provisional names)
    virtual ~EEAEExtraDamageFunctor();      // +0x00 0xac805 (??_G 0xac7e9)
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity);       // +0x28 0xac81c
    virtual void Cure(EEUnit* inUnit, EECalamity* inCalamity);         // +0x2c 0xac881
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity);   // +0x30 (ICF 0xac98e: empty)

    bool mFlag;                             // +0x20
};

class EEAEMindControlFunctor : public EEAreaEffectFunctor {   // vtable 0x43c5ec; sizeof 0x24
public:
    EEAEMindControlFunctor(EEGraphicEffect* inEffect, unsigned long inValue, long inMode);   // 0xac891
    virtual ~EEAEMindControlFunctor();      // +0x00 0xac8d6 (??_G 0xac8ba)
    virtual void Infect(EEUnit* inUnit, EECalamity* inCalamity);       // +0x28 0xac8e1
    virtual void Cure(EEUnit* inUnit, EECalamity* inCalamity);         // +0x2c 0xac918
    virtual void Apply(EEUnit* inUnit, float inElapsedTime, EECalamity* inCalamity);   // +0x30 0xac98e (empty)

    EEGraphicEffect* mEffect;               // +0x20
};

#endif
