// ee/EEAIKindIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EEAIKINDIDS_H
#define EE_EEAIKINDIDS_H

// EEAIArea::mAreaType (+0x64): the area class each creator stores; EECPILocation::mType uses the same ids (Update 0x480a57 re-finds the area of that class at the tile)
enum EEAIAreaKind {
    kAIAreaNone = 0,         // the EEAIArea ctor 0x4110cb stores 0; EECPILocation::Update 0x480a57 stores 0 when no area is found (provisional name)
    kAIAreaContinent = 1,    // RTTI EEAIContinent: EEAIAStarContinentGoal::CreateContinent 0x4156b2 stores 1; EECPILocation::Update: GetContinentAt
    kAIAreaChokePoint = 2,   // RTTI EEAIChokePoint: EEAIAStarChokePointGoal::CreateChokePoint stores 2; EECPILocation::Update: GetChokePointAt
    kAIAreaForest = 3,       // RTTI EEAIForest: EEAIAStarPathAreaGoal::CreateAreas 0x417510 stores 3; EECPILocation::Update: GetForestAt
    kAIAreaTown = 5,         // RTTI EETown: the EETown ctor stores 5; EECPILocation::Update: the player's town containing the tile
    kAIAreaArmy = 6,         // RTTI EECPIArmy: the EECPIArmy ctor stores 6; EECPILocation::Update keeps it
    kAIAreaHill = 7,         // RTTI EEAIHill: EEAIAStarHillGoal::CreateHill stores 7; EECPILocation::Update: GetHillAt; EEEMStorm::Begin dynamic_casts GetRandomArea(7) to EEAIHill
    kAIAreaRectangleArea = 8,  // RTTI EEAIRectangleArea: EEAIContinentManager::CreateScenarioAreas stores 8; EECPILocation::Update: GetRectangleAreaAt
    kAIAreaHerd = 9,         // RTTI EEHerd: EEHerd::Clear stores 9
    kAIAreaPathArea = 10,    // RTTI EEAIPathArea: EEAIAStarPathAreaGoal::CreateAreas 0x417510 stores 10
};

// Continent chain kinds (the EEAIContinent::mCounts index): EEAIContinentManager::NumberContinentChains 0x4201f7 numbers chains of land continents joined across the listed continent categories; AreConnected 0x422872 picks one by the unit's terrain flags (EEUnitClass::Fn2349B1: 1 terrain 9, 2 terrain 1, 8 terrain 0xc)
enum EEAIChainKind {
    kAIChainNone = 0,          // the EEAIAStarGraphContinentalChainGoal ctor stores 0 (mCounts[0] unused) (provisional name)
    kAIChainLandCliff = 1,     // NumberContinentChains 0x4201f7: kind 1 crosses land and cliff continents (provisional name)
    kAIChainLandWater = 2,     // NumberContinentChains 0x4201f7: kind 2 crosses land and water continents (provisional name)
    kAIChainLandWaterCliff = 3,  // NumberContinentChains 0x4201f7: kind 3 crosses land, water and cliff continents (provisional name)
    kAIChainLandSpace = 4,     // NumberContinentChains 0x4201f7: kind 4 crosses land and space continents (provisional name)
    kAIChainLandWaterSpace = 5,  // NumberContinentChains 0x4201f7: kind 5 crosses land, water and space continents (provisional name)
    kAIChainLandCliffSpace = 6,  // NumberContinentChains 0x4201f7: kind 6 crosses land, cliff and space continents (provisional name)
};

