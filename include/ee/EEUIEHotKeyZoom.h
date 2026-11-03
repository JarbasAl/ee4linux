// Hot-key zoom (EEUIEHotKeyZoom; ProcessEvent 0x1d9c3b)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEHOTKEYZOOM_H
#define EE_EEUIEHOTKEYZOOM_H

// vtable 0x445044; ??_G 0x19e1fb; ProcessEvent 0x1d9c3b; sizeof 0x10; ctor inline; new at 0x19dd7e, 0x19dddf, 0x2639f6, 0x263a29
class EEUIEHotKeyZoom : public UIEvent {
public:
    // (provisional name; event ctors assign in the body)
    EEUIEHotKeyZoom(long inDirection) { this->mDirection = inDirection; }
    virtual ~EEUIEHotKeyZoom() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    // +0x0c (new sites store ebx, ecx): ProcessEvent 0x1d9c3b zooms in (smaller zoom) for 0, out otherwise
    // (provisional name)
    long mDirection;
};
#endif
