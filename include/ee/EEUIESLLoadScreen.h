// ee/EEUIESLLoadScreen.h: layout only as far as the verifier-exact
// functions that include it prove it (member names by offset are placeholders).
#ifndef EE_EEUIESLLOADSCREEN_H
#define EE_EEUIESLLOADSCREEN_H

#include "lle/UIEvent.h"
#include "lle/UString.h"

class EEUIESLLoadScreen : public UIEvent {
public:
    EEUIESLLoadScreen();
    // the file extension and the directory (0x1af6c1 is the out-of-line copy the in-game options call)
    EEUIESLLoadScreen(const UString& inExtension, const UString& inDirectory)
        : mExtension(inExtension), mDirectory(inDirectory) {}
    virtual ~EEUIESLLoadScreen();
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
private:
    UString mExtension;                     // +0x0c
    UString mDirectory;                     // +0x1c
};

#endif
