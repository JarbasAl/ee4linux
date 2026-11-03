// Original event RTTI EESEEGameUnitLimit > UIESliderChange > UIEvent.
// vtable443358; actual new0x24 and inherited state only (176be4).
#ifndef EE_EESEEGAMEUNITLIMIT_H
#define EE_EESEEGAMEUNITLIMIT_H
#include "lle/UIESliderChange.h"
class EESEEGameUnitLimit : public UIESliderChange {
public:
    EESEEGameUnitLimit(UITextLabel* inLabel, float inModifier, long inOffset)
        : UIESliderChange(inLabel, inModifier, inOffset) {}
    virtual ~EESEEGameUnitLimit(); //178582; deleting178566
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity); //1af47b
};
#endif
