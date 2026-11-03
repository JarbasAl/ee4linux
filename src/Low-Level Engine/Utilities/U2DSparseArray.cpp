// C:\EEX\Low-Level Engine\Utilities\U2DSparseArray.cpp (file name inferred: sorts between U2DFractal.cpp and
// UDataCompression.cpp)
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x100549b6..0x10056369: the U2DPoint key init chains first, then the members in class order (Node alphabetical;
// the list header's ctor/dtor/GetIsEmpty; the container's ctors, dtor and ??_E; the rest of the list header and of
// the container, alphabetical), then the template COMDATs: vector<U2DPoint<long> >, U2DComputeCenterPoint, UList
// insert/remove/clear, the two UMemoryPool instantiations and the two UList sNodePool statics' init pairs.
// UDataCompression.cpp's ctor starts at 0x10056369.
#include <windows.h>
#include <algorithm>
#include <typeinfo>

#include "lle/LowLevelEngine.h"
#include "lle/U2DPoint.h"
#include "lle/U2DSparseArray.h"

// Header-inline members emitted here, at the point lle/U2DSparseArray.h is included (after the U2DPoint key
// chains): the node's members alphabetical, then the list header's ctor, dtor and GetIsEmpty
// FUNCTION: LLE 0x10054a16
// FUNCTION: LLE 0x10054a20
// FUNCTION: LLE 0x10054a3d
// FUNCTION: LLE 0x10054a4a
// FUNCTION: LLE 0x10054a61
// FUNCTION: LLE 0x10054a6e
// FUNCTION: LLE 0x10054a8a
// FUNCTION: LLE 0x10054aae
// FUNCTION: LLE 0x10054ae4
// FUNCTION: LLE 0x10054aed
// FUNCTION: LLE 0x10054b0a
// FUNCTION: LLE 0x10054b28
// FUNCTION: LLE 0x10054b2f
// FUNCTION: LLE 0x10054b41
// FUNCTION: LLE 0x10054b55
// FUNCTION: LLE 0x10054b64
// FUNCTION: LLE 0x10054b73
// FUNCTION: LLE 0x10054b81
// FUNCTION: LLE 0x10054b8f
// FUNCTION: LLE 0x10054b9c
// FUNCTION: LLE 0x10054ba1

// ---- U2DSparseArrayPointContainer (1)

bool U2DSparseArrayPointContainer::sPoolSizeSet;

// FUNCTION: LLE 0x10054ba9
// Creates an empty sparse point set
U2DSparseArrayPointContainer::U2DSparseArrayPointContainer()
{
    this->mCount = 0;
    this->mOffset = 0;
}

// FUNCTION: LLE 0x10054bd0
// ~U2DSparseArrayPointContainer: inline in lle/U2DSparseArray.h (the exported COMDAT lands here)

// FUNCTION: LLE 0x10054be8
// Copies a sparse point set (the column pointers are shared, not duplicated)
U2DSparseArrayPointContainer::U2DSparseArrayPointContainer(const U2DSparseArrayPointContainer& inContainer)
    : mArray(inContainer.mArray), mOffset(inContainer.mOffset), mCount(inContainer.mCount)
{
}

// ---- U2DSparseArrayPointContainerListHeader (2)

// FUNCTION: LLE 0x10054c98
// Appends the column's points (every inStep-th y of each run) to the list
void U2DSparseArrayPointContainerListHeader::BuildPointList(long inX, std::vector<U2DPoint<long> >& outList,
                                                            long inStep)
{
    for (UListNode<U2DSparseArrayPointContainerNode>* theNode = this->mList.mHead; theNode; theNode = theNode->mNext) {
        for (long y = theNode->mData.mStart; y <= theNode->mData.mEnd; y += inStep)
            outList.push_back(U2DPoint<long>(inX, y));
    }
}

// FUNCTION: LLE 0x10054cd3
// Returns whether the column holds the point's y (stops at the first run starting past it)
bool U2DSparseArrayPointContainerListHeader::Contains(const U2DPoint<long>& inPoint) const
{
    long y = inPoint.mY;
    bool theResult = false;
    UListNode<U2DSparseArrayPointContainerNode>* theNode = this->mList.mHead;
    while (theNode) {
        theResult = theNode->mData.Contains(y);
        if (theResult || theNode->mData.mStart > y)
            break;
        theNode = theNode->mNext;
    }
    return theResult;
}

// FUNCTION: LLE 0x10054cff
// Returns the column's point at the given index across its runs (default point when out of range)
U2DPoint<long> U2DSparseArrayPointContainerListHeader::GetArrayElement(long inX, long inIndex) const
{
    U2DPoint<long> theResult;
    long theCount = 0;
    bool theFound = false;
    UListNode<U2DSparseArrayPointContainerNode>* theNode = this->mList.mHead;
    while (theNode) {
        long theNext = theCount + theNode->mData.Size();
        if (inIndex < theNext) {
            theFound = true;
            break;
        }
        theNode = theNode->mNext;
        theCount = theNext;
    }
    if (theFound) {
        theResult.SetX(inX);
        theResult.SetY(theNode->mData.mStart - theCount + inIndex);
    }
    return theResult;
}

// FUNCTION: LLE 0x10054d3d
// Appends the column's lowest and highest point to the list
void U2DSparseArrayPointContainerListHeader::GetOnlyExtremeEdgePoints(long inX, std::vector<U2DPoint<long> >& outList)
{
    UListNode<U2DSparseArrayPointContainerNode>* theHead = this->mList.mHead;
    if (!theHead)
        return;
    outList.push_back(U2DPoint<long>(inX, theHead->mData.mStart));
    UListNode<U2DSparseArrayPointContainerNode>* theLast = theHead->mNext;
    if (theLast) {
        while (theLast->mNext)
            theLast = theLast->mNext;
        outList.push_back(U2DPoint<long>(inX, theLast->mData.mEnd));
    } else
        outList.push_back(U2DPoint<long>(inX, theHead->mData.mEnd));
}

long U2DSparseArrayPointContainerListHeader::GetRandomIntegerBetween(URandom& inRandom, long inMinimum,
                                                                     long inMaximum) const
{
    long theLimit = inMaximum + 1;
    long theResult = (long)(inRandom.Generate() * (theLimit - inMinimum) + inMinimum);
    if (theResult == theLimit)
        theResult = inMaximum;
    return theResult;
}

// FUNCTION: LLE 0x10054d9c
// Returns a random integer from inMinimum to inMaximum inclusive
long U2DSparseArrayPointContainerListHeader::GetRandomNumberBetween(URandom& inRandom, long inMinimum,
                                                                    long inMaximum) const
{
    long theLimit = inMaximum + 1;
    long theResult = (long)(inRandom.Generate() * (theLimit - inMinimum) + inMinimum);
    if (theResult == theLimit)
        theResult = inMaximum;
    return theResult;
}

// FUNCTION: LLE 0x10054dce
// Returns a random point of the column: a random run, then a random y within it
U2DPoint<long> U2DSparseArrayPointContainerListHeader::GetRandomPoint(long inX, URandom& inRandom)
{
    UListNode<U2DSparseArrayPointContainerNode>* theHead = this->mList.mHead;
    long theIndex = this->GetRandomNumberBetween(inRandom, 0, this->mList.mCount - 1);
    UListNode<U2DSparseArrayPointContainerNode>* theNode = theHead;
    for (long i = 0; theNode && i < theIndex; i++)
        theNode = theNode->mNext;
    return U2DPoint<long>(inX, this->GetRandomNumberBetween(inRandom, theNode->mData.GetStart(), theNode->mData.GetEnd()));
}

// FUNCTION: LLE 0x10054e1e
// Adds a y to the column, extending and merging neighbouring runs or inserting a new run in order
void U2DSparseArrayPointContainerListHeader::Insert(const U2DPoint<long>& inPoint)
{
    long y = inPoint.mY;
    UListNode<U2DSparseArrayPointContainerNode>* thePrevious = NULL;
    UListNode<U2DSparseArrayPointContainerNode>* theNode = this->mList.mHead;
    bool theInserted = false;
    while (theNode) {
        if (theNode->mData.ShouldAddNumber(y)) {
            theNode->mData.Add(y);
            if (thePrevious && thePrevious->mData.ShouldMerge(theNode->mData)) {
                thePrevious->mData.Merge(theNode->mData);
                this->mList.Remove(theNode);
                theNode = thePrevious;
            }
            UListNode<U2DSparseArrayPointContainerNode>* theNext = theNode->mNext;
            if (theNext && theNode->mData.ShouldMerge(theNext->mData)) {
                theNode->mData.Merge(theNext->mData);
                this->mList.Remove(theNode->mNext);
            }
            theInserted = true;
            break;
        }
        if (theNode->mData.mStart > y) {
            this->mList.InsertAfter(thePrevious, U2DSparseArrayPointContainerNode(y, y));
            theInserted = true;
            break;
        }
        thePrevious = theNode;
        theNode = theNode->mNext;
    }
    if (!theInserted)
        this->mList.InsertAfter(thePrevious, U2DSparseArrayPointContainerNode(y, y));
    this->mSize++;
}

// FUNCTION: LLE 0x10054ef3
// Removes a y from the column, shrinking, splitting or deleting its run; false when it was not there
bool U2DSparseArrayPointContainerListHeader::Remove(const U2DPoint<long>& inPoint)
{
    long y = inPoint.mY;
    bool theResult = false;
    UListNode<U2DSparseArrayPointContainerNode>* theNode = this->mList.mHead;
    while (theNode) {
        if (theNode->mData.Contains(y)) {
            theResult = true;
            if (theNode->mData.Remove(y)) {
                if (theNode->mData.GetIsEmpty())
                    this->mList.Remove(theNode);
            } else {
                this->mList.InsertBefore(theNode, U2DSparseArrayPointContainerNode(theNode->mData.mStart, y - 1));
                theNode->mData.mStart = y + 1;
            }
            this->mSize--;
            break;
        }
        theNode = theNode->mNext;
    }
    return theResult;
}

// ---- U2DSparseArrayPointContainer (2)

// FUNCTION: LLE 0x10054f61
// Rebuilds the list from every column's points (every inStep-th y), last column first
void U2DSparseArrayPointContainer::BuildPointList(std::vector<U2DPoint<long> >& outList, long inStep)
{
    outList.clear();
    outList.reserve(this->mCount);
    for (long i = this->mArray.size() - 1; i >= 0; i--) {
        if (this->mArray[i])
            this->mArray[i]->BuildPointList(this->mOffset + i, outList, inStep);
    }
}

// FUNCTION: LLE 0x10054fb9
// Deletes every column and empties the set
void U2DSparseArrayPointContainer::Clear()
{
    for (long i = this->mArray.size() - 1; i >= 0; i--) {
        U2DSparseArrayPointContainerListHeader* theList = this->mArray[i];
        if (theList)
            delete theList;
    }
    this->mArray.clear();
    this->mOffset = 0;
    this->mCount = 0;
}

// FUNCTION: LLE 0x1005500b
// Returns the center point of the set's points (optionally one that is in the set)
U2DPoint<long> U2DSparseArrayPointContainer::ComputeCenterPoint(bool inMustBeInList)
{
    std::vector<U2DPoint<long> > thePoints;
    this->BuildPointList(thePoints, 1);
    U2DPoint<long> theResult;
    theResult = U2DComputeCenterPoint(thePoints, inMustBeInList);
    return theResult;
}

// FUNCTION: LLE 0x10055060
// Returns whether the set holds the point
bool U2DSparseArrayPointContainer::Contains(const U2DPoint<long>& inPoint) const
{
    long theIndex = inPoint.mX - this->mOffset;
    bool theResult = false;
    if (theIndex >= 0 && theIndex < (long)this->mArray.size() && this->mArray[theIndex] != NULL)
        theResult = this->mArray[theIndex]->Contains(inPoint);
    return theResult;
}

// FUNCTION: LLE 0x100550a4
// Returns the number of runs over all columns
long U2DSparseArrayPointContainer::CountContainerNodes()
{
    long theCount = 0;
    for (long i = this->mArray.size() - 1; i >= 0; i--) {
        if (this->mArray[i])
            theCount += this->mArray[i]->CountContainerNodes();
    }
    return theCount;
}

// FUNCTION: LLE 0x100550d9
// Rebuilds the set from a point list, sizing the column array from the list's x range
void U2DSparseArrayPointContainer::Create(const std::vector<U2DPoint<long> >& inPoints)
{
    U2DPoint<long> theMinimum;
    U2DPoint<long> theMaximum;
    U2DSparseArrayPointContainer::SetPoolSize(2000, 65, 1);
    this->Clear();
    if (!this->GetArrayBoundaryPoints(inPoints, theMinimum, theMaximum))
        return;
    this->mOffset = theMinimum.mX;
    this->mArray.resize(theMaximum.mX - theMinimum.mX + 1, NULL);
    for (long i = inPoints.size() - 1; i >= 0; i--)
        this->Insert(inPoints[i]);
}

// FUNCTION: LLE 0x10055169
// Prints the run node pool statistics
void U2DSparseArrayPointContainer::DumpPoolStats()
{
    UList<UListNode<U2DSparseArrayPointContainerNode> >::sNodePool.DumpStats(false);
}

// FUNCTION: LLE 0x10055176
// Grows the column array to the left or right so that it covers the point's x
void U2DSparseArrayPointContainer::ExpandArrayForPoint(const U2DPoint<long>& inPoint)
{
    std::vector<U2DSparseArrayPointContainerListHeader*> theOldArray;
    long theOffset = this->mOffset;
    long theShift = theOffset - inPoint.GetX();
    if (this->mArray.size() > 0) {
        if (theShift > 0) {
            theOldArray = this->mArray;
            this->mArray.clear();
            this->mArray.resize(theOldArray.size() + theShift, NULL);
            for (long i = theOldArray.size() - 1; i >= 0; i--)
                this->mArray[i + theShift] = theOldArray[i];
            this->mOffset = inPoint.GetX();
        } else if (theShift < 0) {
            unsigned long theCount = inPoint.GetX() - theOffset - this->mArray.size() + 1;
            this->mArray.insert(this->mArray.end(), theCount, NULL);
        }
    } else {
        this->mOffset = inPoint.GetX();
        this->mArray.resize(1, NULL);
    }
}

// FUNCTION: LLE 0x100552af
// Returns the points with the lowest and the highest x of a list; false (both 0,0) when it is empty
bool U2DSparseArrayPointContainer::GetArrayBoundaryPoints(const std::vector<U2DPoint<long> >& inPoints,
                                                          U2DPoint<long>& outMinimum, U2DPoint<long>& outMaximum)
{
    bool theResult = false;
    outMinimum = U2DPoint<long>(0, 0);
    outMaximum = U2DPoint<long>(0, 0);
    if (inPoints.size() > 0) {
        theResult = true;
        outMinimum = inPoints.back();
        outMaximum = inPoints.back();
        for (long i = inPoints.size() - 2; i >= 0; i--) {
            if (inPoints[i].GetX() < outMinimum.GetX())
                outMinimum = inPoints[i];
            if (inPoints[i].GetX() > outMaximum.GetX())
                outMaximum = inPoints[i];
        }
    }
    return theResult;
}

// FUNCTION: LLE 0x100553c7
// Returns the point itself if it is in the set, else the nearest point of the set (squared distance)
U2DPoint<long> U2DSparseArrayPointContainer::GetClosestPoint(const U2DPoint<long>& inPoint)
{
    std::vector<U2DPoint<long> > thePoints;
    U2DPoint<long> theResult = inPoint;
    if (this->mCount > 0 && !this->Contains(inPoint)) {
        this->BuildPointList(thePoints, 1);
        theResult = thePoints.back();
        long theBest = inPoint.DistanceToSquared(theResult);
        for (long i = thePoints.size() - 2; i >= 0; i--) {
            long theDistance = inPoint.DistanceToSquared(thePoints[i]);
            if (theDistance <= theBest) {
                theResult = thePoints[i];
                theBest = theDistance;
            }
        }
    }
    return theResult;
}

long U2DSparseArrayPointContainer::GetRandomIntegerBetween(URandom& inRandom, long inMinimum, long inMaximum) const
{
    long theLimit = inMaximum + 1;
    long theResult = (long)(inRandom.Generate() * (theLimit - inMinimum) + inMinimum);
    if (theResult == theLimit)
        theResult = inMaximum;
    return theResult;
}

// FUNCTION: LLE 0x1005549e
// Returns a random point (16 tries at a random column; then the first point; default point when empty)
U2DPoint<long> U2DSparseArrayPointContainer::GetRandomPoint(URandom& inRandom)
{
    U2DPoint<long> theResult;
    if (this->mArray.size() > 0) {
        long theTries = 15;
        while (theTries >= 0) {
            long theIndex = this->GetRandomIntegerBetween(inRandom, 0, this->mArray.size() - 1);
            if (this->mArray[theIndex]) {
                theResult = this->mArray[theIndex]->GetRandomPoint(this->mOffset + theIndex, inRandom);
                break;
            }
            theTries--;
        }
        if (theTries < 0)
            theResult = (*this)[0];
    }
    return theResult;
}

// FUNCTION: LLE 0x1005553a
// Rebuilds the list from the lowest and highest point of each column
void U2DSparseArrayPointContainer::GetOnlyExtremeEdgePoints(std::vector<U2DPoint<long> >& outList)
{
    outList.clear();
    for (long i = this->mArray.size() - 1; i >= 0; i--) {
        if (this->mArray[i])
            this->mArray[i]->GetOnlyExtremeEdgePoints(this->mOffset + i, outList);
    }
}

// FUNCTION: LLE 0x10055582
// Adds a point, growing the column array; a point left of the array with x <= 0 is silently ignored
void U2DSparseArrayPointContainer::Insert(const U2DPoint<long>& inPoint)
{
    long theIndex = inPoint.mX - this->mOffset;
    if (theIndex < 0 && inPoint.mX <= 0)
        return;
    if (theIndex < 0 || theIndex >= (long)this->mArray.size()) {
        this->ExpandArrayForPoint(inPoint);
        this->InsertIntoBucket(inPoint.mX - this->mOffset, inPoint);
    } else
        this->InsertIntoBucket(theIndex, inPoint);
    this->mCount++;
}

// FUNCTION: LLE 0x100555d0
// Adds every point of the list
void U2DSparseArrayPointContainer::Insert(const std::vector<U2DPoint<long> >& inPoints)
{
    for (long i = inPoints.size() - 1; i >= 0; i--)
        this->Insert(inPoints[i]);
}

// FUNCTION: LLE 0x1005560a
// Adds the point to its column (creating it); undoes the caller's count increment for a duplicate
void U2DSparseArrayPointContainer::InsertIntoBucket(long inIndex, const U2DPoint<long>& inPoint)
{
    bool theIsNew = true;
    if (this->mArray[inIndex] == NULL)
        this->mArray[inIndex] = new U2DSparseArrayPointContainerListHeader;
    else
        theIsNew = !this->mArray[inIndex]->Contains(inPoint);
    if (theIsNew)
        this->mArray[inIndex]->Insert(inPoint);
    if (!theIsNew)
        this->mCount--;
}

// FUNCTION: LLE 0x10055677
// Removes a point, deleting its column when it becomes empty; false when it was not there
bool U2DSparseArrayPointContainer::Remove(const U2DPoint<long>& inPoint)
{
    long theIndex = inPoint.mX - this->mOffset;
    bool theResult = false;
    if (theIndex >= 0 && theIndex < (long)this->mArray.size() && this->mArray[theIndex] != NULL) {
        theResult = this->mArray[theIndex]->Remove(inPoint);
        if (theResult) {
            this->mCount--;
            U2DSparseArrayPointContainerListHeader* theList = this->mArray[theIndex];
            if (theList->GetIsEmpty()) {
                delete theList;
                this->mArray[theIndex] = NULL;
            }
        }
    }
    return theResult;
}

// FUNCTION: LLE 0x100556f3
// Sizes the shared run node pool once; returns false when it was already sized
bool U2DSparseArrayPointContainer::SetPoolSize(unsigned long inChunkSize, unsigned long inMaxChunks,
                                               unsigned long inChunkCount)
{
    bool theResult = false;
    if (!U2DSparseArrayPointContainer::sPoolSizeSet) {
        UList<UListNode<U2DSparseArrayPointContainerNode> >::sNodePool.SetSize(inChunkSize, inMaxChunks,
                                                                                inChunkCount);
        U2DSparseArrayPointContainer::sPoolSizeSet = true;
        theResult = true;
    }
    return theResult;
}

// FUNCTION: LLE 0x1005571d
// Returns the point at the given index over all columns in order (default point when out of range)
U2DPoint<long> U2DSparseArrayPointContainer::operator[](long inIndex) const
{
    U2DPoint<long> theResult;
    long theCount = 0;
    for (unsigned long i = 0; i < this->mArray.size(); i++) {
        if (!this->mArray[i])
            continue;
        if (inIndex < theCount + this->mArray[i]->GetSize()) {
            theResult = this->mArray[i]->GetArrayElement(this->mOffset + i, inIndex - theCount);
            break;
        }
        theCount += this->mArray[i]->GetSize();
    }
    return theResult;
}

// Compiler-generated: the U2DPoint key init chains (names in analysis/lle/names.txt)
// SYNTHETIC: LLE 0x100549b6
// SYNTHETIC: LLE 0x100549c0
// SYNTHETIC: LLE 0x100549d0
// SYNTHETIC: LLE 0x100549dc
// SYNTHETIC: LLE 0x100549e6
// SYNTHETIC: LLE 0x100549f0
// SYNTHETIC: LLE 0x10054a00
// SYNTHETIC: LLE 0x10054a0c
// ??_E of the container (vtable slot 0)
// SYNTHETIC: LLE 0x10054c15
// Template COMDATs (names in analysis/lle/names.txt)
// SYNTHETIC: LLE 0x100557b4
// SYNTHETIC: LLE 0x10055866
// SYNTHETIC: LLE 0x1005586d
// SYNTHETIC: LLE 0x10055899
// SYNTHETIC: LLE 0x100558b6
// SYNTHETIC: LLE 0x10055900
// SYNTHETIC: LLE 0x10055a9c
// SYNTHETIC: LLE 0x10055aa3
// SYNTHETIC: LLE 0x10055afd
// SYNTHETIC: LLE 0x10055b70
// SYNTHETIC: LLE 0x10055baa
// SYNTHETIC: LLE 0x10055c0a
// SYNTHETIC: LLE 0x10055d17
// SYNTHETIC: LLE 0x10055d3c
// SYNTHETIC: LLE 0x10055d46
// SYNTHETIC: LLE 0x10055d61
// SYNTHETIC: LLE 0x10055d6d
// SYNTHETIC: LLE 0x10055d92
// SYNTHETIC: LLE 0x10055db4
// SYNTHETIC: LLE 0x10055dee
// SYNTHETIC: LLE 0x10055e70
// SYNTHETIC: LLE 0x10055e8b
// SYNTHETIC: LLE 0x10055ea1
// SYNTHETIC: LLE 0x10055efb
// SYNTHETIC: LLE 0x10055fe3
// SYNTHETIC: LLE 0x10056008
// SYNTHETIC: LLE 0x1005603d
// SYNTHETIC: LLE 0x10056047
// SYNTHETIC: LLE 0x10056062
// SYNTHETIC: LLE 0x1005606e
// SYNTHETIC: LLE 0x10056093
// SYNTHETIC: LLE 0x100560b5
// SYNTHETIC: LLE 0x10056119
// SYNTHETIC: LLE 0x10056135
// SYNTHETIC: LLE 0x1005616f
// SYNTHETIC: LLE 0x100561e2
// SYNTHETIC: LLE 0x1005625c
