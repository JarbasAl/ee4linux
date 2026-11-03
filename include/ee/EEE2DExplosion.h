// C:\EEX\Empire Earth\Graphics Thread\EEE2DExplosion.h (RECONSTRUCTION; file name inferred).
// Animated explosion sprites at the unit's tag points, each played once (RTTI EEE2DExplosion > EEEffect2D; vtable
// 0x43fb2c; EEE2DExplosion.cpp 0xdfd03..0xe0389; sizeof 0x58). Its StopSound/SetSound are the ICF-kept copies for
// EEE2DFire and EECalamity's secondary sound (0xdfd80 named EECalamity::StopSecondarySound; ReleaseSound folds into
// EECalamity::ReleaseSecondarySound 0xad2ad). Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DEXPLOSION_H
#define EE_EEE2DEXPLOSION_H

#include "ee/EEEffect2D.h"

class SESMInstanceNode;

class EEE2DExplosion : public EEEffect2D {
public:
    EEE2DExplosion(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xdfd03 (kind 4)
    EEE2DExplosion();                       // 0xdfd49 (the load-time ctor)
    virtual ~EEE2DExplosion();              // +0x00 0xdfd69 (??_G 0xdfd2d)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xdfe9a (draws every part)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xdfda2
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xdfe55 (the parts at the tag points, 0.4 above the ground)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe00ac
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe00c0
    virtual long Step();                    // +0x4c 0xe00d4 (7 once every part has played)

    void StopSound();                       // 0xdfd80 (the ICF-kept copy)
    void ReleaseSound();                    // (ICF: 0xad2ad)
    void UpdatePart(unsigned long inIndex); // 0xe0145 (animates part inIndex, its sound)
    void SetSound(SESMInstanceNode* inSound);   // 0xe0368
    // inline; UpdatePart 0xe0145 reads mSound through it (inline-budget evidence only) (provisional name)
    SESMInstanceNode* GetSound() const { return this->mSound; }

    SESMInstanceNode* mSound;               // +0x54
};

#endif
