// C:\EEX\Empire Earth\Graphics Thread\EEE2DLinearProjectile.h (RECONSTRUCTION; file name inferred). An animated
// sprite flying along a straight line from the shooter's weapon tag point (or its model's default point) to the
// target (RTTI EEE2DLinearProjectile > EEEffect2D; vtable 0x43fd68; EEE2DLinearProjectile.cpp 0xe2edd..0xe3954;
// sizeof 0x94; kind 0x30). Names are provisional. Requires ee/EEEffect2D.h, lle/GE3DLine.h.
#ifndef EE_EEE2DLINEARPROJECTILE_H
#define EE_EEE2DLINEARPROJECTILE_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DLinearProjectile : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DLinearProjectile-Data" (chain 0xe2edd)

    // 0xe2f20: from inUnit's weapon tag point to inTarget's model point
    EEE2DLinearProjectile(EEUnit* inUnit, EEUnit* inTarget, EEGraphicEffect* inGraphicEffect);
    // 0xe322f: likewise to inTarget
    EEE2DLinearProjectile(EEUnit* inUnit, const GE3DPoint& inTarget, EEGraphicEffect* inGraphicEffect);
    EEE2DLinearProjectile();                // 0xe3486 (the load-time ctor)
    virtual ~EEE2DLinearProjectile();       // +0x00 0xe34ba (??_G 0xe3213)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe3656 (draws the sprite model)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe34d8
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe355a (the speed from the duration)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe35c9
    // +0x3c: the original slot is 0xe93e0 (the ICF-shared `return EEEffect::RepairPointers()`), not EEEffect2D's 0xe8eb1
    // (full-link vslot conflict)
    virtual long RepairPointers() { return EEEffect::RepairPointers(); }
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe37e1
    virtual long Step();                    // +0x4c 0xe3860 (moves along the line, animates)

    GE3DLine mLine;                         // +0x54 (start, unnormalised direction; saved twice)
    GE3DPoint mDrawPosition;                // +0x6c (Virtual24's copy of mPosition)
    EEGraphicObject* mGraphicObject;        // +0x78 (UnloadMedia in the dtor)
    float mT;                               // +0x7c the position on the line, 0..1 (saved)
    float mDrawT;                           // +0x80 (Virtual24's copy of mT)
    float mSpeed;                           // +0x84 (saved)
    float mTime;                            // +0x88 the time into the frame (saved)
    unsigned long mFrame;                   // +0x8c the animation frame (saved)
    unsigned long mDrawFrame;               // +0x90 (Virtual24's copy of mFrame)
};

#endif
