// ee/EESEEChangeEffectAction.h: layout only as far as the verifier-exact
// functions that include it prove it (member names by offset are placeholders).
#ifndef EE_EESEECHANGEEFFECTACTION_H
#define EE_EESEECHANGEEFFECTACTION_H

#include "lle/UIEvent.h"

// RTTI EESEEChangeEffectAction > UIEvent > UReferenceCounter (+0x04 reference count, +0x08 UIEvent's initialize count).
class EESEEChangeEffectAction : public UIEvent {
public:
    virtual ~EESEEChangeEffectAction() {}   // inline (??_G 0x183429 stores the vptr)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
