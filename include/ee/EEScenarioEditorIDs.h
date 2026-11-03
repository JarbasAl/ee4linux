// ee/EEScenarioEditorIDs.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_EESCENARIOEDITORIDS_H
#define EE_EESCENARIOEDITORIDS_H

// Scenario editor main tabs / tab groups (EEUIFScenarioEditor::mTabGroups index, CreateTabGroup 0x59ef5a; sEditorTabs 0x8c46d0 help strings).
enum EESEEditorTabGroup {
    kEditorTabMap = 0,         // sEditorTabs[0] help Language.rc 0x213a 'Map'; CreateTabGroup new EEUIFSEMap
    kEditorTabElevation = 1,   // help 0x213b 'Elevation'; new EEUIFSEElevation
    kEditorTabTerrain = 2,     // help 0x213c 'Terrain'; new EEUIFSETerrain
    kEditorTabUnits = 3,       // help 0x213d 'Units'; new EESEUnit
    kEditorTabTriggers = 4,    // help 0x213e 'Triggers'; new EESETriggerScreen
    kEditorTabPlayers = 5,     // help 0x1b61 'Players'; new EESEPlayerScreen
    kEditorTabInstructions = 6,  // help 0x2143 'Story/Instructions'; new EESEInstructions
};

// Scenario editor form entity groups of the trigger sub-screens (EESETriggerScreen::mCurrentEntityGroup; ShowTab 0x57c467 pairs each with its tab; each sub-screen's AddControl(.., group)).
enum EESEEntityGroup {
    kEntityGroupTrigger = 9,   // ShowTab case 0x7d0 (tab 'Trigger'); EESETSTrigger adds its controls to group 9
    kEntityGroupObject = 10,   // ShowTab case 0x7d3 (tab 'Object'); EESETSObject adds its controls to group 10
    kEntityGroupArea = 11,     // ShowTab case 0x7d4 (tab 'Area'); EESETSArea adds its controls to group 11
    kEntityGroupCondition = 12,  // ShowTab case 0x7d1 (tab 'Condition'); EESETSCondition/EESETSConditionControls add to group 12
    kEntityGroupEffect = 13,   // ShowTab case 0x7d2 (tab 'Effect'); EESETSEffect/EESETSEffectControls add to group 13
};

// Messages posted to the scenario editor's load thread (EESELoadThread::Post* -> HandleMessage 0x57285c). Names are
// provisional from the handlers.
enum EESELoadThreadMessage {
    kLoadThreadCreateGhostUnit = 0x402,   // Handle402 loads the unit class media, EEGhostUnit::Create at the mouse, unloads; posted by EESEUnit (unit list selection) (provisional name)
    kLoadThreadStartScenarioTest = 0x404,  // handler calls gUserInterface->StartScenarioTest(); posted by ExitEditor(true) while mInScenarioEditor (provisional name)
    kLoadThreadStartScenarioTest2 = 0x405,  // handler calls gUserInterface->StartScenarioTest2(); posted by ExitEditor(true) when not mInScenarioEditor (provisional name)
    kLoadThreadLoadEpochMedia = 0x406,    // Handle406 loads the first n tech nodes' media with 'Loading %s Epoch' (0x7659, names 0x75c1 + i) (provisional name)
    kLoadThreadUnloadEpochMedia = 0x407,  // Handle407 unloads the first n tech nodes' media (counterpart of 0x406) (provisional name)
    kLoadThreadLoadTerrain = 0x408,       // Handle408(terrain, false): EETerrainTable::Load + its unit classes' media (0x7b27); posted by EEUIFSETerrain (provisional name)
    kLoadThreadUnloadTerrain = 0x409,     // Handle408(terrain, true): EETerrainTable::Unload + unit class media unload (provisional name)
    kLoadThreadMessageQueueDone = 0x40a,  // handler sets mMessageQueueDone (no poster found in the tree) (provisional name)
    kLoadThreadPostEvent = 0x40b,         // handler gUserInterface->PostEvent(param, 0) and dereferences it; EEUIFScenarioEditor::PostLoadEvent (provisional name)
    kLoadThreadShowLoadedObject = 0x40c,  // handler gUserInterface->ShowLoadedObject(param); EEUIFScenarioEditor::PostShowLoadedObject (provisional name)
};

