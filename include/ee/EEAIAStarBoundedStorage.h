// C:\EEX\Empire Earth\AI Thread\EEAIAStarBoundedStorage.h (RECONSTRUCTION; file name inferred). The
// bounded search's node storage (RTTI EEAIAStarBoundedStorage > EEAIAStarFastStorage; vtable 0x439cf4; sizeof 0x60;
// EEAIAStarBoundedStorage.cpp 0x10bc7..0x10d5d): ctor 0x10bc7 (the +0x50 bounds' inline ctor), ??_G 0x10be7, dtor
// 0x10c03, Virtual10 0x10c0e. Requires ee/EEAIAStarFastStorage.h.
#ifndef EE_EEAIASTARBOUNDEDSTORAGE_H
#define EE_EEAIASTARBOUNDEDSTORAGE_H

class EEAIAStarBoundedStorage : public EEAIAStarFastStorage {
public:
    EEAIAStarBoundedStorage();
    virtual ~EEAIAStarBoundedStorage();
    // +0x10 0x10c0e: sizes the node arrays; when the last search's bounds are valid (and mB48 is set) only those
    // tiles, grown by two, are cleared
    virtual void PrepareGrid(long inWidth, long inHeight);

    // inline (COMDATs 0xaaf29 / 0xab29e after the bounded machine's Start/Expand): the open list keeps the
    // node map in step (FastStorage's mNodes) (provisional name)
    void AddOpen(EEAIAStarNode* inNode)
    {
        EEAIAStarStorage::AddOpen(inNode);
        this->mNodes[this->mWidth * inNode->mX + inNode->mY] = inNode;
    }
    EEAIAStarNode* RemoveBestOpen()
    {
        EEAIAStarNode* theNode = EEAIAStarStorage::RemoveBestOpen();
        if (theNode)
            this->mNodes[this->mWidth * theNode->mX + theNode->mY] = NULL;
        return theNode;
    }

    EEAIAStarBounds mSearchBounds;          // +0x50
};

#endif
