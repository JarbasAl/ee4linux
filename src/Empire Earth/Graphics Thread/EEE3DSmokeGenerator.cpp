// C:\EEX\Empire Earth\Graphics Thread\EEE3DSmokeGenerator.cpp (no __FILE__; name from the RTTI class and its key
// "EEE3DSmokeGenerator-Data"). 0xf9871..0xf9b6d: the key chain, three ctors .. Virtual4C. Unit flags: the exe's module set.
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
#include "ee/EEE3DSmokeGenerator.h"


// SYNTHETIC: EXE 0x4f9871
// SYNTHETIC: EXE 0x4f987b
// SYNTHETIC: EXE 0x4f988c
// SYNTHETIC: EXE 0x4f9898
const UString EEE3DSmokeGenerator::sData("EEE3DSmokeGeneratorData");

// FUNCTION: EXE 0x4f98b4
// Creates a smoke generator for a graphic effect at a fixed point, with its timers at zero
EEE3DSmokeGenerator::EEE3DSmokeGenerator(GE3DPoint& inLocation, EEGraphicEffect* inGraphicEffect, long inKind)
    : EEEffect3D(0, inGraphicEffect, inKind)
{
    GETransformation theTransformation;
    theTransformation.SetPosition(inLocation.GetX(), inLocation.GetY(), inLocation.GetZ());
    this->SetTransformation(theTransformation);
    this->mTime = 0.0f;
    this->mSinceEmit = 0.0f;
}

// SYNTHETIC: EXE 0x4f9929
// (the scalar deleting destructor, emitted with the vtable)

// FUNCTION: EXE 0x4f9945
// Creates a smoke generator that follows a unit, with its timers at zero
EEE3DSmokeGenerator::EEE3DSmokeGenerator(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect, long inKind)
    : EEEffect3D(inUnit, inGraphicEffect, inKind)
{
    this->mTime = 0.0f;
    this->mSinceEmit = 0.0f;
}

// FUNCTION: EXE 0x4f997f
// Creates an empty smoke generator of the given kind (for loading)
EEE3DSmokeGenerator::EEE3DSmokeGenerator(long inKind)
    : EEEffect3D(inKind)
{
}

// ICF-folded into EEE3DMultiEffect's 0xf43b8
EEE3DSmokeGenerator::~EEE3DSmokeGenerator()
{
}

long EEE3DSmokeGenerator::DrawPass2()
{
    return 0;
}

long EEE3DSmokeGenerator::DrawPass4()
{
    return 0;
}

long EEE3DSmokeGenerator::DrawPass5()
{
    return 0;
}

long EEE3DSmokeGenerator::DrawPass6()
{
    return 0;
}

// ICF-folded into 0x1020dc
long EEE3DSmokeGenerator::Queue(EEEffectQueue* inQueues)
{
    return 10;
}

// the empty Virtual2C: ICF-folded into 0xe123d
void EEE3DSmokeGenerator::PrepareQueue()
{
}

long EEE3DSmokeGenerator::SetUp()
{
    return 0;
}

// FUNCTION: EXE 0x4f9997
// Places the effect's first linked effect (the smoke puff) at a location
void EEE3DSmokeGenerator::EmitPuff(GE3DPoint& inLocation)
{
    gEffectManager->CreatePlacedEffect(inLocation, this->mGraphicEffect->mLinkedEffect1, NULL);
}

// FUNCTION: EXE 0x4f99b1
// Reads the generator's running time and time since the last puff; returns the first read error
long EEE3DSmokeGenerator::Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
{
    EEEffect3D::Load(inMajorVersion, inMinorVersion, inFile);
    long theResult = inFile.Read(this->mTime);
    if (theResult == kErrorOK)
        theResult = inFile.Read(this->mSinceEmit);
    return theResult;
}

// FUNCTION: EXE 0x4f99e6
// Writes the generator's running time and time since the last puff; returns the first write error
long EEE3DSmokeGenerator::Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
{
    EEEffect3D::Save(inMajorVersion, inMinorVersion, outFile);
    long theResult = outFile.Write(this->mTime);
    if (theResult == 0)
        theResult = outFile.Write(this->mSinceEmit);
    return theResult;
}

// FUNCTION: EXE 0x4f9a1b
// Emits at most one puff a tick, spread randomly by param 1 around its unit or point, once the emit interval passes;
// returns 7 when the unit dies or, without a unit, after the effect's duration
long EEE3DSmokeGenerator::Step()
{
    GE3DPoint theLocation;
    long theResult = 0;
    float theDelta = gPhysics->GetTickSeconds() * 1000.0f;
    float theTime = theDelta + this->mTime;
    this->mTime = theTime;
    this->mSinceEmit += theDelta;
    if (this->mUnit) {
        this->mUnit->mComplexUnit->mPhysicalModel.GetLocation().GetPosition(theLocation);
        if (this->mUnit->mComplexUnit->GetCommandType() == kActionDead)
            theResult = 7;
    } else {
        this->mTransformation.GetPosition(theLocation);
        if (theTime > this->mGraphicEffect->mDuration)
            theResult = 7;
    }
    if (!(this->mSinceEmit >= this->mGraphicEffect->mTime1))
        return theResult;
    theLocation.SetX(this->mGraphicEffect->GetParam1() * (0.5f - EEEffect::GetRandom()) + theLocation.mX);
    theLocation.SetY(EEEffect::GetRandomSpread(this->mGraphicEffect->GetParam1(), theLocation.GetY()));
    theLocation.SetZ(EEEffect::GetRandomSpread(this->mGraphicEffect->GetParam1(), theLocation.GetZ()));
    this->EmitPuff(theLocation);
    this->mSinceEmit -= this->mGraphicEffect->mTime1;
    return theResult;
}
