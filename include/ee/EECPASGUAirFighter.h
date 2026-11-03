// C:\EEX\Empire Earth\AI Thread\EECPASGUAirFighter.h (RECONSTRUCTION; file name inferred). An attack sub group
// unit kind (RTTI EECPASGUAirFighter > EECPAttackSubGroupUnit; vtable 0x439268; mType 3). Requires ee/EECPAttackSubGroupUnit.h.
#ifndef EE_EECPASGUAIRFIGHTER_H
#define EE_EECPASGUAIRFIGHTER_H

class EECPASGUAirFighter : public EECPAttackSubGroupUnit {
public:
    EECPASGUAirFighter(EEComplexUnit* inUnit, EECPAttackSubGroup* inSubGroup);
    EECPASGUAirFighter(EECPAttackSubGroup* inSubGroup);
    virtual ~EECPASGUAirFighter();
    // +0x34 0x631c: only the base's (the kept copy of an ICF group: EECPASGUAirBomber's slot)
    virtual void ApplyPrejudices(EECPASTargetUnit* inTarget);
    virtual void Update(unsigned long inTime);      // +0x6c 0x61fd
    // the original vtable 0x439268 overrides these like EECPASGUAirBomber: ICF-shared bodies
    virtual void Flee();                    // +0x1c 0xe123d (`ret`, ICF-shared)
    virtual bool IsTargetValid();           // +0x58 0x60b1 (`mov al, 1`, ICF: EECPASGUAirBomber::IsTargetValid)
};

#endif
