// C:\EEX\Empire Earth\Scenario Editor\EESEEPlace.h (RECONSTRUCTION; file name inferred).
// The "place" radio-button event of the graphic-effect type, a twin of ee/EESEEGCPlace.h (RTTI EESEEPlace > UIEvent >
// UReferenceCounter; vtable stored by EESETSETGameGraphicEffect::Virtual10 0x185537's `new`; ??_G 0x185765: the
// user-declared empty dtor; ProcessEvent 0x18547c has GCPlace's body, not ICF-folded with it). Requires lle/UIEvent.h.
#ifndef EE_EESEEPLACE_H
#define EE_EESEEPLACE_H

class EESEEPlace : public UIEvent {
public:
    virtual ~EESEEPlace() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x18547c
};

#endif
