// C:\EEX\Empire Earth\Scenario Editor Actions\EESEARotateUnits.h (RECONSTRUCTION; file name inferred). The
// editor's "rotate the selected units" action (RTTI EESEARotateUnits > EEAction; vtable 0x444940, sizeof 0x10; ctor
// inline at its users). Requires ee/EEAction.h.
#ifndef EE_EESEAROTATEUNITS_H
#define EE_EESEAROTATEUNITS_H

class EESEARotateUnits : public EEAction {
public:
    virtual void Virtual4();                // +0x04 (0x1aa74f)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
    void Rotate(float inYaw);               // 0x1aa7ec: stores the yaw and posts itself (provisional name)

    float mYaw;                             // +0x0c the yaw
};

#endif
