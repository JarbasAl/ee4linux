// C:\EEX\Empire Earth\Actions\EEACenterCamera.h (RECONSTRUCTION; file name inferred). The center-camera action (RTTI
// EEACenterCamera > EEAction > UReferenceCounter; vtable 0x442ba0; sizeof 0x1c; EEACenterCamera.cpp 0x1de1..0x2086):
// Virtual4 0x1de1 moves the camera to mLocation, or follows the unit mUnitID
// (unit type 0x167); SetLocation(tile) 0x1ee0 / SetUnit 0x1f43 fill it in and post the action. The ctor is inline
// (COMDAT 0x16fad4 at its first user, the scenario editor); ??_G is EEAction's 0x1e1b69. Names are provisional.
// Requires ee/EEAction.h, lle/GE3DPoint.h.
#ifndef EE_EEACENTERCAMERA_H
#define EE_EEACENTERCAMERA_H

#include "ee/EEAction.h"

class EEUnit;

class EEACenterCamera : public EEAction {
public:
    EEACenterCamera() {}                    // inline (0x16fad4: EEAction(), the point's zeros, then the vptr)
    virtual void Virtual4();                // +0x04 0x1de1
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    void SetLocation(long inX, long inY);   // 0x1ee0 (the centre of tile (inX, inY); called from 0x16fb20)
    void SetUnit(EEUnit* inUnit);           // 0x1f43 (called from 0x16fa57)

    GE3DPoint mLocation;                    // +0x0c
    long mUnitID;                           // +0x18 (-1: mLocation; untouched by the ctor)
};

#endif
