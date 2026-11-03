// ee/EESEEQuickSave.h (layout proven: EESEEQuickSave sizeof 0x10 = its new-site size, ctor 0x1a0215 exact in
// EESEEQuickSave.cpp; member names are provisional). Layout only as far as the verifier-exact
// functions that include it prove it (member names by offset are placeholders).
#ifndef EE_EESEEQUICKSAVE_H
#define EE_EESEEQUICKSAVE_H

#include "lle/UIEvent.h"

class EEAQuickSaveLoad;

class EESEEQuickSave : public UIEvent {
public:
    EESEEQuickSave(EEAQuickSaveLoad* inOwner);   // 0x1a0215 (the scenario editor passes its EESEAQuickSaveLoad)
    virtual ~EESEEQuickSave();
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
    EEAQuickSaveLoad* mOwner;               // +0x0c (referenced by the ctor; ProcessEvent 0x1af874 hands it to EESEAQuickSaveScenario's ctor)
};

#endif
