// C:\EEX\Empire Earth\AI Thread\EEAIAStarPathSectorStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarPathSectorStorage, vtable 0x43a0d8). The path-sector search's node storage: 0x1b7dc..0x1b991 (the previous
// unit's COMDATs end at 0x1b7dc; EEAIAStarStorage.cpp's node-pool release 0x1b991 follows). ctor, ??_G (after its
// first user), dtor, Virtual10.
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"
#include "ee/EEAIAStarTerrainStorage.h"
#include "ee/EEAIAStarPathSectorStorage.h"

// FUNCTION: EXE 0x41b7dc
// Creates the path-sector search storage with no per-tile sub-sector id grid yet
EEAIAStarPathSectorStorage::EEAIAStarPathSectorStorage()
    : mSubSectorIDs(NULL)
{
}

// SYNTHETIC: EXE 0x41b7f2
// (the scalar deleting destructor)

// FUNCTION: EXE 0x41b80e
// Destroys the storage, freeing its per-tile sub-sector ids
EEAIAStarPathSectorStorage::~EEAIAStarPathSectorStorage()
{
    delete[] this->mSubSectorIDs;
}

// FUNCTION: EXE 0x41b829
// Sizes the node arrays and per-tile sub-sector ids for an inWidth x inHeight search (reallocating on a size change)
// and clears node and tile flags, only the last search's dirty bounds plus one when it can
// NEAR: the block layout and the frameless shape match; the original keeps inWidth/inHeight in
// edx/ebx and reloads the bounds after the validity test (an inline IsValid() gives that compare but then the clamps
// become memory `dec`s); EEAIAStarContinentStorage's Virtual10 0x15a99 has the same shape
void EEAIAStarPathSectorStorage::PrepareGrid(long inWidth, long inHeight)
{
    bool theResized = false;
    long theOldWidth = this->mWidth;
    long theOldHeight = this->mHeight;
    this->mWidth = inWidth;
    this->mHeight = inHeight;
    if (this->mSubSectorIDs == NULL || this->mTileFlags == NULL || this->mNodes == NULL || theOldWidth != inWidth || theOldHeight != inHeight) {
        delete[] this->mSubSectorIDs;
        this->mSubSectorIDs = new unsigned short[this->mHeight * this->mWidth];
        memset(this->mSubSectorIDs, 0, this->mHeight * this->mWidth * sizeof(unsigned short));
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
