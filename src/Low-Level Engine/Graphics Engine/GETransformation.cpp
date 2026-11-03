// C:\EEX\Low-Level Engine\Graphics Engine\GETransformation.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x100349a3..0x10035941: sTransformationStringID's init chain, then the out-of-line GETransformation members. The
// small members (0x1000d5a1..0x1000d9d3) are emitted by GE3DPoint.cpp (see lle/GETransformation.h).
//
// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x100349a3
// SYNTHETIC: LLE 0x100349ad
// SYNTHETIC: LLE 0x100349bd
// SYNTHETIC: LLE 0x100349c9
#include <windows.h>
#include <math.h>
#include <string.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/GE2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPoint.h"
#include "lle/GE3DLine.h"
#include "lle/GE3DPlane.h"
#include "lle/GEOrientation.h"
#include "lle/GETransformation.h"
#include "lle/FSFile.h"

const UString GETransformation::sTransformationStringID("GETransformation-TransformationData");

// FUNCTION: LLE 0x100349e4
// Creates an identity transformation
GETransformation::GETransformation()
{
    this->mMatrix[0][0] = 1.0f;
    this->mMatrix[0][1] = 0.0f;
    this->mMatrix[0][2] = 0.0f;
    this->mMatrix[0][3] = 0.0f;
    this->mMatrix[1][0] = 0.0f;
    this->mMatrix[1][1] = 1.0f;
    this->mMatrix[1][2] = 0.0f;
    this->mMatrix[1][3] = 0.0f;
    this->mMatrix[2][0] = 0.0f;
    this->mMatrix[2][1] = 0.0f;
    this->mMatrix[2][2] = 1.0f;
    this->mMatrix[2][3] = 0.0f;
    this->mMatrix[3][0] = 0.0f;
    this->mMatrix[3][1] = 0.0f;
    this->mMatrix[3][2] = 0.0f;
    this->mMatrix[3][3] = 1.0f;
}

GETransformation::GETransformation(const float inMatrix[4][4])
{
    this->mMatrix[0][0] = inMatrix[0][0];
    this->mMatrix[0][1] = inMatrix[0][1];
    this->mMatrix[0][2] = inMatrix[0][2];
    this->mMatrix[0][3] = inMatrix[0][3];
    this->mMatrix[1][0] = inMatrix[1][0];
    this->mMatrix[1][1] = inMatrix[1][1];
    this->mMatrix[1][2] = inMatrix[1][2];
    this->mMatrix[1][3] = inMatrix[1][3];
    this->mMatrix[2][0] = inMatrix[2][0];
    this->mMatrix[2][1] = inMatrix[2][1];
    this->mMatrix[2][2] = inMatrix[2][2];
    this->mMatrix[2][3] = inMatrix[2][3];
    this->mMatrix[3][0] = inMatrix[3][0];
    this->mMatrix[3][1] = inMatrix[3][1];
    this->mMatrix[3][2] = inMatrix[3][2];
    this->mMatrix[3][3] = inMatrix[3][3];
}

// FUNCTION: LLE 0x10034a36
// Creates a copy of a transformation
GETransformation::GETransformation(const GETransformation& inTransformation)
{
    this->mMatrix[0][0] = inTransformation.mMatrix[0][0];
    this->mMatrix[0][1] = inTransformation.mMatrix[0][1];
    this->mMatrix[0][2] = inTransformation.mMatrix[0][2];
    this->mMatrix[0][3] = inTransformation.mMatrix[0][3];
    this->mMatrix[1][0] = inTransformation.mMatrix[1][0];
    this->mMatrix[1][1] = inTransformation.mMatrix[1][1];
    this->mMatrix[1][2] = inTransformation.mMatrix[1][2];
    this->mMatrix[1][3] = inTransformation.mMatrix[1][3];
    this->mMatrix[2][0] = inTransformation.mMatrix[2][0];
    this->mMatrix[2][1] = inTransformation.mMatrix[2][1];
    this->mMatrix[2][2] = inTransformation.mMatrix[2][2];
    this->mMatrix[2][3] = inTransformation.mMatrix[2][3];
    this->mMatrix[3][0] = inTransformation.mMatrix[3][0];
    this->mMatrix[3][1] = inTransformation.mMatrix[3][1];
    this->mMatrix[3][2] = inTransformation.mMatrix[3][2];
    this->mMatrix[3][3] = inTransformation.mMatrix[3][3];
}

