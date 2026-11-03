// C:\EEX\Empire Earth\AI Thread\EEAIAStarPathSectorStorage.h (RECONSTRUCTION; file name inferred).
// RTTI EEAIAStarPathSectorStorage > EEAIAStarTerrainStorage; vtable 0x43a0d8 (EEAIAStarPathSectorStorage.cpp
// 0x1b7dc..0x1b991): ctor 0x1b7dc, ??_G 0x1b7f2, dtor 0x1b80e (delete[] +0x60), Virtual10 0x1b829 (the kept copy of
// EEAIAStarPathAreaStorage's identical slot). Requires ee/EEAIAStarTerrainStorage.h.
#ifndef EE_EEAIASTARPATHSECTORSTORAGE_H
#define EE_EEAIASTARPATHSECTORSTORAGE_H

class EEAIAStarPathSectorStorage : public EEAIAStarTerrainStorage {
public:
    EEAIAStarPathSectorStorage();
    virtual ~EEAIAStarPathSectorStorage();
    // +0x10 0x1b829: sizes the node arrays for an inWidth x inHeight search (a same-size call only clears the part
    // the last search touched, grown by one node)
    virtual void PrepareGrid(long inWidth, long inHeight);

    unsigned short* mSubSectorIDs;          // +0x60 (delete[] in the dtor; 2 bytes per node)
};

#endif
