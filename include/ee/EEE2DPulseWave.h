// C:\EEX\Empire Earth\Graphics Thread\EEE2DPulseWave.h (RECONSTRUCTION; file name inferred). A
// ring model on a unit that pulses out when the unit is hit (RTTI EEE2DPulseWave > EEEffect2D; vtable 0x43ff60;
// EEE2DPulseWave.cpp 0xe5c63..0xe614f; sizeof 0x70). Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DPULSEWAVE_H
#define EE_EEE2DPULSEWAVE_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DPulseWave : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DPulseWave-Data" (chain 0xe5c63)

    EEE2DPulseWave(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe5ca6 (kind 0x2f, model gU4E8DF4)
    EEE2DPulseWave();                       // 0xe5cff (the load-time ctor)
    virtual ~EEE2DPulseWave();              // +0x00 0xe5d1b (??_G 0xe5ce3)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe5dc6 (draws the ring, scaled by mDrawRadius)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe5d39
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe5da0
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe5fa0
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe6004
    virtual long Step();                    // +0x4c 0xe605a

    EEGraphicObject* mGraphicObject;        // +0x54 (UnloadMedia in the dtor)
    float mAge;                             // +0x58 0..1 (saved)
    float mDrawAge;                         // +0x5c (Virtual24's copy of mAge)
    float mRadius;                          // +0x60 (between the effect's mF2C and mF30; saved)
    float mDrawRadius;                      // +0x64 (Virtual24's copy of mRadius)
    long mHitPoints;                        // +0x68 the unit's hit points last frame (saved)
    bool mShrinking;                        // +0x6c (saved)
};

#endif
