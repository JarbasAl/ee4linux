// ee/EEScenarioIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EESCENARIOIDS_H
#define EE_EESCENARIOIDS_H

// AI effect actions (EESMEffect::mAIAction; EESETSETAI::mTypes; Execute mType 6 switch).
enum EESMAIAction {
    kSMAIActionModifyAIMilitaryTarget = 0,  // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29690 (0x73fa) "Modify AI Military Target"
    kSMAIActionCreateAIMilitaryGroup = 1,  // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29691 (0x73fb) "Create AI Military Group"
    kSMAIActionBuildObject = 2,           // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29692 (0x73fc) "Build Object"
    kSMAIActionMaintainObject = 3,        // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29693 (0x73fd) "Maintain Object"
    kSMAIActionSetAIToggle = 4,           // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29694 (0x73fe) "Set AI Toggle"
    kSMAIActionSetAIVariable = 5,         // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29695 (0x73ff) "Set AI Variable"
    kSMAIActionSetFavoriteUnits = 6,      // EESETSETAI "Action" list 0x73fa+i (GetString(mCurrentType + 0x73fa)): Language.rc 29696 (0x7400) "Set Favorite Units"
};

// AI toggles (EESMEffect::mAIToggle; SetAIToggle 0x5c26da switch).
enum EESMAIToggle {
    kSMAIToggleActive = 0,        // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29625 (0x73b9) "Active"
    kSMAIToggleCheat = 1,         // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29626 (0x73ba) "Cheat"
    kSMAIToggleAttack = 2,        // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29627 (0x73bb) "Attack"
    kSMAIToggleDefend = 3,        // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29628 (0x73bc) "Defend"
    kSMAIToggleBuildPerRepair = 4,  // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29629 (0x73bd) "Build/Repair"
    kSMAIToggleExplore = 5,       // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29630 (0x73be) "Explore"
    kSMAIToggleIgnore = 6,        // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29631 (0x73bf) "Ignore"
    kSMAIToggleGather = 7,        // Set AI Toggle "AI Toggle" list 0x73b9+i (EESETSETAI): Language.rc 29632 (0x73c0) "Gather"
};

// Condition comparison operators (EESMCondition::Compare 0x5ba998; editor list 0x727e..0x7282).
enum EESMCompareOperator {
    kSMCompareLessThan = 0,      // condition operator list 0x727e+i: Language.rc 29310 "<"; EESMCondition::Compare case 0
    kSMCompareLessOrEqual = 1,   // condition operator list 0x727e+i: Language.rc 29311 "<="; EESMCondition::Compare case 1
    kSMCompareEqual = 2,         // condition operator list 0x727e+i: Language.rc 29312 "="; EESMCondition::Compare case 2
    kSMCompareGreaterOrEqual = 3,  // condition operator list 0x727e+i: Language.rc 29313 ">="; EESMCondition::Compare case 3
    kSMCompareGreater = 4,       // condition operator list 0x727e+i: Language.rc 29314 ">"; EESMCondition::Compare case 4
};

// Scenario condition kinds (EESMCondition::mType; EESMCondition::Evaluate 0x5baabf switch; EESETSCondition::mTypes index).
enum EESMConditionType {
    kSMConditionAlwaysTrue = 0,         // EESETSCondition mTypes[0] = EESETSCTAlwaysTrue; EESMCondition::Evaluate case 0 returns true (provisional name)
    kSMConditionObject = 1,             // the scenario-object condition (EEScenarioManager::NewObject stores 1 with mObjectIndex; Evaluate case 1 = EESMObject::Evaluate) (provisional name)
    kSMConditionPlayerAttribute = 2,    // EESETSCondition::RefreshControls GetString(mType + 0x7272): Language.rc 29300 "Player-Attribute"
    kSMConditionPlayerTechnology = 3,   // EESETSCondition::RefreshControls GetString(mType + 0x7272): Language.rc 29301 "Player-Technology"
    kSMConditionTriggerAttribute = 4,   // EESETSCondition::RefreshControls GetString(mType + 0x7272): Language.rc 29302 "Trigger- Attribute"
    kSMConditionPlayerDiplomacy = 5,    // EESETSCondition::RefreshControls GetString(mType + 0x7272): Language.rc 29303 "Player-Diplomacy"
    kSMConditionGameAttribute = 6,      // EESETSCondition::RefreshControls GetString(mType + 0x7272): Language.rc 29304 "Game-Attribute"
    kSMConditionChatMessageContains = 7,  // EESETSCondition::RefreshControls GetString(mType + 0x7272): Language.rc 29305 "Chat Message Contains"
};

