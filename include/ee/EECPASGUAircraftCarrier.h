// C:\EEX\Empire Earth\AI Thread\EECPASGUAircraftCarrier.h (RECONSTRUCTION; file name inferred). An attack sub group
// unit kind (RTTI EECPASGUAircraftCarrier > EECPAttackSubGroupUnit; vtable 0x4391e4; mType 2). Requires ee/EECPAttackSubGroupUnit.h.
#ifndef EE_EECPASGUAIRCRAFTCARRIER_H
#define EE_EECPASGUAIRCRAFTCARRIER_H

class EECPASGUAircraftCarrier : public EECPAttackSubGroupUnit {
public:
    EECPASGUAircraftCarrier(EEComplexUnit* inUnit, EECPAttackSubGroup* inSubGroup);
    EECPASGUAircraftCarrier(EECPAttackSubGroup* inSubGroup);
    virtual ~EECPASGUAircraftCarrier();
    virtual bool Retarget() { return false; }    // +0x14 (ICF-folded into 0x27c0b4)
};

#endif
