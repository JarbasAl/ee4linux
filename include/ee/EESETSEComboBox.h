// C:\EEX\Empire Earth\Scenario Editor\EESETSEComboBox.h (RECONSTRUCTION; file name inferred). The effect screen's
// combo box (EESETSEffectControls::mEffectComboBoxes; new 0xe0 = UIComboBox's size: no members of its own; ctor
// 0x182445, EESETSEComboBox.cpp 0x182445..0x182629). Only what the effect types use;
// its own slots are not declared yet. Requires lle/UIComboBox.h.
#ifndef EE_EESETSECOMBOBOX_H
#define EE_EESETSECOMBOBOX_H

class EESETSEComboBox : public UIComboBox {
public:
    // 0x182445: GERectangle(inLocation, inWidth, 20), as EESETSCComboBox's
    EESETSEComboBox(unsigned long inId, unsigned long inTextBoxType, std::vector<UWideString>& inData, GEFont* inFont,
                    const GE2DPoint& inLocation, unsigned long inWidth, unsigned long inMaxListBoxHeight,
                    UIForm* inForm, unsigned long inIndex, unsigned long inMaxChars, unsigned long inBorderThickness);
protected:
    virtual void ChangeSelection();         // +0x8c 0x1824bd (then the effect screen's Virtual44)
};

#endif
