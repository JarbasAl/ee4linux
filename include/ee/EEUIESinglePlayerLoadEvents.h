// The single-player load screen's events (class prefixes EEUIESPL/SPLE; ProcessEvents 0x1e8e7d..0x1e93df)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIESINGLEPLAYERLOADEVENTS_H
#define EE_EEUIESINGLEPLAYERLOADEVENTS_H

// vtable 0x446ac8; ??_G 0x1e9398; ProcessEvent 0x1e8e7d; sizeof 0x2c; ctor 0x1e9356
class EEUIESPLLoadScreen : public UIEvent {
public:
    EEUIESPLLoadScreen(const UString& inExtension, const UString& inDirectory) : mExtension(inExtension), mDirectory(inDirectory) {}   // out of line 0x1e9356, after its first user (SPLEOpen)
    virtual ~EEUIESPLLoadScreen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    UString mExtension;         // +0x0c (gSavedGameExtension; appended to the selected name)
    UString mDirectory;         // +0x1c ("data\" "Saved Games\")
};

// vtable 0x44b88c; ??_G 0x27451d; ProcessEvent 0x1e900e; sizeof 0x30; ctor 0x2744d4
class EEUIESPLELoadFile : public UIEvent {
public:
#ifdef EE_PORT_LONG32
    EEUIESPLELoadFile(const UString& inExtension, const UString& inDirectory, LONG_PTR inForm) : mExtension(inExtension), mDirectory(inDirectory), mForm(inForm) {}    // (provisional name; out of line copy 0x2744d4)
#else
    EEUIESPLELoadFile(const UString& inExtension, const UString& inDirectory, long inForm) : mExtension(inExtension), mDirectory(inDirectory), mForm(inForm) {}    // (provisional name; out of line copy 0x2744d4)
#endif
    virtual ~EEUIESPLELoadFile() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    UString mExtension;         // +0x0c (gSavedGameExtension; appended to the selected name)
    UString mDirectory;         // +0x1c ("data\" "Saved Games\")
#ifdef EE_PORT_LONG32
    LONG_PTR mForm;                 // +0x2c (provisional name; ctor stores eax)
#else
    long mForm;                 // +0x2c (provisional name; ctor stores eax)
#endif
};

// vtable 0x44b058; ??_G 0x271e05; ProcessEvent 0x1e92a1; sizeof 0xc; ctor inline; new at 0x271b55
class EEUIESPLEOpen : public UIEvent {
public:
    virtual ~EEUIESPLEOpen() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};
#endif
