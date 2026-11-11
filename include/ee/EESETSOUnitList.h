// C:\EEX\Empire Earth\Scenario Editor\EESETSOUnitList.h (RECONSTRUCTION; file name inferred).
// The unit list of the trigger screen's object screen (RTTI EESETSOUnitList > UILBString > UIListBox<UWideString> >
// UIControl > UIEntity; vtable 0x4444a4; sizeof 0x13c = UILBString's: no members; new 0x13c + the imported UILBString
// ctor + the vptr store in EESETSObject's ctor 0x18efb7 (0x190183, id 0x836): an inline ctor forwarding its arguments).
// ChangeSelectionIndex (+0x90) 0x1917b7: the base's with inFireEvent = false, then FireTriggerEvent(0x3eb).
// ??_G 0x1917db stores its own vptr before the imported ~UILBString: a user-declared (empty) dtor.
// Names are provisional. Requires lle/UILBString.h.
#ifndef EE_EESETSOUNITLIST_H
#define EE_EESETSOUNITLIST_H

class EESETSOUnitList : public UILBString {
public:
    // the rectangle is built inside the inline ctor (0x1901f2: the imported GERectangle(const GE2DPoint&, float,
    // float) without a temporary flag; the height unsigned: __ftol, then fild qword)
    EESETSOUnitList(unsigned long inID, std::vector<UWideString>& inStrings, GEFont* inFont, bool inAutoSelect,
                    const GE2DPoint& inLocation, float inWidth, unsigned long inHeight, UIForm* inForm,
                    unsigned long inIndex, unsigned long inBorderThickness)
        : UILBString(inID, inStrings, inFont, inAutoSelect, GERectangle(inLocation, inWidth, (float)inHeight), inForm,
                     inIndex, inBorderThickness) {}
#ifdef EE_PORT
    EESETSOUnitList(unsigned long inID, const std::vector<UWideString>& inStrings, GEFont* inFont, bool inAutoSelect,
                    const GE2DPoint& inLocation, float inWidth, unsigned long inHeight, UIForm* inForm,
                    unsigned long inIndex, unsigned long inBorderThickness)
        : EESETSOUnitList(inID, const_cast<std::vector<UWideString>&>(inStrings), inFont, inAutoSelect, inLocation, inWidth,
                          inHeight, inForm, inIndex, inBorderThickness)
    {
    }
#endif
    virtual ~EESETSOUnitList() {}
    virtual void ChangeSelectionIndex(unsigned long inIndex, bool inFireEvent, bool inScrollTo);   // +0x90 0x1917b7
};

#endif
