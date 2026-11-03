// C:\EEX\Empire Earth\Graphics Thread\EEE2DWinterBreath.h (RECONSTRUCTION; file name inferred). A unit's breath cloud
// in the cold: three rotated billboards drifting from the unit's breath tag point
// (kind 0x16) along its facing (RTTI EEE2DWinterBreath > EEEffect2D; vtable 0x440270; EEE2DWinterBreath.cpp
// 0xe8158..0xe89c6; sizeof 0x8c). Its Virtual24 0xe844d is the ICF-kept copy for EEE2DSmokeMuzzleBlast's; its Virtual30
// folds into EEE2DSmokeMuzzleBlast's 0xe6b62. Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DWINTERBREATH_H
#define EE_EEE2DWINTERBREATH_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DWinterBreath : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DWinterBreath-Data" (chain 0xe8158)

    EEE2DWinterBreath(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe819b (kind 0x16)
    EEE2DWinterBreath();                    // 0xe83f5 (the load-time ctor)
    virtual ~EEE2DWinterBreath();           // +0x00 0xe842f (??_G 0xe83d9)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe84c0 (three billboards 0x10387b)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe844d
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 (0xe6b62)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe86b0
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe8778
    virtual long Step();                    // +0x4c 0xe88c9 (ages; drifts with the wind)

    // 0xe8832: the mouth point of inUnit's class (an offset by unit type through inTransformation) (provisional name)
    GE3DPoint GetMouthPoint(EEUnit* inUnit, unsigned long inFrame, const GETransformation& inTransformation);

    GEVector mVelocity;                     // +0x54 (normalised facing; saved)
    GE3DPoint mDrawPosition;                // +0x60 (Virtual24's copy of mPosition; saved)
    EEGraphicObject* mGraphicObject;        // +0x6c (UnloadMedia in the dtor)
    float mAge;                             // +0x70 in ms (saved)
    float mDrawAge;                         // +0x74 (Virtual24's copy of mAge; saved)
    unsigned long mDuration;                // +0x78 (the effect's mDuration; saved)
    float mFade;                            // +0x7c (the effect's mF30; saved)
    float mSpeed;                           // +0x80 (the effect's mF28: the speed; saved)
    float mSlowDownTime;                    // +0x84 (the effect's mF2C; saved)
    float mRotation;                        // +0x88 (random 0..2pi; saved)
};

#endif