GETransformation& GETransformation::operator=(const GETransformation& inTransformation)
{
    this->mMatrix[0][0] = inTransformation.mMatrix[0][0];
    this->mMatrix[0][1] = inTransformation.mMatrix[0][1];
    this->mMatrix[0][2] = inTransformation.mMatrix[0][2];
    this->mMatrix[0][3] = inTransformation.mMatrix[0][3];
    this->mMatrix[1][0] = inTransformation.mMatrix[1][0];
    this->mMatrix[1][1] = inTransformation.mMatrix[1][1];
    this->mMatrix[1][2] = inTransformation.mMatrix[1][2];
    this->mMatrix[1][3] = inTransformation.mMatrix[1][3];
    this->mMatrix[2][0] = inTransformation.mMatrix[2][0];
    this->mMatrix[2][1] = inTransformation.mMatrix[2][1];
    this->mMatrix[2][2] = inTransformation.mMatrix[2][2];
    this->mMatrix[2][3] = inTransformation.mMatrix[2][3];
    this->mMatrix[3][0] = inTransformation.mMatrix[3][0];
    this->mMatrix[3][1] = inTransformation.mMatrix[3][1];
    this->mMatrix[3][2] = inTransformation.mMatrix[3][2];
    this->mMatrix[3][3] = inTransformation.mMatrix[3][3];
    return *this;
}

// FUNCTION: LLE 0x10034a9d
// Returns whether all sixteen matrix elements equal the other transformation's
bool GETransformation::operator==(const GETransformation& inTransformation)
{
    return this->GetElement(0, 0) == inTransformation.GetElement(0, 0) && this->GetElement(0, 1) == inTransformation.GetElement(0, 1) &&
           this->GetElement(0, 2) == inTransformation.GetElement(0, 2) && this->GetElement(0, 3) == inTransformation.GetElement(0, 3) &&
           this->GetElement(1, 0) == inTransformation.GetElement(1, 0) && this->GetElement(1, 1) == inTransformation.GetElement(1, 1) &&
           this->GetElement(1, 2) == inTransformation.GetElement(1, 2) && this->GetElement(1, 3) == inTransformation.GetElement(1, 3) &&
           this->GetElement(2, 0) == inTransformation.GetElement(2, 0) && this->GetElement(2, 1) == inTransformation.GetElement(2, 1) &&
           this->GetElement(2, 2) == inTransformation.GetElement(2, 2) && this->GetElement(2, 3) == inTransformation.GetElement(2, 3) &&
           this->GetElement(3, 0) == inTransformation.GetElement(3, 0) && this->GetElement(3, 1) == inTransformation.GetElement(3, 1) &&
           this->GetElement(3, 2) == inTransformation.GetElement(3, 2) && this->GetElement(3, 3) == inTransformation.GetElement(3, 3);
}

// FUNCTION: LLE 0x10034b91
// Returns whether the transformation's position equals the point
bool GETransformation::operator==(const GE3DPoint& inPoint) const
{
    return this->GetX() == inPoint.GetX() && this->GetY() == inPoint.GetY() && this->GetZ() == inPoint.GetZ();
}

