// C:\EEX\Empire Earth\Graphics Thread\EEE3DVirus.h (RECONSTRUCTION; file name inferred).
// The virus's 3D effect: EEE3DDisease's shape (the graphic's model on the unit's tag point, its orbiting parts, an
// animation and a sound) plus a scrolling texture (vtable 0x44102c; RTTI EEE3DVirus > EEEffect3D; EEE3DVirus.cpp
// 0xfdbab..0xfe553). sizeof 0xc8. Names are provisional. Requires ee/EEEffect3D.h, ee/EEGraphicObject.h.
#ifndef EE_EEE3DVIRUS_H
#define EE_EEE3DVIRUS_H

#include <vector>

class EEGraphicObject;
class EEOrbital;
class SESMInstanceNode;

class EEE3DVirus : public EEEffect3D {
public:
    EEE3DVirus(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xfdbee (kind 0x42)
    EEE3DVirus();                           // 0xfdc86
    virtual ~EEE3DVirus();                  // +0x00 0xfdcdf (??_G 0xfdc6a)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 0xfdf0e (the draw)
    virtual long DrawPass6();               // +0x18 (`return 0`)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xfdd68 (queue 3, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xfde3c (loads the graphic, copies its orbitals)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xfe1d3
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xfe2c6
    virtual long Step();                    // +0x4c 0xfe328 (animation, scroll, orbitals, sound; 7 once over)

    void StopSound();                       // 0xfdd43
    void ReleaseSound();                    // ICF-folded into EEE3DHurricane's 0xf1798
    void SetSound(SESMInstanceNode* inSound);   // 0xfe52c
    void FollowUnit();                      // the transformation from the unit's tag point (ICF: 0xe904a)

    static const UString sData;             // "EEE3DVirus-Data" (0x4e8d30; provisional name)

    std::vector<EEOrbital*> mOrbitals;      // +0x74 (copies of the graphic's; deleted by the dtor)
    std::vector<float> mAngles;             // +0x84 (the orbitals' angles the draw uses; Virtual24 copies them)
    GE3DPoint mLocation;                    // +0x94 (the position the draw uses)
    float mTime;                            // +0xa0 the time into the current frame (saved)
    float mAge;                             // +0xa4 0..1 (saved)
    float mDrawAge;                         // +0xa8
    float mScroll;                          // +0xac the texture's u offset (saved)
    float mDrawScroll;                      // +0xb0
    long mOrbitalCount;                     // +0xb4
    unsigned long mFrame;                   // +0xb8 the animation frame (saved)
    unsigned long mDrawFrame;               // +0xbc
    EEGraphicObject* mGraphicObject;        // +0xc0
    SESMInstanceNode* mSound;               // +0xc4
};

#endif
