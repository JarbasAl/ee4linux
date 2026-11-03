// EEUIEProcessTimerExpired (RECONSTRUCTION; file name inferred): the UI event EEMCivTimerExpired::Execute
// (0xbd008) posts (RTTI EEUIEProcessTimerExpired > UIEvent; vtable 0x43e32c; sizeof 0xc; the ctor is expanded there).
// ProcessEvent 0xbd11c follows its first user as an inline COMDAT; ??_G is ICF-folded (0x1e60e7).
// Requires lle/UIEvent.h, ee/EEUserInterface.h.
#ifndef EE_EEUIEPROCESSTIMEREXPIRED_H
#define EE_EEUIEPROCESSTIMEREXPIRED_H

class EEUIEProcessTimerExpired : public UIEvent {
public:
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity) { return EEProcessTimerExpired(); }
};

#endif
