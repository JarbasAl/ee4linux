// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAPaint.h (RECONSTRUCTION; file name inferred).
// The base of the editor's painting actions (RTTI EESEAPaint > EEAction; vtable 0x445928: slot 0 the shared ??_G
// 0x1e1b69 (an implicit destructor, ICF-folded with EEAction's), slots 4/8/0x0c pure). Derived: EESEAPaintElevation
// (ee/EEUIFSEPaintElevation.h), EESEAPaintMoutains and EESEAPaintTerrain (ee/EESEAPaintTerrain.h). The inline ctor is
// emitted expanded except where EEAction's ctor is refused (EESEACreateBlankMap 0x1a59bd: the EEAction COMDAT
// 0x1a6695, then this vptr). Requires ee/EEAction.h.
#ifndef EE_EESEAPAINT_H
#define EE_EESEAPAINT_H

// a queued paint area in tiles (EESEAPaintTerrain/EESEAPaintMoutains' deques: 0x10-byte elements, movsd x4; SetArea
// zeroes a local one first: a zeroing default ctor) (provisional name)
struct EESEAPaintRect {
    EESEAPaintRect() : mLeft(0), mTop(0), mRight(0), mBottom(0) {}
    long mLeft;                             // +0x00
    long mTop;                              // +0x04 (top >= bottom: the loops count y down)
    long mRight;                            // +0x08
    long mBottom;                           // +0x0c
};

// EESEAPaintElevation's queued area (0x14-byte elements, rep movsd 5; SetArea zeroes all five) (provisional name)
struct EESEAPaintRegion {
    EESEAPaintRegion() : mL00(0), mLeft(0), mTop(0), mRight(0), mBottom(0) {}
    long mL00;                              // +0x00 (never set by SetArea)
    long mLeft;                             // +0x04
    long mTop;                              // +0x08
    long mRight;                            // +0x0c
    long mBottom;                           // +0x10
};

class EESEAPaint : public EEAction {
public:
    EESEAPaint() : EEAction(false) {}
    // +0x0c: the area to paint, in tiles (left, top, right, bottom); true when anything changed (provisional name)
    virtual bool SetArea(long inLeft, long inTop, long inRight, long inBottom) = 0;
};

#endif
