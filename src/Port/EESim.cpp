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
#include "ee/EECPCivilizationManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EEUnit.h"
#include "ee/EEUnitClassIDs.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEUnit.h"
#include "platform/PlatSim.h"

namespace {

// ------------------------------------------------------------------------------------------------ options

// --stance SECOND,FROM,TO,VALUE: at a game second, one player's diplomatic stance toward another becomes 0 (ally)
// or 1 (enemy). Repeatable. It drives the diplomacy rule the way a player's declaration drives it in a real game.
struct StanceChange {
    double mAt;
    long mFrom;
    long mTo;
    long mValue;
    bool mDone = false;
};

// --starve PLAYER,SECOND: at a game second, every citizen of that player is killed. It forces the no-citizen state the
// surrender rule needs, the way a lost town forces it in a real game. --raze also kills the player's town centres, so
// it cannot train another citizen and the rule's shorter no-centre timer applies. Both are repeatable.
struct Starve {
    double mAt;
    int mPlayer;
    bool mRaze = false;
    bool mDone = false;
};

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
    unsigned mKeepAliveMask = 0;                         // --keep-alive P: P stays in after losing its last town centre
    std::string mReport;                                 // the JSON report ("" or "-": stdout)
    std::string mSyncTrace;                              // one line per communications tick ("": none)
    long mPortOffset = -1;                               // -1: from the process id
    long mAutoSaveMs = 0;                                // the game's autosave period (0: none)
    bool mQuiet = false;
    unsigned mClassicPlayers = 0;                        // --classic-players: bit n is player n
    std::vector<std::pair<std::string, long>> mArmies;   // --army NAME:COUNT: units of every computer player, sent at an enemy
    std::vector<std::pair<std::string, long>> mAirArmies; // --army-air NAME:COUNT: the same with aircraft
    double mArmyEvery = 0;                               // --army-every S: the armies appear again every S game seconds
    double mArmyAt = 120;                                // --army-at S: the game second they appear
    bool mArmyRaid = false;                              // --army-raid: the armies appear beside the enemy's base
    bool mArmySpot = false;                              // --army-spot X,Y: the armies appear at this tile, whatever the map
    long mArmySpotX = 0, mArmySpotY = 0;
    std::vector<StanceChange> mStances;                  // --stance SECOND,FROM,TO,VALUE: timed diplomatic stance changes
    std::vector<Starve> mStarves;                        // --starve/--raze PLAYER,SECOND: forced loss of that player's citizens
    double mWallAt = -1;                                 // --wall SECOND[,COUNT]: defender-owned wall line near each attacker
    long mWallCount = 9;                                 // the number of wall segments in each line
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
    long mEarlyQueued = 0, mEarlyBuilders = 0, mEarlyBaits = 0;
    long mOutdatedSorties = 0, mOutdatedSent = 0, mOutdatedRefused = 0, mOutdatedNoGroup = 0;
    long mCitizenCalls = 0, mCitizenCalled = 0;
    long mWallSkipped = 0, mWallRan = 0;
    long mRetargetScans = 0, mRetargetPicked = 0, mRetargetGroups = 0;
    long mWaveCalls = 0, mWaveSet = 0, mWaveMoved = 0, mWaveKept = 0, mWaveNone = 0;
    long mWeakCalls = 0, mWeakThreatened = 0, mWeakPicked = 0;
    long mBreachCalls = 0, mBreachCrossed = 0, mBreachPicked = 0, mBreachWalls = 0;
    long mBuildAttempts = 0, mBuildFails = 0, mBuildDropped = 0, mBuildTopType = 0, mBuildTopFails = 0;
    long mRecoverChecks = 0, mRecoverStarts = 0, mRecoverEnds = 0, mRecoverMoves = 0;
    long mRecoverOn = 0, mRecoverHadEconomy = 0;
    long mAvoidCalls = 0, mAvoidPenalised = 0, mAvoidCost = 0;
    long mRamCalls = 0, mRamWaits = 0;
    long mAirCalls = 0, mAirSkipped = 0;
    long mLeashCalls = 0, mLeashSkipped = 0;
    long mTetherCalls = 0, mTetherSkipped = 0;
    long mMassChecks = 0, mMassWaits = 0, mMassGoes = 0, mMassWaited = 0;
    long mCatchChecks = 0, mCatchCalls = 0;
    long mGranChecks = 0, mGranRequested = 0;
    long mFishCalls = 0, mFishPicked = 0, mFishChanged = 0, mFishClass = 0;
    long mMineCalls = 0, mMineCapped = 0, mMineMines = 0, mMineCap = 0;
    long mFoodPulled = 0;
    long mCounterFireSorties = 0, mCounterFireSent = 0, mCounterFireShort = 0;
    AgeOfAI::DefenceCounts mDefence = {};
    long mHomeSent = 0, mHomeBaseMoved = 0;
    long mGarrisonChecks = 0, mGarrisonOrders = 0, mGarrisonCitizens = 0;
    long mGarrisonIdle = 0, mGarrisonWood = 0, mGarrisonHolds = 0;
    long mAntiAirChecks = 0, mAntiAirNear = 0, mAntiAirAny = 0;
    long mAntiAirAhead = 0, mAntiAirQueued = 0, mAntiAirSkipped = 0;
    long mAnswerWarChecks = 0, mAnswerWarTransitions = 0, mAnswerWarScheduled = 0;
    long mAnswerWarAnswers = 0, mAnswerWarCancelled = 0;
    long mSurrenderChecks = 0, mSurrenderCitizens = 0, mSurrenderNoCitizen = 0;
    long mSurrenderCanTrain = 0, mSurrenderAtWar = 0, mSurrenderResigned = 0;
    long mHospitalChecks = 0, mHospitals = 0, mHospitalSent = 0, mHospitalHolds = 0, mHospitalDone = 0;
    long mCoverChecks = 0, mCoverNear = 0, mCoverFights = 0, mCoverSent = 0, mCoverHolds = 0, mCoverBack = 0;
    long mCoastChecks = 0, mCoastSites = 0, mCoastCells = 0, mCoastSea = 0, mCoastPlaced = 0;
    long mScoutChecks = 0, mScoutTowns = 0, mScoutFoes = 0, mScoutCells = 0;
    long mScoutPicked = 0, mScoutSent = 0, mScoutBack = 0;
    long mFortChecks = 0, mFortSites = 0, mFortPicked = 0, mFortTowers = 0;
    long mFortBusy = 0, mFortWait = 0, mFortSkipped = 0, mFortExposed = 0;
    long mPopChecks = 0, mPopOverrides = 0, mPopWeighted = 0, mPopCap = 0;
    long mUnitsFirstChecks = 0, mUnitsFirstSwitched = 0;
    long mAnswerFireChecks = 0, mAnswerFireSent = 0;
    long mWatchdogChecks = 0, mWatchdogDrops = 0, mWatchdogKicks = 0;
    long mRaidLeashGroups = 0, mRaidLeashChecks = 0, mRaidLeashCalled = 0;
    long mTightBuildCalls = 0, mTightBuildSites = 0;
    long mLostChecks = 0, mLostSkipped = 0, mLostInReach = 0;
    long mLostRefreshes = 0, mLostCells = 0, mLostZones = 0;
    long mLostMemCells = 0, mLostMemFoes = 0;
    long mTowersQueued = 0, mTowersSkipped = 0;
    bool mHasGatherers = false;                          // a computer player: its gatherers and the gatherers it wants, food..iron
    long mGatherers[5] = {0, 0, 0, 0, 0}, mWanted[5] = {0, 0, 0, 0, 0};
    long mDropSites[7] = {0, 0, 0, 0, 0, 0, 0};          // the player's drop-site buildings by the resource they take (0 and 1: all)
    long mDropDistance[5] = {0, 0, 0, 0, 0};            // the gatherers' mean tile distance to the nearest drop site that takes their resource, x 10
    long mTechs = 0;                                     // the techs the player has researched
};

