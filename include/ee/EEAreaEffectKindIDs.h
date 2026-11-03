// ee/EEAreaEffectKindIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EEAREAEFFECTKINDIDS_H
#define EE_EEAREAEFFECTKINDIDS_H

// EEAreaEffectTemplate::mKind: EEAEManager::CreateAreaEffect creates the area-effect class by kind; names from the RTTI
// class (EEAE prefix dropped).
enum EEAreaEffectKind {
    kAreaEffectHealing = 1,                 // RTTI EEAEHealing: EEAEManager::CreateAreaEffect case 1
    kAreaEffectMorale = 2,                  // RTTI EEAEMorale: EEAEManager::CreateAreaEffect case 2
    kAreaEffectCalamityImmunity = 3,        // RTTI EEAECalamityImmunity: EEAEManager::CreateAreaEffect case 3
    kAreaEffectConversionImmunity = 4,      // RTTI EEAEConversionImmunity: EEAEManager::CreateAreaEffect case 4
    kAreaEffectMoraleFromBuildings = 5,     // RTTI EEAEMoraleFromBuildings: EEAEManager::CreateAreaEffect case 5
    kAreaEffectMoraleFromMilitaryHeroes = 6,  // RTTI EEAEMoraleFromMilitaryHeroes: EEAEManager::CreateAreaEffect case 6
    kAreaEffectEconomyFromUnits = 7,        // RTTI EEAEEconomyFromUnits: EEAEManager::CreateAreaEffect case 7
    kAreaEffectCloaking = 8,                // RTTI EEAECloaking: EEAEManager::CreateAreaEffect case 8
};

#endif // EE_EEAREAEFFECTKINDIDS_H
