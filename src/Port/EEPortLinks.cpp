// Port build only (tools/build_port.py adds this unit to the exe; the VC6 build never compiles it).
//
// The VC6 link of EE-AOC.exe (tools/exe_fulllink.py) resolves some names from the original image instead of from
// source: virtual slots whose body the original shares with another function (identical code folding), base-class
// slots that are one or two instructions long, and globals in the original's .bss that no unit defines. The port
// links no original image, so this unit gives those names a definition. Each comment names the original's code at
// that address (an RVA) and the slot that holds it; a body copied from another function is that function's source.
// A name whose original code is not identified stops the game with its name (EEPortUnwritten).
#include <windows.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/LowLevelEngineCore.h"
#include "lle/GE3DLine.h"
#include "lle/GESurface.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include <stdio.h>
#include "lle/GETransformation.h"
#include "lle/GEPhysicalModel.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSRestorable.h"
#include "lle/FSPersistent.h"
#include "lle/USingleton.h"
#include <typeinfo>
#include "lle/U2DPoint.h"
#include "lle/U2DSparseArray.h"
#include <mbstring.h>
#include "lle/U2DRectangle.h"
#include "lle/U2DPolygon.h"
#include "ee/EmpireEarth.h"
#include "ee/EEPhysics.h"
#include "ee/EEPlayer.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEHumanPlayer.h"
#include "ee/EEComputerPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEEffectManager.h"
#include "ee/EECalamity.h"
#include "ee/EECalamityClass.h"
#include "ee/EELocationCalamityClass.h"
#include "ee/EECCSpecialLocation.h"
#include "ee/EEAreaEffect.h"
#include "ee/EEUnitGoal.h"
#include "ee/EEMap.h"
#include <string.h>
#include "lle/GEModel.h"
#include "lle/GELOD.h"
#include "ee/EEWheel.h"
#include "ee/EEAreaUtilities.h"
#include "ee/EEAIContinentManager.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EEMapDimensions.h"
#include "ee/EECalamityManager.h"
#include "lle/UWideString.h"
#include "ee/EESMDialogMessage.h"
#include "lle/FSFile.h"
#include "ee/EECPILocation.h"
#include "ee/EEUGAttackUnit.h"
#include "ee/EECPASTargetUnit.h"
#include "ee/EECPAGTask.h"
#include "ee/EECPAttackSubGroup.h"
#include "ee/EECPAttackSubGroupUnit.h"
#include <math.h>
#include "ee/EEMedia.h"
#include "ee/EEGraphicEffect.h"
#include "ee/EEUnitSetManager.h"
#include "ee/EECPAttackGroup.h"
#include "ee/EECPAttackCommander.h"
#include "ee/EEPhysicsModel.h"
#include "ee/AStarGraphableObjectBase.h"
#include "ee/EEAIAStarStorage.h"
#include "lle/GEOrientation.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GEMaterial.h"
#include "ee/RTSRegionTable.h"
#include <utility>
#include "ee/EEInfluenceTable.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGHealUnit.h"
#include "ee/EEAIArea.h"
#include "ee/EEAIContinent.h"
#include "ee/EEUGExplore.h"
#include "ee/EETileLocation.h"
#include "ee/EEUnitUtilities.h"
#include "ee/EEAIPathArea.h"
#include "ee/EEAIPathSubSector.h"
#include "lle/GERectangle.h"
#include "ee/EESMArea.h"
#include "ee/EETown.h"
#include <deque>
#include "ee/EEUnitBehavior.h"
#include "ee/EECPUnitRequirement.h"
#include "ee/EECPCombatStrategy.h"
#include "ee/EECPUnitSelector.h"
#include "ee/EECPPersonnel.h"
#include "ee/EEDebugTrace.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EECPCombatManager.h"
#include "ee/EECPASGSea.h"
#include "ee/EECPASGLand.h"
#include "ee/EECPASGTransport.h"
#include "ee/EECPASGSeaTransport.h"
#include "ee/EECPASGSpaceship.h"
#include "ee/EESoundObject.h"
#include "ee/EEUCTransport.h"
#include "ee/EEUCAirplane.h"
#include "ee/EEUGUnloadTransport.h"
#include "ee/EECPAttackFormation.h"
#include "ee/EEHerd.h"
#include "GEWindow.h"
#include "lle/GERhombus.h"
#include "lle/GEBitmap.h"
#include "lle/FSFileSpec.h"
#include "lle/FSArchive.h"
#include "lle/FSPath.h"
#include "lle/TMPoll.h"
#include "lle/TSCriticalSection.h"
#include "lle/TSScheduler.h"
#include "lle/Input System.h"
#include "lle/UIControl.h"
#include "lle/UIEvent.h"
#include "lle/UIEntityGroup.h"
#include "lle/UIBackground.h"
#include "lle/UIFontTable.h"
#include "lle/UIHotKey.h"
#include "lle/UIIMECandidate.h"
#include "lle/UIForm.h"
#include "lle/UIManager.h"
#include "lle/UITextLabel.h"
#include "lle/UISlider.h"
#include "lle/UIScrollBar.h"
#include "lle/UMemoryPool.h"
#include "lle/UList.h"
#include "ee/EEUIManager.h"
#include "ee/EEAction.h"
#include "ee/EEAAddChatMessage.h"
#include "lle/NEMessage.h"
#include "lle/SESoundManager.h"
#include "lle/UResourceFile.h"
#include "lle/NESynchronousServer.h"
#include "lle/NESynchronousClient.h"
#include "ee/EEUserInterface.h"
#include "ee/EEMessage.h"
#include "ee/EEABootClientFromCanceledGame.h"
#include "lle/NESynchronousCommunications.h"
#include "ee/EECommunications.h"
#include "ee/EEUnitPicker.h"
#include "ee/EEACalamity.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"
#include "lle/ISKeyboard.h"
#include "lle/ISMouse.h"
#include "lle/UIButton.h"
#include "lle/GELine.h"
#include "ee/EECameraControl.h"
#include "ee/EEACenterCamera.h"
#include <string>
#include <list>
#include "lle/UICheckBox.h"
#include "lle/UIEditBox.h"
#include "lle/NEProtocol.h"
#include "lle/FSBasicFile.h"
#include "ee/EEAJoinOrHostAGame.h"
#include "ee/EEALasso.h"
#include "ee/EEMiniMap.h"
#include "ee/EEMouse.h"
#include "ee/EEUIFMainGame.h"
#include "ee/EEAMiniMap.h"
#include "ee/EECivilization.h"
#include "ee/EEUIFScenarioEditor.h"
#include "ee/EEUIFSEMainGroup.h"
#include "ee/EESEUnit.h"
#include "ee/EEAMouseClick.h"
#include "ee/EEPPlayerEvent.h"
#include "ee/EEAPlayerEvent.h"
#include "lle/FSMemoryFile.h"
#include "ee/EEAQuickSaveLoad.h"
#include "lle/TSThread.h"
#include "lle/TSEvent.h"
#include "ee/EESEAChangeLoadText.h"
#include "ee/EESELoadThread.h"
#include "ee/EEAResetAllPlayerInfo.h"
#include "lle/UIBlinkingButton.h"
#include "lle/GEFont.h"
#include "lle/GEText.h"
#include "lle/UIFormManager.h"
#include "ee/EESaveLoad.h"
#include "ee/EEASaveGame.h"
#include "ee/EEGameOptions.h"
#include "ee/EEGamePreferences.h"
#include "ee/EEAScroll.h"
#include "ee/EESMMediaStrings.h"
#include "ee/EEPSaveGameHeader.h"
#include "ee/EEClient.h"
#include "ee/EEWonNet.h"
#include "ee/EEAServerMigration.h"
#include "ee/EEGameSound.h"
#include "ee/EEAUpgradeImprovement.h"
#include "ee/EEServer.h"
#include "ee/EEAUserPauseGame.h"
#include "lle/UIListBox.h"
#include "ee/EEAWonNet.h"
#include "ee/EEArtificialIntelligence.h"
#include "ee/EEGlobalCalamityClass.h"
#include "ee/EECCLineOfSight.h"
#include "ee/EECPASGAir.h"
#include "ee/EEUUUnitFunctor.h"
#include "ee/EECPASGHelicopter.h"
#include "ee/EECPASGAirTransport.h"
#include "ee/EECPASGSpaceFighter.h"
#include "ee/EECPASGSpaceTransport.h"
#include "ee/EECPIntelligence.h"
#include "ee/EECPDefenseCommander.h"
#include "ee/EECivPowersManager.h"
#include "ee/EEDoppelganger.h"
#include "ee/EEComplexDoppelganger.h"
#include "ee/EEAEManager.h"
#include "ee/EEContagionHelper.h"
#include "ee/EEGraphicObject.h"
#include "ee/EEDSUIEvents.h"
#include "ee/RTSGraphics.h"
#include "ee/EEGraphics.h"
#include "ee/EERenderObject.h"
#include "ee/EEEffect.h"
#include "ee/EEEffect2D.h"
#include "ee/EEE2DExplosion.h"
#include "ee/EEEffect3D.h"
#include "ee/EEE3DTracerBeam.h"
#include "ee/EEEMStorm.h"
#include "ee/EEEnvironmentManager.h"
#include "ee/EEStartingResources.h"
#include "ee/EEContagionManager.h"
#include "ee/EEPCalamityHelper.h"
#include "LostDebug.h"
#include "ee/EEEvent.h"
#include "ee/EEDatabase.h"
#include "ee/EEAIUnitTargetingTable.h"
#include "ee/EETerrainColorTable.h"
#include "ee/EEPathManager.h"
#include "lle/GEPolygonArray.h"
#include "lle/GEModelManager.h"
#include "ee/EERenderLists.h"
#include "ee/EEROGroundShadows.h"
#include "ee/EEUIFSEBrushGroup.h"
#include "ee/EEROTerrain.h"
#include "ee/EEROUnits.h"
#include "ee/EEUIMiniMap.h"
#include "ee/EEScenarioManager.h"
#include "ee/EERandomMapClass.h"
#include "ee/EERandomMapGenerators.h"
#include "ee/EESEAPaint.h"
#include "ee/EESEAPaintMoutains.h"
#include "lle/ESChecksum.h"
#include "lle/PIMModule.h"
#include "lle/NEProtocolConfiguration.h"
#include "lle/SEConductor.h"
#include "lle/SEMixer.h"
#include "lle/TSReadWriteLock.h"
#include "lle/UMFastMath.h"
#include "lle/URandom.h"
#include "lle/UString.h"
#include "ee/EEABuild.h"
#include "lle/UIEntity.h"
#include "lle/UILBString.h"
#include "lle/UIComboBox.h"
#include "ee/EEUIFileComboBox.h"
#include "ee/EESEEFindText.h"
#include "ee/EESEIAExport.h"
#include "ee/EESEInstructions.h"
#include "lle/UIFileComboBox.h"
#include "ee/EESETSBaseObject.h"
#include "ee/EESETSTrigger.h"
#include "ee/EESETSCondition.h"
#include "lle/UIRadioButton.h"
#include "ee/EESMEffect.h"
#include "ee/EESETriggerScreen.h"
#include "ee/EESEEChangeEffectAction.h"
#include "ee/EESEEGCPlace.h"
#include "ee/EESEEPlace.h"
#include "ee/EESEEvents.h"
#include "ee/EESETSEScenarioComboBox.h"
#include "ee/EESETSUICBPlayer.h"
#include "ee/EESETSEffectControls.h"
#include "ee/EESETSEffectType.h"
#include "ee/EESETSETAI.h"
#include "ee/EESETSETGame.h"
#include "ee/EESETSETObject.h"
#include "ee/EESETSEComboBox.h"
#include "ee/EESEPlayerScreen.h"
#include "ee/EESETSETNoEffect.h"
#include "ee/EESETSETPlayer.h"
#include "ee/EESETSEffect.h"
#include "ee/EESavedGroup.h"
#include "EEDatabase.h"
#include "ee/EETeam.h"
#include "ee/EETileCoordinate.h"
#include "ee/EEUCAirAnimal.h"
#include <stdlib.h>
#include "ee/EEUCAmbient.h"
#include "ee/EEUCBasicCitizen.h"
#include "ee/EEUCBasicPriest.h"
#include "ee/EEUCBuilding.h"
#include "ee/EEUCLandTransport.h"
#include "ee/EEUCFortress.h"
#include "ee/EEUCLandable.h"
#include "ee/EEUCHelicopter.h"
#include "ee/EEUCSpaceship.h"
#include "ee/EEUCHelicopterPriest.h"
#include "ee/EEUCHelicopterTransport.h"
#include "ee/EEUCLand.h"
#include "ee/EEUCLandPriest.h"
#include "ee/EEUCProjectile.h"
#include "ee/EEUGAttackGround.h"
#include "ee/EEUCMissile.h"
#include "lle/GEBoundingBox.h"
#include "ee/EEUnitAction.h"
#include "ee/EEUCNaval.h"
#include "ee/EEUnitVectorItem.h"
#include "ee/EEUCNavalBuilding.h"
#include "ee/EEUCNavalCitizen.h"
#include "ee/EEUCNavalPriest.h"
#include "ee/EEUCNavalTransport.h"
#include "ee/EEAirTrafficControl.h"
#include "ee/EEUCPlanetSpaceship.h"
#include "ee/EEUCResource.h"
#include "ee/EEUCSubmergedNaval.h"
#include "ee/EEUCSeaAnimal.h"
#include "ee/EEUCSpaceBuilding.h"
#include "ee/EEUCSpaceTransport.h"
#include "ee/EEUCTorpedo.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGPatrol.h"
#include "ee/EEUGLoadContainer.h"
#include "ee/EEUGLoadGarrison.h"
#include "ee/EEUIFMGMainGroup.h"
#include "ee/EEButtonObject.h"
#include "ee/EEFormation.h"
#include "ee/EEUICUnitIconBar.h"
#include "ee/EECampaignFormManager.h"
#include "ee/EEUIECampaignSelectScreenEvents.h"
#include "ee/EEUIEEmpireEarthEditorsEvents.h"
#include "ee/EEUISCIFormManager.h"
#include "ee/EEUIESLLoadScreen.h"
#include "ee/EEUIESLSaveScreen.h"
#include "ee/EEUIEGameSettingsEvents.h"
#include "ee/EEUIEMainGameOptionsEvents.h"
#include "ee/EECBUIListBox.h"
#include "ee/EECivilizationManager.h"
#include "ee/EEUIEPDCSEvents.h"
#include "lle/UIFileListBox.h"
#include "ee/EESaveGameFiles.h"
#include "ee/EEUIESaveLoadEvents.h"
#include "ee/EEUICWonNetStatBox.h"
#include "ee/EEUIEWonNetStatsEvents.h"
#include "ee/EEUIFMGStatusBox.h"
#include "lle/ISEntity.h"
#include "ee/EEUIFSEPaint.h"
#include "ee/EEUIFSEPaintElevation.h"
#include "ee/EEUIFMGStatusButton.h"
#include "ee/EEUIFMGUnitHack.h"
#include "ee/EEUIManagerEvents.h"
#include "lle/Graphics Engine.h"
#include "ee/EEColorTable.h"
#include "ee/EEUIEMainGameEvents.h"
#include "ee/EEWUnitsOffMapInfo.h"
#include "ee/EEDoppelgangerManager.h"
#include "ee/EEPPatrolGroup.h"
#include "ee/UReferenceContainer.h"

void EEAdjustNavalPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inSmooth);  // EEFOFindNewTargetNearOldTarget.cpp
static const float kUnitAINoHeading = -10000.0f;  // EEUnitAI.cpp's value

// Stops the game with the name of a function whose original code the reconstruction has not identified
[[noreturn]] static void EEPortUnwritten(const char* inName)
{
    char theText[256];
    wsprintfA(theText, "%s has no source in this build", inName);
    OutputDebugStringA(theText);
    MessageBoxA(NULL, theText, "Empire Earth", MB_OK | MB_ICONSTOP);
    ExitProcess(3);
}

// .bss globals of the original that no unit defines and no object holds; both start at zero there
bool gShowParseTime;                    // 0x566b30
int gLinesParsed;                       // 0x566b34

// 0x14804f: RTSWorld::GetUnit's body; the original's EEWorld slot is the same code
EEUnit* EEWorld::GetUnit(unsigned long inUnitID)
{
    return static_cast<EEUnit*>(this->RTSWorld::GetUnit(inUnitID));
}

// 0x41976: EECPASGAir::RequestUnits's body (EECPASGAir.cpp)
void EECPASGSea::RequestUnits(std::vector<long>& inClasses, std::vector<long>& inUnitIDs, long inContinent,
                              long inTownID)
{
    if (this->mRequest != NULL) {
        this->mGroup->mCommander->mPersonnel->ReleaseRequest(this->mRequest->mID);
        this->mRequest->Dereference();
    }
    EETown* theTown = NULL;
    if (inTownID != 0)
        theTown = this->mPlayer->GetTownByID(inTownID);
    if (theTown == NULL)
        this->mRequest = this->mGroup->mCommander->mPersonnel->NewContinentRequest(inClasses, inUnitIDs, inContinent);
    else {
        EECPPersonnel* thePersonnel = this->mGroup->mCommander->mPersonnel;
        this->mRequest = thePersonnel->NewTownRequest(inClasses, inUnitIDs, inTownID, theTown->mHull.GetCenter());
    }
    if (this->mRequest != NULL && this->mPlayer2->mCombatManager->mIsWonderRace)
        this->mRequest->mDeadline = gPhysics->mGameTime + 300000;
}

// 0x41976, as above
void EECPASGSpaceship::RequestUnits(std::vector<long>& inClasses, std::vector<long>& inUnitIDs, long inContinent,
                                    long inTownID)
{
    if (this->mRequest != NULL) {
        this->mGroup->mCommander->mPersonnel->ReleaseRequest(this->mRequest->mID);
        this->mRequest->Dereference();
    }
    EETown* theTown = NULL;
    if (inTownID != 0)
        theTown = this->mPlayer->GetTownByID(inTownID);
    if (theTown == NULL)
        this->mRequest = this->mGroup->mCommander->mPersonnel->NewContinentRequest(inClasses, inUnitIDs, inContinent);
    else {
        EECPPersonnel* thePersonnel = this->mGroup->mCommander->mPersonnel;
        this->mRequest = thePersonnel->NewTownRequest(inClasses, inUnitIDs, inTownID, theTown->mHull.GetCenter());
    }
    if (this->mRequest != NULL && this->mPlayer2->mCombatManager->mIsWonderRace)
        this->mRequest->mDeadline = gPhysics->mGameTime + 300000;
}

// 0x4dff2: EECPASGSea::IsAt's body (EECPASGSea.cpp)
bool EECPASGSpaceship::IsAt(const EECPILocation& inLocation)
{
    EEAIPathSubSector* theTarget = inLocation.GetPathSubSector();
    EETileLocation theTile;
    bool theResult = this->FindPosition(theTile, true, false);
    if (!theResult || theTile.mX == -10000)
        return theResult;
    EEAIPathSubSector* theCurrent = gContinentManager.GetPathSubSectorAt(theTile);
    if (theCurrent == NULL)
        return theResult;
    if (theCurrent != theTarget && (unsigned long)theTile.DistanceToSquared(inLocation.mTile) > 64) {
        theResult = false;
        if (theCurrent->IsNeighbor(theTarget))
            theResult = this->AreAllUnitsSettled();
        else if (theTarget == NULL)
            theResult = true;
        else if (!this->mFlags.mFlags[theTarget->mKind])
            theResult = (unsigned long)theTile.DistanceToSquared(inLocation.mTile) <= 900;
    } else
        theResult = true;
    return theResult;
}

// 0x4dfe6: EECPASGSea::IsAtLocation's body, which passes its argument on to EECPAttackSubGroup::IsAt (0x5e430)
// as the location's address
void EECPASGSpaceship::IsAtLocation(long inArg1)
{
    EECPAttackSubGroup::IsAt(*reinterpret_cast<const EECPILocation*>(inArg1));
}

// 0x50ddc: EECPASGSpaceTransport::Load's body; the sea base's Load is the same function as the spaceship base's
long EECPASGSeaTransport::Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
{
    long theResult = EECPASGSea::Load(inMajorVersion, inMinorVersion, inFile);
    if (theResult == 0)
        theResult = EECPASGTransport::Load(inMajorVersion, inMinorVersion, inFile);
    return theResult;
}

// 0x44ada: EECPASGAirTransport::Restore's body; the sea base's Restore is the same function as the air base's
long EECPASGSeaTransport::Restore(unsigned long inMajorVersion, unsigned long inMinorVersion)
{
    long theResult = EECPASGSea::Restore(inMajorVersion, inMinorVersion);
    if (theResult == 0) {
        theResult = EECPASGTransport::Restore(inMajorVersion, inMinorVersion);
    }
    return theResult;
}

// 0x44740: EECPASGAirTransport::LoadCargo's body
void EECPASGSeaTransport::LoadCargo()
{
    std::vector<EEComplexUnit*> theUnits;
    for (long i = this->mUnits.size() - 1; i >= 0; i--) {
        if (!this->mUnits[i]->IsDone()) {
            EEComplexUnit* theUnit = this->mUnits[i]->mUnit;
            theUnits.push_back(theUnit);
        }
    }
    if (theUnits.size() > 0 && this->mLand != NULL)
        this->mLand->BoardTransports(&theUnits);
    this->mCargoStage = 1;
}

// 0x44c2f: EECPASGAirTransport::UnloadAt's body
void EECPASGSeaTransport::UnloadAt(const EECPILocation& inLocation)
{
    EEAIAStarBounds theBounds;
    theBounds.Set(inLocation.mTile);
    std::vector<EEComplexUnit*> theFailed;
    long i;
    for (i = this->mUnits.size() - 1; i >= 0; i--) {
        EEComplexUnit* theUnit = this->mUnits[i]->mUnit;
        EEUGUnloadTransport* theGoal = new EEUGUnloadTransport(theUnit, inLocation.mTile.mX,
                                                              inLocation.mTile.GetY(), true);
        bool theSkip = theUnit->GetNumCargoUnits() == 0;
        if (!theSkip) {
            theSkip = !theGoal->CanUnloadAtDestination();
            if (theSkip && this->mLand != NULL)
                this->mLand->AvoidTile(inLocation.mTile);
        }
        if (theSkip) {
            if (theGoal != NULL)
                delete theGoal;
            theFailed.push_back(theUnit);
        } else if (!this->GiveGoal(theUnit, theGoal, 0, kActionPrepareToMove) && theGoal != NULL) {
            delete theGoal;
        }
    }
    if (theFailed.size() != 0) {
        theBounds.Grow(7, true);
        for (i = theFailed.size() - 1; i >= 0; i--) {
            EEComplexUnit* theUnit = theFailed[i];
            EEUnitGoal* theGoal = new EEUGLocation((float)gPhysics->GetRandom(theBounds.mLeft, theBounds.mRight),
                (float)gPhysics->GetRandom(theBounds.mTop, theBounds.mBottom), 0.0f, true, true, false, 0.0f,
                false, false);
            if (!this->GiveGoal(theUnit, theGoal, 0, kActionPrepareToMove) && theGoal != NULL)
                delete theGoal;
        }
    }
    this->mCargoStage = 2;
}

// 0x2907a3: empty
void EECalamityClass::Place(EEUnit* inUnit, unsigned long inX, unsigned long inY)
{}

// 0xe123d: empty
void EEClient::HandleStopGame()
{}

// 0x18aa9: returns true
bool EEClient::IsClient()
{ return true; }

// 0xac9f3: returns false
bool EEClient::IsServer()
{ return false; }

// 0x18aa9: returns true
bool EEComplexDoppelganger::IsComplex()
{ return true; }

// 0xe123d: empty
void EEComplexUnit::FnE123D()
{}

// 0xe123d: empty
void EECPDefenseCommander::Refresh()
{}

// 0xe123d: empty
void EEEnvironmentManager::FnE123D()
{}

// 0xe123d: empty
void EEPlayer::Virtual60()
{}

// 0xe123d: empty
void EEROGroundShadows::FrameDone()
{}

// 0xe123d: empty
void EEROGroundShadows::SceneStarting()
{}

// 0xe123d: empty
void EEROTerrain::FrameDone()
{}

// 0xe123d: empty
void EEROTerrain::SceneStarting()
{}

// 0xe123d: empty
void EEROUnits::SceneStarting()
{}

// 0xe123d: empty
void EESEAPaintMoutains::Virtual8()
{}

// 0xe123d: empty
void EEServer::BeforeRemovingPlayers()
{}

// 0xac9f3: returns false
bool EEServer::IsClient()
{ return false; }

// 0x18aa9: returns true
bool EEServer::IsServer()
{ return true; }

// 0xe123d: empty
void EESETSCondition::Deactivate()
{}

// 0xe123d: empty
void EESETSEffect::Deactivate()
{}

// 0xe123d: empty
void EESETSTrigger::Deactivate()
{}

// 0x27c0b4: returns false
bool EEUCAmbient::IsPickable()
{ return false; }

// 0x18aa9: returns true
bool EEUCBasicPriest::CanAttackMove()
{ return true; }

// 0x18aa9: returns true
bool EEUCBuilding::IsBuilding()
{ return true; }

// 0x18aa9: returns true
bool EEUCBuilding::IsLandBuilding()
{ return true; }

// 0x18aa9: returns true
bool EEUCFortress::IsLandBuilding()
{ return true; }

// 0x18aa9: returns true
bool EEUCHelicopterPriest::FliesOver()
{ return true; }

