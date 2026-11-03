// C:\EEX\Low-Level Engine\Graphics Engine\GE3DLine.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// Opens with GE3DPlane's ctors and the shared Define(point, vector) body: this unit is the first in link order to
// emit them (see GE2DPoint.cpp for the same pattern with the basic types).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UWideString.h"
#include "lle/UString.h"
#include "lle/FSMemoryFile.h"
#include "lle/UMFastMath.h"
#include "lle/GE2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPoint.h"
#include "lle/GE3DLine.h"
#include "lle/GE3DPlane.h"

#define IS_ZERO(x) ((x) >= -0.0001f && (x) <= 0.0001f)

static const float kTolerance = 0.05f;   // 0x1009780c (unit-local; negated with fchs, so not a literal)

// FUNCTION: LLE 0x1000cb72
// Creates a plane through the origin facing +z
GE3DPlane::GE3DPlane()
{
    this->mNormal.SetZ(1.0f);
}

// FUNCTION: LLE 0x1000cb97
// Creates the plane through three points
GE3DPlane::GE3DPlane(const GE3DPoint& inPoint1, const GE3DPoint& inPoint2, const GE3DPoint& inPoint3)
{
    this->Define(inPoint1, inPoint2, inPoint3);
}

// FUNCTION: LLE 0x1000cbce
// GE3DLine::Define(const GE3DPoint&, const GEVector&) is header-inline (GE3DLine.h); its exported copy is ICF-folded
// with GE3DPlane::Define(point, vector) here.

// FUNCTION: LLE 0x1000cbfa
// Tests whether the line passes within inTolerance of a point; outT gets the nearer of the two crossing parameters
bool GE3DLine::Intersects(const GE3DPoint& inPoint, float inTolerance, float& outT) const
{
    GEVector theDelta(this->mStart.GetX() - inPoint.GetX(), this->mStart.GetY() - inPoint.GetY(), this->mStart.GetZ() - inPoint.GetZ());
    float theA = this->mDirection.GetMagnitudeSquared();
    float theB = 2.0f * (this->mDirection.GetX() * theDelta.GetX() + this->mDirection.GetY() * theDelta.GetY() + this->mDirection.GetZ() * theDelta.GetZ());
    float theC = inPoint.GetX() * inPoint.GetX() + inPoint.GetY() * inPoint.GetY() + inPoint.GetZ() * inPoint.GetZ() +
                 this->mStart.GetX() * this->mStart.GetX() + this->mStart.GetY() * this->mStart.GetY() + this->mStart.GetZ() * this->mStart.GetZ() -
                 2.0f * (inPoint.GetX() * this->mStart.GetX() + inPoint.GetY() * this->mStart.GetY() +
                         inPoint.GetZ() * this->mStart.GetZ()) -
                 inTolerance * inTolerance;
    float theDiscriminant = theB * theB - 4.0f * theA * theC;
    if (theDiscriminant < 0.0f)
        return false;
    if (theDiscriminant > 0.0f) {
        float theRoot = sqrt(theDiscriminant);
        float theInverse = 1.0f / (2.0f * theA);
        float theT1 = (theRoot - theB) * theInverse;
        float theT2 = (-theB - theRoot) * theInverse;
        if ((theT1 < 0.0f ? -theT1 : theT1) < (theT2 < 0.0f ? -theT2 : theT2))
            outT = theT1;
        else
            outT = theT2;
    } else
        outT = -theB / (2.0f * theA);
    return true;
}

