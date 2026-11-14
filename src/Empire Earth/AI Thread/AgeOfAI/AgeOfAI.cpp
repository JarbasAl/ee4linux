// The AI engine choice (AgeOfAI.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI),
// dll/eeaimod.c and dll/engine/scenario_ai.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include <windows.h>

#include <cstdio>
#include <string>

#include "AgeOfAI.h"
#include "AgeOfAISettings.h"

namespace AgeOfAI {

namespace {

// ageofai.ini beside the game's executable, read once
const Settings& Loaded()
{
    static const bool sRead = [] {
        char thePath[MAX_PATH];
        DWORD theLength = GetModuleFileNameA(NULL, thePath, sizeof thePath);
        std::string theFolder(thePath, theLength);
        size_t theSlash = theFolder.find_last_of("\\/");
        std::string theFile = (theSlash == std::string::npos ? std::string() : theFolder.substr(0, theSlash + 1)) + "ageofai.ini";
        for (const std::string& theLine : ReadSettings(theFile.c_str()))
            fprintf(stderr, "ageofai: %s\n", theLine.c_str());
        return true;
    }();
    (void)sRead;
    return gSettings;
}

unsigned sClassicPlayers;

}

void SetClassicPlayers(unsigned inMask)
{
    sClassicPlayers = inMask;
}

Engine EngineOf(unsigned long inPlayerID)
{
    bool theClassic = inPlayerID < 16 && ((sClassicPlayers >> inPlayerID) & 1);
    return Loaded().enabled && !theClassic ? kEngineAgeOfAI : kEngineClassic;
}

bool ClearsScenarioFlag(unsigned long inPlayerID)
{
    return EngineOf(inPlayerID) == kEngineAgeOfAI && gSettings.scenario_ai;
}

}