// 0x18aa9: returns true
bool EEUCLandable::CanAttackMove()
{ return true; }

// 0x18aa9: returns true
bool EEUCLand::CanAttackMove()
{ return true; }

// 0x156dd8: empty
void EEUCMissile::AddToRegion(EEUnit*)
{}

// 0x156dd8: empty
void EEUCMissile::RemoveFromRegion(EEUnit*)
{}

// 0xac9f3: returns false
bool EEUCNavalBuilding::IsLandBuilding()
{ return false; }

// 0x156dd8: empty
void EEUCProjectile::AddToRegion(EEUnit*)
{}

// 0x156dd8: empty
void EEUCProjectile::RemoveFromRegion(EEUnit*)
{}

// 0xac9f3: returns false
bool EEUCSpaceBuilding::IsLandBuilding()
{ return false; }

// 0x18aa9: returns true
bool EEUCSubmergedNaval::CanAttackMove()
{ return true; }

// 0x29059d: returns false
bool EEUCTorpedo::CheckWaterImpact(EEComplexUnit* inUnit)
{ return false; }

// 0x18aa9: returns true
bool EEUGLoadContainer::IsLoadGoal()
{ return true; }

// 0xe123d: empty
void EEUIFScenarioEditor::FnE123D()
{}

// 0xe123d: empty
void EEUIManager::FnE123D()
{}

// 0x27c0b4: returns false
bool EEUnit::Fn27C0B4()
{ return false; }

// 0xe123d: empty
void EEWonNet::PlayerJoined()
{}

// 0xe123d: empty
void EEWorld::TeamsChanged()
{}

// 0xe123d: empty
void RTSPlayer::Clear()
{}

// 0x27cf38: returns 0
long RTSUnit::RepairPointers(unsigned long, unsigned long)
{ return 0; }

// 0x157269: empty
void EEComplexUnit::DescribeLong(UWideString& ioString)
{}

// 0x157269: empty
void EEComplexUnit::ResearchCompleted(unsigned long)
{}

// 0xe123d: empty
void EESEAPaintElevation::Virtual8()
{}

// 0x2101da: empty
void EEUnitClass::CargoRemoved(EEComplexUnit* inUnit, EEComplexUnit* inCargo)
{}

// 0x2101da: empty
void EEUnitClass::CargoUnloaded(EEComplexUnit* inUnit, EEComplexUnit* inCargo)
{}

// 0xac9c8: empty
void EEUnitClass::GateReplaced(EEUnit* inGate, EEUnit* inUnit)
{}

// 0x1720c6: empty
void EEAAddChatMessage::Virtual8()
{}

// 0x1720c6: empty
void EEABootClientFromCanceledGame::Virtual8()
{}

// 0x1720c6: empty
void EEACalamity::Virtual8()
{}

// 0x1720c6: empty
void EEACenterCamera::Virtual8()
{}

// 0x1720c6: empty
void EEAJoinOrHostAGame::Virtual8()
{}

// 0x1720c6: empty
void EEALasso::Virtual8()
{}

// 0x1720c6: empty
void EEAMiniMap::Virtual8()
{}

// 0x1720c6: empty
void EEAMouseClick::Virtual8()
{}

// 0x1720c6: empty
void EEAPlayerEvent::Virtual8()
{}

// 0x1720c6: empty
void EEAQuickSaveLoad::Virtual8()
{}

// 0x1720c6: empty
void EEAResetAllPlayerInfo::Virtual8()
{}

// 0x1720c6: empty
void EEASaveGame::Virtual8()
{}

// 0x1720c6: empty
void EEAScroll::Virtual8()
{}

// 0x1720c6: empty
void EEAServerMigration::Virtual8()
{}

// 0x1720c6: empty
void EEAUserPauseGame::Virtual8()
{}

// 0x1720c6: empty
void EEAWonNet::Virtual8()
{}

// 0x1ad67e: returns 0
EEContagionFunctor* EECalamityClass::GetContagionFunctor()
{ return 0; }

// 0x157269: empty
void EECalamityClass::UnloadMedia(EEPlayer* inPlayer)
{}

// EECPASGAir::Launch's body (twin)
void EECPASGSpaceFighter::Launch(long inTime)
{
    for (long i = this->mUnits.size() - 1; i >= 0; i--) {
        this->mUnits[i]->mNextRetargetTime = inTime;
        this->mUnits[i]->mNeedsOrders = true;
    }
    if (this->mTask.mIsActive)
        this->SetDestination(this->mTask.mLocation);
}

// EECPASGAir::RequestRemoval's body (twin)
void EECPASGSpaceFighter::RequestRemoval()
{
    for (long i = this->mUnits.size() - 1; i >= 0; i--) {
        if (this->mUnits[i]->IsDone())
            continue;
        if (this->mUnits[i]->GetUnit()->mContainer == NULL)
            this->mUnits[i]->GetUnit()->SetFlightTime(0);
        else
            this->mUnits[i]->GetUnit()->mContainer->ClearGoalQueue(0);
        this->mUnits[i]->GetUnit()->ClearGoalQueue(0);
    }
    EECPAttackSubGroup::RequestRemoval();
 EE_LOST_DEBUG(1); /* LOST-DEBUG: none: exact only (lostdebug_search, smallest n) */
}

// EECPASGAirTransport::LoadCargo's body (twin)
void EECPASGSpaceTransport::LoadCargo()
{
    std::vector<EEComplexUnit*> theUnits;
    for (long i = this->mUnits.size() - 1; i >= 0; i--) {
        if (!this->mUnits[i]->IsDone()) {
            EEComplexUnit* theUnit = this->mUnits[i]->mUnit;
            theUnits.push_back(theUnit);
        }
    }
    if (theUnits.size() > 0 && this->mLand != NULL)
        this->mLand->BoardTransports(&theUnits);
    this->mCargoStage = 1;
}

// EECPASGSeaTransport::PlaceFormation's body (twin)
void EECPASGSpaceTransport::PlaceFormation(const EECPILocation& inLocation)
{
    EETileLocation theTile;
    this->FindPosition(theTile, true, true);
    float theDistance = EEGetSeaHeading(theTile, inLocation.mTile);
    this->mFormation->Build();
    this->mFormation->mNoForcedMove = false;
    for (long i = this->mFormation->GetNumNodes() - 1; i >= 0; i--)
        this->mFormation->mNodes[i]->mSpacing = 2.0f;
    this->mFormation->Place(inLocation, theDistance);
}

// 0x44ada: EECPASGAirTransport::Restore's body; the air base's Restore is the same function as the spaceship base's
long EECPASGSpaceTransport::Restore(unsigned long inMajorVersion, unsigned long inMinorVersion)
{
    long theResult = EECPASGSpaceship::Restore(inMajorVersion, inMinorVersion);
    if (theResult == 0) {
        theResult = EECPASGTransport::Restore(inMajorVersion, inMinorVersion);
    }
    return theResult;
}

// EECPASGAirTransport::UnloadAt's body (twin)
void EECPASGSpaceTransport::UnloadAt(const EECPILocation& inLocation)
{
    EEAIAStarBounds theBounds;
    theBounds.Set(inLocation.mTile);
    std::vector<EEComplexUnit*> theFailed;
    long i;
    for (i = this->mUnits.size() - 1; i >= 0; i--) {
        EEComplexUnit* theUnit = this->mUnits[i]->mUnit;
        EEUGUnloadTransport* theGoal = new EEUGUnloadTransport(theUnit, inLocation.mTile.mX,
                                                              inLocation.mTile.GetY(), true);
        bool theSkip = theUnit->GetNumCargoUnits() == 0;
        if (!theSkip) {
            theSkip = !theGoal->CanUnloadAtDestination();
            if (theSkip && this->mLand != NULL)
                this->mLand->AvoidTile(inLocation.mTile);
        }
        if (theSkip) {
            if (theGoal != NULL)
                delete theGoal;
            theFailed.push_back(theUnit);
        } else if (!this->GiveGoal(theUnit, theGoal, 0, kActionPrepareToMove) && theGoal != NULL) {
            delete theGoal;
        }
    }
    if (theFailed.size() != 0) {
        theBounds.Grow(7, true);
        for (i = theFailed.size() - 1; i >= 0; i--) {
            EEComplexUnit* theUnit = theFailed[i];
            EEUnitGoal* theGoal = new EEUGLocation((float)gPhysics->GetRandom(theBounds.mLeft, theBounds.mRight),
                (float)gPhysics->GetRandom(theBounds.mTop, theBounds.mBottom), 0.0f, true, true, false, 0.0f,
                false, false);
            if (!this->GiveGoal(theUnit, theGoal, 0, kActionPrepareToMove) && theGoal != NULL)
                delete theGoal;
        }
    }
    this->mCargoStage = 2;
}

// 0x157269: empty
void EECPDefenseCommander::RespondToUnit(EEComplexUnit* inUnit)
{}

// 0x163d6f: empty
void EECPDefenseCommander::TargetPlayerNotice(EEPlayer* inPlayer, bool inFlag)
{}

// 0x1ad67e: returns 0
long EEEffect::DrawPass2()
{ return 0; }

// 0x1ad67e: returns 0
long EEEffect::DrawPass3()
{ return 0; }

// 0x1ad67e: returns 0
long EEEffect::DrawPass4()
{ return 0; }

// 0x1ad67e: returns 0
long EEEffect::DrawPass5()
{ return 0; }

// 0x1ad67e: returns 0
long EEEffect::DrawPass6()
{ return 0; }

// EECCLineOfSight::CanCastAt's body: returns 0
long EEGlobalCalamityClass::CanCastAt(EEComplexUnit* inUnit, GE3DPoint& inLocation, unsigned long inX, unsigned long inY,
                                      bool inCheck, EEUnit* inTargetUnit)
{ return 0; }

// 0x236201: returns 1
long EEGlobalCalamityClass::GetTargetKind()
{ return 1; }

// 0x208ef3: EESavedGroup::Assign's code. The caller (EEMSavedGroupCreate) passes a saved group and the selection.
void EEGroup::Assign(EEGroup* inGroup)
{
    static_cast<EESavedGroup*>(this)->Assign(static_cast<EESelectedGroup*>(inGroup));
}

// 0x29059d (slot +0xd0): returns false
bool EEPlayer::AvoidsEnemies(EEComplexUnit* inUnit)
{ return false; }

// EEPlayer::GetNumHeroes's body (twin)
long EEPlayer::GetHeroCount(bool inFirst, bool inSecond)
{
    long theResult = 0;
    if (inFirst)
        theResult = this->mNumHeroes;
    if (inSecond)
        theResult += this->mNumHeroesInProduction;
    return theResult;
}

// 0x156dd8 (slot +0xac): an empty body
void EEPlayer::UnitStranded(EEComplexUnit* inUnit)
{}

// 0x157269 (slot +0xd4): an empty body
void EEPlayer::WonderStarted(EEComplexUnit* inUnit)
{}

// 0x157269: empty
void EEROTerrain::Virtual14(long inArg1)
{}

// 0x11aa6c: RTSGraphics::PostSetRasterizer's code
void EESELoadThread::Post407(void* inParameter1, void* inParameter2)
{
    this->mThread->Post(0x407, inParameter1, inParameter2, true);
}

// 0x182d69: EESETSEffect::Show's code, which shows the controls at +0x34 and +0x30
void EESETriggerScreen::ShowText()
{
    this->mTextEditBox->SetDisplayMode(kDisplayModeShown);
    this->mTextLabel->SetDisplayMode(kDisplayModeShown);
}

// 0x17c5d9: EESETriggerScreen::HideText's code, which hides the controls at +0x34 and +0x30
void EESETSEffect::Hide()
{
    this->mTypeComboBox->SetDisplayMode(kDisplayModeHidden);
    this->mTitleLabel->SetDisplayMode(kDisplayModeHidden);
}

// EESETSETAI::ChangeAction's body (twin)
void EESETSETGame::ChangeAction(EESMEffect* inEffect, EESETSEffectControls* inControls)
{
    unsigned long theSelection;
    this->mTypes[this->mCurrentType]->VirtualC(inControls);
    inControls->mComboBoxes[0]->GetCurrentSelection(theSelection);
    this->mCurrentType = theSelection;
    this->mTypes[this->mCurrentType]->Show(inControls);
    this->mTypes[this->mCurrentType]->Load(inEffect, inControls);
    this->UpdateName(inEffect, inControls);
}

// 0xb62d3: the store to +0x14 (EEContagionHelper::SetFunctor's code)
void EESoundObject::SetVolume(float inVolume)
{
    this->mVolume = inVolume;
}

// EEUCBasicPriest::UnitCreated's body (twin)
void EEUCBasicCitizen::UnitCreated(EEComplexUnit* inUnit)
{     EEUnitClass::UnitCreated(inUnit); }