struct Sample {
    long mOverlapPairs = 0, mOverlapDeepest = 0;   // land units that overlap by 0.1 tile or more, and the deepest overlap x 100
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
// the land units (soldiers and citizens not at work, none held) whose circles, as wide as the game's size for them, overlap
void MeasureOverlap(Sample& ioSample)
{
    struct Disc {
        float mX, mY, mSize;
    };
    std::vector<Disc> theDiscs;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++) {
        EEPlayer* thePlayer = gWorld.GetPlayer(i);
        if (thePlayer == NULL)
            continue;
        for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
            if (theUnit->mHitPoints <= 0 || theUnit->mUnitClass == NULL || theUnit->mTileX < 0 || theUnit->mComplexUnit == NULL)
                continue;
            unsigned long theCategory = theUnit->mUnitClass->mCategory;
            if (theCategory != kUnitCategoryLand && theCategory != kUnitCategoryLandCitizen && theCategory != kUnitCategoryLand8
                && theCategory != kUnitCategoryLand15)
                continue;
            EEComplexUnit* theComplex = theUnit->mComplexUnit;
            if (theComplex->mHolder != NULL || theComplex->mCollisionRadius <= 0)
                continue;
            unsigned char theKind = theComplex->mGoalState != NULL ? theComplex->mGoalState->mKind : 0;
            if (theComplex->mIsCitizen && (theKind == 6 || theKind == 4))
                continue;
            Disc theDisc = {theComplex->mPhysicalModel.GetLocation().GetX(), theComplex->mPhysicalModel.GetLocation().GetY(),
                            theComplex->mCollisionRadius};
            theDiscs.push_back(theDisc);
        }
    }
    for (size_t a = 0; a < theDiscs.size() && a < 2000; a++)
        for (size_t b = a + 1; b < theDiscs.size() && b < 2000; b++) {
            float theDX = theDiscs[a].mX - theDiscs[b].mX, theDY = theDiscs[a].mY - theDiscs[b].mY;
            float theOverlap = (theDiscs[a].mSize + theDiscs[b].mSize) * 0.5f - sqrtf(theDX * theDX + theDY * theDY);
            if (theOverlap >= 0.1f)
                ioSample.mOverlapPairs++;
            if ((long)(theOverlap * 100) > ioSample.mOverlapDeepest)
                ioSample.mOverlapDeepest = (long)(theOverlap * 100);
        }
}

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
        if (!p.mHuman) {
            EEComputerPlayer* theComputer = dynamic_cast<EEComputerPlayer*>(thePlayer);
            if (theComputer != NULL && theComputer->mCivilizationManager != NULL && theComputer->mCivilizationManager->mResourceManager != NULL) {
                EECPResourceManager* theResources = theComputer->mCivilizationManager->mResourceManager;
                p.mHasGatherers = true;
                if (thePlayer->mTechnologies != NULL)
                    for (unsigned long e = 0; e < kTechEpochAll; e++)
                        for (std::map<long, EETechTreeNode*>::iterator n = thePlayer->mTechnologies->mEpochs[e].mNodes.begin();
                             n != thePlayer->mTechnologies->mEpochs[e].mNodes.end(); ++n)
                            if (n->second != NULL && n->second->mIsResearched)
                                p.mTechs++;
                struct Drop { long mX, mY, mType; };
                std::vector<Drop> theDrops;
                for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext))
                    if (theUnit->mHitPoints > 0 && theUnit->mUnitClass != NULL && theUnit->mUnitClass->IsDropSite() && theUnit->mComplexUnit != NULL
                        && !theUnit->mComplexUnit->IsFoundation()) {
                        Drop theDrop = {theUnit->mTileX, theUnit->mTileY, (long)theUnit->mUnitClass->mResourceType};
                        theDrops.push_back(theDrop);
                        if (theDrop.mType >= 0 && theDrop.mType < 7)
                            p.mDropSites[theDrop.mType]++;
                    }
                for (int r = 0; r < 5; r++) {
                    double theSum = 0;
                    long theN = 0;
                    for (size_t g = 0; g < theResources->mGatherers[r + 2].size(); g++) {
                        EEComplexUnit* theGatherer = theResources->mGatherers[r + 2][g];
                        double theBest = -1;
                        for (size_t d = 0; d < theDrops.size(); d++) {
                            if (theDrops[d].mType != r + 2 && theDrops[d].mType > 1)
                                continue;
                            double theDX = (double)(theDrops[d].mX - theGatherer->mTileX), theDY = (double)(theDrops[d].mY - theGatherer->mTileY);
                            double theDistance = sqrt(theDX * theDX + theDY * theDY);
                            if (theBest < 0 || theDistance < theBest)
                                theBest = theDistance;
                        }
                        if (theBest >= 0) {
                            theSum += theBest;
                            theN++;
                        }
                    }
                    p.mDropDistance[r] = theN > 0 ? (long)(theSum / theN * 10.0) : -1;
                }
                for (int r = 0; r < 5; r++) {
                    p.mGatherers[r] = (long)theResources->mGatherers[r + 2].size();
                    p.mWanted[r] = (long)theResources->mWantedGatherers[r + 2];
                }
            }
        }
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
            AgeOfAI::EarlyCounts theEarly = AgeOfAI::EarlyCountsOf(i);
            p.mEarlyQueued = (long)theEarly.mQueued;
            p.mEarlyBuilders = (long)theEarly.mBuilders;
            p.mEarlyBaits = (long)theEarly.mBaits;
            AgeOfAI::OutdatedCounts theOutdated = AgeOfAI::OutdatedCountsOf(i);
            p.mOutdatedSorties = (long)theOutdated.mSorties;
            p.mOutdatedSent = (long)theOutdated.mSent;
            p.mOutdatedRefused = (long)theOutdated.mRefused;
            p.mOutdatedNoGroup = (long)theOutdated.mNoGroup;
            AgeOfAI::CitizenAttackCounts theCitizen = AgeOfAI::CitizenAttackCountsOf(i);
            p.mCitizenCalls = (long)theCitizen.mCalls;
            p.mCitizenCalled = (long)theCitizen.mCalled;
            AgeOfAI::WallCounts theWall = AgeOfAI::WallCountsOf(i);
            p.mWallSkipped = (long)theWall.mSkipped;
            p.mWallRan = (long)theWall.mRan;
            AgeOfAI::RetargetCounts theRetarget = AgeOfAI::RetargetCountsOf(i);
            p.mRetargetScans = (long)theRetarget.mScans;
            p.mRetargetPicked = (long)theRetarget.mPicked;
            p.mRetargetGroups = (long)theRetarget.mSubGroups;
            AgeOfAI::AvoidTowerCounts theAvoid = AgeOfAI::AvoidTowerCountsOf(i);
            p.mAvoidCalls = (long)theAvoid.mCalls;
            p.mAvoidPenalised = (long)theAvoid.mPenalised;
            p.mAvoidCost = (long)theAvoid.mCost;
            AgeOfAI::RamWaitCounts theRam = AgeOfAI::RamWaitCountsOf(i);
            p.mRamCalls = (long)theRam.mCalls;
            p.mRamWaits = (long)theRam.mWaits;
            AgeOfAI::AirSkipCounts theAir = AgeOfAI::AirSkipCountsOf(i);
            p.mAirCalls = (long)theAir.mCalls;
            p.mAirSkipped = (long)theAir.mSkipped;
            AgeOfAI::LeashCounts theLeash = AgeOfAI::LeashCountsOf(i);
            p.mLeashCalls = (long)theLeash.mCalls;
            p.mLeashSkipped = (long)theLeash.mSkipped;
            AgeOfAI::TetherCounts theTether = AgeOfAI::TetherCountsOf(i);
            p.mTetherCalls = (long)theTether.mCalls;
            p.mTetherSkipped = (long)theTether.mSkipped;
            AgeOfAI::MassWaitCounts theMass = AgeOfAI::MassWaitCountsOf(i);
            p.mMassChecks = (long)theMass.mChecks;
            p.mMassWaits = (long)theMass.mWaits;
            p.mMassGoes = (long)theMass.mGoes;
            p.mMassWaited = (long)theMass.mWaited;
            AgeOfAI::CatchUpAIsCounts theCatch = AgeOfAI::CatchUpAIsCountsOf(i);
            p.mCatchChecks = (long)theCatch.mChecks;
            p.mCatchCalls = (long)theCatch.mCalls;
            AgeOfAI::GranaryCounts theGran = AgeOfAI::GranaryCountsOf(i);
            p.mGranChecks = (long)theGran.mChecks;
            p.mGranRequested = (long)theGran.mRequested;
            AgeOfAI::FishingCounts theFishing = AgeOfAI::FishingCountsOf(i);
            p.mFishCalls = (long)theFishing.mCalls;
            p.mFishPicked = (long)theFishing.mPicked;
            p.mFishChanged = (long)theFishing.mChanged;
            p.mFishClass = theFishing.mLastClass;
            AgeOfAI::MineCapCounts theMine = AgeOfAI::MineCapCountsOf(i);
            p.mMineCalls = (long)theMine.mCalls;
            p.mMineCapped = (long)theMine.mCapped;
            p.mMineMines = theMine.mMines;
            p.mMineCap = theMine.mCap;
            AgeOfAI::WaveFocusCounts theWave = AgeOfAI::WaveFocusCountsOf(i);
            p.mWaveCalls = (long)theWave.mCalls;
            p.mWaveSet = (long)theWave.mSet;
            p.mWaveMoved = (long)theWave.mMoved;
            p.mWaveKept = (long)theWave.mKept;
            p.mWaveNone = (long)theWave.mNone;
            AgeOfAI::WeakSpotCounts theWeak = AgeOfAI::WeakSpotCountsOf(i);
            p.mWeakCalls = (long)theWeak.mCalls;
            p.mWeakThreatened = (long)theWeak.mThreatened;
            p.mWeakPicked = (long)theWeak.mPicked;
            AgeOfAI::BreachCounts theBreach = AgeOfAI::BreachCountsOf(i);
            p.mBreachCalls = (long)theBreach.mCalls;
            p.mBreachCrossed = (long)theBreach.mCrossed;
            p.mBreachPicked = (long)theBreach.mPicked;
            p.mBreachWalls = (long)theBreach.mWalls;
            AgeOfAI::BuildFixCounts theBuildFix = AgeOfAI::BuildFixCountsOf(i);
            p.mBuildAttempts = (long)theBuildFix.mAttempts;
            p.mBuildFails = (long)theBuildFix.mFails;
            p.mBuildDropped = (long)theBuildFix.mDropped;
            p.mBuildTopType = (long)theBuildFix.mTopType;
            p.mBuildTopFails = (long)theBuildFix.mTopFails;
            AgeOfAI::RecoverCounts theRecover = AgeOfAI::RecoverCountsOf(i);
            p.mRecoverChecks = (long)theRecover.mChecks;
            p.mRecoverStarts = (long)theRecover.mStarts;
            p.mRecoverEnds = (long)theRecover.mEnds;
            p.mRecoverMoves = (long)theRecover.mMoves;
            p.mRecoverOn = (long)theRecover.mOn;
            p.mRecoverHadEconomy = (long)theRecover.mHadEconomy;
            p.mFoodPulled = (long)AgeOfAI::FoodCountsOf(i).mPulled;
            AgeOfAI::CounterFireCounts theCounterFire = AgeOfAI::CounterFireCountsOf(i);
            p.mCounterFireSorties = (long)theCounterFire.mSorties;
            p.mCounterFireSent = (long)theCounterFire.mSent;
            p.mCounterFireShort = (long)theCounterFire.mShort;
            p.mDefence = AgeOfAI::DefenceCountsOf(i);
            AgeOfAI::HomeCounts theHome = AgeOfAI::HomeCountsOf(i);
            p.mHomeSent = (long)theHome.mSent;
            p.mHomeBaseMoved = (long)theHome.mBaseMoved;
            AgeOfAI::GarrisonCounts theGarrison = AgeOfAI::GarrisonCountsOf(i);
            p.mGarrisonChecks = (long)theGarrison.mChecks;
            p.mGarrisonOrders = (long)theGarrison.mOrders;
            p.mGarrisonCitizens = (long)theGarrison.mCitizens;
            p.mGarrisonIdle = (long)theGarrison.mIdle;
            p.mGarrisonWood = (long)theGarrison.mWood;
            p.mGarrisonHolds = (long)theGarrison.mHolds;
            AgeOfAI::AntiAirCounts theAntiAir = AgeOfAI::AntiAirCountsOf(i);
            p.mAntiAirChecks = (long)theAntiAir.mChecks;
            p.mAntiAirNear = (long)theAntiAir.mNear;
            p.mAntiAirAny = (long)theAntiAir.mAny;
            p.mAntiAirAhead = (long)theAntiAir.mAhead;
            p.mAntiAirQueued = (long)theAntiAir.mQueued;
            p.mAntiAirSkipped = (long)theAntiAir.mSkipped;
            AgeOfAI::AnswerWarCounts theAnswerWar = AgeOfAI::AnswerWarCountsOf(i);
            p.mAnswerWarChecks = (long)theAnswerWar.mChecks;
            p.mAnswerWarTransitions = (long)theAnswerWar.mTransitions;
            p.mAnswerWarScheduled = (long)theAnswerWar.mScheduled;
            p.mAnswerWarAnswers = (long)theAnswerWar.mAnswers;
            p.mAnswerWarCancelled = (long)theAnswerWar.mCancelled;
            AgeOfAI::SurrenderCounts theSurrender = AgeOfAI::SurrenderCountsOf(i);
            p.mSurrenderChecks = (long)theSurrender.mChecks;
            p.mSurrenderCitizens = (long)theSurrender.mCitizens;
            p.mSurrenderNoCitizen = (long)theSurrender.mNoCitizen;
            p.mSurrenderCanTrain = (long)theSurrender.mCanTrain;
            p.mSurrenderAtWar = (long)theSurrender.mAtWar;
            p.mSurrenderResigned = (long)theSurrender.mResigned;
            AgeOfAI::HospitalCounts theHospital = AgeOfAI::HospitalCountsOf(i);
            p.mHospitalChecks = (long)theHospital.mChecks;
            p.mHospitals = (long)theHospital.mHospitals;
            p.mHospitalSent = (long)theHospital.mSent;
            p.mHospitalHolds = (long)theHospital.mHolds;
            p.mHospitalDone = (long)theHospital.mDone;
            AgeOfAI::CoverCounts theCover = AgeOfAI::CoverCountsOf(i);
            p.mCoverChecks = (long)theCover.mChecks;
            p.mCoverNear = (long)theCover.mNear;
            p.mCoverFights = (long)theCover.mFights;
            p.mCoverSent = (long)theCover.mSent;
            p.mCoverHolds = (long)theCover.mHolds;
            p.mCoverBack = (long)theCover.mBack;
            AgeOfAI::CoastCounts theCoast = AgeOfAI::CoastCountsOf(i);
            p.mCoastChecks = (long)theCoast.mChecks;
            p.mCoastSites = (long)theCoast.mSites;
            p.mCoastCells = (long)theCoast.mCells;
            p.mCoastSea = (long)theCoast.mSea;
            p.mCoastPlaced = (long)theCoast.mPlaced;
            AgeOfAI::ScoutCounts theScout = AgeOfAI::ScoutCountsOf(i);
            p.mScoutChecks = (long)theScout.mChecks;
            p.mScoutTowns = (long)theScout.mTowns;
            p.mScoutFoes = (long)theScout.mFoes;
            p.mScoutCells = (long)theScout.mCells;
            p.mScoutPicked = (long)theScout.mPicked;
            p.mScoutSent = (long)theScout.mSent;
            p.mScoutBack = (long)theScout.mBack;
            AgeOfAI::SiteFortCounts theForts = AgeOfAI::SiteFortCountsOf(i);
            p.mFortChecks = (long)theForts.mChecks;
            p.mFortSites = (long)theForts.mSites;
            p.mFortPicked = (long)theForts.mPicked;
            p.mFortTowers = (long)theForts.mTowers;
            p.mFortBusy = (long)theForts.mBusy;
            p.mFortWait = (long)theForts.mWait;
            p.mFortSkipped = (long)theForts.mSkipped;
            p.mFortExposed = (long)theForts.mExposed;
            AgeOfAI::PopCapCounts thePopCap = AgeOfAI::PopCapCountsOf(i);
            p.mPopChecks = (long)thePopCap.mChecks;
            p.mPopOverrides = (long)thePopCap.mOverrides;
            p.mPopWeighted = (long)thePopCap.mWeighted;
            p.mPopCap = (long)thePopCap.mCap;
            AgeOfAI::UnitsFirstCounts theUnitsFirst = AgeOfAI::UnitsFirstCountsOf(i);
            p.mUnitsFirstChecks = (long)theUnitsFirst.mChecks;
            p.mUnitsFirstSwitched = (long)theUnitsFirst.mSwitched;
            AgeOfAI::AnswerFireCounts theAnswerFire = AgeOfAI::AnswerFireCountsOf(i);
            p.mAnswerFireChecks = (long)theAnswerFire.mChecks;
            p.mAnswerFireSent = (long)theAnswerFire.mSent;
            AgeOfAI::TransportWatchdogCounts theWatchdog = AgeOfAI::TransportWatchdogCountsOf(i);
            p.mWatchdogChecks = (long)theWatchdog.mChecks;
            p.mWatchdogDrops = (long)theWatchdog.mDrops;
            p.mWatchdogKicks = (long)theWatchdog.mKicks;
            AgeOfAI::RaidLeashCounts theRaidLeash = AgeOfAI::RaidLeashCountsOf(i);
            p.mRaidLeashGroups = (long)theRaidLeash.mGroups;
            p.mRaidLeashChecks = (long)theRaidLeash.mChecks;
            p.mRaidLeashCalled = (long)theRaidLeash.mCalled;
            AgeOfAI::TightBuildCounts theTightBuild = AgeOfAI::TightBuildCountsOf(i);
            p.mTightBuildCalls = (long)theTightBuild.mCalls;
            p.mTightBuildSites = (long)theTightBuild.mSites;
            AgeOfAI::AvoidLostCounts theLost = AgeOfAI::AvoidLostCountsOf(i);
            p.mLostChecks = (long)theLost.mChecks;
            p.mLostSkipped = (long)theLost.mSkipped;
            p.mLostInReach = (long)theLost.mInReach;
            AgeOfAI::AvoidLostZoneCounts theLostZones = AgeOfAI::AvoidLostZoneCountsOf(i);
            p.mLostRefreshes = (long)theLostZones.mRefreshes;
            p.mLostCells = (long)theLostZones.mCells;
            p.mLostZones = (long)theLostZones.mZones;
            p.mLostMemCells = (long)theLostZones.mMemCells;
            p.mLostMemFoes = (long)theLostZones.mMemFoes;
            AgeOfAI::TowerLayoutCounts theTowers = AgeOfAI::TowerLayoutCountsOf(i);
            p.mTowersQueued = (long)theTowers.mQueued;
            p.mTowersSkipped = (long)theTowers.mSkipped;
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
    MeasureOverlap(theSample);
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
long sNextArmies = -1;   // the game millisecond of the next armies
bool sWallsSpawned = false;


