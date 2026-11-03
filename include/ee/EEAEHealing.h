// EEAEHealing.h (RECONSTRUCTION; file name inferred). The healing area effect (RTTI EEAEHealing > EEAreaEffect;
// vtable 0x44baec; EEAEHealing.cpp 0x27ace1..0x27adb0; EEAEManager::CreateAreaEffect makes it for template kind 1).
// Names are provisional. Requires ee/EEAreaEffect.h.
#ifndef EE_EEAEHEALING_H
#define EE_EEAEHEALING_H

class EEAEHealing : public EEAreaEffect {
public:
    EEAEHealing(EEAreaEffectTemplate* inTemplate);   // 0x27ace1
    virtual ~EEAEHealing() {}               // +0x00 (inline: ??_G 0x27acfe stores the vptr and calls the base dtor)
    // +0x08 0x27ad20: only units that IsRepairable accepts, then the base test
    virtual bool Covers(unsigned long inPlayer, EEComplexUnit* inUnit, const GE2DPoint& inPoint);
    virtual long GetStrength();             // +0x18 0x27ad4f: mTemplate->mL40
    // +0x30 (inline: emitted with the vtable after the ctor, 0x27acf9; the first `xor al, al; ret 0xc` in link
    // order, so LINK's ICF folds EEAreaEffect's +0x2c/+0x30/+0x34 into it): healing modifies no value. Slot
    // (provisional name; +0x30 is the one the other effects override)
    virtual bool Modify(long inKind, long inValue, long& outValue) { return false; }
    // +0x48 0x27ad56: heals a living unit by mTemplate->mL40 per second (at least 1), up to its maximum
    virtual void Tick(EEComplexUnit* inUnit, float inElapsed);
};

#endif
