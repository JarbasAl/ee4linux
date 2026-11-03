// C:\EEX\Low-Level Engine\Graphics Engine\GEModel Save.cpp (file name inferred)
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1001e2fc..0x1001eb9f: GEModel::Save and its chunk writers (.cem "SSMF" version 2.0). A unit of its own: it has
// no U2D key init chains (the other GEModel part files all do) and ends with its own UList pool registrations
// (0x1001eb7b..0x1001eb9f) before GEModel.cpp's chains. The leading pool COMDATs (0x1001e2fc..0x1001e4e8) have no
// source here.
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/GE2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPoint.h"
#include "lle/GEOrientation.h"
#include "lle/GETransformation.h"
#include "lle/UReferenceCounter.h"
#include "lle/FSFileSpec.h"
#include "lle/GEColor.h"
#include "lle/GETextureCoordinate.h"
#include "lle/GEVertexArray.h"
#include "lle/GEPolygonArray.h"
#include "lle/GEMaterial.h"
#include "lle/GEModel.h"

// FUNCTION: LLE 0x1001e4e8
// Saves the model as .cem 2.0 (a top model first recomputes bounds, sorts tag points and removes scale),
// then its sub-models; on a chunk error rewinds the file
long GEModel::Save(FSFileSpec& inFile)
{
    long theResult;
    long thePosition;
    if (this->mParentModel == NULL) {
        this->ComputeBoundingVolumes();
        this->SortTagPoints();
        this->mGraphicsScale = 1.0f;
        this->RemoveScale();
    }
    if (inFile.GetIsOpen()) {
        theResult = inFile.GetPosition(thePosition);
        if (theResult == kErrorOK) {
            if ((theResult = this->SaveHeaderChunk(inFile)) == kErrorOK && (theResult = this->SaveCountsChunk(inFile)) == kErrorOK &&
                (theResult = this->SaveMiscellaneousChunk(inFile)) == kErrorOK &&
                (theResult = this->SavePolygonsChunk(inFile)) == kErrorOK &&
                (theResult = this->SaveMaterialsChunk(inFile)) == kErrorOK &&
                (theResult = this->SaveTagPointsChunk(inFile)) == kErrorOK && (theResult = this->SaveFrameChunk(inFile)) == kErrorOK) {
                for (unsigned long i = 0; i < this->mSubModelCount; i++)
                    this->mSubModels[i]->Save(inFile);
            } else
                inFile.SetPosition(thePosition, kFileOriginBegin, false);
        }
    } else
        theResult = kErrorNotOpen;
    if (theResult == kErrorNativeError)
        ESGetNativeError();
    return theResult;
}

// FUNCTION: LLE 0x1001e5fb
// Writes the "SSMF" tag and version 2.0; a short tag write still returns success
long GEModel::SaveHeaderChunk(FSFileSpec& inFile)
{
    unsigned long theWritten;
    long theResult = inFile.Write("SSMF", 4, theWritten);
    if (theResult != kErrorOK || theWritten != 4)
        return theResult;
    theResult = inFile.Write((unsigned short)2);       // major version
    if (theResult == kErrorOK)
        theResult = inFile.Write((unsigned short)0);   // minor version
    return theResult;
}

// FUNCTION: LLE 0x1001e651
// Writes the polygon, vertex, tag point, material, frame, sub-model and LOD counts
long GEModel::SaveCountsChunk(FSFileSpec& inFile)
{
    long theResult = inFile.Write(this->mPolygonCount);
    if (theResult != kErrorOK)
        return theResult;
    theResult = inFile.Write(this->mVertexCount);
    if (theResult != kErrorOK)
        return theResult;
    theResult = inFile.Write(this->mTagPointCount);
    if (theResult != kErrorOK)
        return theResult;
    theResult = inFile.Write(this->mMaterialCount);
    if (theResult != kErrorOK)
        return theResult;
    theResult = inFile.Write(this->mFrameCount);
    if (theResult != kErrorOK)
        return theResult;
    theResult = inFile.Write(this->mSubModelCount);
    if (theResult == kErrorOK)
        theResult = inFile.Write(this->mLODCount);
    return theResult;
}

// FUNCTION: LLE 0x1001e6ed
// Writes the model name and centre point
long GEModel::SaveMiscellaneousChunk(FSFileSpec& inFile)
{
    long theResult = this->mName.Save(inFile);
    if (theResult == kErrorOK)
        theResult = this->mCenterPoint.Save(inFile);
    return theResult;
}

// FUNCTION: LLE 0x1001e716
// Writes each LOD's polygon count and vertex indices; returns the first write error
long GEModel::SavePolygonsChunk(FSFileSpec& inFile)
{
    long theResult = kErrorOK;
    for (unsigned long i = 0; i < this->mLODCount; i++) {
        unsigned long theCount = this->mPolygonArrays[i]->GetSize();
        theResult = inFile.Write(theCount);
        if (theResult == kErrorOK) {
            for (unsigned long j = 0; j < this->mPolygonArrays[i]->GetSize() && theResult == kErrorOK; j++) {
                unsigned long theIndex1 = this->mPolygonArrays[i]->GetVertexIndex1(j);
                theResult = inFile.Write(theIndex1);
                if (theResult != kErrorOK)
                    continue;
                unsigned long theIndex2 = this->mPolygonArrays[i]->GetVertexIndex2(j);
                theResult = inFile.Write(theIndex2);
                if (theResult == kErrorOK) {
                    unsigned long theIndex3 = this->mPolygonArrays[i]->GetVertexIndex3(j);
                    theResult = inFile.Write(theIndex3);
                }
            }
        }
    }
    return theResult;
}

