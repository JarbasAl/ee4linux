// C:\EEX\Empire Earth\Graphics Thread\EEE3DLinearProjectile.h (RECONSTRUCTION; file name inferred). A model flying
// along a straight line from one point to another, its orbiting parts turning (vtable 0x440938; RTTI
// EEE3DLinearProjectile > EEEffect3D; EEE3DLinearProjectile.cpp 0xf2d93..0xf3580). Names are provisional.
// Requires ee/EEEffect3D.h, ee/EEGraphicObject.h, lle/GE3DLine.h.
#ifndef EE_EEE3DLINEARPROJECTILE_H
#define EE_EEE3DLINEARPROJECTILE_H

#include <vector>

class EEGraphicObject;
class EEOrbital;

class EEE3DLinearProjectile : public EEEffect3D {
public:
    // 0xf2dd6 (kind 0x25, no unit; EEEffectManager 0x1066e8)
    EEE3DLinearProjectile(GE3DPoint& inFrom, GE3DPoint& inTo, EEGraphicEffect* inGraphicEffect);
    EEE3DLinearProjectile();                // 0xf2f45
    virtual ~EEE3DLinearProjectile();       // +0x00 0xf2f9f (??_G 0xf2f29)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 0xf3172 (the draw)
    virtual long DrawPass6();               // +0x18 (`return 0`)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf2ffe (queue 3, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xf30ba (loads the graphic, copies its orbitals)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xf3373
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xf346d
    virtual long Step();                    // +0x4c 0xf34b9 (moves along the line; 7 at its end)

    static const UString sData;             // "EEE3DLinearProjectile-Data" (0x4e8be8; provisional name)

    std::vector<EEOrbital*> mOrbitals;      // +0x74
    std::vector<float> mAngles;             // +0x84
    GE3DLine mPath;                         // +0x94
    GE3DPoint mLocation;                    // +0xac (the position the draw uses)
    EEGraphicObject* mGraphicObject;        // +0xb8
    float mPosition;                        // +0xbc 0..1 along mPath (saved)
    float mDrawPosition;                    // +0xc0 (Virtual24 copies mPosition)
    float mSpeed;                           // +0xc4 per millisecond (saved)
    long mOrbitalCount;                     // +0xc8
};

#endif
