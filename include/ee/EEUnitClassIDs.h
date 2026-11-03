// ee/EEUnitClassIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EEUNITCLASSIDS_H
#define EE_EEUNITCLASSIDS_H

// The AI's unit kinds (dbobjects record +0x224, EEUnitClass::mL1D0 / mAIKind; counter kinds mL1D4..mL1DC). Provisional
// names from the dbobjects members of each kind.
enum EEUnitAIKind {
    kAIKindNone = 0,                      // 386 dbobjects records (resources, projectiles, ambients); the AI tests `!= 0` for 'has a kind' (provisional name)
    kAIKindAntiTank = 1,                  // data: AT guns, Bazooka Infantry, Grenade Launcher (provisional name)
    kAIKindSpecial = 3,                   // data: Priest, Prophet, Citizen, Female Citizen, Diplomat, Emissary, Senator, Flame Thrower, Trident/Triton submarines; EECPASGUPCTargetIsUnitSetScreenRangedOrSpecialUnits (provisional name)
    kAIKindFieldWeapon = 0xa,             // data: Stone Thrower, Scorpion, Balistae, Culverin, Bronze Cannon, Catapult ME (provisional name)
    kAIKindFighter = 0xc,                 // data: the fighters and carrier fighters (provisional name)
    kAIKindAntiAir = 0xd,                 // data: Stinger Soldier, AA guns, AA tanks, missile towers, Anti Missile Battery (provisional name)
    kAIKindBomber = 0xe,                  // data: the bombers, AT fighters, Transport C-47 (provisional name)
    kAIKindAircraftCarrier = 0xf,         // data: Enterprise, Nexus, Japanese Flattop carriers (provisional name)
    kAIKindArtillery = 0x10,              // data: Bombard, Artillery, Colossus (provisional name)
    kAIKindNavalTransport = 0x11,         // data: the naval transports, fishing boats, Naval Mine (provisional name)
    kAIKindSiege = 0x14,                  // data: rams, siege towers, catapults, trebuchets, siege cannons (provisional name)
    kAIKindSubmarine = 0x15,              // data: Nautilus, Hammerhead, U Boat (provisional name)
    kAIKindArcher = 0x18,                 // data: bowmen, slingers, cavalry/chariot/elephant archers (provisional name)
    kAIKindFortification = 0x19,          // data: walls, gates, towers, barbed wire, tank obstacles (provisional name)
    kAIKindBuilding = 0x1a,               // data: the buildings and wonders (85 records) (provisional name)
    kAIKindHero = 0x1b,                   // data: the heroes (50 records); EECPUGatherHeroesCount counts it (provisional name)
    kAIKindAntiSubmarineHelicopter = 0x1c,  // data: Sea King, Sea King II (provisional name)
    kAIKindMechZeus = 0x1e,               // data: Mech Zeus, Mech Zeus (prototype) (provisional name)
    kAIKindSpaceCarrier = 0x27,           // data: Sp15 Space Carrier (provisional name)
};