std::string LowerCase(std::string inText)
{
    for (char& c : inText)
        c = (char)tolower((unsigned char)c);
    return inText;
}

// The first unit class whose name has inName in it (without case), land or flying as asked, or NULL
EEUnitClass* FindUnitClass(const std::string& inName, bool inAir)
{
    for (unsigned long i = 0; i < 871; i++) {
        EEUnitClass* theClass = gUnitClassTable[i];
        if (theClass == NULL)
            continue;
        if (inAir) {
            if (theClass->mCategory != kUnitCategoryAirplane && theClass->mCategory != kUnitCategoryHelicopter
                && theClass->mCategory != kUnitCategoryHelicopterTransport)
                continue;
        } else if (theClass->mCategory != kUnitCategoryLand)
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

// Spawn one block of units of a class, in rows of 12 tiles from the given tile, and send them at the target.
long SpawnUnits(EEUnitClass* inClass, long inCount, unsigned long inOwner, EEUnit* inTarget,
                long inFromX, long inFromY, long& ioSlot)
{
    if (inClass == NULL)
        return 0;
    long theMade = 0;
    for (long n = 0; n < inCount; ioSlot++) {
        if (ioSlot > 4000)
            break;
        long theX = inFromX + (ioSlot % 12) - 6, theY = inFromY + (ioSlot / 12);
        EEUnit* theUnit = inClass->CreateUnit(theX, theY, (long)inOwner, false, NULL);
        if (theUnit == NULL || theUnit->mComplexUnit == NULL)
            continue;
        theUnit->mComplexUnit->SetGoal(new EEUGAttackUnit(theUnit->mComplexUnit, inTarget, false, false), false, 0);
        n++;
        theMade++;
    }
    return theMade;
}

// --army and --army-air: at the given game second, each computer player gets the armies of the options, a block of
// units six tiles from its buildings towards the next computer player, each sent to attack that player's nearest
// building. The units come from the first class of that name, land or flying as asked, the same for every player: a
// fair fight on any map.
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
        if (sOptions.mArmyRaid) {
            // Eight tiles from the building the army attacks, on the side of its own base: the distance to the target
            // is the same for both sides, so neither raid lands closer to what it came for.
            double theHomeX = theBaseX - theTarget->mTileX, theHomeY = theBaseY - theTarget->mTileY;
            double theHomeLength = sqrt(theHomeX * theHomeX + theHomeY * theHomeY);
            if (theHomeLength < 1)
                continue;
            theFromX = theTarget->mTileX + (long)lround(theHomeX / theHomeLength * 8);
            theFromY = theTarget->mTileY + (long)lround(theHomeY / theHomeLength * 8);
        }
        if (sOptions.mArmySpot) {
            theFromX = sOptions.mArmySpotX;
            theFromY = sOptions.mArmySpotY;
        }
        long theMade = 0, theSlot = 0;
        for (const std::pair<std::string, long>& theArmy : sOptions.mArmies)
            theMade += SpawnUnits(FindUnitClass(theArmy.first, false), theArmy.second, thePlayers[k], theTarget,
                                  theFromX, theFromY, theSlot);
        for (const std::pair<std::string, long>& theArmy : sOptions.mAirArmies)
            theMade += SpawnUnits(FindUnitClass(theArmy.first, true), theArmy.second, thePlayers[k], theTarget,
                                  theFromX, theFromY, theSlot);
        Note("ARMY player %u: %d units at %d,%d, to attack player %u's building at %d,%d", (unsigned)thePlayers[k], (int)theMade,
             (int)theFromX, (int)theFromY, (unsigned)thePlayers[(k + 1) % thePlayers.size()], (int)theTarget->mTileX,
             (int)theTarget->mTileY);
    }
}

