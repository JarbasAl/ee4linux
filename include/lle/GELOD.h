// GELODPolygon, GELODVertex, GELODMetaVertex (RECONSTRUCTION; file name inferred). The edge-collapse helpers of
// GEModel::ComputeLODReduction ('GEModel LOD.cpp', 0x1001d75e..0x1001e08a). Included at the end of GEModel.h: every
// GEModel.h includer carries the three UList<GELOD*>::sNodePool init pairs (GEModel.cpp, the GEModel part files,
// GEModelManager.cpp, GEPhysicalModel.cpp, GERasterizer.cpp, Graphics Engine.cpp), and GEBoundingBox.cpp emits these
// classes' inline members right after GEModel's (0x10011a4e..0x10011b84). Layouts from the implicit copy ctor /
// operator= COMDATs there; member names are provisional unless an export names them.
#ifndef LLE_GELOD_H
#define LLE_GELOD_H

#include "lle/UList.h"
#include "lle/GETextureCoordinate.h"

class GELODVertex;

class LLE_API GELODPolygon {                // sizeof 0x30 (operator new(0x30), ComputeLODReduction)
public:
    GELODPolygon(GELODVertex* inVertex1, GELODVertex* inVertex2, GELODVertex* inVertex3);   // 0x1001d75e
    // not inline: CollapseEdge/ComputeLODReduction call it before operator delete (the empty body is ICF-folded
    // into a bare ret, 0x1000c16a)
    ~GELODPolygon();
    float ComputeArea();                    // 0x1001d7c0
    void ComputeNormal();                   // 0x1001d85d
    GEVector& GetNormal() { return this->mNormal; }
    GELODVertex* GetVertex(unsigned long inIndex) { return this->mVertices[inIndex]; }
    bool HasVertex(GELODVertex* inVertex)
        { return this->mVertices[0] == inVertex || this->mVertices[1] == inVertex || this->mVertices[2] == inVertex; }
    void ReplaceVertex(GELODVertex* inOld, GELODVertex* inNew);   // 0x1001d919

    GELODVertex* mVertices[3];              // +0x00
    GETextureCoordinate mTextureCoordinates[3];   // +0x0c (provisional name; 8-byte elements: copy ctor rep movsd,
                                            // operator= element loop; the ctor zeroes them with fldz)
    GEVector mNormal;                       // +0x24 (ComputeNormal's CrossProduct target)
};

class LLE_API GELODVertex {                 // sizeof 0x20 (operator new(0x20), ComputeLODReduction)
public:
    GELODVertex(GE3DPoint& inPosition, unsigned long inIndex);   // 0x1001d958
    ~GELODVertex() {}                       // 0x10011afd: the two list dtors
    void AddAdjacentPolygon(GELODPolygon* inPolygon);   // 0x1001d9bd
    void AddAdjacentVertex(GELODVertex* inVertex);      // 0x1001d995
    UListNode<GELODPolygon*>* GetAdjacentPolygonListHead() { return this->mAdjacentPolygons.mHead; }
    UListNode<GELODVertex*>* GetAdjacentVertexListHead() { return this->mAdjacentVertices.mHead; }
    unsigned long GetIndex() { return this->mIndex; }
    GE3DPoint& GetPosition() { return this->mPosition; }
    void RemoveAdjacentPolygon(GELODPolygon* inPolygon);   // 0x1001d9e5
    void SetPosition(GE3DPoint& inPosition) { this->mPosition = inPosition; }
    void UpdateAdjacentVertexList();        // 0x1001da09

    UList<GELODVertex*> mAdjacentVertices;  // +0x00
    UList<GELODPolygon*> mAdjacentPolygons; // +0x08
    GE3DPoint mPosition;                    // +0x10
    unsigned long mIndex;                   // +0x1c
};

class LLE_API GELODMetaVertex {             // sizeof 0x18 (operator new(0x18), BuildMetaVertexList)
public:
    GELODMetaVertex() {}                    // 0x10011b48: the two list ctors only
    ~GELODMetaVertex() {}                   // 0x10011b58
    void AddAdjacentMetaVertex(GELODMetaVertex* inVertex);   // 0x1001da68
    void AddVertex(GELODVertex* inVertex);  // 0x1001da44
    static void BuildMetaVertexList(UList<GELODVertex*>& inVertices, UList<GELODMetaVertex*>& outMetaVertices);
    static void CollapseEdge(UList<GELODPolygon*>& ioPolygons, UList<GELODVertex*>& ioVertices,
                             UList<GELODMetaVertex*>& ioMetaVertices, GELODMetaVertex* inVertex);
    static void ComputeAllCollapseCosts(UList<GELODMetaVertex*>& ioMetaVertices);
    float ComputeEdgeCollapseCost(GELODMetaVertex* inVertex);
    static void ComputeMetaVertexAdjacencyInfo(UList<GELODMetaVertex*>& ioMetaVertices);
    UListNode<GELODMetaVertex*>* GetAdjacentVertexListHead() { return this->mAdjacentVertices.mHead; }
    bool GetIsOnBorder();
    GELODMetaVertex* GetMinEdgeToCollapse() { return this->mMinEdgeToCollapse; }
    static GELODMetaVertex* GetMinEdgeToCollapse(UList<GELODMetaVertex*>& inMetaVertices);
    float GetMinEdgeToCollapseCost() { return this->mMinEdgeToCollapseCost; }
    UListNode<GELODVertex*>* GetVertexListHead() { return this->mVertices.mHead; }

    UList<GELODVertex*> mVertices;          // +0x00
    UList<GELODMetaVertex*> mAdjacentVertices;   // +0x08
    float mMinEdgeToCollapseCost;           // +0x10
    GELODMetaVertex* mMinEdgeToCollapse;    // +0x14
};

#endif
