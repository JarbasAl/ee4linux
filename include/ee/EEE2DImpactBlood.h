// C:\EEX\Empire Earth\Graphics Thread\EEE2DImpactBlood.h (RECONSTRUCTION; file name inferred).
// An impact sprite drawn red, only with the blood option on (RTTI EEE2DImpactBlood > EEE2DImpact; vtable 0x4411dc;
// EEE2DImpactBlood.cpp 0xe1867..0xe1a37: two colour constants and Virtual18; its ctor and ??_G 0x105dff are inline
// COMDATs emitted by EEEffectManager.cpp, not declared here yet). Requires ee/EEE2DImpact.h.
#ifndef EE_EEE2DIMPACTBLOOD_H
#define EE_EEE2DIMPACTBLOOD_H

#include "ee/EEE2DImpact.h"

class EEE2DImpactBlood : public EEE2DImpact {
public:
    // inline (EEEffectManager.cpp: EEE2DImpact's ctor with kind 7, then this class's vptr)
    EEE2DImpactBlood(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect) : EEE2DImpact(inUnit, inGraphicEffect, 7) {}
    EEE2DImpactBlood() : EEE2DImpact(7) {}
    // user-declared empty (??_G 0x105dff stores this class's vptr, then ~EEE2DImpact)
    virtual ~EEE2DImpactBlood() {}
    virtual long DrawPass6();               // +0x18 0xe18c9 (EEE2DImpact's draw with the material tinted red)
};

#endif
