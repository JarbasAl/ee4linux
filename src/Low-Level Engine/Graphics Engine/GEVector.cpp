// C:\EEX\Low-Level Engine\Graphics Engine\GEVector.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10035941..0x100367fc: the static init chains (the four string IDs, sQuantizedVectorArray, the zero vector), the
// QuantizedVectorArray ctor/dtor, then the quantization members. The basic GEVector operations are emitted in the
// GE2DPoint.cpp block (see lle/GEVector.h). GetQuantizedVersion expands the inline-asm UMFloatToLong, which turns
// off most of VC6's optimization for that function.
//
// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x10035941
// SYNTHETIC: LLE 0x1003594b
// SYNTHETIC: LLE 0x1003595b
// SYNTHETIC: LLE 0x10035967
// SYNTHETIC: LLE 0x10035982
// SYNTHETIC: LLE 0x1003598c
// SYNTHETIC: LLE 0x1003599c
// SYNTHETIC: LLE 0x100359a8
// SYNTHETIC: LLE 0x100359c3
// SYNTHETIC: LLE 0x100359cd
// SYNTHETIC: LLE 0x100359dd
// SYNTHETIC: LLE 0x100359e9
// SYNTHETIC: LLE 0x10035a04
// SYNTHETIC: LLE 0x10035a0e
// SYNTHETIC: LLE 0x10035a1e
// SYNTHETIC: LLE 0x10035a2a
// SYNTHETIC: LLE 0x10035a45
// SYNTHETIC: LLE 0x10035a4f
// SYNTHETIC: LLE 0x10035a59
// SYNTHETIC: LLE 0x10035a65
// SYNTHETIC: LLE 0x10035a80
// SYNTHETIC: LLE 0x10035a8a
// SYNTHETIC: LLE 0x10035aa3
#include <windows.h>

#define LLE_GEVECTOR_CPP    // GEVector's statics are visible here only (see lle/GEVector.h) (provisional name)
#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/GE2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPoint.h"
#include "lle/GE3DLine.h"
#include "lle/GE3DPlane.h"
#include "lle/FSFile.h"