// FUNCTION: LLE 0x1000cd82
// Tests whether two 3D lines meet (within 0.05 in the third axis); outT/outLineT get the parameters on each line
bool GE3DLine::Intersects(const GE3DLine& inLine, float& outT, float& outLineT) const
{
    GEVector theDelta(this->mStart.GetX() - inLine.mStart.GetX(), this->mStart.GetY() - inLine.mStart.GetY(),
                      this->mStart.GetZ() - inLine.mStart.GetZ());
    bool theResult = false;
    float theInverse;
    float theT;
    float theLineT;
    float theDifference;
    float theXY = inLine.mDirection.GetY() * this->mDirection.GetX() - inLine.mDirection.GetX() * this->mDirection.GetY();
    float theXZ = this->mDirection.GetX() * inLine.mDirection.GetZ() - inLine.mDirection.GetX() * this->mDirection.GetZ();
    float theYZ = this->mDirection.GetY() * inLine.mDirection.GetZ() - inLine.mDirection.GetY() * this->mDirection.GetZ();
    if (IS_ZERO(theXY) || (IS_ZERO(theDelta.GetX()) && IS_ZERO(theDelta.GetY()))) {
        if (IS_ZERO(theYZ) || (IS_ZERO(theDelta.GetY()) && IS_ZERO(theDelta.GetZ()))) {
            if (!IS_ZERO(theXZ) && !(IS_ZERO(theDelta.GetX()) && IS_ZERO(theDelta.GetZ()))) {
                theInverse = 1.0f / theXZ;
                theT = (inLine.mDirection.GetX() * theDelta.GetZ() - inLine.mDirection.GetZ() * theDelta.GetX()) * theInverse;
                theLineT = (this->mDirection.GetX() * theDelta.GetZ() - this->mDirection.GetZ() * theDelta.GetX()) * theInverse;
                theDifference = this->mDirection.GetY() * theT + this->mStart.GetY() -
                                    (inLine.mDirection.GetY() * theLineT + inLine.mStart.GetY());
                if (theDifference > -kTolerance && theDifference < kTolerance)
                    theResult = true;
                else
                    theResult = false;
                outT = theT;
                outLineT = theLineT;
            }
        } else {
            theInverse = 1.0f / theYZ;
            theT = (inLine.mDirection.GetY() * theDelta.GetZ() - theDelta.GetY() * inLine.mDirection.GetZ()) * theInverse;
            theLineT = (this->mDirection.GetY() * theDelta.GetZ() - theDelta.GetY() * this->mDirection.GetZ()) * theInverse;
            theDifference = this->mDirection.GetX() * theT + this->mStart.GetX() -
                                (inLine.mDirection.GetX() * theLineT + inLine.mStart.GetX());
            if (theDifference > -kTolerance && theDifference < kTolerance)
                theResult = true;
            else
                theResult = false;
            outT = theT;
            outLineT = theLineT;
        }
    } else {
        theInverse = 1.0f / theXY;
        theT = (inLine.mDirection.GetX() * theDelta.GetY() - inLine.mDirection.GetY() * theDelta.GetX()) * theInverse;
        theLineT = (this->mDirection.GetX() * theDelta.GetY() - this->mDirection.GetY() * theDelta.GetX()) * theInverse;
        theDifference = this->mDirection.GetZ() * theT + this->mStart.GetZ() -
                            (inLine.mDirection.GetZ() * theLineT + inLine.mStart.GetZ());
        if (theDifference > -kTolerance && theDifference < kTolerance)
            theResult = true;
        else
            theResult = false;
        outT = theT;
        outLineT = theLineT;
    }
    return theResult;
}

// FUNCTION: LLE 0x1000d000
// Intersects two lines in the xy plane; false when they are parallel, else the parameters on each line
bool GE3DLine::IntersectsIn2D(const GE3DLine& inLine, float& outT, float& outLineT) const
{
    GEVector theDelta(this->mStart.GetX() - inLine.mStart.GetX(), this->mStart.GetY() - inLine.mStart.GetY(), 0.0f);
    float theDenominator = inLine.mDirection.GetY() * this->mDirection.GetX() - inLine.mDirection.GetX() * this->mDirection.GetY();
    bool theResult;
    if (theDenominator != 0.0f) {
        theResult = true;
        float theInverse = 1.0f / theDenominator;
        outT = (inLine.mDirection.GetX() * theDelta.GetY() - inLine.mDirection.GetY() * theDelta.GetX()) * theInverse;
        outLineT = (this->mDirection.GetX() * theDelta.GetY() - this->mDirection.GetY() * theDelta.GetX()) * theInverse;
    } else
        theResult = false;
    return theResult;
}

