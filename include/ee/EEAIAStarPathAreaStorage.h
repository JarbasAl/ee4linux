// C:\EEX\Empire Earth\AI Thread\EEAIAStarPathAreaStorage.h (RECONSTRUCTION; file name inferred). RTTI
// EEAIAStarPathAreaStorage > EEAIAStarTerrainStorage; vtable 0x439fdc (EEAIAStarPathAreaStorage.cpp 0x19072..0x190bf):
// ctor 0x19072, ??_G 0x19088, dtor 0x190a4 (delete[] +0x60). Its Virtual10 is ICF-folded into
// EEAIAStarPathSectorStorage's 0x1b829 (identical code). Requires ee/EEAIAStarTerrainStorage.h.
#ifndef EE_EEAIASTARPATHAREASTORAGE_H
#define EE_EEAIASTARPATHAREASTORAGE_H

class EEAIAStarPathAreaStorage : public EEAIAStarTerrainStorage {
public:
    EEAIAStarPathAreaStorage();
    virtual ~EEAIAStarPathAreaStorage();
    virtual void PrepareGrid(long inArg1, long inArg2);  // +0x10 (folded into 0x1b829)

    unsigned short* mPathAreaIDs;           // +0x60 (delete[] in the dtor; 2 bytes per node)
};

#endif
