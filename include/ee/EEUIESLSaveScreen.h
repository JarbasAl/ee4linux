// C:\EEX\Empire Earth\UI Events\EEUIESLSaveScreen.h (RECONSTRUCTION; file path inferred; layout proven:
// EEUIESLSaveScreen sizeof 0x2c = its new-site size, ctor 0x1aa9ef exact in EESEASaveScenario.cpp; member names
// provisional). Opens the save screen (RTTI EEUIESLSaveScreen > UIEvent; vtable 0x4459b4, sizeof 0x2c). Its
// ProcessEvent 0x1e6fab sits in the UI Events group; the inline ctor 0x1aa9ef, ??_G 0x1aaa31 and dtor 0x1aaa4d are
// emitted by their first user, EESEASaveScenario.cpp. Requires lle/UIEvent.h, lle/UString.h.
#ifndef EE_EEUIESLSAVESCREEN_H
#define EE_EEUIESLSAVESCREEN_H

class EEUIESLSaveScreen : public UIEvent {
public:
    // the file extension (gScenarioExtension / gSavedGameExtension) and the directory
    EEUIESLSaveScreen(const UString& inExtension, const UString& inDirectory)
        : mExtension(inExtension), mDirectory(inDirectory) {}
    virtual ~EEUIESLSaveScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    UString mExtension;                     // +0x0c
    UString mDirectory;                     // +0x1c
};

#endif
