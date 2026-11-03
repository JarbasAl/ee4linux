// C:\EEX\Empire Earth\Graphics Thread\EEE2DImpact.h (RECONSTRUCTION; file name inferred). An
// animated impact sprite at a point or a unit's tag point, with a frame-synchronised sound (RTTI EEE2DImpact >
// EEEffect2D; vtable 0x43fc44; EEE2DImpact.cpp 0xe0ffa..0xe1867; sizeof 0x88; EEE2DMuzzleFlash derives from it).
// Layout from the three ctors, the dtor, Virtual24 and Load/Save; provisional names. Requires ee/EEEffect2D.h,
// lle/GE3DPoint.h.
#ifndef EE_EEE2DIMPACT_H
#define EE_EEE2DIMPACT_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;
class SESMInstanceNode;

class EEE2DImpact : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DImpact-Data" (0x4e8848; chain 0xe0ffa, guard 0x4e8840)

    EEE2DImpact(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect, long inKind);   // 0xe103d
    EEE2DImpact(const GE3DPoint& inPosition, EEGraphicEffect* inGraphicEffect, long inKind);   // 0xe10af (no unit)
    EEE2DImpact(long inKind);               // 0xe1113 (the load-time ctor)
    virtual ~EEE2DImpact();                 // +0x00 0xe1158 (??_G 0xe1093)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe1290 (draws the sprite model)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe11ca
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d, this unit's copy)
    virtual long SetUp();                   // +0x30 0xe123e (loads the graphic object, places the effect)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe1497
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe1524
    virtual long Step();                    // +0x4c 0xe166b (animates; the sound follows the frames)
    virtual void PickTagPoint();            // +0x54 0xe158e (picks one of the unit's tag points of kind 6)

    void StopSound();                       // 0xe117b
    void ReleaseSound();                    // 0xe11a0
    void UpdatePosition();                  // 0xe15e3 (mPosition = the unit's tag point mTagIndex)
    void SetSound(SESMInstanceNode* inSound);   // 0xe17ed

    GE3DPoint mStart;                       // +0x54 (the ctor's point; the tag point's position; saved)
    GE3DPoint mDrawPosition;                // +0x60 (Virtual24's copy of mStart)
    float mLifeFraction;                    // +0x6c (saved; Virtual4C: the unit-relative age)
    float mTime;                            // +0x70 the time into the current frame (saved)
    unsigned long mFrame;                   // +0x74 the animation frame (saved)
    unsigned long mDrawFrame;               // +0x78 (Virtual24's copy of mFrame)
    EEGraphicObject* mGraphicObject;        // +0x7c (UnloadMedia in the dtor)
    SESMInstanceNode* mSound;               // +0x80
    unsigned long mTagIndex;                // +0x84 (saved from version 0.0xf5)
};

#endif
