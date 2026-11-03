// C:\EEX\Empire Earth\Graphics Thread\EEE3DNuclearExplosion.h (RECONSTRUCTION; file name inferred). The mushroom
// cloud: two rising, scrolling models of the graphic (parts 11 and 9) over a ground flash
// (vtable 0x440a60; RTTI EEE3DNuclearExplosion > EEEffect3D; EEE3DNuclearExplosion.cpp 0xf43c7..0xf4bf3, key chain
// "EEE3DNuclearExplosion-Data"; kind 0x28). Names are provisional. Requires ee/EEEffect3D.h.
#ifndef EE_EEE3DNUCLEAREXPLOSION_H
#define EE_EEE3DNUCLEAREXPLOSION_H

class EEGraphicObject;

class EEE3DNuclearExplosion : public EEEffect3D {
public:
    EEE3DNuclearExplosion(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xf440a (kind 0x28)
    EEE3DNuclearExplosion();                // 0xf4465
    virtual ~EEE3DNuclearExplosion();       // +0x00 0xf4479 (??_G 0xf4449)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xf4601 (the draw)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf449d (draw copies; queue 4)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xf4524 (at the unit, or the centre of its tiles on the ground)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xf4af8
    virtual long RepairPointers();          // +0x3c (EEEffect::RepairPointers: ICF-folded into EEE3DBlastWave's 0xe93e0)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xf4b41
    virtual long Step();                    // +0x4c 0xf4b79 (7 when the rise ends)

    static const UString sData;             // "EEE3DNuclearExplosion-Data" (0x4e8c00; provisional name)

    EEGraphicObject* mGraphicObject;        // +0x74
    float mFlash;                           // +0x78 0..1 by the duration (saved)
    float mDrawFlash;                       // +0x7c (Virtual24 copies mFlash)
    float mRise;                            // +0x80 0..1 by the effect's mF28 (saved)
    float mDrawRise;                        // +0x84 (Virtual24 copies mRise)
    float mScroll;                          // +0x88 this tick's texture scroll
    float mDrawScroll;                      // +0x8c (Virtual24 takes mScroll and clears it)
};

#endif
