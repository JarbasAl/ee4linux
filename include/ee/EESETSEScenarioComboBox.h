// C:\EEX\Empire Earth\Scenario Editor\EESETSEScenarioComboBox.h (RECONSTRUCTION; file name inferred). A combo box
// of the scenario's areas or objects (RTTI EESETSEScenarioComboBox > UIComboBox; vtable 0x4438bc, sizeof 0xe8,
// ctor 0x17e291, ??_G 0x17e31a; in EESETSArea.cpp's range). The ctor stores its second
// argument at +0xe0 and its first at +0xe4 after UIComboBox's ctor. Only what matched code uses; names are provisional.
// Requires lle/UIComboBox.h.
#ifndef EE_EESETSESCENARIOCOMBOBOX_H
#define EE_EESETSESCENARIOCOMBOBOX_H

class EESETSEScenarioComboBox : public UIComboBox {
public:
    // 0x17e291: UIComboBox(inId, inTextBoxType, inData, inFont, GERectangle(inLocation, inWidth, 20), ...), then
    // mMode = inMode, mLE4 = inArg1 (EESETSConditionControls' ctor passes 1, 0)
    EESETSEScenarioComboBox(long inScreen, unsigned long inMode, unsigned long inId, unsigned long inTextBoxType,
                            std::vector<UWideString> inData, GEFont* inFont, const GE2DPoint& inLocation, float inWidth,
                            unsigned long inMaxListBoxHeight, UIForm* inForm, unsigned long inIndex,
                            unsigned long inMaxChars, unsigned long inBorderThickness);
    virtual ~EESETSEScenarioComboBox() {}   // inline (??_G 0x17e31a stores the vtable)
protected:
    virtual void ChangeSelection();         // slot 0x8c 0x17ad28
    virtual long MouseRButtonUp();          // slot 0x48 0x17add4 (not written yet)
public:
    unsigned long mMode;                    // +0xe0 (3 objects, 4 areas: EESEEvents' combo helper 0x16fe85)
    long mScreen;                           // +0xe4
};

#endif
