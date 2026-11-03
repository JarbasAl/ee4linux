// Keyboard scrolling (EEUIEKeyboardScroll; ProcessEvent 0x1d9b6f)
// (RECONSTRUCTION from RTTI/vtables/new sites; header and file names inferred.)
// Classes in ProcessEvent address order (the .cpp's definition order); declaration order, inline ProcessEvents
// and dtors are for the unit to prove (a unit's ??_G COMDATs follow its first user in declaration order).
// Requires lle/UIEvent.h.
#ifndef EE_EEUIEKEYBOARDSCROLL_H
#define EE_EEUIEKEYBOARDSCROLL_H

// vtable 0x44a998; ??_G 0x26479a; ProcessEvent 0x1d9b6f; sizeof 0x10; ctor inline; new at 0x263757, 0x26378d, 0x2637c2, 0x2637fc
class EEUIEKeyboardScroll : public UIEvent {
public:
    // (provisional name; event ctors assign in the body)
    EEUIEKeyboardScroll(long inDirection) : mDirection(inDirection) {}
    virtual ~EEUIEKeyboardScroll() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    // +0x0c (new sites store 2, 3, ebx, ecx): ProcessEvent 0x1d9b6f scrolls +y for 0, -y for 1, -x for 2, +x for 3
    // (provisional name)
    long mDirection;
};
#endif
