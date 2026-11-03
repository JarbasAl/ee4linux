// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGAttribute.h (RECONSTRUCTION; file name inferred). The main
// game form's attribute (improvement) button: an EEUIBUpgrade that also draws its level as a
// number (RTTI EEUIFMGAttribute > EEUIBUpgrade; vtable 0x44aa78; sizeof 0x138). EEUIFMGAttribute.cpp is
// 0x26783d..0x267ad9 (ctor, ??_G, dtor, CopyData, DrawTextA, GetIconRectangle). The inline overrides are ICF-folded
// elsewhere (the tab slots 0x1720c6, UpdateKeyboard 0x1ad67e). Names are provisional. Requires ee/EEUIBUpgrade.h.
#ifndef EE_EEUIFMGATTRIBUTE_H
#define EE_EEUIFMGATTRIBUTE_H

class GEText;

class EEUIFMGAttribute : public EEUIBUpgrade {
public:
    // 0x26783d (callers 0x1a526d, 0x274868)
    EEUIFMGAttribute(GEFont* inFont, UIForm* inForm, const GERectangle& inRectangle, unsigned long inIndex,
                     unsigned long inId, const GEColor& inTextColor, bool inScenarioEditor,
                     unsigned long inBorderThickness);
    virtual ~EEUIFMGAttribute();
    virtual long CopyData();
    virtual long DrawTextA();
    // 0x267a6b: the rectangle the status box stretches the button's bitmaps to (EEUIFMGStatusBox 0x26ae16)
    GERectangle GetIconRectangle() const;
protected:
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}
    virtual long UpdateKeyboard() { return 0; }

    GEText* mLevelText;                     // +0x130
    bool mShowLevel;                        // +0x134 mL128 > 0 as of the last CopyData
};

#endif
