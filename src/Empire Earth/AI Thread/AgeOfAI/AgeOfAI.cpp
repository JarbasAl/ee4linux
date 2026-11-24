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

// A player's own settings: a profile file read in full, or the shared tables of ageofai.ini
struct Profile {
    bool mSet = false;
    Settings mGeneral;
    SettingsAI mAI[3];
};
Profile sProfiles[16];

const Profile* ProfileOf(unsigned long inPlayerID)
{
    Loaded();
    return inPlayerID < 16 && sProfiles[inPlayerID].mSet ? &sProfiles[inPlayerID] : NULL;
}

}

void SetPlayerSettings(unsigned long inPlayerID, const char* inPath)
{
    if (inPlayerID >= 16)
        return;
    Profile& theProfile = sProfiles[inPlayerID];
    theProfile.mSet = true;
    for (const std::string& theLine : ReadSettings(inPath, theProfile.mGeneral, theProfile.mAI, true))
        fprintf(stderr, "ageofai: player %lu (%s): %s\n", inPlayerID, inPath, theLine.c_str());
}

const Settings& GeneralOf(unsigned long inPlayerID)
{
    const Profile* theProfile = ProfileOf(inPlayerID);
    return theProfile != NULL ? theProfile->mGeneral : gSettings;
}

bool UnitsFirstOn(unsigned long inPlayerID)
{
    return GeneralOf(inPlayerID).units_first != 0;
}

const SettingsAI& SettingsAIOf(unsigned long inPlayerID, unsigned long inDifficulty)
{
    const Profile* theProfile = ProfileOf(inPlayerID);
    unsigned long theDifficulty = inDifficulty > 2 ? kDifficultyMedium : inDifficulty;
    return theProfile != NULL ? theProfile->mAI[theDifficulty] : gSettingsAI[theDifficulty];
}

void SetClassicPlayers(unsigned inMask)
{
    sClassicPlayers = inMask;
}

namespace {
WallCounts sWalls[16];
}

bool GameWallsOn(unsigned long inPlayerID)
{
    return SettingsAIOf(inPlayerID, kDifficultyMedium).game_walls != 0;
}

void NoteWallUpdate(unsigned long inPlayerID, bool inSkipped)
{
    if (inPlayerID >= 16)
        return;
    WallCounts& theCounts = sWalls[inPlayerID];
    if (inSkipped)
        theCounts.mSkipped++;
    else
        theCounts.mRan++;
}

WallCounts WallCountsOf(unsigned long inPlayerID)
{
    return inPlayerID < 16 ? sWalls[inPlayerID] : WallCounts();
}

namespace {
struct BuildFixState {
    BuildFixCounts mCounts;
    unsigned short mStreak[871];
    unsigned long mFailsByType[871];
};
BuildFixState sBuildFix[16];
}

bool BuildFixResult(unsigned long inPlayerID, unsigned long inType, bool inPlaced)
{
    if (inPlayerID >= 16)
        return false;
    BuildFixState& theState = sBuildFix[inPlayerID];
    theState.mCounts.mAttempts++;
    if (inPlaced) {
        if (inType < 871)
            theState.mStreak[inType] = 0;
        return false;
    }
    theState.mCounts.mFails++;
    if (inType >= 871)
        return false;
    unsigned short& theStreak = theState.mStreak[inType];
    if (theStreak < 3)
        theStreak++;
    theState.mFailsByType[inType]++;
    if (theState.mFailsByType[inType] > theState.mCounts.mTopFails) {
        theState.mCounts.mTopFails = theState.mFailsByType[inType];
        theState.mCounts.mTopType = inType;
    }
    if (theStreak < 3 || !SettingsAIOf(inPlayerID, kDifficultyMedium).build_fix)
        return false;
    theStreak = 0;
    theState.mCounts.mDropped++;
    return true;
}

BuildFixCounts BuildFixCountsOf(unsigned long inPlayerID)
{
    return inPlayerID < 16 ? sBuildFix[inPlayerID].mCounts : BuildFixCounts();
}

Engine EngineOf(unsigned long inPlayerID)
{
    bool theClassic = inPlayerID < 16 && ((sClassicPlayers >> inPlayerID) & 1);
    return GeneralOf(inPlayerID).enabled && !theClassic ? kEngineAgeOfAI : kEngineClassic;
}

bool ClearsScenarioFlag(unsigned long inPlayerID)
{
    return EngineOf(inPlayerID) == kEngineAgeOfAI && GeneralOf(inPlayerID).scenario_ai;
}

}
