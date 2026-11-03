// C:\EEX\Low-Level Engine\User Interface\UIEMouseWheelScroll.h (RECONSTRUCTION). The form's default mouse wheel event
// (UIForm::Init news one): forwards the wheel to the control under the mouse. UIEMouseWheelScroll.cpp
// (0x1006de9d..0x1006e304) holds the ctor, dtor and ProcessEvent; the implicit copy ctor is emitted first. Vtable
// 0x10099968. sizeof 0xc. Requires lle/UIEvent.h.
#ifndef LLE_UIEMOUSEWHEELSCROLL_H
#define LLE_UIEMOUSEWHEELSCROLL_H

#include "lle/UIEvent.h"

class LLE_API UIEMouseWheelScroll : public UIEvent {
public:
    UIEMouseWheelScroll();
    virtual ~UIEMouseWheelScroll();
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
