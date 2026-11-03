// C:\EEX\Empire Earth\UI Events\EEUIEWheelZoom.h (RECONSTRUCTION; file name inferred). The mouse wheel's zoom
// event (RTTI EEUIEWheelZoom > UIEvent; vtable 0x445058, created by the UI's event factory). Requires
// lle/UIEvent.h.
#ifndef EE_EEUIEWHEELZOOM_H
#define EE_EEUIEWHEELZOOM_H

class EEUIEWheelZoom : public UIEvent {
public:
    virtual ~EEUIEWheelZoom() {}            // user-declared (??_G 0x59e17a stores this vtable first)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
