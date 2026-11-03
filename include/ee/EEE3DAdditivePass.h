// C:\EEX\Empire Earth\Graphics Thread\EEE3DAdditivePass.h (RECONSTRUCTION; file name inferred).
// The unit's own model drawn again additively over it (vtable 0x4402d8; RTTI EEE3DAdditivePass > EEEffect3D;
// EEE3DAdditivePass.cpp 0xe89c6..0xe8ed8, ending with EEEffect3D's out-of-line ctor, Virtual3C and ??_G). sizeof 0xcc.
// Names are provisional. Requires ee/EEEffect3D.h.
#ifndef EE_EEE3DADDITIVEPASS_H
#define EE_EEE3DADDITIVEPASS_H

class EEGraphicObject;
class EEGraphicObject;
typedef EEGraphicObject EEModel;   // ee/EEGraphicObject.h (as ee/EEUnit.h)

class EEE3DAdditivePass : public EEEffect3D {
public:
    EEE3DAdditivePass(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe8a09 (kind 0x27)
    EEE3DAdditivePass();                    // 0xe8a8d
    virtual ~EEE3DAdditivePass();           // +0x00 0xe8aaa (??_G 0xe8a71)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 0xe8bfb (the draw)
    virtual long DrawPass6();               // +0x18 (`return 0`: ICF-folded into 0xfca98)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe8ae0 (queue 3)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xe8b78
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xe8d9c
    virtual long RepairPointers();          // +0x3c 0xe8dcc
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xe8e01
    virtual long Step();                    // +0x4c 0xe8e15 (7 once the unit is dead)

    static const UString sData;             // "EEE3DAdditivePass-Data" (0x4e8a50; provisional name)

    GETransformation mDrawTransformation;   // +0x74 (Virtual24 copies mTransformation)
    unsigned long mFrame;                   // +0xb4 (the unit's model frame)
    unsigned long mModelIndex;              // +0xb8 (the unit's mFirstFrame2)
    float mDrawScale;                       // +0xbc (Virtual24 copies mScale)
    float mScale;                           // +0xc0 (the unit stats' mF98)
    EEGraphicObject* mGraphicObject;        // +0xc4
    EEModel* mModel;                        // +0xc8 (the unit's Virtual54())
};

#endif
