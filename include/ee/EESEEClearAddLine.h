// C:\EEX\Empire Earth\Scenario Editor\EESEEClearAddLine.h (RECONSTRUCTION; file name inferred). The change-text
// effect's radio-button event (RTTI EESEEClearAddLine > UIEvent > UReferenceCounter; vtable 0x444008; sizeof 0xc;
// new in EESETSETMediaChangeText::Virtual14 0x1866ab; ??_G 0x1869d1: a user-declared empty dtor). ProcessEvent
// 0x1864d0 hides (button 0x988) or shows the form's controls 0x983 and 0x96d.
// Requires lle/UIEvent.h.
#ifndef EE_EESEECLEARADDLINE_H
#define EE_EESEECLEARADDLINE_H

class EESEEClearAddLine : public UIEvent {
public:
    virtual ~EESEEClearAddLine() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x1864d0
};

#endif
