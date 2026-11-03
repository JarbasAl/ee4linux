// C:\EEX\Empire Earth\Graphics Thread\EEE2DParticle.h (RECONSTRUCTION; file name inferred).
// A pooled billboard particle with velocity, drag, gravity and wind that can bounce off the terrain (RTTI
// EEE2DParticle > EEEffect2D; vtable 0x43fea4; EEE2DParticle.cpp 0xe4729..0xe56d0 with UMemoryPool<EEE2DParticle>'s
// COMDATs at its end; sizeof 0x8c: the pool chunk's `imul 0x8c`). Names are provisional. Requires ee/EEEffect2D.h,
// lle/UMemoryPool.h, lle/GE3DLine.h.
#ifndef EE_EEE2DPARTICLE_H
#define EE_EEE2DPARTICLE_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

class EEE2DParticle : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DParticle-Data" (chain 0xe4729; guard bit 1)
    static UMemoryPool<EEE2DParticle> sPool;   // 0x4e8920 (chain 0xe476c; guard bit 2; vtable 0x43fe9c)

    EEE2DParticle(long inKind = 0x2e);      // 0xe47b1 (the pool chunk builds its objects with the default)
    virtual ~EEE2DParticle();               // +0x00 0xe4800 (??_E 0xe55fc after the pool's COMDATs)
    static bool SetPoolSize(unsigned long inChunkSize, unsigned long inMaxChunks);   // 0xe481e
    // 0xe489b / 0xe49c7 / 0xe4a4a / 0xe4adf: a particle from the pool at the unit, at a point, on a line (start and
    // velocity), or for loading
    static EEE2DParticle* Create(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);
    static EEE2DParticle* Create(const GE3DPoint& inPosition, EEGraphicEffect* inGraphicEffect);
    static EEE2DParticle* Create(const GE3DLine& inLine, EEGraphicEffect* inGraphicEffect);
    static EEE2DParticle* Create();
    static void ReleasePool();              // 0xe4b13 (stats, then the pool's release)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe5078 (the billboard: flat 0x10387b or camera-facing 0x10317d)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe4833 (Update first)
    virtual void ReleaseResources();        // +0x28 0xe4b2b
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe4b47
    virtual void LastDereference();         // +0x34 0xe4f32 (back to the pool)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe4f4b
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe517c
    virtual long Step();                    // +0x4c 0xe520d

    void Clear();                           // 0xe4be3
    long Update();                          // 0xe4c31 (moves the particle by the time since mLastTime; 7: done)

    GEVector mVelocity;                     // +0x54 (saved)
    GE3DPoint mDrawPosition;                // +0x60 (Update's new position)
    EEGraphicObject* mGraphicObject;        // +0x6c (Virtual28 unloads it)
    float mDrawAge;                         // +0x70 0..1
    float mAge;                             // +0x74 in ms (saved)
    float mRotation;                        // +0x78 (saved)
    float mBrightness;                      // +0x7c (random around the effect's mF2C by mF30, clamped to 0..1; saved)
    float mLastTime;                        // +0x80 (gPhysics->mF94 at the last update; saved since minor 0xda)
    float mDuration;                        // +0x84 (the effect's mDuration)
    bool mMoved;                            // +0x88 (Update moved it; Virtual4C applies mDrawPosition)
};

#endif
