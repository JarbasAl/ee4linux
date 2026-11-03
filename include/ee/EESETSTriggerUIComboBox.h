// C:\EEX\Empire Earth\Scenario Editor\EESETSTriggerUIComboBox.h (RECONSTRUCTION; file name inferred). The trigger
// sub-screen's condition/effect combo box (RTTI EESETSTriggerUIComboBox > UIComboBox; vtable 0x4447f8, sizeof 0xe4;
// ctor 0x195c35 and ??_G 0x195f98 after their first user, EESETSTrigger's ctor). The ctor stores
// its first argument at +0xe0 after UIComboBox's ctor (1 conditions, 2 effects); vtable slot +0x48 is 0x195cb5.
// Names are provisional. Requires lle/UIComboBox.h.
#ifndef EE_EESETSTRIGGERUICOMBOBOX_H
#define EE_EESETSTRIGGERUICOMBOBOX_H
#include "ee/EEScenarioEditorIDs.h"

class EESETSTriggerUIComboBox : public UIComboBox {
public:
    EESETSTriggerUIComboBox(unsigned long inKind, unsigned long inId, unsigned long inTextBoxType,
                            std::vector<UWideString> inData, GEFont* inFont, const GE2DPoint& inLocation, float inWidth,
                            unsigned long inMaxListBoxHeight, UIForm* inForm, unsigned long inIndex,
                            unsigned long inMaxChars, unsigned long inBorderThickness)
        : UIComboBox(inId, inTextBoxType, inData, inFont, GERectangle(inLocation, inWidth, 20.0f), inMaxListBoxHeight,
                     inForm, inIndex, inMaxChars, inBorderThickness)
    {
        this->mKind = inKind;
    }
    // inline (??_G 0x195f98 clears +0xe0 before the vptr store)
    virtual ~EESETSTriggerUIComboBox() { this->mKind = kSETriggerScreenTrigger; }
    // +0x48 0x195cb5: selects the entry's tab on the trigger screen (EESETSTrigger.cpp)
    virtual long MouseRButtonUp();

    unsigned long mKind;                    // +0xe0
};

#endif