// The first wall class of the game's data (the family of walls, not the AI's own notion of a fortification)
EEUnitClass* FindWallClass()
{
    for (unsigned long i = 0; i < 871; i++) {
        EEUnitClass* theClass = gUnitClassTable[i];
        if (theClass != NULL && theClass->mFamily == kUnitFamilyWalls)
            return theClass;
    }
    return NULL;
}

// --wall: at the given second each defender gets a line of wall segments three tiles from the next attacker's base,
// laid across the direction to that attacker's enemy. The attacker sees them beside its own base and its land attack
// routes cross them, so Age of AI's attack_breach rule can be exercised in a real game.
void SpawnWalls()
{
    sWallsSpawned = true;
    EEUnitClass* theWall = FindWallClass();
    if (theWall == NULL)
        return;
    std::vector<unsigned long> thePlayers;
    for (unsigned long i = 1; i < gWorld.GetNumPlayers(); i++)
        if ((long)i != gWorld.mCurrentPlayer && gWorld.GetPlayer(i) != NULL && gWorld.GetPlayer(i)->mFirstUnit != NULL)
            thePlayers.push_back(i);
    for (size_t k = 0; k < thePlayers.size() && thePlayers.size() > 1; k++) {
        EEPlayer* theAttacker = gWorld.GetPlayer(thePlayers[k]);
        EEPlayer* theDefender = gWorld.GetPlayer(thePlayers[(k + 1) % thePlayers.size()]);
        long theAX, theAY, theEX, theEY;
        if (!BaseOf(theAttacker, theAX, theAY) || !BaseOf(theDefender, theEX, theEY))
            continue;
        double theDirX = theEX - theAX, theDirY = theEY - theAY;
        double theLength = sqrt(theDirX * theDirX + theDirY * theDirY);
        if (theLength < 1)
            continue;
        double theUX = theDirX / theLength, theUY = theDirY / theLength;
        long theMade = 0;
        for (long n = 0; n < sOptions.mWallCount; n++) {
            long theOffset = n - (sOptions.mWallCount - 1) / 2;
            long theX = theAX + (long)lround(theUX * 3.0 - theUY * theOffset);
            long theY = theAY + (long)lround(theUY * 3.0 + theUX * theOffset);
            if (theWall->CreateUnit(theX, theY, (long)theDefender->mPlayerID, false, NULL) != NULL)
                theMade++;
        }
        Note("WALL player %u: %d segments owned by player %u near %d,%d", (unsigned)theAttacker->mPlayerID, (int)theMade,
             (unsigned)theDefender->mPlayerID, (int)theAX, (int)theAY);
    }
}

