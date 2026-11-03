// EESEEFindText.h (RECONSTRUCTION; file name inferred). The instructions screen's text edit event
// (RTTI EESEEFindText > UIEvent; vtable 0x442dd0; sizeof 0xc: EESEInstructions's ctor 0x1707ff news it and sets it as
// the edit boxes' 0x3f1 event; ProcessEvent 0x1af2a9 replaces "///<ids>" by the resource strings). Moved here from
// EEUIFSETRBPaintType.cpp. Requires lle/UIEvent.h.
#ifndef EE_EESEEFINDTEXT_H
#define EE_EESEEFINDTEXT_H

class EESEEFindText : public UIEvent {
public:
    virtual ~EESEEFindText() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x1af2a9
};

#endif
