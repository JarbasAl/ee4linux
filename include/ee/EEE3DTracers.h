// C:\EEX\Empire Earth\Graphics Thread\EEE3DTracers.h (RECONSTRUCTION; file name inferred). A
// tracer model flying along a line from one unit's tag point to another's, in the shooter's player colour or textured
// (vtable 0x440fcc; RTTI EEE3DTracers > EEEffect3D; EEE3DTracers.cpp 0xfd330..0xfdbab). Its Virtual30 is
// EEE3DPulseCannonBall's (ICF 0xf6e5e). EEE3DShells derives from it (vtable 0x441238: only Virtual14 0xf90e9 of its
// own; its ctor is inline, its ??_G 0x106016 is emitted by EEEffectManager.cpp). Names are provisional.
// Requires ee/EEEffect3D.h, lle/GE3DLine.h, lle/GEColor.h.
#ifndef EE_EEE3DTRACERS_H
#define EE_EEE3DTRACERS_H

class EEGraphicObject;

class EEE3DTracers : public EEEffect3D {
public:
    // 0xfd373 (from inFrom's weapon tag point to inTo's model point; EEModel request 1)
    EEE3DTracers(EEUnit* inFrom, EEUnit* inTo, EEGraphicEffect* inGraphicEffect, long inKind);
    EEE3DTracers(long inKind);              // 0xfd75e

    static const UString sData;             // "EEE3DTracers-Data" (0x4e8d18; provisional name)
    virtual ~EEE3DTracers();                // +0x00 0xfd7ab (??_G 0xfd742)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xfd850 (the draw)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xfd7d2 (queue 4, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 (ICF-folded into EEE3DPulseCannonBall's 0xf6e5e)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xfda2e
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xfda8b
    virtual long Step();                    // +0x4c 0xfdad4 (7 at the end: the impact effect, the target's hit)

    GE3DLine mPath;                         // +0x74 (saved)
    GE3DPoint mLocation;                    // +0x8c (the position the draw uses)
    EEGraphicObject* mGraphicObject;        // +0x98
    GEColor mColor;                         // +0x9c (the shooter's player colour)
    float mPosition;                        // +0xa8 0..1 along mPath (saved)
    float mDrawPosition;                    // +0xac (Virtual24 copies mPosition)
    float mSpeed;                           // +0xb0 per millisecond (saved)
};

class EEE3DShells : public EEE3DTracers {
public:
    EEE3DShells(EEUnit* inFrom, EEUnit* inTo, EEGraphicEffect* inGraphicEffect, long inKind)
        : EEE3DTracers(inFrom, inTo, inGraphicEffect, inKind) {}
    // inline (EEEffectManager::Load 0x106e41: EEE3DTracers(0x26), then this class's vptr)
    EEE3DShells(long inKind) : EEE3DTracers(inKind) {}
    // user-declared empty (its ??_G 0x106016 stores this class's vptr, then ~EEE3DTracers)
    virtual ~EEE3DShells() {}
    virtual long DrawPass5();               // +0x14 0xf90e9 (EEE3DShells.cpp: the draw)
};

#endif