// Unit class categories (dbobjects record +0x120, EEUnitClass::mCategory): EEUnitClass::CreateUnitClasses 0x6358b1 builds one EEUC* subclass per category.
enum EEUnitCategory {
    kUnitCategoryAmbient = 0,               // CreateUnitClasses 0x6358b1: category 0 -> new EEUCAmbient (RTTI); setrilo UnitClassCode
    kUnitCategoryResource = 1,              // CreateUnitClasses 0x6358b1: category 1 -> new EEUCResource (RTTI); setrilo UnitClassCode
    kUnitCategoryBuilding = 2,              // CreateUnitClasses 0x6358b1: category 2 -> new EEUCBuilding (RTTI); setrilo UnitClassCode
    kUnitCategoryAirplane = 3,              // CreateUnitClasses 0x6358b1: category 3 -> new EEUCAirplane (RTTI); setrilo UnitClassCode
    kUnitCategoryNaval = 4,                 // CreateUnitClasses 0x6358b1: category 4 -> new EEUCNaval (RTTI); setrilo UnitClassCode
    kUnitCategoryLand = 5,                  // CreateUnitClasses 0x6358b1: category 5 -> new EEUCLand (RTTI); setrilo UnitClassCode
    kUnitCategoryLandCitizen = 6,           // CreateUnitClasses 0x6358b1: category 6 -> new EEUCLandCitizen (RTTI); setrilo UnitClassCode
    kUnitCategoryHelicopter = 7,            // CreateUnitClasses 0x6358b1: category 7 -> new EEUCHelicopter (RTTI); setrilo UnitClassCode
    kUnitCategoryLand8 = 8,                 // setrilo unit_class.h UnitClassCode kLand8; CreateUnitClasses 0x6358b1 builds EEUCLand for it; no dbobjects record has it
    kUnitCategoryProjectile = 9,            // CreateUnitClasses 0x6358b1: category 9 -> new EEUCProjectile (RTTI); setrilo UnitClassCode
    kUnitCategorySubmergedNaval = 0xa,      // CreateUnitClasses 0x6358b1: category 10 -> new EEUCSubmergedNaval (RTTI); setrilo UnitClassCode
    kUnitCategoryBomb = 0xb,                // CreateUnitClasses 0x6358b1: category 11 -> new EEUCBomb (RTTI); setrilo UnitClassCode
    kUnitCategoryMissile = 0xc,             // CreateUnitClasses 0x6358b1: category 12 -> new EEUCMissile (RTTI); setrilo UnitClassCode
    kUnitCategoryLandPriest = 0xd,          // CreateUnitClasses 0x6358b1: category 13 -> new EEUCLandPriest (RTTI); setrilo UnitClassCode
    kUnitCategoryNavalPriest = 0xe,         // CreateUnitClasses 0x6358b1: category 14 -> new EEUCNavalPriest (RTTI); setrilo UnitClassCode
    kUnitCategoryLand15 = 0xf,              // setrilo unit_class.h UnitClassCode kLand15; CreateUnitClasses 0x6358b1 builds EEUCLand for it; no dbobjects record has it
    kUnitCategoryLandTransport = 0x10,      // CreateUnitClasses 0x6358b1: category 16 -> new EEUCLandTransport (RTTI); setrilo UnitClassCode
    kUnitCategoryHelicopterTransport = 0x11,  // CreateUnitClasses 0x6358b1: category 17 -> new EEUCHelicopterTransport (RTTI); setrilo UnitClassCode
    kUnitCategoryNavalTransport = 0x12,     // CreateUnitClasses 0x6358b1: category 18 -> new EEUCNavalTransport (RTTI); setrilo UnitClassCode
    kUnitCategoryBuilding19 = 0x13,         // setrilo unit_class.h UnitClassCode kBuilding19; CreateUnitClasses 0x6358b1 builds EEUCBuilding for it; no dbobjects record has it
    kUnitCategoryTorpedo = 0x14,            // CreateUnitClasses 0x6358b1: category 20 -> new EEUCTorpedo (RTTI); setrilo UnitClassCode
    kUnitCategoryNavalBuilding = 0x15,      // CreateUnitClasses 0x6358b1: category 21 -> new EEUCNavalBuilding (RTTI); setrilo UnitClassCode
    kUnitCategoryNavalCitizen = 0x16,       // CreateUnitClasses 0x6358b1: category 22 -> new EEUCNavalCitizen (RTTI); setrilo UnitClassCode
    kUnitCategoryLandMilitaryHero = 0x17,   // CreateUnitClasses 0x6358b1: category 23 -> new EEUCLandMilitaryHero (RTTI); setrilo UnitClassCode
    kUnitCategoryMiningBuilding = 0x18,     // CreateUnitClasses 0x6358b1: category 24 -> new EEUCMiningBuilding (RTTI); setrilo UnitClassCode
    kUnitCategoryGate = 0x19,               // CreateUnitClasses 0x6358b1: category 25 -> new EEUCGate (RTTI); setrilo UnitClassCode
    kUnitCategoryHelicopterPriest = 0x1a,   // CreateUnitClasses 0x6358b1: category 26 -> new EEUCHelicopterPriest (RTTI); setrilo UnitClassCode
    kUnitCategorySeaAnimal = 0x1b,          // CreateUnitClasses 0x6358b1: category 27 -> new EEUCSeaAnimal (RTTI); setrilo UnitClassCode
    kUnitCategoryAirAnimal = 0x1c,          // CreateUnitClasses 0x6358b1: category 28 -> new EEUCAirAnimal (RTTI); setrilo UnitClassCode
    kUnitCategoryFortress = 0x1d,           // CreateUnitClasses 0x6358b1: category 29 -> new EEUCFortress (RTTI); setrilo UnitClassCode
    kUnitCategoryBuildingPriest = 0x1e,     // CreateUnitClasses 0x6358b1: category 30 -> new EEUCBuildingPriest (RTTI); setrilo UnitClassCode
    kUnitCategorySpaceship = 0x1f,          // CreateUnitClasses 0x6358b1: category 31 -> new EEUCSpaceship (RTTI); setrilo UnitClassCode
    kUnitCategorySpaceBuilding = 0x20,      // CreateUnitClasses 0x6358b1: category 32 -> new EEUCSpaceBuilding (RTTI); setrilo UnitClassCode
    kUnitCategorySpaceTransport = 0x21,     // CreateUnitClasses 0x6358b1: category 33 -> new EEUCSpaceTransport (RTTI); setrilo UnitClassCode
    kUnitCategoryPlanetSpaceship = 0x22,    // CreateUnitClasses 0x6358b1: category 34 -> new EEUCPlanetSpaceship (RTTI); setrilo UnitClassCode
};

