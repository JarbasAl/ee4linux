// C:\EEX\Empire Earth\Graphics Thread\EEE3DMultiEffect.h (RECONSTRUCTION; file name inferred).
// An effect that only starts its graphic effect's two or three sub-effects (mEffect10/14/18) and ends (vtable
// 0x440a00; RTTI EEE3DMultiEffect > EEEffect3D; EEE3DMultiEffect.cpp 0xf4247..0xf43c7; its ??_G is ICF-folded
// into EEE3DSmokeGenerator's 0xf9929). sizeof 0x74. Names are provisional. Requires ee/EEEffect3D.h.
#ifndef EE_EEE3DMULTIEFFECT_H
#define EE_EEE3DMULTIEFFECT_H

class EEE3DMultiEffect : public EEEffect3D {
public:
    // 0xf4247 (EEEffectManager::CreatePlacedEffect 0x106484): the sub-effects through CreatePlacedEffect
    EEE3DMultiEffect(GE3DPoint& inLocation, EEGraphicEffect* inGraphicEffect, long inKind);
    // 0xf42c3 (EEEffectManager::CreateLineEffect 0x1065e9): through CreateLineEffect
    EEE3DMultiEffect(GE3DLine& inLine, EEGraphicEffect* inGraphicEffect, long inKind);
    // 0xf4339 (EEEffectManager 0x105c39): through CreateUnitEffect
    EEE3DMultiEffect(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect, long inKind);
    // +0x00 0xf43b8: empty (its own vptr store is dead: EEEffect3D's inline dtor stores its vptr next); the ICF
    // representative of every such class's dtor
    virtual ~EEE3DMultiEffect();
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf42bd (10: never drawn)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0x1720c6)
    virtual long SetUp();                   // +0x30 (`return 0`: ICF-folded into 0xfca98)
    virtual long Step();                    // +0x4c 0xf43c3 (7: done at once)
};

#endif