// Scenario effect kinds (EESMEffect::mType; EESETSEffect::mTypes index; EESMEffect::Execute 0x5bd5eb switch).
enum EESMEffectType {
    kSMEffectNone = 0,   // EESETSEffect mTypes[0] = EESETSETNoEffect; EEScenarioManager::Reset stores 0 in the null effect (provisional name)
    kSMEffectObject = 1,  // EESETSEffect ctor type names 0x7289+i for mTypes[1..6]: Language.rc 29321 (0x7289) "Object"
    kSMEffectPlayer = 2,  // EESETSEffect ctor type names 0x7289+i for mTypes[1..6]: Language.rc 29322 (0x728a) "Player"
    kSMEffectTrigger = 3,  // EESETSEffect ctor type names 0x7289+i for mTypes[1..6]: Language.rc 29323 (0x728b) "Trigger"
    kSMEffectMedia = 4,  // EESETSEffect ctor type names 0x7289+i for mTypes[1..6]: Language.rc 29324 (0x728c) "Media"
    kSMEffectGame = 5,   // EESETSEffect ctor type names 0x7289+i for mTypes[1..6]: Language.rc 29325 (0x728d) "Game"
    kSMEffectAI = 6,     // EESETSEffect ctor type names 0x7289+i for mTypes[1..6]: Language.rc 29326 (0x728e) "AI"
};

// Game effect actions (EESMEffect::mGameAction; EESETSETGame::mTypes; Execute mType 5 switch).
enum EESMGameAction {
    kSMGameActionDaylightHour = 0,        // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29575 (0x7387) "Daylight Hour"
    kSMGameActionDayPerNightCycleTime = 1,  // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29576 (0x7388) "Day/Night Cycle Time"
    kSMGameActionCalamity = 2,            // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29577 (0x7389) "Calamity"
    kSMGameActionGraphicEffect = 3,       // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29578 (0x738a) "Graphic Effect"
    kSMGameActionStartCinematicMode = 4,  // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29579 (0x738b) "Start-Cinematic Mode"
    kSMGameActionEndCinematicMode = 5,    // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29580 (0x738c) "End-Cinematic Mode"
    kSMGameActionAmbientWeather = 6,      // EESETSETGame "Action" list 0x7387+i (GetString(mCurrentType + 0x7387)): Language.rc 29581 (0x738d) "Ambient-Weather"
};

// Media effect actions (EESMEffect::mMediaAction; EESETSETMedia::mTypes; Execute mType 4 switch).
enum EESMMediaAction {
    kSMMediaActionPlaySound = 0,      // EESETSETMedia "Action" list 0x7355+i (GetString(mCurrentType + 0x7355)): Language.rc 29525 (0x7355) "Play Sound"
    kSMMediaActionChangeText = 1,     // EESETSETMedia "Action" list 0x7355+i (GetString(mCurrentType + 0x7355)): Language.rc 29526 (0x7356) "Change Text"
    kSMMediaActionSendDialogue = 2,   // EESETSETMedia "Action" list 0x7355+i (GetString(mCurrentType + 0x7355)): Language.rc 29527 (0x7357) "Send Dialogue"
    kSMMediaActionSendChatMessage = 3,  // EESETSETMedia "Action" list 0x7355+i (GetString(mCurrentType + 0x7355)): Language.rc 29528 (0x7358) "Send Chat Message"
};

// Change Text effect targets (EESMEffect::mTextObject; Execute media case 1 switch: hints/instructions/history).
enum EESMMediaText {
    kSMMediaTextHints = 0,       // Change Text "Text Object" list 0x736e+i (EESETSETMedia): Language.rc 29550 (0x736e) "Hints"
    kSMMediaTextInstructions = 1,  // Change Text "Text Object" list 0x736e+i (EESETSETMedia): Language.rc 29551 (0x736f) "Instructions"
    kSMMediaTextHistory = 2,     // Change Text "Text Object" list 0x736e+i (EESETSETMedia): Language.rc 29552 (0x7370) "History"
};

// Object effect actions (EESMEffect::mObjectAction; EESETSETObject::mTypes/mCurrentType; Execute mType 1 switch).
enum EESMObjectAction {
    kSMObjectActionCreate = 0,         // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29350 (0x72a6) "Create"
    kSMObjectActionFreeze = 1,         // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29351 (0x72a7) "Freeze"
    kSMObjectActionUnfreeze = 2,       // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29352 (0x72a8) "Unfreeze"
    kSMObjectActionKill = 3,           // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29353 (0x72a9) "Kill"
    kSMObjectActionRemove = 4,         // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29354 (0x72aa) "Remove"
    kSMObjectActionTaskObject = 5,     // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29355 (0x72ab) "Task-Object"
    kSMObjectActionTaskArea = 6,       // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29356 (0x72ac) "Task-Area"
    kSMObjectActionStance = 7,         // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29357 (0x72ad) "Stance"
    kSMObjectActionUnitAttribute = 8,  // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29358 (0x72ae) "Unit-Attribute"
    kSMObjectActionOwner = 9,          // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29359 (0x72af) "Owner"
    kSMObjectActionClassName = 10,     // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29360 (0x72b0) "Class-Name"
    kSMObjectActionFaceObject = 11,    // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29361 (0x72b1) "Face Object"
    kSMObjectActionClassAttribute = 12,  // EESETSETObject "Action" list 0x72a6+i (GetString(mCurrentType + 0x72a6)): Language.rc 29362 (0x72b2) "Class-Attribute"
};

