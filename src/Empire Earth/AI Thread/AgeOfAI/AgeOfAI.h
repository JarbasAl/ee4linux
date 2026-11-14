// The AI engine a computer player plays: the classic AI, the game's own, or Age of AI. [general] enabled=1 in
// ageofai.ini, in the game folder, makes every computer player an Age of AI player; the classic AI is the default.
//
// Age of AI is Grille's (https://github.com/Grille/Empire-Earth-Age-of-AI, GPL-3.0). Its parts in this folder are
// ported from 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07; each file names the upstream files it comes from.
#ifndef AGEOFAI_AGEOFAI_H
#define AGEOFAI_AGEOFAI_H

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
