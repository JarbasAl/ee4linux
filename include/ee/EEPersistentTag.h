// ee/EEPersistentTag.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EEPERSISTENTTAG_H
#define EE_EEPERSISTENTTAG_H

// FSPersistent::mTagID of the saved-game chunks (EEGameStateLoader::RestorePersistents switches on it; each
// FSPersistent subclass passes its tag): names from the class (ctor, dynamic_cast); setrilo game_load.h SavedChunkType
// agrees.
enum EEPersistentTag {
    kPersistentChecksum = 1,    // RTTI EEPChecksum: tag 1
    kPersistentSeed = 2,        // EEPTGeneric<unsigned long> (setrilo kChunkSeed): tag 2
    kPersistentCamera = 3,      // EEPTRestorable<GE3DPoint> (setrilo kChunkCamera): tag 3
    kPersistentMap = 4,         // EEPTPRestorable<EEMap>: tag 4
    kPersistentPlayers = 5,     // RTTI EEPPlayerHelper: tag 5
    kPersistentScenario = 6,    // EEPTPRestorable<EEScenarioManager>: tag 6
    kPersistentWorld = 7,       // RTTI EEPWorldHelper: tag 7
    kPersistentMultiplayer = 8,  // RTTI EEPMultiplayerHelper (restored as nothing): tag 8
    kPersistentEffects = 10,    // EEPTPRestorable<EEEffectManager>: tag 10
    kPersistentCalamities = 11,  // RTTI EEPCalamityHelper: tag 11
    kPersistentEnvironment = 12,  // EEPTPRestorable<EEEnvironmentManager>: tag 12
};

#endif // EE_EEPERSISTENTTAG_H
