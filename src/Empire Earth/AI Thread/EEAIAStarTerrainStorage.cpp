// C:\EEX\Empire Earth\AI Thread\EEAIAStarTerrainStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarTerrainStorage, vtable 0x43a108). The terrain searches' node storage: 0x1c666..0x1c740 (EEAIAStarStorage.cpp
// ends at 0x1c666; EEAIChokePoint's ctor starts at 0x1c740). ctor, ??_G (after its first user), dtor, Push, Reset, Pop.
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"
#include "ee/EEAIAStarTerrainStorage.h"

// FUNCTION: EXE 0x41c666
// Creates the terrain flood-fill storage with the whole grid marked dirty
EEAIAStarTerrainStorage::EEAIAStarTerrainStorage()
{
    this->Reset();
}

// SYNTHETIC: EXE 0x41c68d
// (the scalar deleting destructor)

// FUNCTION: EXE 0x41c6a9
// Destroys the storage (bases only)
EEAIAStarTerrainStorage::~EEAIAStarTerrainStorage()
{
}

// FUNCTION: EXE 0x41c6b4
// Pushes a node onto the unsorted open stack, flags its tile open and records it in the tile's node slot
void EEAIAStarTerrainStorage::AddOpen(EEAIAStarNode* inNode)
{
    unsigned char* theFlags = this->mTileFlags + inNode->mX * this->mWidth + inNode->mY;
    *theFlags |= kAStarTileOpen;
    if (this->mFastOpenList)
        this->mFastOpenList->mPrev = inNode;
    inNode->mNext = this->mFastOpenList;
    this->mFastOpenList = inNode;
    this->mNodes[inNode->mX * this->mWidth + inNode->mY] = inNode;
}

// FUNCTION: EXE 0x41c6ee
// Marks the whole grid as dirty so the next search clears every tile
void EEAIAStarTerrainStorage::Reset()
{
    this->mDirtyBounds.mTop = 0;
    this->mDirtyBounds.mLeft = 0;
    this->mDirtyBounds.mBottom = this->mHeight - 1;
    this->mDirtyBounds.mRight = this->mWidth - 1;
}

// FUNCTION: EXE 0x41c705
// Pops the most recently opened node (no cost order), clearing its open flag and tile node slot; NULL when empty
EEAIAStarNode* EEAIAStarTerrainStorage::RemoveBestOpen()
{
    EEAIAStarNode* theNode = NULL;
    if (this->mFastOpenList) {
        theNode = this->mFastOpenList;
        this->mFastOpenList = theNode->mNext;
        theNode->mNext = NULL;
        theNode->mPrev = NULL;
        unsigned char* theFlags = this->mTileFlags + theNode->mX * this->mWidth + theNode->mY;
        *theFlags &= ~kAStarTileOpen;
        this->mNodes[theNode->mX * this->mWidth + theNode->mY] = NULL;
    }
    return theNode;
}
