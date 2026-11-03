// C:\EEX\Empire Earth\Graphics Thread\EEE3DMultiEffect.cpp (no __FILE__; name from the RTTI class and its key
// "EEE3DMultiEffect-Data"). 0xf4247..0xf43c7: three ctors .. Virtual4C (no key chain). Unit flags: the exe's module set.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GE3DPlane.h"
#include "lle/GE3DLine.h"
#include "lle/GEOrientation.h"
#include "lle/GETransformation.h"
#include "lle/GETerrainMesh.h"
#include "lle/GESurface.h"
#include "lle/GETextureManager.h"
#include "lle/GETexture.h"
#include "lle/GEScreen.h"
#include "lle/GEViewport.h"
#include "lle/GEVertexArray.h"
#include "lle/GERasterizer.h"
#include "lle/GEMaterial.h"
#include "lle/GEModel.h"
#include "lle/SESoundManager.h"

#include "ee/EmpireEarth.h"
#include "ee/EEMap.h"
#include "ee/EEUnit.h"
#include "ee/EEPlayer.h"
#include "ee/RTSWorld.h"
#include "ee/EEWorld.h"
#include "ee/EEPhysics.h"
#include "ee/RTSGraphics.h"
#include "ee/EEGraphics.h"
#include "ee/EEMedia.h"
#include "ee/EEGameSound.h"
#include "ee/EEGraphicEffect.h"
#include "ee/EEGraphicObject.h"
#include "ee/EERenderObject.h"
#include "ee/EEEffect.h"
#include "ee/EEEffect2D.h"
#include "ee/EEEffect3D.h"
#include "ee/EEEffectManager.h"
#include "ee/EEE3DMultiEffect.h"

// FUNCTION: EXE 0x4f4247
// Creates the effect's two (or three) linked effects at a point; the multi-effect itself does nothing more
EEE3DMultiEffect::EEE3DMultiEffect(GE3DPoint& inLocation, EEGraphicEffect* inGraphicEffect, long inKind)
    : EEEffect3D(0, inGraphicEffect, inKind)
{
    gEffectManager->CreatePlacedEffect(inLocation, this->mGraphicEffect->mLinkedEffect1, NULL);
    gEffectManager->CreatePlacedEffect(inLocation, this->mGraphicEffect->mLinkedEffect2, NULL);
    if (this->mGraphicEffect->mLinkedEffect3)
        gEffectManager->CreatePlacedEffect(inLocation, this->mGraphicEffect->mLinkedEffect3, NULL);
}

// FUNCTION: EXE 0x4f42bd
// Never queues anything: returns 10
long EEE3DMultiEffect::Queue(EEEffectQueue* inQueues)
{
    return 10;
}

// FUNCTION: EXE 0x4f42c3
// Creates the effect's two (or three) linked effects along a line
EEE3DMultiEffect::EEE3DMultiEffect(GE3DLine& inLine, EEGraphicEffect* inGraphicEffect, long inKind)
    : EEEffect3D(0, inGraphicEffect, inKind)
{
    gEffectManager->CreateLineEffect(inLine, this->mGraphicEffect->mLinkedEffect1, NULL);
    gEffectManager->CreateLineEffect(inLine, this->mGraphicEffect->mLinkedEffect2, NULL);
    if (this->mGraphicEffect->mLinkedEffect3)
        gEffectManager->CreateLineEffect(inLine, this->mGraphicEffect->mLinkedEffect3, NULL);
}

// FUNCTION: EXE 0x4f4339
// Creates the effect's two (or three) linked effects on a unit
EEE3DMultiEffect::EEE3DMultiEffect(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect, long inKind)
    : EEEffect3D(0, inGraphicEffect, inKind)
{
    gEffectManager->CreateUnitEffect(inUnit, this->mGraphicEffect->mLinkedEffect1, NULL, 0, 0, 0);
    gEffectManager->CreateUnitEffect(inUnit, this->mGraphicEffect->mLinkedEffect2, NULL, 0, 0, 0);
    if (this->mGraphicEffect->mLinkedEffect3)
        gEffectManager->CreateUnitEffect(inUnit, this->mGraphicEffect->mLinkedEffect3, NULL, 0, 0, 0);
}

// FUNCTION: EXE 0x4f43b8
// Destroys the multi-effect (nothing to release)
EEE3DMultiEffect::~EEE3DMultiEffect()
{
}

// the empty PrepareQueue: ICF-folded into 0x1720c6
void EEE3DMultiEffect::PrepareQueue()
{
}

long EEE3DMultiEffect::SetUp()
{
    return 0;
}

// FUNCTION: EXE 0x4f43c3
// Returns 7, so the multi-effect is removed on its first step
long EEE3DMultiEffect::Step()
{
    return 7;
}
