// C:\EEX\Empire Earth\Civilization Builder\EEAEMoraleFromMilitaryHeroes.cpp (name from RTTI
// EEAEMoraleFromMilitaryHeroes; folder inferred: no __FILE__). 0x27bf0a..0x27bf4b: the morale from military heroes
// area effect (vtable 0x44bdc8): its ctor and Virtual4C; EEAreaEffect.cpp starts at 0x27bf4b. Unit flags: the exe's
// module set.
#include <windows.h>
#include <vector>
#include <functional>
#include <algorithm>

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
#include "lle/U2DSparseArray.h"
#include "lle/U2DPolygon.h"
#include "lle/USingleton.h"

#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEMap.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEGameVariant.h"
#include "ee/RTSRegionTable.h"
#include "ee/EEUnitCollector.h"
#include "ee/EEAreaEffect.h"
#include "ee/EEAreaEffectTemplate.h"
#include "ee/EEAEMorale.h"

// FUNCTION: EXE 0x67bf0a
// Creates a morale-from-military-heroes effect from its template
EEAEMoraleFromMilitaryHeroes::EEAEMoraleFromMilitaryHeroes(EEAreaEffectTemplate* inTemplate)
    : EEAEMorale(inTemplate)
{
}

// FUNCTION: EXE 0x67bf22
// Returns the hero's leadership as the morale level while the hero is alive, else 0
long EEAEMoraleFromMilitaryHeroes::GetLevel()
{
    if (this->mUnit->mHitPoints > 0)
        return UMFloatToLong(this->mUnit->mStats->mLeadership);
    return 0;
}
