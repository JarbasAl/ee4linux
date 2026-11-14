// The headless simulation, "ee-aoc --sim [options]": a seeded computer-against-computer random-map game of the game's
// own code, with no splash, no movies and no rendering, run as fast as the CPU allows. It writes a JSON report and,
// with --sync-trace, one line per communications tick to compare two runs.
//
// The driver runs on a thread of its own:
//   1. it drives the menus in-process: main menu, single player, random map, the lobby's slots and options set by line
//      name, Start. EEMStartLoading::Execute takes the pinned seed (platform/PlatSim.h);
//   2. when the game runs, it marks the idle human's civilization as chosen and locked, so the civilization timer
//      opens no form, and sets gPlatSimRun: the physics thread then runs one communications tick after the other with
//      no wall clock (RTSPhysics::Main);
//   3. after each communications tick, on the physics thread (AfterTick): the samples, the trace line, and the end
//      of the run (--game-seconds, the game's own end, or the last computer standing);
//   4. it writes the report and exits without the game's shutdown.
//
// Ported from Protryon/empire_earth_next, src/Empire Earth/Empire Earth/EESim.cpp at ddb437ab (no license granted by
// its authors), without its AI-engine, rules and settings options. The sync trace follows Grille's Age of AI
// (dll/test/trace.c at 39d8ee42, GPL-3.0).
#include <windows.h>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <deque>
#include <list>
#include <map>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"
#include "lle/GESurface.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "GEWindow.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/FSPath.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/SESoundManager.h"
#include "lle/Input System.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UIButton.h"
#include "lle/NEMessage.h"
#include "lle/NESynchronousCommunications.h"
#include "lle/GELine.h"
#include "lle/UITextLabel.h"
#include "lle/UIListBox.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UILBString.h"
#include "lle/UIFormManager.h"
#include "lle/UResourceFile.h"
#include "lle/UIEditBox.h"
#include "lle/UICheckBox.h"
#include "lle/UIRadioButton.h"
#include "lle/UIScrollBar.h"
#include "lle/UIFileListBox.h"
#include "lle/UIComboBox.h"
#include "lle/UIBlinkingButton.h"
#include "lle/UIBitmap.h"
#include "lle/NESynchronousServer.h"
#include "lle/FSRestorable.h"
#include "ee/EmpireEarth.h"
#include "AgeOfAI/AgeOfAI.h"
#include "ee/EEAction.h"
#include "ee/EEARemovePlayer.h"
#include "ee/EEMessage.h"
#include "ee/EECommunications.h"
#include "ee/EEUserInterface.h"
#include "ee/EEUIManager.h"
#include "ee/EEServer.h"
#include "ee/EEWonNet.h"
#include "ee/EEGameOptions.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEGamePreferences.h"
#include "ee/EESMMediaStrings.h"
#include "ee/EEPSaveGameHeader.h"
#include "ee/EESaveGameFiles.h"
#include "ee/EEGameVariant.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EERandomMapGenerators.h"
#include "ee/EEUICPing.h"
#include "ee/EEUIELobbyEvents.h"
#include "ee/EEUIEPostGameSummaryEvents.h"
#include "ee/EEStartingResources.h"
#include "ee/EEStringIDs.h"
#include "ee/EETechTreeTechID.h"
#include "ee/EEPlayer.h"
#include "ee/EEPhysics.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPlane.h"
#include "lle/GE3DLine.h"
#include "lle/GETransformation.h"
#include "lle/GEPhysicalModel.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFile.h"
#include "lle/U2DSparseArray.h"
#include "lle/U2DPolygon.h"
#include "lle/USingleton.h"
#include "ee/EEComputerPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEUnitClassIDs.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEUnit.h"
#include "platform/PlatSim.h"

namespace {

// ------------------------------------------------------------------------------------------------ options

struct Options {
    unsigned long mSeed = 2;
    long mComputers = 2;
    std::string mMapSize = "Small";
    std::string mMapType;
    std::string mDifficulty = "Hard";
    std::vector<std::pair<long, std::string>> mLobby;   // --lobby CONTROL=VALUE
    double mGameSeconds = 2700;
    double mSampleSeconds = 30;
    double mWallTimeout = 3600;
    long mPTRate = 40;
    bool mNoHumanDefeat = true;
    std::string mReport;                                 // the JSON report ("" or "-": stdout)
    std::string mSyncTrace;                              // one line per communications tick ("": none)
    long mPortOffset = -1;                               // -1: from the process id
    long mAutoSaveMs = 0;                                // the game's autosave period (0: none)
    bool mQuiet = false;
    unsigned mClassicPlayers = 0;                        // --classic-players: bit n is player n
    std::vector<std::pair<std::string, long>> mArmies;   // --army NAME:COUNT: units of every computer player, sent at an enemy
    double mArmyAt = 120;                                // --army-at S: the game second they appear
};

Options sOptions;
FILE* sSyncFile = NULL;
std::mutex sOutLock;
std::chrono::steady_clock::time_point sStartWall;
std::chrono::steady_clock::time_point sGameWall;

void Note(const char* inFormat, ...)
{
    if (sOptions.mQuiet)
        return;
    double theSeconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - sStartWall).count();
    char theText[1024];
    va_list theArguments;
    va_start(theArguments, inFormat);
    vsnprintf(theText, sizeof(theText), inFormat, theArguments);
    va_end(theArguments);
    std::lock_guard<std::mutex> theGuard(sOutLock);
    fprintf(stderr, "[sim %7.1f] %s\n", theSeconds, theText);
    fflush(stderr);
}

[[noreturn]] void Quit(int inCode)
{
    {
        std::lock_guard<std::mutex> theGuard(sOutLock);
        fflush(stdout);
        fflush(stderr);
        if (sSyncFile)
            fflush(sSyncFile);
    }
    _exit(inCode);
}

// a failed run: the report with the error, then the exit (the game is built without exceptions)
[[noreturn]] void Fail(const std::string& inError);

std::string JsonString(const std::string& inText)
{
    std::string theOut = "\"";
    for (unsigned char c : inText) {
        if (c == '"' || c == '\\') {
            theOut += '\\';
            theOut += (char)c;
        } else if (c < 0x20 || c >= 0x7f) {
            char theEscape[8];
            snprintf(theEscape, sizeof(theEscape), "\\u%04x", c);
            theOut += theEscape;
        } else
            theOut += (char)c;
    }
    return theOut + "\"";
}

// ------------------------------------------------------------------------------------------------ the samples

