// C:\EEX\Empire Earth\Scenario Editor\EESETSETSAAAttributeChange.h (RECONSTRUCTION; file name inferred). The attribute
// combo-box event of the "set AI attribute" effect type (RTTI EESETSETSAAAttributeChange > UIEvent >
// UReferenceCounter; vtable 0x444384; sizeof 0x14: new 0x14 in EESETSETSetAIAttribute::Virtual10 0x18c72e, which
// passes itself and the controls; members stored before the vptr: an init list). ProcessEvent 0x18c4e3 =
// mType->ShowAttributeRanges(the combo's +0xd0 selection, mControls), returns 0. ??_G 0x18c941. Names are provisional.
// Requires lle/UIEvent.h, ee/EESETSETAI.h.
#ifndef EE_EESETSETSAAATTRIBUTECHANGE_H
#define EE_EESETSETSAAATTRIBUTECHANGE_H

class EESETSETSetAIAttribute;
class EESETSEffectControls;

class EESETSETSAAAttributeChange : public UIEvent {
public:
    EESETSETSAAAttributeChange(EESETSETSetAIAttribute* inType, EESETSEffectControls* inControls)
        : mType(inType), mControls(inControls) {}
    virtual ~EESETSETSAAAttributeChange() {}                      // (??_G: 0x2b bytes, three vptr stores: a user-declared empty dtor)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x18c4e3

    EESETSETSetAIAttribute* mType;          // +0x0c
    EESETSEffectControls* mControls;        // +0x10
};

#endif