// Kill a player's citizens, the way an overrun town loses its workers. Collect them first: Kill unlinks a unit from
// its owner's list, so killing while walking that list would skip the next one.
void StarvePlayer(Starve& inStarve)
{
    inStarve.mDone = true;
    EEPlayer* thePlayer = inStarve.mPlayer >= 0 && inStarve.mPlayer < (long)gWorld.GetNumPlayers()
        ? gWorld.GetPlayer((unsigned long)inStarve.mPlayer) : NULL;
    if (thePlayer == NULL)
        return;
    std::vector<EEUnit*> theCitizens, theCentres;
    for (EEUnit* theUnit = thePlayer->mFirstUnit; theUnit != NULL; theUnit = static_cast<EEUnit*>(theUnit->mNext)) {
        if (theUnit->mUnitClass == NULL)
            continue;
        unsigned long theCategory = theUnit->mUnitClass->mCategory;
        if (theCategory == kUnitCategoryLandCitizen || theCategory == kUnitCategoryNavalCitizen)
            theCitizens.push_back(theUnit);
        else if (inStarve.mRaze && (theUnit->mUnitClass->mUnitType == kUnitTypeTownCenter
                 || theUnit->mUnitClass->mUnitType == kUnitTypeCapitol
                 || theUnit->mUnitClass->mUnitType == kUnitTypeSettlement))
            theCentres.push_back(theUnit);
    }
    for (EEUnit* theUnit : theCitizens)
        theUnit->Kill(0, 0, 0);
    for (EEUnit* theUnit : theCentres)
        theUnit->Kill(0, 0, 0);
    Note("%s player %ld at game ms %ld: %zu citizens, %zu town centres killed",
         inStarve.mRaze ? "RAZE" : "STARVE", inStarve.mPlayer, gPhysics->mGameTime,
         theCitizens.size(), theCentres.size());
}

