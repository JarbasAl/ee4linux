// C:\EEX\Empire Earth\AI Thread\EEAIAStarPathAreaStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarPathAreaStorage, vtable 0x439fdc). The path-area search's node storage: 0x19072..0x190bf (the previous
// unit's COMDATs end at 0x19072, EEAIAStarPathfindingGoal's code starts at 0x190bf). ctor, ??_G (after its first
// user), dtor, then Virtual10 (ICF-folded into EEAIAStarPathSectorStorage's identical 0x1b829, so no address of its own;
// the path sectors read this storage's mP60 through EEAIAStarPathSectorGoal::mPathAreaStorage).
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"
#include "ee/EEAIAStarTerrainStorage.h"
#include "ee/EEAIAStarPathAreaStorage.h"

// FUNCTION: EXE 0x419072
// Creates the path-area search storage with no per-tile area id grid yet
EEAIAStarPathAreaStorage::EEAIAStarPathAreaStorage()
    : mPathAreaIDs(NULL)
{
}

// SYNTHETIC: EXE 0x419088
// (the scalar deleting destructor)

// FUNCTION: EXE 0x4190a4
// Destroys the storage, freeing its per-tile path area ids
EEAIAStarPathAreaStorage::~EEAIAStarPathAreaStorage()
{
    delete[] this->mPathAreaIDs;
}

// (no address: ICF-folded into EEAIAStarPathSectorStorage::PrepareGrid 0x1b829, the same code)
// Sizes the node arrays and the per-tile path-area ids for an inWidth x inHeight search (reallocating on a size
// change) and clears the node and tile flags, only the last search's dirty bounds plus one when it can
void EEAIAStarPathAreaStorage::PrepareGrid(long inWidth, long inHeight)
{
    bool theResized = false;
    long theOldWidth = this->mWidth;
    long theOldHeight = this->mHeight;
    this->mWidth = inWidth;
    this->mHeight = inHeight;
    if (this->mPathAreaIDs == NULL || this->mTileFlags == NULL || this->mNodes == NULL || theOldWidth != inWidth || theOldHeight != inHeight) {
        delete[] this->mPathAreaIDs;
        this->mPathAreaIDs = new unsigned short[this->mHeight * this->mWidth];
        memset(this->mPathAreaIDs, 0, this->mHeight * this->mWidth * sizeof(unsigned short));
        delete[] this->mTileFlags;
        this->mTileFlags = new unsigned char[this->mHeight * this->mWidth];
        delete[] this->mNodes;
        this->mNodes = new EEAIAStarNode*[this->mHeight * this->mWidth];
        theResized = true;
    }
    if (!theResized && this->mDirtyBounds.mLeft <= this->mDirtyBounds.mRight && this->mDirtyBounds.mTop <= this->mDirtyBounds.mBottom) {
        this->mDirtyBounds.mTop = this->mDirtyBounds.mTop - 1;
        if (this->mDirtyBounds.mTop < 0)
            this->mDirtyBounds.mTop = 0;
        this->mDirtyBounds.mLeft = this->mDirtyBounds.mLeft - 1;
        if (this->mDirtyBounds.mLeft < 0)
            this->mDirtyBounds.mLeft = 0;
        this->mDirtyBounds.mRight = min(this->mWidth - 1, this->mDirtyBounds.mRight + 1);
        this->mDirtyBounds.mBottom = min(this->mHeight - 1, this->mDirtyBounds.mBottom + 1);
        long theStart = this->mDirtyBounds.mLeft * this->mWidth + this->mDirtyBounds.mTop;
        long theCount = this->mWidth * this->mDirtyBounds.mRight + this->mDirtyBounds.mBottom - theStart;
        memset(this->mNodes + theStart, 0, theCount * sizeof(EEAIAStarNode*));
        memset(this->mTileFlags + theStart, 0, theCount);
    } else {
        memset(this->mNodes, 0, this->mHeight * this->mWidth * sizeof(EEAIAStarNode*));
        memset(this->mTileFlags, 0, this->mHeight * this->mWidth);
    }
    this->mHasBounds = false;
    this->mDirtyBounds.Clear();
}