// EEAIContinent::mCategory (+0x11a): the terrain class of a continent; EEAIAStarContinentGoal::AcceptTile 0x41575c takes it from the fill's first tile (also the path-area/sector goals' copies and the manager's category filters)
enum EEAIContinentCategory {
    kContinentCategoryNone = 0,   // unset (EEAIAStarContinentGoal ctor 0x4155c0 / CreateContinent 0x4156b2 reset; AcceptTile 0x41575c classifies the first tile while 0); as a filter any category (GetLargestContinent 0x422952; GetRandomContinent 0x422dc8: any but the map edge) (provisional name)
    kContinentCategoryLand = 1,   // setrilo ai_continents.h:215 '1 land'; AcceptTile 0x41575c: any terrain but 1, 9, 0xc
    kContinentCategoryWater = 2,  // setrilo ai_continents.h:215 '2 deep water'; AcceptTile 0x41575c: terrain 1; CreateContinents counts it into mNumWaterTiles
    kContinentCategoryCliff = 3,  // setrilo ai_continents.h:215 '3 cliff'; AcceptTile 0x41575c: terrain 9 (the cliff terrain EEMap::UpdateCliffs sets)
    kContinentCategorySpace = 4,  // AcceptTile 0x41575c: terrain 0xc; CreateContinents counts it into mNumSpaceTiles; EECPAttackGroup::AssignSpaceTask and EEUCSpaceTransport::CanUnloadAt test it (provisional name)
    kContinentCategoryMapEdge = 5,  // AcceptTile 0x41575c: EEMap::IsEdgeTile tiles (setrilo's base-game rim); EEHerd 0x4a4aa2 leaves the map through a random one (provisional name)
};

// EEAIAStarMachineBase::mType (+0x0c): EEPathManager's path machines by the unit's movement (the path request: may stand on terrain 1 -> 2, on terrain 0xc -> 0x10, else 1) and its bounded machine
enum EEAIMachineType {
    kAIMachineLandPath = 1,    // EEPathManager::NewPathMachine: new EEPathMachine(1); the path request's default type (provisional name)
    kAIMachineWaterPath = 2,   // EEPathManager path request: a unit that may stand on terrain 1 gets type 2; FinishFailedPath then uses EEAIBPGModifierSea (provisional name)
    kAIMachineBoundedPath = 4,  // EEPathManager::PrepareBoundedMachine: new EEBoundedPathMachine(4); FinishPath reads the bounded goal for it (provisional name)
    kAIMachineSpacePath = 0x10,  // EEPathManager path request: a unit that may stand on terrain 0xc gets type 0x10; FinishFailedPath then uses EEAIBPGModifierSpace (provisional name)
};

// The region ids a path area carries (EEAIPathArea +0x84..+0x8a): EEAIContinentManager::CreatePathAreaKind 0x4204ea grows one kind per pass (EEAIAStarPathAreaGoal::mMode); GetContinent 0x422b86, GetAdjacentAreaID 0x421a18 and EEAIContinent::GetShoreInRegions select the id by it
enum EEAIPathKind {
    kAIPathNone = 0,       // EEAIAStarPathAreaGoal::mMode 0: the plain path-area pass (CreateAreas 0x417510), no region id (provisional name)
    kAIPathWaterBody = 1,  // setrilo ai_continents.h:153 'kinds 1 and 2: its water body and land mass'; GetContinent 0x422b86: mWaterBodyID; CreatePathAreaKind 0x4204ea: terrain 1 / 10
    kAIPathLandMass = 2,   // setrilo ai_continents.h:153; GetContinent 0x422b86: mLandMassID; CreatePathAreaKind 0x4204ea: not terrain 1 / 0xc
    kAIPathSpaceRegion = 3,  // GetContinent 0x422b86: mSpaceRegionID; CreatePathAreaKind 0x4204ea: terrain 0xc (provisional name)
    kAIPathNonSpace = 4,   // GetContinent 0x422b86: mW84; CreatePathAreaKind 0x4204ea and GetAdjacentAreaID 0x421a18: tiles not terrain 0xc (and not on the map's diagonal edges) (provisional name)
};

