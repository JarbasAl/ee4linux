// C:\EEX\Empire Earth\Civilization Builder\EEAEEconomy.cpp (file name inferred: no __FILE__; from RTTI EEAEEconomy).
// 0x27ac2c..0x27ace1: EEAEEconomy::Modify, then EEAEEconomyFromUnits::GetRate (0x27ac9e). EEAEHealing.cpp starts at
// 0x27ace1. Unit flags: the exe's module set.
#include <windows.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/GEColor.h"

#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GE3DLine.h"
#include "lle/GEViewport.h"
#include "lle/GETransformation.h"
#include "lle/GEPhysicalModel.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFile.h"
#include "lle/U2DPoint.h"

#include "ee/EEUnit.h"
#include "ee/EEAreaEffect.h"
#include "ee/EEAreaEffectTemplate.h"
#include "ee/EEAEEconomy.h"

// FUNCTION: EXE 0x67ac2c
// Scales a gather amount (kind 2) by the effect's rate clamped to 0..10; other kinds are not modified
bool EEAEEconomy::Modify(long inKind, long inValue, long& outValue)
{
    bool theResult = false;
    if (inKind == 2) {
        float theRate = 0.0f > (this->GetRate() > 10.0f ? 10.0f : this->GetRate()) ? 0.0f :
                        (this->GetRate() > 10.0f ? 10.0f : this->GetRate());
        outValue = UMFloatToLong(theRate * inValue);
        theResult = true;
    }
    return theResult;
}

// FUNCTION: EXE 0x67ac9e
// Returns the unit class's garrison bonus clamped to 0..10 as the economy rate
float EEAEEconomyFromUnits::GetRate()
{
    EEUnitClass* theClass = this->mUnit->mUnitClass;
    return min(max(0.0f, theClass->mGarrisonBonus), 10.0f);

}
