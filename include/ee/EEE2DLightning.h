// C:\EEX\Empire Earth\Graphics Thread\EEE2DLightning.h (RECONSTRUCTION; file name inferred). An
// animated lightning sprite model above a unit (its model's +0x158 point) or at a point, with a frame-synchronised
// sound (RTTI EEE2DLightning > EEEffect2D; vtable 0x43fd0c; EEE2DLightning.cpp 0xe28be..0xe2edd; sizeof 0x68). Its
// Virtual24 0xe2a0b is the ICF-kept copy for EEE2DShockwave's. Names are provisional. Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DLIGHTNING_H
#define EE_EEE2DLIGHTNING_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;
class SESMInstanceNode;

class EEE2DLightning : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DLightning-Data" (chain 0xe28be)

    EEE2DLightning(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe2901 (kind 8)
    EEE2DLightning(const GE3DPoint& inPosition, EEGraphicEffect* inGraphicEffect);   // 0xe294b
    EEE2DLightning();                       // 0xe2982 (the load-time ctor)
    virtual ~EEE2DLightning();              // +0x00 0xe29a2 (??_G 0xe292f)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe2ba8 (draws on frames 0 and 2)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe2a0b
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe2a6c
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe2d31
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe2d7b
    virtual long Step();                    // +0x4c 0xe2db3 (animates once, the sound)

    void StopSound();                       // 0xe29c5
    void ReleaseSound();                    // 0xe29e7
    void SetSound(SESMInstanceNode* inSound);   // 0xe2ebc

    float mTime;                            // +0x54 the time into the frame (saved)
    unsigned long mFrame;                   // +0x58 the animation frame (saved)
    unsigned long mDrawFrame;               // +0x5c (Virtual24's copy of mFrame)
    EEGraphicObject* mGraphicObject;        // +0x60 (UnloadMedia in the dtor)
    SESMInstanceNode* mSound;               // +0x64
};

#endif
