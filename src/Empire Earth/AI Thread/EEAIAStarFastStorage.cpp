// C:\EEX\Empire Earth\AI Thread\EEAIAStarFastStorage.cpp (file name inferred: no __FILE__; RTTI class
// EEAIAStarFastStorage, vtable 0x439e9c). The A* storage with a node map: 0x15c36..0x15dae (EEAIAStarContinentStorage's
// Virtual10 ends at 0x15c36, EEAIAStarGraphContinentalChainGoal's ctor starts at 0x15dae). ctor, ??_G (after its first
// user), dtor, Add, Remove, Virtual10.
#include <windows.h>
#include <string.h>

#include "lle/LowLevelEngine.h"

#include "ee/EEAIAStarStorage.h"
#include "ee/EEAIAStarFastStorage.h"

// FUNCTION: EXE 0x415c36
// Creates the A* storage with no node map
EEAIAStarFastStorage::EEAIAStarFastStorage()
    : mNodes(NULL)
{
}

// SYNTHETIC: EXE 0x415c4c
// (the scalar deleting destructor)

// FUNCTION: EXE 0x415c68
// Destroys the storage, freeing its node map
EEAIAStarFastStorage::~EEAIAStarFastStorage()
{
    delete[] this->mNodes;
}

// FUNCTION: EXE 0x415c83
// Puts a node at the head of the closed list, flags its tile closed and records it in the node map
void EEAIAStarFastStorage::AddClosed(EEAIAStarNode* inNode)
{
    if (this->mFastClosedList)
        this->mFastClosedList->mPrev = inNode;
    inNode->mNext = this->mFastClosedList;
    this->mFastClosedList = inNode;
    unsigned char* theFlags = this->mTileFlags + inNode->mX * this->mWidth + inNode->mY;
    *theFlags |= kAStarTileClosed;
    this->mNodes[inNode->mX * this->mWidth + inNode->mY] = inNode;
}

// FUNCTION: EXE 0x415cbd
// Unlinks a node from the closed list, clears its tile's closed flag and removes it from the node map
void EEAIAStarFastStorage::RemoveClosed(EEAIAStarNode* inNode)
{
    if (inNode->mPrev) {
        inNode->mPrev->mNext = inNode->mNext;
        if (inNode->mNext)
            inNode->mNext->mPrev = inNode->mPrev;
    } else {
        this->mFastClosedList = inNode->mNext;
        if (inNode->mNext)
            inNode->mNext->mPrev = NULL;
    }
    inNode->mNext = NULL;
    inNode->mPrev = NULL;
    unsigned char* theFlags = this->mTileFlags + inNode->mX * this->mWidth + inNode->mY;
    *theFlags &= ~kAStarTileClosed;
    this->mNodes[inNode->mX * this->mWidth + inNode->mY] = NULL;
}

// FUNCTION: EXE 0x415d1c
// Sizes the node map and tile flags to the grid (reallocating on a size change) and clears both
void EEAIAStarFastStorage::PrepareGrid(long inWidth, long inHeight)
{
    long theOldWidth = this->mWidth;
    long theOldHeight = this->mHeight;
    this->mWidth = inWidth;
    this->mHeight = inHeight;
    if (this->mTileFlags == NULL || this->mNodes == NULL || theOldWidth != inWidth || theOldHeight != inHeight) {
        delete[] this->mTileFlags;
        this->mTileFlags = new unsigned char[this->mHeight * this->mWidth];
        delete[] this->mNodes;
        this->mNodes = new EEAIAStarNode*[this->mHeight * this->mWidth];
    }
    memset(this->mNodes, 0, this->mHeight * this->mWidth * sizeof(EEAIAStarNode*));
    memset(this->mTileFlags, 0, this->mHeight * this->mWidth);
}