// Player effect actions (EESMEffect::mPlayerAction; EESETSETPlayer::mTypes/mCurrentType; Execute mType 2 switch).
enum EESMPlayerAction {
    kSMPlayerActionVictory = 0,          // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29452 (0x730c) "Victory"
    kSMPlayerActionScriptCamera = 1,     // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29453 (0x730d) "Script-Camera"
    kSMPlayerActionDefeat = 2,           // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29454 (0x730e) "Defeat"
    kSMPlayerActionDiplomacy = 3,        // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29455 (0x730f) "Diplomacy"
    kSMPlayerActionSetAttributeLevel = 4,  // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29456 (0x7310) "Set-Attribute Level"
    kSMPlayerActionModifyTechTree = 5,   // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29457 (0x7311) "Modify-Tech-Tree"
    kSMPlayerActionAwardCivPoints = 6,   // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29458 (0x7312) "Award-Civ-Points"
    kSMPlayerActionFadeIn = 7,           // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29459 (0x7313) "Fade-In"
    kSMPlayerActionFadeOut = 8,          // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29460 (0x7314) "Fade-Out"
    kSMPlayerActionVisibilityOn = 9,     // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29461 (0x7315) "Visibility-On"
    kSMPlayerActionVisibilityOff = 10,   // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29462 (0x7316) "Visibility-Off"
    kSMPlayerActionMiniMapFlare = 11,    // EESETSETPlayer "Action" list 0x730c+i (GetString(mCurrentType + 0x730c)): Language.rc 29463 (0x7317) "MiniMap-Flare"
    kSMPlayerActionTotal = 12,           // count of player actions; EESMEffect::Load resets mPlayerAction >= 12 to 0 (provisional name)
};

