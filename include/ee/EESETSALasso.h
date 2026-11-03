// C:\EEX\Empire Earth\Scenario Editor\EESETSALasso.h (RECONSTRUCTION; file name inferred).
// The object screen's lasso action (RTTI EESETSALasso > EESEALasso > EEALasso > EEAction > UReferenceCounter; vtable
// 0x444610; sizeof 0x10 = the base's: no members). EESETSObject's ctor 0x18efb7 news it (+0xd8): base ctor out of line
// (0x19177d, the refused inline `EESEALasso() {}`), then the vptr store: an inline implicit ctor. Slot +0x04 0x19181f
// calls EESEALasso::Virtual4, then the object screen's UpdateSelection (as EESETSAMouseClick). ??_G 0x191795 (shared
// with EESEALasso's). Names are provisional. Requires ee/EESEALasso.h.
#ifndef EE_EESETSALASSO_H
#define EE_EESETSALASSO_H

class EESETSALasso : public EESEALasso {
public:
    virtual void Virtual4();                // +0x04 0x19181f
};

#endif