// EEUCBasicCitizen::UpdateCitizen's body (twin)
void EEUCBasicCitizen::Update(EEComplexUnit* inUnit)
{
    long theTimer = inUnit->mWorkTimer;
    long theStep = 0;
    if (theTimer > 0) {
        theStep = gPhysics->mTickTimeMs;
        if (theStep == 0)
            theStep = 1;
        theStep = theTimer - theStep;
        if (theStep > 0)
            inUnit->SetWorkTimer(theStep);
        else
            inUnit->SetWorkTimer(0);
    }
    if (inUnit->mWorkTimer == 0) {
        long theAction = inUnit->mCommand ? inUnit->mCommand->mType : 0;
        switch (theAction) {
        case kActionRepairBuilding:
        case kActionGatherResource:
            if (inUnit->mGoalState != NULL)
                inUnit->mGoalState->Work(inUnit);
            theTimer = inUnit->mWorkTimer;
            if (theTimer > 0) {
                inUnit->SetWorkTimer(inUnit->mWorkTimer > -theStep ? inUnit->mWorkTimer + theStep : 1);
            }
            break;
        }
    }
    inUnit->Fn1FA98C();
    if (inUnit->mCurrentResearch == kTechNone)
        return;
    long theFrame = gPhysics->mTickTimeMs;
    if (theFrame == 0)
        theFrame = 1;
    long theLeft = inUnit->mProductionTime - theFrame;
    if (inUnit->mPlayer->mInstantBuild) {
        if (theLeft < 0)
            goto done;
        theLeft = -1;
    }
    if (theLeft > 0)
        return;
done:
    inUnit->SetProductionTime(0);
    EETechTreeNode* theNode = inUnit->mPlayer->mTechnologies->Get(inUnit->mCurrentResearch);
    if (theNode != NULL && !theNode->mData->mMakesUnit) {
        long theTechnology = inUnit->mCurrentResearch;
        bool theResearch = true;
        EECivPower thePower;
        if (theNode->mData->mKind == 3 && gCivPowersManager.GetPower(kCivPowerBundeswehr, thePower)
            && thePower.mTechnology == theNode->mData->mPrerequisite1) {
            EEPlayUnitEffect(inUnit, theTechnology);
            theResearch = false;
        }
        inUnit->PlayEffect(100000);
        inUnit->SetProductionTime(0);
        if (theResearch)
            inUnit->mPlayer->mTechnologies->Research(theTechnology, inUnit);
    }
}

// EEUCBasicCitizen::UpdateEffects's body (twin)
void EEUCBasicPriest::UpdateEffects(EEComplexUnit* inUnit)
{
    if (this->mRippleEffect != NULL && inUnit->mHitPoints != 0 && (unsigned long)this->mRippleEffect->mDetailLevel <= gEffectDetail)
        this->UpdateRipples(inUnit);
}

// EEUCFortress::AddToRegion's body (twin)
void EEUCBuilding::AddToRegion(EEUnit* inUnit)
{     EEAddBuildingRegion(inUnit); }

// EEUCFortress::RemoveFromRegion's body (twin)
void EEUCBuilding::RemoveFromRegion(EEUnit* inUnit)
{     EERemoveBuildingRegion(inUnit); }

// EEUCBuilding::AddToTile's body (twin)
void EEUCFortress::AddToTile(EEUnit* inUnit)
{     EEAddToFootprintTiles(inUnit); }

// EEUCBuilding::RemoveFromTile's body (twin)
void EEUCFortress::RemoveFromTile(EEUnit* inUnit)
{     EERemoveFromFootprintTiles(inUnit); }

// 0x157269: empty
void EEUCFortress::UpdateEffects(EEComplexUnit*)
{}

// EEUCSpaceTransport::AddToTile's body (twin)
void EEUCHelicopterPriest::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// EEUCSpaceTransport::RemoveFromTile's body (twin)
void EEUCHelicopterPriest::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// EEUCLandPriest::Update's body (twin)
void EEUCHelicopterPriest::Update(EEComplexUnit* inUnit)
{     EEUCBasicPriest::Update(inUnit); }

// 0x157269: empty
void EEUCHelicopterPriest::UpdateEffects(EEComplexUnit*)
{}

// EEUCSpaceTransport::AddToTile's body (twin)
void EEUCLandable::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// EEUCAirAnimal::Animate's body (twin)
long EEUCLandable::Animate(EEComplexUnit* inUnit)
{     return EEUnitClass::Animate(inUnit); }

// EEUCSpaceTransport::RemoveFromTile's body (twin)
void EEUCLandable::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// EEUCNavalTransport::FindBoardingLocation's body (twin)
bool EEUCLandTransport::FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation)
{     return EEUCTransport::FindBoardingLocation(inUnit, inTransport, inLocation, outLocation); }

// 0x2135bc: returns true
bool EEUCLandTransport::HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo)
{ return true; }

// EEUCSpaceBuilding::ActionChanged's body (twin)
void EEUCNavalBuilding::ActionChanged(EEComplexUnit* inUnit)
{     EEUCBuilding::ActionChanged(inUnit); }

// EEUCNavalPriest::CreateUnitAt's body (twin)
EEUnit* EEUCNavalBuilding::CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation(inTransformation);
    theTransformation.SetX(theTransformation.GetX());
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// EEUCNavalCitizen::CreateUnit's body (twin)
EEUnit* EEUCNavalBuilding::CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation;
    theTransformation.SetX((float)inX);
    theTransformation.SetY((float)inY);
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// EEUCSpaceBuilding::HasArrived's body (twin)
bool EEUCNavalBuilding::HasArrived(EEComplexUnit* inUnit, float inValue)
{
    GE3DPoint theLocation;
    bool theResult = false;
    if (inUnit->mCollisionState == 8 || inValue <= 0.05f)
        theResult = true;
    else {
        inUnit->GetNextWaypointPoint(theLocation);
        if (inUnit->GetTileX() == UMFloor(theLocation.GetX()) && inUnit->GetTileY() == UMFloor(theLocation.GetY()))
            theResult = true;
    }
    return theResult;
}

// EEUCNavalCitizen::LocationToTile's body (twin)
void EEUCNavalBuilding::LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY)
{
    GE3DPoint thePoint;
    GE3DLine theLine(::gWorld.mLine);
    theLine.SetStart(inLocation);
    float theT;
    theLine.SolveForZ(0.0f, theT, thePoint);
    outX = UMFloor(thePoint.GetX());
    outY = UMFloor(thePoint.GetY());
}

// EEUCSpaceBuilding::UnitCreated's body (twin)
void EEUCNavalBuilding::UnitCreated(EEComplexUnit* inUnit)
{
    EEUCBuilding::UnitCreated(inUnit);
    inUnit->SetBuilt(1, 0, 1);
}

// EEUCBasicCitizen::UpdateEffects's body (twin)
void EEUCNavalBuilding::UpdateEffects(EEComplexUnit* inUnit)
{
    if (this->mRippleEffect != NULL && inUnit->mHitPoints != 0 && (unsigned long)this->mRippleEffect->mDetailLevel <= gEffectDetail)
        this->UpdateRipples(inUnit);
}

// EEUCNavalCitizen::CreateUnit's body (twin)
EEUnit* EEUCNavalPriest::CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation;
    theTransformation.SetX((float)inX);
    theTransformation.SetY((float)inY);
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// EEUCLandPriest::Update's body (twin)
void EEUCNavalPriest::Update(EEComplexUnit* inUnit)
{     EEUCBasicPriest::Update(inUnit); }

// EEUCSpaceTransport::AddToTile's body (twin)
void EEUCNavalTransport::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// EEUCNaval::AdjustPath's body (twin)
void EEUCNavalTransport::AdjustPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inSmooth)
{     EEAdjustNavalPath(inUnit, ioPath, inSmooth); }

// EEUCSpaceTransport::CanCarry's body (twin)
bool EEUCNavalTransport::CanCarry(EEComplexUnit* inUnit1, EEComplexUnit* inUnit2)
{
    bool theResult = false;
    if (inUnit1->mTileX != -10000 && inUnit2->mTileX != -10000) {
        unsigned short theFirst = gContinentManager.GetContinentID(inUnit1->mTileX, inUnit1->GetTileY());
        unsigned short theSecond = gContinentManager.GetContinentID(inUnit2->GetTileX(), inUnit2->GetTileY());
        if (EEUCTransport::CanCarry(inUnit1, inUnit2) && this->ContinentsConnect(theFirst, theSecond))
            theResult = true;
        else
            theResult = false;
    }
    return theResult;
}

// EEUCNavalPriest::CreateUnitAt's body (twin)
EEUnit* EEUCNavalTransport::CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation(inTransformation);
    theTransformation.SetX(theTransformation.GetX());
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// EEUCNavalCitizen::CreateUnit's body (twin)
EEUnit* EEUCNavalTransport::CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation;
    theTransformation.SetX((float)inX);
    theTransformation.SetY((float)inY);
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// EEUCNaval::EvaluateCondition's body (twin)
long EEUCNavalTransport::EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent)
{
    GE3DPoint thePosition;
    GEBoundingBox theBox;
    GEBoundingBox theTransformedBox;
    EEComplexUnit* theUnit = inUnit;
    const GEPhysicalModel& thePhysicalModel = theUnit->mPhysicalModel;
    GEModel* theModel = thePhysicalModel.GetModel();
    long theResult = 0;
    if (inEvent != kTriggerDyingAnimationFinished)
        theResult = EEUnitClass::EvaluateCondition(theUnit, inEvent);
    else {
        theUnit->GetPosition(thePosition);
        GETransformation theTransformation(false);
        thePhysicalModel.GetLocation().Apply(theModel->GetInternalTransform(thePhysicalModel.GetFrame()),
                                             theTransformation);
        float theScale = thePhysicalModel.GetScale();
        theTransformation.Scale(theScale, theScale, theScale);
        theTransformation.SetX(thePosition.GetX());
        theTransformation.SetY(thePosition.GetY());
        theTransformation.SetZ(thePosition.GetZ());
        theBox = theModel->GetBoundingBox(theUnit->mPhysicalModel.GetFrame());
        theBox.Transform(theTransformation, theTransformedBox);
        if (theTransformedBox.GetTopRightBack().GetZ() < 0.0f)
            theResult = 1;
    }
    return theResult;
}

// EEUCSpaceTransport::FindRendezvousTiles's body (twin)
void EEUCNavalTransport::FindRendezvousTiles(std::vector<EEComplexUnit*>* inUnits, EEComplexUnit* inTransport, U2DPoint<long>* outTransportTile, U2DPoint<long>* outUnitTile)
{
    U2DPoint<long> theGather;
    U2DPoint<long> theFrom(inTransport->mTileX, inTransport->mTileY);
    unsigned short theUnitContinent = gContinentManager.GetContinentID((*inUnits->begin())->mTileX, (*inUnits->begin())->mTileY);
    unsigned short theTransportContinent = gContinentManager.GetContinentID(inTransport->mTileX, inTransport->mTileY);
    this->FindGatheringTile(inUnits, theFrom, &theGather);
    if (theUnitContinent == theTransportContinent) {
        EEUCTransport::FindRendezvousTiles(inUnits, inTransport, outTransportTile, outUnitTile);
    } else {
        U2DPoint<long> theTransportTile(inTransport->mTileX, inTransport->mTileY);
        U2DPoint<long> theUnitTile((*inUnits->begin())->mTileX, (*inUnits->begin())->mTileY);
        EETileCoordinate theStart(inTransport->mTileX, inTransport->mTileY);
        this->FindShoreTile(inTransport, theStart, &theUnitTile, &theTransportTile, true, outTransportTile);
        theFrom = *outTransportTile;
        theTransportTile = U2DPoint<long>((*inUnits->begin())->mTileX, (*inUnits->begin())->mTileY);
        this->FindLandingTile(inTransport, &theFrom, &theTransportTile, outUnitTile);
    }
    this->FindMeetingTiles(inTransport, (*inUnits->begin()), outTransportTile, outUnitTile);
}

// 0x2135bc: returns true
bool EEUCNavalTransport::HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo)
{ return true; }

// EEUCNavalCitizen::LocationToTile's body (twin)
void EEUCNavalTransport::LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY)
{
    GE3DPoint thePoint;
    GE3DLine theLine(::gWorld.mLine);
    theLine.SetStart(inLocation);
    float theT;
    theLine.SolveForZ(0.0f, theT, thePoint);
    outX = UMFloor(thePoint.GetX());
    outY = UMFloor(thePoint.GetY());
}

// EEUCSpaceTransport::RemoveFromTile's body (twin)
void EEUCNavalTransport::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// EEUCAirplane::Update's body (twin)
void EEUCPlanetSpaceship::Update(EEComplexUnit* inUnit)
{     EEUCLandable::Update(inUnit); }