// Player attribute ids of the player conditions/effects (EESMCondition::GetPlayerValue 0x5baf78, EESMEffect::SetPlayerAttribute 0x5bcb1f; list built in EESETSConditionType/EESETSETPlayer).
enum EESMPlayerValue {
    kSMPlayerValueAnyResource = 0,           // player attribute list (EESETSConditionType/EESETSETPlayer) entry 0: Language.rc 29670 "Any Resource"
    kSMPlayerValueAllResources = 1,          // player attribute list (EESETSConditionType/EESETSETPlayer) entry 1: Language.rc 29671 "All Resources"
    kSMPlayerValueStone = 2,                 // player attribute list (EESETSConditionType/EESETSETPlayer) entry 2: Language.rc 29672 "Stone"
    kSMPlayerValueGold = 3,                  // player attribute list (EESETSConditionType/EESETSETPlayer) entry 3: Language.rc 29673 "Gold"
    kSMPlayerValueWood = 4,                  // player attribute list (EESETSConditionType/EESETSETPlayer) entry 4: Language.rc 29674 "Wood"
    kSMPlayerValueFood = 5,                  // player attribute list (EESETSConditionType/EESETSETPlayer) entry 5: Language.rc 29675 "Food"
    kSMPlayerValueIron = 6,                  // player attribute list (EESETSConditionType/EESETSETPlayer) entry 6: Language.rc 29676 "Iron"
    kSMPlayerValueCurrentPop = 7,            // player attribute list (EESETSConditionType/EESETSETPlayer) entry 7: Language.rc 29677 "Current Pop"
    kSMPlayerValuePopCap = 8,                // player attribute list (EESETSConditionType/EESETSETPlayer) entry 8: Language.rc 29678 "Pop Cap"
    kSMPlayerValueUnitCount = 9,             // player attribute list (EESETSConditionType/EESETSETPlayer) entry 9: Language.rc 29679 "Unit Count"
    kSMPlayerValueDifficultyLevel = 10,      // player attribute list (EESETSConditionType/EESETSETPlayer) entry 10: Language.rc 29680 "Difficulty Level"
    kSMPlayerValueVariable0 = 11,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 0); GetPlayerValue mScenarioVariables[0]
    kSMPlayerValueVariable1 = 12,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 1); GetPlayerValue mScenarioVariables[1]
    kSMPlayerValueVariable2 = 13,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 2); GetPlayerValue mScenarioVariables[2]
    kSMPlayerValueVariable3 = 14,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 3); GetPlayerValue mScenarioVariables[3]
    kSMPlayerValueVariable4 = 15,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 4); GetPlayerValue mScenarioVariables[4]
    kSMPlayerValueVariable5 = 16,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 5); GetPlayerValue mScenarioVariables[5]
    kSMPlayerValueVariable6 = 17,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 6); GetPlayerValue mScenarioVariables[6]
    kSMPlayerValueVariable7 = 18,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 7); GetPlayerValue mScenarioVariables[7]
    kSMPlayerValueVariable8 = 19,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 8); GetPlayerValue mScenarioVariables[8]
    kSMPlayerValueVariable9 = 20,            // list entries 11..20: sprintf("%s%d", Language.rc 0x79e0 "Variable", 9); GetPlayerValue mScenarioVariables[9]
    kSMPlayerValueFoodSent = 21,             // list entry: Language.rc 30116 "Food" + 0x7bd6 "Sent"; GetPlayerValue mTributes[0]
    kSMPlayerValueFoodReceived = 22,         // list entry: Language.rc 30116 "Food" + 0x7bd8 "Received"; GetPlayerValue mTributes[5]
    kSMPlayerValueWoodSent = 23,             // list entry: Language.rc 30117 "Wood" + 0x7bd6 "Sent"; GetPlayerValue mTributes[1]
    kSMPlayerValueWoodReceived = 24,         // list entry: Language.rc 30117 "Wood" + 0x7bd8 "Received"; GetPlayerValue mTributes[6]
    kSMPlayerValueStoneSent = 25,            // list entry: Language.rc 30118 "Stone" + 0x7bd6 "Sent"; GetPlayerValue mTributes[2]
    kSMPlayerValueStoneReceived = 26,        // list entry: Language.rc 30118 "Stone" + 0x7bd8 "Received"; GetPlayerValue mTributes[7]
    kSMPlayerValueGoldSent = 27,             // list entry: Language.rc 30119 "Gold" + 0x7bd6 "Sent"; GetPlayerValue mTributes[3]
    kSMPlayerValueGoldReceived = 28,         // list entry: Language.rc 30119 "Gold" + 0x7bd8 "Received"; GetPlayerValue mTributes[8]
    kSMPlayerValueIronSent = 29,             // list entry: Language.rc 30120 "Iron" + 0x7bd6 "Sent"; GetPlayerValue mTributes[4]
    kSMPlayerValueIronReceived = 30,         // list entry: Language.rc 30120 "Iron" + 0x7bd8 "Received"; GetPlayerValue mTributes[9]
    kSMPlayerValueStatBuildingsLost = 31,    // list entry 31: Language.rc 31201 "Buildings Lost" (statistic; GetPlayerValue mSavedGroups.GetLastValue(1))
    kSMPlayerValueStatBuildingsRazed = 32,   // list entry 32: Language.rc 31202 "Buildings Razed" (statistic; GetPlayerValue mSavedGroups.GetLastValue(2))
    kSMPlayerValueStatUnitsKilled = 33,      // list entry 33: Language.rc 31203 "Units Killed" (statistic; GetPlayerValue mSavedGroups.GetLastValue(3))
    kSMPlayerValueStatUnitsLost = 34,        // list entry 34: Language.rc 31204 "Units Lost" (statistic; GetPlayerValue mSavedGroups.GetLastValue(4))
    kSMPlayerValueStatConversions = 35,      // list entry 35: Language.rc 31205 "Conversions" (statistic; GetPlayerValue mSavedGroups.GetLastValue(5))
    kSMPlayerValueStatCalamities = 36,       // list entry 36: Language.rc 31206 "Calamities" (statistic; GetPlayerValue mSavedGroups.GetLastValue(6))
    kSMPlayerValueStatFood = 37,             // list entry 37: Language.rc 31207 "Food" (statistic; GetPlayerValue mSavedGroups.GetLastValue(7))
    kSMPlayerValueStatWood = 38,             // list entry 38: Language.rc 31208 "Wood" (statistic; GetPlayerValue mSavedGroups.GetLastValue(8))
    kSMPlayerValueStatGold = 39,             // list entry 39: Language.rc 31209 "Gold" (statistic; GetPlayerValue mSavedGroups.GetLastValue(9))
    kSMPlayerValueStatStone = 40,            // list entry 40: Language.rc 31210 "Stone" (statistic; GetPlayerValue mSavedGroups.GetLastValue(10))
    kSMPlayerValueStatIron = 41,             // list entry 41: Language.rc 31211 "Iron" (statistic; GetPlayerValue mSavedGroups.GetLastValue(11))
    kSMPlayerValueStatEpochReached = 42,     // list entry 42: Language.rc 31212 "Epoch Reached" (statistic; GetPlayerValue mSavedGroups.GetLastValue(12))
    kSMPlayerValueStatArmySize = 43,         // list entry 43: Language.rc 31213 "Army Size" (statistic; GetPlayerValue mSavedGroups.GetLastValue(13))
    kSMPlayerValueStatCitizenCount = 44,     // list entry 44: Language.rc 31214 "Citizen Count" (statistic; GetPlayerValue mSavedGroups.GetLastValue(14))
    kSMPlayerValueStatHeroCount = 45,        // list entry 45: Language.rc 31215 "Hero Count" (statistic; GetPlayerValue mSavedGroups.GetLastValue(15))
    kSMPlayerValueStatWonderCount = 46,      // list entry 46: Language.rc 31216 "Wonder Count" (statistic; GetPlayerValue mSavedGroups.GetLastValue(16))
    kSMPlayerValueStatTributeSent = 47,      // list entry 47: Language.rc 31217 "Tribute Sent" (statistic; GetPlayerValue mSavedGroups.GetLastValue(17))
    kSMPlayerValueStatTributeReceived = 48,  // list entry 48: Language.rc 31218 "Tribute Received" (statistic; GetPlayerValue mSavedGroups.GetLastValue(18))
    kSMPlayerValueStatResearchCount = 49,    // list entry 49: Language.rc 31219 "Research Count" (statistic; GetPlayerValue mSavedGroups.GetLastValue(19))
    kSMPlayerValueStatHotKeysPressed = 50,   // list entry 50: Language.rc 31220 "Hot Keys Pressed" (statistic; GetPlayerValue mSavedGroups.GetLastValue(20))
    kSMPlayerValueStatMouseClicks = 51,      // list entry 51: Language.rc 31221 "Mouse Clicks" (statistic; GetPlayerValue mSavedGroups.GetLastValue(21))
    kSMPlayerValueStatResearchAvailable = 52,  // list entry 52: Language.rc 31222 "Research Available" (statistic; GetPlayerValue mSavedGroups.GetLastValue(22))
    kSMPlayerValueStatTotalPopulation = 53,  // list entry 53: Language.rc 31223 "Total Population" (statistic; GetPlayerValue mSavedGroups.GetLastValue(23))
    kSMPlayerValueStatTilesExplored = 54,    // list entry 54: Language.rc 31224 "Tiles Explored" (statistic; GetPlayerValue mSavedGroups.GetLastValue(24))
    kSMPlayerValueStatFutureUse1 = 55,       // list entry 55: Language.rc 31225 "Stat - for future use1" (statistic; GetPlayerValue mSavedGroups.GetLastValue(25))
    kSMPlayerValueStatFutureUse2 = 56,       // list entry 56: Language.rc 31226 "Stat - for future use2" (statistic; GetPlayerValue mSavedGroups.GetLastValue(26))
    kSMPlayerValueStatFutureUse3 = 57,       // list entry 57: Language.rc 31227 "Stat - for future use3" (statistic; GetPlayerValue mSavedGroups.GetLastValue(27))
    kSMPlayerValueStatFutureUse4 = 58,       // list entry 58: Language.rc 31228 "Stat - for future use4" (statistic; GetPlayerValue mSavedGroups.GetLastValue(28))
    kSMPlayerValueStatFutureUse5 = 59,       // list entry 59: Language.rc 31229 "Stat - for future use5" (statistic; GetPlayerValue mSavedGroups.GetLastValue(29))
    kSMPlayerValueStatFutureUse6 = 60,       // list entry 60: Language.rc 31230 "Stat - for future use6" (statistic; GetPlayerValue mSavedGroups.GetLastValue(30))
    kSMPlayerValueMiningLimit = 61,          // list entry 61: Language.rc 29681 "Mining Limit"
    kSMPlayerValuePlaceholder1 = 62,         // list entry 62: Language.rc 29682 "Placeholder1"
    kSMPlayerValuePlaceholder2 = 63,         // list entry 63: Language.rc 29683 "Placeholder2"
    kSMPlayerValuePlaceholder3 = 64,         // list entry 64: Language.rc 29684 "Placeholder3"
    kSMPlayerValuePlaceholder4 = 65,         // list entry 65: Language.rc 29685 "Placholder4"
    kSMPlayerValuePlaceholder5 = 66,         // list entry 66: Language.rc 29686 "Placeholder5"
};

