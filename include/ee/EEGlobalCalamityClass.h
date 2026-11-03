// C:\EEX\Empire Earth\Calamities\EEGlobalCalamityClass.h (RECONSTRUCTION; file name inferred). The calamity
// classes that are not cast at a location (RTTI EECCCloaking > EEGlobalCalamityClass > EECalamityClass). No vtable
// of its own in the image; its members are trivial and ICF-folded (0x236201, 0xb202f). Requires
// ee/EECalamityClass.h, ee/EECalamity.h (CreateCalamity's new).
#ifndef EE_EEGLOBALCALAMITYCLASS_H
#define EE_EEGLOBALCALAMITYCLASS_H

class EEGlobalCalamityClass : public EECalamityClass {
public:
    // 0xae578 (after its only caller, EECalamityManager::CreateGlobalCalamity): a new calamity of this class cast by
    // inCasterUnit, Reset (provisional name)
    EECalamity* CreateCalamity(EEUnit* inCasterUnit)
    {
        EECalamity* theCalamity = new EECalamity(this, inCasterUnit);
        this->Reset(theCalamity);
        return theCalamity;
    }
    virtual long GetTargetKind();           // +0x14 (ICF-folded 0x236201: 1)
    // +0x3c (ICF-folded 0xb202f: 0)
    virtual long CanCastAt(EEComplexUnit* inUnit, GE3DPoint& inLocation, unsigned long inX, unsigned long inY,
                           bool inCheck, EEUnit* inTargetUnit);
};

#endif
