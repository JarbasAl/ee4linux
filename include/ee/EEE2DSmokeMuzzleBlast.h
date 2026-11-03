// C:\EEX\Empire Earth\Graphics Thread\EEE2DSmokeMuzzleBlast.h (RECONSTRUCTION; file name inferred). A gun's smoke
// puff: three rotated billboards drifting from the shooter's weapon tag point towards the
// target (RTTI EEE2DSmokeMuzzleBlast > EEEffect2D; vtable 0x44001c; EEE2DSmokeMuzzleBlast.cpp 0xe6621..0xe6ef2;
// sizeof 0x7c). Its Virtual24 folds into EEE2DWinterBreath's 0xe844d; its Virtual30 0xe6b62 is the ICF-kept copy for
// EEE2DWinterBreath's. Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DSMOKEMUZZLEBLAST_H
#define EE_EEE2DSMOKEMUZZLEBLAST_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DSmokeMuzzleBlast : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DSmokeMuzzleBlast-Data" (chain 0xe6621)

    // 0xe6664: from inUnit's weapon tag point towards inTarget's model point (kind 0x14)
    EEE2DSmokeMuzzleBlast(EEUnit* inUnit, EEUnit* inTarget, EEGraphicEffect* inGraphicEffect);
    // 0xe691e: likewise towards inTarget
    EEE2DSmokeMuzzleBlast(EEUnit* inUnit, const GE3DPoint& inTarget, EEGraphicEffect* inGraphicEffect);
    EEE2DSmokeMuzzleBlast();                // 0xe6b0a (the load-time ctor)
    virtual ~EEE2DSmokeMuzzleBlast();       // +0x00 0xe6b44 (??_G 0xe6902)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe6b76 (three billboards 0x10387b)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 (0xe844d)
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe6b62
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe6d5a
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe6dae
    virtual long Step();                    // +0x4c 0xe6df1 (ages; drifts with the wind)

    GEVector mVelocity;                     // +0x54 (normalised, towards the target; saved)
    GE3DPoint mDrawPosition;                // +0x60 (Virtual24's copy of mPosition)
    EEGraphicObject* mGraphicObject;        // +0x6c (UnloadMedia in the dtor)
    float mAge;                             // +0x70 in ms (saved)
    float mDrawAge;                         // +0x74 (Virtual24's copy of mAge)
    float mRotation;                        // +0x78 (random 0..2pi; saved)
};

#endif
