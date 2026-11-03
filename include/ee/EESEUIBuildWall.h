// C:\EEX\Empire Earth\Scenario Editor\EESEUIBuildWall.h (RECONSTRUCTION; file name inferred).
// The scenario editor's wall and gate placement modes (RTTI EESEUIBuildWall > EEUIBuildWall, vtable 0x444ee8;
// EESEUIBuildGate > EEUIBuildGate, vtable 0x444ecc): by-value members of the unit tab (EESEUnit +0x1c, +0x3c). Their
// Cancel/Build overrides sit at 0x1971ac..0x197261 (before EESEUICCommandUnits.cpp); the ??_G 0x19cbff/0x19cc1c
// (ICF with the bases') close EESEUnit.cpp. Empty inline dtors. Requires ee/EEUIBuildWall.h, ee/EEUIBuildGate.h.
#ifndef EE_EESEUIBUILDWALL_H
#define EE_EESEUIBUILDWALL_H

class EESEUIBuildWall : public EEUIBuildWall {
public:
    virtual ~EESEUIBuildWall() {}         // user-declared (EESEUnit dtor 0x59bb38 member-dtor scheduling)
    virtual void Cancel();                  // +0x04 0x19721b
    virtual void Build();                   // +0x14 0x197202
};

class EESEUIBuildGate : public EEUIBuildGate {
public:
    virtual ~EESEUIBuildGate() {}         // user-declared (as EESEUIBuildWall)
    virtual void Cancel();                  // +0x04 0x1971c8
    virtual void Build(const GE2DPoint& inLocation, long inDirection);   // +0x14 0x1971ac
};

#endif
