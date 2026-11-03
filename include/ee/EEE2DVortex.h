// C:\EEX\Empire Earth\Graphics Thread\EEE2DVortex.h (RECONSTRUCTION; file name inferred). A
// spinning model with orbiting sub-models at a point that follows its unit (RTTI EEE2DVortex > EEEffect2D; vtable
// 0x4401b4; EEE2DVortex.cpp 0xe773c..0xe7d67; sizeof 0xb4). Queued on pass 3. Names are provisional; the members the
// code does not touch are plain longs. Requires ee/EEEffect2D.h, ee/EEGraphicObject.h (EEOrbital).
#ifndef EE_EEE2DVORTEX_H
#define EE_EEE2DVORTEX_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;
class EEOrbital;

class EEE2DVortex : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DVortex-Data" (chain 0xe773c)

    EEE2DVortex(const GE3DPoint& inPosition, EEGraphicEffect* inGraphicEffect);   // 0xe777f (kind 0x31, gU4E8DF4)
    EEE2DVortex();                          // 0xe77ff (the load-time ctor)
    virtual ~EEE2DVortex();                 // +0x00 0xe784d (??_G 0xe77e3)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 0xe7949 (draws the model and its orbitals)
    virtual long DrawPass6();               // +0x18 (0xfca98)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe787e
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe790f
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe7b50
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe7c1c
    virtual long Step();                    // +0x4c 0xe7c57

    std::vector<EEOrbital*> mOrbitals;      // +0x54 (copies of the graphic's, made by Load)
    std::vector<float> mAngles;             // +0x64 (the orbitals' angles the draw uses)
    GE3DPoint mDrawPosition;                // +0x74 (Virtual24's copy of mPosition)
    float mSpin;                            // +0x80 0..1 (saved)
    float mDrawSpin;                        // +0x84 (Virtual24's copy of mSpin)
    long mL88;                              // +0x88
    float mAge;                             // +0x8c 0..1 (saved)
    float mDrawAge;                         // +0x90 (Virtual24's copy of mAge)
    long mL94;                              // +0x94
    long mL98;                              // +0x98
    unsigned long mFrame;                   // +0x9c
    unsigned long mDrawFrame;               // +0xa0 (Virtual24's copy of mFrame)
    long mOrbitalCount;                     // +0xa4
    long mLA8;                              // +0xa8
    EEGraphicObject* mGraphicObject;        // +0xac (UnloadMedia in the dtor)
    long mLB0;                              // +0xb0
};

#endif