struct PlayerSample {
    unsigned long mIndex = 0;
    bool mHuman = false;
    long mEpoch = -1;
    long mPopulation = 0;       // EEPlayer::mPopulationCount
    long mVictoryUnits = 0;     // EEPlayer::mEndOfGameCount
    long mCitizens = 0;
    long mArmy = 0;             // live units that are neither citizens nor buildings (priests and heroes too)
    long mBuildings = 0;
    long mStrength = 0;         // the army's hit points times attack, summed
    long mValue = 0;            // the cost of every live unit and building, all resources summed
    long mResources[5] = {0, 0, 0, 0, 0};
    unsigned long mResult = 0;  // EEPlayer::mGameResult: 1 victorious, 2 defeated
    // an Age of AI player's memory (AgeOfAI::MemoryOf); mHasIntel is false for the others
    bool mHasIntel = false;
    long mIntelRecords = 0, mIntelBuildings = 0, mIntelSoldiers = 0, mIntelSeenNow = 0, mIntelCells = 0, mIntelArmy = 0;
};

struct Sample {
    long mGameMs = 0;
    unsigned long mPT = 0;
    double mWall = 0;
    std::vector<PlayerSample> mPlayers;
};

struct Defeat {
    unsigned long mPlayer;
    long mGameMs;
    unsigned long mPT;
};

std::mutex sStateLock;
std::condition_variable sDone;
bool sFinished = false;
std::string sOutcome;
std::vector<Sample> sSamples;
std::vector<Defeat> sDefeats;
std::vector<unsigned long> sResults;          // per world player, the last result seen
std::vector<std::vector<long>> sEpochMs;      // per world player: the game ms each epoch was first reached (-1: not)
long sNextSampleMs = 0;
long sStandingSince = -1;                     // game ms the last-computer-standing condition holds from

bool IsBuildingCategory(long inCategory)
{
    return inCategory == kUnitCategoryBuilding || inCategory == kUnitCategoryNavalBuilding
        || inCategory == kUnitCategoryMiningBuilding || inCategory == kUnitCategoryGate
        || inCategory == kUnitCategoryFortress || inCategory == kUnitCategoryBuildingPriest
        || inCategory == kUnitCategorySpaceBuilding;
}

bool IsCounted(EEUnit* inUnit)
{
    if (inUnit->mComplexUnit == NULL || !(inUnit->mHitPoints > 0))
        return false;
    long theCategory = inUnit->mUnitClass->mCategory;
    // the AI's path points (ambient units), resources and shots
    return theCategory != kUnitCategoryAmbient && theCategory != kUnitCategoryResource
        && theCategory != kUnitCategoryProjectile;
}

// under the world lock (the physics thread's, or the driver's read lock)
Sample TakeSample()
{
    Sample theSample;
    theSample.mGameMs = gPhysics->mGameTime;
    theSample.mPT = gPhysics->mNumTicksDone;
    theSample.mWall = std::chrono::duration<double>(std::chrono::steady_clock::now() - sGameWall).count();
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(i);
        if (thePlayer == NULL)
            continue;
        PlayerSample p;
        p.mIndex = i;
        p.mHuman = (long)i == gWorld.mCurrentPlayer;
        p.mEpoch = thePlayer->mTechnologies != NULL ? thePlayer->mTechnologies->mCurrentEpoch : -1;
        p.mPopulation = thePlayer->mPopulationCount;
        p.mVictoryUnits = thePlayer->mEndOfGameCount;
        p.mResult = thePlayer->mGameResult;
        if (!p.mHuman && AgeOfAI::EngineOf(i) == AgeOfAI::kEngineAgeOfAI) {
            const AgeOfAI::Intel::Memory& theMemory = AgeOfAI::MemoryOf(i);
            p.mHasIntel = true;
            p.mIntelRecords = (long)theMemory.Records().size();
            for (const AgeOfAI::Intel::Record& theRecord : theMemory.Records()) {
                p.mIntelBuildings += theRecord.mKind == AgeOfAI::Intel::kBuilding;
                p.mIntelSoldiers += theRecord.mKind == AgeOfAI::Intel::kSoldier;
            }
            p.mIntelSeenNow = (long)theMemory.SeenNow();
            p.mIntelCells = theMemory.CellsLooked(0, true);
            float theHP = 0, theAttack = 0, thePower = 0;
            int theCount = 0;
            theMemory.Army(theHP, theAttack, thePower, theCount);
            p.mIntelArmy = theCount;
        }
        for (long r = 0; r < 5; r++)
            p.mResources[r] = thePlayer->mResourceCaps[kResourceFood + r];
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (!IsCounted(theUnit))
                continue;
            long theCategory = theUnit->mUnitClass->mCategory;
            if (theCategory == kUnitCategoryLandCitizen || theCategory == kUnitCategoryNavalCitizen)
                p.mCitizens++;
            else if (IsBuildingCategory(theCategory))
                p.mBuildings++;
            else {
                p.mArmy++;
                if (theUnit->mStats != NULL)
                    p.mStrength += (long)(theUnit->mHitPoints * theUnit->mStats->mAttack);
            }
            EETechTreeNode* theNode = thePlayer->mTechnologies != NULL
                ? thePlayer->mTechnologies->Get(theUnit->mUnitClass->mTechnology) : NULL;
            if (theNode != NULL && theNode->mData != NULL)
                for (long r = kResourceFood; r <= kResourceIron; r++)
                    p.mValue += theNode->GetCost(r, theUnit->mUnitClass->mUnitType, 1.0f);
        }
        theSample.mPlayers.push_back(p);
    }
    return theSample;
}

// FNV-1a over 32-bit words
uint32_t Hash(uint32_t ioHash, uint32_t inValue)
{
    return (ioHash ^ inValue) * 16777619u;
}

// The sync trace line: "<PT> <game ms> <seed> <stock hash> <units> <unit hash>". The stock hash covers every player's
// resources; the unit hash is a sum of each unit's hash (owner, id, class, tile, hit points), so the order of the units
// does not matter. Two runs of one seed part at the first line that differs.
void WriteSyncLine()
{
    uint32_t theStock = 2166136261u;
    uint32_t theUnits = 0;
    unsigned long theCount = 0;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(i);
        if (thePlayer == NULL)
            continue;
        for (long r = kResourceFood; r <= kResourceIron; r++)
            theStock = Hash(theStock, (uint32_t)thePlayer->mResourceCaps[r]);
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            uint32_t theHash = 2166136261u;
            theHash = Hash(theHash, (uint32_t)i);
            theHash = Hash(theHash, (uint32_t)theUnit->mUnitID);
            theHash = Hash(theHash, (uint32_t)(theUnit->mUnitClass ? theUnit->mUnitClass->mTechnology : -1));
            theHash = Hash(theHash, (uint32_t)theUnit->mTileX);
            theHash = Hash(theHash, (uint32_t)theUnit->mTileY);
            theHash = Hash(theHash, (uint32_t)theUnit->mHitPoints);
            theUnits += theHash;
            theCount++;
        }
    }
    fprintf(sSyncFile, "%lu %ld %lu %08x %lu %08x\n", gPhysics->mNumTicksDone, gPhysics->mGameTime,
            gPhysics->mRandom.GetSeed(), theStock, theCount, theUnits);
}

