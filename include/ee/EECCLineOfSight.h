// C:\EEX\Empire Earth\Calamities\EECCLineOfSight.h (RECONSTRUCTION; file name inferred). The line-of-sight
// Layout proven: EECCLineOfSight sizeof 0x2c = its new-site size, ctor 0xb1f4f exact in EECCLineOfSight.cpp. Member
// names are provisional.
// calamity class: raises the caster's sight while it lasts (vtable 0x43cacc). EECCLineOfSight.cpp is
// 0xb1f4f..0xb2034. sizeof 0x2c. Requires ee/EEGlobalCalamityClass.h.
#ifndef EE_EECCLINEOFSIGHT_H
#define EE_EECCLINEOFSIGHT_H

class EECCLineOfSight : public EEGlobalCalamityClass {
public:
    EECCLineOfSight();                      // 0xb1f4f
    virtual ~EECCLineOfSight();             // 0xb1f7d
    virtual bool Start(EECalamity* inCalamity);                  // +0x04 0xb1f88
    virtual void Stop(EECalamity* inCalamity);                   // +0x08 0xb1fc4
    virtual void Initialize(EECalamityData* inData);             // +0x20 0xb2000
    virtual void Reset(EECalamity* inCalamity);                  // +0x24 (empty: ICF-folded 0x157269)
    virtual long Update(EECalamity* inCalamity, float inElapsedTime);   // +0x38 0xb2019
    // +0x3c 0xb202f: 0 (the cloaking class's slot is folded into this copy)
    virtual long CanCastAt(EEComplexUnit* inUnit, GE3DPoint& inLocation, unsigned long inX, unsigned long inY,
                           bool inCheck, EEUnit* inTargetUnit);

protected:
    float mSightBonus;                      // +0x28 (record +0x64; added to the caster's +0x250) (provisional name)
};

#endif