// Unit families (dbobjects record +0x68, EEUnitClass::mL1C8 / mFamily): the row index of dbfamily.dat.
enum EEUnitFamily {
    kUnitFamilyNoFamily = 0,              // dbfamily.dat row 0 "No Family"
    kUnitFamilyTank = 2,                  // dbfamily.dat row 2 "Tank"
    kUnitFamilyBuilding = 3,              // dbfamily.dat row 3 "Building"
    kUnitFamilyAntiTank = 4,              // dbfamily.dat row 4 "Anti Tank"
    kUnitFamilyAircraft = 5,              // dbfamily.dat row 5 "Aircraft"
    kUnitFamilyPriest = 6,                // dbfamily.dat row 6 "Priest"
    kUnitFamilySubmarine = 7,             // dbfamily.dat row 7 "Submarine"
    kUnitFamilyHero = 8,                  // dbfamily.dat row 8 "Hero"
    kUnitFamilyShip = 9,                  // dbfamily.dat row 9 "Ship"
    kUnitFamilySpearThrower = 0xa,        // dbfamily.dat row 10 "Spear Thrower"
    kUnitFamilyMachines = 0xb,            // dbfamily.dat row 11 "Machines"
    kUnitFamilyHumanMusket = 0xc,         // dbfamily.dat row 12 "Human Musket"
    kUnitFamilyHumanSword = 0xd,          // dbfamily.dat row 13 "Human Sword"
    kUnitFamilyMountedArcher = 0xe,       // dbfamily.dat row 14 "Mounted Archer"
    kUnitFamilyLancer = 0xf,              // dbfamily.dat row 15 "Lancer"
    kUnitFamilyHelicopter = 0x10,         // dbfamily.dat row 16 "Helicopter"
    kUnitFamilyAnimal = 0x11,             // dbfamily.dat row 17 "Animal"
    kUnitFamilyFish = 0x12,               // dbfamily.dat row 18 "Fish"
    kUnitFamilyCuriassier = 0x13,         // dbfamily.dat row 19 "Curiassier"
    kUnitFamilyBomber = 0x14,             // dbfamily.dat row 20 "Bomber"
    kUnitFamilyBattleship = 0x15,         // dbfamily.dat row 21 "Battleship"
    kUnitFamilyMines = 0x16,              // dbfamily.dat row 22 "Mines"
    kUnitFamilyHuman = 0x17,              // dbfamily.dat row 23 "Human"
    kUnitFamilyMechMinotaur = 0x19,       // dbfamily.dat row 25 "Mech - Minotaur"
    kUnitFamilyMechMedium = 0x1a,         // dbfamily.dat row 26 "Mech - Medium"
    kUnitFamilyMechZeus = 0x1b,           // dbfamily.dat row 27 "Mech - Zeus"
    kUnitFamilyMechCyclops = 0x1c,        // dbfamily.dat row 28 "Mech - Cyclops"
    kUnitFamilyHumanArcher = 0x1d,        // dbfamily.dat row 29 "Human Archer"
    kUnitFamilyRam = 0x1e,                // dbfamily.dat row 30 "Ram"
    kUnitFamilySiege = 0x1f,              // dbfamily.dat row 31 "Siege"
    kUnitFamilyLandAA = 0x20,             // dbfamily.dat row 32 "Land AA"
    kUnitFamilyMechApollo = 0x21,         // dbfamily.dat row 33 "Mech - Apollo"
    kUnitFamilyTankStrong = 0x22,         // dbfamily.dat row 34 "Tank - Strong"
    kUnitFamilyAmbient = 0x23,            // dbfamily.dat row 35 "Ambient"
    kUnitFamilyCitizen = 0x24,            // dbfamily.dat row 36 "Citizen"
    kUnitFamilyTowers = 0x25,             // dbfamily.dat row 37 "Towers"
    kUnitFamilyHumanMachineGun = 0x26,    // dbfamily.dat row 38 "Human Machine Gun"
    kUnitFamilyMedievalFieldWeapon = 0x27,  // dbfamily.dat row 39 "Medieval Field Weapon"
    kUnitFamilyHumanSpear = 0x28,         // dbfamily.dat row 40 "Human Spear"
    kUnitFamilyMountedSpear = 0x29,       // dbfamily.dat row 41 "Mounted Spear"
    kUnitFamilyAtomicBomber = 0x2a,       // dbfamily.dat row 42 "Atomic Bomber"
    kUnitFamilyMechUnderwater = 0x2b,     // dbfamily.dat row 43 "Mech - Underwater"
    kUnitFamilyWalls = 0x2c,              // dbfamily.dat row 44 "Walls"
    kUnitFamilyViking = 0x2d,             // dbfamily.dat row 45 "Viking"
    kUnitFamilyAABuilding = 0x2e,         // dbfamily.dat row 46 "AA Building"
    kUnitFamilyShipGalley = 0x2f,         // dbfamily.dat row 47 "Ship Galley"
    kUnitFamilyWarElephant = 0x30,        // dbfamily.dat row 48 "War Elephant"
    kUnitFamilyElephantArcher = 0x31,     // dbfamily.dat row 49 "Elephant Archer"
    kUnitFamilyAircraftCarrier = 0x32,    // dbfamily.dat row 50 "Aircraft Carrier"
    kUnitFamilyEliteGuard = 0x34,         // dbfamily.dat row 52 "Elite Guard"
    kUnitFamilyArtillery = 0x35,          // dbfamily.dat row 53 "Artillery"
    kUnitFamilyCavalryArcher = 0x36,      // dbfamily.dat row 54 "Cavalry Archer"
    kUnitFamilyICBM = 0x37,               // dbfamily.dat row 55 "ICBM"
    kUnitFamilySpaceship = 0x38,          // dbfamily.dat row 56 "Spaceship"
    kUnitFamilySpaceFighter = 0x39,       // dbfamily.dat row 57 "Space Fighter"
    kUnitFamilySpaceCorvette = 0x3a,      // dbfamily.dat row 58 "Space Corvette"
    kUnitFamilyTowerSpaceTurret = 0x3b,   // dbfamily.dat row 59 "Tower - Space Turret"
};