// Player screen pages (EESEPlayerScreen::mTabs index, Fn1761C8 0x5761c8 inPage, EESEPlayerRow::Show 0x579d63 inPage); EESETab::Check 0x5731b0 maps each tab id to its page.
enum EESEPlayerScreenPage {
    kPlayerScreenPlayerPage1 = 0,  // Language.rc 0x5027 'Player Page 1' (tab id 0x12c -> Fn1761C8(0))
    kPlayerScreenPlayerPage2 = 1,  // Language.rc 0x5028 'Player Page 2' (tab id 0x15d -> Fn1761C8(1))
    kPlayerScreenAISettings = 2,  // Language.rc 0x5055 'AI Settings' (tab id 0x170 -> Fn1761C8(2))
    kPlayerScreenTechTree = 3,   // Language.rc 0x5029 'Tech Tree' (tab id 0x12d -> Fn1761C8(3))
    kPlayerScreenOptions = 4,    // Language.rc 0x5042 'Options' (tab id 0x15a -> Fn1761C8(4))
};

// Control ids of the player screen's tab buttons (EESETab ctor 0x573060 picks the label by id).
enum EESEPlayerTabControlID {
    kPlayerTabPlayerPage1 = 0x12c,  // EESETab ctor case 0x12c: Language.rc 0x5027 'Player Page 1'
    kPlayerTabTechTree = 0x12d,   // EESETab ctor case 0x12d: Language.rc 0x5029 'Tech Tree'
    kPlayerTabOptions = 0x15a,    // EESETab ctor case 0x15a: Language.rc 0x5042 'Options'
    kPlayerTabPlayerPage2 = 0x15d,  // EESETab ctor case 0x15d: Language.rc 0x5028 'Player Page 2'
    kPlayerTabAISettings = 0x170,  // EESETab ctor case 0x170: Language.rc 0x5055 'AI Settings'
};

// Control ids of the player screen's tech tree move buttons (EESEBSelect ctor: button text by id; Select 0x572f3b moves between list 0x154 and 0x155).
enum EESESelectButtonID {
    kSelectButtonMoveRight = 0x156,   // button text '>'; Select moves the selected entry of list 0x154 (theLeft true), repeats while held (provisional name)
    kSelectButtonMoveLeft = 0x157,    // button text '<'; Select moves the selected entry of list 0x155 back, repeats while held (provisional name)
    kSelectButtonMoveAllRight = 0x158,  // button text '>>'; Select moves every entry of list 0x154 (provisional name)
    kSelectButtonMoveAllLeft = 0x159,  // button text '<<'; Select moves every entry of list 0x155 (provisional name)
};

// The player screen's tech-tree list filter radio buttons (EESEPlayerScreen::mTypeButtons index; labels Language.rc 0x5033 + index; Fn1760AD 0x5760ad picks the tech list).
enum EESETechListType {
    kSETechListUnits = 0,       // Language.rc 0x5033 'Units' (button 0x14a)
    kSETechListBuildings = 1,   // Language.rc 0x5034 'Buildings' (button 0x14b)
    kSETechListHeroes = 2,      // Language.rc 0x5035 'Heroes' (button 0x14c)
    kSETechListTechnologies = 3,  // Language.rc 0x5036 'Technologies' (button 0x14d; Fn1760AD's fallback list)
};

// Control ids of the scenario editor tool bar buttons (sToolbarButtons 0x8c47b0; CreateToolbarButtons 0x59f0b1 creates each button's event).
enum EESEToolbarButtonID {
    kToolbarButtonLoad = 10,     // sToolbarButtons help Language.rc 0x1b58 'Load'; new EESEELoadScenario
    kToolbarButtonSave = 11,     // help 0x1b59 'Save'; new EESEESaveScenario
    kToolbarButtonQuickLoad = 12,  // help 0x2140 'Quick Load'; new EESEEQuickLoad
    kToolbarButtonQuickSave = 13,  // help 0x2141 'Quick Save'; new EESEEQuickSave
    kToolbarButtonTest = 14,     // help 0x2142 'Test'; new EESEETestScenario
    kToolbarButtonExit = 15,     // help 0x213f 'Exit'; new EESEEExitScenario
};

