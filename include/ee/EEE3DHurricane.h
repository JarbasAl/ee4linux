// C:\EEX\Empire Earth\Graphics Thread\EEE3DHurricane.h (RECONSTRUCTION; file name inferred).
// The hurricane's 3D effect: the graphic's model on the unit, its orbiting parts, an animation and a sound (the
// EEE3DDisease shape), a scale that grows in and fades out after the unit's death, and sub-effects spawned at random
// tag points (vtable 0x440870; RTTI EEE3DHurricane > EEEffect3D; EEE3DHurricane.cpp 0xf1600..0xf2046). sizeof 0xc8.
// Names are provisional. Requires ee/EEEffect3D.h, ee/EEGraphicObject.h.
#ifndef EE_EEE3DHURRICANE_H
#define EE_EEE3DHURRICANE_H

#include <vector>

class EEGraphicObject;
class EEOrbital;
class SESMInstanceNode;

class EEE3DHurricane : public EEEffect3D {
public:
    EEE3DHurricane(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xf1643 (kind 0x24)
    EEE3DHurricane();                       // 0xf16db
    virtual ~EEE3DHurricane();              // +0x00 0xf1734 (??_G 0xf16bf)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xf1a61 (the draw)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf17c2 (queue 4, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xf1953 (loads the graphic, copies its orbitals)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xf1c17
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xf1d1c
    virtual long Step();                    // +0x4c 0xf1d90 (animation, scale, spawns, sound)
    // +0x58 0xf188a: a random tag point (kind 6) of the graphic on the unit (the spawned effects' location)
    virtual void GetSpawnPoint(GE3DPoint& outPoint);

    void StopSound();                       // ICF-folded into EEE3DVirus's 0xfdd43
    void ReleaseSound();                    // 0xf1798
    void SetSound(SESMInstanceNode* inSound);   // ICF-folded into EEE3DVirus's 0xfe52c

    static const UString sData;             // "EEE3DHurricane-Data" (0x4e8bd0; provisional name)

    std::vector<EEOrbital*> mOrbitals;      // +0x74 (copies of the graphic's; deleted by the dtor)
    std::vector<float> mAngles;             // +0x84 (the orbitals' angles the draw uses; Virtual24 copies them)
    GE3DPoint mLocation;                    // +0x94 (the position the draw uses)
    float mTime;                            // +0xa0 the time into the current frame (saved)
    float mSpawnTime;                       // +0xa4 until the next sub-effect (saved)
    float mScale;                           // +0xa8 0.1..1 (saved)
    float mDrawScale;                       // +0xac
    float mAge;                             // +0xb0 (saved)
    unsigned long mFrame;                   // +0xb4 the animation frame (saved)
    unsigned long mDrawFrame;               // +0xb8
    long mOrbitalCount;                     // +0xbc
    EEGraphicObject* mGraphicObject;        // +0xc0
    SESMInstanceNode* mSound;               // +0xc4
};

#endif
