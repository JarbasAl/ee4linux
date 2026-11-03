// EEAIAStarMachine<S, G, M>::Run and its flood-fill steps (COMDATs 0x1ceb1..0x1d6b1 for EEAIChokePoint's machine, twins
// 0x268be.. for EEAIContinentManager's four, EEPathManager's 0xaa4e5..). Moved from EEAIChokePoint.cpp so every
// instantiating unit emits them. Inferred header: they belong in ee/EEAIAStarMachine.h once
// the storages share one AddOpen/RemoveBestOpen/AddClosed API.
#ifndef EE_EEAIASTARMACHINESTEPS_H
#define EE_EEAIASTARMACHINESTEPS_H
#include "ee/EEAIKindIDs.h"
template <class S, class G, class M>
void EEAIAStarMachine<S, G, M>::Run()
{
    if (!this->mFound) {
        this->mNumRuns = 0;
        this->mMapWidth = this->mMap->mSize;
        this->mMapHeight = this->mMap->mSize;
        this->mRevs = 0;
        Init(this->mMap->mSize, this->mMap->mSize);
        this->mGoal.Attach(this->mMap, this, this);
        this->mGoal.Prepare();
        this->Start();
        this->Expand();
    } else
        this->Expand();
}

template <class S, class G, class M>
bool EEAIAStarMachine<S, G, M>::Start()
{
    bool theResult = this->TestTile(this->mGoal.mStartTile.GetX(), this->mGoal.mStartTile.GetY());
    const U2DPoint<long>& theGoal = this->mGoal.mGoalTile;
    this->mTile = theGoal;
    mCurrentNode = NewNode(this->mGoal.mStartTile.GetX(), this->mGoal.mStartTile.GetY());
    mCurrentNode->mPathCost = 0;
    mCurrentNode->mEstimate = this->mGoal.Estimate(this->mGoal.mStartTile, this->mTile);
    mCurrentNode->mCost = mCurrentNode->mEstimate + mCurrentNode->mPathCost;
    AddOpen(mCurrentNode);
    return theResult;
}

template <class S, class G, class M>
void EEAIAStarMachine<S, G, M>::Expand()
{
    this->mSteps = 0;
    this->mNumRuns++;
    this->mRevs++;
    for (;;) {
        mCurrentNode = RemoveBestOpen();
        if (this->mGoal.IsGoalNode(mCurrentNode)) {
            this->mFound = false;
            break;
        }
        long theX = mCurrentNode->mX;
        long theY = mCurrentNode->mY;
        long theLeft = theX - 1;
        long theBottom = theY - 1;
        long theRight = theX + 1;
        long theTop = theY + 1;
        long theParentX;
        long theParentY;
        if (mCurrentNode->mParent) {
            theParentX = mCurrentNode->mParent->mX;
            theParentY = mCurrentNode->mParent->mY;
        } else {
            theParentX = 0;
            theParentY = 0;
        }
        bool theBelow = this->TestTile(theX, theBottom);
        bool theLeftOK = this->TestTile(theLeft, theY);
        bool theRightOK = this->TestTile(theRight, theY);
        bool theAbove = this->TestTile(theX, theTop);
        if (theBelow && (theParentX != theX || theParentY != theBottom))
            this->AddNode(mCurrentNode, theX, theBottom);
        if (theLeftOK && (theParentX != theLeft || theParentY != theY))
            this->AddNode(mCurrentNode, theLeft, theY);
        if (theRightOK && (theParentX != theRight || theParentY != theY))
            this->AddNode(mCurrentNode, theRight, theY);
        if (theAbove && (theParentX != theX || theParentY != theTop))
            this->AddNode(mCurrentNode, theX, theTop);
        if (this->mCheckCorners) {
            if ((theLeftOK || theBelow) && (theParentX != theLeft || theParentY != theBottom) &&
                this->TestTile(theLeft, theBottom))
                this->AddNode(mCurrentNode, theLeft, theBottom);
            if ((theBelow || theRightOK) && (theParentX != theRight || theParentY != theBottom) &&
                this->TestTile(theRight, theBottom))
                this->AddNode(mCurrentNode, theRight, theBottom);
            if ((theRightOK || theAbove) && (theParentX != theRight || theParentY != theTop) &&
                this->TestTile(theRight, theTop))
                this->AddNode(mCurrentNode, theRight, theTop);
            if ((theAbove || theLeftOK) && (theParentX != theLeft || theParentY != theTop) &&
                this->TestTile(theLeft, theTop))
                this->AddNode(mCurrentNode, theLeft, theTop);
        } else {
            if (this->TestTile(theLeft, theBottom) && (theParentX != theLeft || theParentY != theBottom))
                this->AddNode(mCurrentNode, theLeft, theBottom);
            if (this->TestTile(theRight, theBottom) && (theParentX != theRight || theParentY != theBottom))
                this->AddNode(mCurrentNode, theRight, theBottom);
            if (this->TestTile(theRight, theTop) && (theParentX != theRight || theParentY != theTop))
                this->AddNode(mCurrentNode, theRight, theTop);
            if (this->TestTile(theLeft, theTop) && (theParentX != theLeft || theParentY != theTop))
                this->AddNode(mCurrentNode, theLeft, theTop);
        }
        AddClosed(mCurrentNode);
        if (this->mGoal.IsStepLimit(this->mSteps)) {
            mCurrentNode = 0;
            this->mFound = true;
            break;
        }
        if (this->mGoal.IsRevLimit(this->mRevs)) {
            mCurrentNode = 0;
            this->mFound = false;
            break;
        }
        this->mSteps++;
        this->mRevs++;
    }
    if (!this->mFound)
        this->mGoal.SearchDone(this);
    if (this->mGoal.mSearchFailed) {
        if (mCurrentNode) {
            AddClosed(mCurrentNode);
            mCurrentNode = 0;
        }
        this->Restart();
    } else if (!this->mFound && mCurrentNode)
        mPath = mCurrentNode;
    else
        mCurrentNode = 0;
}