// Trigger editor entry kinds: the EESETriggerScreen screen / tab index (mScreens, mTabs, mCurrentScreen; CreateScreen 0x57c1e5) and the kind of list a scenario combo box shows or belongs to (EESETSEScenarioComboBox mMode/mLE4, EESETSTriggerUIComboBox mKind). Tab labels Language.rc 0x765b + kind.
enum EESETriggerScreenKind {
    kSETriggerScreenTrigger = 0,  // Language.rc 0x765b 'Trigger'; CreateScreen case 0 new EESETSTrigger
    kSETriggerScreenCondition = 1,  // Language.rc 0x765c 'Condition'; CreateScreen case 1 new EESETSCondition
    kSETriggerScreenEffect = 2,   // Language.rc 0x765d 'Effect'; CreateScreen case 2 new EESETSEffect
    kSETriggerScreenObject = 3,   // Language.rc 0x765e 'Object'; CreateScreen case 3 new EESETSObject
    kSETriggerScreenArea = 4,     // Language.rc 0x765f 'Area'; CreateScreen case 4 new EESETSArea
};

// Control ids of the trigger editor's five tab radio buttons (EESETriggerScreen ctor: new UIRadioButton(2000 + kind, string 0x765b + kind); ShowTab 0x57c467 switches on them).
enum EESETriggerTabControlID {
    kSETriggerTabTrigger = 0x7d0,  // ctor UIRadioButton(2000 + 0, Language.rc 0x765b 'Trigger'); ShowTab -> screen 0
    kSETriggerTabCondition = 0x7d1,  // ctor UIRadioButton(2000 + 1, Language.rc 0x765c 'Condition'); ShowTab -> screen 1
    kSETriggerTabEffect = 0x7d2,   // ctor UIRadioButton(2000 + 2, Language.rc 0x765d 'Effect'); ShowTab -> screen 2
    kSETriggerTabObject = 0x7d3,   // ctor UIRadioButton(2000 + 3, Language.rc 0x765e 'Object'); ShowTab -> screen 3
    kSETriggerTabArea = 0x7d4,     // ctor UIRadioButton(2000 + 4, Language.rc 0x765f 'Area'); ShowTab -> screen 4
};

// Unit tab list sort modes (EESEUnit::GetSortMode: the Alpha/Epoch/Family radio buttons; EEUIFSELBUnitList::SetSortMode 0x599d49 comparators).
enum EESEUnitSortMode {
    kUnitSortAlpha = 0,  // GetSortMode: radio 0x3ee Language.rc 0x5038 'Alpha'; name comparator
    kUnitSortEpoch = 1,  // radio 0x3ef Language.rc 0x5039 'Epoch'; EESEUnitEpochLess
    kUnitSortFamily = 2,  // radio 0x3f0 Language.rc 0x503a 'Family'; EESEUnitLayerLess (mL1C8 = family)
};

// Unit tab unit type filter (EEUIFSEBUnitType::Check sets sUnitTypeMode; EESEUnit::RefreshUnitList filters by it; sUnitTypeButtons 0x8c4458 help strings).
enum EESEUnitTypeMode {
    kUnitPaletteLandUnits = 0,  // button 0x406 help Language.rc 8501 'Land Units'; RefreshUnitList mDomains & 2
    kUnitPaletteAirUnits = 1,  // button 0x407 help 8505 'Air Units'; RefreshUnitList mDomains & 1 or & 8
    kUnitPaletteNavalUnits = 2,  // button 0x408 help 8502 'Naval Units'; RefreshUnitList mDomains & 4
    kUnitPaletteStructures = 3,  // button 0x409 help 8504 'Structures'; RefreshUnitList IsLandBuilding
    kUnitPaletteHeroes = 4,    // button 0x40a help 8503 'Heroes'; RefreshUnitList hero category
    kUnitPaletteResources = 5,  // button 0x40b help 8500 'Resources'; RefreshUnitList remainder
};

