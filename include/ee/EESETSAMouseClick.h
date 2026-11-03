// C:\EEX\Empire Earth\Scenario Editor\EESETSAMouseClick.h (RECONSTRUCTION; file name inferred). The object screen's
// mouse-click action (RTTI EESETSAMouseClick > EESEAMouseClick > EEAMouseClick > EEAction > UReferenceCounter;
// vtable 0x444624; sizeof 0x10 = the base's: no members). EESETSObject's ctor 0x18efb7
// news it (+0xd4): the base ctor is called out of line (0x19176b, the refused inline `EESEAMouseClick() {}`), then the
// vptr store: an inline implicit ctor. Slot +0x04 0x1917fe calls EESEAMouseClick::Virtual4, then the trigger screen's
// object screen's UpdateSelection (gUIManager +0x130 -> +0x20c -> +0x10 -> +0x10; a tail jump). ??_G 0x191951
// (ICF-shared with EESEAMouseClick's). Names are provisional. Requires ee/EESEAMouseClick.h.
#ifndef EE_EESETSAMOUSECLICK_H
#define EE_EESETSAMOUSECLICK_H

class EESETSAMouseClick : public EESEAMouseClick {
public:
    virtual void Virtual4();                // +0x04 0x1917fe
};

#endif
