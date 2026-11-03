// C:\EEX\Low-Level Engine\User Interface\UIFormManager.h (RECONSTRUCTION). An application object a form owns
// (UIForm::SetFormManager deletes the previous one). Interface only: the vtable (0x10098b00) holds the scalar deleting
// dtor alone; the inline members (ctor 0x100594ef, dtor 0x100594f8, implicit copy ctor 0x100594ff, ??_E 0x1005950a)
// are emitted first by UIBackground.cpp. sizeof 4 (??_E stride).
#ifndef LLE_UIFORMMANAGER_H
#define LLE_UIFORMMANAGER_H

class LLE_API UIFormManager {
public:
    UIFormManager() {}
    virtual ~UIFormManager() {}
};

#endif
