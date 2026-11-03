// EEAEEconomy.h (RECONSTRUCTION; file name inferred). The economy area effects (RTTI EEAEEconomy > EEAreaEffect,
// vtable 0x44bcd0 with _purecall at +0x18/+0x4c/+0x50; EEAEEconomyFromUnits > EEAEEconomy, vtable 0x44bb80;
// EEAEEconomy.cpp 0x27ac2c..0x27ace1; EEAEManager::CreateAreaEffect makes EEAEEconomyFromUnits). Names
// are provisional. Requires ee/EEAreaEffect.h.
#ifndef EE_EEAEECONOMY_H
#define EE_EEAEECONOMY_H

class EEAEEconomy : public EEAreaEffect {
public:
    // inline (CreateAreaEffect 0x27b25c calls EEAreaEffect's ctor directly)
    EEAEEconomy(EEAreaEffectTemplate* inTemplate) : EEAreaEffect(inTemplate) {}
    virtual ~EEAEEconomy() {}               // +0x00 (inline: ??_G 0x27b3b0 stores EEAEEconomy's vptr, then the base dtor)
    virtual long GetStrength() = 0;         // +0x18 _purecall
    // +0x30 0x27ac2c: for kind 2, outValue = inValue scaled by the rate (Virtual4C clamped to 0..5)
    virtual bool Modify(long inKind, long inValue, long& outValue);
    virtual float GetRate() = 0;            // +0x4c _purecall: the rate
    virtual long Virtual50() = 0;           // +0x50 _purecall
};

class EEAEEconomyFromUnits : public EEAEEconomy {
public:
    EEAEEconomyFromUnits(EEAreaEffectTemplate* inTemplate) : EEAEEconomy(inTemplate) {}   // inline (0x27b26d)
    virtual long GetStrength() { return (long)(this->GetRate() * 10.0f); }  // +0x18 0x27b39b (inline: EEAEManager.cpp's range)
    // +0x4c 0x27ac9e: the unit's class +0x154 clamped to 0..5 (needs EEUnitClass::mL154 as a float)
    virtual float GetRate();
    virtual long Virtual50() { return this->GetStrength(); }  // +0x50 0x27b3ab (inline)
};

#endif
