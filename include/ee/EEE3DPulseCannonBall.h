// C:\EEX\Empire Earth\Graphics Thread\EEE3DPulseCannonBall.h (RECONSTRUCTION; file name inferred). Two pulsing models
// flying along a line from a unit's weapon tag point (vtable 0x440b88; RTTI EEE3DPulseCannonBall > EEEffect3D;
// EEE3DPulseCannonBall.cpp 0xf69e3..0xf7394). EEE3DTracers' sibling (the same line/point/object/colour head). Names are
// provisional. Requires ee/EEEffect3D.h, lle/GE3DLine.h, lle/GEColor.h.
#ifndef EE_EEE3DPULSECANNONBALL_H
#define EE_EEE3DPULSECANNONBALL_H

class EEGraphicObject;

class EEE3DPulseCannonBall : public EEEffect3D {
public:
    // 0xf6a26 (ret 0x14; EEModel request 1)
    EEE3DPulseCannonBall(EEUnit* inUnit, const GE3DPoint& inTarget, EEGraphicEffect* inGraphicEffect, long inKind,
                         long inArg5);
    EEE3DPulseCannonBall(long inKind);      // 0xf6d54
    virtual ~EEE3DPulseCannonBall();        // +0x00 0xf6da1 (??_G 0xf6d38)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xf6ef6 (the draw: the ball and its glow, model +0x248)
    // +0x20 0xf717b (EEEffect::ReplaceUnit; EEE3DBlastWave's folds into it)
    virtual void ReplaceUnit(EEUnit* inOldUnit, EEUnit* inNewUnit);
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf6dc8 (queue 4, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xf6e5e (loads the graphic; EEE3DTracers' folds into it)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xf6e75
    virtual long RepairPointers();          // +0x3c (EEEffect::RepairPointers; ICF-folded into EEE3DBlastWave's
                                            // 0xe93e0: the original vtable holds it, not EEEffect3D's inline forwarder)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xf710e
    virtual long Step();                    // +0x4c 0xf718b (moves; pulses the scales; 7 past 1.5)

    static const UString sData;             // "EEE3DPulseCannonBall-Data" (0x4e8c80; provisional name)

    GE3DLine mPath;                         // +0x74 (saved)
    GE3DPoint mLocation;                    // +0x8c (the position the draw uses)
    EEGraphicObject* mGraphicObject;        // +0x98
    GEColor mColor;                         // +0x9c
    float mScale;                           // +0xa8 the ball's (saved)
    float mDrawScale;                       // +0xac
    float mGlowScale;                       // +0xb0 (ctor 0.1)
    float mDrawGlowScale;                   // +0xb4
    float mPosition;                        // +0xb8 along mPath (saved; past 1 the ball pulses out)
    float mDrawPosition;                    // +0xbc
    float mSpeed;                           // +0xc0 per millisecond (saved)
};

#endif