// EEUCAmbient::RemoveFromTile's body (twin)
void EEUCResource::RemoveFromTile(EEUnit* inUnit)
{
    long theSize = this->GetTileSize();
    EEMap* theMap = ::gWorld.mMap;
    long theX = inUnit->mTileX;
    long theY = inUnit->mTileY;
    long theEndX = theX + theSize;
    long theEndY = theY + theSize;
    EEMapTile* theTile = theMap->GetTile(theX, theY);
    if (theTile != NULL)
        theTile->RemoveUnit(inUnit);
    for (long i = theX; i < theEndX; i++) {
        for (long j = theY; j < theEndY; j++) {
            if (i == theX && j == theY)
                continue;
            EEMapTile* theOther = theMap->GetTile(i, j);
            if (theOther != NULL)
                theOther->RemoveUnit(inUnit);
        }
    }
}

// EEUCBasicCitizen::UpdateEffects's body (twin)
void EEUCSeaAnimal::UpdateEffects(EEComplexUnit* inUnit)
{
    if (this->mRippleEffect != NULL && inUnit->mHitPoints != 0 && (unsigned long)this->mRippleEffect->mDetailLevel <= gEffectDetail)
        this->UpdateRipples(inUnit);
}

// EEUCNavalBuilding::EvaluateCondition's body (twin)
long EEUCSpaceBuilding::EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent)
{
    long theResult = 0;
    bool theHandled;
    switch (inEvent) {
    case kTriggerFighterHasGoal:
        theHandled = inUnit->GetQueuedGoal(1) != NULL;
        break;
    case kTriggerAirplaneAvailableToLaunch:
        theHandled = this->SelectUnitToRelease(inUnit);
        break;
    case kTriggerDyingAnimationFinished:
        theHandled = this->HasSunk(inUnit);
        break;
    default:
        theResult = EEUCBuilding::EvaluateCondition(inUnit, inEvent);
        theHandled = false;
        break;
    }
    if (theHandled)
        theResult = 1;
    return theResult;
}

// 0xb3257: returns true
bool EEUCSpaceBuilding::HasSunk(EEComplexUnit* inUnit)
{ return true; }

// EEUCNavalBuilding::SelectUnitToRelease's body (twin)
bool EEUCSpaceBuilding::SelectUnitToRelease(EEComplexUnit* inUnit)
{
    bool theResult = false;
    unsigned long theCount = inUnit->GetNumCargoUnits();
    for (unsigned long i = 0; !theResult && i < theCount; i++) {
        EEComplexUnit* theCargo = inUnit->mCargo[i];
        if (theCargo->mHitPoints != theCargo->mStats->mMaxHitPoints)
            continue;
        bool theReady = false;
        if (theCargo->mPlayer->IsHuman()) {
            if (theCargo->GetQueueGoals(1).size() > 0) {
                EEUnitGoal* theGoal = (theCargo->mGoalQueues.begin() + 1)->GetFront();
                if (dynamic_cast<EEUGAttackUnit*>(theGoal) == 0 &&
                    dynamic_cast<EEUGLocation*>(theGoal) == 0 &&
                    dynamic_cast<EEUGExplore*>(theGoal) == 0)
                    theGoal = NULL;
                if (theGoal != NULL)
                    theReady = true;
            }
            if (!theReady && inUnit->GetQueueGoals(1).size() > 0) {
                EEUnitGoal* theGoal = (inUnit->mGoalQueues.begin() + 1)->GetFront();
                if (dynamic_cast<EEUGAttackUnit*>(theGoal) == 0 &&
                    dynamic_cast<EEUGLocation*>(theGoal) == 0 &&
                    dynamic_cast<EEUGExplore*>(theGoal) == 0)
                    theGoal = NULL;
                if (theGoal != NULL)
                    theReady = true;
            }
        }
        if (theReady || (inUnit->mPlayer->mComputerPlayer != NULL && theCargo->mGoalState != NULL)) {
            inUnit->SetExitingUnitID(theCargo->mUnitID);
            theResult = true;
        }
    }
    return theResult;
}

// 0x157269: empty
void EEUCSpaceBuilding::UpdateEffects(EEComplexUnit*)
{}

// EEUCNavalTransport::FindBoardingLocation's body (twin)
bool EEUCSpaceTransport::FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation)
{     return EEUCTransport::FindBoardingLocation(inUnit, inTransport, inLocation, outLocation); }

// EEUCNavalTransport::FindUnloadTiles's body (twin)
void EEUCSpaceTransport::FindUnloadTiles(EEComplexUnit* inTransport, U2DPoint<long>* inDestination, U2DPoint<long>* outUnloadTile, U2DPoint<long>* outFirstCargoTile, U2DPoint<long>* outTile)
{
    std::vector<EEUnit*> theUnits;
    std::vector<U2DPoint<long> > theTiles;
    float theRadius = this->mUnloadRange;
    float theRange = theRadius * theRadius;
    EETileCoordinate theFrom = inTransport->GetTile();
    this->FindShoreTile(inTransport, theFrom, inDestination, inDestination, true, outUnloadTile);
    this->FindLandingTile(inTransport, outUnloadTile, inDestination, outFirstCargoTile);
    if (outUnloadTile->DistanceToSquaredF(*inDestination) < theRange
        && this->CanUnloadNear(NULL, inTransport->GetCargo(0), outUnloadTile, outFirstCargoTile, inDestination)) {
        if (this->CanUnloadNear(NULL, *(inTransport->mCargo.begin() + 0), outUnloadTile, outFirstCargoTile, inDestination))
            *outTile = *inDestination;
        else
            *outTile = *outFirstCargoTile;
    } else {
        EEGetTileLine(outFirstCargoTile->GetX(), outFirstCargoTile->GetY(), inDestination->GetX(),
                   inDestination->GetY(), &theTiles, 1, 0);
        std::vector<U2DPoint<long> >::iterator theTile = theTiles.begin();
        while (true) {
            if (theTile == theTiles.end())
                break;
            U2DPoint<long>* thePoint = &(*theTile);
            float theDistance = outUnloadTile->DistanceToSquaredF(*thePoint);
            EEMapTile* theMapTile = ::gWorld.mMap->GetTile(thePoint->GetX(), thePoint->GetY());
            if (thePoint == NULL || theDistance > theRange
                || EEGetBlockingUnits(inTransport->GetPlayer(), this->GetUnitType(), theMapTile, 0, theUnits)
                || !this->CanUnloadNear(NULL, inTransport->mCargo[0], outUnloadTile, outFirstCargoTile, thePoint))
                break;
            theTile++;
        }
        if (theTile != theTiles.begin() && theTiles.size() > 0)
            theTile--;
        if (theTile != theTiles.end())
            *outTile = *theTile;
        else
            *outTile = *outFirstCargoTile;
    }
}

// 0x2135bc: returns true
bool EEUCSpaceTransport::HasRoom(EEComplexUnit* inUnit, EEComplexUnit* inCargo)
{ return true; }

// EEUCNavalPriest::AdjustPath's body (twin)
void EEUCSubmergedNaval::AdjustPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inSmooth)
{
    std::vector<GE3DPoint> thePath;
    long theCount = ioPath.size();
    long theStep = UMFloatToLong(this->mInnerCollisionSphere);
    if (inSmooth != true)
        return;
    thePath.reserve(ioPath.size());
    // a collision sphere that rounds to 0 keeps every point (it used to divide by zero); as in
    // EEUCNavalCitizen::AdjustPath
    if (theStep == 0)
        theStep = 1;
    for (long i = 0; i < theCount; i++) {
        if (i == theCount - 1 || i == 0 || i % theStep == 0)
            thePath.push_back(ioPath[i]);
    }
    ioPath = thePath;
}

// EEUCAirAnimal::Animate's body (twin)
long EEUCSubmergedNaval::Animate(EEComplexUnit* inUnit)
{     return EEUnitClass::Animate(inUnit); }

// EEUCSpaceTransport::RemoveFromTile's body (twin)
void EEUCSubmergedNaval::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// EEUCBasicPriest::UnitCreated's body (twin)
void EEUCSubmergedNaval::UnitCreated(EEComplexUnit* inUnit)
{     EEUnitClass::UnitCreated(inUnit); }

// EEUCBasicCitizen::UpdateEffects's body (twin)
void EEUCTorpedo::UpdateEffects(EEComplexUnit* inUnit)
{
    if (this->mRippleEffect != NULL && inUnit->mHitPoints != 0 && (unsigned long)this->mRippleEffect->mDetailLevel <= gEffectDetail)
        this->UpdateRipples(inUnit);
}

// 0x157269: empty
void EEUCTransport::BadlyHurt(EEUnit* inUnit)
{}

// EEUCBasicCitizen::UpdateEffects's body (twin)
void EEUCTransport::UpdateEffects(EEComplexUnit* inUnit)
{
    if (this->mRippleEffect != NULL && inUnit->mHitPoints != 0 && (unsigned long)this->mRippleEffect->mDetailLevel <= gEffectDetail)
        this->UpdateRipples(inUnit);
}

// 0x21d0b4: EEUGAttackGround::GetGoalLocation's code, which copies the point at +0x20
void EEUGBuildRepair::GetGoalLocation(GE3DPoint& outLocation)
{
    outLocation = this->mTargetLocation;
}

// 0x12c734: returns +0x38 (EECivilization::GetMaxPoints's code)
EEComplexUnit* EEUGHealUnit::GetTarget()
{
    return this->mTarget;
}

// 0xc1ee0: returns 6
long EEUGLoadContainer::GetMaxPathAttempts()
{ return 6; }

// 0x227264: returns 0x1f40
long EEUGLoadContainer::GetStartAction()
{ return 0x1f40; }

// 0x220a18: returns true
bool EEUGLoadContainer::Suits(EEUnit* inUnit)
{ return true; }

// 0xb3257: returns true
bool EEUGLoadGarrison::CanContain(EEComplexUnit* inUnit)
{ return true; }

// EEUICUnitIconBar::MouseRButtonDown's body (twin)
long EEUIMiniMap::MouseLButtonDown()
{
    this->CaptureMouse();
    return 0;
}

// 0x157269: empty
void EEUnitClass::Fn157269(EEComplexUnit* inUnit)
{}

