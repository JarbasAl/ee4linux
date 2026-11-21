// The AI engine a computer player plays: the classic AI, the game's own, or Age of AI. [general] enabled=1 in
// ageofai.ini, in the game folder, makes every computer player an Age of AI player; the classic AI is the default.
//
// Age of AI is Grille's (https://github.com/Grille/Empire-Earth-Age-of-AI, GPL-3.0). Its parts in this folder are
// ported from 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07; each file names the upstream files it comes from.
#ifndef AGEOFAI_AGEOFAI_H
#define AGEOFAI_AGEOFAI_H

#include "AgeOfAIIntel.h"

class EECPBuildManager;
class EEComputerPlayer;
class EETechTreeEpoch;
class EEPlayer;
class EECPResourceManager;
class EECPAttackSubGroup;
class EEComplexUnit;
class EEUnit;
class GE3DPoint;

namespace AgeOfAI {

struct Settings;
struct SettingsAI;

enum Engine { kEngineClassic, kEngineAgeOfAI };

// The engine of the computer player with this player number (1..15). The first call reads ageofai.ini and writes its
// mistakes to the standard error.
Engine EngineOf(unsigned long inPlayerID);

// A player's own settings file, read in full at once, in place of ageofai.ini (the simulator's --player-ini): a named
// AI of the benchmark. GeneralOf and SettingsAIOf give a player's settings, its file's or the shared ones of ageofai.ini. A
// difficulty above hard reads the medium table.
void SetPlayerSettings(unsigned long inPlayerID, const char* inPath);
const Settings& GeneralOf(unsigned long inPlayerID);
const SettingsAI& SettingsAIOf(unsigned long inPlayerID, unsigned long inDifficulty);

// Players that play the classic AI whatever ageofai.ini says: bit n is player n. A game then holds both engines, so
// one game compares them (the simulator's --classic-players).
void SetClassicPlayers(unsigned inMask);

// True when the player's update runs with the scenario flag clear ([general] scenario_ai). A scenario or campaign
// started from the menu sets the flag, and several of the classic AI's routines then skip their planning, so the AI
// only gathers. Editor test-play and saved games leave it clear. dll/engine/scenario_ai.c upstream.
bool ClearsScenarioFlag(unsigned long inPlayerID);

// What an Age of AI player knows of its enemies (AgeOfAIIntel.h), kept per player number. TickIntel takes the
// player's scan when one is due (every 3 game seconds) and [ai_base] honest_vision is on for its difficulty;
// EEComputerPlayer::Update calls it for an Age of AI player.
void TickIntel(EEComputerPlayer* inPlayer, unsigned long inNow);
const Intel::Memory& MemoryOf(unsigned long inPlayerID);
bool SeenBy(EEUnit* inUnit, EEPlayer* inViewer);   // attack.c seen_by: what the player knows of a unit

// Aiming ahead ([general] lead_targets; AgeOfAILead.h). LeadShot moves a plain projectile's aim to where its moving target
// will be when it comes down; EEUnitClass::FireShot calls it. The counts are the shots of plain projectiles and how
// many were led, and the projectiles that came down on a unit and on the ground (the simulator's report).
bool LeadTargetsOn(unsigned long inPlayerID);   // the shooter's owner's [general] lead_targets
void LeadShot(EEComplexUnit* inShooter, EEUnit* inTarget, GE3DPoint& ioAim);
// Homing missiles ([general] lead_missiles, off in the shipped file). LeadMissile moves a missile's steering point to
// where its moving target will be when the missile gets there. EEPMMissile::Home calls it after it finds the drop target.
bool LeadMissilesOn(unsigned long inPlayerID);
void LeadMissile(EEComplexUnit* inMissile, EEUnit* inTarget, GE3DPoint& ioAim);
struct ProjectileCounts {
    unsigned long mFired, mLed, mHitUnit, mHitGround, mMissilesLed;
};
ProjectileCounts& Projectiles();

// Spreading missiles ([general] spread_missiles, on in the shipped file). A launcher whose target is already covered by
// missiles in flight holds its shot or sends it at another enemy of the same family. EEUCMissile::LaunchProjectile and
// EEPMMissile::Home keep the in-flight shares, EEUnitClass::FireShot asks before it finds the drop location.
bool SpreadMissilesOn(unsigned long inPlayerID);
void NoteMissileLaunch(EEComplexUnit* inMissile);
void NoteMissileStep(EEComplexUnit* inMissile, EEUnit* inTarget);
EEUnit* SpreadBeforeFire(EEComplexUnit* inShooter, bool& outHold);   // alternate target, or outHold: keep the shot
struct SpreadCounts {
    unsigned long mLaunched, mHeld, mSwitched;
};
SpreadCounts MissileSpreadCounts();   // the simulator's report

// Lobbing shot by shot ([general] lob_each_shot; AgeOfAILob.h). EEUGAttackUnit calls LobBeforeShot at the start of each
// attack of a unit that may fire over walls.
bool LobEachShotOn(unsigned long inPlayerID);   // the shooter's owner's [general] lob_each_shot
void LobBeforeShot(EEComplexUnit* inShooter, EEUnit* inTarget);

// The early game of an Age of AI player ([ai_base] early_towers, tower_bait): EEComputerPlayer::Update calls it.
void TickEarly(EEComputerPlayer* inPlayer, unsigned long inNow);
struct EarlyCounts {
    unsigned long mQueued, mPlaced, mBuilders, mBaits;   // towers queued, core spots handed out, builders added, citizens sent to bait
};
EarlyCounts EarlyCountsOf(unsigned long inPlayerID);

// The economy of an Age of AI player (AgeOfAIWorkers.h). OnIncome is called from EEComputerPlayer::AddResource, RatesUpdate
// from its update, EconTargets from EECPResourceManager::Update after the game's count of wanted gatherers.
void OnIncome(EEComputerPlayer* inPlayer, long inResource, long inAmount);
void RatesUpdate(EEComputerPlayer* inPlayer, unsigned long inNow);
void EconTargets(EECPResourceManager* inResources);
// The slot tallies of the mines near a drop site (all, and the home continent's), for the fill pass
void MineSlots(EEComputerPlayer* inPlayer, unsigned long inNow, long outSlots[7], long outHomeSlots[7]);
// FILL THE MINES (mines.c mine_fill, off in the shipped file): turn spare wood- and food-gatherers toward metal mines
void MineFill(EECPResourceManager* inResources, unsigned long inNow);
// LATE FOOD TARGET (econ.c late_targets food_to_metals, off in the shipped file): cap the farmers' target while food piles up
void LateFoodTargets(EECPResourceManager* inResources, unsigned long inNow);
// MORE GRANARIES (econ.c more_granaries, [ai_base] more_granaries): ask the build manager for a granary when the food
// target and the finished farms call for one. Called from EECPResourceManager::Update.
void MoreGranaries(EECPResourceManager* inResources, unsigned long inNow);
struct GranaryCounts {
    unsigned long mChecks, mRequested;   // passes run, granary requests made
};
GranaryCounts GranaryCountsOf(unsigned long inPlayerID);
// FISHING BOAT (navy.c hook_fish_type, [ai_base] fishing_boat): the game picks a boat by epoch alone and from the World
// War I age on asks for one it cannot train. PickFishingBoat returns the newest available fishing boat class instead.
// EECPCivilizationManager::GetFishingBoatClass calls it for Age of AI players only.
long PickFishingBoat(EEPlayer* inPlayer, long inGameClass);
struct FishingCounts {
    unsigned long mCalls, mPicked, mChanged;   // gate calls, newest-boat picks, picks that differ from the game's
    long mLastClass;                           // the class last returned
};
FishingCounts FishingCountsOf(unsigned long inPlayerID);
// MINE CAP (citizens.c cz_mine_cap, [ai_base] citizen_mine_cap on by default): the citizens wanted cannot exceed the
// seen mines times the gatherer slots per mine (table 0x16) and a half, nor half the population cap. From the Copper
// Age on, the count is refreshed every 30 game seconds. EECPUnitManager::GetWantedCitizens calls it for Age of AI players.
long MineCapWantedCitizens(EEComputerPlayer* inPlayer, long inCount);
struct MineCapCounts {
    unsigned long mCalls, mCapped, mTime;   // queries seen, queries the cap reduced, last refresh
    long mMines, mCap;                      // last count of seen mines, last cap
};
MineCapCounts MineCapCountsOf(unsigned long inPlayerID);
// PRODUCTION SYNC (econ.c prod_sync, off in the shipped file): repair the in-production count after a game load
void ProductionSync(EEComputerPlayer* inPlayer, unsigned long inNow);
// The best tower of the line the player can build now (0: none), whether the build manager has one queued, and the player's
// towers within a square around a tile
long NewestTowerOf(EEPlayer* inPlayer);
bool TowerIsQueued(EECPBuildManager* inBuild);
void TowerCounts(EEPlayer* inPlayer, long inX, long inY, long inRadius, int& outFinished, int& outAll);
// The defence layout's extra towers (DefenceTick, every 20 game seconds)
void DefenceTick(EEComputerPlayer* inPlayer, unsigned long inNow);
void TickPacing(EEComputerPlayer* inPlayer, unsigned long inNow);
// The game pulls an AI's next-epoch attempt forward when a human reaches an epoch. epoch_pacing=timer goes without it.
bool EpochCatchUpSkipped(EEComputerPlayer* inPlayer);
// Another AI reaching a new epoch pulls this AI's attempt forward too (econ.c catch_up_ais, on in the shipped file).
// Checked every two game seconds; the first sighting of a player records its epoch without a catch-up call.
void CatchUpAIsTick(EEComputerPlayer* inPlayer, unsigned long inNow);
struct CatchUpAIsCounts {
    unsigned long mChecks, mCalls;   // two-second checks, catch-up calls made for a newer AI
};
CatchUpAIsCounts CatchUpAIsCountsOf(unsigned long inPlayerID);
void GatherResearch(EEComputerPlayer* inPlayer, unsigned long inNow);
// An epoch's line upgrades (kind 7): false when the game researches them itself, true when they are scheduled for research
// The game's placement test of a building type on every tile of its footprint, inside the map's border
bool CanPlace(EEPlayer* inPlayer, long inType, long inX, long inY);
// The drop sites by the forests (WoodDrops, called once a game second)
void WoodDrops(EEComputerPlayer* inPlayer, unsigned long inNow);
// The drop site by the home continent's far mine (mines.c mine_drops, mine_fill mine_settlement or citizen_plan
// mine_settlement, both off in the shipped file), called once a game second
void MineDrops(EEComputerPlayer* inPlayer, unsigned long inNow);
// The settings the player's own fields take, once per player on its first update
void ApplySettings(EEComputerPlayer* inPlayer);
void UpgradesRetry(EEComputerPlayer* inPlayer, unsigned long inNow);
bool EpochUpgrades(EEComputerPlayer* inPlayer, EETechTreeEpoch* inEpoch);
// The stone of the core towers still missing (EarlyTowers): the worker plan counts it
long EarlyTowerStone(EEComputerPlayer* inPlayer);

// The mine search of an Age of AI player (sites.c): EECPResourceManager::FindResourceTarget calls it for such a player. The rebalance
// (econ.c, with the mine crowds) runs after each assignment pass.
EEUnit* FindMine(EECPResourceManager* inResources, long inType, EEComplexUnit* inUnit, bool inBackwards);
void MineCrowds(EECPResourceManager* inResources);
void Rebalance(EECPResourceManager* inResources);
// Before the assignment (food.c food_hunt, food_plan hunt_near): idle citizens hunt animals near a drop site while the
// player is in the Stone Age. Off by default.
void FoodHunt(EECPResourceManager* inResources);
// After the rebalance (food.c food_pull): gatherers move to forage bushes while food is 2 or more short of its target
void FoodPull(EECPResourceManager* inResources);
// WILD SETTLEMENT (food_plan wild_settlement, off in the shipped file): a Settlement by rich wild food far from the town
void FoodCamp(EEComputerPlayer* inPlayer, unsigned long inNow);
struct FoodCounts {
    unsigned long mPulled;   // gatherers moved to food
};
FoodCounts FoodCountsOf(unsigned long inPlayerID);

// What is dangerous for the citizens (attack.c): SiteDanger is what the mine, food and wood searches refuse; RaidersKnown is raiders in
// sight or remembered; SafetyTick (every 4 seconds, from the resource manager's update) refreshes the defences seen and the places of
// the citizens' hurts.
bool SiteDanger(EEPlayer* inPlayer, EEUnit* inSite, bool inFood, int inResource);
bool RaidersNear(EEPlayer* inPlayer, long inX, long inY, int inR0);
bool RaidersKnown(EEPlayer* inPlayer, long inX, long inY, int inR0);
// Idle citizens far from the towns walk to the nearest safe one; the base follows the towns (CitizensHome, every 3 game seconds)
void CitizensHome(EEComputerPlayer* inPlayer, unsigned long inNow);
// An enemy army in the base: the defenders gather beyond it, then strike (MusterTick, every 3 game seconds)
void MusterTick(EEComputerPlayer* inPlayer, unsigned long inNow);
// Hold the town ([ai_base] hold_town, off by default): keep home soldiers from trickling out, sortie when strong, hold/retreat when weak
void HoldTownTick(EEComputerPlayer* inPlayer, unsigned long inNow);
// A building with a ground attack, and its range and attack (attack.c defence_stats); a player the stances make an enemy (is_enemy)
bool DefenceStats(EEUnit* inUnit, float& outRange, float& outAttack);
bool IsEnemyOf(EEPlayer* inPlayer, EEPlayer* inOther);

// The outdated units' attack (army.c, AgeOfAIArmy.h): every 5 game minutes, 8 or more land units that no upgrade reaches any
// more attack the enemy building nearest their middle as one of the game's attack groups. EEComputerPlayer::Update calls it.
void OutdatedAttack(EEComputerPlayer* inPlayer, unsigned long inNow);
struct OutdatedCounts {
    unsigned long mSorties, mSent, mRefused, mNoGroup;   // groups made, units in them, launches the combat manager refused, accepted with no group
};
OutdatedCounts OutdatedCountsOf(unsigned long inPlayerID);

// Citizen attacks ([ai_base] citizen_attacks): at the shipped 0 the mod calls off an attack group made at least half of
// citizens, as upstream's citizen_attack_scan; the setting's 1 allows the original citizen attack. EEComputerPlayer::Update
// calls the scan every 5 game seconds. Unlike the other parsed-but-inert rows, this one is ACTIVE at the shipped 0: porting
// it is a deliberate re-rating, since the port had no call-off at all and upstream's shipped setting calls one off.
void CitizenAttackScan(EEComputerPlayer* inPlayer, unsigned long inNow);
struct CitizenAttackCounts {
    unsigned long mCalls, mCalled;   // scans made, citizen groups sent home and disbanded
};
CitizenAttackCounts CitizenAttackCountsOf(unsigned long inPlayerID);

// The game's walls ([ai_base] game_walls): the shipped 0 skips the game's automatic wall ring for an Age of AI player;
// the mod's own walls come through defence_layout's build_wall, not that update. A 1 leaves the game's update running.
// EECPWallManager::Update reads the gate on every call and counts the calls it skips and the calls it runs.
struct WallCounts {
    unsigned long mSkipped, mRan;
};
bool GameWallsOn(unsigned long inPlayerID);   // [ai_base] game_walls for this player's settings
void NoteWallUpdate(unsigned long inPlayerID, bool inSkipped);
WallCounts WallCountsOf(unsigned long inPlayerID);

// Retargeting ([ai_base] attack_retarget, off in the shipped file): an attack group whose target has left the game or
// stopped being an enemy picks the enemy building nearest the group, and the group and its subgroups take that player
// as their new target. EEComputerPlayer::Update calls the scan every game tick; it acts every 10 seconds.
void RetargetTick(EEComputerPlayer* inPlayer, unsigned long inNow);
struct RetargetCounts {
    unsigned long mScans, mPicked, mSubGroups;   // scans made, groups retargeted, subgroups re-pointed
};
RetargetCounts RetargetCountsOf(unsigned long inPlayerID);

// A wave's goal (stage.c wave_focus, [ai_base] wave_focus off in the shipped file): every land route of an attack group
// goes to one enemy building at a time, on the same land as the route, until that building dies or the land changes.
// EECPASGLand::RequestPath calls it before the game costs the route. ioTo carries the goal tile back to the caller.
void WaveFocusRoute(EECPAttackSubGroup* inSubGroup, long inFromX, long inFromY, long& ioToX, long& ioToY);
struct WaveFocusCounts {
    unsigned long mCalls, mSet, mMoved, mKept, mNone;   // routes seen, goals chosen, goals changed, goals already reached, no goal found
};
WaveFocusCounts WaveFocusCountsOf(unsigned long inPlayerID);

// A weak building near a threatened goal (attack.c find_weak_spot, [ai_base] attack_weak_spots on by default): when a
// land route's goal is an enemy building under known defences, the route goes to a seen enemy building of the same land
// mass within attack_weak_spot_radius that no known defence covers. EECPILocation::SetUnit records the new goal.
void WeakSpotRoute(EECPAttackSubGroup* inSubGroup, long inFromX, long inFromY, long& ioToX, long& ioToY);
struct WeakSpotCounts {
    unsigned long mCalls, mThreatened, mPicked;   // routes seen, goals under known defences, goals changed
};
WeakSpotCounts WeakSpotCountsOf(unsigned long inPlayerID);

// The route cost of known enemy defences (attack.c avoid_towers, [ai_base] avoid_towers on by default): a land
// attack route pays extra for each subsector inside their reach. The port costs a subsector where upstream costs a
// tile; the margin and the weight come from the settings. EEAIGPGModifierLandAttackSubGroupRoutePlanner::GetCost
// calls this for an Age of AI player only.
long AvoidTowerRouteCost(unsigned long inPlayerID, long inX, long inY);
struct AvoidTowerCounts {
    unsigned long mCalls, mPenalised, mCost;   // route steps seen, steps in range, cost added
};
AvoidTowerCounts AvoidTowerCountsOf(unsigned long inPlayerID);

// A ram's target is guarded (attack.c guarded and hook_asgu_score, [ai_base] attack_siege): enemy land soldiers within
// 5 tiles of it make the ram wait instead of scoring the target. EECPAttackSubGroupUnit::ScoreTarget calls this for a
// land ram only; AgeOfAIDanger.cpp owns the setting, the engine gate and the counters.
bool RamTargetGuarded(EEPlayer* inPlayer, long inX, long inY);
struct RamWaitCounts {
    unsigned long mCalls, mWaits;   // guarded tests, targets refused
};
RamWaitCounts RamWaitCountsOf(unsigned long inPlayerID);

// Enemy aircraft beyond a land soldier's reach (attack.c hook_asgu_score, [ai_base] attack_ignore_air): the scorer
// refuses them so an attack does not chase planes circling over their own town. In reach they stay targets.
bool AirTargetOutOfReach(EEPlayer* inPlayer, EEComplexUnit* inTarget, EEComplexUnit* inScorer);
struct AirSkipCounts {
    unsigned long mCalls, mSkipped;   // aircraft targets scored, targets refused as out of reach
};
AirSkipCounts AirSkipCountsOf(unsigned long inPlayerID);

// The sum of known defences covering a tile (attack.c threat_at, [ai_base] avoid_towers_margin): the number the
// route cost and the attack leash use. Unlike AvoidTowerRouteCost this is not gated by avoid_towers.
float ThreatAt(unsigned long inPlayerID, long inX, long inY);

// A land attack does not chase an enemy standing in known tower range unless its own goal is there (attack.c
// hook_asgu_score, [ai_base] attack_leash). EEAIGPG calls this with the target and the subgroup goal tiles.
bool LeashSkipsTarget(EEPlayer* inPlayer, long inX, long inY, long inGoalX, long inGoalY);
struct LeashCounts {
    unsigned long mCalls, mSkipped;   // leash tests, enemies left alone
};
LeashCounts LeashCountsOf(unsigned long inPlayerID);

// A land soldier does not pick a target more than army_mass_tether tiles from its subgroup's pack unless the target is
// within its own range + 2 tiles (mass.c mass_tether, [ai_base] army_mass=tether; default off). The pack is the middle
// of the subgroup's own members; upstream uses the members away from base. only_buildings limits the rule to enemy
// buildings and forts. EEAIGPG calls this with the subgroup, the scorer and the target.
bool TetherSkipsTarget(EEPlayer* inPlayer, EECPAttackSubGroup* inGroup, EEComplexUnit* inScorer, EEComplexUnit* inTarget);
struct TetherCounts {
    unsigned long mCalls, mSkipped;   // tether tests, targets refused as too far from the pack
};
TetherCounts TetherCountsOf(unsigned long inPlayerID);

// While the known enemy armies are stronger than army_mass_ratio percent of the player's land army, an attack due soon
// waits, up to army_mass_wait minutes (mass.c ms_wait, [ai_base] army_mass=wait_stronger; default off). The game's next
// attack time is pushed out 20 seconds at a time. EEAIGPG calls this once per AI tick.
void MassWaitTick(EEComputerPlayer* inPlayer, unsigned long inNow);
struct MassWaitCounts {
    unsigned long mChecks, mWaits, mGoes, mWaited;   // due-time checks, new waits, releases, extensions (5 per push, as upstream)
};
MassWaitCounts MassWaitCountsOf(unsigned long inPlayerID);

// Enemy siege weapons and defences going up near the base: soldiers sally, and citizens fight builders and small rushes
// (CounterSiege, every 2 game seconds; docs/ageofai.md "Counter-siege and the citizen militia")
void CounterSiege(EEComputerPlayer* inPlayer, unsigned long inNow);
struct DefenceCounts {
    unsigned long mMilitiaOrders;       // citizens sent at enemies
    unsigned long mMilitiaRefused;      // early militia: fights the citizens kept out of
    unsigned long mMilitiaCaught;       // militia_estimate: fights of citizens caught by faster soldiers
    unsigned long mCounterSiegeOrders;  // soldiers sent at siege weapons and towers going up
    unsigned long mCounterSiegeSkipped; // threats left: too few soldiers and no citizens' answer
};
DefenceCounts DefenceCountsOf(unsigned long inPlayerID);
// Idle citizens home (econ.c citizens_home, AgeOfAIHome.h; CitizensHome above)
// Forage near (food_plan forage_near, food.c): the free bush nearest a drop site that takes food, within the reach of
// the pass, or NULL (EECPResourceManager::FindFarm takes it before the game's first bush in centre order)
EEUnit* FindForage(EECPResourceManager* inResources, bool inNear, EEComplexUnit* inUnit);
struct HomeCounts {
    unsigned long mSent;        // idle citizens sent to a town
    unsigned long mBaseMoved;   // base points moved to a town
};
HomeCounts HomeCountsOf(unsigned long inPlayerID);
// The extra towers of the defence layout (defence.c, AgeOfAITowers.h; DefenceTick above)
struct TowerLayoutCounts {
    unsigned long mQueued;      // towers queued
    unsigned long mSkipped;     // looks that queued none: nothing to build, one already queued, or no spot the player can pay for
};
TowerLayoutCounts TowerLayoutCountsOf(unsigned long inPlayerID);
void SafetyTick(EEComputerPlayer* inPlayer, unsigned long inNow);
bool RefuseRepair(EEComputerPlayer* inPlayer, EEComplexUnit* inCitizen, EEUnit* inBuilding);
// An attack goal for inUnit, as unit_goal with goal_attack (attack.c) gives it; false: refused (dead, off the map, a
// unit with no goals, or disabled)
bool AttackUnit(EEUnit* inUnit, EEUnit* inTarget);

// Counter-fire (counterfire.c, AgeOfAICounterFire.h): soldiers sent at enemies that shoot from beyond their victims'
// reach. CounterFire is called every tick and acts every 2 game seconds; CounterFireUnit says whether a unit is
// committed to it.
void CounterFire(EEComputerPlayer* inPlayer, unsigned long inNow);
bool CounterFireUnit(EEPlayer* inPlayer, EEUnit* inUnit, unsigned long inNow);
struct CounterFireCounts {
    unsigned long mSorties, mSent, mShort;   // targets attacked, soldiers sent, targets left because the soldiers were too weak
};
CounterFireCounts CounterFireCountsOf(unsigned long inPlayerID);

// Clears inFlag for the scope's life when inClear, and gives it back its value after
class ScenarioFlagScope {
public:
    ScenarioFlagScope(bool& inFlag, bool inClear) : mFlag(inFlag), mSaved(inFlag)
    {
        if (inClear)
            inFlag = false;
    }
    ~ScenarioFlagScope() { this->mFlag = this->mSaved; }
    ScenarioFlagScope(const ScenarioFlagScope&) = delete;
    ScenarioFlagScope& operator=(const ScenarioFlagScope&) = delete;

private:
    bool& mFlag;
    bool mSaved;
};

}

#endif