// Set-Attribute Level modes (EESMEffect::SetPlayerAttribute inMode: 0 sets, 2 negates the amount).
enum EESMPlayerValueMode {
    kSMValueModeToExactly = 0,   // Set-Attribute Level mode list 0x733c+i (EESETSETPlayer): Language.rc 29500 (0x733c) "To Exactly"
    kSMValueModeToIncreaseBy = 1,  // Set-Attribute Level mode list 0x733c+i (EESETSETPlayer): Language.rc 29501 (0x733d) "To Increase By"
    kSMValueModeToDecreaseBy = 2,  // Set-Attribute Level mode list 0x733c+i (EESETSETPlayer): Language.rc 29502 (0x733e) "To Decrease By"
};

// Object stance effect values (EESMEffect::mStance; SetUnitsStance 0x5c2811 switch; 5..10 index EEUnitBehavior::sTable).
enum EESMStance {
    kSMStanceExplore = 0,    // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29475 (0x7323) "Explore"
    kSMStanceIdle = 1,       // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29476 (0x7324) "Idle"
    kSMStanceLocked = 2,     // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29477 (0x7325) "Locked"
    kSMStanceUnlocked = 3,   // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29478 (0x7326) "Unlocked"
    kSMStanceTownPatrol = 4,  // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29479 (0x7327) "Town Patrol"
    kSMStanceScout = 5,      // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29480 (0x7328) "Scout"
    kSMStanceStandGround = 6,  // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29481 (0x7329) "Stand Ground"
    kSMStanceGuard = 7,      // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29482 (0x732a) "Guard"
    kSMStanceHarass = 8,     // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29483 (0x732b) "Harass"
    kSMStanceCautious = 9,   // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29484 (0x732c) "Cautious"
    kSMStanceAggressive = 10,  // Stance effect "Stance" list 0x7323+i (EESETSETSetObjectStance): Language.rc 29485 (0x732d) "Aggressive"
};

