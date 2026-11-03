// C:\EEX\Empire Earth\Scenario Editor\EEUIFSEESElevation.h (RECONSTRUCTION; file name inferred). The paint-elevation
// slider (RTTI EEUIFSEESElevation > UISlider; vtable 0x445214; ctor 0x1a1397: a vertical slider 20 pixels wide (right
// = left + 19.0f), border 2; ChangeRange 0x1a1437 shows the level in the form's control 0x9f). Requires
// lle/UISlider.h.
#ifndef EE_EEUIFSEESELEVATION_H
#define EE_EEUIFSEESELEVATION_H

#include "lle/UISlider.h"

class EEUIFSEESElevation : public UISlider {
public:
    EEUIFSEESElevation(unsigned long inId, const GE2DPoint& inPosition, unsigned long inHeight, UIForm* inForm,
                       unsigned long inIndex);
    virtual ~EEUIFSEESElevation();
    virtual void ChangeRange(unsigned long inValue);    // slot 0x90
};

#endif
