// C:\EEX\Empire Earth\Graphics Thread\EEE3DEarthquake.h (RECONSTRUCTION; file name inferred).
// The earthquake's crack: a strip model built along the ground from one point to another, growing at the effect's
// speed, held, then closing again (vtable 0x440678; RTTI EEE3DEarthquake > EEEffect3D; EEE3DEarthquake.cpp
// 0xec6e4..0xed606). sizeof 0x94 (EEEffectManager 0x1066ef: operator new(0x94)). Names are provisional.
// Requires ee/EEEffect3D.h.
#ifndef EE_EEE3DEARTHQUAKE_H
#define EE_EEE3DEARTHQUAKE_H

class EEGraphicObject;
class GEModel;

class EEE3DEarthquake : public EEEffect3D {
public:
    // 0xec727 (kind 0x1a, no unit; EEEffectManager::CreateEffectBetween 0x106707)
    EEE3DEarthquake(GE3DPoint& inFrom, GE3DPoint& inTo, EEGraphicEffect* inGraphicEffect);
    EEE3DEarthquake();                      // 0xec87b
    virtual ~EEE3DEarthquake();             // +0x00 0xec88f (??_G 0xec85f)
    virtual long DrawPass2();               // +0x08 0xed384 (the draw)
    virtual long DrawPass4();               // +0x10 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 (likewise)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xecf53 (moves the strip's front; queue 0)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xed365 (loads the graphic, builds the model)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xed424
    virtual long RepairPointers();          // +0x3c 0xed4d4 (EEEffect::RepairPointers, then builds the model)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xed4e9
    virtual long Step();                    // +0x4c 0xed56c (grow, hold, close; 7 once closed)

    long BuildModel();                      // 0xec8bc (5 vertices and 8 polygons per segment) (provisional name)

    static const UString sData;             // "EEE3DEarthquake-Data" (0x4e8b40; provisional name)

    GEModel* mModel;                        // +0x74 (gModelManager->CreateModel, referenced)
    EEGraphicObject* mGraphicObject;        // +0x78
    float mDistance;                        // +0x7c the front's distance from the start (saved)
    float mSpeed;                           // +0x80 mLength / the duration
    float mLength;                          // +0x84 (saved)
    unsigned long mSegmentCount;            // +0x88 the length rounded up (saved)
    float mHoldTime;                        // +0x8c (saved; 15000 once fully grown)
    bool mGrowing;                          // +0x90 (saved)
    bool mClosing;                          // +0x91 (saved)
};

#endif
