// EEAECloaking.h (RECONSTRUCTION; file name inferred). The cloaking area effect (RTTI EEAECloaking > EEAreaEffect;
// vtable 0x44ba94; EEAECloaking.cpp 0x27aa0b..0x27ac2c; EEAEManager::CreateAreaEffect makes it for template kind 8).
// Names are provisional. Requires ee/EEAreaEffect.h.
#ifndef EE_EEAECLOAKING_H
#define EE_EEAECLOAKING_H

class EEAECloaking : public EEAreaEffect {
public:
    EEAECloaking(EEAreaEffectTemplate* inTemplate);   // 0x27aa0b
    virtual ~EEAECloaking();                // +0x00 0x27aa46 (??_G 0x27aa2a)
    virtual void AppliedTo(EEComplexUnit* inUnit);   // +0x04 0x27aa51: the unit's SetCloakableChain(true)
    // +0x08 0x27aa5f: the template's test with the owner's tech-tree node for the unit's class not of kind 4
    virtual bool Covers(unsigned long inPlayer, EEComplexUnit* inUnit, const GE2DPoint& inPoint);
    virtual long GetStrength();             // +0x18 0x27aa23: mTemplate->mL3C
    virtual bool Modify(long inKind, long inValue, long& outValue);      // +0x30 0x27ab3a: kind 3 -> Virtual18()
    virtual void RemovedFrom(EEComplexUnit* inUnit);  // +0x38 0x27ab53: uncloaks the unit (civ power 0x31)
};

#endif