// Unit types (dbobjects record +0x74, EEUnitClass::mUnitType, the gUnitClassTable index); only the ids the code uses, named from their dbobjects record.
enum EEUnitTypeID {
    kUnitTypePriest = 1,                             // dbobjects.dat unit type 1 (record +0x74) "Priest"
    kUnitTypeArch03SimpleBowman = 0xa,               // dbobjects.dat unit type 10 (record +0x74) "Arch03 - Simple Bowman"
    kUnitTypeArch06CavalryArcher = 0xb,              // dbobjects.dat unit type 11 (record +0x74) "Arch06 - Cavalry Archer"
    kUnitTypeArch04ChariotArcher = 0xc,              // dbobjects.dat unit type 12 (record +0x74) "Arch04 - Chariot Archer"
    kUnitTypeGunSpear04StoneThrower = 0xe,           // dbobjects.dat unit type 14 (record +0x74) "Gun Spear04 Stone Thrower"
    kUnitTypeArch05CompositeBow = 0xf,               // dbobjects.dat unit type 15 (record +0x74) "Arch05 - Composite Bow"
    kUnitTypeArch05CrossBow = 0x10,                  // dbobjects.dat unit type 16 (record +0x74) "Arch05 - Cross Bow"
    kUnitTypeArch06LongBow = 0x11,                   // dbobjects.dat unit type 17 (record +0x74) "Arch06 - Long Bow"
    kUnitTypeInf04ShortSword = 0x16,                 // dbobjects.dat unit type 22 (record +0x74) "Inf04 - Short Sword"
    kUnitTypeInf06LongSword = 0x17,                  // dbobjects.dat unit type 23 (record +0x74) "Inf06 - LongSword"
    kUnitTypeInf04Phalanx = 0x19,                    // dbobjects.dat unit type 25 (record +0x74) "Inf04 - Phalanx"
    kUnitTypeCitizen = 0x20,                         // dbobjects.dat unit type 32 (record +0x74) "Citizen"
    kUnitTypeBarracks = 0x22,                        // dbobjects.dat unit type 34 (record +0x74) "b  Barracks"
    kUnitTypeTownCenter = 0x23,                      // dbobjects.dat unit type 35 (record +0x74) "b  Town Center"
    kUnitTypeDock = 0x24,                            // dbobjects.dat unit type 36 (record +0x74) "b  Dock"
    kUnitTypeHouse = 0x25,                           // dbobjects.dat unit type 37 (record +0x74) "b  House"
    kUnitTypeTemple = 0x27,                          // dbobjects.dat unit type 39 (record +0x74) "b  Temple"
    kUnitTypeCapitol = 0x29,                         // dbobjects.dat unit type 41 (record +0x74) "b  Capitol"
    kUnitTypeFarm = 0x2a,                            // dbobjects.dat unit type 42 (record +0x74) "b  Farm"
    kUnitTypeSettlement = 0x2b,                      // dbobjects.dat unit type 43 (record +0x74) "b  Settlement"
    kUnitTypeGuardTowerPaleo = 0x2f,                 // dbobjects.dat unit type 47 (record +0x74) "b  Guard Tower - Paleo"
    kUnitTypeGranary = 0x34,                         // dbobjects.dat unit type 52 (record +0x74) "b  Granary"
    kUnitTypeArcheryRange = 0x36,                    // dbobjects.dat unit type 54 (record +0x74) "b  Archery Range"
    kUnitTypeResForageBush = 0x37,                   // dbobjects.dat unit type 55 (record +0x74) "Res - Forage Bush"
    kUnitTypeLighthouseAtAlexandria = 0x3b,          // dbobjects.dat unit type 59 (record +0x74) "w  Lighthouse at Alexandria"
    kUnitTypeUniversity = 0x3c,                      // dbobjects.dat unit type 60 (record +0x74) "b  University"
    kUnitTypeHospital = 0x3e,                        // dbobjects.dat unit type 62 (record +0x74) "b  Hospital"
    kUnitTypeSiegeFactory = 0x3f,                    // dbobjects.dat unit type 63 (record +0x74) "b  Siege Factory"
    kUnitTypeStable = 0x40,                          // dbobjects.dat unit type 64 (record +0x74) "b  Stable"
    kUnitTypeNavyYard = 0x41,                        // dbobjects.dat unit type 65 (record +0x74) "b  Navy Yard"
    kUnitTypeAirport = 0x42,                         // dbobjects.dat unit type 66 (record +0x74) "b  Airport"
    kUnitTypeMissileBase = 0x43,                     // dbobjects.dat unit type 67 (record +0x74) "x Missile Base"
    kUnitTypeTankFactory = 0x44,                     // dbobjects.dat unit type 68 (record +0x74) "b  Tank Factory"
    kUnitTypeCyberFactory = 0x45,                    // dbobjects.dat unit type 69 (record +0x74) "b  Cyber Factory"
    kUnitTypeCarrierFighter12F14 = 0x50,             // dbobjects.dat unit type 80 (record +0x74) "a CarrierFighter12 F-14"
    kUnitTypeCarrierFighter11Corsair1 = 0x55,        // dbobjects.dat unit type 85 (record +0x74) "a CarrierFighter11 Corsair1"
    kUnitTypeBalloon10ObservationBalloon = 0x5a,     // dbobjects.dat unit type 90 (record +0x74) "a Balloon10 Observation Balloon"
    kUnitTypeInf05Pilum = 0x60,                      // dbobjects.dat unit type 96 (record +0x74) "Inf05 - Pilum"
    kUnitTypeSiege04Tower = 0x67,                    // dbobjects.dat unit type 103 (record +0x74) "Siege04 - Tower"
    kUnitTypeGunSiege06Trebuchet = 0x69,             // dbobjects.dat unit type 105 (record +0x74) "Gun Siege06 Trebuchet"
    kUnitTypeGunSiege09SerpentineCannon = 0x6b,      // dbobjects.dat unit type 107 (record +0x74) "Gun Siege09 Serpentine Cannon"
    kUnitTypeS02FishingBoatStone = 0x6c,             // dbobjects.dat unit type 108 (record +0x74) "s02 Fishing Boat Stone"
    kUnitTypeS04FishingBoatBronze = 0x6d,            // dbobjects.dat unit type 109 (record +0x74) "s04 Fishing Boat Bronze"
    kUnitTypeS11EnterpriseAircraftCarrier = 0x7e,    // dbobjects.dat unit type 126 (record +0x74) "s11 Enterprise Aircraft Carrier"
    kUnitTypeCav04BronzeSpearCavalry = 0x89,         // dbobjects.dat unit type 137 (record +0x74) "Cav04 - Bronze Spear Cavalry"
    kUnitTypeCav06Knight = 0x8b,                     // dbobjects.dat unit type 139 (record +0x74) "Cav06 - Knight"
    kUnitTypeGunCannon09BronzeCannon = 0x8e,         // dbobjects.dat unit type 142 (record +0x74) "Gun Cannon09 - Bronze Cannon"
    kUnitTypeGunSpear06Balistae = 0x90,              // dbobjects.dat unit type 144 (record +0x74) "Gun Spear06 Balistae"
    kUnitTypeAA10Gun88mm = 0x91,                     // dbobjects.dat unit type 145 (record +0x74) "b  AA10 Gun 88mm"
    kUnitTypeAA13MissileTower = 0x93,                // dbobjects.dat unit type 147 (record +0x74) "b  AA13 Missile Tower"
    kUnitTypeS10Cruiser = 0xa3,                      // dbobjects.dat unit type 163 (record +0x74) "s10 Cruiser"
    kUnitTypeH24HannibalMorale = 0xa5,               // dbobjects.dat unit type 165 (record +0x74) "h2-4 Hannibal (Morale)"
    kUnitTypeHurricane = 0xb6,                       // dbobjects.dat unit type 182 (record +0x74) "Hurricane"
    kUnitTypeMechZeusPrototype = 0xc2,               // dbobjects.dat unit type 194 (record +0x74) "x Mech Zeus (prototype)"
    kUnitTypeX04TrojanHorse = 0xd1,                  // dbobjects.dat unit type 209 (record +0x74) "x04 Trojan Horse"
    kUnitTypeS11FishingBoatModern = 0xd3,            // dbobjects.dat unit type 211 (record +0x74) "s11 Fishing Boat Modern"
    kUnitTypeParachuteBomb = 0xd5,                   // dbobjects.dat unit type 213 (record +0x74) "Parachute Bomb"
    kUnitTypeProphet = 0xd8,                         // dbobjects.dat unit type 216 (record +0x74) "Prophet"
    kUnitTypePathPoint = 0xdf,                       // dbobjects.dat unit type 223 (record +0x74) "PathPoint"
    kUnitTypeCav07RoyalCuirassierSword = 0xec,       // dbobjects.dat unit type 236 (record +0x74) "Cav07 - Royal Cuirassier (Sword)"
    kUnitTypeTree01Maple = 0xee,                     // dbobjects.dat unit type 238 (record +0x74) "Tree 01 [Maple]"
    kUnitTypeTree02Maple = 0xef,                     // dbobjects.dat unit type 239 (record +0x74) "Tree 02 [Maple]"
    kUnitTypeAirportRunway = 0xf0,                   // dbobjects.dat unit type 240 (record +0x74) "b  Airport Runway"
    kUnitTypeWaypoint = 0x104,                       // dbobjects.dat unit type 260 (record +0x74) "Waypoint"
    kUnitTypeResGiraffe = 0x124,                     // dbobjects.dat unit type 292 (record +0x74) "Res - Giraffe"
    kUnitTypeFoundation2x2 = 0x132,                  // dbobjects.dat unit type 306 (record +0x74) "Foundation 2x2"
    kUnitTypeFoundation3x3 = 0x133,                  // dbobjects.dat unit type 307 (record +0x74) "Foundation 3x3"
    kUnitTypeSiege06HeavyTower = 0x134,              // dbobjects.dat unit type 308 (record +0x74) "Siege06 - Heavy Tower"
    kUnitTypeS09RoyalGalleon = 0x135,                // dbobjects.dat unit type 309 (record +0x74) "s09 Royal Galleon"
    kUnitTypeFoundation1x1 = 0x136,                  // dbobjects.dat unit type 310 (record +0x74) "Foundation 1x1"
    kUnitTypeCav07CarabineersGun = 0x137,            // dbobjects.dat unit type 311 (record +0x74) "Cav07 - Carabineers (gun)"
    kUnitTypeCav09ImperialCuirassierSword = 0x138,   // dbobjects.dat unit type 312 (record +0x74) "Cav09 - Imperial Cuirassier Sword"
    kUnitTypeResShark = 0x13a,                       // dbobjects.dat unit type 314 (record +0x74) "Res - Shark"
    kUnitTypeResFish = 0x13b,                        // dbobjects.dat unit type 315 (record +0x74) "Res - Fish"
    kUnitTypeResGoat = 0x146,                        // dbobjects.dat unit type 326 (record +0x74) "Res - Goat"
    kUnitTypeResEagle = 0x147,                       // dbobjects.dat unit type 327 (record +0x74) "Res - Eagle"
    kUnitTypeTankObstacle = 0x148,                   // dbobjects.dat unit type 328 (record +0x74) "b  Tank Obstacle"
    kUnitTypeTree12Spruce = 0x151,                   // dbobjects.dat unit type 337 (record +0x74) "Tree 12 [Spruce]"
    kUnitTypeTree03Spruce = 0x152,                   // dbobjects.dat unit type 338 (record +0x74) "Tree 03 [Spruce]"
    kUnitTypeTree07Spruce = 0x153,                   // dbobjects.dat unit type 339 (record +0x74) "Tree 07 [Spruce]"
    kUnitTypeTree09Spruce = 0x154,                   // dbobjects.dat unit type 340 (record +0x74) "Tree 09 [Spruce]"
    kUnitTypeTree10Spruce = 0x155,                   // dbobjects.dat unit type 341 (record +0x74) "Tree 10 [Spruce]"
    kUnitTypeTree11Spruce = 0x156,                   // dbobjects.dat unit type 342 (record +0x74) "Tree 11 [Spruce]"
    kUnitTypeTree15Cactus = 0x157,                   // dbobjects.dat unit type 343 (record +0x74) "Tree 15 [Cactus]"
    kUnitTypeTree16Cactus = 0x158,                   // dbobjects.dat unit type 344 (record +0x74) "Tree 16 [Cactus]"
    kUnitTypeTree19 = 0x159,                         // dbobjects.dat unit type 345 (record +0x74) "Tree 19"
    kUnitTypeTree20 = 0x15a,                         // dbobjects.dat unit type 346 (record +0x74) "Tree 20"
    kUnitTypeTree18 = 0x15e,                         // dbobjects.dat unit type 350 (record +0x74) "Tree 18"
    kUnitTypeTree24 = 0x15f,                         // dbobjects.dat unit type 351 (record +0x74) "Tree 24"
    kUnitTypeTree25 = 0x160,                         // dbobjects.dat unit type 352 (record +0x74) "Tree 25"
    kUnitTypeTree21Palm = 0x161,                     // dbobjects.dat unit type 353 (record +0x74) "Tree 21 [Palm]"
    kUnitTypeCameraObject = 0x167,                   // dbobjects.dat unit type 359 (record +0x74) "x Camera Object"
    kUnitTypeFoundation6x6 = 0x168,                  // dbobjects.dat unit type 360 (record +0x74) "Foundation 6x6"
    kUnitTypeVolcano = 0x16e,                        // dbobjects.dat unit type 366 (record +0x74) "Volcano"
    kUnitTypeVolcanoProjectile = 0x16f,              // dbobjects.dat unit type 367 (record +0x74) "Volcano - Projectile"
    kUnitTypeInf08Halberdier = 0x170,                // dbobjects.dat unit type 368 (record +0x74) "Inf08 - Halberdier"
    kUnitTypeResHorse = 0x175,                       // dbobjects.dat unit type 373 (record +0x74) "Res - Horse"
    kUnitTypeS10NavalMine = 0x182,                   // dbobjects.dat unit type 386 (record +0x74) "s10 Naval Mine"
    kUnitTypeAmbStarfish3 = 0x1cc,                   // dbobjects.dat unit type 460 (record +0x74) "Amb - Starfish3"
    kUnitTypeGuardTowerCopper = 0x1cd,               // dbobjects.dat unit type 461 (record +0x74) "b  Guard Tower - Copper"
    kUnitTypeGuardTowerBronze = 0x1ce,               // dbobjects.dat unit type 462 (record +0x74) "b  Guard Tower - Bronze"
    kUnitTypeGuardTowerMiddle = 0x1cf,               // dbobjects.dat unit type 463 (record +0x74) "b  Guard Tower - Middle"
    kUnitTypeGuardTowerImperial = 0x1d0,             // dbobjects.dat unit type 464 (record +0x74) "b  Guard Tower - Imperial"
    kUnitTypeGuardTowerWW1 = 0x1d1,                  // dbobjects.dat unit type 465 (record +0x74) "b  Guard Tower - WW1"
    kUnitTypeGuardTowerDigital = 0x1d2,              // dbobjects.dat unit type 466 (record +0x74) "b  Guard Tower - Digital"
    kUnitTypeX11HauptmannDurerProphet = 0x1db,       // dbobjects.dat unit type 475 (record +0x74) "x11 Hauptmann Durer [Prophet]"
    kUnitTypeFlare2 = 0x1dd,                         // dbobjects.dat unit type 477 (record +0x74) "x Flare 2"
    kUnitTypeFemaleCitizen = 0x1e0,                  // dbobjects.dat unit type 480 (record +0x74) "Female Citizen"
    kUnitTypeBalloon09HotAirBalloon = 0x1ff,         // dbobjects.dat unit type 511 (record +0x74) "a Balloon09 Hot Air Balloon"
    kUnitTypeAmbCattails1 = 0x234,                   // dbobjects.dat unit type 564 (record +0x74) "Amb - Cattails 1"
    kUnitTypeCyberLaboratory = 0x24a,                // dbobjects.dat unit type 586 (record +0x74) "b  Cyber Laboratory"
    kUnitTypePillbox = 0x25e,                        // dbobjects.dat unit type 606 (record +0x74) "x Pillbox"
    kUnitTypeAntiMatterStorm = 0x294,                // dbobjects.dat unit type 660 (record +0x74) "Anti Matter Storm"
    kUnitTypeResDolphin = 0x295,                     // dbobjects.dat unit type 661 (record +0x74) "Res - Dolphin"
    kUnitTypeFortress = 0x2a0,                       // dbobjects.dat unit type 672 (record +0x74) "b  Fortress"
    kUnitTypeCarrierFighter13Avenger = 0x2a8,        // dbobjects.dat unit type 680 (record +0x74) "a CarrierFighter13 Avenger"
    kUnitTypeGranaryImproved = 0x2ba,                // dbobjects.dat unit type 698 (record +0x74) "b  Granary Improved"
    kUnitTypeFoundationAirport = 0x2cd,              // dbobjects.dat unit type 717 (record +0x74) "Foundation - Airport"
    kUnitTypeEmissary = 0x2d4,                       // dbobjects.dat unit type 724 (record +0x74) "Emissary"
    kUnitTypeFlamingArrow = 0x2d5,                   // dbobjects.dat unit type 725 (record +0x74) "Flaming Arrow"
    kUnitTypeArch03SimpleBowmanFlaming = 0x2d6,      // dbobjects.dat unit type 726 (record +0x74) "Arch03 - Simple Bowman(Flaming)"
    kUnitTypeSpaceDock = 0x2d8,                      // dbobjects.dat unit type 728 (record +0x74) "b  Space Dock"
    kUnitTypeArch05CompositeBowFlaming = 0x2d9,      // dbobjects.dat unit type 729 (record +0x74) "Arch05 - Composite Bow(Flaming)"
    kUnitTypeSp15CapitalShip = 0x2da,                // dbobjects.dat unit type 730 (record +0x74) "Sp15 - Capital Ship"
    kUnitTypeCatalina = 0x2db,                       // dbobjects.dat unit type 731 (record +0x74) "a Catalina"
    kUnitTypeX04RomanLegionnaire = 0x2dc,            // dbobjects.dat unit type 732 (record +0x74) "x04 Roman Legionnaire"
    kUnitTypeArch06LongBowFlaming = 0x2dd,           // dbobjects.dat unit type 733 (record +0x74) "Arch06 - Long Bow(Flaming)"
    kUnitTypeParatrooperPlane = 0x2e1,               // dbobjects.dat unit type 737 (record +0x74) "a Paratrooper Plane"
    kUnitTypePilum = 0x2e4,                          // dbobjects.dat unit type 740 (record +0x74) "Pilum"
    kUnitType14AntiMissileBattery = 0x2e5,           // dbobjects.dat unit type 741 (record +0x74) "14 Anti Missile Battery"
    kUnitTypeInf04ShortSwordCrusader = 0x2e6,        // dbobjects.dat unit type 742 (record +0x74) "Inf04 - Short Sword(Crusader)"
    kUnitTypeInf06LongSwordCrusader = 0x2e7,         // dbobjects.dat unit type 743 (record +0x74) "Inf06 - LongSword(Crusader)"
    kUnitTypeCav04BronzeSpearCavalryCrusader = 0x2e8,  // dbobjects.dat unit type 744 (record +0x74) "Cav04 - Bronze Spear Cavalry(Crusader)"
    kUnitTypeCav06KnightCrusader = 0x2e9,            // dbobjects.dat unit type 745 (record +0x74) "Cav06 - Knight(Crusader)"
    kUnitTypeInf15CyberNinja = 0x2ec,                // dbobjects.dat unit type 748 (record +0x74) "Inf15 - Cyber Ninja"
    kUnitTypeCpMissileBase = 0x2ed,                  // dbobjects.dat unit type 749 (record +0x74) "cp Missile Base"
    kUnitTypeCpICBM = 0x2ee,                         // dbobjects.dat unit type 750 (record +0x74) "cp ICBM"
    kUnitTypeInf10RadioMan = 0x2ef,                  // dbobjects.dat unit type 751 (record +0x74) "Inf10 - Radio Man"
    kUnitTypePriestTower = 0x2f0,                    // dbobjects.dat unit type 752 (record +0x74) "b  Priest Tower"
    kUnitTypeSp15SpaceCarrier = 0x2f3,               // dbobjects.dat unit type 755 (record +0x74) "Sp15 - Space Carrier"
    kUnitTypeSp15SpaceTransport = 0x2f4,             // dbobjects.dat unit type 756 (record +0x74) "Sp15 - Space Transport"
    kUnitTypeSp15SpaceFighter = 0x2f6,               // dbobjects.dat unit type 758 (record +0x74) "Sp15 - Space Fighter"
    kUnitTypeGuardTowerSpace = 0x2fd,                // dbobjects.dat unit type 765 (record +0x74) "b  Guard Tower - Space"
    kUnitTypeWallSpace = 0x2fe,                      // dbobjects.dat unit type 766 (record +0x74) "b  Wall - Space"
    kUnitTypeGateSpace = 0x2ff,                      // dbobjects.dat unit type 767 (record +0x74) "b  Gate - Space"
    kUnitTypeAA15MissileTower = 0x300,               // dbobjects.dat unit type 768 (record +0x74) "b  AA15 Missile Tower"
    kUnitTypeSpaceTurret = 0x302,                    // dbobjects.dat unit type 770 (record +0x74) "b  Space Turret"
    kUnitTypeInf10SASExplosiveExpert = 0x303,        // dbobjects.dat unit type 771 (record +0x74) "Inf10 - SAS Explosive Expert"
    kUnitTypeBombForSASExplosiveExpert = 0x305,      // dbobjects.dat unit type 773 (record +0x74) "Bomb for SAS Explosive Expert"
    kUnitTypeCapitalShipSpecialWeapon = 0x309,       // dbobjects.dat unit type 777 (record +0x74) "Capital Ship Special Weapon"
    kUnitTypeCarrierFighter11Dauntless = 0x30c,      // dbobjects.dat unit type 780 (record +0x74) "a Carrier Fighter11 Dauntless"
    kUnitTypeCarrierFighter11Zero = 0x30d,           // dbobjects.dat unit type 781 (record +0x74) "a Carrier Fighter11 Zero"
    kUnitTypeGatePalisades = 0x311,                  // dbobjects.dat unit type 785 (record +0x74) "b  Gate - Palisades"
    kUnitTypeGuardTowerPalisades = 0x312,            // dbobjects.dat unit type 786 (record +0x74) "b  Guard Tower - Palisades"
    kUnitTypeTeleporter = 0x313,                     // dbobjects.dat unit type 787 (record +0x74) "b  Teleporter"
    kUnitTypeOrbitalSpaceStation = 0x31f,            // dbobjects.dat unit type 799 (record +0x74) "w  Orbital Space Station"
    kUnitTypeMarketCP = 0x321,                       // dbobjects.dat unit type 801 (record +0x74) "b  Market (CP)"
    kUnitTypeGuardTowerBamboo = 0x322,               // dbobjects.dat unit type 802 (record +0x74) "b  Guard Tower - Bamboo"
    kUnitTypeMeteor = 0x325,                         // dbobjects.dat unit type 805 (record +0x74) "Meteor"
    kUnitTypeFighter15PlanetaryFighter = 0x330,      // dbobjects.dat unit type 816 (record +0x74) "a Fighter15 Planetary Fighter"
    kUnitTypeRoboticFarm = 0x33a,                    // dbobjects.dat unit type 826 (record +0x74) "b  Robotic Farm"
    kUnitTypeSpaceFallowFarm = 0x33b,                // dbobjects.dat unit type 827 (record +0x74) "Space Fallow Farm"
    kUnitTypeCapitalShipYamato = 0x33c,              // dbobjects.dat unit type 828 (record +0x74) "x Capital Ship Yamato"
};