// NEAR: one store of b23 is scheduled one instruction later in the original; tried declaration order, GetElement
// FUNCTION: LLE 0x10034bc5
// Composes this transformation with another (this x other) into outTransformation;
// the bottom row is copied from the other
void GETransformation::Apply(const GETransformation& inTransformation, GETransformation& outTransformation) const
{
    float a00 = this->mMatrix[0][0], a01 = this->mMatrix[0][1], a02 = this->mMatrix[0][2], a03 = this->mMatrix[0][3];
    float a10 = this->mMatrix[1][0], a11 = this->mMatrix[1][1], a12 = this->mMatrix[1][2], a13 = this->mMatrix[1][3];
    float a20 = this->mMatrix[2][0], a21 = this->mMatrix[2][1], a22 = this->mMatrix[2][2], a23 = this->mMatrix[2][3];
    float b00 = inTransformation.mMatrix[0][0], b01 = inTransformation.mMatrix[0][1];
    float b02 = inTransformation.mMatrix[0][2], b03 = inTransformation.mMatrix[0][3];
    float b10 = inTransformation.mMatrix[1][0], b11 = inTransformation.mMatrix[1][1];
    float b12 = inTransformation.mMatrix[1][2], b13 = inTransformation.mMatrix[1][3];
    float b20 = inTransformation.mMatrix[2][0], b21 = inTransformation.mMatrix[2][1];
    float b22 = inTransformation.mMatrix[2][2], b23 = inTransformation.mMatrix[2][3];
    outTransformation.mMatrix[0][0] = a00 * b00 + a01 * b10 + a02 * b20;
    outTransformation.mMatrix[0][1] = a00 * b01 + a01 * b11 + a02 * b21;
    outTransformation.mMatrix[0][2] = a00 * b02 + a01 * b12 + a02 * b22;
    outTransformation.mMatrix[0][3] = a00 * b03 + a01 * b13 + a02 * b23 + a03;
    outTransformation.mMatrix[1][0] = a10 * b00 + a11 * b10 + a12 * b20;
    outTransformation.mMatrix[1][1] = a10 * b01 + a11 * b11 + a12 * b21;
    outTransformation.mMatrix[1][2] = a10 * b02 + a11 * b12 + a12 * b22;
    outTransformation.mMatrix[1][3] = a10 * b03 + a11 * b13 + a12 * b23 + a13;
    outTransformation.mMatrix[2][0] = a20 * b00 + a21 * b10 + a22 * b20;
    outTransformation.mMatrix[2][1] = a20 * b01 + a21 * b11 + a22 * b21;
    outTransformation.mMatrix[2][2] = a20 * b02 + a21 * b12 + a22 * b22;
    outTransformation.mMatrix[2][3] = a20 * b03 + a21 * b13 + a22 * b23 + a23;
    outTransformation.mMatrix[3][0] = inTransformation.mMatrix[3][0];
    outTransformation.mMatrix[3][1] = inTransformation.mMatrix[3][1];
    outTransformation.mMatrix[3][2] = inTransformation.mMatrix[3][2];
    outTransformation.mMatrix[3][3] = inTransformation.mMatrix[3][3];
}

void GETransformation::Apply(const GE3DLine& inLine, GE3DLine& outLine) const
{
    GE3DPoint theStart;
    GEVector theDirection;
    this->Apply(inLine.GetStart(), theStart);
    this->Apply(inLine.GetDirection(), theDirection);
    outLine.Define(theStart, theDirection);
}

// FUNCTION: LLE 0x10034d9e
// Transforms a plane (its point and normal) into outPlane
void GETransformation::Apply(const GE3DPlane& inPlane, GE3DPlane& outPlane) const
{
    GE3DPoint thePoint;
    GEVector theNormal;
    this->Apply(inPlane.GetPoint(), thePoint);
    this->Apply(inPlane.GetNormal(), theNormal);
    outPlane.Define(thePoint, theNormal);
}

// FUNCTION: LLE 0x10034df8
// Extracts yaw, pitch and roll angles from the rotation part into an orientation
void GETransformation::ConvertToOrientation(GEOrientation& outOrientation) const
{
    float theYaw = UMArcTan2(this->mMatrix[1][0], this->mMatrix[0][0]);
    outOrientation.mYaw = theYaw;
    float thePitch = UMArcCos(-this->mMatrix[2][1]) - 1.5707964f;
    outOrientation.mPitch = thePitch;
    float theRoll = UMArcTan2(this->mMatrix[2][0], this->mMatrix[0][0]);
    outOrientation.mRoll = theRoll;
}

// FUNCTION: LLE 0x10034e48
// Reads the sixteen matrix elements from a file, stopping at the first error
long GETransformation::Load(FSFile& inFile)
{
    long theResult = kErrorOK;
    int i;
    unsigned long j;
    for (i = 0; i < 4 && theResult == kErrorOK; i++)
        for (j = 0; j < 4 && theResult == kErrorOK; j++)
            theResult = inFile.Read(this->mMatrix[i][j]);
    return theResult;
}

// FUNCTION: LLE 0x10034e89
// Resets the transformation to identity
void GETransformation::MakeIdentity()
{
    this->mMatrix[0][0] = 1.0f;
    this->mMatrix[0][1] = 0.0f;
    this->mMatrix[0][2] = 0.0f;
    this->mMatrix[0][3] = 0.0f;
    this->mMatrix[1][0] = 0.0f;
    this->mMatrix[1][1] = 1.0f;
    this->mMatrix[1][2] = 0.0f;
    this->mMatrix[1][3] = 0.0f;
    this->mMatrix[2][0] = 0.0f;
    this->mMatrix[2][1] = 0.0f;
    this->mMatrix[2][2] = 1.0f;
    this->mMatrix[2][3] = 0.0f;
    this->mMatrix[3][0] = 0.0f;
    this->mMatrix[3][1] = 0.0f;
    this->mMatrix[3][2] = 0.0f;
    this->mMatrix[3][3] = 1.0f;
}

