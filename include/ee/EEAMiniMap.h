// C:\EEX\Empire Earth\Actions\EEAMiniMap.h (RECONSTRUCTION; file name inferred). The mini-map click action (RTTI
// EEAMiniMap > EEAction > UReferenceCounter; vtable 0x44b94c; sizeof 0x1c; EEAMiniMap.cpp 0x2a84..0x2fb0): Virtual4
// 0x2a94 dispatches on mMode: 0 centres the camera on the clicked map point
// (CenterCamera 0x2d17), 1 sends the selected group there (SendGroup 0x2aaa: a rally point for a building selection,
// else a path). The ctor is inline (COMDAT 0x276420 at its first user, the mini-map UI); ??_G is EEAction's 0x1e1b69.
// Names are provisional. Requires ee/EEAction.h, lle/GE2DPoint.h.
#ifndef EE_EEAMINIMAP_H
#define EE_EEAMINIMAP_H

#include "ee/EEAction.h"

class EEAMiniMap : public EEAction {
public:
    EEAMiniMap() : mMode(0) {}              // inline (0x276420: EEAction(), the point's zeros, mMode, the vptr)
    virtual void Virtual4();                // +0x04 0x2a94
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    void SendGroup();                       // 0x2aaa (mMode 1)
    void CenterCamera();                    // 0x2d17 (mMode 0)

    GE2DPoint mPoint;                       // +0x0c the mini-map point (the file-static transformation maps it)
    long mMode;                             // +0x14
    // +0x18 the key modifier state at the click (gISKeyboard->GetModifierState(); bit 0: queue, bit 1: run): stored as
    // one dword by EEUIMiniMap::MouseRButtonDown 0x275f51 (was two bitfields)
    unsigned long mModifiers;               // +0x18
};

// 0x4e5ba0 (defined in EEAMiniMap.cpp; the mini-map UI 0x2763df sets it): mini-map point -> map location
// (provisional name)
extern GETransformation gMiniMapTransformation;

#endif