// EEUnitClass::mFlags2D0 (+0x2d0): where the class moves; the EEUnitClass ctor 0x62f317 sets the bits by mCategory (setrilo unit_class.h movement_ '1 air, 2 land, 4 water'; 8 = the AoC space categories)
enum EEUnitMovementFlag {
    kMovementAir = 1,            // setrilo movement_ bit 1 'air'; EEUnitClass ctor 0x62f317 (EEUnitClass.cpp:388): categories Airplane, Helicopter, HelicopterTransport, HelicopterPriest, AirAnimal
    kMovementLand = 2,           // setrilo movement_ bit 2 'land'; EEUnitClass ctor 0x62f317 (EEUnitClass.cpp:388): Land, LandCitizen, LandPriest, LandTransport, LandMilitaryHero, towers
    kMovementSea = 4,            // setrilo movement_ bit 4 'water'; EEUnitClass ctor 0x62f317 (EEUnitClass.cpp:388): Naval, NavalCitizen, Land8, SubmergedNaval, NavalPriest, NavalTransport, NavalBuilding, SeaAnimal
    kMovementAirOrSea = kMovementAir | kMovementSea,       // kMovementAir | kMovementSea (EECPExplorationManager.cpp:295, EEUnitClass.cpp:6384) (provisional name)
    kMovementLandOrSea = kMovementLand | kMovementSea,      // kMovementLand | kMovementSea (EEFOFindNewTargetNearOldTarget.cpp:724) (provisional name)
    kMovementSpace = 8,          // EEUnitClass ctor 0x62f317 (EEUnitClass.cpp:388): Spaceship, SpaceTransport, SpaceBuilding, PlanetSpaceship; EECPExplorationManager::RequestExplorer kecpekSpace (provisional name)
    kMovementSeaOrSpace = kMovementSea | kMovementSpace,   // kMovementSea | kMovementSpace (EEComplexUnit.cpp:1063) (provisional name)
    kMovementAirSeaOrSpace = kMovementAir | kMovementSea | kMovementSpace,  // kMovementAir | kMovementSea | kMovementSpace; RequestExplorer case 0 (land explorers) takes classes without them (provisional name)
};

