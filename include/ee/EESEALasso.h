// C:\EEX\Empire Earth\Scenario Editor Actions\EESEALasso.h (RECONSTRUCTION; file name inferred). The
// editor's lasso action (RTTI EESEALasso > EEALasso; vtable 0x4446a0, sizeof 0x10; ctor 0x19177d and ??_G 0x191795
// emitted by their first user). Requires ee/EEALasso.h.
#ifndef EE_EESEALASSO_H
#define EE_EESEALASSO_H

class EESEALasso : public EEALasso {
public:
    EESEALasso() {}
    virtual ~EESEALasso() {}   // user-declared empty (its ??_G stores the vptr first; EESETSObject.cpp)
    virtual void Virtual4();                // +0x04 (0x1a73ef)
    virtual void SelectInRectangle(const GERectangle& inRectangle, long inArg2);  // +0x0c (0x1a745b)
    // 0x1a7358: picks the units in inRectangle; selects them (or, with inDeselect, deselects the selected ones)
    // (provisional names)
    void SelectUnits(const GERectangle& inRectangle, long inUnitType, bool inDeselect, bool inDeselectOnly);
};

#endif