// EEUnitClass::ActionChanged's body (twin)
void EEUnitClass::HandleActionChanged(EEComplexUnit* inUnit)
{
    inUnit->ReleaseHealEffect();
    switch (inUnit->mCommand ? inUnit->mCommand->mType : 0) {
    case kActionAnimalParentalVengance:
        inUnit->mHerd->AttackBack(inUnit, true);
        break;
    case kActionAnimalRetaliation:
        if (inUnit->mHerd != NULL)
            inUnit->mHerd->HandleUnitHit(inUnit);
        break;
    case kActionCheckRange:
        if (inUnit->mInitialContactHeading == kUnitAINoHeading) {
            GEVector theUp(0.0f, 1.0f, 0.0f);
            GEVector theDirection(0.0f, 0.0f, 0.0f);
            const GETransformation& theLocation = inUnit->mPhysicalModel.GetLocation();
            GE3DPoint thePoint(theLocation.GetX(), theLocation.GetY(), 0.0f);
            theLocation.Apply(theUp, theDirection);
            float theYaw = UMArcTan2(-theDirection.GetX(), theDirection.GetY());
            if (inUnit->mInitialContactLocation != thePoint)
                inUnit->mInitialContactLocation = thePoint;
            if (inUnit->mInitialContactHeading != theYaw)
                inUnit->mInitialContactHeading = theYaw;
        }
        break;
    case kActionRetaliateAgainstAttacker:
        this->Attacked(inUnit);
        break;
    case kActionRunFromAttacker:
        this->FleeAttacker(inUnit);
        break;
    case kActionShouldIFollowEnemyUnit:
        if (inUnit->mInitialContactHeading == kUnitAINoHeading) {
            GEVector theUp(0.0f, 1.0f, 0.0f);
            GEVector theDirection(0.0f, 0.0f, 0.0f);
            const GETransformation& theLocation = inUnit->mPhysicalModel.GetLocation();
            GE3DPoint thePoint(theLocation.GetX(), theLocation.GetY(), 0.0f);
            theLocation.Apply(theUp, theDirection);
            float theYaw = UMArcTan2(-theDirection.GetX(), theDirection.GetY());
            inUnit->SetInitialContactLocation(thePoint);
            if (inUnit->mInitialContactHeading != theYaw)
                inUnit->mInitialContactHeading = theYaw;
        }
        break;
    case kActionWaitForReload:
        if ((inUnit->mUnitClass->mDomains & kMovementAir) == 0 && inUnit->mUnitClass->mCategory != kUnitCategoryPlanetSpaceship
            && inUnit->GetAnimation() != (unsigned long)EEGetAttackFrameSet(inUnit)) {
            inUnit->SetCurrentPhysicsAction(3);
            if (inUnit->GetAnimation() != 0)
                inUnit->SetAnimation(0, -1, 1);
        }
        break;
    case kActionReturnToInitialContactLocation: {
        EEUnitGoal* theLocation = new EEUGLocation(&inUnit->mInitialContactLocation, false, false, true, inUnit->mInitialContactHeading, false, false);
        EEUnitGoal* theGoal = inUnit->mGoalState;
        if (theGoal != NULL && (theGoal->mKind == kGoalPatrol || theGoal->mKind == kGoalLocation || theGoal->mKind == kGoalExplore))
            inUnit->SetGoal(theLocation, 1, 0);
        else
            inUnit->SetGoal(theLocation, 0, 0);
        inUnit->SetTargetSearchTimer(0);
        break;
    }
    case kActionPostCalamityFlee:
        inUnit->FleeAfterCalamity(0);
        break;
    case kActionIdle: {
        inUnit->SetCurrentPhysicsAction(3);
        EEUnitClassUpgrade* theStats = inUnit->mStats;
        inUnit->mNextTriggerTime = -1;
        unsigned long theState = inUnit->GetAnimation();
        if (inUnit->IsSwimming())
            inUnit->SetAnimation(2, -1, 0);
        else if ((!this->IsLandBuilding() || this->mAttack > 0) && theState != 7 && (inUnit->mStats->mCanFireOverWalls || theState != 8)) {
            if (gPhysics->GetRandom() < 0.5f || inUnit->mStats->mCanFireOverWalls)
                inUnit->SetAnimation(7, -1, 0);
            else
                inUnit->SetAnimation(8, -1, 0);
            inUnit->SetIsAnimationPaused(true);
            inUnit->SetAnimationTime(gPhysics->GetRandom(0L, (long)theStats->mModel->mHoldTimes[inUnit->GetAnimation()]));
        }
        inUnit->DropPath();
        if (this->mDomains & kMovementSeaOrSpace)
            inUnit->UpdateWaterState(false);
        inUnit->mPlayer->UnitGoalDone(inUnit);
        if (!((inUnit->mUnitClass->mCategory == kUnitCategoryLandCitizen || inUnit->mUnitClass->mCategory == kUnitCategoryNavalCitizen) && inUnit->mContainer != NULL
              && inUnit->GetQueueGoals(0).size() > 1))
            inUnit->ClearGoals(0);
        inUnit->mInitialContactHeading = kUnitAINoHeading;
        if (inUnit->mGoalState == NULL)
            gPathManager.CancelPaths(inUnit, true);
        break;
    }
    case kActionResourcesRotting:
        inUnit->SetCurrentPhysicsAction(6);
        if (!inUnit->mUnitClass->mIsWall)
            inUnit->SetAnimation(5, -1, 0);
        inUnit->SetRottingAwayTime(0);
        inUnit->ClearGoals(0);
        if (inUnit->mNextPart != NULL) {
            inUnit->mNextPart->StartAction(kActionResourcesRotting);
            inUnit->mNextPart->mUnitClass->ActionChanged(inUnit->mNextPart);
        }
        break;
    case kActionDying:
        this->UnitDied(inUnit);
        break;
    case kActionReacquireGoal:
        if (inUnit->mGoalState != NULL && inUnit->mGoalState->mKind == kGoalLoadTransport)
            inUnit->mGoalState->Step(inUnit);
        break;
    case kActionUnitRotting:
        inUnit->mPlayer->DeselectUnit(inUnit);
        inUnit->SetRottingAwayTime(this->mRottingTime);
        if (inUnit->mNextPart != NULL) {
            inUnit->mNextPart->StartAction(kActionUnitRotting);
            inUnit->mNextPart->mUnitClass->ActionChanged(inUnit->mNextPart);
        }
        break;
    case kActionFadingOutInSpace:
        inUnit->SetCloakedChain(true);
        inUnit->SetOpacityChain(1.0f);
        break;
    case kActionKillSilently:
        inUnit->Kill(false, false, false);
        break;
    case kActionKill:
        inUnit->Kill(true, false, false);
        break;
    case kActionResourcesDepleting:
        inUnit->SetCurrentPhysicsAction(3);
        break;
    case kActionDead: {
        inUnit->SetCurrentPhysicsAction(6);
        inUnit->mPlayer->KillUnit(inUnit, false);
        for (EEComplexUnit* theUnit = inUnit->mNextPart; theUnit != NULL; theUnit = theUnit->mNextPart)
            inUnit->mPlayer->KillUnit(theUnit, false);
        break;
    }
    case kActionPrepareToMove: {
        EEUnitGoal* theGoal = inUnit->mGoalState;
        if ((unsigned long)theGoal->mPathAttempts > (unsigned long)theGoal->GetMaxPathAttempts()) {
            inUnit->StartAction(kActionIdle);
            inUnit->mUnitClass->ActionChanged(inUnit);
        } else
            this->PathToGoal(inUnit);
        break;
    }
    case kActionMoveToValidMapLocation:
        this->MoveToTileCentre(inUnit);
        break;
    case kActionGetNextAutoWaypoint:
        inUnit->ClearGoals(0);
        break;
    case kActionGetNextPatrolPoint:
        inUnit->mInitialContactHeading = kUnitAINoHeading;
        if (inUnit->mGoalState != NULL && inUnit->mGoalState->mKind == kGoalPatrol)
            inUnit->mGoalState->Step(inUnit);
        break;
    case kActionFleeDanger:
        this->Flee(inUnit);
        break;
    case kActionGetNextMoveWaypoint:
        this->NextWaypoint(inUnit);
        break;
    case kActionFindSafeLocation:
        this->FindSafeLocation(inUnit);
        break;
    case kActionAdvance:
        inUnit->SetCurrentPhysicsAction(0);
        inUnit->SetHasArrived(false);
        if (inUnit->mUnitClass->mCategory == kUnitCategoryLandCitizen && inUnit->mResourceAmount > 0 && inUnit->mUnitClass->mResourceAmount == 0)
            inUnit->SetAnimation(2, -1, 0);
        else if (inUnit->IsSwimming())
            inUnit->SetAnimation(2, -1, 0);
        else
            inUnit->SetAnimation(0xd, -1, 0);
        break;
    case kActionTurnToUnitFacing: {
        EEUnitGoal* theGoal = inUnit->mGoalState;
        if (theGoal != NULL && theGoal->mKind == kGoalLocation && static_cast<EEUGLocation*>(theGoal)->FaceYaw(inUnit))
            break;
        inUnit->SetCurrentPhysicsAction(5);
        if (inUnit->mUnitClass->mCategory == kUnitCategoryLandCitizen && inUnit->mResourceAmount > 0 && inUnit->mUnitClass->mResourceAmount == 0
            && inUnit->GetAnimation() != 2)
            inUnit->SetAnimation(2, -1, 0);
        else if (inUnit->IsSwimming())
            inUnit->SetAnimation(2, -1, 0);
        else if (inUnit->GetAnimation() != 0xd)
            inUnit->SetAnimation(0xd, -1, 0);
        break;
    }
    case kActionRepathAroundObstacle:
        this->FindPath(inUnit);
        break;
    case kActionHealUnit:
        this->Heal(inUnit);
        break;
    case kActionLookForHealee: {
        EEUnit* theTarget = NULL;
        EEUnitGoal* theGoal = inUnit->mGoalState;
        if (this->FindTargetNear(inUnit, inUnit->mStats->GetLineOfSight(), &theTarget)) {
            EEUnitGoal* theHeal = new EEUGHealUnit(static_cast<EEComplexUnit*>(theTarget), false);
            if (theGoal != NULL && (theGoal->mKind == kGoalPatrol || theGoal->mKind == kGoalLocation || theGoal->mKind == kGoalExplore))
                inUnit->SetGoal(theHeal, 1, 0);
            else
                inUnit->SetGoal(theHeal, 0, 0);
        }
        inUnit->SetHealSearchTimer(this->mHealDelay);
        break;
    }
    case kActionAttackEnemyUnit:
    case kActionAttackLocation:
    case kActionFaceEnemyUnit:
    case kActionCastCalamity:
    case kActionFaceLocation:
    case kActionPrepareToCastCalamity:
    case kActionCalamityPreCast:
    case kActionChangeTargetForSpecialEnemy:
    case kActionInsufficientResources:
    case kActionChangeTargetForSpecialRepair:
    case kActionReacquireEnemyUnit:
    case kActionFindUnexploredArea:
    case kActionLoadIntoContainer:
    case kActionUnloadTransport:
    case kActionMoveToRendezvous:
    case kActionPickedUpLoadableUnits:
    case kActionGetContainerProgress:
    case kActionGetNextTransportWaypoint:
    case kActionBumpExtraLoadableUnits:
    case kActionGarrisonUpgrade:
    case kActionWaitForGarrisonCost:
    case kActionWaitForPassengers:
    case kActionWaitForTransport:
    case kActionCannotReachContainer:
    case kActionTurnToFaceHealee:
        if (inUnit->mGoalState != NULL)
            inUnit->mGoalState->Step(inUnit);
        break;
    case kActionSpecialLocation:
        break;
    default:
        return;
    }
    EEUnitGoal* theGoal = inUnit->mGoalState;
    if (theGoal != NULL && !gWorldClearing)
        theGoal->SetStartTime(gPhysics->mGameTime);
}

// 0x1f1f81: returns +0x60 (EEComplexDoppelganger::GetTileX's code)
EEUnitClass* EEUnitClassUpgrade::GetUnitClass()
{
    return this->mUnitClass;
}

// 0x157269: empty
void EEWorld::CameraChanged(const GETransformation& inTransformation)
{}

// RTSWorld::GetPlayerIndex's body (twin)
long EEWorld::GetPlayerIndex(EEPlayer* inPlayer)
{
    long theResult = -1;
    for (unsigned long i = 0; i < this->mNumPlayers; i++) {
        if (this->mPlayers[i] == inPlayer) {
            theResult = i;
        }
    }
    return theResult;
}

// 0x157269: an empty body
void RTSUnit::CopyFrom(RTSUnit* inUnit)
{}

// 0x1720c6 (slot +0x08)
void EEAUpgradeImprovement::Virtual8()
{}

// 0x29d172 (slot +0x34): _purecall in the original
void EEGroup::Validate()
{
    EEPortUnwritten("EEGroup::Validate (pure virtual)");
}

// 0xac9f3 (slot +0x74)
bool EEPlayer::IsHuman()
{ return false; }

// 0x156dd8 (slot +0x24)
void EESEInstructions::MouseMove(const GE2DPoint& inLocation)
{}

// 0x1720c6 (slot +0x0c)
void EEUIFMGMainGroup::Select()
{}

// 0x1720c6 (slot +0x08)
void EEUIFMGMainGroup::Virtual08()
{}

// 0x29d172 (slot +0x88): _purecall in the original
EESEAPaint* EEUIFSEPaint::CreatePaintAction()
{
    EEPortUnwritten("EEUIFSEPaint::CreatePaintAction (pure virtual)");
}

// 0x1720c6 (slot +0x78)
void EEUIFSEPaint::TabbedFrom()
{}

// 0x1720c6 (slot +0x74)
void EEUIFSEPaint::TabbedTo()
{}

// 0x1ad67e (slot +0x7c)
long EEUIFSEPaint::UpdateKeyboard()
{ return 0; }

// 0x156dd8 (slot +0x8c)
void EEUnitClass::BadlyHurt(EEUnit* inUnit)
{}

// 0xac9f3 (slot +0x10)
bool EEUnitClass::CanAttackMove()
{ return false; }

// 0xac9f3 (slot +0x5c)
bool EEUnitClass::FliesOver()
{ return false; }

// 0x156dd8 (slot +0x04)
void EEUnitClass::GroundLocation(GE3DPoint& ioLocation)
{}

// 0xac9f3 (slot +0x48)
bool EEUnitClass::IsBuilding()
{ return false; }

// 0xac9f3 (slot +0x54)
bool EEUnitClass::IsLandBuilding()
{ return false; }

// 0x29059d (slot +0x58)
bool EEUnitClass::IsPartLeftOut(EEComplexUnit* inUnit)
{ return false; }

// 0x18aa9 (slot +0x60)
bool EEUnitClass::IsPickable()
{ return true; }

// 0x220a18 (slot +0x64)
bool EEUnitClass::IsSelectable(EEUnit* inUnit)
{ return true; }

// 0x792bc (slot +0xac)
bool EEUnitClass::LaunchProjectile(EEComplexUnit* inUnit, const GE3DPoint& inLocation)
{ return false; }

// 0x157269 (slot +0xb8)
void EEUnitClass::Update(EEComplexUnit* inUnit)
{}

// 0x157269 (slot +0xbc)
void EEUnitClass::UpdateEffects(EEComplexUnit* inUnit)
{}

// 0x29059d (slot +0x4c)
bool EEUnitClass::Virtual4C(unsigned long)
{ return false; }

// 0x156dd8 (slot +0x50)
void EEUnitClass::Virtual50(unsigned long)
{}

// 0x1ad67e (slot +0x70)
long EEUnitClass::Virtual70()
{ return 0; }

