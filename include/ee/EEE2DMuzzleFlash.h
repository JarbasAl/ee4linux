// C:\EEX\Empire Earth\Graphics Thread\EEE2DMuzzleFlash.h (RECONSTRUCTION; file name inferred).
// An impact sprite at a unit's weapon tag point (RTTI EEE2DMuzzleFlash > EEE2DImpact; vtable 0x43fe3c;
// EEE2DMuzzleFlash.cpp 0xe4529..0xe4729; sizeof 0x88, no members of its own). Every slot but Virtual30 forwards to
// EEE2DImpact's. Requires ee/EEE2DImpact.h.
#ifndef EE_EEE2DMUZZLEFLASH_H
#define EE_EEE2DMUZZLEFLASH_H

#include "ee/EEE2DImpact.h"

class EEE2DMuzzleFlash : public EEE2DImpact {
public:
    EEE2DMuzzleFlash(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe4529 (kind 9)
    EEE2DMuzzleFlash();                     // 0xe4563 (the load-time ctor)
    virtual ~EEE2DMuzzleFlash();            // +0x00 0xe4577 (??_G 0xe4547)
    virtual long DrawPass6();               // +0x18 0xe46f7
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe4582
    virtual long SetUp();                   // +0x30 0xe458e (the weapon tag point through the unit's model)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe46fc
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe4710
    virtual long Step();                    // +0x4c 0xe4724
};

#endif
