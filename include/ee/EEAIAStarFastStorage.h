// C:\EEX\Empire Earth\AI Thread\EEAIAStarFastStorage.h (RECONSTRUCTION; file name inferred).
// RTTI EEAIAStarFastStorage > EEAIAStarStorage; vtable 0x439e9c (EEAIAStarFastStorage.cpp 0x15c36..0x160b7): ctor
// 0x15c36 (+0x4c = 0 in the init list), ??_G 0x15c4c, dtor 0x15c68 (delete[] +0x4c), Add, Remove, Virtual10 0x15d1c. Requires
// ee/EEAIAStarStorage.h.
#ifndef EE_EEAIASTARFASTSTORAGE_H
#define EE_EEAIASTARFASTSTORAGE_H
#include "ee/EEAIKindIDs.h"

class EEAIAStarFastStorage : public EEAIAStarStorage {
public:
    EEAIAStarFastStorage();                 // 0x15c36
    virtual ~EEAIAStarFastStorage();        // 0x15c68
    // +0x10 0x15d1c: sizes the node arrays for an inWidth x inHeight search (reallocated when the size changes), then
    // clears them
    virtual void PrepareGrid(long inWidth, long inHeight);

    // hides EEAIAStarStorage::AddClosed (the bounded Expand 0xaaf4c calls 0x15c83 where the path Expand 0xaa98d calls
    // 0x1ba43)
    void AddClosed(EEAIAStarNode* inNode);  // 0x15c83: in front of the open list, flag 8, the node map (was Add)
    void RemoveClosed(EEAIAStarNode* inNode);   // 0x15cbd (hides EEAIAStarStorage::RemoveClosed: the bounded AddNode 0xab8f7 calls it; was Remove)
    // inline (COMDATs 0x1d74b / 0x1d773, first user the machines' AddNode 0x28fc9): the
    // tile's node when it is closed (flag 8) / open (flag 4), else 0 (provisional names)
    EEAIAStarNode* FindClosed(long inX, long inY)
    {
        return this->mTileFlags[this->mWidth * inX + inY] & kAStarTileClosed ? this->mNodes[this->mWidth * inX + inY] : 0;
    }
    EEAIAStarNode* FindOpen(long inX, long inY)
    {
        return this->mTileFlags[this->mWidth * inX + inY] & kAStarTileOpen ? this->mNodes[this->mWidth * inX + inY] : 0;
    }

    EEAIAStarNode** mNodes;                 // +0x4c (delete[] in the dtor; one per tile)
};

#endif
