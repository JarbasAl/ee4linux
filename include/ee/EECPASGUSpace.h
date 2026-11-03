// C:\EEX\Empire Earth\AI Thread\EECPASGUSpace.h (RECONSTRUCTION; file name inferred). An attack sub group
// unit kind (RTTI EECPASGUSpace > EECPAttackSubGroupUnit; vtable 0x4399cc; mType 0xc). Requires
// ee/EECPAttackSubGroupUnit.h.
#ifndef EE_EECPASGUSPACE_H
#define EE_EECPASGUSPACE_H

class EECPASGUSpace : public EECPAttackSubGroupUnit {
public:
    EECPASGUSpace(EEComplexUnit* inUnit, EECPAttackSubGroup* inSubGroup);
    EECPASGUSpace(EECPAttackSubGroup* inSubGroup);
    virtual ~EECPASGUSpace();
    virtual void Update(unsigned long inTime);      // +0x6c 0xd294
};

#endif