void Finish(const char* inOutcome)
{
    gPlatSimRun = 0;
    std::lock_guard<std::mutex> theGuard(sStateLock);
    if (sFinished)
        return;
    sOutcome = inOutcome;
    sFinished = true;
    sDone.notify_all();
}

bool sArmiesSpawned = false;

std::string LowerCase(std::string inText)
{
    for (char& c : inText)
        c = (char)tolower((unsigned char)c);
    return inText;
}

// The first land unit class whose name has inName in it (without case), or NULL
EEUnitClass* FindUnitClass(const std::string& inName)
{
    for (unsigned long i = 0; i < 871; i++) {
        EEUnitClass* theClass = gUnitClassTable[i];
        if (theClass == NULL || theClass->mCategory != kUnitCategoryLand)
            continue;
        if (LowerCase((const char*)theClass->GetName()).find(LowerCase(inName)) != std::string::npos)
            return theClass;
    }
    return NULL;
}

// The mean tile of a player's buildings; false when it has none
bool BaseOf(EEPlayer* inPlayer, long& outX, long& outY)
{
    long theCount = 0, theX = 0, theY = 0;
    for (EEUnit* theUnit = inPlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (!IsCounted(theUnit) || !IsBuildingCategory(theUnit->mUnitClass->mCategory))
            continue;
        theX += theUnit->mTileX;
        theY += theUnit->mTileY;
        theCount++;
    }
    if (theCount == 0)
        return false;
    outX = theX / theCount;
    outY = theY / theCount;
    return true;
}

// --army: at the given game second, each computer player gets the armies of the options, a block of units six tiles from
// its buildings towards the next computer player, each sent to attack that player's nearest building. The units come
// from the first land class of that name, the same for every player: a fair fight on any map.
void SpawnArmies()
{
    sArmiesSpawned = true;
    std::vector<unsigned long> thePlayers;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++)
        if ((long)i != gWorld.mCurrentPlayer && gWorld.GetPlayer(i) != NULL && gWorld.GetPlayer(i)->mFirstUnit != NULL)
            thePlayers.push_back(i);
    for (size_t k = 0; k < thePlayers.size() && thePlayers.size() > 1; k++) {
        EEPlayer* theOwner = gWorld.GetPlayer(thePlayers[k]);
        EEPlayer* theEnemy = gWorld.GetPlayer(thePlayers[(k + 1) % thePlayers.size()]);
        long theBaseX, theBaseY, theEnemyX, theEnemyY;
        if (!BaseOf(theOwner, theBaseX, theBaseY) || !BaseOf(theEnemy, theEnemyX, theEnemyY))
            continue;
        EEUnit* theTarget = NULL;
        long theBest = 0;
        for (EEUnit* theUnit = theEnemy->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (!IsCounted(theUnit) || !IsBuildingCategory(theUnit->mUnitClass->mCategory))
                continue;
            long theDistance = (theUnit->mTileX - theBaseX) * (theUnit->mTileX - theBaseX)
                + (theUnit->mTileY - theBaseY) * (theUnit->mTileY - theBaseY);
            if (theTarget == NULL || theDistance < theBest) {
                theTarget = theUnit;
                theBest = theDistance;
            }
        }
        double theDX = theEnemyX - theBaseX, theDY = theEnemyY - theBaseY, theLength = sqrt(theDX * theDX + theDY * theDY);
        if (theTarget == NULL || theLength < 1)
            continue;
        long theFromX = theBaseX + (long)lround(theDX / theLength * 6), theFromY = theBaseY + (long)lround(theDY / theLength * 6);
        long theMade = 0, theSlot = 0;
        for (const std::pair<std::string, long>& theArmy : sOptions.mArmies) {
            EEUnitClass* theClass = FindUnitClass(theArmy.first);
            if (theClass == NULL)
                continue;
            for (long n = 0; n < theArmy.second; theSlot++) {
                if (theSlot > 4000)
                    break;
                long theX = theFromX + (theSlot % 12) - 6, theY = theFromY + (theSlot / 12);   // rows of 12 tiles
                EEUnit* theUnit = theClass->CreateUnit(theX, theY, (long)thePlayers[k], false, NULL);
                if (theUnit == NULL || theUnit->mComplexUnit == NULL)
                    continue;
                theUnit->mComplexUnit->SetGoal(new EEUGAttackUnit(theUnit->mComplexUnit, theTarget, false, false), false, 0);
                n++;
                theMade++;
            }
        }
        Note("ARMY player %lu: %ld units at %ld,%ld, to attack player %lu's building at %ld,%ld", thePlayers[k], theMade, theFromX,
             theFromY, thePlayers[(k + 1) % thePlayers.size()], (long)theTarget->mTileX, (long)theTarget->mTileY);
    }
}