// 0x29d172 (slot +0x14): _purecall in the original
bool RTSPlayer::IsActive()
{
    EEPortUnwritten("RTSPlayer::IsActive (pure virtual)");
}

// 0x29d172 (slot +0x30): _purecall in the original
long RTSPlayer::LoadUnit(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
{
    EEPortUnwritten("RTSPlayer::LoadUnit (pure virtual)");
}

// 0x29d172 (slot +0x08): _purecall in the original
void RTSPlayer::RecountPopulation()
{
    EEPortUnwritten("RTSPlayer::RecountPopulation (pure virtual)");
}

// 0x29d172 (slot +0x34): _purecall in the original
void RTSPlayer::RemovalsDone()
{
    EEPortUnwritten("RTSPlayer::RemovalsDone (pure virtual)");
}

// 0x29d172 (slot +0x38): _purecall in the original
void RTSPlayer::RemovingUnit(RTSUnit* inUnit)
{
    EEPortUnwritten("RTSPlayer::RemovingUnit (pure virtual)");
}

// 0x29d172 (slot +0x3c): _purecall in the original
long RTSPlayer::SaveUnit(RTSUnit* inUnit, unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
{
    EEPortUnwritten("RTSPlayer::SaveUnit (pure virtual)");
}

// 0x29d172 (slot +0x2c): _purecall in the original
void RTSPlayer::UnitRemoved(RTSUnit* inUnit)
{
    EEPortUnwritten("RTSPlayer::UnitRemoved (pure virtual)");
}

// 0x157269 (slot +0x00)
void EEPCalamityHelper::GetChecksum(ESChecksum& ioChecksum) const
{}

// 0x157269 (slot +0x1c)
void EEPCalamityHelper::Virtual1C(long inArg)
{}

// 0x18357c (slot +0x08): EESETSETBuildObject::Store's body
void EESETSETMaintainObject::Store(EESMEffect* inEffect, EESETSEffectControls* inControls)
{
    inControls->mScenarioComboBoxes[1]->GetCurrentSelection(inEffect->mTargetObject);
}

// 0x1835f1 (slot +0x14): EESETSETBuildObject::Show's body
void EESETSETMaintainObject::Show(EESETSEffectControls* inControls)
{
    GERectangle theRect;
    UWideString theText;
    GE2DPoint theLocation;
    std::vector<UWideString> theNames;
    theLocation.SetX(405.0f);
    theLocation.SetY(641.0f);
    theRect = GERectangle(theLocation, GE2DPoint(theLocation.GetX() + 200.0f, theLocation.GetY() + 20.0f));
    gResource->GetString(IDS_TRIGGER_SCREEN_TAB_OBJECT, theText);
    inControls->mLabels[3]->SetRectangle(theRect);
    inControls->mLabels[3]->SetText(theText);
    inControls->mLabels[3]->SetDisplayMode(kDisplayModeShown);
    theLocation.SetY(inControls->mLabels[3]->GetRectangle().GetBottom() + 3.0f);
    inControls->mScenarioComboBoxes[1]->SetLocation(theLocation);
    inControls->mScenarioComboBoxes[1]->ChangeWidth(200);
    inControls->mScenarioComboBoxes[1]->mMode = kSETriggerScreenObject;
    inControls->mScenarioComboBoxes[1]->DeleteAllEntries();
    gWorld.mScenarioManager->GetObjectNames(theNames);
    inControls->mScenarioComboBoxes[1]->AppendStringArray(theNames);
    inControls->mScenarioComboBoxes[1]->SetDisplayMode(kDisplayModeShown);
}

// 0x189891 (slot +0x10): EESETSETPVisibility::Load's body
void EESETSETPCameraFadeIn::Load(EESMEffect* inEffect, EESETSEffectControls* inControls)
{
    UIEvent* theAction = inControls->mPlayers[0]->GetTriggerEvent(kControlTriggerSelectionChanged);
    if (theAction)
        theAction->Reference();
    inControls->mPlayers[0]->SetTriggerEvent(kControlTriggerSelectionChanged, NULL);
    inControls->mPlayers[0]->SetSelectedPlayer(inEffect->mPlayer);
    if (theAction) {
        inControls->mPlayers[0]->SetTriggerEvent(kControlTriggerSelectionChanged, theAction);
        theAction->Dereference();
    }
}

// 0x1898eb (slot +0x14): EESETSETPVisibility::Show's body
void EESETSETPCameraFadeIn::Show(EESETSEffectControls* inControls)
{
    inControls->mPlayers[0]->SetFlags(0xde);
}

// 0x189891 (slot +0x10): EESETSETPVisibility::Load's body
void EESETSETPCameraFadeOut::Load(EESMEffect* inEffect, EESETSEffectControls* inControls)
{
    UIEvent* theAction = inControls->mPlayers[0]->GetTriggerEvent(kControlTriggerSelectionChanged);
    if (theAction)
        theAction->Reference();
    inControls->mPlayers[0]->SetTriggerEvent(kControlTriggerSelectionChanged, NULL);
    inControls->mPlayers[0]->SetSelectedPlayer(inEffect->mPlayer);
    if (theAction) {
        inControls->mPlayers[0]->SetTriggerEvent(kControlTriggerSelectionChanged, theAction);
        theAction->Dereference();
    }
}

// 0x1898eb (slot +0x14): EESETSETPVisibility::Show's body
void EESETSETPCameraFadeOut::Show(EESETSEffectControls* inControls)
{
    inControls->mPlayers[0]->SetFlags(0xde);
}

// 0x156dd8 (slot +0x00)
void EETeam::GetChecksum(ESChecksum& ioChecksum) const
{}

// 0x157269 (slot +0x04)
void EEUCAirAnimal::GroundLocation(GE3DPoint& ioLocation)
{}

// 0xac9f3 (slot +0x10)
bool EEUCAirAnimal::CanAttackMove()
{ return false; }

// 0x157269 (slot +0xbc)
void EEUCAirAnimal::UpdateEffects(EEComplexUnit*)
{}

// 0x18aa9 (slot +0x10)
bool EEUCHelicopter::CanAttackMove()
{ return true; }

// 0x21a404 (slot +0x2c): EEUCSpaceship::GetSoundParameters's body
void EEUCHelicopter::GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch)
{
    EEUnitClass::GetSoundParameters(inUnit, inSound, outVolume, outPan, outPitch);
    EEGetDopplerPitch(inUnit, inSound, outVolume, outPan, outPitch);
}

// 0x18aa9 (slot +0x5c)
bool EEUCHelicopter::FliesOver()
{ return true; }

// 0x21a51a (slot +0x78): EEUCSpaceship::UnitCreated's body
void EEUCHelicopter::UnitCreated(EEComplexUnit* inUnit)
{
    GE3DPoint thePosition(0.0f, 0.0f, 0.0f);
    GETransformation theTransformation(inUnit->mPhysicalModel.GetLocation());
    EEUnitClass::UnitCreated(inUnit);
    thePosition.SetX(theTransformation.GetX());
    thePosition.SetY(theTransformation.GetY());
    thePosition.SetZ(theTransformation.GetZ());
    float theZ = this->GetHeightAt(inUnit, thePosition);
    theTransformation.SetZ(theZ);
    inUnit->SetTransformation(theTransformation, true);
    inUnit->SetHeight(theZ);
}

// 0x21aaa9 (slot +0x84): EEUCSpaceTransport::AddToTile's body
void EEUCHelicopter::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// 0x21aaf5 (slot +0x98): EEUCSpaceTransport::RemoveFromTile's body
void EEUCHelicopter::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// 0x2137f8 (slot +0xa0): EEUCHelicopterTransport::EvaluateCondition's body
long EEUCHelicopter::EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent)
{
    long theResult = 0;
    bool theFlag = false;
    switch (inEvent) {
    case kTriggerDyingAnimationFinished:
        if (inUnit->mPhysicalModel.GetLocation().GetZ() < inUnit->mTile->GetHeight())
            theFlag = true;
        else
            theFlag = false;
        break;
    case kTriggerMovePreparationComplete:
        theFlag = (long)inUnit->mWaypoints.size() > 0;
        break;
    default:
        theResult = EEUnitClass::EvaluateCondition(inUnit, inEvent);
        break;
    }
    if (theFlag)
        theResult = 1;
    return theResult;
}

// 0x20afe9 (slot +0xc0): EEUCAirAnimal::Animate's body
long EEUCHelicopter::Animate(EEComplexUnit* inUnit)
{
    return EEUnitClass::Animate(inUnit);
}

// 0x18aa9 (slot +0x10)
bool EEUCHelicopterTransport::CanAttackMove()
{ return true; }

// 0x21a404 (slot +0x2c): EEUCSpaceship::GetSoundParameters's body
void EEUCHelicopterTransport::GetSoundParameters(EEUnit* inUnit, EESoundObject* inSound, float* outVolume, float* outPan, float* outPitch)
{
    EEUnitClass::GetSoundParameters(inUnit, inSound, outVolume, outPan, outPitch);
    EEGetDopplerPitch(inUnit, inSound, outVolume, outPan, outPitch);
}

// 0x21aaa9 (slot +0x84): EEUCSpaceTransport::AddToTile's body
void EEUCHelicopterTransport::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// 0x21aaf5 (slot +0x98): EEUCSpaceTransport::RemoveFromTile's body
void EEUCHelicopterTransport::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// 0x157269 (slot +0xbc)
void EEUCHelicopterTransport::UpdateEffects(EEComplexUnit*)
{}

// 0x20afe9 (slot +0xc0): EEUCAirAnimal::Animate's body
long EEUCHelicopterTransport::Animate(EEComplexUnit* inUnit)
{
    return EEUnitClass::Animate(inUnit);
}

// 0x218d33 (slot +0xfc): EEUCNavalTransport::FindBoardingLocation's body
bool EEUCHelicopterTransport::FindBoardingLocation(EEComplexUnit* inUnit, EEComplexUnit* inTransport, GE3DPoint& inLocation, GE3DPoint& outLocation)
{
    return EEUCTransport::FindBoardingLocation(inUnit, inTransport, inLocation, outLocation);
}

// 0x218a45 (slot +0x04): EEUCNavalTransport::GroundLocation's body
void EEUCNaval::GroundLocation(GE3DPoint& ioLocation)
{
    if (ioLocation.GetZ() < 0.0f)
        ioLocation.SetZ(0.0f);
}

// 0x218490 (slot +0x08): EEUCNavalCitizen::LocationToTile's body
void EEUCNaval::LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY)
{
    GE3DPoint thePoint;
    GE3DLine theLine(::gWorld.mLine);
    theLine.SetStart(inLocation);
    float theT;
    theLine.SolveForZ(0.0f, theT, thePoint);
    outX = UMFloor(thePoint.GetX());
    outY = UMFloor(thePoint.GetY());
}

// 0x18aa9 (slot +0x10)
bool EEUCNaval::CanAttackMove()
{ return true; }

// 0x218605 (slot +0x30): EEUCNavalCitizen::CreateUnit's body
EEUnit* EEUCNaval::CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation;
    theTransformation.SetX((float)inX);
    theTransformation.SetY((float)inY);
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// 0x218909 (slot +0x34): EEUCNavalPriest::CreateUnitAt's body
EEUnit* EEUCNaval::CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation(inTransformation);
    theTransformation.SetX(theTransformation.GetX());
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// 0x21aaa9 (slot +0x84): EEUCSpaceTransport::AddToTile's body
void EEUCNaval::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// 0x21aaf5 (slot +0x98): EEUCSpaceTransport::RemoveFromTile's body
void EEUCNaval::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// 0x20d336 (slot +0xbc): EEUCBasicCitizen::UpdateEffects's body
void EEUCNaval::UpdateEffects(EEComplexUnit* inUnit)
{
    if (this->mRippleEffect != NULL && inUnit->mHitPoints != 0 && (unsigned long)this->mRippleEffect->mDetailLevel <= gEffectDetail)
        this->UpdateRipples(inUnit);
}

// 0x218a45 (slot +0x04): EEUCNavalTransport::GroundLocation's body
void EEUCNavalCitizen::GroundLocation(GE3DPoint& ioLocation)
{
    if (ioLocation.GetZ() < 0.0f)
        ioLocation.SetZ(0.0f);
}

// 0x218909 (slot +0x34): EEUCNavalPriest::CreateUnitAt's body
EEUnit* EEUCNavalCitizen::CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator)
{
    GETransformation theTransformation(inTransformation);
    theTransformation.SetX(theTransformation.GetX());
    theTransformation.SetZ(0.0f);
    return EEUnitClass::CreateUnitAt(theTransformation, inPlayer, inIsFoundation, inCreator);
}

// 0x18aa9 (slot +0x10)
bool EEUCSpaceship::CanAttackMove()
{ return true; }

// 0x21aaa9 (slot +0x84): EEUCSpaceTransport::AddToTile's body
void EEUCSpaceship::AddToTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->mUnits = theTile->AddUnit(theUnit, theTile->mUnits);
}

