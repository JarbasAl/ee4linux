// PACKED BUILDINGS ([ai_base] tight_build, shipped on). Grille's packing.c asks for a free spot touching the most of a
// player's own production buildings near its base, then leaves the game's own search alone when no spot qualifies.
// The game's CanBuildAt already checks the tiles round a footprint for one walkable arc and a lane at least a citizen
// wide, so this reduced port uses it for that rule and walks the same ring only to count touching kindred buildings.
// Upstream's clearance rules for drop sites, mines, trees, gates and raiders, its row-length cap and its continent check
// are left to CanBuildAt or omitted: the site is still a real free build tile that the game's builder can reach.
#include <windows.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
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
#include "platform/PlatSim.h"
#include "ee/EEMap.h"
#include "ee/EEUnit.h"
#include "ee/EEUnitClassIDs.h"
#include "ee/EECPBuildManager.h"
#include "ee/EECPUnitManager.h"
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGLoadGarrison.h"
#include "ee/EEResourceType.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEUnitBehavior.h"
#include "ee/EECPCombatManager.h"
#include "ee/EECPAttackCommander.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackSubGroup.h"
#include "AgeOfAI/AgeOfAIStats.h"
#include "AgeOfAI.h"
#include "AgeOfAIDefence.h"
#include "AgeOfAIHome.h"
#include "AgeOfAITowers.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"
#include "AgeOfAI/AgeOfAITightBuildRules.h"

#include "lle/LowLevelEngine.h"
#include "lle/GETerrainMesh.h"
#include "lle/U2DPoint.h"
#include "ee/EEMap.h"
#include "ee/EEUnit.h"
#include "ee/EEPlayer.h"
#include "ee/EEComputerPlayer.h"
#include "ee/EECPBuildManager.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEAIKindIDs.h"
#include "ee/EEPhysics.h"
#include "AgeOfAI/AgeOfAI.h"
#include "AgeOfAI/AgeOfAIRoles.h"
#include "AgeOfAI/AgeOfAITightBuildRules.h"