// EESETSUICBPlayer::mFlags (+0xe0): the entries FillList 0x597014 shows, one per bit (the entry's string)
enum EESETSPlayerListFlag {
    kPlayerListWorld = 1,             // FillList: IDS_TRIGGER_PLAYER_WORLD
    kPlayerListAnyHuman = 2,          // FillList: IDS_ANY_HUMAN
    kPlayerListAnyComputer = 4,       // FillList: IDS_ANY_COMPUTER
    kPlayerListAllHuman = 8,          // FillList: IDS_ALL_HUMAN
    kPlayerListAllComputer = 0x10,    // FillList: IDS_ALL_COMPUTER
    kPlayerListAnyPlayer = 0x20,      // FillList: IDS_ANY_PLAYER
    kPlayerListAnyExcludeWorld = 0x40,  // FillList: IDS_ANY_EXCLUDE_WORLD
    kPlayerListPlayers = 0x80,        // FillList: IDS_TRIGGER_PLAYER once per player (1..mPlayer)
};

// Control ids (UIForm::GetControlById, dbuicontrols id +0x28c) of the civilization-builder forms; names from the
// dbuicontrols names ("CB " dropped).
enum EECivBuilderControlID {
    kCivBuilderIdCivNameMainTitle = 0xc4,       // dbuicontrols form 7 id 196 "CB Civ Name (Main Title)"
    kCivBuilderIdCivPtsLeftLabel = 0xd1,        // dbuicontrols form 7 id 209 "CB Civ Pts Left Label"
    kCivBuilderIdCivPtsLeft = 0xd2,             // dbuicontrols form 7 id 210 "CB Civ Pts Left (#)"
    kCivBuilderIdNewCivNameTextField = 0x19a,   // dbuicontrols form 15 id 410 "CB New Civ: Name Text Field"
    kCivBuilderIdSaveCivFileListBox = 0x1a4,    // dbuicontrols form 16 id 420 "CB Save Civ: File List Box"
    kCivBuilderIdLoadCivFileListBox = 0x1a9,    // dbuicontrols form 17 id 425 "CB Load Civ: File List Box"
    kCivBuilderIdCivAcceptChangesButton = 0x1c0,  // dbuicontrols form 7 id 448 "CB Civ Accept Changes Button"
    kCivBuilderIdSaveCivNameTextField = 0x232,  // dbuicontrols form 16 id 562 "CB Save Civ: Name Text Field"
};

// Control ids of the EEUIFSEElevation panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESEElevationControlID {
    kSEElevationRaiseButton = 0x96,    // EEUIFSEElevation.cpp:141 mRaiseButton = new UIRadioButton(id 0x96) (provisional name)
    kSEElevationLowerButton = 0x97,    // EEUIFSEElevation.cpp:154 mLowerButton = new UIRadioButton(id 0x97) (provisional name)
    kSEElevationElevationTitle = 0x98,  // EEUIFSEElevation.cpp:180 mElevationTitle = new UITextLabel(id 0x98) (provisional name)
    kSEElevationStrengthTitle = 0x99,  // EEUIFSEElevation.cpp:192 mStrengthTitle = new UITextLabel(id 0x99) (provisional name)
    kSEElevationElevationSlider = 0x9d,  // EEUIFSEElevation.cpp:216 mElevationSlider = new EEUIFSEESElevation(id 0x9d) (provisional name)
    kSEElevationStrengthLabel = 0x9f,  // EEUIFSEElevation.cpp:205 mStrengthLabel = new UITextLabel(id 0x9f) (provisional name)
    kSEElevationLevelButton = 0xa0,    // EEUIFSEElevation.cpp:166 mLevelButton = new UIRadioButton(id 0xa0) (provisional name)
    kSEElevationPaint = 0xa1,          // EEUIFSEElevation.cpp:135 mPaint = new EEUIFSEPaintElevation(id 0xa1) (provisional name)
    kSEElevationStrengthSlider = 0xa2,  // EEUIFSEElevation.cpp:227 mStrengthSlider = new UISlider(id 0xa2) (provisional name)
};

