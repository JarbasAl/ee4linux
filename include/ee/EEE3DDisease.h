// C:\EEX\Empire Earth\Graphics Thread\EEE3DDisease.h (RECONSTRUCTION; file name inferred).
// The plague's 3D effect: the graphic's model drawn on the unit, its orbiting parts copied from the graphic and
// turned every frame, an animation and a sound (vtable 0x44061c; RTTI EEE3DDisease > EEEffect3D; EEE3DDisease.cpp
// 0xebde9..0xec6e4). sizeof 0xc0. Names are provisional. Requires ee/EEEffect3D.h, ee/EEGraphicObject.h.
#ifndef EE_EEE3DDISEASE_H
#define EE_EEE3DDISEASE_H

#include <vector>

class EEGraphicObject;
class EEOrbital;
class SESMInstanceNode;

class EEE3DDisease : public EEEffect3D {
public:
    EEE3DDisease(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xebe2c (kind 0x23)
    EEE3DDisease();                         // 0xebec4
    virtual ~EEE3DDisease();                // +0x00 0xebf1d (??_G 0xebea8)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 0xec19d (the draw)
    virtual long DrawPass6();               // +0x18 (`return 0`)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xebfd0 (queue 2, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xec098 (loads the graphic, copies its orbitals)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xec3aa
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xec487
    virtual long Step();                    // +0x4c 0xec4d7 (animation, orbitals, sound; 7 once over)

    void StopSound();                       // 0xebf81
    void ReleaseSound();                    // 0xebfa6
    void SetSound(SESMInstanceNode* inSound);   // 0xec6bd

    static const UString sData;             // "EEE3DDisease-Data" (0x4e8b28; provisional name)

    std::vector<EEOrbital*> mOrbitals;      // +0x74 (copies of the graphic's; deleted by the dtor)
    std::vector<float> mAngles;             // +0x84 (the orbitals' angles the draw uses; Virtual24 copies them)
    GE3DPoint mLocation;                    // +0x94 (the position the draw uses)
    float mTime;                            // +0xa0 the time into the current frame (saved)
    unsigned long mFrame;                   // +0xa4 the animation frame (saved)
    unsigned long mDrawFrame;               // +0xa8 (Virtual24 copies mFrame)
    float mAge;                             // +0xac 0..1 (saved)
    float mDrawAge;                         // +0xb0 (Virtual24 copies mAge)
    long mOrbitalCount;                     // +0xb4
    EEGraphicObject* mGraphicObject;        // +0xb8
    SESMInstanceNode* mSound;               // +0xbc
};

#endif