// the physics thread, after each communications tick, under the physics lock
void AfterTick()
{
    long theTime = gPhysics->mGameTime;
    unsigned long theCount = gWorld.GetNumPlayers();
    if (!sOptions.mArmies.empty() && !sArmiesSpawned && theTime >= (long)(sOptions.mArmyAt * 1000))
        SpawnArmies();
    if (sSyncFile)
        WriteSyncLine();
    if (sResults.size() < theCount)
        sResults.resize(theCount, 0);
    for (unsigned long i = 1; i < theCount; i++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(i);
        unsigned long theResult = thePlayer ? thePlayer->mGameResult : 0;
        long theEpoch = thePlayer && thePlayer->mTechnologies ? thePlayer->mTechnologies->mCurrentEpoch : -1;
        if (theEpoch >= 0) {
            std::lock_guard<std::mutex> theGuard(sStateLock);
            if (sEpochMs.size() < theCount)
                sEpochMs.resize(theCount);
            std::vector<long>& theTimes = sEpochMs[i];
            if ((long)theTimes.size() <= theEpoch)
                theTimes.resize(theEpoch + 1, -1);
            for (long e = 0; e <= theEpoch; e++)   // an advance reaches every epoch up to it
                if (theTimes[e] < 0)
                    theTimes[e] = theTime;
        }
        if (theResult != sResults[i]) {
            sResults[i] = theResult;
            if (theResult == 2)
                sDefeats.push_back(Defeat{i, theTime, gPhysics->mNumTicksDone});
            Note("RESULT player %lu %s at game ms %ld PT %lu", i,
                 theResult == 1 ? "victorious" : theResult == 2 ? "defeated" : "?", theTime, gPhysics->mNumTicksDone);
        }
    }
    if (theTime >= sNextSampleMs) {
        Sample theSample = TakeSample();
        {
            std::lock_guard<std::mutex> theGuard(sStateLock);
            sSamples.push_back(theSample);
        }
        sNextSampleMs += (long)(sOptions.mSampleSeconds * 1000);
        if (!sOptions.mQuiet) {
            std::string theLine;
            for (const PlayerSample& p : theSample.mPlayers) {
                char thePart[160];
                snprintf(thePart, sizeof(thePart), " p%lu e%ld c%ld a%ld b%ld", p.mIndex, p.mEpoch, p.mCitizens,
                         p.mArmy, p.mBuildings);
                theLine += thePart;
            }
            Note("game s %ld PT %lu:%s", theTime / 1000, theSample.mPT, theLine.c_str());
        }
    }
    if (gPhysics->mGameOver) {
        Finish("game-end");
        return;
    }
    if (sOptions.mNoHumanDefeat) {
        long theAlive = 0;
        bool theOthersDone = true;
        for (unsigned long i = 1; i < theCount; i++) {
            EEPlayer* thePlayer = gWorld.GetPlayer(i);
            if (thePlayer == NULL || (long)i == gWorld.mCurrentPlayer)
                continue;
            if (thePlayer->mEndOfGameCount > 0)
                theAlive++;
            else if (thePlayer->mGameResult == 0)
                theOthersDone = false;
        }
        if (theAlive <= 1 && theOthersDone) {
            if (sStandingSince < 0)
                sStandingSince = theTime;
            else if (theTime - sStandingSince >= 1000) {
                Finish("last-computer-standing");
                return;
            }
        } else
            sStandingSince = -1;
    } else {
        bool theAll = theCount > 1;
        for (unsigned long i = 1; i < theCount; i++) {
            EEPlayer* thePlayer = gWorld.GetPlayer(i);
            if (thePlayer && thePlayer->mGameResult == 0)
                theAll = false;
        }
        if (theAll) {
            Finish("game-end");
            return;
        }
    }
    if (theTime >= (long)(sOptions.mGameSeconds * 1000))
        Finish("time-cap");
}

// ------------------------------------------------------------------------------------------------ the menus

const unsigned long kTriggerEvent = 0x3f0;
const unsigned long kFormMain = 12, kFormSingle = 6, kFormLobbyID = 26, kFormGame = 11;
const long kPlayerType = 30, kTeam = 62, kCitizens = 126;
const long kVisibleSlots = 8;

struct UILock {
    UILock() { gUIManager->LockUIForCopyData(); }
    ~UILock() { gUIManager->UnlockUIForCopyData(); }
};

unsigned long TopForm()
{
    if (gUIManager == NULL)
        return 0;
    UILock theLock;
    UIForm* theForm = gUIManager->GetCurrentForm();
    return theForm ? theForm->GetFormID() : 0;
}

void SimSleep(long inMilliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(inMilliseconds));
}

void WaitFor(const char* inWhat, double inSeconds, const std::function<bool()>& inTest)
{
    auto theEnd = std::chrono::steady_clock::now() + std::chrono::duration<double>(inSeconds);
    while (!inTest()) {
        if (std::chrono::steady_clock::now() > theEnd)
            Fail(std::string("timed out waiting for ") + inWhat + " (top form " + std::to_string(TopForm()) + ")");
        SimSleep(50);
    }
}

void WaitForm(unsigned long inForm, double inSeconds = 120)
{
    WaitFor(("form " + std::to_string(inForm)).c_str(), inSeconds, [&] { return TopForm() == inForm; });
}

void Click(unsigned long inForm, unsigned long inControl)
{
    UILock theLock;
    UIForm* theForm = gUIManager->GetCurrentForm();
    if (!theForm || theForm->GetFormID() != inForm || inControl >= theForm->GetControlCount())
        Fail("click: form " + std::to_string(inForm) + " control " + std::to_string(inControl) + " is not on top");
    UIControl* theControl = theForm->GetControl(inControl);
    if (!theControl || !theControl->GetTriggerEvent(kTriggerEvent))
        Fail("click: control " + std::to_string(inControl) + " has no trigger event");
    theControl->FireTriggerEvent(kTriggerEvent);
}

UIComboBox* Combo(UIForm* inForm, long inControl)
{
    if (!inForm || inControl < 0 || (unsigned long)inControl >= inForm->GetControlCount())
        return NULL;
    return dynamic_cast<UIComboBox*>(inForm->GetControl(inControl));
}

// the combo's lines and selection on the lobby form; false when the control is not a combo there
bool ComboState(long inControl, std::vector<std::string>& outLines, long& outSelection)
{
    UILock theLock;
    UIForm* theForm = gUIManager->GetCurrentForm();
    if (!theForm || theForm->GetFormID() != kFormLobbyID)
        return false;
    UIComboBox* theCombo = Combo(theForm, inControl);
    if (!theCombo || !theCombo->GetListBox())
        return false;
    outLines.clear();
    const UILBString* theList = theCombo->GetListBox();
    for (unsigned long k = 0; k < theList->GetLineCount(); k++) {
        const char* theText = (const char*)theList->GetData(k);
        outLines.push_back(theText ? theText : "");
    }
    unsigned long theSelection = 0xffffffffUL;
    theCombo->GetCurrentSelection(theSelection);
    outSelection = (long)(int32_t)theSelection;
    return true;
}

std::string Lower(std::string inText)
{
    size_t theFirst = inText.find_first_not_of(" \t");
    size_t theLast = inText.find_last_not_of(" \t");
    inText = theFirst == std::string::npos ? std::string() : inText.substr(theFirst, theLast - theFirst + 1);
    for (char& c : inText)
        c = (char)tolower((unsigned char)c);
    return inText;
}

// a line by name: "#k" an index, else the exact line, else the one line it starts, else the one line containing it
long Resolve(const std::string& inValue, const std::vector<std::string>& inLines, const std::string& inWhat)
{
    if (!inValue.empty() && inValue[0] == '#') {
        long k = atol(inValue.c_str() + 1);
        if (k < 0 || k >= (long)inLines.size())
            Fail(inWhat + ": index " + inValue + " outside the combo's " + std::to_string(inLines.size()) + " lines");
        return k;
    }
    std::string theKey = Lower(inValue);
    std::vector<long> theHits;
    for (size_t k = 0; k < inLines.size(); k++)
        if (Lower(inLines[k]) == theKey)
            return (long)k;
    for (size_t k = 0; k < inLines.size(); k++)
        if (Lower(inLines[k]).compare(0, theKey.size(), theKey) == 0)
            theHits.push_back((long)k);
    if (theHits.empty())
        for (size_t k = 0; k < inLines.size(); k++)
            if (Lower(inLines[k]).find(theKey) != std::string::npos)
                theHits.push_back((long)k);
    if (theHits.size() != 1) {
        std::string theAll;
        for (const std::string& theLine : inLines)
            theAll += (theAll.empty() ? "" : "|") + theLine;
        Fail(inWhat + ": '" + inValue + "' matches " + std::to_string(theHits.size()) + " of the lines " + theAll);
    }
    return theHits[0];
}