// Control ids of the EESEInstructions panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESEInstructionsControlID {
    kSEInstructionsLocalizationEditBox = 0x45b,  // EESEInstructions.cpp:354 mLocalizationEditBox = new UIEditBox(id 0x45b) (provisional name)
};

// Control ids of the EEUIFSEMap panel (scenario editor): the ids its controls are built with (the ctor id argument);
// names from the member each control is stored in (provisional names).
enum EESEMapControlID {
    kSEMapDefaultTerrainLabel = 0x64,  // EEUIFSEMap.cpp:296 mDefaultTerrainLabel = new UITextLabel(id 0x64) (provisional name)
    kSEMapWidthLabel = 0x66,         // EEUIFSEMap.cpp:242 mWidthLabel = new UITextLabel(id 0x66) (provisional name)
    kSEMapHeightLabel = 0x67,        // EEUIFSEMap.cpp:254 mHeightLabel = new UITextLabel(id 0x67) (provisional name)
    kSEMapMapStyleLabel = 0x68,      // EEUIFSEMap.cpp:311 mMapStyleLabel = new UITextLabel(id 0x68) (provisional name)
    kSEMapDefaultTerrain = 0x69,     // EEUIFSEMap.cpp:353 mDefaultTerrain = new UIComboBox(id 0x69) (provisional name)
    kSEMapMapStyle = 0x6a,           // EEUIFSEMap.cpp:328 mMapStyle = new UIComboBox(id 0x6a) (provisional name)
    kSEMapWidth = 0x6b,              // EEUIFSEMap.cpp:271 mWidth = new UIComboBox(id 0x6b) (provisional name)
    kSEMapHeight = 0x6c,             // EEUIFSEMap.cpp:276 mHeight = new UIComboBox(id 0x6c) (provisional name)
    kSEMapSeedEditBox = 0x70,        // EEUIFSEMap.cpp:370 mSeedEditBox = new UIEditBox(id 0x70) (provisional name)
    kSEMapSeedLabel = 0x71,          // EEUIFSEMap.cpp:383 mSeedLabel = new UITextLabel(id 0x71) (provisional name)
    kSEMapRandomMapSize = 0x72,      // EEUIFSEMap.cpp:470 mRandomMapSize = new UIComboBox(id 0x72) (provisional name)
    kSEMapRandomMapSizeLabel = 0x73,  // EEUIFSEMap.cpp:450 mRandomMapSizeLabel = new UITextLabel(id 0x73) (provisional name)
    kSEMapElevationFileLabel = 0x74,  // EEUIFSEMap.cpp:493 mElevationFileLabel = new UITextLabel(id 0x74) (provisional name)
    kSEMapElevationFile = 0x75,      // EEUIFSEMap.cpp:506 mElevationFile = new EEUIFileComboBox(id 0x75) (provisional name)
    kSEMapLandCutoffLabel = 0x76,    // EEUIFSEMap.cpp:602 mLandCutoffLabel = new UITextLabel(id 0x76) (provisional name)
    kSEMapLandHeightLabel = 0x77,    // EEUIFSEMap.cpp:576 mLandHeightLabel = new UITextLabel(id 0x77) (provisional name)
    kSEMapLandCutoff = 0x78,         // EEUIFSEMap.cpp:614 mLandCutoff = new UIEditBox(id 0x78) (provisional name)
    kSEMapLandHeight = 0x79,         // EEUIFSEMap.cpp:588 mLandHeight = new UIEditBox(id 0x79) (provisional name)
    kSEMapWaterCutoffLabel = 0x7a,   // EEUIFSEMap.cpp:550 mWaterCutoffLabel = new UITextLabel(id 0x7a) (provisional name)
    kSEMapWaterDepthLabel = 0x7b,    // EEUIFSEMap.cpp:524 mWaterDepthLabel = new UITextLabel(id 0x7b) (provisional name)
    kSEMapWaterDepth = 0x7c,         // EEUIFSEMap.cpp:536 mWaterDepth = new UIEditBox(id 0x7c) (provisional name)
    kSEMapWaterCutoff = 0x7d,        // EEUIFSEMap.cpp:562 mWaterCutoff = new UIEditBox(id 0x7d) (provisional name)
};

