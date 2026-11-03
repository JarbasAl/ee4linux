// C:\EEX\Empire Earth\AI Thread\EEAIAStarTerrainStorage.h (RECONSTRUCTION; file name inferred). RTTI
// EEAIAStarTerrainStorage > EEAIAStarFastStorage; vtable 0x43a108 (EEAIAStarTerrainStorage.cpp 0x1c666..0x1d846):
// ctor 0x1c666 (the +0x50 bounds' inline ctor, then Reset 0x1c6ee), ??_G 0x1c68d, dtor 0x1c6a9
// (no members to destroy: the vptr, then the base dtor). Slot +0x10 is the base's (0x15d1c). Requires
// ee/EEAIAStarFastStorage.h.
#ifndef EE_EEAIASTARTERRAINSTORAGE_H
#define EE_EEAIASTARTERRAINSTORAGE_H

class EEAIAStarTerrainStorage : public EEAIAStarFastStorage {
public:
    EEAIAStarTerrainStorage();              // 0x1c666
    virtual ~EEAIAStarTerrainStorage();     // 0x1c6a9
    void Reset();                           // 0x1c6ee (+0x5c/+0x54 = 0, +0x50 = mL40 - 1, +0x58 = mL3C - 1) (provisional name)
    // one storage API with EEAIAStarStorage (the A* machines call AddOpen/RemoveBestOpen/AddClosed of their S)
    void AddOpen(EEAIAStarNode* inNode);    // 0x1c6b4: flag 4, in front of mFastOpenList, the node map (was Push)
    EEAIAStarNode* RemoveBestOpen();        // 0x1c705: the front of mFastOpenList, or 0 (was Pop)

    EEAIAStarBounds mDirtyBounds;           // +0x50 (its own; Reset: mL00 = mL40 - 1, mL08 = mL3C - 1)
};

#endif
