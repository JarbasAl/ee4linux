// C:\EEX\Empire Earth\Graphics Thread\EEE3DCitizenBeam.h (RECONSTRUCTION; file name inferred).
// A beam model from a unit's first tag point towards a point while the unit plays a given animation (vtable 0x4403f4;
// RTTI EEE3DCitizenBeam > EEEffect3D; EEE3DCitizenBeam.cpp 0xe9474..0xe9bee, key chain "EEE3DCitizenBeam-Data").
// sizeof 0xc0. Names are provisional. Requires ee/EEEffect3D.h, lle/GE3DLine.h.
#ifndef EE_EEE3DCITIZENBEAM_H
#define EE_EEE3DCITIZENBEAM_H

class EEGraphicObject;

class EEE3DCitizenBeam : public EEEffect3D {
public:
    EEE3DCitizenBeam(EEUnit* inUnit, const GE3DPoint& inTarget, EEGraphicEffect* inGraphicEffect, long inKind);   // 0xe94b7
    EEE3DCitizenBeam(long inKind);          // 0xe9584
    virtual ~EEE3DCitizenBeam();            // +0x00 0xe95e2 (??_G 0xe9568)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xe9872 (the draw)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe97d7 (queue 4, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xe9855 (loads the graphic)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xe99fd
    virtual long RepairPointers();          // +0x3c (EEEffect::RepairPointers: ICF-folded into EEE3DBlastWave's 0xe93e0)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xe9a7d
    virtual long Step();                    // +0x4c 0xe9aec (on while the unit plays the effect's animation)

    void Aim();                             // 0xe9606: the beam from the unit's tag point 0 towards mEnd (provisional name)

    static const UString sData;             // "EEE3DCitizenBeam-Data" (0x4e8a80; provisional name)

    EEGraphicObject* mGraphicObject;        // +0x74
    GE3DPoint mEnd;                         // +0x78 (the ctor's point, raised by the effect's mF28; saved)
    GE3DPoint mDrawLocation;                // +0x84 (Virtual24 copies the transformation's position)
    GE3DPoint mMiddle;                      // +0x90 (half way along the beam: the sub-effect's place; saved)
    float mF9C;                             // +0x9c (saved; not set here)
    GE3DLine mLine;                         // +0xa0 (saved)
    float mScale;                           // +0xb8 (flickers with the physics tick)
    bool mActive;                           // +0xbc (saved)
    bool mDrawActive;                       // +0xbd (Virtual24 copies mActive)
};

#endif