// Control ids of the EESERBUnitTypes panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESERBUnitTypesControlID {
    kSERBUnitTypesTechList = 0x154,           // EESERBUnitTypes.cpp:647 mTechList = new EESEUITechListBox(id 0x154) (provisional name)
    kSERBUnitTypesOtherTechList = 0x155,      // EESERBUnitTypes.cpp:682 mOtherTechList = new EESEUITechListBox(id 0x155) (provisional name)
    kSERBUnitTypesAdd = 0x156,                // EESERBUnitTypes.cpp:658 mAdd = new EESEBSelect(id 0x156) (provisional name)
    kSERBUnitTypesRemove = 0x157,             // EESERBUnitTypes.cpp:664 mRemove = new EESEBSelect(id 0x157) (provisional name)
    kSERBUnitTypesAddAll = 0x158,             // EESERBUnitTypes.cpp:670 mAddAll = new EESEBSelect(id 0x158) (provisional name)
    kSERBUnitTypesRemoveAll = 0x159,          // EESERBUnitTypes.cpp:676 mRemoveAll = new EESEBSelect(id 0x159) (provisional name)
    kSERBUnitTypesDisplayEpoch = 0x15e,       // EESERBUnitTypes.cpp:694 mDisplayEpoch = new UICheckBox(id 0x15e) (provisional name)
    kSERBUnitTypesPerPlayerCountLabel = 0x162,  // EESERBUnitTypes.cpp:1233 mPerPlayerCountLabel = new UITextLabel(id 0x162) (provisional name)
};

// Control ids of the EESETSCondition panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESETSConditionControlID {
    kSETSConditionTitleLabel = 0x8fc,  // EESETSCondition.cpp:99 mTitleLabel = new UITextLabel(id 0x8fc) (provisional name)
    kSETSConditionTypeComboBox = 0x8fd,  // EESETSCondition.cpp:112 mTypeComboBox = new UIComboBox(id 0x8fd) (provisional name)
};

// Control ids of the EESETSEComboBox panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESETSEComboBoxControlID {
    kSETSEComboBoxTextEditBox = 0x983,  // EESETSEComboBox.cpp:191 mTextEditBox = new UIEditBox(id 0x983) (provisional name)
};

// Control ids of the EESETSEffect panel (scenario editor): the ids its controls are built with (the ctor id argument);
// names from the member each control is stored in (provisional names).
enum EESETSEffectControlID {
    kSETSEffectTitleLabel = 0x960,  // EESETSEffect.cpp:110 mTitleLabel = new UITextLabel(id 0x960) (provisional name)
    kSETSEffectTypeComboBox = 0x961,  // EESETSEffect.cpp:123 mTypeComboBox = new UIComboBox(id 0x961) (provisional name)
};

// Control ids of the EESETSObject panel (scenario editor): the ids its controls are built with (the ctor id argument);
// names from the member each control is stored in (provisional names).
enum EESETSObjectControlID {
    kSETSObjectUnitsRadio = 0x834,     // EESETSObject.cpp:168 mUnitsRadio = new UIRadioButton(id 0x834) (provisional name)
    kSETSObjectAreaCheck = 0x846,      // EESETSObject.cpp:400 mAreaCheck = new UITriStateCheckBox(id 0x846) (provisional name)
    kSETSObjectNearCheck = 0x848,      // EESETSObject.cpp:430 mNearCheck = new UITriStateCheckBox(id 0x848) (provisional name)
    kSETSObjectVisibleByCheck = 0x84e,  // EESETSObject.cpp:563 mVisibleByCheck = new UITriStateCheckBox(id 0x84e) (provisional name)
    kSETSObjectHasStateCheck = 0x850,  // EESETSObject.cpp:593 mHasStateCheck = new UITriStateCheckBox(id 0x850) (provisional name)
    kSETSObjectSelectedByCheck = 0x852,  // EESETSObject.cpp:623 mSelectedByCheck = new UITriStateCheckBox(id 0x852) (provisional name)
    kSETSObjectSelectUnits = 0x855,    // EESETSObject.cpp:161 mSelectUnits = new EESEUICTriggerSelectUnits(id 0x855) (provisional name)
};

