// ee/EESEEConditionPlayerResource.h: layout only as far as the verifier-exact
// functions that include it prove it (member names by offset are placeholders).
#ifndef EE_EESEECONDITIONPLAYERRESOURCE_H
#define EE_EESEECONDITIONPLAYERRESOURCE_H

#include "lle/UIEvent.h"

// RTTI EESEEConditionPlayerResource > UIEvent > UReferenceCounter (+0x04 reference count, +0x08 UIEvent's initialize count).
class EESEEConditionPlayerResource : public UIEvent {
public:
    virtual ~EESEEConditionPlayerResource() {}  // (??_G 0x17ee71: three vptr stores)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
