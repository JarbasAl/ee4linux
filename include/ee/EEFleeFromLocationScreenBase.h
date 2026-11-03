// EEFleeFromLocationScreenBase.h (RECONSTRUCTION; file name inferred). The point screens of a
// unit's flee search EEComplexUnit::FindFleePoint 0x1f9c9b (it calls slot +0x04 with each candidate point): RTTI
// EEFleeFromLocationScreenBase (no base; vtable 0x43c028: ??_G 0x64dae, +0x04 the shared `mov al, 1; ret 4` 0x220a18)
// and EEFleeFromLocationScreenCheckPathAreaID > base (vtable 0x43c018: +0x04 0x64d4f, EECPAttackSubGroupUnit.cpp).
// EECPAttackSubGroupUnit::Flee builds the derived one on its stack (8 bytes). Names are provisional.
// Requires lle/GE3DPoint.h (FindFleePoint passes its GE3DPoint candidate straight to +0x04).
#ifndef EE_EEFLEEFROMLOCATIONSCREENBASE_H
#define EE_EEFLEEFROMLOCATIONSCREENBASE_H

class EEFleeFromLocationScreenBase {
public:
    virtual ~EEFleeFromLocationScreenBase() {}
    virtual bool Screen(const GE3DPoint& inPoint) { return true; }   // +0x04
};

class EEFleeFromLocationScreenCheckPathAreaID : public EEFleeFromLocationScreenBase {
public:
    virtual bool Screen(const GE3DPoint& inPoint);   // +0x04 0x64d4f

    unsigned short mAreaID;                 // +0x04
    bool mUseWaterAreas;                    // +0x06 (the continent kind: 2 when set, else 1)
};

#endif