// FUNCTION: LLE 0x10034ed9
// Computes the inverse of a rigid transformation (transposed rotation, negated rotated position)
void GETransformation::MakeInverse(GETransformation& outInverse) const
{
    GE3DPoint thePosition;
    GE3DPoint theNegative(-this->mMatrix[0][3], -this->mMatrix[1][3], -this->mMatrix[2][3]);
    outInverse.mMatrix[0][0] = this->mMatrix[0][0];
    outInverse.mMatrix[0][1] = this->mMatrix[1][0];
    outInverse.mMatrix[0][2] = this->mMatrix[2][0];
    outInverse.mMatrix[0][3] = 0.0f;
    outInverse.mMatrix[1][0] = this->mMatrix[0][1];
    outInverse.mMatrix[1][1] = this->mMatrix[1][1];
    outInverse.mMatrix[1][2] = this->mMatrix[2][1];
    outInverse.mMatrix[1][3] = 0.0f;
    outInverse.mMatrix[2][0] = this->mMatrix[0][2];
    outInverse.mMatrix[2][1] = this->mMatrix[1][2];
    outInverse.mMatrix[2][2] = this->mMatrix[2][2];
    outInverse.mMatrix[2][3] = 0.0f;
    outInverse.mMatrix[3][0] = 0.0f;
    outInverse.mMatrix[3][1] = 0.0f;
    outInverse.mMatrix[3][2] = 0.0f;
    outInverse.mMatrix[3][3] = 1.0f;
    outInverse.Apply(theNegative, thePosition);
    outInverse.SetPosition(thePosition.GetX(), thePosition.GetY(), thePosition.GetZ());
}