// Unit-local constants (loaded from memory, away from the shared __real pool) (provisional names)
const float kQuantizedVectorStep = 0.05f;   // 0x1009809c: 2 / kQuantizedVectorSize
const float kQuantizedVectorSize = 40.0f;   // 0x100980f0
// The unit cube's six faces, each sampled on a 41 x 41 grid of normalized directions, and the six face planes
// (-X, -Y, -Z, +X, +Y, +Z). sizeof 0x1d8e0.
struct GEVector::QuantizedVectorArray {
    QuantizedVectorArray();
    ~QuantizedVectorArray();
    GEVector mVectors[41][6 * 41];          // +0x00000
    GE3DPlane* mPlanes[6];                  // +0x1d8c8
};
const UString GEVector::sVectorXStringID("GEVector-VectorX");
const UString GEVector::sVectorYStringID("GEVector-VectorY");
const UString GEVector::sVectorZStringID("GEVector-VectorZ");
const UString GEVector::sQuantizedVectorIndexStringID("GEVector-QuantizedVectorIndex");
GEVector::QuantizedVectorArray GEVector::sQuantizedVectorArray;
static GEVector sZeroVector;                // 0x1010ff60: the quantized version of (0, 0, 0) (provisional name)
// NEAR: the original calls the out-of-line GEVector() COMDAT (0xc240) in a loop to build mVectors, where this
// inlines the three fldz (inline budget: tools/inline_sim.py gives the vector-ctor iterator's ctor share 49, the
// original needs < 41; tried accessor-heavy copies, an in-class ctor, default ctor + setters for theVector)
// FUNCTION: LLE 0x10035ab0
// Builds the table of quantized unit directions (a 41 x 41 grid on each cube face) and the six face planes
GEVector::QuantizedVectorArray::QuantizedVectorArray()
{
    int theX, theY, theZ;
    float theFirst;
    for (theY = 0; theY < 41; theY++) {
        theFirst = theY * kQuantizedVectorStep - 1.0f;
        for (theZ = 0; theZ < 41; theZ++) {
            GEVector theVector(-1.0f, theFirst, theZ * kQuantizedVectorStep - 1.0f);
            theVector.Normalize();
            this->mVectors[theY][0 * 41 + theZ] = theVector;
            theVector.SetX(-theVector.GetX());
            this->mVectors[theY][3 * 41 + theZ] = theVector;
        }
    }
    for (theX = 0; theX < 41; theX++) {
        theFirst = theX * kQuantizedVectorStep - 1.0f;
        for (theZ = 0; theZ < 41; theZ++) {
            GEVector theVector(theFirst, -1.0f, theZ * kQuantizedVectorStep - 1.0f);
            theVector.Normalize();
            this->mVectors[theX][1 * 41 + theZ] = theVector;
            theVector.SetY(-theVector.GetY());
            this->mVectors[theX][4 * 41 + theZ] = theVector;
        }
    }
    for (theX = 0; theX < 41; theX++) {
        theFirst = theX * kQuantizedVectorStep - 1.0f;
        for (theY = 0; theY < 41; theY++) {
            GEVector theVector(theFirst, theY * kQuantizedVectorStep - 1.0f, -1.0f);
            theVector.Normalize();
            this->mVectors[theX][2 * 41 + theY] = theVector;
            theVector.SetZ(-theVector.GetZ());
            this->mVectors[theX][5 * 41 + theY] = theVector;
        }
    }
    this->mPlanes[0] = new GE3DPlane(GE3DPoint(-1.0f, 0.0f, 0.0f), GEVector(-1.0f, 0.0f, 0.0f));
    this->mPlanes[1] = new GE3DPlane(GE3DPoint(0.0f, -1.0f, 0.0f), GEVector(0.0f, -1.0f, 0.0f));
    this->mPlanes[2] = new GE3DPlane(GE3DPoint(0.0f, 0.0f, -1.0f), GEVector(0.0f, 0.0f, -1.0f));
    this->mPlanes[3] = new GE3DPlane(GE3DPoint(1.0f, 0.0f, 0.0f), GEVector(1.0f, 0.0f, 0.0f));
    this->mPlanes[4] = new GE3DPlane(GE3DPoint(0.0f, 1.0f, 0.0f), GEVector(0.0f, 1.0f, 0.0f));
    this->mPlanes[5] = new GE3DPlane(GE3DPoint(0.0f, 0.0f, 1.0f), GEVector(0.0f, 0.0f, 1.0f));
}

// FUNCTION: LLE 0x10035ea3
// Destroys the quantized direction table's six face planes
GEVector::QuantizedVectorArray::~QuantizedVectorArray()
{
    delete this->mPlanes[0];
    delete this->mPlanes[1];
    delete this->mPlanes[2];
    delete this->mPlanes[3];
    delete this->mPlanes[4];
    delete this->mPlanes[5];
}

