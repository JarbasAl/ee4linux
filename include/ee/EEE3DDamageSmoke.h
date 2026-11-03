// C:\EEX\Empire Earth\Graphics Thread\EEE3DDamageSmoke.h (RECONSTRUCTION; file name inferred).
// Smoke puffs from a damaged unit's tag points, more often the lower its hit points (vtable 0x4404b4; RTTI
// EEE3DDamageSmoke > EEE3DSmokeGenerator; EEE3DDamageSmoke.cpp 0xea703..0xea9e2, no key chain). Names are provisional.
// Requires ee/EEE3DSmokeGenerator.h.
#ifndef EE_EEE3DDAMAGESMOKE_H
#define EE_EEE3DDAMAGESMOKE_H

class EEE3DDamageSmoke : public EEE3DSmokeGenerator {
public:
    EEE3DDamageSmoke(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xea703 (kind 0x2c)
    EEE3DDamageSmoke();                     // 0xea73d
    virtual ~EEE3DDamageSmoke();            // +0x00 0xea751 (??_G 0xea721; the base's empty dtor 0xf43b8)
    // +0x4c 0xea75c: a puff (mLinkedEffect1) at every tag point of kind 0 once the time since the last one passes the
    // interval for the unit's health; 7 above the effect's health limit or under command 0x1773
    virtual long Step();
};

#endif