// Control ids of the EESETSTrigger panel (scenario editor): the ids its controls are built with (the ctor id argument);
// names from the member each control is stored in (provisional names).
enum EESETSTriggerControlID {
    kSETSTriggerSecondsLabel = 0x9cb,            // EESETSTrigger.cpp:179 mSecondsLabel = new UITextLabel(id 0x9cb) (provisional name)
    kSETSTriggerPrevConditionsButton = 0x9dc,    // EESETSTrigger.cpp:433 mPrevConditionsButton = new UIButton(id 0x9dc) (provisional name)
    kSETSTriggerNextConditionsButton = 0x9dd,    // EESETSTrigger.cpp:442 mNextConditionsButton = new UIButton(id 0x9dd) (provisional name)
    kSETSTriggerPrevEffectsButton = 0x9de,       // EESETSTrigger.cpp:451 mPrevEffectsButton = new UIButton(id 0x9de) (provisional name)
    kSETSTriggerNextEffectsButton = 0x9df,       // EESETSTrigger.cpp:460 mNextEffectsButton = new UIButton(id 0x9df) (provisional name)
    kSETSTriggerConditionDurationEditBox = 0x9e2,  // EESETSTrigger.cpp:491 mConditionDurationEditBox = new UIEditBox(id 0x9e2) (provisional name)
};

// Control ids of the EEUIFSETerrain panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESETerrainControlID {
    kSETerrainTerrainButton = 0xc8,        // EEUIFSETerrain.cpp:365 mTerrainButton = new EEUIFSETRBPaintType(id 0xc8) (provisional name)
    kSETerrainBitmap = 0xc9,               // EEUIFSETerrain.cpp:457 mBitmap = new UIBitmap(id 0xc9) (provisional name)
    kSETerrainMountainsButton = 0xca,      // EEUIFSETerrain.cpp:374 mMountainsButton = new EEUIFSETRBPaintType(id 0xca) (provisional name)
    kSETerrainTerrainTypeLabel = 0xcb,     // EEUIFSETerrain.cpp:383 mTerrainTypeLabel = new UITextLabel(id 0xcb) (provisional name)
    kSETerrainTerrainType = 0xcc,          // EEUIFSETerrain.cpp:417 mTerrainType = new EESEUITerrainTypeLB(id 0xcc) (provisional name)
    kSETerrainTerrainColorTypeLabel = 0xce,  // EEUIFSETerrain.cpp:393 mTerrainColorTypeLabel = new UITextLabel(id 0xce) (provisional name)
    kSETerrainTerrainColorType = 0xcf,     // EEUIFSETerrain.cpp:437 mTerrainColorType = new EESEUITerrainColorLB(id 0xcf) (provisional name)
};

// Control ids of the EESETriggerScreen panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EESETriggerScreenControlID {
    kSETriggerScreenDuplicateButton = 0x7d6,  // EESETriggerScreen.cpp:169 mDuplicateButton = new UIButton(id 0x7d6) (provisional name)
    kSETriggerScreenDeleteButton = 0x7d7,   // EESETriggerScreen.cpp:183 mDeleteButton = new UIButton(id 0x7d7) (provisional name)
    kSETriggerScreenNameEditBox = 0x7db,    // EESETriggerScreen.cpp:254 mNameEditBox = new UIEditBox(id 0x7db) (provisional name)
    kSETriggerScreenNameLabel = 0x7dc,      // EESETriggerScreen.cpp:244 mNameLabel = new UITextLabel(id 0x7dc) (provisional name)
};

// Control ids of the EEUIFScenarioEditor panel (scenario editor): the ids its controls are built with (the ctor id
// argument); names from the member each control is stored in (provisional names).
enum EEScenarioEditorControlID {
    kScenarioEditorHideUICheckBox = 0x12,  // EEUIFScenarioEditor.cpp:436 mHideUICheckBox = new UICheckBox(id 0x12) (provisional name)
};

#endif // EE_EESCENARIOEDITORIDS_H
