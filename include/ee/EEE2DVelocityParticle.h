// C:\EEX\Empire Earth\Graphics Thread\EEE2DVelocityParticle.h (RECONSTRUCTION; file name inferred). A smoke puff
// thrown along a line's direction, slowing down over the effect's mL64 (RTTI EEE2DVelocityParticle > EEE2DSmokePlume;
// vtable 0x440158; EEE2DVelocityParticle.cpp 0xe753d..0xe773c; sizeof 0x8c).
// Names are provisional. Requires ee/EEE2DSmokePlume.h, lle/GE3DLine.h.
#ifndef EE_EEE2DVELOCITYPARTICLE_H
#define EE_EEE2DVELOCITYPARTICLE_H

#include "ee/EEE2DSmokePlume.h"

class EEE2DVelocityParticle : public EEE2DSmokePlume {
public:
    static const UString sData;             // "EEE2DVelocityParticle-Data" (0x4e89f0; chain 0xe753d, guard 0x4e89e8)

    EEE2DVelocityParticle(const GE3DLine& inLine, EEGraphicEffect* inGraphicEffect);   // 0xe7580 (kind 0xc)
    // inline: the load-time ctor (COMDAT 0x1072a8, after its first user in EEEffectManager.cpp's range)
    EEE2DVelocityParticle() : EEE2DSmokePlume(0xc) {}
    virtual ~EEE2DVelocityParticle() {}    // +0x00 inline (??_G 0xe75d4: own vptr, then ~EEE2DSmokePlume)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe75f6
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe761e
    virtual long Step();                    // +0x4c 0xe7646

    GEVector mVelocity;                     // +0x80 (the line's direction; saved)
};

#endif
