// C:\EEX\Empire Earth\Graphics Thread\EEE2DVelocityParticle.cpp (no __FILE__; name from the RTTI class and its key
// "EEE2DVelocityParticle-Data"). 0xe753d..0xe773c: the key chain, the ctor .. Step; EEE2DVortex.cpp follows.
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
#include "ee/EEGraphicEffect.h"
#include "ee/EEGraphicObject.h"
#include "ee/EERenderObject.h"
#include "ee/EEEffect.h"
#include "ee/EEEffect2D.h"
#include "ee/EEE2DSmokePlume.h"
#include "ee/EEE2DVelocityParticle.h"

// SYNTHETIC: EXE 0x4e753d
// SYNTHETIC: EXE 0x4e7547
// SYNTHETIC: EXE 0x4e7558
// SYNTHETIC: EXE 0x4e7564
const UString EEE2DVelocityParticle::sData("EEE2DVelocityParticle-Data");

// FUNCTION: EXE 0x4e7580
// Creates a moving smoke puff (kind 12) at the line's start with the line's direction as its velocity
EEE2DVelocityParticle::EEE2DVelocityParticle(const GE3DLine& inLine, EEGraphicEffect* inGraphicEffect)
    : EEE2DSmokePlume(inLine.GetStart(), inGraphicEffect, 0xc)
{
    this->mVelocity = inLine.GetDirection();
}

// SYNTHETIC: EXE 0x4e75d4
// (the scalar deleting destructor, emitted with the vtable)

// FUNCTION: EXE 0x4e75f6
// Reads the smoke puff state, then the velocity; returns the velocity's read result (the puff's is ignored)
long EEE2DVelocityParticle::Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
{
    EEE2DSmokePlume::Load(inMajorVersion, inMinorVersion, inFile);
    return this->mVelocity.Load(inFile);
}

// FUNCTION: EXE 0x4e761e
// Writes the smoke puff state, then the velocity; returns the velocity's write result (the puff's is ignored)
long EEE2DVelocityParticle::Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
{
    EEE2DSmokePlume::Save(inMajorVersion, inMinorVersion, outFile);
    return this->mVelocity.Save(outFile);
}

// NEAR: x87 operand order (the step/damping factors stay on the FP stack in the original)
// FUNCTION: EXE 0x4e7646
// Ages the puff and moves it by its velocity (damped to zero by integer parameter 1 ms), the wind and a fading
// rise; returns 7 (remove) once its life is over
long EEE2DVelocityParticle::Step()
{
    GEVector theWind = gWorld.mWindDirection;
    float theElapsed = gPhysics->GetTickTime();
    float theStep = theElapsed * 0.125f;
    long theResult = 0;
    float theDuration = this->mGraphicEffect->mDuration;
    this->mAge += theElapsed / theDuration;
    if (this->mAge <= 1.0f) {
        float theDamping = 1.0f - this->mAge * theDuration / this->mGraphicEffect->mIntParam1;
        if (theDamping < 0.0f)
            theDamping = 0.0f;
        this->mStart.SetX(this->mStart.GetX() + (this->mVelocity.GetX() * theDamping + theWind.GetX() * theStep));
        this->mStart.SetY(this->mStart.GetY() + (this->mVelocity.GetY() * theDamping + theWind.GetY() * theStep));
        this->mStart.SetZ(this->mStart.GetZ() + ((1.0f - this->mAge) * this->mGraphicEffect->GetParam3() * theElapsed
                                     + this->mVelocity.GetZ() * theDamping + theWind.GetZ() * theStep));
        this->SetPosition(this->mStart);
    } else {
        theResult = 7;
    }
    return theResult;
}
