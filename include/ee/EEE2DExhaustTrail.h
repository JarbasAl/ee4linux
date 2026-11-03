// C:\EEX\Empire Earth\Graphics Thread\EEE2DExhaustTrail.h (RECONSTRUCTION; file name inferred).
// Spinning exhaust sprites at a unit's tag points (RTTI EEE2DExhaustTrail > EEEffect2D; vtable 0x43fac4;
// EEE2DExhaustTrail.cpp 0xdf68d..0xdfd03; sizeof 0x60). Queued on pass 3. Names are provisional. Requires
// ee/EEEffect2D.h.
#ifndef EE_EEE2DEXHAUSTTRAIL_H
#define EE_EEE2DEXHAUSTTRAIL_H

#include "ee/EEEffect2D.h"

class EEE2DExhaustTrail : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DExhaustTrail-Data" (0x4e8800; chain 0xdf68d)

    EEE2DExhaustTrail(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xdf6d0 (kind 3)
    EEE2DExhaustTrail();                    // 0xdf736 (the load-time ctor)
    virtual ~EEE2DExhaustTrail();           // +0x00 0xdf752 (??_G 0xdf71a)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 0xdf857 (draws the parts, turned by mDrawAngle)
    virtual long DrawPass6();               // +0x18 (0xfca98)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xdf75d
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xdf816 (the parts at the tag points)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xdfac1
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xdfaf6
    virtual long Step();                    // +0x4c 0xdfb2b (spins, animates; 7 once every part has played)

    void UpdatePart(unsigned long inIndex); // 0xdfbe6

    float mAngle;                           // +0x54 (saved)
    float mDrawAngle;                       // +0x58 (Virtual24's copy of mAngle)
    bool mClockwise;                        // +0x5c (random in the ctor; saved)
};

#endif
