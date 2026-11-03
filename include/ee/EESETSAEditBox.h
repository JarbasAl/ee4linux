// C:\EEX\Empire Earth\Scenario Editor\EESETSAEditBox.h (RECONSTRUCTION; file name inferred).
// An edit box of the trigger screen's object screen (RTTI EESETSAEditBox > UIEditBox > UIControl > UIEntity; vtable
// 0x444564; sizeof 0x1e0 = UIEditBox's: no members; new 0x1e0 + the imported UIEditBox ctor + the vptr store in
// EESETSObject's ctor 0x18efb7 (0x18f42c, 0x18f5f9; ids 0x837, ..): an inline ctor forwarding its arguments).
// ProcessEnterKey (+0x8c) 0x1918da: UIEditBox::ProcessEnterKey, then the object screen's slot +0x24 and its
// UpdateSelection. ??_G 0x191918 calls the imported ~UIEditBox without storing its own vptr: an implicit dtor.
// Names are provisional. Requires lle/UIEditBox.h.
#ifndef EE_EESETSAEDITBOX_H
#define EE_EESETSAEDITBOX_H

class EESETSAEditBox : public UIEditBox {
public:
    // the location, width and height build the rectangle inside the inline ctor (0x18f474: the imported
    // GERectangle(const GE2DPoint&, float, float) without a temporary flag; the height unsigned: fild qword)
    EESETSAEditBox(unsigned long inID, GEFont* inFont, unsigned long inMaxChars, bool inNumbersOnly,
                   const GE2DPoint& inLocation, float inWidth, unsigned long inHeight, UIForm* inForm,
                   unsigned long inIndex, unsigned long inBorderThickness, bool inMultiLine, bool inPasswordBox)
        : UIEditBox(inID, inFont, inMaxChars, inNumbersOnly, GERectangle(inLocation, inWidth, (float)inHeight), inForm,
                    inIndex, inBorderThickness, inMultiLine, inPasswordBox) {}
protected:
    virtual void ProcessEnterKey();         // +0x8c 0x1918da
};

#endif
