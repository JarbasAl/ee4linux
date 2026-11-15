// The fight estimate's game part (AgeOfAIStats.h): each group's range, speed, minimum range, reload and area from the
// unit's type and its owner's stats, and the damage of one shot as the game computes it - the attack (the owner's stats
// of the type, else the type's) x the game's hit factor (gToHitTable by the attacker's ammunition) x the weapon table
// (gWeaponToHitFactors[target weapon][attacker weapon]), less the target's armour against the attacker's weapon. The
// game's height factor and the cloak factor are not counted, and neither is area damage in one shot.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/stats.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include <windows.h>
#include <unistd.h>
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
#include "ee/EECPResourceManager.h"
#include "ee/EECPCivilizationManager.h"
#include "ee/EEUGGather.h"
#include "ee/EEUGBuildRepair.h"
#include "ee/EEUGLocation.h"
#include "ee/EEUGAttackUnit.h"
#include "AgeOfAI.h"
#include "AgeOfAIRoles.h"
#include "AgeOfAISettings.h"

#include "ee/EEUnitBehavior.h"
#include "AgeOfAI/AgeOfAIStats.h"

extern EEToHitTable gToHitTable;           // 0x5318e8

namespace AgeOfAI {
namespace Stats {

namespace {

long OwnerOf(EEUnit* inUnit)
{
    return inUnit->mPlayer != NULL ? (long)inUnit->mPlayer->GetPlayerID() : 0;
}

int ShotOf(EEUnitClass* inAttacker, EEUnitClassUpgrade* inAttackerStats, long inAttackerOwner, EEUnitClass* inTarget,
           EEUnitClassUpgrade* inTargetStats, long inTargetOwner)
{
    if (inAttacker == NULL || inTarget == NULL)
        return 0;
    float theAttack = inAttackerStats != NULL ? inAttackerStats->mAttack : (float)inAttacker->mAttack;
    long theWeapon = inAttacker->mWeapon, theTargetWeapon = inTarget->mWeapon;
    if (theAttack <= 0 || theWeapon < 0 || theWeapon > 5 || theTargetWeapon < 0 || theTargetWeapon > 5)
        return 0;
    float theFactor = gToHitTable.GetAttackFactor(inAttacker->mAmmoType, inTarget, inAttackerOwner, inTargetOwner)
                      * gWeaponToHitFactors[theTargetWeapon][theWeapon];
    return ShotDamage(theAttack, theFactor, inTargetStats != NULL ? inTargetStats->mArmor[theWeapon] : 0.0f);
}

// seconds between shots: the owner's stats of the type, else the type's; 1 when neither has one
float ClassReload(EEUnitClass* inClass, EEUnitClassUpgrade* inStats)
{
    long theMs = inStats != NULL ? inStats->mReloadTime : 0;
    if (theMs <= 0 && inClass != NULL)
        theMs = inClass->mReloadTime;
    return theMs > 0 ? theMs / 1000.0f : 1.0f;
}

// a live unit's own reload (scenario triggers change it), else its class's
float UnitReload(EEUnit* inUnit)
{
    long theOwn = inUnit->mComplexUnit != NULL ? inUnit->mComplexUnit->mReloadTimer : 0;
    return theOwn > 0 ? theOwn / 1000.0f : ClassReload(inUnit->mUnitClass, inUnit->mStats);
}

Group KindOf(EEUnit* inRep, EEUnitClass* inClass, EEUnitClassUpgrade* inStats, long inOwner)
{
    Group theKind = {};
    theKind.mRep = inRep;
    theKind.mClass = inClass;
    theKind.mStats = inStats;
    theKind.mType = (long)inClass->mUnitType;
    theKind.mOwner = inOwner;
    theKind.mRange = inStats != NULL ? inStats->mRange : inClass->mRange;
    theKind.mSpeed = inStats != NULL ? inStats->mMaxVelocity : 0.0f;
    theKind.mMinRange = inClass->mMinRange;
    theKind.mReload = inRep != NULL ? UnitReload(inRep) : ClassReload(inClass, inStats);
    theKind.mArea = inStats != NULL ? inStats->mAreaEffect : 0.0f;
    return theKind;
}

}

EEUnitClassUpgrade* Record(EEPlayer* inPlayer, long inType)
{
    if (inPlayer == NULL || inType < 0 || (unsigned long)inType >= inPlayer->mUnitClassUpgrades.size())
        return NULL;
    return inPlayer->mUnitClassUpgrades[inType];
}

int Shot(const Group& inAttacker, const Group& inTarget)
{
    return ShotOf(inAttacker.mClass, inAttacker.mStats, inAttacker.mOwner, inTarget.mClass, inTarget.mStats, inTarget.mOwner);
}

int UnitShot(EEUnit* inAttacker, EEUnit* inTarget)
{
    return ShotOf(inAttacker->mUnitClass, inAttacker->mStats, OwnerOf(inAttacker), inTarget->mUnitClass, inTarget->mStats,
                  OwnerOf(inTarget));
}

float Range(EEUnit* inUnit)
{
    return inUnit->mStats != NULL ? inUnit->mStats->mRange : inUnit->mUnitClass->mRange;
}

float Speed(EEUnit* inUnit)
{
    return inUnit->mStats != NULL ? inUnit->mStats->mMaxVelocity : 0.0f;
}

bool Add(Side& ioSide, EEUnit* inUnit)
{
    if (inUnit->mUnitClass == NULL)
        return false;
    return Put(ioSide, KindOf(inUnit, inUnit->mUnitClass, inUnit->mStats, OwnerOf(inUnit)), (int)inUnit->mHitPoints,
               (int)inUnit->mTileX, (int)inUnit->mTileY);
}

bool AddRecord(Side& ioSide, long inType, long inOwner, int inHP, int inX, int inY)
{
    EEUnitClass* theClass = inType >= 0 && inType < 871 ? gUnitClassTable[inType] : NULL;
    EEPlayer* thePlayer = inOwner >= 0 && inOwner < 16 && (unsigned long)inOwner < gWorld.GetNumPlayers()
                          ? gWorld.GetPlayer(inOwner) : NULL;
    if (theClass == NULL || thePlayer == NULL)
        return false;
    return Put(ioSide, KindOf(NULL, theClass, Record(thePlayer, inType), inOwner), inHP, inX, inY);
}

void Estimate(Side& ioOwn, Side& ioFoe, Result& outResult)
{
    Estimate(ioOwn, ioFoe, Shot, outResult);
}

float Ratio(Side& ioOwn, Side& ioFoe)
{
    return Ratio(ioOwn, ioFoe, Shot);
}

}
}
