// EEAEImmunity.h (RECONSTRUCTION; file name inferred). The immunity area effects, all inline (CreateAreaEffect
// 0x27b16e builds them; their ??_G 0x27b357 / 0x27b379 sit in EEAEManager.cpp's range): RTTI EEAECalamityImmunity
// (vtable 0x44bc28) and EEAEConversionImmunity (vtable 0x44bbd8), both > EEAreaEffect, overriding only +0x20
// (`mov al, 1; ret`, ICF 0x18aa9). Names are provisional. Requires ee/EEAreaEffect.h.
#ifndef EE_EEAEIMMUNITY_H
#define EE_EEAEIMMUNITY_H

class EEAECalamityImmunity : public EEAreaEffect {
public:
    EEAECalamityImmunity(EEAreaEffectTemplate* inTemplate) : EEAreaEffect(inTemplate) {}
    virtual ~EEAECalamityImmunity() {}                      // +0x00 (inline: ??_G stores the vptr, then the base dtor)
    virtual bool IsImmunity() { return true; }  // +0x20
};

class EEAEConversionImmunity : public EEAreaEffect {
public:
    EEAEConversionImmunity(EEAreaEffectTemplate* inTemplate) : EEAreaEffect(inTemplate) {}
    virtual ~EEAEConversionImmunity() {}                      // +0x00 (inline: ??_G stores the vptr, then the base dtor)
    virtual bool IsImmunity() { return true; }  // +0x20
};

#endif
