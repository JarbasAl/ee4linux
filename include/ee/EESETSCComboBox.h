// C:\EEX\Empire Earth\Scenario Editor\EESETSCComboBox.h (RECONSTRUCTION; file name inferred). The condition
// controls' combo box (RTTI EESETSCComboBox > UIComboBox; vtable 0x443960, sizeof 0xe0; ctor 0x17e33d; its ??_G is
// the ICF-shared 0x1824df). ChangeSelection (+0x8c 0x17e3b5) calls UIComboBox's, then the
// condition screen's Virtual44 (the auto name). EESETSCComboBox.cpp 0x17daa8..0x17e49e. Requires lle/UIComboBox.h.
#ifndef EE_EESETSCCOMBOBOX_H
#define EE_EESETSCCOMBOBOX_H

class EESETSCComboBox : public UIComboBox {
public:
    // the 19-pixel high list box rectangle at inLocation, inWidth wide (GERectangle(inLocation, inWidth, 20))
    EESETSCComboBox(unsigned long inId, unsigned long inTextBoxType, std::vector<UWideString>& inData, GEFont* inFont,
                    const GE2DPoint& inLocation, unsigned long inWidth, unsigned long inMaxListBoxHeight,
                    UIForm* inForm, unsigned long inIndex, unsigned long inMaxChars, unsigned long inBorderThickness);
protected:
    virtual void ChangeSelection();         // +0x8c 0x17e3b5
};

#endif
