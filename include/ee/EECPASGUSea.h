// C:\EEX\Empire Earth\AI Thread\EECPASGUSea.h (RECONSTRUCTION; file name inferred). An attack sub group
// unit kind (RTTI EECPASGUSea > EECPAttackSubGroupUnit; vtable 0x439948; mType 5). Requires ee/EECPAttackSubGroupUnit.h.
#ifndef EE_EECPASGUSEA_H
#define EE_EECPASGUSEA_H

class EECPASGUSea : public EECPAttackSubGroupUnit {
public:
    EECPASGUSea(EEComplexUnit* inUnit, EECPAttackSubGroup* inSubGroup);
    EECPASGUSea(EECPAttackSubGroup* inSubGroup);
    virtual ~EECPASGUSea();
    // +0x6c 0xd21f: only the base's (the kept copy of an ICF group: EECPASGUMechPriest's and EECPASGUTransport's slots)
    virtual void Update(unsigned long inTime);
};

#endif