// Built without global optimisation, the /Og exception of CLAUDE.md rule 4: `this` reloaded from [ebp-0xe8] for every
// member, every local memory-resident (bools tested through movzx), an inline-`this` temp for SetDirection, `leave`.
// The rest of the unit is normal -O1. Evidence: 11.6 as plain -O1 source, exact with the pragma alone.
// - Frame order without /Og follows VC6's local-name hash (16 buckets, last-declared first in a bucket; probed),
//   so theVectorFound/theScale and theIntersects' declaration position are chosen to give the original layout
//   (provisional names).
// - Three slots per conversion (fistp, copy, copy) are a float -> long inline wrapping UMFloatToLong.
// - (col + face * 41) * 12 indexing means mVectors is [41][6 * 41].
// The conversion helper (provisional name)
inline long GEQuantizedIndex(float inValue)
{
    return UMFloatToLong(inValue);
}
#pragma optimize("g", off)
// FUNCTION: LLE 0x10035f0e
// Returns the nearest entry of the quantized direction table: the cube face the vector's ray hits,
// then the grid cell there (the shared zero vector for (0, 0, 0))
GEVector* GEVector::GetQuantizedVersion() const
{
    GEVector* theQuantizedVector = NULL;
    GE3DLine theLine;
    float theScale = kQuantizedVectorSize;
    bool theIntersects;
    bool theCheckNegativeX = true;
    bool theCheckNegativeY = true;
    bool theCheckPositiveX = true;
    bool theCheckPositiveY = true;
    bool theCheckPositiveZ = true;
    bool theVectorFound = false;
    float theT, theFirst, theSecond;
    if (this->mX != 0.0f || this->mY != 0.0f || this->mZ != 0.0f) {
        theLine.SetDirection(*this);
        if (this->mX < 0.0f)
            theCheckPositiveX = false;
        else
            theCheckNegativeX = false;
        if (this->mY < 0.0f)
            theCheckPositiveY = false;
        else
            theCheckNegativeY = false;
        if (this->mZ < 0.0f)
            theCheckPositiveZ = false;
        if (theCheckPositiveZ) {
            theIntersects = theLine.Intersects(*GEVector::sQuantizedVectorArray.mPlanes[5], theT);
            if (theIntersects && theT > 0.0f) {
                theFirst = theT * this->mX;
                theSecond = theT * this->mY;
                if (theFirst >= -1.0f && theFirst <= 1.0f && theSecond >= -1.0f && theSecond <= 1.0f) {
                    theVectorFound = true;
                    theFirst = (theFirst + 1.0f) * 0.5f * theScale;
                    theSecond = (theSecond + 1.0f) * 0.5f * theScale;
                    theQuantizedVector = &GEVector::sQuantizedVectorArray.mVectors[GEQuantizedIndex(theFirst)][5 * 41 + GEQuantizedIndex(theSecond)];
                }
            }
        }
        if (theCheckPositiveY && !theVectorFound) {
            theIntersects = theLine.Intersects(*GEVector::sQuantizedVectorArray.mPlanes[4], theT);
            if (theIntersects && theT > 0.0f) {
                theFirst = theT * this->mX;
                theSecond = theT * this->mZ;
                if (theFirst >= -1.0f && theFirst <= 1.0f && theSecond >= -1.0f && theSecond <= 1.0f) {
                    theVectorFound = true;
                    theFirst = (theFirst + 1.0f) * 0.5f * theScale;
                    theSecond = (theSecond + 1.0f) * 0.5f * theScale;
                    theQuantizedVector = &GEVector::sQuantizedVectorArray.mVectors[GEQuantizedIndex(theFirst)][4 * 41 + GEQuantizedIndex(theSecond)];
                }
            }
        }
        if (theCheckPositiveX && !theVectorFound) {
            theIntersects = theLine.Intersects(*GEVector::sQuantizedVectorArray.mPlanes[3], theT);
            if (theIntersects && theT > 0.0f) {
                theFirst = theT * this->mY;
                theSecond = theT * this->mZ;
                if (theFirst >= -1.0f && theFirst <= 1.0f && theSecond >= -1.0f && theSecond <= 1.0f) {
                    theVectorFound = true;
                    theFirst = (theFirst + 1.0f) * 0.5f * theScale;
                    theSecond = (theSecond + 1.0f) * 0.5f * theScale;
                    theQuantizedVector = &GEVector::sQuantizedVectorArray.mVectors[GEQuantizedIndex(theFirst)][3 * 41 + GEQuantizedIndex(theSecond)];
                }
            }
        }
        if (theCheckNegativeX && !theVectorFound) {
            theIntersects = theLine.Intersects(*GEVector::sQuantizedVectorArray.mPlanes[0], theT);
            if (theIntersects && theT > 0.0f) {
                theFirst = theT * this->mY;
                theSecond = theT * this->mZ;
                if (theFirst >= -1.0f && theFirst <= 1.0f && theSecond >= -1.0f && theSecond <= 1.0f) {
                    theVectorFound = true;
                    theFirst = (theFirst + 1.0f) * 0.5f * theScale;
                    theSecond = (theSecond + 1.0f) * 0.5f * theScale;
                    theQuantizedVector = &GEVector::sQuantizedVectorArray.mVectors[GEQuantizedIndex(theFirst)][0 * 41 + GEQuantizedIndex(theSecond)];
                }
            }
        }
        if (theCheckNegativeY && !theVectorFound) {
            theIntersects = theLine.Intersects(*GEVector::sQuantizedVectorArray.mPlanes[1], theT);
            if (theIntersects && theT > 0.0f) {
                theFirst = theT * this->mX;
                theSecond = theT * this->mZ;
                if (theFirst >= -1.0f && theFirst <= 1.0f && theSecond >= -1.0f && theSecond <= 1.0f) {
                    theVectorFound = true;
                    theFirst = (theFirst + 1.0f) * 0.5f * theScale;
                    theSecond = (theSecond + 1.0f) * 0.5f * theScale;
                    theQuantizedVector = &GEVector::sQuantizedVectorArray.mVectors[GEQuantizedIndex(theFirst)][1 * 41 + GEQuantizedIndex(theSecond)];
                }
            }
        }
        if (!theVectorFound) {
            theIntersects = theLine.Intersects(*GEVector::sQuantizedVectorArray.mPlanes[2], theT);
            if (theIntersects && theT > 0.0f) {
                theFirst = theT * this->mX;
                theSecond = theT * this->mY;
                if (theFirst >= -1.0f && theFirst <= 1.0f && theSecond >= -1.0f && theSecond <= 1.0f) {
                    theVectorFound = true;
                    theFirst = (theFirst + 1.0f) * 0.5f * theScale;
                    theSecond = (theSecond + 1.0f) * 0.5f * theScale;
                    theQuantizedVector = &GEVector::sQuantizedVectorArray.mVectors[GEQuantizedIndex(theFirst)][2 * 41 + GEQuantizedIndex(theSecond)];
                }
            }
        }
    } else
        theQuantizedVector = &sZeroVector;
    ESVERIFY_AT(theQuantizedVector != NULL, 383);
    return theQuantizedVector;
}
#pragma optimize("g", on)