struct Chosen {
    long mControl;
    std::string mLabel, mValue, mLine;
    long mIndex;
};
std::vector<Chosen> sChosen;

void Select(long inControl, const std::string& inValue, const std::string& inLabel)
{
    std::vector<std::string> theLines;
    long theSelection = -1;
    if (!ComboState(inControl, theLines, theSelection))
        Fail(inLabel + ": control " + std::to_string(inControl) + " is not a combo on the lobby");
    long theIndex = Resolve(inValue, theLines, inLabel);
    if (theSelection != theIndex) {
        {
            UILock theLock;
            UIComboBox* theCombo = Combo(gUIManager->GetCurrentForm(), inControl);
            if (!theCombo)
                Fail(inLabel + ": the lobby went away");
            theCombo->SetCurrentSelection((unsigned long)theIndex);
        }
        WaitFor((inLabel + " to stick").c_str(), 20, [&] {
            std::vector<std::string> theNow;
            long theNowSelected = -1;
            return ComboState(inControl, theNow, theNowSelected) && theNowSelected == theIndex;
        });
        SimSleep(200);   // the lobby's update round trip through the local server
    }
    sChosen.push_back(Chosen{inControl, inLabel, inValue, theLines[theIndex], theIndex});
    Note("LOBBY %s: control %ld = %ld '%s'", inLabel.c_str(), inControl, theIndex, theLines[theIndex].c_str());
}

void SetUpLobby()
{
    long n = sOptions.mComputers;
    WaitFor("the lobby's combos", 60, [&] {
        std::vector<std::string> theLines;
        long theSelection;
        for (long k = 0; k < kVisibleSlots; k++)
            if (!ComboState(kPlayerType + k, theLines, theSelection))
                return false;
        return ComboState(177, theLines, theSelection) && ComboState(179, theLines, theSelection)
            && ComboState(207, theLines, theSelection);
    });
    SimSleep(1500);   // the local server's readiness and queued lobby updates
    // (control, value), applied in order, absolutely
    std::vector<std::pair<long, std::string>> theOptions = {
        {175, "Random Map"}, {177, "Small"}, {179, "Continental"}, {181, "Standard - High"},
        {183, "Prehistoric Age"}, {185, "Space Age"}, {187, "1200"}, {189, "Off"}, {205, "Standard"},
        {207, "Easy"}, {209, "Very Fast"}};
    auto set = [&](long inControl, const std::string& inValue) {
        for (auto& theOption : theOptions)
            if (theOption.first == inControl) {
                theOption.second = inValue;
                return;
            }
        theOptions.push_back({inControl, inValue});
    };
    if (!sOptions.mMapSize.empty())
        set(177, sOptions.mMapSize);
    if (!sOptions.mMapType.empty())
        set(179, sOptions.mMapType);
    if (!sOptions.mDifficulty.empty())
        set(207, sOptions.mDifficulty);
    for (auto& theLobby : sOptions.mLobby)
        set(theLobby.first, theLobby.second);
    for (long s = 0; s <= n; s++) {   // the playing slots: no team (free for all), 5 starting citizens
        bool theTeam = false, theCitizens = false;
        for (auto& theOption : theOptions) {
            theTeam = theTeam || theOption.first == kTeam + s;
            theCitizens = theCitizens || theOption.first == kCitizens + s;
        }
        if (!theTeam)
            theOptions.push_back({kTeam + s, "-"});
        if (!theCitizens)
            theOptions.push_back({kCitizens + s, "5"});
    }
    Select(kPlayerType, "#0", "slot 0 human");
    for (long s = 1; s < kVisibleSlots; s++)
        Select(kPlayerType + s, s <= n ? "Computer" : "Open", "slot " + std::to_string(s));
    for (auto& theOption : theOptions)
        Select(theOption.first, theOption.second, "control " + std::to_string(theOption.first));
    SimSleep(1000);
    for (const Chosen& c : sChosen) {
        std::vector<std::string> theLines;
        long theSelection = -1;
        if (!ComboState(c.mControl, theLines, theSelection) || theSelection != c.mIndex)
            Fail("lobby selection of control " + std::to_string(c.mControl) + " changed after setting it");
    }
}

// ------------------------------------------------------------------------------------------------ the report

std::string PlayersJson(const Sample& inSample)
{
    std::string theOut = "[";
    for (size_t k = 0; k < inSample.mPlayers.size(); k++) {
        const PlayerSample& p = inSample.mPlayers[k];
        char theIntel[256] = "";
        if (p.mHasIntel)
            snprintf(theIntel, sizeof(theIntel),
                     ", \"intel\": {\"records\": %ld, \"buildings\": %ld, \"soldiers\": %ld, \"seen_now\": %ld, "
                     "\"cells_looked\": %ld, \"army\": %ld}",
                     p.mIntelRecords, p.mIntelBuildings, p.mIntelSoldiers, p.mIntelSeenNow, p.mIntelCells, p.mIntelArmy);
        char theText[768];
        snprintf(theText, sizeof(theText),
                 "%s{\"index\": %lu, \"engine\": \"%s\", \"human\": %s, \"result\": %lu, \"epoch\": %ld, \"population\": %ld, "
                 "\"victory_units\": %ld, \"citizens\": %ld, \"army\": %ld, \"buildings\": %ld, \"strength\": %ld, "
                 "\"value\": %ld, \"resources\": {\"food\": %ld, \"wood\": %ld, \"stone\": %ld, \"gold\": %ld, "
                 "\"iron\": %ld}%s}",
                 k ? ", " : "", p.mIndex,
                 p.mHuman                                                           ? "human"
                 : AgeOfAI::EngineOf(p.mIndex) == AgeOfAI::kEngineAgeOfAI ? "ageofai"
                                                                               : "classic",
                 p.mHuman ? "true" : "false", p.mResult, p.mEpoch, p.mPopulation,
                 p.mVictoryUnits, p.mCitizens, p.mArmy, p.mBuildings, p.mStrength, p.mValue, p.mResources[0],
                 p.mResources[1], p.mResources[2], p.mResources[3], p.mResources[4], theIntel);
        theOut += theText;
    }
    return theOut + "]";
}