// Task-to-object/area actions (EESMEffect::mTaskAction); values above kSMTaskActionCalamityBase are calamity classes.
enum EESMTaskAction {
    kSMTaskActionNormal = 0,      // Task-Object/Task-Area "Action" list 0x72bf+i (EESETSETObject mActions.push_back(theIndex - 0x72bf)): Language.rc 29375 (0x72bf) "Normal"
    kSMTaskActionPatrol = 1,      // Task-Object/Task-Area "Action" list 0x72bf+i (EESETSETObject mActions.push_back(theIndex - 0x72bf)): Language.rc 29376 (0x72c0) "Patrol"
    kSMTaskActionAttackMove = 2,  // Task-Object/Task-Area "Action" list 0x72bf+i (EESETSETObject mActions.push_back(theIndex - 0x72bf)): Language.rc 29377 (0x72c1) "Attack-Move"
    kSMTaskActionLoad = 3,        // Task-Object/Task-Area "Action" list 0x72bf+i (EESETSETObject mActions.push_back(theIndex - 0x72bf)): Language.rc 29378 (0x72c2) "Load"
    kSMTaskActionUnload = 4,      // Task-Object/Task-Area "Action" list 0x72bf+i (EESETSETObject mActions.push_back(theIndex - 0x72bf)): Language.rc 29379 (0x72c3) "Unload"
    kSMTaskActionGarrison = 5,    // Task-Object/Task-Area "Action" list 0x72bf+i (EESETSETObject mActions.push_back(theIndex - 0x72bf)): Language.rc 29380 (0x72c4) "Garrison"
    kSMTaskActionCalamityBase = 6,  // calamity task actions are the calamity class mL18 + 6 (EESETSETObject); TaskUnitsTo* test mTaskAction > 6 (provisional name)
};

// Operator linking a trigger condition/effect to the next (EESMTConditionWrapper/EESMTEffectWrapper::mOperator; EESETSTrigger::ShowOperator).
enum EESMTriggerOperator {
    kSMTriggerOperatorEnd = 0,  // EESETSTrigger::ShowOperator case 0 label Language.rc 0x76bd "End"; wrapper ctors store 0; EESMTConditionWrapper::Evaluate case 0 stops
    kSMTriggerOperatorAnd = 1,  // Language.rc 0x76b6 "And" (EESETSTrigger combo line 0); EEUIESaveScenarioTriggersOnly writes " AND" for 1
    kSMTriggerOperatorOr = 2,  // Language.rc 0x76b7 "Or" (combo line 1); EEUIESaveScenarioTriggersOnly writes " OR" for 2
};

// Trigger effect states (EESMEffect::mTriggerState; Execute mType 3).
enum EESMTriggerState {
    kSMTriggerStateOn = 0,      // Trigger effect "State" list 0x73d2+i (EESETSETTrigger GetString(theSelection + 0x73d2)): Language.rc 29650 (0x73d2) "On"
    kSMTriggerStateOff = 1,     // Trigger effect "State" list 0x73d2+i (EESETSETTrigger GetString(theSelection + 0x73d2)): Language.rc 29651 (0x73d3) "Off"
    kSMTriggerStateFired = 2,   // Trigger effect "State" list 0x73d2+i (EESETSETTrigger GetString(theSelection + 0x73d2)): Language.rc 29652 (0x73d4) "Fired"
    kSMTriggerStateNotFired = 3,  // Trigger effect "State" list 0x73d2+i (EESETSETTrigger GetString(theSelection + 0x73d2)): Language.rc 29653 (0x73d5) "Not Fired"
};

