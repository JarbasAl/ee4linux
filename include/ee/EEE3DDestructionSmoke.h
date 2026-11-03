// C:\EEX\Empire Earth\Graphics Thread\EEE3DDestructionSmoke.h (RECONSTRUCTION; file name inferred). Smoke
// lines thrown out around a destroyed unit (vtable 0x4405b8; RTTI EEE3DDestructionSmoke >
// EEE3DSmokeGenerator; EEE3DDestructionSmoke.cpp 0xebbba..0xebde9). Names are provisional.
// Requires ee/EEE3DSmokeGenerator.h.
#ifndef EE_EEE3DDESTRUCTIONSMOKE_H
#define EE_EEE3DDESTRUCTIONSMOKE_H

class EEE3DDestructionSmoke : public EEE3DSmokeGenerator {
public:
    EEE3DDestructionSmoke(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xebbfd (kind 0x1e)
    // inline (EEEffectManager::Load 0x106a4d: EEE3DSmokeGenerator(0x1e), then this class's vptr)
    EEE3DDestructionSmoke() : EEE3DSmokeGenerator(0x1e) {}
    // +0x30 0xebc56: the lines (mLinkedEffect1), their number from the radius; no return statement (eax is the last
    // CreateLineEffect's)
    virtual long SetUp();
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xebd9d
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xebdc3
    // +0x58: empty (the original vtable holds an empty 1-argument body, ICF-folded into 0x157269; not the generator's
    // puff)
    virtual void EmitPuff(GE3DPoint& inLocation);

    static const UString sData;             // "EEE3DDestructionSmoke" (0x4e8b10; provisional name)

    float mRadius;                          // +0x7c half the unit class's +0x130 (saved)
};

#endif