// FUNCTION: LLE 0x1000d074
// Intersects the line with a plane; false when parallel, else outT gets the parameter of the crossing point
bool GE3DLine::Intersects(const GE3DPlane& inPlane, float& outT) const
{
    const GE3DPoint& thePoint = inPlane.GetPoint();
    GEVector theDelta(thePoint.GetX() - this->mStart.GetX(), thePoint.GetY() - this->mStart.GetY(), thePoint.GetZ() - this->mStart.GetZ());
    float theDenominator = inPlane.GetNormal().DotProduct(this->mDirection);
    if (theDenominator != 0.0f) {
        outT = theDelta.DotProduct(inPlane.GetNormal()) / theDenominator;
        return true;
    }
    return false;
}

// FUNCTION: LLE 0x1000d0e3
// Finds the parameter where the line reaches a given x; false when the line runs parallel to x
bool GE3DLine::SolveForX(float inX, float& outT) const
{
    float theDirection = this->mDirection.GetX();
    if (theDirection != 0.0f) {
        outT = (inX - this->mStart.GetX()) / theDirection;
        return true;
    }
    return false;
}

// FUNCTION: LLE 0x1000d10c
// Finds the parameter and the point where the line reaches a given x; false when it runs parallel to x
bool GE3DLine::SolveForX(float inX, float& outT, GE3DPoint& outPoint) const
{
    float theDirection = this->mDirection.GetX();
    if (theDirection != 0.0f) {
        outT = (inX - this->mStart.GetX()) / theDirection;
        outPoint.mX = inX;
        outPoint.mY = this->mDirection.GetY() * outT + this->mStart.GetY();
        outPoint.mZ = this->mDirection.GetZ() * outT + this->mStart.GetZ();
        return true;
    }
    return false;
}

// FUNCTION: LLE 0x1000d159
// Finds the parameter where the line reaches a given y; false when the line runs parallel to y
bool GE3DLine::SolveForY(float inY, float& outT) const
{
    float theDirection = this->mDirection.GetY();
    if (theDirection != 0.0f) {
        outT = (inY - this->mStart.GetY()) / theDirection;
        return true;
    }
    return false;
}

// FUNCTION: LLE 0x1000d183
// Finds the parameter and the point where the line reaches a given y; false when it runs parallel to y
bool GE3DLine::SolveForY(float inY, float& outT, GE3DPoint& outPoint) const
{
    float theDirection = this->mDirection.GetY();
    if (theDirection != 0.0f) {
        outT = (inY - this->mStart.GetY()) / theDirection;
        outPoint.mX = this->mDirection.GetX() * outT + this->mStart.GetX();
        outPoint.mY = inY;
        outPoint.mZ = this->mDirection.GetZ() * outT + this->mStart.GetZ();
        return true;
    }
    return false;
}

// FUNCTION: LLE 0x1000d1d4
// Finds the parameter where the line reaches a given z; false when the line runs parallel to z
bool GE3DLine::SolveForZ(float inZ, float& outT) const
{
    float theDirection = this->mDirection.GetZ();
    if (theDirection != 0.0f) {
        outT = (inZ - this->mStart.GetZ()) / theDirection;
        return true;
    }
    return false;
}

// FUNCTION: LLE 0x1000d1fe
// Finds the parameter and the point where the line reaches a given z; false when it runs parallel to z
bool GE3DLine::SolveForZ(float inZ, float& outT, GE3DPoint& outPoint) const
{
    float theDirection = this->mDirection.GetZ();
    if (theDirection != 0.0f) {
        outT = (inZ - this->mStart.GetZ()) / theDirection;
        outPoint.mX = this->mDirection.GetX() * outT + this->mStart.GetX();
        outPoint.mY = this->mDirection.GetY() * outT + this->mStart.GetY();
        outPoint.mZ = inZ;
        return true;
    }
    return false;
}

// Compiler-generated for this unit (names in analysis/lle/names.txt):

// ---- Exported members with no address of their own (ICF-folded into GE3DPlane::Load/Save 0xd52f/0xd54f)
long GE3DLine::Load(FSFile& inFile)
{
    long theResult = this->mStart.Load(inFile);
    if (theResult == kErrorOK)
        theResult = this->mDirection.Load(inFile);
    return theResult;
}

long GE3DLine::Save(FSFile& inFile) const
{
    long theResult = this->mStart.Save(inFile);
    if (theResult == kErrorOK)
        theResult = this->mDirection.Save(inFile);
    return theResult;
}