// FUNCTION: LLE 0x1003676f
// Reads a quantized direction index from an open file and returns its table entry (the zero vector if
// out of range)
long GEVector::LoadQuantizedIndex(FSFile& inFile, GEVector*& outVector)
{
    long theResult;
    unsigned short theIndex;
    if (inFile.GetIsOpen()) {
        theResult = inFile.Read(theIndex);
        if (theResult == kErrorOK) {
            if (theIndex < sizeof(GEVector::sQuantizedVectorArray.mVectors) / sizeof(GEVector))
                outVector = &GEVector::sQuantizedVectorArray.mVectors[0][0] + theIndex;
            else
                outVector = &sZeroVector;
        }
    } else
        theResult = 4;
    return theResult;
}

// FUNCTION: LLE 0x100367be
// Writes the vector's quantized direction index to an open file
long GEVector::SaveQuantizedIndex(FSFile& inFile) const
{
    unsigned short theIndex = this->GetQuantizedVersion() - &GEVector::sQuantizedVectorArray.mVectors[0][0];
    if (inFile.GetIsOpen())
        return inFile.Write(theIndex);
    return 4;
}

// The zero vector's destructor (the members below have no address of their own)
// FUNCTION: LLE 0x10035aaf
// Destroys the file-static zero vector at shutdown (an empty body)

// ---- Exported members with no address of their own (ICF-folded: Load 0xdc23, Save 0x20dc6)
long GEVector::Load(FSFile& inFile)
{
    long theResult;
    if (inFile.GetIsOpen()) {
        theResult = inFile.Read(this->mX);
        if (theResult == 0) {
            theResult = inFile.Read(this->mY);
            if (theResult == 0)
                theResult = inFile.Read(this->mZ);
        }
    } else
        theResult = 4;
    return theResult;
}

long GEVector::Save(FSFile& inFile) const
{
    long theResult;
    if (inFile.GetIsOpen()) {
        theResult = inFile.Write(this->mX);
        if (theResult == 0) {
            theResult = inFile.Write(this->mY);
            if (theResult == 0)
                theResult = inFile.Write(this->mZ);
        }
    } else
        theResult = 4;
    return theResult;
}
