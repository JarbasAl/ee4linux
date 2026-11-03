// ee/EEUIEGrabScroll.h: layout only as far as the verifier-exact
// Layout proven: EEUIEGrabScroll sizeof 0x14 = its new-site size, ctor 0x19e2d2 exact in EEUIEGrabScroll.cpp. Member
// names are provisional.
// functions that include it prove it (member names by offset are placeholders).
#ifndef EE_EEUIEGRABSCROLL_H
#define EE_EEUIEGRABSCROLL_H

#include "lle/UIEvent.h"

class EEUIEGrabScroll : public UIEvent {
public:
    EEUIEGrabScroll();
    virtual ~EEUIEGrabScroll();
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
private:
    // verified by ProcessEvent 0x1d99ff: +0x0c is the form's mouse-move event saved
    // while the grab lasts (GetMouseMoveEvent, Reference'd; restored with SetMouseMoveEvent, then Dereference'd), +0x10
    // the grab flag (set while button 5 is down). Names kept for the ds unit; the ctor 0x19e2d2 / dtor 0x19e309 sit
    // with their first user in another unit (the ctor has two users, 0x19e013 and 0x263266: inline in the real header)
    UIEvent* mSavedMouseMoveEvent;          // +0x0c
    bool mIsGrabbing;                       // +0x10
};

#endif
