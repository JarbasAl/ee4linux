// C:\EEX\Empire Earth\Graphics Thread\EEE2DFireTrail.h (RECONSTRUCTION; file name inferred). A
// rising, growing fire puff (RTTI EEE2DFireTrail > EEEffect2D; vtable 0x43fbe4; EEE2DFireTrail.cpp 0xe0cf5..0xe0ffa;
// sizeof 0x70). Layout from the ctors 0xe0d38/0xe0da5, the dtor, Virtual24 (the shared copy 0xe5804) and Load/Save;
// names are provisional. Requires ee/EEEffect2D.h, lle/GE3DPoint.h.
#ifndef EE_EEE2DFIRETRAIL_H
#define EE_EEE2DFIRETRAIL_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DFireTrail : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DFireTrail-Data" (0x4e8830; chain 0xe0cf5, guard 0x4e8828)

    EEE2DFireTrail(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe0d38 (kind 0xe)
    EEE2DFireTrail();                       // 0xe0da5 (the load-time ctor)
    virtual ~EEE2DFireTrail();              // +0x00 0xe0dd0 (??_G 0xe0d89)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF 0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe0e50 (draws the puff)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 (ICF-folded into 0xe5804, EEE2DPlasmaPulse's range)
    virtual void PrepareQueue();            // +0x2c (empty: ICF 0xe123d)
    virtual long SetUp();                   // +0x30 0xe0dee (starts at the unit, loads the graphic object)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe0ec9
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe0f0f
    virtual long Step();                    // +0x4c 0xe0f44 (ages, drifts with the wind, grows)

    GE3DPoint mDrawPosition;                // +0x54 (Virtual24's copy of mPosition)
    EEGraphicObject* mGraphicObject;        // +0x60 (UnloadMedia in the dtor)
    float mAge;                             // +0x64 (0..1 over the effect's duration; saved)
    float mDrawAge;                         // +0x68 (Virtual24's copy of mAge)
    float mRotation;                        // +0x6c (random 0..2pi; saved)
};

#endif
