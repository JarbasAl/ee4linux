// ee/EESEEQuickLoad.h (layout proven: EESEEQuickLoad sizeof 0x10 = its new-site size, ctor 0x1a01c1 exact in
// EESEEQuickLoad.cpp; member names are provisional). Layout only as far as the verifier-exact
// functions that include it prove it (member names by offset are placeholders).
#ifndef EE_EESEEQUICKLOAD_H
#define EE_EESEEQUICKLOAD_H

#include "lle/UIEvent.h"

class EESEEQuickLoad : public UIEvent {
public:
    EESEEQuickLoad(UReferenceCounter* inOwner);
    virtual ~EESEEQuickLoad();
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
private:
    UReferenceCounter* mOwner;              // +0x0c
};

#endif
