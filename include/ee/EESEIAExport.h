// C:\EEX\Empire Earth\Scenario Editor\EESEIAExport.h (RECONSTRUCTION; provisional names). The
// instructions screen's text export/import: the export action (RTTI EESEIAExport > EEAction; vtable 0x442bb0, sizeof
// 0x54; Virtual4 0x16ff5f), the import action (RTTI EESEIAImport > EEAction; vtable 0x442bc0; Virtual4 0x170345; built by
// EESEInstructions.cpp's question answer 0x1705a2) and the export button's event (RTTI EESEIEExport > UIEvent; vtable
// 0x442bd4; ProcessEvent 0x170170). Requires ee/EEAction.h, lle/FSFileSpec.h, lle/UIEvent.h.
#ifndef EE_EESEIAEXPORT_H
#define EE_EESEIAEXPORT_H

extern unsigned long gSETextVersion;        // 0x4c2cb8 (= 1: the "{Version_%d}" of the trigger text files) (provisional name)

class EESEIAExport : public EEAction {
public:
    EESEIAExport(const FSFileSpec& inFile) : mFile(inFile) {}
    virtual ~EESEIAExport() {}              // ??_G 0x17013d
    virtual void Virtual4();                // +0x04 0x16ff5f (writes the triggers' text)
    virtual void Virtual8() {}              // +0x08 (the shared empty slot 0x1720c6)

    FSFileSpec mFile;                       // +0x0c
};

// what the import question hands its answer (new 0x4c at 0x1704c4; the bool after the copied file spec)
struct EESEImportInfo {
    FSFileSpec mFile;                       // +0x00
    bool mValidate;                         // +0x48
};

class EESEIAImport : public EEAction {
public:
    // inline (EESEImportTextAnswer 0x1705a2: the base's stores, the file copy, then the flag)
    EESEIAImport(const FSFileSpec& inFile, bool inValidate) : mFile(inFile), mValidate(inValidate) {}
    virtual ~EESEIAImport() {}              // ??_G 0x170605
    virtual void Virtual4();                // +0x04 0x170345 (reads the triggers' text)
    virtual void Virtual8() {}              // +0x08 (the shared empty slot 0x1720c6)

    FSFileSpec mFile;                       // +0x0c
    bool mValidate;                         // +0x54 (true: only validate, then ask)
};

// the import button's event (RTTI EESEIEImport > UIEvent; vtable 0x442be8; newed by EESEInstructions's ctor)
class EESEIEImport : public UIEvent {
public:
    virtual ~EESEIEImport() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x170638
};

class EESEIEExport : public UIEvent {
public:
    virtual ~EESEIEExport() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x170170
};

#endif