// the physics thread, after each communications tick, under the physics lock
void AfterTick()
{
    long theTime = gPhysics->mGameTime;
    unsigned long theCount = gWorld.GetNumPlayers();
    if (sNextArmies < 0)
        sNextArmies = (long)(sOptions.mArmyAt * 1000);
    if ((!sOptions.mArmies.empty() || !sOptions.mAirArmies.empty()) && theTime >= sNextArmies
        && (!sArmiesSpawned || sOptions.mArmyEvery > 0)) {
        SpawnArmies();
        sNextArmies = theTime + (long)(sOptions.mArmyEvery * 1000);
    }
    if (sOptions.mWallAt >= 0 && !sWallsSpawned && theTime >= (long)(sOptions.mWallAt * 1000))
        SpawnWalls();
    for (Starve& theStarve : sOptions.mStarves) {
        if (!theStarve.mDone && theTime >= (long)(theStarve.mAt * 1000))
            StarvePlayer(theStarve);
    }
    for (StanceChange& theChange : sOptions.mStances) {
        if (theChange.mDone || theTime < (long)(theChange.mAt * 1000))
            continue;
        theChange.mDone = true;
        gWorld.SetDiplomaticStance((unsigned long)theChange.mFrom, theChange.mTo, theChange.mValue);
        Note("STANCE at game ms %ld: player %ld toward player %ld = %ld", theTime, theChange.mFrom, theChange.mTo,
             theChange.mValue);
    }
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
        char theIntel[4096] = "";
        if (p.mHasIntel)
            snprintf(theIntel, sizeof(theIntel),
                     ", \"intel\": {\"records\": %ld, \"buildings\": %ld, \"soldiers\": %ld, \"seen_now\": %ld, "
                     "\"cells_looked\": %ld, \"army\": %ld, \"early_towers_queued\": %ld, \"early_builders\": %ld, \"early_baits\": %ld, "
                     "\"outdated_sorties\": %ld, \"outdated_sent\": %ld, \"outdated_refused\": %ld, \"outdated_no_group\": %ld, "
                     "\"citizen_calls\": %ld, \"citizen_called\": %ld, "
                     "\"wall_skipped\": %ld, \"wall_ran\": %ld, "
                     "\"retarget_scans\": %ld, \"retarget_picked\": %ld, \"retarget_groups\": %ld, "
                     "\"avoid_calls\": %ld, \"avoid_penalised\": %ld, \"avoid_cost\": %ld, "
                     "\"ram_calls\": %ld, \"ram_waits\": %ld, "
                     "\"air_calls\": %ld, \"air_skipped\": %ld, "
                     "\"leash_calls\": %ld, \"leash_skipped\": %ld, "
                     "\"tether_calls\": %ld, \"tether_skipped\": %ld, "
                     "\"mass_checks\": %ld, \"mass_waits\": %ld, \"mass_goes\": %ld, \"mass_waited\": %ld, "
                     "\"catch_checks\": %ld, \"catch_calls\": %ld, "
                     "\"granary_checks\": %ld, \"granary_requested\": %ld, "
                     "\"fish_calls\": %ld, \"fish_picked\": %ld, \"fish_changed\": %ld, \"fish_class\": %ld, "
                     "\"mine_calls\": %ld, \"mine_capped\": %ld, \"mine_mines\": %ld, \"mine_cap\": %ld, "
                     "\"wave_calls\": %ld, \"wave_set\": %ld, \"wave_moved\": %ld, \"wave_kept\": %ld, \"wave_none\": %ld, "
                     "\"weak_calls\": %ld, \"weak_threatened\": %ld, \"weak_picked\": %ld, "
                     "\"breach_calls\": %ld, \"breach_crossed\": %ld, \"breach_picked\": %ld, \"breach_walls\": %ld, "
                     "\"build_attempts\": %ld, \"build_fails\": %ld, \"build_dropped\": %ld, \"build_top_type\": %ld, \"build_top_fails\": %ld, "
                     "\"recover_checks\": %ld, \"recover_starts\": %ld, \"recover_ends\": %ld, \"recover_moves\": %ld, "
                     "\"recover_on\": %ld, \"recover_had_economy\": %ld, "
                     "\"food_pulled\": %ld, "
                     "\"counter_fire_sorties\": %ld, \"counter_fire_sent\": %ld, \"counter_fire_short\": %ld, "
                     "\"militia_orders\": %u, \"militia_refused\": %u, \"militia_caught\": %u, \"counter_siege_orders\": %u, "
                     "\"counter_siege_skipped\": %u, \"home_sent\": %ld, \"home_base_moved\": %ld, "
                     "\"garrison_checks\": %ld, \"garrison_orders\": %ld, \"garrison_citizens\": %ld, "
                     "\"garrison_idle\": %ld, \"garrison_wood\": %ld, \"garrison_holds\": %ld, "
                     "\"anti_air_checks\": %ld, \"anti_air_near\": %ld, \"anti_air_any\": %ld, "
                     "\"anti_air_ahead\": %ld, \"anti_air_queued\": %ld, \"anti_air_skipped\": %ld, "
                     "\"answer_war_checks\": %ld, \"answer_war_transitions\": %ld, \"answer_war_scheduled\": %ld, "
                     "\"answer_war_answers\": %ld, \"answer_war_cancelled\": %ld, "
                     "\"surrender_checks\": %ld, \"surrender_citizens\": %ld, \"surrender_no_citizen\": %ld, "
                     "\"surrender_can_train\": %ld, \"surrender_at_war\": %ld, \"surrender_resigned\": %ld, "
                     "\"hospital_checks\": %ld, \"hospitals\": %ld, \"hospital_sent\": %ld, \"hospital_holds\": %ld, \"hospital_done\": %ld, "
                     "\"cover_checks\": %ld, \"cover_near\": %ld, \"cover_fights\": %ld, \"cover_sent\": %ld, \"cover_holds\": %ld, \"cover_back\": %ld, "
                     "\"coast_checks\": %ld, \"coast_sites\": %ld, \"coast_cells\": %ld, \"coast_sea\": %ld, \"coast_placed\": %ld, "
                     "\"scout_checks\": %ld, \"scout_towns\": %ld, \"scout_foes\": %ld, \"scout_cells\": %ld, "
                     "\"scout_picked\": %ld, \"scout_sent\": %ld, \"scout_back\": %ld, "
                     "\"fort_checks\": %ld, \"fort_sites\": %ld, \"fort_picked\": %ld, \"fort_towers\": %ld, "
                     "\"fort_busy\": %ld, \"fort_wait\": %ld, \"fort_skipped\": %ld, \"fort_exposed\": %ld, "
                     "\"pop_checks\": %ld, \"pop_overrides\": %ld, \"pop_weighted\": %ld, \"pop_cap\": %ld, "
                     "\"units_first_checks\": %ld, \"units_first_switched\": %ld, "
                     "\"answer_fire_checks\": %ld, \"answer_fire_sent\": %ld, "
                     "\"watchdog_checks\": %ld, \"watchdog_drops\": %ld, \"watchdog_kicks\": %ld, "
                     "\"raid_leash_groups\": %ld, \"raid_leash_checks\": %ld, \"raid_leash_called\": %ld, "
                     "\"tight_build_calls\": %ld, \"tight_build_sites\": %ld, "
                     "\"lost_checks\": %ld, \"lost_skipped\": %ld, \"lost_in_reach\": %ld, "
                     "\"lost_refreshes\": %ld, \"lost_cells\": %ld, \"lost_zones\": %ld, "
                     "\"lost_mem_cells\": %ld, \"lost_mem_foes\": %ld, "
                     "\"towers_queued\": %ld, \"towers_skipped\": %ld}",
                     p.mIntelRecords, p.mIntelBuildings, p.mIntelSoldiers, p.mIntelSeenNow, p.mIntelCells, p.mIntelArmy,
                     p.mEarlyQueued, p.mEarlyBuilders, p.mEarlyBaits, p.mOutdatedSorties, p.mOutdatedSent, p.mOutdatedRefused,
                     p.mOutdatedNoGroup, p.mCitizenCalls, p.mCitizenCalled, p.mWallSkipped, p.mWallRan, p.mRetargetScans, p.mRetargetPicked, p.mRetargetGroups,
                     p.mAvoidCalls, p.mAvoidPenalised, p.mAvoidCost,
                     p.mRamCalls, p.mRamWaits,
                     p.mAirCalls, p.mAirSkipped,
                     p.mLeashCalls, p.mLeashSkipped,
                     p.mTetherCalls, p.mTetherSkipped,
                     p.mMassChecks, p.mMassWaits, p.mMassGoes, p.mMassWaited,
                     p.mCatchChecks, p.mCatchCalls,
                     p.mGranChecks, p.mGranRequested,
                     p.mFishCalls, p.mFishPicked, p.mFishChanged, p.mFishClass,
                     p.mMineCalls, p.mMineCapped, p.mMineMines, p.mMineCap,
                     p.mWaveCalls, p.mWaveSet, p.mWaveMoved, p.mWaveKept, p.mWaveNone,
                     p.mWeakCalls, p.mWeakThreatened, p.mWeakPicked,
                     p.mBreachCalls, p.mBreachCrossed, p.mBreachPicked, p.mBreachWalls,
                     p.mBuildAttempts, p.mBuildFails, p.mBuildDropped, p.mBuildTopType, p.mBuildTopFails,
                     p.mRecoverChecks, p.mRecoverStarts, p.mRecoverEnds, p.mRecoverMoves,
                     p.mRecoverOn, p.mRecoverHadEconomy,
                     p.mFoodPulled, p.mCounterFireSorties, p.mCounterFireSent, p.mCounterFireShort,
                     (unsigned)p.mDefence.mMilitiaOrders, (unsigned)p.mDefence.mMilitiaRefused, (unsigned)p.mDefence.mMilitiaCaught,
                     (unsigned)p.mDefence.mCounterSiegeOrders, (unsigned)p.mDefence.mCounterSiegeSkipped,
                     p.mHomeSent, p.mHomeBaseMoved,
                     p.mGarrisonChecks, p.mGarrisonOrders, p.mGarrisonCitizens,
                     p.mGarrisonIdle, p.mGarrisonWood, p.mGarrisonHolds,
                     p.mAntiAirChecks, p.mAntiAirNear, p.mAntiAirAny,
                     p.mAntiAirAhead, p.mAntiAirQueued, p.mAntiAirSkipped,
                     p.mAnswerWarChecks, p.mAnswerWarTransitions, p.mAnswerWarScheduled,
                     p.mAnswerWarAnswers, p.mAnswerWarCancelled,
                     p.mSurrenderChecks, p.mSurrenderCitizens, p.mSurrenderNoCitizen,
                     p.mSurrenderCanTrain, p.mSurrenderAtWar, p.mSurrenderResigned,
                     p.mHospitalChecks, p.mHospitals, p.mHospitalSent, p.mHospitalHolds, p.mHospitalDone,
                     p.mCoverChecks, p.mCoverNear, p.mCoverFights, p.mCoverSent, p.mCoverHolds, p.mCoverBack,
                     p.mCoastChecks, p.mCoastSites, p.mCoastCells, p.mCoastSea, p.mCoastPlaced,
                     p.mScoutChecks, p.mScoutTowns, p.mScoutFoes, p.mScoutCells,
                     p.mScoutPicked, p.mScoutSent, p.mScoutBack,
                     p.mFortChecks, p.mFortSites, p.mFortPicked, p.mFortTowers,
                     p.mFortBusy, p.mFortWait, p.mFortSkipped, p.mFortExposed,
                     p.mPopChecks, p.mPopOverrides, p.mPopWeighted, p.mPopCap,
                     p.mUnitsFirstChecks, p.mUnitsFirstSwitched,
                     p.mAnswerFireChecks, p.mAnswerFireSent,
                     p.mWatchdogChecks, p.mWatchdogDrops, p.mWatchdogKicks,
                     p.mRaidLeashGroups, p.mRaidLeashChecks, p.mRaidLeashCalled,
                     p.mTightBuildCalls, p.mTightBuildSites,
                     p.mLostChecks, p.mLostSkipped, p.mLostInReach,
                     p.mLostRefreshes, p.mLostCells, p.mLostZones,
                     p.mLostMemCells, p.mLostMemFoes,
                     p.mTowersQueued, p.mTowersSkipped);
        char theGatherers[480] = "";
        if (p.mHasGatherers)
            snprintf(theGatherers, sizeof(theGatherers),
                     ", \"gatherers\": [%d, %d, %d, %d, %d], \"wanted\": [%d, %d, %d, %d, %d], \"drop_sites\": [%d, %d, %d, %d, %d, %d, %d], \"drop_distance_x10\": [%d, %d, %d, %d, %d], \"techs\": %d", (int)p.mGatherers[0],
                     (int)p.mGatherers[1], (int)p.mGatherers[2], (int)p.mGatherers[3], (int)p.mGatherers[4], (int)p.mWanted[0],
                     (int)p.mWanted[1], (int)p.mWanted[2], (int)p.mWanted[3], (int)p.mWanted[4], (int)p.mDropSites[0], (int)p.mDropSites[1],
                     (int)p.mDropSites[2], (int)p.mDropSites[3], (int)p.mDropSites[4], (int)p.mDropSites[5], (int)p.mDropSites[6],
                     (int)p.mDropDistance[0], (int)p.mDropDistance[1], (int)p.mDropDistance[2], (int)p.mDropDistance[3], (int)p.mDropDistance[4], (int)p.mTechs);
        // The record grows with the empire (a long game's intel and gatherer lines are long), so the room it gets is
        // the room its parts took, not a fixed one a large sample would be cut in the middle of.
        std::string theText(sizeof theIntel + sizeof theGatherers + 512, '\0');
        int theLength = snprintf(&theText[0], theText.size(),
                 "%s{\"index\": %lu, \"engine\": \"%s\", \"human\": %s, \"result\": %lu, \"epoch\": %ld, \"population\": %ld, "
                 "\"victory_units\": %ld, \"citizens\": %ld, \"army\": %ld, \"buildings\": %ld, \"strength\": %ld, "
                 "\"value\": %ld, \"resources\": {\"food\": %ld, \"wood\": %ld, \"stone\": %ld, \"gold\": %ld, "
                 "\"iron\": %ld}%s%s}",
                 k ? ", " : "", p.mIndex,
                 p.mHuman                                                           ? "human"
                 : AgeOfAI::EngineOf(p.mIndex) == AgeOfAI::kEngineAgeOfAI ? "ageofai"
                                                                               : "classic",
                 p.mHuman ? "true" : "false", p.mResult, p.mEpoch, p.mPopulation,
                 p.mVictoryUnits, p.mCitizens, p.mArmy, p.mBuildings, p.mStrength, p.mValue, p.mResources[0],
                 p.mResources[1], p.mResources[2], p.mResources[3], p.mResources[4], theIntel, theGatherers);
        theText.resize(theLength > 0 ? (size_t)theLength : 0);
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
        const AgeOfAI::SpreadCounts& theSpread = AgeOfAI::MissileSpreadCounts();
        char theProjectiles[256];
        snprintf(theProjectiles, sizeof(theProjectiles),
                 "  \"projectiles\": {\"fired\": %u, \"led\": %u, \"hit_unit\": %u, \"hit_ground\": %u, \"missiles_led\": %u, "
                 "\"spread_launched\": %u, \"spread_held\": %u, \"spread_switched\": %u},\n", (unsigned)theCounts.mFired,
                 (unsigned)theCounts.mLed, (unsigned)theCounts.mHitUnit, (unsigned)theCounts.mHitGround, (unsigned)theCounts.mMissilesLed,
                 (unsigned)theSpread.mLaunched, (unsigned)theSpread.mHeld, (unsigned)theSpread.mSwitched);
        j += theProjectiles;
    }
    {
        long thePairs = 0, theDeepest = 0, theCount = 0;
        for (const Sample& theSample : sSamples) {
            thePairs += theSample.mOverlapPairs;
            theCount++;
            if (theSample.mOverlapDeepest > theDeepest)
                theDeepest = theSample.mOverlapDeepest;
        }
        char theOverlap[200];
        snprintf(theOverlap, sizeof(theOverlap), "  \"overlap\": {\"pairs_mean\": %.1f, \"deepest_x100\": %d, \"samples\": %d},\n",
                 theCount ? (double)thePairs / (double)theCount : 0.0, (int)theDeepest, (int)theCount);
        j += theOverlap;
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
    // The report goes beside its name and is renamed into place, so a reader never sees half of one. The temporary
    // name carries this process: two sims told to write one report each write their own, and the last rename wins with
    // a whole file, where a shared temporary left the two halves of a report mixed together.
    std::string theTemporary = sOptions.mReport + ".tmp." + std::to_string((long)getpid());
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
    "  --army-air NAME:COUNT  the same with the first flying class with NAME in its name (a test of air targets)\n"
    "  --army-every S         the armies appear again every S game seconds (a raid after raid)\n"
    "  --army-at S            the game second the armies appear (default 120)\n"
    "  --army-raid            the armies appear eight tiles from the enemy's base, on the side of their own\n"
    "  --army-spot X,Y        the armies appear at this tile, even off the map (a test of units put off it)\n"
    "  --classic-players L    player numbers, comma apart, that play the classic AI whatever ageofai.ini says\n"
    "  --player-ini P=FILE    player P reads its Age of AI settings from FILE in full, not from ageofai.ini\n"
    "  --lobby CONTROL=VALUE  any lobby combo of form 26 by line name or #index (e.g. 209=#2)\n"
    "  --stance S,F,T,V        at game second S set player F's stance toward T: V 0 ally, 1 enemy (repeatable)\n"
    "  --wall S[,N]           at game second S each defender gets N wall segments near the next attacker's base\n"
    "                         (default 9), so its land attack routes cross a wall it can see; exercises attack_breach\n"
    "  --starve P,S           at game second S kill every citizen of player P; exercises the surrender rule\n"
    "  --raze P,S             the same, and also kill P's Town Centres/Capitols/Settlements so it cannot retrain\n"
    "  --keep-alive P         keep P in the game after losing its last town centre, so its 60 s surrender timer can run\n"
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
        else if (theOption == "--army" || theOption == "--army-air") {
            bool theAir = theOption == "--army-air";
            std::string theText = value();
            size_t theColon = theText.rfind(':');
            long theCount = theColon == std::string::npos ? 0 : atol(theText.c_str() + theColon + 1);
            if (theCount < 1 || theColon == 0) {
                fprintf(stderr, "ee-aoc --sim: %s takes NAME:COUNT, a part of a %s unit's name and how many\n",
                        theOption.c_str(), theAir ? "flying" : "land");
                return 64;
            }
            (theAir ? sOptions.mAirArmies : sOptions.mArmies).push_back(
                std::make_pair(theText.substr(0, theColon), theCount));
        } else if (theOption == "--army-every")
            sOptions.mArmyEvery = atof(value().c_str());
        else if (theOption == "--army-raid")
            sOptions.mArmyRaid = true;
        else if (theOption == "--army-spot") {
            if (sscanf(value().c_str(), "%ld,%ld", &sOptions.mArmySpotX, &sOptions.mArmySpotY) != 2) {
                fprintf(stderr, "ee-aoc --sim: --army-spot takes X,Y\n");
                return 64;
            }
            sOptions.mArmySpot = true;
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
        else if (theOption == "--player-ini") {
            std::string theValue = value();
            size_t theEqual = theValue.find('=');
            long thePlayer = theEqual == std::string::npos ? 0 : atol(theValue.substr(0, theEqual).c_str());
            if (thePlayer < 1 || thePlayer > 15) {
                fprintf(stderr, "ee-aoc --sim: --player-ini P=FILE takes a player number 1..15\n");
                return 64;
            }
            AgeOfAI::SetPlayerSettings((unsigned long)thePlayer, theValue.substr(theEqual + 1).c_str());
        }
        else if (theOption == "--lobby") {
            std::string theValue = value();
            size_t theEqual = theValue.find('=');
            if (theEqual == std::string::npos) {
                fprintf(stderr, "ee-aoc --sim: --lobby CONTROL=VALUE\n");
                return 64;
            }
            sOptions.mLobby.push_back({atol(theValue.substr(0, theEqual).c_str()), theValue.substr(theEqual + 1)});
        } else if (theOption == "--stance") {
            std::string theValue = value();
            StanceChange theChange;
            if (sscanf(theValue.c_str(), "%lf,%ld,%ld,%ld", &theChange.mAt, &theChange.mFrom, &theChange.mTo,
                       &theChange.mValue) != 4
                || theChange.mAt < 0 || theChange.mFrom < 1 || theChange.mFrom > 15 || theChange.mTo < 1
                || theChange.mTo > 15 || theChange.mFrom == theChange.mTo
                || (theChange.mValue != 0 && theChange.mValue != 1)) {
                fprintf(stderr, "ee-aoc --sim: --stance takes SECOND,FROM,TO,VALUE; VALUE 0 ally, 1 enemy\n");
                return 64;
            }
            sOptions.mStances.push_back(theChange);
        } else if (theOption == "--starve" || theOption == "--raze") {
            Starve theStarve;
            theStarve.mRaze = theOption == "--raze";
            if (sscanf(value().c_str(), "%d,%lf", &theStarve.mPlayer, &theStarve.mAt) != 2
                || theStarve.mPlayer < 1 || theStarve.mPlayer > 15 || theStarve.mAt < 0) {
                fprintf(stderr, "ee-aoc --sim: %s takes PLAYER,SECOND; PLAYER 1..15\n", theOption.c_str());
                return 64;
            }
            sOptions.mStarves.push_back(theStarve);
        } else if (theOption == "--wall") {
            std::string theValue = value();
            long theCount = 9;
            double theAt = -1;
            if (sscanf(theValue.c_str(), "%lf,%ld", &theAt, &theCount) < 1 || theAt < 0 || theCount < 1 || theCount > 40) {
                fprintf(stderr, "ee-aoc --sim: --wall takes SECOND[,COUNT] with COUNT 1 to 40\n");
                return 64;
            }
            sOptions.mWallAt = theAt;
            sOptions.mWallCount = theCount;
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
        else if (theOption == "--keep-alive") {
            long thePlayer = atol(value().c_str());
            if (thePlayer < 1 || thePlayer > 31) {
                fprintf(stderr, "ee-aoc --sim: --keep-alive takes a player 1..31\n");
                return 64;
            }
            sOptions.mKeepAliveMask |= 1u << thePlayer;
        }
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
    gPlatSimKeepAliveMask = sOptions.mKeepAliveMask;
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