std::string ListJson(const std::vector<unsigned long>& inValues)
{
    std::string theOut = "[";
    for (size_t k = 0; k < inValues.size(); k++)
        theOut += (k ? ", " : "") + std::to_string(inValues[k]);
    return theOut + "]";
}

void WriteReport(int inCode, const std::string& inError)
{
    std::string j = "{\n  \"kind\": \"ee-sim-v1\",\n";
    j += "  \"setup\": {\"seed\": " + std::to_string(sOptions.mSeed) + ", \"computers\": " +
         std::to_string(sOptions.mComputers) + ", \"game_seconds\": " + std::to_string((long)sOptions.mGameSeconds) +
         ", \"pt_rate\": " + std::to_string(sOptions.mPTRate) + ", \"no_human_defeat\": " +
         (sOptions.mNoHumanDefeat ? "true" : "false") + ",\n    \"lobby\": [";
    for (size_t k = 0; k < sChosen.size(); k++)
        j += std::string(k ? ", " : "") + "{\"control\": " + std::to_string(sChosen[k].mControl) + ", \"value\": " +
             JsonString(sChosen[k].mValue) + ", \"index\": " + std::to_string(sChosen[k].mIndex) + ", \"line\": " +
             JsonString(sChosen[k].mLine) + "}";
    j += "]},\n";
    std::lock_guard<std::mutex> theGuard(sStateLock);
    j += "  \"outcome\": " + JsonString(inError.empty() ? sOutcome : "error") + ",\n";
    if (!inError.empty())
        j += "  \"error\": " + JsonString(inError) + ",\n";
    double theWall = std::chrono::duration<double>(std::chrono::steady_clock::now() - sStartWall).count();
    double theGame = std::chrono::duration<double>(std::chrono::steady_clock::now() - sGameWall).count();
    char theText[256];
    snprintf(theText, sizeof(theText), "  \"wall_seconds\": %.2f,\n  \"wall_seconds_game\": %.2f,\n", theWall,
             sGameWall.time_since_epoch().count() ? theGame : 0.0);
    j += theText;
    const Sample* theLast = sSamples.empty() ? NULL : &sSamples.back();
    j += "  \"game_ms\": " + std::to_string(theLast ? theLast->mGameMs : 0) + ",\n";
    j += "  \"pt\": " + std::to_string(theLast ? theLast->mPT : 0) + ",\n";
    j += "  \"no_human_defeat_hits\": " + std::to_string((long)gPlatSimNoHumanDefeatHits) + ",\n";
    std::vector<unsigned long> theAlive, theWinners;
    if (theLast)
        for (const PlayerSample& p : theLast->mPlayers) {
            if (!p.mHuman && p.mVictoryUnits > 0)
                theAlive.push_back(p.mIndex);
            if (p.mResult == 1)
                theWinners.push_back(p.mIndex);
        }
    j += "  \"alive_computers\": " + ListJson(theAlive) + ",\n  \"winners\": " + ListJson(theWinners) + ",\n";
    j += "  \"winner\": " + std::string(sOutcome == "last-computer-standing" && theAlive.size() == 1
                                             ? std::to_string(theAlive[0]) : "null") + ",\n";
    j += "  \"defeat_order\": [";
    for (size_t k = 0; k < sDefeats.size(); k++)
        j += std::string(k ? ", " : "") + "{\"player\": " + std::to_string(sDefeats[k].mPlayer) + ", \"game_ms\": " +
             std::to_string(sDefeats[k].mGameMs) + ", \"pt\": " + std::to_string(sDefeats[k].mPT) + "}";
    j += "],\n";
    {
        const AgeOfAI::ProjectileCounts& theCounts = AgeOfAI::Projectiles();
        char theProjectiles[200];
        snprintf(theProjectiles, sizeof(theProjectiles),
                 "  \"projectiles\": {\"fired\": %u, \"led\": %u, \"hit_unit\": %u, \"hit_ground\": %u},\n", (unsigned)theCounts.mFired,
                 (unsigned)theCounts.mLed, (unsigned)theCounts.mHitUnit, (unsigned)theCounts.mHitGround);
        j += theProjectiles;
    }
    j += "  \"players\": " + (theLast ? PlayersJson(*theLast) : std::string("[]")) + ",\n";
    // per world player, the game ms each epoch was first reached (index: the epoch; an epoch never reached is missing
    // from the end)
    j += "  \"epoch_ms\": {";
    bool theFirst = true;
    for (size_t i = 1; i < sEpochMs.size(); i++) {
        if (sEpochMs[i].empty())
            continue;
        j += std::string(theFirst ? "" : ", ") + "\"" + std::to_string(i) + "\": [";
        theFirst = false;
        for (size_t e = 0; e < sEpochMs[i].size(); e++)
            j += (e ? ", " : "") + std::to_string(sEpochMs[i][e]);
        j += "]";
    }
    j += "},\n  \"samples\": [\n";
    for (size_t k = 0; k < sSamples.size(); k++) {
        snprintf(theText, sizeof(theText), "    {\"game_ms\": %ld, \"pt\": %lu, \"wall\": %.2f, \"players\": ",
                 sSamples[k].mGameMs, sSamples[k].mPT, sSamples[k].mWall);
        j += theText + PlayersJson(sSamples[k]) + (k + 1 < sSamples.size() ? "},\n" : "}\n");
    }
    j += "  ],\n  \"exit_code\": " + std::to_string(inCode) + "\n}\n";
    if (sOptions.mReport.empty() || sOptions.mReport == "-") {
        std::lock_guard<std::mutex> theOut(sOutLock);
        fputs(j.c_str(), stdout);
        fflush(stdout);
        return;
    }
    std::string theTemporary = sOptions.mReport + ".tmp";
    if (FILE* theFile = fopen(theTemporary.c_str(), "wb")) {
        fwrite(j.data(), 1, j.size(), theFile);
        fclose(theFile);
        rename(theTemporary.c_str(), sOptions.mReport.c_str());
    }
}

void Fail(const std::string& inError)
{
    gPlatSimRun = 0;
    Note("FAILED: %s", inError.c_str());
    WriteReport(2, inError);
    Quit(2);
}

// ------------------------------------------------------------------------------------------------ the driver

