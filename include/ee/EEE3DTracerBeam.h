// C:\EEX\Empire Earth\Graphics Thread\EEE3DTracerBeam.h (RECONSTRUCTION, from EEE3DTracerBeam.cpp
// 0xfc382..0xfd330; file name inferred). A beam from a unit to a unit or a point (kind 0x40; vtable 0x440f6c; RTTI
// EEE3DTracerBeam > EEEffect3D). sizeof 0xb0. Names are provisional. Requires ee/EEEffect3D.h.
#ifndef EE_EEE3DTRACERBEAM_H
#define EE_EEE3DTRACERBEAM_H

#include "ee/EEEffect3D.h"

class EEGraphicObject;

class EEE3DTracerBeam : public EEEffect3D {
public:
    // 0xfc3c5: from inFrom to inTo's model centre (inTo referenced)
    EEE3DTracerBeam(EEUnit* inFrom, EEUnit* inTo, EEGraphicEffect* inGraphicEffect, long inKind);
    EEE3DTracerBeam(EEUnit* inFrom, GE3DPoint& inTo, EEGraphicEffect* inGraphicEffect, long inKind);   // 0xfc49d
    EEE3DTracerBeam(long inKind);           // 0xfc543 (the load-time ctor)
    virtual ~EEE3DTracerBeam();             // +0x00 0xfc574 (??_G 0xfc481)
    virtual long DrawPass2();               // +0x08 0xfca98 (`return 0`: the ICF representative of many)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xfca9b (the draw)
    virtual void ReplaceUnit(EEUnit* inOldUnit, EEUnit* inNewUnit);  // +0x20 0xfd10f
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xfc9d7
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xfca62
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xfcf21
    virtual long RepairPointers();          // +0x3c 0xfd000
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xfd04e
    virtual long Step();                    // +0x4c 0xfd140

    void Aim();                             // 0xfc5a4 (the source tag point on mUnit, towards the target) (provisional name)

    static const UString sData;             // "EEE3DTracerBeam-Data" (0x4e8d00; provisional name)

    EEUnit* mTarget;                        // +0x74 (referenced; 0: a point target)
    EEGraphicObject* mGraphicObject;        // +0x78
    GE3DPoint mTargetPoint;                 // +0x7c
    float mScroll;                          // +0x88
    float mDrawScroll;                       // +0x8c
    float mAge;                             // +0x90
    float mDrawAge;                         // +0x94
    float mLength;                           // +0x98
    float mDrawLength;                       // +0x9c
    float mWidth;                            // +0xa0
    float mDrawWidth;                        // +0xa4
    bool mIsTimed;                          // +0xa8
    bool mIsImpactCreated;                  // +0xa9
    long mWeaponTagPoint;                   // +0xac (the unit's complex unit +0x2ec)
};

#endif