// The AI's kind of a tile and of the path area / sub-sector grown from it (EEAIGetTileKind 0x41ed2c, EEAIAStarPathAreaGoal::AcceptTile, EEAIAStarPathSectorGoal::ClassifyTile; EEAIPathArea::mKind, EEAIPathSubSector::mKind); shore / near-shore by the manager's tile value (UpdateTileValues 0x4231d7: < 0 a shore tile, > 0 next to shore tiles)
enum EEAITileKind {
    kAITileNone = 0,             // EEAIGetTileKind 0x41ed2c off the map; ctors and resets store 0 (provisional name)
    kAITileLand = 1,             // setrilo ai_continents.h:149/427 '1 land'; EEAIGetTileKind 0x41ed2c: land continent, not terrain 10
    kAITileWater = 2,            // setrilo ai_continents.h:149 '2 still water'; EEAIGetTileKind 0x41ed2c: water continent, tile value 0
    kAITileCliff = 3,            // setrilo ai_continents.h:149 '3 cliffs'; EEAIGetTileKind 0x41ed2c: cliff continent
    kAITileForest = 4,           // setrilo ai_continents.h:149 '4 forest'; EEAIAStarPathAreaGoal::CreateAreas 0x417510 makes an EEAIForest for it; EEAIPathSubSector::GetBlockingUnit 0x42bf45: a tree
    kAITileGaiaObstacle = 5,     // setrilo route_planners.h:56 '5 a class with +600 of player 0's'; EEAIPathSubSector::GetBlockingUnit 0x42bf45: a tile-blocking unit of player 0 (no AoC goal produces 5)
    kAITileMapEdge = 6,          // setrilo ai_continents.h:149 '6 the rim'; EEAIGetTileKind 0x41ed2c: map-edge continent
    kAITileShallows = 7,         // setrilo ai_continents.h:427 'shallows 7 / 8 / 9 by the weight's sign'; EEAIGetTileKind 0x41ed2c: terrain 10 on land, tile value 0
    kAITileShallowsShore = 8,    // EEAIGetTileKind 0x41ed2c: terrain 10, tile value < 0 (UpdateTileValues 0x4231d7: a shore tile) (provisional name)
    kAITileShallowsNearShore = 9,  // EEAIGetTileKind 0x41ed2c: terrain 10, tile value > 0 (next to shore tiles) (provisional name)
    kAITileShore = 10,           // EEAIGetTileKind 0x41ed2c: water or space continent, tile value < 0 (a shore tile) (provisional name)
    kAITileNearShore = 11,       // EEAIGetTileKind 0x41ed2c: water or space continent, tile value > 0 (next to shore tiles) (provisional name)
    kAITileUnitObstacle = 0xc,   // setrilo ai_continents.h:428 '12 / 13 / 14 with a unit in the way'; EEAIPathSubSector::GetBlockingUnit 0x42bf45: another player's tile-blocking unit
    kAITileWall = 0xd,           // setrilo route_planners.h:57 '0xD a class with +612 that is not a gate'; GetBlockingUnit 0x42bf45: mIsWall, category not 0x19
    kAITileGate = 0xe,           // setrilo route_planners.h:57 '0xE a gate'; GetBlockingUnit 0x42bf45: unit category 0x19; EECPASGLand::IsGatePassable
    kAITileSpace = 0xf,          // EEAIGetTileKind 0x41ed2c: space continent (terrain 0xc), tile value 0 (provisional name)
};

// One byte per tile / node of the A* storages (EEAIAStarStorage::mTileFlags, EEAIAStarGraphStorage::mFlags): the goal's verdict on the tile (EEAIAStarMachine::TestTile, MarkTile) and the open / closed list it is on
enum EEAIAStarTileFlag {
    kAStarTileAccepted = 1,  // EEAIAStarMachine::TestTile: mGoal.AcceptTile said yes (cached; MarkTile(x, y, true)) (provisional name)
    kAStarTileRejected = 2,  // TestTile: AcceptTile said no (MarkTile(x, y, false)) (provisional name)
    kAStarTileOpen = 4,    // AddOpen sets it, RemoveBestOpen / RemoveNearestOpen clear it; GetOpenNode tests it (provisional name)
    kAStarTileClosed = 8,  // AddClosed sets it, RemoveClosed clears it; GetClosedNode tests it (provisional name)
};

#endif // EE_EEAIKINDIDS_H
