// C:\EEX\Empire Earth\AI Thread\EEAIAStarChokePointStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarChokePointStorage, vtable 0x439e3c). The choke point search's node storage: 0x1540d..0x155c0
// (EEAIAStarChokePointGoal.cpp ends at 0x1540d; EEAIAStarContinentGoal's code starts at 0x155c0). ctor, ??_G (after
// its first user), dtor, Virtual10.
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"
#include "ee/EEAIAStarTerrainStorage.h"
#include "ee/EEAIAStarChokePointStorage.h"

// FUNCTION: EXE 0x41540d
// Creates the choke point search storage with no choke point id grid
EEAIAStarChokePointStorage::EEAIAStarChokePointStorage()
    : mChokePointIDs(NULL)
{
}

// SYNTHETIC: EXE 0x415423
// (the scalar deleting destructor)

// FUNCTION: EXE 0x41543f
// Destroys the storage, freeing its choke point id grid
EEAIAStarChokePointStorage::~EEAIAStarChokePointStorage()
{
    delete[] this->mChokePointIDs;
}

// NEAR: as EEAIAStarPathSectorStorage::PrepareGrid 0x1b829 (the same shape without the node-array test)
// FUNCTION: EXE 0x41545a
// Sizes the id, node and tile-flag grids (reallocating, ids zeroed, on a size change) and clears the node and flag
// grids, only the span covering the dirty bounds plus 1 when unchanged; then forgets the dirty bounds
void EEAIAStarChokePointStorage::PrepareGrid(long inWidth, long inHeight)
{
    bool theResized = false;
    long theOldWidth = this->mWidth;
    long theOldHeight = this->mHeight;
    this->mWidth = inWidth;
    this->mHeight = inHeight;
    if (this->mChokePointIDs == NULL || this->mTileFlags == NULL || theOldWidth != inWidth || theOldHeight != inHeight) {
        delete[] this->mChokePointIDs;
        this->mChokePointIDs = new unsigned long[this->mHeight * this->mWidth];
        memset(this->mChokePointIDs, 0, this->mHeight * this->mWidth * sizeof(unsigned long));
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
