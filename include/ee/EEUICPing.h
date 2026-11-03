// C:\EEX\Empire Earth\User Interface Thread\EEUICPing.h (RECONSTRUCTION; file path inferred). The lobby's ping meter
// control: five 10x10 bars 12 pixels apart, the
// first mNumBars + 1 drawn (RTTI EEUICPing > UIControl, vtable 0x449f1c; EEUICPing.cpp 0x258464..0x25861c: ctor,
// ??_G, dtor, DrawPolygons, SetPing). The inline overrides are ICF-folded elsewhere (slot +0x28 0xac9f3, the mouse
// and keyboard slots 0x1ad67e, MouseMove 0x25744c, the tab slots 0x1720c6). Names are provisional. Requires
// lle/UIControl.h, lle/GERhombus.h.
#ifndef EE_EEUICPING_H
#define EE_EEUICPING_H

class EEUICPing : public UIControl {
public:
    EEUICPing(unsigned long inId, const GE2DPoint& inLocation, UIForm* inForm, unsigned long inIndex);   // 0x258464
    virtual ~EEUICPing();                   // +0x00 (??_G 0x25855c, dtor 0x258578)
    virtual long DrawPolygons();            // +0x18 0x2585a6
    virtual bool GetTabable() const { return false; }
    void SetPing(float inPing);             // 0x2585e7 (the lobby passes the slot's +0x1c float)
protected:
    virtual long MouseLButtonDown() { return 0; }
    virtual long MouseLButtonUp() { return 0; }
    virtual long MouseMove(const GE2DPoint& inLocation) { return 0; }
    virtual long MouseRButtonDown() { return 0; }
    virtual long MouseRButtonUp() { return 0; }
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}
    virtual long UpdateKeyboard() { return 0; }

    GERhombus mBars[5];                     // +0x0a0
    unsigned long mNumBars;                 // +0x118 (sizeof 0x11c)
};

#endif
