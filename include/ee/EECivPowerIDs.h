// ee/EECivPowerIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EECIVPOWERIDS_H
#define EE_EECIVPOWERIDS_H

// Civilization power ids (EECivPower::mID, dbcivpowers.dat record +0x68; EECivPowersManager::GetPower 0x6831d0 /
// GetPowerRecord index the records by id - 0x26; HasPower tests the civilization attribute index); names from the
// record names (+0x00).
enum EECivPowerID {
    kCivPowerFlamingArrows = 0x26,          // dbcivpowers.dat record 0 id (+0x68) 38 "Flaming Arrows"
    kCivPowerSlavery = 0x27,                // dbcivpowers.dat record 1 id (+0x68) 39 "Slavery"
    kCivPowerPriestTower = 0x28,            // dbcivpowers.dat record 2 id (+0x68) 40 "Priest Tower"
    kCivPowerCrusaders = 0x29,              // dbcivpowers.dat record 3 id (+0x68) 41 "Crusaders"
    kCivPowerFanaticism = 0x2a,             // dbcivpowers.dat record 4 id (+0x68) 42 "Fanaticism"
    kCivPowerEmissaries = 0x2b,             // dbcivpowers.dat record 5 id (+0x68) 43 "Emissaries"
    kCivPowerExploration = 0x2c,            // dbcivpowers.dat record 6 id (+0x68) 44 "Exploration"
    kCivPowerInsurance = 0x2d,              // dbcivpowers.dat record 7 id (+0x68) 45 "Insurance"
    kCivPowerMetallurgy = 0x2e,             // dbcivpowers.dat record 8 id (+0x68) 46 "Metallurgy"
    kCivPowerAdvancedMining = 0x2f,         // dbcivpowers.dat record 9 id (+0x68) 47 "Advanced Mining"
    kCivPowerPathfinding = 0x30,            // dbcivpowers.dat record 10 id (+0x68) 48 "Pathfinding"
    kCivPowerCamouflage = 0x31,             // dbcivpowers.dat record 11 id (+0x68) 49 "Camouflage"
    kCivPowerBundeswehr = 0x32,             // dbcivpowers.dat record 12 id (+0x68) 50 "Bundeswehr"
    kCivPowerSAS = 0x33,                    // dbcivpowers.dat record 13 id (+0x68) 51 "SAS"
    kCivPowerParatroopers = 0x34,           // dbcivpowers.dat record 14 id (+0x68) 52 "Paratroopers"
    kCivPowerMarket = 0x35,                 // dbcivpowers.dat record 15 id (+0x68) 53 "Market"
    kCivPowerCyberNinja = 0x36,             // dbcivpowers.dat record 16 id (+0x68) 54 "Cyber Ninja"
    kCivPowerMissileBase = 0x37,            // dbcivpowers.dat record 17 id (+0x68) 55 "Missile Base"
    kCivPowerCloaking = 0x38,               // dbcivpowers.dat record 18 id (+0x68) 56 "Cloaking"
    kCivPowerAdaptation = 0x39,             // dbcivpowers.dat record 19 id (+0x68) 57 "Adaptation"
    kCivPowerExpansionism = 0x3a,           // dbcivpowers.dat record 20 id (+0x68) 58 "Expansionism"
    kCivPowerConquistadors = 0x3b,          // dbcivpowers.dat record 21 id (+0x68) 59 "Conquistadors"
    kCivPowerJustInTimeManufacturing = 0x3c,  // dbcivpowers.dat record 22 id (+0x68) 60 "Just In Time Manufacturing"
};

#endif // EE_EECIVPOWERIDS_H
