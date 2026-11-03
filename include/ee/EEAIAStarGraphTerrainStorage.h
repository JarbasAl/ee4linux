// C:\EEX\Empire Earth\AI Thread\EEAIAStarGraphTerrainStorage.h (RECONSTRUCTION, STUB; file name inferred). RTTI
// EEAIAStarGraphTerrainStorage > EEAIAStarGraphStorage; vtable 0x43a2c8 (??_G 0x285c1; slots
// +0x04..+0x10 are the base's). Only the base is declared (its own code is in another unit's range). Requires
// ee/EEAIAStarGraphStorage.h.
#ifndef EE_EEAIASTARGRAPHTERRAINSTORAGE_H
#define EE_EEAIASTARGRAPHTERRAINSTORAGE_H

class EEAIAStarGraphTerrainStorage : public EEAIAStarGraphStorage {
public:
    // a user dtor: ??_G 0x285c1 stores this vtable before calling the base dtor
    virtual ~EEAIAStarGraphTerrainStorage() {}
    // the graph machine's AddOpen/RemoveBestOpen go to the fast open list here (0x27434/0x274a3 call Push
    // 0x16bb5 / Pop 0x16be1 directly) (provisional name)
    void AddOpen(EEAIAStarGraphNode* inNode) { this->Push(inNode); }
    void UpdateOpen(EEAIAStarGraphNode* inNode) {}   // the fast open list is unsorted (0x28aa7 calls nothing)
    EEAIAStarGraphNode* RemoveBestOpen() { return this->Pop(); }
};

#endif
