// C:\EEX\Empire Earth\AI Thread\EEAIAStarHillStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarHillStorage, vtable 0x439f64). The hill search's node storage: 0x171b5..0x173cc (EEAIAStarHillGoal.cpp ends
// at 0x171b5; EEAIAStarPathAreaGoal's ctor starts at 0x173cc). ctor, ??_G (after its first user), dtor, the tile
// array's ??_E (ICF: EEAIAStarContinentTile's is the name), Virtual10. The twin of EEAIAStarContinentStorage.cpp.
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"
#include "ee/EEAIAStarTerrainStorage.h"
#include "ee/EEAIAStarHillStorage.h"

// FUNCTION: EXE 0x4171b5
// Creates the hill search storage with no hill tile grid
EEAIAStarHillStorage::EEAIAStarHillStorage()
    : mTiles60(NULL)
{
}

// SYNTHETIC: EXE 0x4171cb
// (the scalar deleting destructor)

// FUNCTION: EXE 0x4171e7
// Destroys the storage, freeing its hill tile grid
EEAIAStarHillStorage::~EEAIAStarHillStorage()
{
    delete[] this->mTiles60;
}

// SYNTHETIC: EXE 0x417207
// (EEAIAStarHillTile's vector deleting destructor, ICF-folded with EEAIAStarContinentTile's)

// NEAR: as EEAIAStarContinentStorage::PrepareGrid 0x15a99 (the same shape; register choice of inWidth/inHeight)
// FUNCTION: EXE 0x417239
// Sizes the hill tile, node and flag grids (reallocating on a size change) and clears the node and flag grids,
// only the span covering the dirty bounds plus 1 when unchanged; then forgets the dirty bounds
void EEAIAStarHillStorage::PrepareGrid(long inWidth, long inHeight)
{
    bool theResized = false;
    long theOldWidth = this->mWidth;
    long theOldHeight = this->mHeight;
    this->mWidth = inWidth;
    this->mHeight = inHeight;
    if (this->mTiles60 == NULL || this->mTileFlags == NULL || theOldWidth != inWidth || theOldHeight != inHeight) {
        delete[] this->mTiles60;
        this->mTiles60 = new EEAIAStarHillTile[this->mHeight * this->mWidth];
        memset(this->mTiles60, 0, this->mHeight * this->mWidth * 2);
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
