// C:\EEX\Empire Earth\AI Thread\EEAIAStarBoundedStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarBoundedStorage, vtable 0x439cf4). The bounded search's node storage: 0x10bc7..0x10d5d (the
// EEAIAStarBoundedPathGoal unit ends at 0x10bc7, EEAIAStarChokePointGoal's code starts at 0x10d5d). ctor, ??_G (after
// its first user), dtor, Virtual10.
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"
#include "ee/EEAIAStarBoundedStorage.h"

// FUNCTION: EXE 0x410bc7
// Creates an empty node storage for the bounded path search
EEAIAStarBoundedStorage::EEAIAStarBoundedStorage()
{
}

// SYNTHETIC: EXE 0x410be7
// (the scalar deleting destructor)

// FUNCTION: EXE 0x410c03
// Destroys the storage (base members only)
EEAIAStarBoundedStorage::~EEAIAStarBoundedStorage()
{
}

// FUNCTION: EXE 0x410c0e
// Sizes the node and tile-flag grids (reallocating on a size change) and clears them, only the span covering the
// last search bounds plus 2 when they were set; then forgets the bounds
void EEAIAStarBoundedStorage::PrepareGrid(long inWidth, long inHeight)
{
    EEAIAStarBounds theBounds = this->mBounds;
    long theOldWidth = this->mWidth;
    long theOldHeight = this->mHeight;
    this->mWidth = inWidth;
    this->mHeight = inHeight;
    if (this->mTileFlags == NULL || this->mNodes == NULL || theOldWidth != inWidth || theOldHeight != inHeight) {
        delete[] this->mTileFlags;
        this->mTileFlags = new unsigned char[this->mWidth * this->mHeight];
        delete[] this->mNodes;
        this->mNodes = new EEAIAStarNode*[this->mWidth * this->mHeight];
    }
    if (this->mHasBounds && theBounds.mLeft <= theBounds.mRight && theBounds.mTop <= theBounds.mBottom) {
        long theFirstColumn = theBounds.mTop - 2 < 0 ? 0 : theBounds.mTop - 2;
        long theFirstRow = theBounds.mLeft - 2 < 0 ? 0 : theBounds.mLeft - 2;
        long theLastRow = min(this->mWidth - 1, theBounds.mRight + 2);
        long theLastColumn = min(this->mHeight - 1, theBounds.mBottom + 2);
        long theStart = this->mWidth * theFirstRow + theFirstColumn;
        long theCount = this->mWidth * theLastRow - theStart + theLastColumn;
        memset(this->mNodes + theStart, 0, theCount * sizeof(EEAIAStarNode*));
        memset(this->mTileFlags + theStart, 0, theCount);
    } else {
        memset(this->mNodes, 0, this->mWidth * this->mHeight * sizeof(EEAIAStarNode*));
        memset(this->mTileFlags, 0, this->mWidth * this->mHeight);
    }
    this->mHasBounds = false;
    this->mBounds.Clear();
}