// EEUnitClass::mKindFlags (+0x1e0): the class's kind for selection and commands; the EEUnitClass ctor 0x62f317 sets one kind by mCategory (setrilo unit_class.cpp layer_) and ORs the mCanEnter* bits; EEGroup::mKindFlags ORs its units' (UpdateKindFlags); EEUIFMainGame::UpdateCursorKind picks the command by them
enum EEUnitKindFlag {
    kUnitKindCitizen = 1,                      // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category LandCitizen; EEGroup::FindUnitWithFlags(1) finds the gatherer/repairer (provisional name)
    kUnitKindMilitary = 2,                     // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): Airplane, Naval, Land, Helicopter, SubmergedNaval, LandMilitaryHero, Spaceship..., towers (provisional name)
    kUnitKindPriest = 4,                       // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): LandPriest, NavalPriest, HelicopterPriest (not unit type 0xd8) (provisional name)
    kUnitKindResource = 8,                     // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category Resource (provisional name)
    kUnitKindBuilding = 0x10,                  // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): Building, Building19, NavalBuilding, MiningBuilding, Gate, SpaceBuilding (not towers) (provisional name)
    kUnitKindBuildingPriest = kUnitKindPriest | kUnitKindBuilding,            // kUnitKindPriest | kUnitKindBuilding: EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category BuildingPriest (provisional name)
    kUnitKindPathPoint = 0x20,                 // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category Ambient with unit type 0xdf (kUnitTypePathPoint, dbobjects 'PathPoint')
    kUnitKindNavalCitizen = 0x40,              // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category NavalCitizen (provisional name)
    kUnitKindHelicopterTransport = 0x80,       // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category HelicopterTransport (RTTI EEUCHelicopterTransport); EECanEnterUnit pairs it with kUnitKindCanEnterAirTransports
    kUnitKindLandTransport = 0x100,            // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category LandTransport (RTTI EEUCLandTransport)
    kUnitKindNavalTransport = 0x200,           // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category NavalTransport (RTTI EEUCNavalTransport)
    kUnitKindCanEnterAirTransports = 0x400,    // EEUnitClass.cpp:516: set when mCanEnterAirTransports
    kUnitKindCanEnterLandTransports = 0x800,   // EEUnitClass.cpp:518: set when mCanEnterLandTransports
    kUnitKindCanEnterSeaTransports = 0x1000,   // EEUnitClass.cpp:520: set when mCanEnterSeaTransports
    kUnitKindCanEnterFortresses = 0x2000,      // EEUnitClass.cpp:522: set when mCanEnterFortresses (provisional name)
    kUnitKindFortress = 0x4000,                // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category Fortress (RTTI EEUCFortress)
    kUnitKindLongRange = 0x8000,               // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): added to kUnitKindMilitary when mRange > mLineOfSight; Fn265D7A tests it (provisional name)
    kUnitKindLongRangeMilitary = kUnitKindLongRange | kUnitKindMilitary,       // kUnitKindLongRange | kUnitKindMilitary (EEUnitClass.cpp:480) (provisional name)
    kUnitKindSpaceTransport = 0x10000,         // EEUnitClass ctor 0x62f317 switch (mCategory) (EEUnitClass.cpp:442): category SpaceTransport (RTTI EEUCSpaceTransport)
    kUnitKindCanEnterSpaceTransports = 0x20000,  // EEUnitClass.cpp:524: set when mCanEnterSpaceTransports (provisional name)
};

