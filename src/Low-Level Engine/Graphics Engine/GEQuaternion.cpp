// C:\EEX\Low-Level Engine\Graphics Engine\GEQuaternion.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10023219..0x10023655: matrix <-> quaternion conversion, Normalize and Slerp (Shoemake's algorithms). Every other
// GEQuaternion member is header-inline. No init chains.
#include <windows.h>
#include <math.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/GE2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPoint.h"
#include "lle/GEOrientation.h"
#include "lle/GETransformation.h"
#include "lle/GEQuaternion.h"
#include "lle/UMFastMath.h"

// FUNCTION: LLE 0x10023219
// Sets the quaternion from a transformation's rotation (vector part negated), then normalises it
void GEQuaternion::ConvertFrom(const GETransformation& inTransformation)
{
    float theTrace, theScale, theQuaternion[4];
    int i, j, k;
    int theNext[3] = {1, 2, 0};
    theTrace = inTransformation.GetElement(0, 0) + inTransformation.GetElement(2, 2) + inTransformation.GetElement(1, 1);
    if (theTrace > 0.0f) {
        theScale = (float)sqrt(theTrace + 1.0f);
        this->mW = theScale * 0.5f;
        theScale = 0.5f / theScale;
        this->mX = (inTransformation.GetElement(1, 2) - inTransformation.GetElement(2, 1)) * theScale;
        this->mY = (inTransformation.GetElement(2, 0) - inTransformation.GetElement(0, 2)) * theScale;
        this->mZ = (inTransformation.GetElement(0, 1) - inTransformation.GetElement(1, 0)) * theScale;
    } else {
        i = 0;
        if (inTransformation.GetElement(1, 1) > inTransformation.GetElement(0, 0))
            i = 1;
        if (inTransformation.GetElement(2, 2) > inTransformation.GetElement(i, i))
            i = 2;
        j = theNext[i];
        k = theNext[j];
        theScale = (float)sqrt(inTransformation.GetElement(i, i) - (inTransformation.GetElement(j, j) + inTransformation.GetElement(k, k)) + 1.0f);
        theQuaternion[i] = theScale * 0.5f;
        if (theScale != 0.0f)
            theScale = 0.5f / theScale;
        theQuaternion[3] = (inTransformation.GetElement(j, k) - inTransformation.GetElement(k, j)) * theScale;
        theQuaternion[j] = (inTransformation.GetElement(i, j) + inTransformation.GetElement(j, i)) * theScale;
        theQuaternion[k] = (inTransformation.GetElement(i, k) + inTransformation.GetElement(k, i)) * theScale;
        this->mX = theQuaternion[0];
        this->mY = theQuaternion[1];
        this->mZ = theQuaternion[2];
        this->mW = theQuaternion[3];
    }
    this->mX = -this->mX;
    this->mY = -this->mY;
    this->mZ = -this->mZ;
    this->Normalize();
}

// FUNCTION: LLE 0x100233b7
// Writes the quaternion's rotation into a transformation (no translation), then normalises the matrix
void GEQuaternion::ConvertTo(GETransformation& outTransformation) const
{
    GEQuaternion theQuaternion;
    float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
    theQuaternion = *this;
    theQuaternion.Normalize();
    s = 2.0f;
    xs = theQuaternion.mX * s;
    ys = theQuaternion.mY * s;
    zs = theQuaternion.mZ * s;
    xx = theQuaternion.mX * xs;
    xy = theQuaternion.mX * ys;
    xz = theQuaternion.mX * zs;
    yy = theQuaternion.mY * ys;
    yz = theQuaternion.mY * zs;
    zz = theQuaternion.mZ * zs;
    wx = theQuaternion.mW * xs;
    wy = theQuaternion.mW * ys;
    wz = theQuaternion.mW * zs;
    outTransformation.SetElement(0, 0, 1.0f - (yy + zz));
    outTransformation.SetElement(0, 1, xy - wz);
    outTransformation.SetElement(0, 2, xz + wy);
    outTransformation.SetElement(0, 3, 0.0f);
    outTransformation.SetElement(1, 0, xy + wz);
    outTransformation.SetElement(1, 1, 1.0f - (xx + zz));
    outTransformation.SetElement(1, 2, yz - wx);
    outTransformation.SetElement(1, 3, 0.0f);
    outTransformation.SetElement(2, 0, xz - wy);
    outTransformation.SetElement(2, 1, yz + wx);
    outTransformation.SetElement(2, 2, 1.0f - (xx + yy));
    outTransformation.SetElement(2, 3, 0.0f);
    outTransformation.SetElement(3, 0, 0.0f);
    outTransformation.SetElement(3, 1, 0.0f);
    outTransformation.SetElement(3, 2, 0.0f);
    outTransformation.SetElement(3, 3, 1.0f);
    outTransformation.Normalize();
}

// FUNCTION: LLE 0x100234ca
// Scales the quaternion to unit length
// NEAR: the inline UMFastMath::Sqrt gives the fstp st(4) parameter shape (plain sqrt is further off); the second
// push ecx of the double argument area is scheduled one load earlier than the original
void GEQuaternion::Normalize()
{
    float theScale = 1.0f / UMFastMath::Sqrt(this->mX * this->mX + this->mY * this->mY + this->mZ * this->mZ + this->mW * this->mW);
    this->mW *= theScale;
    this->mX *= theScale;
    this->mY *= theScale;
    this->mZ *= theScale;
}

// FUNCTION: LLE 0x10023526
// Interpolates between this quaternion and a target along the shorter arc (linear when they are nearly equal)
void GEQuaternion::Slerp(const GEQuaternion& inTarget, float inT, GEQuaternion& outQuaternion) const
{
    float theTarget[4];
    double theOmega, theSinOmega, theScale0, theScale1;
    double theCosOmega = inTarget.mZ * this->mZ + inTarget.mY * this->mY + inTarget.mX * this->mX + this->mW * inTarget.mW;
    if (theCosOmega < 0.0) {
        theCosOmega = -theCosOmega;
        theTarget[0] = -inTarget.mX;
        theTarget[1] = -inTarget.mY;
        theTarget[2] = -inTarget.mZ;
        theTarget[3] = -inTarget.mW;
    } else {
        theTarget[0] = inTarget.mX;
        theTarget[1] = inTarget.mY;
        theTarget[2] = inTarget.mZ;
        theTarget[3] = inTarget.mW;
    }
    if (1.0 - theCosOmega > 0.001f) {
        theOmega = UMArcCos(theCosOmega);
        theSinOmega = UMSin((float)theOmega);
        theScale0 = UMSin((float)((1.0 - inT) * theOmega)) / theSinOmega;
        theScale1 = UMSin((float)(inT * theOmega)) / theSinOmega;
    } else {
        theScale0 = 1.0 - inT;
        theScale1 = inT;
    }
    outQuaternion.mX = (float)(this->mX * theScale0 + theTarget[0] * theScale1);
    outQuaternion.mY = (float)(this->mY * theScale0 + theTarget[1] * theScale1);
    outQuaternion.mZ = (float)(this->mZ * theScale0 + theTarget[2] * theScale1);
    outQuaternion.mW = (float)(this->mW * theScale0 + theTarget[3] * theScale1);
}
