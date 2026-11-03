// C:\EEX\Empire Earth\...\EEAMouseClick.h (RECONSTRUCTION; file path inferred). The mouse click
// action (RTTI EEAMouseClick > EEAction; vtable 0x438dc8, sizeof 0x10; ctor 0x2fb0, dtor 0x2fe4 out of line): holds
// the selection message it builds (+0x0c, referenced). Requires ee/EEAction.h, ee/EEMessage.h.
#ifndef EE_EEAMOUSECLICK_H
#define EE_EEAMOUSECLICK_H

class EEGroup;

class EEAMouseClick : public EEAction {
public:
    EEAMouseClick();
    virtual ~EEAMouseClick();
    virtual void Virtual4();                // +0x04 (0x3009)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    // +0x0c (0x3350, `ret 0x1c`): builds the message for a click; inArg5 set: nothing to do (EESEAMouseClick)
    virtual void Execute(float inX, float inY, bool inArg3, unsigned long inModifiers, bool inIsRotating, float inFacing,
                           bool inArg7);   // floats for args 1/2/6 (EEUICCommandUnits 0x258283)

    // 0x31d3 (ret 4): the click at the mouse position (inModifiers & 1: add to the selection; EEUICSelectUnits::
    // MouseLButtonUp 0x259c9f; provisional name)
    void SelectAtMouse(unsigned long inModifiers);
    // 0x3046 (ret 0xc; Virtual0C 0x3d05): the load-transport message for a group clicked onto inTargetID (inKind: the
    // target's kind flag picks the transport category; provisional names)
    EEMessage* CreateLoadTransportMessage(EEGroup* inGroup, long inKind, unsigned long inTargetID);
    void SetMessage(EEMessage* inMessage) { this->mMessage = inMessage; }  // inline (EEAMouseClick::Execute 0x403350: the original refuses its early message ctors)

    EEMessage* mMessage;                    // +0x0c
};

#endif
