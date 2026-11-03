// C:\EEX\Empire Earth\Graphics Thread\EEE2DLightGlow.h (RECONSTRUCTION; file name inferred).
// Glowing light sprites on a unit's light tag points, drawn while the unit is on a visible tile (RTTI EEE2DLightGlow >
// EEEffect2D; vtable 0x43fca8; EEE2DLightGlow.cpp 0xe1a37..0xe28be; kind 0xd; sizeof 0x6c). Names are provisional.
// Requires ee/EEEffect2D.h.
#ifndef EE_EEE2DLIGHTGLOW_H
#define EE_EEE2DLIGHTGLOW_H

#include "ee/EEEffect2D.h"

class EEGraphicObject;

// One glow (0x34 bytes: new[] with a cookie, delete[] through ??_E; ctor 0xe24cb out of line after Virtual30, its
// first user). Names are provisional.
struct EEE2DLightGlowSpot {
    EEE2DLightGlowSpot();

    GE3DPoint mPosition;                    // +0x00
    GE3DPoint mDrawPosition;                // +0x0c (Virtual24's copy of mPosition)
    GEColor mColor;                         // +0x18 (1, 1, 1)
    unsigned long mOnTime;                  // +0x24 (Virtual54 0x4e2849: lit while (age - mStart) % mPeriod <= it)
    unsigned long mPeriod;                  // +0x28 (Virtual54 0x4e2846: div)
    unsigned long mStart;                   // +0x2c (Virtual54 0x4e282f: unsigned, compared with mAge)
    bool mVisible;                          // +0x30
    bool mDrawVisible;                      // +0x31 (Virtual24's copy of mVisible; Virtual18 draws the visible ones)
};

class EEE2DLightGlow : public EEEffect2D {
public:
    static const UString sData;             // "EEE2DLightGlow-Data" (chain 0xe1a37)
    static GEModel* sModel;                 // 0x4e8890 (CreateModel; ReleaseModel at shutdown)

    EEE2DLightGlow(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect, bool inKeepsUnit);  // 0xe1a7a (kind 0xd)
    EEE2DLightGlow();                       // 0xe1ad1 (the load-time ctor; builds sModel once)
    virtual ~EEE2DLightGlow();              // +0x00 0xe1b05 (??_G 0xe1ab5)
    void CreateModel();                     // 0xe1b35 (an 8-vertex star model)
    virtual long DrawPass2();               // +0x08 (0xfca98)
    virtual long DrawPass4();               // +0x10 (0xfca98)
    virtual long DrawPass5();               // +0x14 (0xfca98)
    virtual long DrawPass6();               // +0x18 0xe2512 (a camera-facing quad per visible glow)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe1f89
    virtual void PrepareQueue();            // +0x2c (empty: 0xe123d)
    virtual long SetUp();                   // +0x30 0xe2067 (the glows from the unit's light tag points)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x38 0xe257b
    virtual long RepairPointers();          // +0x3c 0xe25a1 (EEEffect's, then Virtual30)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);  // +0x40 0xe25b6
    virtual long Step();                    // +0x4c 0xe25dc (7 once the unit is dead; hidden off the visible tiles)
    virtual long PlaceGlows();              // +0x54 0xe266f (moves the glows with the unit; 7 once the unit is dead: eax 0x4e28b7)
    static void ReleaseModel(bool inShutdown);  // 0xe24fa (URegisterShutdown)
    long Hidden();                          // (ICF-folded into 0x228735: 9)

    EEE2DLightGlowSpot* mSpots;             // +0x54 (new[])
    EEGraphicObject* mGraphicObject;        // +0x58 (UnloadMedia in the dtor)
    float mDrawAge;                         // +0x5c (Virtual24: mAge / the effect's mL70, at most 1)
    float mAge;                             // +0x60 (saved)
    unsigned long mSpotCount;               // +0x64
    bool mHidden;                           // +0x68 (Virtual4C: the unit is off the visible tiles)
};

#endif