// FUNCTION: LLE 0x1001e83a
// Writes each material's name, texture index, per-LOD polygon ranges, vertex range and texture file name
long GEModel::SaveMaterialsChunk(FSFileSpec& inFile)
{
    long theResult = kErrorOK;
    for (unsigned long i = 0; i < this->mMaterialCount && theResult == kErrorOK; i++) {
        theResult = this->mMaterials[i]->GetName().Save(inFile);
        if (theResult != kErrorOK)
            continue;
        unsigned long theTextureIndex = this->mMaterials[i]->GetTextureIndex();
        theResult = inFile.Write(theTextureIndex);
        if (theResult != kErrorOK)
            continue;
        for (unsigned long j = 0; j < this->mLODCount && theResult == kErrorOK; j++) {
            unsigned long theStart = this->mMaterials[i]->GetPolygonStartingIndex(j);
            theResult = inFile.Write(theStart);
            if (theResult == kErrorOK) {
                unsigned long theLength = this->mMaterials[i]->GetPolygonRangeLength(j);
                theResult = inFile.Write(theLength);
            }
        }
        if (theResult != kErrorOK)
            continue;
        unsigned long theVertexStart = this->mMaterials[i]->GetVertexStartingIndex();
        theResult = inFile.Write(theVertexStart);
        if (theResult != kErrorOK)
            continue;
        unsigned long theVertexLength = this->mMaterials[i]->GetVertexRangeLength();
        theResult = inFile.Write(theVertexLength);
        if (theResult == kErrorOK)
            theResult = this->mTextureFilenames[i].Save(inFile);
    }
    return theResult;
}

// FUNCTION: LLE 0x1001e988
// Writes the tag point names; returns the first write error
long GEModel::SaveTagPointsChunk(FSFileSpec& inFile)
{
    long theResult = kErrorOK;
    for (unsigned long i = 0; i < this->mTagPointCount && theResult == kErrorOK; i++)
        theResult = this->mTagPointNames[i].Save(inFile);
    return theResult;
}

// FUNCTION: LLE 0x1001e9c2
// Writes each frame's radius, vertices, tag points, transform and bounding box; returns the first write error
long GEModel::SaveFrameChunk(FSFileSpec& inFile)
{
    long theResult = kErrorOK;
    for (unsigned long i = 0; i < this->mFrameCount; i++) {
        theResult = inFile.Write(this->mBoundingRadii[i]);
        if (theResult != kErrorOK)
            break;
        theResult = this->SaveVertexSubChunk(inFile, i);
        if (theResult != kErrorOK)
            break;
        theResult = this->SaveTagPointsSubChunk(inFile, i);
        if (theResult != kErrorOK)
            break;
        theResult = this->mInternalTransforms[i].Save(inFile);
        if (theResult != kErrorOK)
            break;
        theResult = this->mBoundingBoxes[i].Save(inFile);
        if (theResult != kErrorOK)
            break;
    }
    return theResult;
}

// FUNCTION: LLE 0x1001ea4e
// Writes a frame's vertices (point, normal, texture coordinate); returns the first write error
long GEModel::SaveVertexSubChunk(FSFileSpec& inFile, unsigned long inFrame)
{
    GEVector theNormal;
    GE3DPoint thePoint;
    GETextureCoordinate theCoordinate;
    long theResult = kErrorOK;
    for (unsigned long i = 0; i < this->mVertexCount; i++) {
        this->mVertexArrays[inFrame]->GetPoint(thePoint, i);
        theResult = thePoint.Save(inFile);
        if (theResult != kErrorOK)
            break;
        this->mVertexArrays[inFrame]->GetNormal(theNormal, i);
        theResult = theNormal.Save(inFile);
        if (theResult != kErrorOK)
            break;
        this->mVertexArrays[inFrame]->GetTextureCoordinate(theCoordinate, i);
        theResult = theCoordinate.Save(inFile);
        if (theResult != kErrorOK)
            break;
    }
    return theResult;
}

// FUNCTION: LLE 0x1001eb0b
// Writes a frame's tag point positions; returns the first write error
long GEModel::SaveTagPointsSubChunk(FSFileSpec& inFile, unsigned long inFrame)
{
    long theResult = kErrorOK;
    for (unsigned long i = 0; i < this->mTagPointCount; i++) {
        theResult = this->mFrameTagPoints[inFrame][i].Save(inFile);
        if (theResult != kErrorOK)
            break;
    }
    return theResult;
}

// FUNCTION: LLE 0x1001eb5a
// Does nothing (empty in the retail build)
void GEModel::SaveData(FSFileSpec& inFile)
{
}

// UList<GELODVertex*>/<GELODPolygon*>/<GELODMetaVertex*>::sNodePool init wrapper and atexit registrars (GELOD.h via
// GEModel.h; the $D ctors are GEBoundingBox.cpp's COMDATs 0x10014ed4/0x10014efb/0x10014f22)
// SYNTHETIC: LLE 0x1001eb5d
// SYNTHETIC: LLE 0x1001eb7b
// SYNTHETIC: LLE 0x1001eb87
// SYNTHETIC: LLE 0x1001eb93