// 0x21aaf5 (slot +0x98): EEUCSpaceTransport::RemoveFromTile's body
void EEUCSpaceship::RemoveFromTile(EEUnit* inUnit)
{
    EEMap* theMap = ::gWorld.mMap;
    EEUnit* theUnit = inUnit->mComplexUnit;
    EEMapTile* theTile = theMap->GetTile(inUnit->mTileX, inUnit->mTileY);
    if (theTile != NULL)
        theTile->RemoveUnit(theUnit);
}

// 0x20afe9 (slot +0xc0): EEUCAirAnimal::Animate's body
long EEUCSpaceship::Animate(EEComplexUnit* inUnit)
{
    return EEUnitClass::Animate(inUnit);
}

// 0x91f11: the size of the vector at +0x04 (UReferenceContainer::GetSize's code)
unsigned long EEArtificialIntelligence::GetNumComputerPlayers() const
{
    return this->mComputerPlayers.size();
}

// 0x91f11, as above
unsigned long EECPAttackGroup::GetNumSubGroups() const
{
    return this->mSubGroups.size();
}

// 0x91f11, as above
unsigned long EECPIntelligencePlayerFile::GetNumArmies() const
{
    return this->mArmies.size();
}

// 0x91f11, as above
unsigned long EEGraphicObject::GetNumParts() const
{
    return this->mTurretParts.size();
}

// 0x138e4b: the size of the vector at +0x14
unsigned long EEGraphicObject::GetOrbitalCount() const
{
    return this->mOrbitals.size();
}

// 0x196dff: the size of the vector at +0x2c
long EEUnitClass::GetNumResearchTechnologies()
{
    return this->mResearchTechnologies.size();
}

// 0x216d8d (slot +0x6c): returns +0x258
long EEUnitClass::GetPopulationCost()
{
    return this->mWeight;
}

// 0x2907a3 (slot +0x08): an empty body
void EEUnitClass::LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY)
{}

// 0x2907a3 (slot +0x0c): an empty body
void EEUnitClass::AdjustPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inSmooth)
{}

// 0x2907a3 (slot +0x18): an empty body
void EEUnitClass::CargoAdded(EEComplexUnit* inUnit, EEComplexUnit* inCargo, bool inArg)
{}

// 0x221f43 (slot +0x70; EEUGGather::Suits's code): true for the unit categories 6 and 0x16
bool EEUGBuildRepair::Suits(EEUnit* inUnit)
{
    unsigned long theCategory = inUnit->mUnitClass->mCategory;
    return theCategory == 6 || theCategory == 0x16;
}

// 0xe123d: an empty body
EEUIFMGStatusBox::~EEUIFMGStatusBox()
{}

// 0x163d6f (slot +0x28): an empty body
void EECalamityClass::LoadMedia(EEPlayer* inPlayer, bool inIsOnDemand)
{}

// 0x157269 (slot +0x2c): an empty body. windows.h renames it PlaySoundA here, as in every unit that refers to it.
void EECalamityClass::PlaySound(EECalamity* inCalamity)
{}

// 0xdfd80 (EEE2DExplosion::StopSound's code, which works on +0x54): stops the secondary sound and releases it
void EECalamity::StopSecondarySound()
{
    if (this->mSecondarySound) {
        gSoundManager->Stop(this->mSecondarySound, false);
        this->ReleaseSecondarySound();
    }
}

// 0xe8eb1 (slot +0x3c): a jump to EEEffect::RepairPointers
long EEEffect2D::RepairPointers()
{
    return EEEffect::RepairPointers();
}

// 0x11c531 (slot +0x3c): calls the empty 0x157269 with its argument
void EEPMAirAnimal::Collide(EEComplexUnit* inUnit)
{}

// 0x1600dc (slot +0x40): calls EEPhysicsModel::Push (0x155984)
bool EEPMHelicopter::Push(EEComplexUnit* inUnit, EEComplexUnit* inOther)
{
    return EEPhysicsModel::Push(inUnit, inOther);
}

// 0x1600cc (slot +0x44): calls EEPhysicsModel::PushMoving (0x155247)
bool EEPMHelicopter::PushMoving(EEComplexUnit* inUnit, EEComplexUnit* inOther)
{
    return EEPhysicsModel::PushMoving(inUnit, inOther);
}

// 0x15d8de (slot +0x1c): EEPMSeaAnimal::TurnToward's body
void EEPMAirAnimal::TurnToward(EEComplexUnit* inUnit, const GE3DPoint& inFrom, const GE3DPoint& inTo, float* outA, float* outB, float* outC, bool inUseFacingOffset)
{
    GEVector theForward(0.0f, 1.0f, 0.0f);
    GEVector theResult;
    float theStep = gPhysics->mTickSeconds;
    float theAccel = inUnit->mStats->mAngularAcceleration;
    float theMax = inUnit->mStats->mMaxAngularVelocity;
    unsigned long theState = inUnit->mCurrentPhysicsOrder;
    float theRate = inUnit->mAngularVelocity.mYaw;
    if (theState == 5) {
        EEPhysicsModel::TurnToward(inUnit, inFrom, inTo, outA, outB, outC, inUseFacingOffset);
        return;
    }
    inUnit->mPhysicalModel.GetLocation().Apply(theForward, theResult);
    float theHeading = UMArcTan2(-theResult.mX, theResult.mY);
    float theNew;
    if (theState == 0) {
        theNew = theRate - theAccel * theStep;
        if (theNew < -theMax)
            theNew = -theMax;
    } else {
        theNew = theAccel * theStep + theRate;
        if (theNew > theMax)
            theNew = theMax;
    }
    *outA = 0.0f;
    *outB = theNew;
    *outC = theNew * theStep + theHeading;
}
// 0x15c146 (slot +0x2c): EEPMNaval::PitchMode's body
long EEPMSpaceship::PitchMode(EEComplexUnit* inUnit, float inRate, float inAngle)
{
    if (inAngle < 0.0f)
        inAngle = -inAngle;
    if (inRate < 0.0f)
        inRate = -inRate;
    EEUnitClass* theClass = inUnit->mUnitClass;
    float theTime = gPhysics->mTicksPerGameSecond;
    float theTicks = theTime * inRate / theClass->GetAngularAcceleration();
    float theStep = gPhysics->mTickSeconds;
    float theStop = (theTicks + 1.0f) * theTicks * 0.5f;
    theStop = theStop * (theStep * theStep) * theClass->GetAngularAcceleration();
    return inAngle <= theStop ? 4 : 3;
}
// 0x1898eb (slot +0x14): EESETSETPVisibility::Show's body
void EESETSETPlayerVictory::Show(EESETSEffectControls* inControls)
{
    inControls->mPlayers[0]->SetFlags(0xde);
}
// 0x18daaa (slot +0x0c): EESETSETSetObjectAttribute::VirtualC's body
void EESETSETSetObjectClassAttribute::VirtualC(EESETSEffectControls* inControls)
{
    inControls->mEffectComboBoxes[1]->SetDisplayMode(kDisplayModeHidden);
    inControls->mEffectComboBoxes[0]->SetDisplayMode(kDisplayModeHidden);
    inControls->mLabels[3]->SetDisplayMode(kDisplayModeHidden);
    inControls->mCheckBoxes[0]->SetDisplayMode(kDisplayModeHidden);
    inControls->mEditBoxes[0]->SetDisplayMode(kDisplayModeHidden);
}
// 0x4ec4b: EECPASGSeaTransport::SendUnitTo's body
void EECPASGSpaceTransport::SendUnitTo(EECPAttackSubGroupUnit* inUnit, const EECPILocation& inLocation, bool inFlag)
{
    GE2DPoint thePoint(0.0f, 0.0f);
    EECPILocation theLocation(inLocation);
    if (gWorld.mMap->GetTileAtLocation(theLocation.mTile) == NULL)
        theLocation.ResetToArea();
    if (inUnit == NULL || gWorld.mMap->GetTileAtLocation(theLocation.mTile) == NULL)
        return;
    thePoint.SetX(theLocation.mTile.mX);
    thePoint.SetY(theLocation.mTile.mY);
    EEUnitGoal* theGoal = new EEUGLocation(thePoint, true, inFlag, false, 0.0f, false, false);
    if (!this->GiveGoal(inUnit->GetUnit(), theGoal, inFlag, kActionPrepareToMove) && theGoal != NULL)
        delete theGoal;
}

// 0x27cf38 (slot +0x04): returns 0
long EEUIECECampaignNameChanged::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUIECEClose::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUIEMGOECloseOptions::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUIEMSETestButton::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUIEPDCSFormClose::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUIESPELearningScenarios::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUISIEMouseOutButton::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x27cf38 (slot +0x04): returns 0
long EEUISIEMouseOverButton::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{ return 0; }

// 0x1e136e (slot +0x04): EEUIEMGOEGameBriefing::ProcessEvent's body
long EEUIEMGDisplayScenarioIntro::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    EEDisplayScenarioIntro();
    return kErrorOK;
}

// 0x1d2aa1 (slot +0x04): EEUIECSSEBack::ProcessEvent's body
long EEUIESLECancelSave::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    gUserInterface->CloseForm(NULL);
    return kErrorOK;
}

// 0x1d6811 (slot +0x04): EEUIEEEEMainMenu::ProcessEvent's body
long EEUIESPLECancel::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    inForm->GetManager()->Pop();
    return kErrorOK;
}

// 0x2547cf (slot +0x04): EEDSUIHideDebugSoundPanel::ProcessEvent's body
long EEUIEWNSCancel::ProcessEvent(UIForm* inForm, UIEntity* inEntity)
{
    gUIManager->Pop();
    return kErrorOK;
}

// No retail body is identified: the item's position in the list, and whether it is there
template <>
bool UReferenceContainer<EEComplexUnit*>::Contains(EEComplexUnit* const& inItem,
                                                   std::vector<EEComplexUnit*>::iterator& outIterator)
{
    outIterator = std::find(this->mItems.begin(), this->mItems.end(), inItem);
    return outIterator != this->mItems.end();
}

// 0x8282: U2DComputeCenterPoint<long>'s code; it reads the flag as a bool
U2DPoint<long> EEFn08282(std::vector<EETileCoordinate>& inTiles, long inMustBeInList)
{
    return U2DComputeCenterPoint(inTiles, (unsigned char)inMustBeInList != 0);
}

// 0x27c0b4: returns false
bool EEModeKeyboardNone(void* inForm)
{
    return false;
}

// 0x27c0b4: returns false
bool EEFn27C0B4B(void* inForm)
{
    return false;
}

// returns 0 (the code of EEE3DTracerBeam::DrawPass2)
long EEModeMouseNone(void* inLocation, void* inForm)
{
    return 0;
}

// 0xfca98: returns 0
long EECBTrace(bool inFlag)
{
    return 0;
}

// 0xfca98: returns 0
long EEFn0FCA98()
{
    return 0;
}

// 0x1720c6: an empty body
void EEPhysicsTrace(const char* inFormat, unsigned long inUnitID, GEOrientation inOrientation)
{}

// EEStepAsideFromSite's code
void EEPlaceUnit(EEComplexUnit* inUnit, EEUnitClass* inClass, EEPlayer* inPlayer, const U2DPoint<long>& inTile, bool inArg)
{
    EEStepAsideFromSite(inUnit, inClass, inPlayer, const_cast<U2DPoint<long>&>(inTile), inArg);
}

// No retail body: EEUCAirplane's slot table has no such entry
long EEUCAirplane::FindUnloadTiles(EEComplexUnit* inTransport, U2DPoint<long>* inDestination,
                                   U2DPoint<long>* outUnloadTile, U2DPoint<long>* outFirstCargoTile,
                                   U2DPoint<long>* outTile)
{
    EEPortUnwritten("EEUCAirplane::FindUnloadTiles");
}

// 0x1498c2 (slot +0x20) is operator='s code, which takes an argument this declaration lacks
void EEPCalamityHelper::Virtual20()
{
    EEPortUnwritten("EEPCalamityHelper::Virtual20");
}

#ifdef EE_PORT_LINUX
// The Linux C++ ABI emits a class's vtable and typeinfo only with its first non-inline virtual function; MSVC and
// MinGW emit them wherever they are used. These classes' first virtual functions have no body elsewhere.
#include "ee/EEFOFindNewTargetScreenBase.h"

// 0x220a18 (slots +0x04 and +0x08): mov al, 1; ret 4
bool EEFOFindNewTargetScreenBase::Virtual04(EEComplexUnit* inUnit)
{
    return true;
}

bool EEFOFindNewTargetScreenBase::Virtual08(EEComplexUnit* inUnit)
{
    return true;
}

// No retail body is identified: every media class the game makes has its own
long EEMedia::LoadMedia()
{
    EEPortUnwritten("EEMedia::LoadMedia");
}

long EEMedia::UnloadMedia()
{
    EEPortUnwritten("EEMedia::UnloadMedia");
}
#endif
