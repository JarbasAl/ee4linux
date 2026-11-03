// ee/EEImprovementIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EEIMPROVEMENTIDS_H
#define EE_EEIMPROVEMENTIDS_H

// Unit-class improvement kinds (the EEUnitClassImprovement switches, EEUIBUpgrade::mAttribute): names from
// EEUnitClassImprovement::GetName (Language.rc strings); the armour kinds as setrilo unit_class_upgrade.h UpgradeKind.
enum EEImprovementKind {
    kImprovementAreaEffect = 0,       // EEUnitClassImprovement::GetName case 0: IDS_AREA_EFFECT
    kImprovementAttack = 1,           // EEUnitClassImprovement::GetName case 1: IDS_IMPROVEMENT_ATTACK
    kImprovementArrowArmor = 2,       // EEUnitClassImprovement::GetName case 2: IDS_ARROW
    kImprovementGunArmor = 3,         // EEUnitClassImprovement::GetName case 3: IDS_GUN
    kImprovementLaserArmor = 4,       // EEUnitClassImprovement::GetName case 4: IDS_LASER
    kImprovementMissileArmor = 5,     // EEUnitClassImprovement::GetName case 5: IDS_MISSILE
    kImprovementPierceArmor = 6,      // EEUnitClassImprovement::GetName case 6: IDS_PIERCE
    kImprovementShockArmor = 7,       // EEUnitClassImprovement::GetName case 7: IDS_SHOCK
    kImprovementFlightTimeOrPower = 8,  // EEUnitClassImprovement::GetName case 8: IDS_IMPROVEMENT_FLIGHT_TIME or IDS_IMPROVEMENT_POWER by mFlightTime
    kImprovementHitPoints = 9,        // EEUnitClassImprovement::GetName case 9: IDS_IMPROVEMENT_HIT_POINTS
    kImprovementLeadership = 10,      // EEUnitClassImprovement::GetName case 10: IDS_LEADERSHIP
    kImprovementRange = 11,           // EEUnitClassImprovement::GetName case 11: IDS_IMPROVEMENT_RANGE
    kImprovementRepair = 12,          // EEUnitClassImprovement::GetName case 12: IDS_IMPROVEMENT_REPAIR
    kImprovementSpeed = 14,           // EEUnitClassImprovement::GetName case 14: IDS_IMPROVEMENT_SPEED
    kImprovementTransport = 15,       // EEUnitClassImprovement::GetName case 15: IDS_TRANSPORT
};

#endif // EE_EEIMPROVEMENTIDS_H