// Scenario unit attributes (EESMObject::GetUnitAttribute switch, EEChangeUnit/EEModifyUnitAttribute, EESMEffect::mAttribute; Language.rc 29100+N).
enum EESMUnitAttribute {
    kSMUnitAttributeHitPoints = 0,                    // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29100 (0x71ac) "HitPoints"
    kSMUnitAttributeAnyResource = 1,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29101 (0x71ad) "Any Resource"
    kSMUnitAttributeStone = 2,                        // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29102 (0x71ae) "Stone"
    kSMUnitAttributeGold = 3,                         // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29103 (0x71af) "Gold"
    kSMUnitAttributeWood = 4,                         // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29104 (0x71b0) "Wood"
    kSMUnitAttributeFood = 5,                         // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29105 (0x71b1) "Food"
    kSMUnitAttributeIron = 6,                         // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29106 (0x71b2) "Iron"
    kSMUnitAttributeFlightTime = 7,                   // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29107 (0x71b3) "Flight Time"
    kSMUnitAttributeCargoCount = 8,                   // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29108 (0x71b4) "Cargo Count"
    kSMUnitAttributeGarrisonLevel = 9,                // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29109 (0x71b5) "Garrison Level"
    kSMUnitAttributeAmmunition = 10,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29110 (0x71b6) "Ammunition"
    kSMUnitAttributeAnimationIsPaused = 11,           // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29111 (0x71b7) "Animation Is Paused"
    kSMUnitAttributeReloadTime = 12,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29112 (0x71b8) "Reload Time"
    kSMUnitAttributeGraphicScale = 13,                // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29113 (0x71b9) "Graphic Scale"
    kSMUnitAttributePower = 14,                       // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29114 (0x71ba) "Power"
    kSMUnitAttributeRottingTime = 15,                 // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29115 (0x71bb) "Rotting Time"
    kSMUnitAttributeConstructionComplete = 16,        // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29116 (0x71bc) "Construction Complete"
    kSMUnitAttributeUnitVariable1 = 17,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29117 (0x71bd) "Unit Variable1"
    kSMUnitAttributeUnitVariable2 = 18,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29118 (0x71be) "Unit Variable2"
    kSMUnitAttributeUnitVariable3 = 19,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29119 (0x71bf) "Unit Variable3"
    kSMUnitAttributeShockArmorPlus = 20,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29120 (0x71c0) "ShockArmor+"
    kSMUnitAttributeArrowArmorPlus = 21,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29121 (0x71c1) "ArrowArmor+"
    kSMUnitAttributePierceArmorPlus = 22,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29122 (0x71c2) "PierceArmor+"
    kSMUnitAttributeGunArmorPlus = 23,                // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29123 (0x71c3) "GunArmor+"
    kSMUnitAttributeLaserArmorPlus = 24,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29124 (0x71c4) "LaserArmor+"
    kSMUnitAttributeMissileArmorPlus = 25,            // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29125 (0x71c5) "MissileArmor+"
    kSMUnitAttributeReloadTimePlus = 26,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29126 (0x71c6) "ReloadTime+"
    kSMUnitAttributeAttackPlus = 27,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29127 (0x71c7) "Attack+"
    kSMUnitAttributeWorkRatePlus = 28,                // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29128 (0x71c8) "WorkRate+"
    kSMUnitAttributeFlightTimePlus = 29,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29129 (0x71c9) "FlightTime+"
    kSMUnitAttributeLeadershipPlus = 30,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29130 (0x71ca) "Leadership+"
    kSMUnitAttributeMaxObjectsCarriedPlus = 31,       // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29131 (0x71cb) "MaxObjectsCarried+"
    kSMUnitAttributeRepairPlus = 32,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29132 (0x71cc) "Repair+"
    kSMUnitAttributeHitPointsPlus = 33,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29133 (0x71cd) "HitPoints+"
    kSMUnitAttributeCarryAmountPlus = 34,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29134 (0x71ce) "CarryAmount+"
    kSMUnitAttributeContainerHealingHPPerSecPlus = 35,  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29135 (0x71cf) "ContainerHealingHP/Sec+"
    kSMUnitAttributeRangePlus = 36,                   // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29136 (0x71d0) "Range+"
    kSMUnitAttributeLineOfSightPlus = 37,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29137 (0x71d1) "LineOfSight+"
    kSMUnitAttributeMaxVelocityPlus = 38,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29138 (0x71d2) "MaxVelocity+"
    kSMUnitAttributePowerPlus = 39,                   // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29139 (0x71d3) "Power+"
    kSMUnitAttributeAnimalAIActivePlus = 40,          // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29140 (0x71d4) "Animal AI Active+"
    kSMUnitAttributeCameraFocusPlus = 41,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29141 (0x71d5) "CameraFocus+"
    kSMUnitAttributeTotalShockArmor = 42,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29142 (0x71d6) "Total ShockArmor"
    kSMUnitAttributeTotalArrowArmor = 43,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29143 (0x71d7) "Total ArrowArmor"
    kSMUnitAttributeTotalPierceArmor = 44,            // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29144 (0x71d8) "Total PierceArmor"
    kSMUnitAttributeTotalGunArmor = 45,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29145 (0x71d9) "Total GunArmor"
    kSMUnitAttributeTotalLaserArmor = 46,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29146 (0x71da) "Total LaserArmor"
    kSMUnitAttributeTotalMissileArmor = 47,           // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29147 (0x71db) "Total MissileArmor"
    kSMUnitAttributeTotalReloadTime = 48,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29148 (0x71dc) "Total ReloadTime"
    kSMUnitAttributeTotalAttack = 49,                 // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29149 (0x71dd) "Total Attack"
    kSMUnitAttributeTotalWorkRate = 50,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29150 (0x71de) "Total WorkRate"
    kSMUnitAttributeTotalFlightTime = 51,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29151 (0x71df) "Total FlightTime"
    kSMUnitAttributeTotalLeadership = 52,             // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29152 (0x71e0) "Total Leadership"
    kSMUnitAttributeTotalMaxObjectsCarried = 53,      // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29153 (0x71e1) "Total MaxObjectsCarried"
    kSMUnitAttributeTotalRepair = 54,                 // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29154 (0x71e2) "Total Repair"
    kSMUnitAttributeTotalHitPoints = 55,              // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29155 (0x71e3) "Total HitPoints"
    kSMUnitAttributeTotalCarryAmount = 56,            // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29156 (0x71e4) "Total CarryAmount"
    kSMUnitAttributeTotalHealingHPPerSec = 57,        // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29157 (0x71e5) "Total HealingHP/Sec"
    kSMUnitAttributeTotalRange = 58,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29158 (0x71e6) "Total Range"
    kSMUnitAttributeTotalLineOfSight = 59,            // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29159 (0x71e7) "Total LineOfSight"
    kSMUnitAttributeTotalMaxVelocity = 60,            // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29160 (0x71e8) "Total MaxVelocity"
    kSMUnitAttributeTotalPower = 61,                  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29161 (0x71e9) "Total Power"
    kSMUnitAttributeTotalPlaceHolder2 = 62,           // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29162 (0x71ea) "Total PlaceHolder2"
    kSMUnitAttributeTotalPlaceHolder3 = 63,           // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29163 (0x71eb) "Total PlaceHolder3"
    kSMUnitAttributeTotalTransportWeight = 64,        // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29164 (0x71ec) "Total TransportWeight"
    kSMUnitAttributeTotalWeight = 65,                 // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29165 (0x71ed) "Total Weight"
    kSMUnitAttributeTotalTileSize = 66,               // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29166 (0x71ee) "Total Tile Size"
    kSMUnitAttributeTotalCruiseAltitude = 67,         // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29167 (0x71ef) "Total Cruise Altitude"
    kSMUnitAttributeTotalMinimumRange = 68,           // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29168 (0x71f0) "Total Minimum Range"
    kSMUnitAttributeTotalCanEnterAirTransports = 69,  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29169 (0x71f1) "Total Can Enter Air Transports"
    kSMUnitAttributeTotalCanEnterLandTransports = 70,  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29170 (0x71f2) "Total Can Enter Land Transports"
    kSMUnitAttributeTotalCanEnterSeaTransports = 71,  // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29171 (0x71f3) "Total Can Enter Sea Transports"
    kSMUnitAttributeTotalPlaceHolder1 = 72,           // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29172 (0x71f4) "Total Place Holder1"
    kSMUnitAttributeTotalPlaceHolder2B = 73,          // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29173 (0x71f5) "Total Place Holder2"; B: the text equals entry 62 but for the space in Place Holder
    kSMUnitAttributeTotalPlaceHolder3B = 74,          // unit attribute list 0x71ac+i (EESETSObject/EESETSETObject GetString(i + 0x71ac)): Language.rc 29174 (0x71f6) "Total Place Holder3"; B: the text equals entry 63 but for the space in Place Holder
};

// Unit-attribute change modes (EEChangeUnit inMode: 0 adds, 1 subtracts, 2 sets).
enum EESMUnitAttributeMode {
    kSMAttributeModeIncrease = 0,  // unit attribute operator list 0x7396+i (EESETSETSAAAttributeChange): Language.rc 29590 (0x7396) "Increase"
    kSMAttributeModeDecrease = 1,  // unit attribute operator list 0x7396+i (EESETSETSAAAttributeChange): Language.rc 29591 (0x7397) "Decrease"
    kSMAttributeModeExactly = 2,  // unit attribute operator list 0x7396+i (EESETSETSAAAttributeChange): Language.rc 29592 (0x7398) "Exactly"
};

#endif // EE_EESCENARIOIDS_H
