// C:\EEX\Empire Earth\Scenario Editor\EESETSETMSDCameraScrollChange.h (RECONSTRUCTION; file name inferred). The
// camera-scroll check-box event of the "send dialog" media effect type (RTTI EESETSETMSDCameraScrollChange > UIEvent >
// UReferenceCounter; vtable 0x444020; sizeof 0x14: new 0x14 in EESETSETMediaSendDialog::Virtual14 0x186e1f, which
// passes itself and the controls; members stored before the vptr: an init list). ProcessEvent 0x186bb0 =
// mType->ShowCameraScrollSpeed(mControls), returns 0. ??_G 0x187416. Names are provisional.
// Requires lle/UIEvent.h, ee/EESETSETMedia.h.
#ifndef EE_EESETSETMSDCAMERASCROLLCHANGE_H
#define EE_EESETSETMSDCAMERASCROLLCHANGE_H

class EESETSETMediaSendDialog;
class EESETSEffectControls;

class EESETSETMSDCameraScrollChange : public UIEvent {
public:
    EESETSETMSDCameraScrollChange(EESETSETMediaSendDialog* inType, EESETSEffectControls* inControls)
        : mType(inType), mControls(inControls) {}
    virtual ~EESETSETMSDCameraScrollChange() {}                      // (??_G: 0x2b bytes, three vptr stores: a user-declared empty dtor)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x186bb0

    EESETSETMediaSendDialog* mType;         // +0x0c
    EESETSEffectControls* mControls;        // +0x10
};

#endif
