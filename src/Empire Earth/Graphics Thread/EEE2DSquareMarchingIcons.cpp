// C:\EEX\Empire Earth\Graphics Thread\EEE2DSquareMarchingIcons.cpp (no __FILE__; name from the RTTI class). Marching
// icons on a square: 0xe72fa..0xe753d (EEE2DSmokePlume.cpp ends at 0xe72fa, EEE2DVelocityParticle.cpp's key chains
// start at 0xe753d). Unit flags: the exe's module set.
#include <windows.h>
#include <math.h>

#include "lle/LowLevelEngine.h"
#include "lle/GE2DPoint.h"
#include "lle/GE3DPoint.h"
#include "lle/GEVector.h"

#include "ee/EEMedia.h"
#include "ee/EEGraphicEffect.h"
#include "ee/EEEffect.h"
#include "ee/EEEffect2D.h"
#include "ee/EEE2DMarchingIcons.h"
#include "ee/EEE2DSquareMarchingIcons.h"

// FUNCTION: EXE 0x4e72fa
// Creates an empty square of marching icons for loading from a saved game
EEE2DSquareMarchingIcons::EEE2DSquareMarchingIcons()
    : EEE2DMarchingIcons(0xf)
{
}

// SYNTHETIC: EXE 0x4e730e
// (the scalar deleting destructor, emitted with the vtable)

// FUNCTION: EXE 0x4e732a
// Creates a square of marching icons around a unit, half side the effect's parameter 1 (radius to the corners)
EEE2DSquareMarchingIcons::EEE2DSquareMarchingIcons(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect)
    : EEE2DMarchingIcons(inUnit, inGraphicEffect, 0xf)
{
    float theHalfSide = inGraphicEffect->mParam1;
    this->SetBoundingRadius(sqrt(theHalfSide * theHalfSide + theHalfSide * theHalfSide));
}

// FUNCTION: EXE 0x4e7365
// Destroys the square of icons (the base class does the cleanup)
EEE2DSquareMarchingIcons::~EEE2DSquareMarchingIcons()
{
}

// FUNCTION: EXE 0x4e7370
// Adds the icon offset for an angle, mapped onto the square's perimeter (one side per quarter turn), if shown
void EEE2DSquareMarchingIcons::AddIcon(float inAngle, std::vector<GE3DPoint>& outPoints)
{
    GE3DPoint thePoint;
    float theRadius = this->mGraphicEffect->mParam1;
    long theSign;
    if (inAngle >= 0.0f)
        theSign = 1;
    else
        theSign = -1;
    while (theSign * inAngle > 6.2831855f)
        inAngle -= theSign * 6.2831855f;
    float theFraction = (float)fabs(inAngle) * 0.15915494f;
    if (theFraction > 0.875f || theFraction <= 0.125f) {
        thePoint.SetX(theRadius);
        theFraction += 0.125f;
        if (theFraction > 1.0f)
            theFraction -= 1.0f;
        thePoint.SetY((theFraction * 4.0f - 0.5f) * theRadius * 2.0f);
    } else if (theFraction <= 0.375f) {
        thePoint.SetY(theRadius);
        thePoint.SetX(-(((theFraction - 0.125f) * 4.0f - 0.5f) * theRadius * 2.0f));
    } else if (theFraction <= 0.625f) {
        thePoint.SetX(-theRadius);
        thePoint.SetY(-(((theFraction - 0.375f) * 4.0f - 0.5f) * theRadius * 2.0f));
    } else {
        thePoint.SetY(-theRadius);
        thePoint.SetX(((theFraction - 0.625f) * 4.0f - 0.5f) * theRadius * 2.0f);
    }
    if (this->IsIconShown(thePoint))
        outPoints.push_back(thePoint);
}

// FUNCTION: EXE 0x4e74ed
// Lists the shown icon offsets spaced evenly around the square, starting at the given angle
void EEE2DSquareMarchingIcons::ComputeOffsets(float inAngle, std::vector<GE3DPoint>& outPoints)
{
    long theCount = this->GetIconCount();
    if (theCount <= 0)
        return;
    float theStep = 6.2831855f / theCount;
    for (long i = 0; i < theCount; i++)
        this->AddIcon(inAngle + i * theStep, outPoints);
}