void Drive()
{
    int theCode = 0;
    WaitForm(kFormMain, 300);
    Note("UI main menu");
    Click(kFormMain, 0);
    WaitForm(kFormSingle);
    Click(kFormSingle, 1);
    WaitForm(kFormLobbyID);
    Note("UI lobby");
    SetUpLobby();
    Click(kFormLobbyID, 10);
    Note("UI start");
    long theExpected = sOptions.mComputers + 2;
    WaitFor("the game", 600, [&] {
        return TopForm() == kFormGame && gPhysics != NULL && gPhysics->mTimerRunning &&
               (long)gWorld.GetNumPlayers() == theExpected && gWorld.GetMap() != NULL;
    });
    SimSleep(500);
    {
        // the idle human's civilization counts as chosen and locked: the civilization timer sends its messages but
        // opens no form, and the computers' civilizations are not locked at its expiry
        gWorld.mLock->WriteLock();
        EEPlayer* theHuman = gWorld.GetCurrentPlayer();
        theHuman->mCivilizationChosen = true;
        theHuman->mCivilizationLocked = true;
        sResults.assign(gWorld.GetNumPlayers(), 0);
        gWorld.mLock->WriteUnlock();
    }
    Note("GAME players %lu local %ld: running", gWorld.GetNumPlayers(), gWorld.mCurrentPlayer);
    sGameWall = std::chrono::steady_clock::now();
    gPlatSimAfterTick = AfterTick;
    gPlatSimRun = 1;
    {
        std::unique_lock<std::mutex> theLock(sStateLock);
        if (!sDone.wait_for(theLock, std::chrono::duration<double>(sOptions.mWallTimeout), [] { return sFinished; })) {
            gPlatSimRun = 0;
            sOutcome = "wall-timeout";
            theCode = 3;
        }
    }
    gPlatSimRun = 0;
    SimSleep(50);
    if (gPhysics != NULL) {
        // the final state: the physics thread has stopped between two communications ticks
        gWorld.mLock->ReadLock();
        Sample theSample = TakeSample();
        gWorld.mLock->ReadUnlock();
        std::lock_guard<std::mutex> theGuard(sStateLock);
        if (sSamples.empty() || sSamples.back().mGameMs != theSample.mGameMs)
            sSamples.push_back(theSample);
    }
    {
        std::lock_guard<std::mutex> theGuard(sStateLock);
        const Sample* theLast = sSamples.empty() ? NULL : &sSamples.back();
        std::string theUnits;
        if (theLast)
            for (const PlayerSample& p : theLast->mPlayers)
                theUnits += " " + std::to_string(p.mIndex) + ":" + std::to_string(p.mPopulation) + "/" +
                            std::to_string(p.mVictoryUnits);
        double theGame = std::chrono::duration<double>(std::chrono::steady_clock::now() - sGameWall).count();
        Note("SIM %s: PT %lu game s %ld units%s; %.1f wall s in game (%.0fx real time)", sOutcome.c_str(),
             theLast ? theLast->mPT : 0UL, theLast ? theLast->mGameMs / 1000 : 0L, theUnits.c_str(), theGame,
             theLast && theGame > 0 ? theLast->mGameMs / 1000.0 / theGame : 0.0);
    }
    WriteReport(theCode, "");
    Quit(theCode);
}

const char* kUsage =
    "ee-aoc --sim: the headless simulation. Runs a seeded computer-against-computer random-map game of the game's own\n"
    "code with no rendering, as fast as the CPU allows, and writes a JSON report.\n"
    "  --seed N               the start seed (default 2)\n"
    "  --ai N                 computer players, 1..7 (default 2)\n"
    "  --map-size NAME        lobby map size line (default Small)\n"
    "  --map-type NAME        lobby map type line (default Continental)\n"
    "  --difficulty NAME      lobby difficulty line (default Hard)\n"
    "  --army NAME:COUNT      at --army-at, each computer player gets COUNT units of the first land class with NAME\n"
    "                         in its name, six tiles from its buildings towards the next player, sent to attack\n"
    "                         that player's nearest building (repeat for more kinds)\n"
    "  --army-at S            the game second the armies appear (default 120)\n"
    "  --classic-players L    player numbers, comma apart, that play the classic AI whatever ageofai.ini says\n"
    "  --lobby CONTROL=VALUE  any lobby combo of form 26 by line name or #index (e.g. 209=#2)\n"
    "  --game-seconds S       stop at this game time (default 2700)\n"
    "  --sample-seconds S     report sample interval in game seconds (default 30)\n"
    "  --wall-timeout S       give up after this wall time (default 3600)\n"
    "  --pt-rate N            minimum and maximum physics ticks per second (default 40)\n"
    "  --human-defeat         let the idle human be defeated (default: kept in)\n"
    "  --report FILE          the JSON report (default stdout)\n"
    "  --sync-trace FILE      one line per communications tick: PT, game ms, seed, stock hash, units, unit hash\n"
    "  --autosave-ms N        the game's autosave period in game time (default 0: none)\n"
    "  --port-offset N        move the game's TCP ports 33334/33335 by N (default: from the process id)\n"
    "  --quiet                no progress on stderr\n";

// a path made absolute now (the game may change the current directory)
std::string Absolute(const std::string& inPath)
{
    if (inPath.empty() || inPath == "-" || inPath[0] == '/')
        return inPath;
    char theDirectory[4096];
    return getcwd(theDirectory, sizeof(theDirectory)) ? std::string(theDirectory) + "/" + inPath : inPath;
}

// a fault in the simulation: the signal, the address and the return addresses on stderr, then the signal's default
// action (the handler is reset). The frames read as ee-aoc(+0xOFFSET): addr2line -f -C -e ee-aoc 0xOFFSET names them.
void Fault(int inSignal, siginfo_t* inInfo, void*)
{
    char theText[200];
    int n = snprintf(theText, sizeof(theText), "EE-SIM FAULT signal %d at %p thread %lu; frames:\n", inSignal,
                     inInfo ? inInfo->si_addr : NULL, (unsigned long)(uintptr_t)pthread_self());
    if (n > 0 && write(STDERR_FILENO, theText, (size_t)n) < 0)
        return;
    void* theFrames[48];
    backtrace_symbols_fd(theFrames, backtrace(theFrames, 48), STDERR_FILENO);
}

void InstallFaultHandler()
{
    void* theFrame[1];
    backtrace(theFrame, 1);   // loads the unwinder now, not inside the handler
    struct sigaction theAction = {};
    theAction.sa_sigaction = Fault;
    theAction.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sigemptyset(&theAction.sa_mask);
    for (int theSignal : {SIGSEGV, SIGBUS, SIGILL, SIGABRT})
        sigaction(theSignal, &theAction, NULL);
    // EE_PORT_FPE_LOG's handler (src/Port/Linux/FloatWatch.cpp) keeps SIGFPE
    if (!getenv("EE_PORT_FPE_LOG"))
        sigaction(SIGFPE, &theAction, NULL);
}

}   // namespace

