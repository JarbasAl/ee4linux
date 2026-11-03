// C:\EEX\Empire Earth\...\EEAQuickSaveLoad.h (RECONSTRUCTION; file path inferred). The quick
// save/load action (RTTI EEAQuickSaveLoad > EEAction; vtable 0x44517c, sizeof 0x2c; Virtual4 0x1a5529 writes or reads
// the quick save through an FSMemoryFile). The inline ctor/dtor are emitted in EEUIFScenarioEditor.cpp (0x1a00c6,
// 0x1a0102, ??_G 0x1a013e). Names are provisional. Requires ee/EEAction.h, lle/UString.h, lle/FSMemoryFile.h.
#ifndef EE_EEAQUICKSAVELOAD_H
#define EE_EEAQUICKSAVELOAD_H

class EEGameState;

class EEAQuickSaveLoad : public EEAction {
public:
    EEAQuickSaveLoad(const UString& inName) : EEAction(false), mName(inName), mP20(NULL), mFile(NULL), mIsLoad(false),
        mWriteToFile(false) {}
    virtual ~EEAQuickSaveLoad()
    {
        delete this->mFile;
        if (this->mP20 != NULL)
            delete this->mP20;
    }
    virtual void Virtual4();                // +0x04 (0x1a5529)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    // 0x1a57d3: writes the memory file to data\Scenarios\<inName> (provisional name)
    void SaveToFile(const UString& inName);

    UString mName;                          // +0x0c (appended to "..." for the file name)
    EEGameState* mGameState;                // +0x1c (not set by the ctor; the UI thread sets &gGameState before a quick load)
    unsigned char* mP20;                    // +0x20
    FSMemoryFile* mFile;                    // +0x24 (0x1a56a4: deleted, then a new 0x78-byte file)
    bool mIsLoad;                           // +0x28 (set: load; clear: save, as EESEAQuickSaveScenario does)
    bool mWriteToFile;                      // +0x29
};

#endif
