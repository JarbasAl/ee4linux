// C:\EEX\Empire Earth\Graphics Thread\EEE2DSmokePlume.h (RECONSTRUCTION; file name inferred). A
// smoke puff that rises and drifts with the wind (RTTI EEE2DSmokePlume > EEEffect2D; vtable 0x440084;
// EEE2DSmokePlume.cpp 0xe6ef2..0xe72fa; sizeof 0x80; EEE2DVelocityParticle derives from it). Names are provisional.
// Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DSMOKEPLUME_H
#define EE_EEE2DSMOKEPLUME_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DSmokePlume : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DSmokePlume-Data" (chain 0xe6ef2)

    EEE2DSmokePlume(const GE3DPoint& inPosition, EEGraphicEffect* inGraphicEffect, long inKind);   // 0xe6f35
    EEE2DSmokePlume(long inKind);           // 0xe7020 (the load-time ctor)
    virtual ~EEE2DSmokePlume();             // +0x00 0xe705e (??_G 0xe7004)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe711d (the billboard 0x10387b)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe707c
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe70ef
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe718e
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe71f1
    virtual long Step();                    // +0x4c 0xe7243 (ages; drifts with the wind, rises)

    float mAge;                             // +0x54 0..1 (saved)
    GE3DPoint mStart;                       // +0x58 the puff's position (saved)
    GE3DPoint mDrawPosition;                // +0x64 (Virtual24's copy of mStart)
    EEGraphicObject* mGraphicObject;        // +0x70 (UnloadMedia in the dtor)
    float mDrawAge;                         // +0x74 (Virtual24's copy of mAge)
    float mRotation;                        // +0x78 (random 0..2pi; saved)
    float mBrightness;                      // +0x7c (random around the effect's mF28 by mF2C, clamped to 0..1; saved)
};

#endif
