// C:\EEX\Empire Earth\Graphics Thread\EEE2DFire.h (RECONSTRUCTION; file name inferred). Fire
// sprites on a damaged unit's tag points: the more damage, the more parts burn (RTTI EEE2DFire > EEEffect2D; vtable
// 0x43fb88; EEE2DFire.cpp 0xe0389..0xe0cf5; sizeof 0x5c). Its StopSound/ReleaseSound/SetSound are ICF-folded into
// EEE2DExplosion's copies (0xdfd80, 0xad2ad, 0xe0368). Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DFIRE_H
#define EE_EEE2DFIRE_H

#include "ee/EEEffect2D.h"

class SESMInstanceNode;

class EEE2DFire : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DFire-Data" (0x4e8818; chain 0xe0389)

    EEE2DFire(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe03cc (kind 5)
    EEE2DFire();                            // 0xe0412 (the load-time ctor)
    virtual ~EEE2DFire();                   // +0x00 0xe0432 (??_G 0xe03f6)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe06d1 (draws the burning parts)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe0449 (the burning count from the unit's damage)
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe05e6 (the parts at the tag points, random start frames)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe08d1
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe08f7
    virtual long Step();                    // +0x4c 0xe0920

    void UpdatePart(unsigned long inIndex); // 0xe09cb (animates part inIndex, moves it with the unit, its sound)
    void StopSound();                       // (ICF: 0xdfd80)
    void ReleaseSound();                    // (ICF: 0xad2ad)
    void SetSound(SESMInstanceNode* inSound);   // (ICF: 0xe0368)

    SESMInstanceNode* mSound;               // +0x54
    long mLastPart;                         // +0x58 the last burning part (saved)
};

#endif