// EEUnit::mHiddenFlags (+0x44): setrilo unit.h flags_ 'bit 4 moved, bit 5 hidden, bits 0-1 the hidden check's phase' (EEUnit::UpdateHidden 0x628f90)
enum EEUnitHiddenFlag {
    kHiddenFlagPhaseMask = 3,  // setrilo unit.h flags_ bits 0-1: the hidden check's phase ((game time / 1000) % 3)
    kHiddenFlagMoved = 0x10,  // setrilo unit.h flags_ bit 4 'moved'; EEUnit::MoveTo sets it
    kHiddenFlagHidden = 0x20,  // setrilo unit.h flags_ bit 5 'hidden'; UpdateHidden returns it
};

// The media EEUnitClassUpgrade::LoadMedia 0x63bcc3 loads, one bit each (inArg); EETechTree::GetUnitClassMediaMask 0x5cc710 starts from all of them and clears the bits of media from epochs before the start epoch
enum EEUnitMediaFlag {
    kMediaAcknowledgeSound1 = 1,  // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mSounds74[0] (GetAcknowledgeSound / SetAcknowledgeSound index 0) (provisional name)
    kMediaAcknowledgeSound2 = 2,  // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mSounds74[1] (provisional name)
    kMediaCreationSound = 4,     // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mSound6C (SetCreationSound) (provisional name)
    kMediaDeathSound = 8,        // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mSound70 (SetDeathSound; UnitDied plays it) (provisional name)
    kMediaSelectionSound1 = 0x10,  // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mSounds7C[0] (GetSelectionSound / SetSelectionSound index 0) (provisional name)
    kMediaSelectionSound2 = 0x20,  // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mSounds7C[1] (provisional name)
    kMediaModel = 0x40,          // EEUnitClassUpgrade::LoadMedia 0x63bcc3: mModel->LoadMedia() (provisional name)
    kMediaAll = kMediaAcknowledgeSound1 | kMediaAcknowledgeSound2 | kMediaCreationSound | kMediaDeathSound | kMediaSelectionSound1 | kMediaSelectionSound2 | kMediaModel,            // every media bit (GetUnitClassMediaMask's start value) (provisional name)
};

#endif // EE_EEUNITCLASSIDS_H