template <class S, class G, class M>
bool EEAIAStarMachine<S, G, M>::Restart()
{
    const bool& theAtGoal = this->mGoal.mAcceptNearest;
    bool theDone = theAtGoal;
    mPath = 0;
    if (theAtGoal) {
        EEAIAStarNode* theNode = this->NextNode();
        if (theNode) {
            mPath = theNode;
            mCurrentNode = theNode;
            this->mTile.mX = theNode->mX;
            this->mTile.mY = theNode->mY;
        } else {
            mCurrentNode = NewNode(this->mGoal.mStartTile.GetX(), this->mGoal.mStartTile.GetY());
            mCurrentNode->mPathCost = 0;
            mCurrentNode->mEstimate = 1;
            mCurrentNode->mCost = 1;
            mPath = mCurrentNode;
        }
        this->mFound = false;
        theDone = true;
    }
    while (!theDone) {
        EEAIAStarNode* theNode = this->NextNode();
        if (theNode) {
            theDone = this->mGoal.AcceptTile(theNode->mX, theNode->mY, true);
            if (theDone) {
                mPath = theNode;
                mCurrentNode = theNode;
                this->mTile.SetX(theNode->mX);
                this->mTile.SetY(theNode->mY);
                this->mFound = false;
            } else
                ReleaseNode(theNode);
        } else {
            mCurrentNode = NewNode(this->mGoal.mStartTile.GetX(), this->mGoal.mStartTile.GetY());
            mCurrentNode->mPathCost = 0;
            mCurrentNode->mEstimate = 1;
            mCurrentNode->mCost = 1;
            this->mFound = false;
            mPath = mCurrentNode;
            theDone = true;
        }
    }
    return theDone;
}

template <class S, class G, class M>
bool EEAIAStarMachine<S, G, M>::TestTile(unsigned long inX, unsigned long inY)
{
    bool theResult = false;
    if (inX < this->mMapWidth && inY < this->mMapHeight) {
        unsigned char* theFlag = GetTileFlags(inX, inY);
        if (*theFlag & kAStarTileAccepted || *theFlag & kAStarTileRejected)
            theResult = *theFlag & kAStarTileAccepted;
        else {
            theResult = this->mGoal.AcceptTile(inX, inY, false);
            if (!theResult)
                *theFlag |= kAStarTileRejected;
            else
                *theFlag |= kAStarTileAccepted;
        }
        ExtendBounds(inX, inY);
    }
    return theResult;
}

template <class S, class G, class M>
EEAIAStarNode* EEAIAStarMachine<S, G, M>::NextNode()
{
    EEAIAStarNode* theOpen = RemoveNearestOpen(this->mTile);
    EEAIAStarNode* theClosed = RemoveNearestClosed(this->mTile);
    if (theOpen) {
        if (theClosed) {
            if (theOpen->mCost <= theClosed->mCost)
                AddClosed(theClosed);
            else {
                AddOpen(theOpen);
                theOpen = theClosed;
            }
        }
    } else
        theOpen = theClosed;
    return theOpen;
}

template <class S, class G, class M>
void EEAIAStarMachine<S, G, M>::AddNode(EEAIAStarNode* inParent, long inX, long inY)
{
    unsigned long theCost = this->mGoal.GetCost(inParent->mX, inParent->mY, inX, inY) + inParent->mPathCost;
    EEAIAStarNode* theOpen = FindOpen(inX, inY);
    EEAIAStarNode* theClosed = FindClosed(inX, inY);
    if (theOpen && !this->mGoal.IsCheaper(theCost, theOpen->mPathCost))
        return;
    if (theClosed) {
        if (!this->mGoal.IsCheaper(theCost, theClosed->mPathCost))
            return;
        RemoveClosed(theClosed);
        ReleaseNode(theClosed);
    }
    if (!theOpen) {
        EEAIAStarNode* theNode = NewNode(inX, inY);
        theNode->mParent = inParent;
        theNode->mPathCost = theCost;
        theNode->mEstimate = this->mGoal.Estimate(U2DPoint<long>(theNode->mX, theNode->mY), this->mTile);
        theNode->mCost = theNode->mEstimate + theCost;
        AddOpen(theNode);
    } else {
        theOpen->mPathCost = theCost;
        theOpen->mParent = inParent;
        if (theOpen->mCost != theOpen->mEstimate + theCost) {
            theOpen->mCost = theOpen->mEstimate + theCost;
            UpdateOpen(theOpen);
        }
    }
}

#endif
