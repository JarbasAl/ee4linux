// C:\EEX\Empire Earth\Civilization Builder\EEAECloaking.cpp (name from RTTI EEAECloaking; folder inferred: no
// __FILE__). 0x27aa0b..0x27ac2c: the cloaking area effect (vtable 0x44ba94): ctor, Virtual18, ??_G, dtor,
// Virtual04, Virtual08, Virtual30, Virtual38. The economy area effects' slots follow at 0x27ac2c (EEAEEconomy.cpp).
// Unit flags: the exe's module set.
#include <windows.h>
#include <vector>
#include <map>
#include <set>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GE3DLine.h"
#include "lle/GEViewport.h"
#include "lle/GETransformation.h"
#include "lle/GEPhysicalModel.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFile.h"
#include "lle/U2DPoint.h"

#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEUnitSetManager.h"
#include "ee/EECivPowersManager.h"
#include "ee/EEMiniMap.h"
#include "ee/EEAreaUtilities.h"
#include "ee/EEAreaEffect.h"
#include "ee/EEAreaEffectTemplate.h"
#include "ee/EEAECloaking.h"

// FUNCTION: EXE 0x67aa0b
// Creates a cloaking area effect from its template
EEAECloaking::EEAECloaking(EEAreaEffectTemplate* inTemplate)
    : EEAreaEffect(inTemplate)
{
}

// FUNCTION: EXE 0x67aa23
// Returns the template's maximum level as the cloaking strength
long EEAECloaking::GetStrength()
{
    return this->mTemplate->mMaxLevel;
}

// SYNTHETIC: EXE 0x67aa2a
// (the scalar deleting destructor; 0x1c bytes)

// FUNCTION: EXE 0x67aa46
// Destroys the cloaking area effect
EEAECloaking::~EEAECloaking()
{
}

// FUNCTION: EXE 0x67aa51
// Makes the unit (and the units it carries) cloakable
void EEAECloaking::AppliedTo(EEComplexUnit* inUnit)
{
    inUnit->SetCloakableChain(true);
}

// FUNCTION: EXE 0x67aa5f
// Tells whether a unit of the template's unit set, owned by an affected player, is inside the effect's area
// (units whose technology is a kind-4 tech tree node are excluded)
bool EEAECloaking::Covers(unsigned long inPlayer, EEComplexUnit* inUnit, const GE2DPoint& inPoint)
{
    GE2DPoint theLocation(0.0f, 0.0f);
    unsigned long theOwner = inUnit->GetPlayer()->mPlayerID;
    bool theResult = false;
    if (gUnitSetManager.Contains(inUnit->GetUnitClass()->GetUnitType(), this->mTemplate->mUnitSet)) {
        long theTechnology = inUnit->GetUnitClass()->mTechnology;
        EETechTreeNode* theNode = gWorld.mPlayers[inPlayer]->GetTechTree()->Get(theTechnology);
        if ((theNode == NULL || theNode->mData->mKind != 4) && this->mTemplate->AffectsPlayer(inPlayer, theOwner)) {
            EEAreaEffectTemplate* theTemplate = this->mTemplate;
            if (theTemplate->mRadius > 0.0f) {
                theLocation.SetXY(inUnit->mPhysicalModel.GetLocation().GetX(),
                                  inUnit->mPhysicalModel.GetLocation().GetY());
                if (theTemplate->IsWithin(theLocation, inPoint, this->GetScale(), inUnit))
                    theResult = true;
            } else {
                theResult = true;
            }
        }
    }
    return theResult;
}

// FUNCTION: EXE 0x67ab3a
// Reports the cloaking strength for modifier kind 3; other kinds are not modified
bool EEAECloaking::Modify(long inKind, long inValue, long& outValue)
{
    bool theResult = false;
    if (inKind == 3) {
        outValue = this->GetStrength();
        theResult = true;
    }
    return theResult;
}

// FUNCTION: EXE 0x67ab53
// Restores the unit's own cloakability, unless a Camouflage civ power keeps an idle or hidden unit cloaked;
// moves a unit that was a visible cloaked enemy off the mini map
void EEAECloaking::RemovedFrom(EEComplexUnit* inUnit)
{
    if (gCivPowersManager.HasPower(inUnit->GetPlayer(), kCivPowerCamouflage) &&
        gUnitSetManager.GetSet(gCivPowersManager.GetPowerRecord(kCivPowerCamouflage)->mUnitSet) != NULL &&
        gUnitSetManager.GetSet(gCivPowersManager.GetPowerRecord(kCivPowerCamouflage)->mUnitSet)->Contains(inUnit->mUnitClass->mUnitType)) {
        if (inUnit->GetCommandType() == kActionIdle && (inUnit->mCamouflageTimer & 0x7fffffff) == 0 || inUnit->mU32C > 0)
            return;
    }
    bool theVisible = EEIsCloakedEnemyOf(inUnit, gWorld.mPlayers[gWorld.mCurrentPlayer]);
    inUnit->SetCloakableChain(inUnit->mUnitClass->mIsCloakable);
    if (theVisible)
        gMiniMap->MoveUnit(inUnit, -10000, -10000, inUnit->mTileX, inUnit->mTileY);
}