// NEAR: only the sqrt argument differs (the original keeps x and y on the FPU stack); tried float/double sums,
// accessor/member operands in both orders, UMFastMath::Sqrt
// FUNCTION: LLE 0x10034f87
// Builds a camera transformation at the eye looking toward the target (special case for straight up or down)
void GETransformation::MakeLookAtTransform(GE3DPoint& inEye, GE3DPoint& inTarget)
{
    static const float kLookDown[4][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    GEVector theDirection(inTarget.GetX() - inEye.GetX(), inTarget.GetY() - inEye.GetY(), inTarget.GetZ() - inEye.GetZ());
    theDirection.Normalize();
    if (theDirection.mX >= -0.0001f && theDirection.mX <= 0.0001f &&
        theDirection.mY >= -0.0001f && theDirection.mY <= 0.0001f) {
        memcpy(this->mMatrix, kLookDown, sizeof(this->mMatrix));
        if (theDirection.mZ < 0.0f) {
            this->mMatrix[1][2] = 1.0f;
            this->mMatrix[2][1] = -1.0f;
        }
    } else {
        float theLength = theDirection.GetX() * theDirection.GetX() + theDirection.GetY() * theDirection.GetY();
        theLength = (float)sqrt(theLength);
        GEVector theRight(theDirection.GetY() / theLength, -(theDirection.GetX() / theLength), 0.0f);
        GEVector theUp;
        theRight.CrossProduct(theDirection, theUp);
        theUp.Normalize();
        this->mMatrix[0][0] = theRight.GetX();
        this->mMatrix[1][0] = theRight.GetY();
        this->mMatrix[2][0] = theRight.GetZ();
        this->mMatrix[3][0] = 0.0f;
        this->mMatrix[0][1] = theDirection.GetX();
        this->mMatrix[1][1] = theDirection.GetY();
        this->mMatrix[2][1] = theDirection.GetZ();
        this->mMatrix[3][1] = 0.0f;
        this->mMatrix[0][2] = theUp.GetX();
        this->mMatrix[1][2] = theUp.GetY();
        this->mMatrix[2][2] = theUp.GetZ();
        this->mMatrix[3][2] = 0.0f;
        this->mMatrix[0][3] = 0.0f;
        this->mMatrix[1][3] = 0.0f;
        this->mMatrix[2][3] = 0.0f;
        this->mMatrix[3][3] = 1.0f;
    }
}

// FUNCTION: LLE 0x100350e5
// Rescales the three rotation columns to unit length
void GETransformation::Normalize()
{
    unsigned long i;
    float theScale;
    for (i = 0; i < 3; i++) {
        theScale = this->mMatrix[0][i] * this->mMatrix[0][i] + this->mMatrix[1][i] * this->mMatrix[1][i] + this->mMatrix[2][i] * this->mMatrix[2][i];
        theScale = 1.0f / (float)sqrt(theScale);
        this->mMatrix[0][i] *= theScale;
        this->mMatrix[1][i] *= theScale;
        this->mMatrix[2][i] *= theScale;
    }
}

// FUNCTION: LLE 0x1003513a
// Rotates the transformation by yaw, pitch and roll applied after it (about its own axes)
void GETransformation::Revolve(float inYaw, float inPitch, float inRoll)
{
    GETransformation theCopy(*this);
    GETransformation theRotation(false);
    theRotation.mMatrix[0][3] = 0.0f;
    theRotation.mMatrix[1][3] = 0.0f;
    theRotation.mMatrix[2][3] = 0.0f;
    theRotation.mMatrix[3][0] = 0.0f;
    theRotation.mMatrix[3][1] = 0.0f;
    theRotation.mMatrix[3][2] = 0.0f;
    theRotation.mMatrix[3][3] = 1.0f;
    theRotation.SetOrientationYPR(inYaw, inPitch, inRoll);
    theCopy.Apply(theRotation, *this);
}

// FUNCTION: LLE 0x100351a1
// Rotates the transformation by yaw, pitch and roll applied before it (about the parent axes)
void GETransformation::Rotate(float inYaw, float inPitch, float inRoll)
{
    GETransformation theCopy(*this);
    GETransformation theRotation(false);
    theRotation.mMatrix[0][3] = 0.0f;
    theRotation.mMatrix[1][3] = 0.0f;
    theRotation.mMatrix[2][3] = 0.0f;
    theRotation.mMatrix[3][0] = 0.0f;
    theRotation.mMatrix[3][1] = 0.0f;
    theRotation.mMatrix[3][2] = 0.0f;
    theRotation.mMatrix[3][3] = 1.0f;
    theRotation.SetOrientationYPR(inYaw, inPitch, inRoll);
    theRotation.Apply(theCopy, *this);
}

// FUNCTION: LLE 0x10035208
// Scales the transformation by x, y and z (scale applied before it)
void GETransformation::Scale(float inX, float inY, float inZ)
{
    GETransformation theCopy(*this);
    GETransformation theScale(false);
    theScale.SetScale(inX, inY, inZ);
    theScale.mMatrix[0][3] = 0.0f;
    theScale.mMatrix[1][3] = 0.0f;
    theScale.mMatrix[2][3] = 0.0f;
    theScale.Apply(theCopy, *this);
}

// FUNCTION: LLE 0x1003525b
// Writes the sixteen matrix elements to a file, stopping at the first error
long GETransformation::Save(FSFile& inFile) const
{
    long theResult = kErrorOK;
    int i;
    unsigned long j;
    for (i = 0; i < 4 && theResult == kErrorOK; i++)
        for (j = 0; j < 4 && theResult == kErrorOK; j++)
            theResult = inFile.Write(this->mMatrix[i][j]);
    return theResult;
}

// FUNCTION: LLE 0x1003529c
// Sets the rotation from yaw, pitch and roll in pitch-roll-yaw order (position kept)
void GETransformation::SetOrientationPRY(float inYaw, float inPitch, float inRoll)
{
    float theCosYaw, theSinYaw, theNegSinYaw, theCosPitch, theSinPitch, theNegSinPitch, theCosRoll, theSinRoll, theNegSinRoll;
    float thePR10, thePR12, thePR20, thePR22;
    theCosYaw = UMCos(inYaw);
    theSinYaw = UMSin(inYaw);
    theNegSinYaw = -theSinYaw;
    theCosPitch = UMCos(inPitch);
    theSinPitch = UMSin(inPitch);
    theNegSinPitch = -theSinPitch;
    theCosRoll = UMCos(inRoll);
    theSinRoll = UMSin(inRoll);
    theNegSinRoll = -theSinRoll;
    thePR10 = theNegSinPitch * theSinRoll;
    thePR12 = theNegSinPitch * theCosRoll;
    thePR20 = theCosPitch * theSinRoll;
    thePR22 = theCosPitch * theCosRoll;
    this->mMatrix[0][0] = theCosRoll * theCosYaw;
    this->mMatrix[0][1] = theCosRoll * theNegSinYaw;
    this->mMatrix[0][2] = theNegSinRoll;
    this->mMatrix[1][0] = thePR10 * theCosYaw + theCosPitch * theSinYaw;
    this->mMatrix[1][1] = thePR10 * theNegSinYaw + theCosPitch * theCosYaw;
    this->mMatrix[1][2] = thePR12;
    this->mMatrix[2][0] = thePR20 * theCosYaw + theSinPitch * theSinYaw;
    this->mMatrix[2][1] = thePR20 * theNegSinYaw + theSinPitch * theCosYaw;
    this->mMatrix[2][2] = thePR22;
}

// FUNCTION: LLE 0x10035395
// Sets the rotation from yaw, pitch and roll in pitch-yaw-roll order (position kept)
void GETransformation::SetOrientationPYR(float inYaw, float inPitch, float inRoll)
{
    float theCosYaw, theSinYaw, theNegSinYaw, theCosPitch, theSinPitch, theNegSinPitch, theCosRoll, theSinRoll, theNegSinRoll;
    float thePY10, thePY11, thePY20, thePY21;
    theCosYaw = UMCos(inYaw);
    theSinYaw = UMSin(inYaw);
    theNegSinYaw = -theSinYaw;
    theCosPitch = UMCos(inPitch);
    theSinPitch = UMSin(inPitch);
    theNegSinPitch = -theSinPitch;
    theCosRoll = UMCos(inRoll);
    theSinRoll = UMSin(inRoll);
    theNegSinRoll = -theSinRoll;
    thePY10 = theCosPitch * theSinYaw;
    thePY11 = theCosPitch * theCosYaw;
    thePY20 = theSinPitch * theSinYaw;
    thePY21 = theSinPitch * theCosYaw;
    this->mMatrix[0][0] = theCosYaw * theCosRoll;
    this->mMatrix[0][1] = theNegSinYaw;
    this->mMatrix[0][2] = theCosYaw * theNegSinRoll;
    this->mMatrix[1][0] = thePY10 * theCosRoll + theNegSinPitch * theSinRoll;
    this->mMatrix[1][1] = thePY11;
    this->mMatrix[1][2] = thePY10 * theNegSinRoll + theNegSinPitch * theCosRoll;
    this->mMatrix[2][0] = thePY20 * theCosRoll + theCosPitch * theSinRoll;
    this->mMatrix[2][1] = thePY21;
    this->mMatrix[2][2] = thePY20 * theNegSinRoll + theCosPitch * theCosRoll;
}

// 95.18: register lifetimes of theRP01/theSinRoll differ (the other five orders are exact with the same shape);
// tried temp order, term order, -theSinRoll inline
// FUNCTION: LLE 0x1003548e
// Sets the rotation from yaw, pitch and roll in roll-pitch-yaw order (position kept)
void GETransformation::SetOrientationRPY(float inYaw, float inPitch, float inRoll)
{
    float theCosYaw, theSinYaw, theNegSinYaw, theCosPitch, theSinPitch, theNegSinPitch, theCosRoll, theSinRoll, theNegSinRoll;
    float theRP01, theRP02, theRP21, theRP22;
    theCosYaw = UMCos(inYaw);
    theSinYaw = UMSin(inYaw);
    theNegSinYaw = -theSinYaw;
    theCosPitch = UMCos(inPitch);
    theSinPitch = UMSin(inPitch);
    theNegSinPitch = -theSinPitch;
    theCosRoll = UMCos(inRoll);
    theSinRoll = UMSin(inRoll);
    theNegSinRoll = -theSinRoll;
    theRP02 = theNegSinRoll * theCosPitch;
    theRP22 = theCosRoll * theCosPitch;
    theRP01 = theNegSinRoll * theSinPitch;
    theRP21 = theCosRoll * theSinPitch;
    this->mMatrix[0][0] = theCosRoll * theCosYaw + theRP01 * theSinYaw;
    this->mMatrix[0][1] = theCosRoll * theNegSinYaw + theRP01 * theCosYaw;
    this->mMatrix[0][2] = theRP02;
    this->mMatrix[1][0] = theCosPitch * theSinYaw;
    this->mMatrix[1][1] = theCosPitch * theCosYaw;
    this->mMatrix[1][2] = theNegSinPitch;
    this->mMatrix[2][0] = theSinRoll * theCosYaw + theRP21 * theSinYaw;
    this->mMatrix[2][1] = theSinRoll * theNegSinYaw + theRP21 * theCosYaw;
    this->mMatrix[2][2] = theRP22;
}

// FUNCTION: LLE 0x10035580
// Sets the rotation from yaw, pitch and roll in roll-yaw-pitch order (position kept)
void GETransformation::SetOrientationRYP(float inYaw, float inPitch, float inRoll)
{
    float theCosYaw, theSinYaw, theNegSinYaw, theCosPitch, theSinPitch, theNegSinPitch, theCosRoll, theSinRoll, theNegSinRoll;
    float theRY00, theRY01, theRY20, theRY21;
    theCosYaw = UMCos(inYaw);
    theSinYaw = UMSin(inYaw);
    theNegSinYaw = -theSinYaw;
    theCosPitch = UMCos(inPitch);
    theSinPitch = UMSin(inPitch);
    theNegSinPitch = -theSinPitch;
    theCosRoll = UMCos(inRoll);
    theSinRoll = UMSin(inRoll);
    theNegSinRoll = -theSinRoll;
    theRY00 = theCosRoll * theCosYaw;
    theRY01 = theCosRoll * theNegSinYaw;
    theRY20 = theSinRoll * theCosYaw;
    theRY21 = theSinRoll * theNegSinYaw;
    this->mMatrix[0][0] = theRY00;
    this->mMatrix[0][1] = theRY01 * theCosPitch + theNegSinRoll * theSinPitch;
    this->mMatrix[0][2] = theRY01 * theNegSinPitch + theNegSinRoll * theCosPitch;
    this->mMatrix[1][0] = theSinYaw;
    this->mMatrix[1][1] = theCosYaw * theCosPitch;
    this->mMatrix[1][2] = theCosYaw * theNegSinPitch;
    this->mMatrix[2][0] = theRY20;
    this->mMatrix[2][1] = theRY21 * theCosPitch + theCosRoll * theSinPitch;
    this->mMatrix[2][2] = theRY21 * theNegSinPitch + theCosRoll * theCosPitch;
}

// FUNCTION: LLE 0x10035675
// Sets the rotation from yaw, pitch and roll in yaw-pitch-roll order (position kept)
void GETransformation::SetOrientationYPR(float inYaw, float inPitch, float inRoll)
{
    float theCosYaw, theSinYaw, theNegSinYaw, theCosPitch, theSinPitch, theNegSinPitch, theCosRoll, theSinRoll, theNegSinRoll;
    float theYP01, theYP02, theYP11, theYP12;
    theCosYaw = UMCos(inYaw);
    theSinYaw = UMSin(inYaw);
    theNegSinYaw = -theSinYaw;
    theCosPitch = UMCos(inPitch);
    theSinPitch = UMSin(inPitch);
    theNegSinPitch = -theSinPitch;
    theCosRoll = UMCos(inRoll);
    theSinRoll = UMSin(inRoll);
    theNegSinRoll = -theSinRoll;
    theYP01 = theNegSinYaw * theCosPitch;
    theYP02 = theNegSinYaw * theNegSinPitch;
    theYP11 = theCosYaw * theCosPitch;
    theYP12 = theCosYaw * theNegSinPitch;
    this->mMatrix[0][0] = theCosYaw * theCosRoll + theYP02 * theSinRoll;
    this->mMatrix[0][1] = theYP01;
    this->mMatrix[0][2] = theCosYaw * theNegSinRoll + theYP02 * theCosRoll;
    this->mMatrix[1][0] = theSinYaw * theCosRoll + theYP12 * theSinRoll;
    this->mMatrix[1][1] = theYP11;
    this->mMatrix[1][2] = theSinYaw * theNegSinRoll + theYP12 * theCosRoll;
    this->mMatrix[2][0] = theCosPitch * theSinRoll;
    this->mMatrix[2][1] = theSinPitch;
    this->mMatrix[2][2] = theCosPitch * theCosRoll;
}

// FUNCTION: LLE 0x10035769
// Sets the rotation from yaw, pitch and roll in yaw-roll-pitch order (position kept)
void GETransformation::SetOrientationYRP(float inYaw, float inPitch, float inRoll)
{
    float theCosYaw, theSinYaw, theNegSinYaw, theCosPitch, theSinPitch, theNegSinPitch, theCosRoll, theSinRoll, theNegSinRoll;
    float theYR00, theYR02, theYR10, theYR12;
    theCosYaw = UMCos(inYaw);
    theSinYaw = UMSin(inYaw);
    theNegSinYaw = -theSinYaw;
    theCosPitch = UMCos(inPitch);
    theSinPitch = UMSin(inPitch);
    theNegSinPitch = -theSinPitch;
    theCosRoll = UMCos(inRoll);
    theSinRoll = UMSin(inRoll);
    theNegSinRoll = -theSinRoll;
    theYR00 = theCosYaw * theCosRoll;
    theYR02 = theCosYaw * theNegSinRoll;
    theYR10 = theSinYaw * theCosRoll;
    theYR12 = theSinYaw * theNegSinRoll;
    this->mMatrix[0][0] = theYR00;
    this->mMatrix[0][1] = theNegSinYaw * theCosPitch + theYR02 * theSinPitch;
    this->mMatrix[0][2] = theNegSinYaw * theNegSinPitch + theYR02 * theCosPitch;
    this->mMatrix[1][0] = theYR10;
    this->mMatrix[1][1] = theCosYaw * theCosPitch + theYR12 * theSinPitch;
    this->mMatrix[1][2] = theCosYaw * theNegSinPitch + theYR12 * theCosPitch;
    this->mMatrix[2][0] = theSinRoll;
    this->mMatrix[2][1] = theCosRoll * theSinPitch;
    this->mMatrix[2][2] = theCosRoll * theCosPitch;
}

// FUNCTION: LLE 0x1003585b
// Makes the transformation a pure scale by x, y and z (position column kept)
void GETransformation::SetScale(float inX, float inY, float inZ)
{
    this->mMatrix[0][0] = inX;
    this->mMatrix[0][1] = 0.0f;
    this->mMatrix[0][2] = 0.0f;
    this->mMatrix[1][0] = 0.0f;
    this->mMatrix[1][1] = inY;
    this->mMatrix[1][2] = 0.0f;
    this->mMatrix[2][0] = 0.0f;
    this->mMatrix[2][1] = 0.0f;
    this->mMatrix[2][2] = inZ;
    this->mMatrix[3][0] = 0.0f;
    this->mMatrix[3][1] = 0.0f;
    this->mMatrix[3][2] = 0.0f;
    this->mMatrix[3][3] = 1.0f;
}

// FUNCTION: LLE 0x100358a4
// Returns true (no consistency check in the retail build)
bool GETransformation::Verify() const
{
    return true;
}

// FUNCTION: LLE 0x100358a7
// Resets the rotation to identity, keeping the position
void GETransformation::ZeroOrientation()
{
    this->mMatrix[0][0] = 1.0f;
    this->mMatrix[0][1] = 0.0f;
    this->mMatrix[0][2] = 0.0f;
    this->mMatrix[1][0] = 0.0f;
    this->mMatrix[1][1] = 1.0f;
    this->mMatrix[1][2] = 0.0f;
    this->mMatrix[2][0] = 0.0f;
    this->mMatrix[2][1] = 0.0f;
    this->mMatrix[2][2] = 1.0f;
    this->mMatrix[3][0] = 0.0f;
    this->mMatrix[3][1] = 0.0f;
    this->mMatrix[3][2] = 0.0f;
    this->mMatrix[3][3] = 1.0f;
}

// FUNCTION: LLE 0x100358e8
// Writes a copy with identity rotation and this transformation's position into outTransformation
void GETransformation::ZeroOrientation(GETransformation& outTransformation) const
{
    outTransformation.mMatrix[0][0] = 1.0f;
    outTransformation.mMatrix[0][1] = 0.0f;
    outTransformation.mMatrix[0][2] = 0.0f;
    outTransformation.mMatrix[0][3] = this->mMatrix[0][3];
    outTransformation.mMatrix[1][0] = 0.0f;
    outTransformation.mMatrix[1][1] = 1.0f;
    outTransformation.mMatrix[1][2] = 0.0f;
    outTransformation.mMatrix[1][3] = this->mMatrix[1][3];
    outTransformation.mMatrix[2][0] = 0.0f;
    outTransformation.mMatrix[2][1] = 0.0f;
    outTransformation.mMatrix[2][2] = 1.0f;
    outTransformation.mMatrix[2][3] = this->mMatrix[2][3];
    outTransformation.mMatrix[3][0] = 0.0f;
    outTransformation.mMatrix[3][1] = 0.0f;
    outTransformation.mMatrix[3][2] = 0.0f;
    outTransformation.mMatrix[3][3] = 1.0f;
}

// ---- Exported members with no address of their own (ICF-folded: GetChecksum's `return 0` 0x32e79, Dump's empty
// body 0x80c07)
void GETransformation::Dump() const
{
}

unsigned long GETransformation::GetChecksum() const
{
    return 0;
}
