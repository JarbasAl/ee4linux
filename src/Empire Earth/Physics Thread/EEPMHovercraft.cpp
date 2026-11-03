// C:\EEX\Empire Earth\Physics Thread\EEPMHovercraft.cpp (file name inferred: no __FILE__; RTTI EEPMHovercraft; one unit per
// physics-model class, alphabetical link order). Range 0x15a0cf..0x15a0db; split out of a single EEPMLand.cpp
// (each unit ends with its own inline-COMDAT cluster).
#include <windows.h>
#include <vector>
#include <math.h>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/U2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GETerrainMesh.h"
#include "lle/FSFileSpec.h"

#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEMap.h"
#include "ee/EEPlayer.h"
#include "ee/EEUnit.h"
#include "ee/EEPhysics.h"
#include "ee/EEDatabaseTables.h"
#include "ee/EEPhysicsModel.h"

// FUNCTION: EXE 0x55a0cf
// Resolves collisions as a land unit
void EEPMHovercraft::Collide(EEComplexUnit* inUnit)
{
    EEPMLand::Collide(inUnit);
}
