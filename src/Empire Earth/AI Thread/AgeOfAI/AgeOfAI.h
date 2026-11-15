// The AI engine a computer player plays: the classic AI, the game's own, or Age of AI. [general] enabled=1 in
// ageofai.ini, in the game folder, makes every computer player an Age of AI player; the classic AI is the default.
//
// Age of AI is Grille's (https://github.com/Grille/Empire-Earth-Age-of-AI, GPL-3.0). Its parts in this folder are
// ported from 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07; each file names the upstream files it comes from.
#ifndef AGEOFAI_AGEOFAI_H
#define AGEOFAI_AGEOFAI_H

#include "AgeOfAIIntel.h"

class EEComputerPlayer;
class EETechTreeEpoch;
class EEPlayer;
class EECPResourceManager;
class EEComplexUnit;
class EEUnit;
class GE3DPoint;

namespace AgeOfAI {

enum Engine { kEngineClassic, kEngineAgeOfAI };

// The engine of the computer player with this player number (1..15). The first call reads ageofai.ini and writes its
// mistakes to the standard error.
Engine EngineOf(unsigned long inPlayerID);

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
bool LeadTargetsOn();
void LeadShot(EEComplexUnit* inShooter, EEUnit* inTarget, GE3DPoint& ioAim);
struct ProjectileCounts {
    unsigned long mFired, mLed, mHitUnit, mHitGround;
};
ProjectileCounts& Projectiles();

// Lobbing shot by shot ([general] lob_each_shot; AgeOfAILob.h). EEUGAttackUnit calls LobBeforeShot at the start of each
// attack of a unit that may fire over walls.
bool LobEachShotOn();
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
void TickPacing(EEComputerPlayer* inPlayer, unsigned long inNow);
void GatherResearch(EEComputerPlayer* inPlayer, unsigned long inNow);
// An epoch's line upgrades (kind 7): false when the game researches them itself, true when they are scheduled for research
// The game's placement test of a building type on every tile of its footprint, inside the map's border
bool CanPlace(EEPlayer* inPlayer, long inType, long inX, long inY);
// The drop sites by the forests (WoodDrops, called once a game second)
void WoodDrops(EEComputerPlayer* inPlayer, unsigned long inNow);
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

// What is dangerous for the citizens (attack.c): SiteDanger is what the mine, food and wood searches refuse; RaidersKnown is raiders in
// sight or remembered; SafetyTick (every 4 seconds, from the resource manager's update) refreshes the defences seen and the places of
// the citizens' hurts.
bool SiteDanger(EEPlayer* inPlayer, EEUnit* inSite, bool inFood, int inResource);
bool RaidersNear(EEPlayer* inPlayer, long inX, long inY, int inR0);
bool RaidersKnown(EEPlayer* inPlayer, long inX, long inY, int inR0);
// An enemy army in the base: the defenders gather beyond it, then strike (MusterTick, every 3 game seconds)
void MusterTick(EEComputerPlayer* inPlayer, unsigned long inNow);
void SafetyTick(EEComputerPlayer* inPlayer, unsigned long inNow);
bool RefuseRepair(EEComputerPlayer* inPlayer, EEComplexUnit* inCitizen, EEUnit* inBuilding);

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