namespace AgeOfAI {
namespace {

struct TightBuildState {
    TightBuildCounts mCounts;
    unsigned long mLast = 0;
};

TightBuildState sTightBuild[16];

// The ring round a footprint: four sides of s+1 tiles, starting at its north-west corner, as in packing.c tb_ring_ok.
void FootprintRing(long inX, long inY, long inSize, std::vector<U2DPoint<long> >& outRing)
{
    static const long kDX[4] = { 0, 1, 0, -1 };
    static const long kDY[4] = { -1, 0, 1, 0 };
    outRing.clear();
    for (long theSide = 0; theSide < 4; theSide++) {
        long theRX = theSide == 0 || theSide == 3 ? inX - 1 : inX + inSize;
        long theRY = theSide < 2 ? inY - 1 : inY + inSize;
        long theSX = -kDY[theSide], theSY = kDX[theSide];
        for (long i = 0; i <= inSize; i++)
            outRing.push_back(U2DPoint<long>(theRX + theSX * i, theRY + theSY * i));
    }
}

// A tile is walkable when it is land and no building stands on it. Count one touching kindred building for the score.
void RingTile(EEPlayer* inPlayer, long inX, long inY, bool& outWalkable, bool& outTouch)
{
    EEMap* theMap = gWorld.GetMap();
    EETile* theTile = theMap != NULL ? theMap->GetTile(inX, inY) : NULL;
    outWalkable = false;
    outTouch = false;
    if (theTile == NULL)
        return;
    long theTerrain = theTile->GetTerrainType();
    outWalkable = theTerrain != kTerrainDeepWater && theTerrain != kTerrainShallowWater
        && theTerrain != kTerrainCliff && theTerrain != kTerrainSpace;
    for (RTSTileUnitNode* theNode = theTile->mUnits; theNode != NULL; theNode = theNode->mNext) {
        EEUnit* theUnit = theNode->mUnit;
        EEUnitClass* theClass = theUnit != NULL ? theUnit->mUnitClass : NULL;
        if (theClass == NULL || theClass->mUnitType <= 0)
            continue;
        if (Roles::IsBuildingCategory(theClass->mCategory))
            outWalkable = false;
        if (theUnit->mPlayer == inPlayer && TightBuildRules::IsKinType(theClass->mUnitType))
            outTouch = true;
    }
}

}  // namespace

void TightBuildTick(EEComputerPlayer* inPlayer, unsigned long inNow)
{
    if (inPlayer == NULL || EngineOf(inPlayer->mPlayerID) != kEngineAgeOfAI)
        return;
    TightBuildState& theState = sTightBuild[inPlayer->mPlayerID & 15];
    if (inNow < theState.mLast) {
        std::memset(&theState, 0, sizeof(theState));
    }
    theState.mLast = inNow;
}

bool TightBuildSite(EECPBuildManager* inManager, long inUnitClass, EETileCoordinate& outTile)
{
    if (inManager == NULL || inManager->GetPlayer() == NULL || gWorld.GetMap() == NULL
        || !TightBuildRules::IsPackedType(inUnitClass))
        return false;
    EEComputerPlayer* thePlayer = inManager->GetPlayer();
    unsigned long thePID = thePlayer->mPlayerID;
    if (EngineOf(thePID) != kEngineAgeOfAI || !SettingsAIOf(thePID, kDifficultyMedium).tight_build)
        return false;
    TightBuildState& theState = sTightBuild[thePID & 15];
    theState.mCounts.mCalls++;

    EEUnitClass* theClass = gUnitClassTable[inUnitClass];
    const EETileCoordinate& theBase = inManager->mTownCenterTile;
    long theSize = theClass != NULL ? theClass->GetTileSize() : 0;
    if (theSize < 1 || theSize > TightBuildRules::kMaxSize || theBase.GetX() < 0 || theBase.GetY() < 0)
        return false;
    unsigned short theContinent = gContinentManager.GetContinent(theBase, kAIPathLandMass);

    std::vector<U2DPoint<long> > theRing;
    bool theFound = false;
    long theBest = 0, theBestRing = 0;
    for (long theRingIndex = 2; theRingIndex <= TightBuildRules::kRadius
         && !(theFound && theRingIndex > theBestRing + TightBuildRules::kMoreRings); theRingIndex++) {
        for (long theDX = -theRingIndex; theDX <= theRingIndex; theDX++) {
            for (long theDY = -theRingIndex; theDY <= theRingIndex; theDY++) {
                if (theDX != theRingIndex && theDX != -theRingIndex && theDY != theRingIndex && theDY != -theRingIndex)
                    continue;
                EETileCoordinate theSite(theBase.GetX() + theDX - theSize / 2,
                                         theBase.GetY() + theDY - theSize / 2);
                if (!inManager->CanBuildAt(theContinent, inUnitClass, theSite, theSize, false, true,
                                           theBase.GetX(), theBase.GetY()))
                    continue;
                FootprintRing(theSite.GetX(), theSite.GetY(), theSize, theRing);
                bool theFlags[4 * (TightBuildRules::kMaxSize + 1)];
                long theFlagCount = 0, theTouch = 0, theWalkable = 0;
                for (std::vector<U2DPoint<long> >::const_iterator it = theRing.begin(); it != theRing.end(); ++it) {
                    bool theOK, theKind;
                    RingTile(thePlayer, it->GetX(), it->GetY(), theOK, theKind);
                    theFlags[theFlagCount++] = theOK;
                    if (theKind)
                        theTouch++;
                }
                if (!TightBuildRules::RingWalkable(theFlags, theFlagCount, theSize + 2, theWalkable))
                    continue;
                long theScore = TightBuildRules::SpotScore(theTouch, theRingIndex);
                if (theFound && theScore <= theBest)
                    continue;
                theFound = true;
                theBest = theScore;
                theBestRing = theRingIndex;
                outTile = theSite;
            }
        }
    }
    if (theFound)
        theState.mCounts.mSites++;
    return theFound;
}

TightBuildCounts TightBuildCountsOf(unsigned long inPlayerID)
{
    return sTightBuild[inPlayerID & 15].mCounts;
}

}  // namespace AgeOfAI
