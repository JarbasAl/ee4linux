// C:\EEX\Empire Earth\Graphics Thread\EEE2DPlasmaPulse.h (RECONSTRUCTION; file name inferred).
// An animated sprite model on a unit with a frame-synchronised sound (RTTI EEE2DPlasmaPulse > EEEffect2D; vtable
// 0x43ff04; EEE2DPlasmaPulse.cpp 0xe56d0..0xe5c63; sizeof 0x74). Its Virtual24 0xe5804 is the ICF-kept copy for
// EEE2DFireTrail's. Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DPLASMAPULSE_H
#define EE_EEE2DPLASMAPULSE_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;
class SESMInstanceNode;

class EEE2DPlasmaPulse : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DPlasmaPulse-Data" (chain 0xe56d0)

    EEE2DPlasmaPulse(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe5713 (kind 0xa)
    EEE2DPlasmaPulse();                     // 0xe576c (the load-time ctor)
    virtual ~EEE2DPlasmaPulse();            // +0x00 0xe579b (??_G 0xe5750)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe58f0 (draws the sprite model)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe5804
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe5877
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe5a78
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe5abe
    virtual long Step();                    // +0x4c 0xe5af6 (follows the unit, animates, the sound)

    void StopSound();                       // 0xe57be
    void ReleaseSound();                    // 0xe57e0
    void SetSound(SESMInstanceNode* inSound);   // 0xe5c42

    GE3DPoint mDrawPosition;                // +0x54 (Virtual24's copy of mPosition)
    float mTime;                            // +0x60 the time into the frame (saved)
    unsigned long mFrame;                   // +0x64 the animation frame (saved)
    unsigned long mDrawFrame;               // +0x68 (Virtual24's copy of mFrame)
    EEGraphicObject* mGraphicObject;        // +0x6c (UnloadMedia in the dtor)
    SESMInstanceNode* mSound;               // +0x70
};

#endif