// The settings the simulation runs with, over the saved ones (WinMain, after gGamePreferences.Load): a window, a fixed
// physics rate, no autosave, no music
void EESimPreferences()
{
    gGamePreferences.mSettings.mFullScreen = false;
    gGamePreferences.mSettings.mGameBitDepth = 32;
    gGamePreferences.mSettings.mTextureBitDepth = 32;
    gGamePreferences.mSettings.mGameWindowWidth = 800;
    gGamePreferences.mSettings.mGameWindowHeight = 600;
    gGamePreferences.mSettings.mMinPTsPerSecond = sOptions.mPTRate;
    gGamePreferences.mSettings.mMaxPTsPerSecond = sOptions.mPTRate;
    gGamePreferences.mSettings.mAutoSaveInMilliseconds = (unsigned long)sOptions.mAutoSaveMs;
    gGamePreferences.mSettings.mMusicEnabled = false;
    gGamePreferences.mSettings.mWaitForVSync = false;
}

// The driver's thread (WinMain, after the engine and its threads are up, before the interface loop)
void EESimStart()
{
    std::thread(Drive).detach();
}

// The options and the switches, from main for "ee-aoc --sim ..." (argv[0] is "--sim"). Returns -1 when the game is to
// start, or the exit code.
int EESimMain(int argc, char** argv)
{
    sStartWall = std::chrono::steady_clock::now();
    for (int i = 1; i < argc; i++) {
        std::string theOption = argv[i];
        auto value = [&]() -> std::string {
            if (i + 1 >= argc) {
                fprintf(stderr, "ee-aoc --sim: %s needs a value\n", theOption.c_str());
                exit(64);
            }
            return argv[++i];
        };
        if (theOption == "-h" || theOption == "--help") {
            fputs(kUsage, stdout);
            return 0;
        } else if (theOption == "--seed")
            sOptions.mSeed = strtoul(value().c_str(), NULL, 0);
        else if (theOption == "--ai")
            sOptions.mComputers = atol(value().c_str());
        else if (theOption == "--map-size")
            sOptions.mMapSize = value();
        else if (theOption == "--map-type")
            sOptions.mMapType = value();
        else if (theOption == "--difficulty")
            sOptions.mDifficulty = value();
        else if (theOption == "--army") {
            std::string theText = value();
            size_t theColon = theText.rfind(':');
            long theCount = theColon == std::string::npos ? 0 : atol(theText.c_str() + theColon + 1);
            if (theCount < 1) {
                fprintf(stderr, "ee-aoc --sim: --army takes NAME:COUNT, a part of a land unit's name and how many\n");
                return 64;
            }
            sOptions.mArmies.push_back(std::make_pair(theText.substr(0, theColon), theCount));
        } else if (theOption == "--army-at")
            sOptions.mArmyAt = atof(value().c_str());
        else if (theOption == "--classic-players") {
            std::string theList = value();
            for (const char* p = theList.c_str(); *p;) {
                char* theEnd;
                long n = strtol(p, &theEnd, 10);
                if (theEnd == p || n < 1 || n > 15) {
                    fprintf(stderr, "ee-aoc --sim: --classic-players takes player numbers 1..15, comma apart\n");
                    return 64;
                }
                sOptions.mClassicPlayers |= 1u << n;
                p = *theEnd == ',' ? theEnd + 1 : theEnd;
            }
            AgeOfAI::SetClassicPlayers(sOptions.mClassicPlayers);
        }
        else if (theOption == "--lobby") {
            std::string theValue = value();
            size_t theEqual = theValue.find('=');
            if (theEqual == std::string::npos) {
                fprintf(stderr, "ee-aoc --sim: --lobby CONTROL=VALUE\n");
                return 64;
            }
            sOptions.mLobby.push_back({atol(theValue.substr(0, theEqual).c_str()), theValue.substr(theEqual + 1)});
        } else if (theOption == "--game-seconds")
            sOptions.mGameSeconds = atof(value().c_str());
        else if (theOption == "--sample-seconds")
            sOptions.mSampleSeconds = atof(value().c_str());
        else if (theOption == "--wall-timeout")
            sOptions.mWallTimeout = atof(value().c_str());
        else if (theOption == "--pt-rate")
            sOptions.mPTRate = atol(value().c_str());
        else if (theOption == "--human-defeat")
            sOptions.mNoHumanDefeat = false;
        else if (theOption == "--no-human-defeat")
            sOptions.mNoHumanDefeat = true;
        else if (theOption == "--report")
            sOptions.mReport = Absolute(value());
        else if (theOption == "--sync-trace")
            sOptions.mSyncTrace = Absolute(value());
        else if (theOption == "--quiet")
            sOptions.mQuiet = true;
        else if (theOption == "--port-offset")
            sOptions.mPortOffset = atol(value().c_str());
        else if (theOption == "--autosave-ms")
            sOptions.mAutoSaveMs = atol(value().c_str());
        else {
            fprintf(stderr, "ee-aoc --sim: unknown option %s (--help)\n", theOption.c_str());
            return 64;
        }
    }
    if (sOptions.mComputers < 1 || sOptions.mComputers > 7 || sOptions.mPTRate < 1 || sOptions.mPTRate > 250 ||
        !(sOptions.mGameSeconds > 0) || !(sOptions.mSampleSeconds > 0)) {
        fprintf(stderr, "ee-aoc --sim: an option is out of range (--help)\n");
        return 64;
    }
    if (!sOptions.mSyncTrace.empty()) {
        sSyncFile = fopen(sOptions.mSyncTrace.c_str(), "w");
        if (!sSyncFile) {
            fprintf(stderr, "ee-aoc --sim: cannot write %s\n", sOptions.mSyncTrace.c_str());
            return 66;
        }
    }
    gPlatSimMode = 1;
    gPlatSimSeedSet = 1;
    gPlatSimSeed = (uint32_t)sOptions.mSeed;
    gPlatSimNoHumanDefeat = sOptions.mNoHumanDefeat ? 1 : 0;
    gPlatSimPortOffset = sOptions.mPortOffset >= 0 ? (int)sOptions.mPortOffset : 2000 + 2 * (int)(getpid() % 8000);
    InstallFaultHandler();
    Note("EE-SIM seed %lu, %ld computers, cap %.0f game s, ports %d/%d", sOptions.mSeed, sOptions.mComputers,
         sOptions.mGameSeconds, 33334 + gPlatSimPortOffset, 33335 + gPlatSimPortOffset);
    return -1;
}

// The game's own start-up returned before the driver ended the run: it was quit, or it failed to start
void EESimGameEnded(int inResult)
{
    WriteReport(4, "the game ended before the simulation (start-up failed?)");
    Quit(inResult ? inResult : 4);
}
