// C:\EEX\Empire Earth\Scenario Editor\EESEEDisplayEpoch.h (RECONSTRUCTION; file name inferred).
// A scenario editor UI event (RTTI EESEEDisplayEpoch > UIEvent; vtable 0x4431e4: ??_G 0x1759f4, ProcessEvent
// 0x172c33, which sits in EESEPSSetPlayers.cpp's range between the class-static chains and the button's ctor).
// ProcessEvent refreshes the player screen (the editor's tab group 5). Requires lle/UIEvent.h.
#ifndef EE_EESEEDISPLAYEPOCH_H
#define EE_EESEEDISPLAYEPOCH_H

class EESEEDisplayEpoch : public UIEvent {
public:
    virtual ~EESEEDisplayEpoch() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x172c33
};

#endif
