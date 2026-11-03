// C:\EEX\Empire Earth\Scenario Editor\EESEEGCPlace.h (RECONSTRUCTION; file name inferred).
// The "place" radio-button event of the game-calamity effect type (RTTI EESEEGCPlace > UIEvent > UReferenceCounter;
// vtable 0x443f34; sizeof 0xc, no members: new 0xc in EESETSETGameCalamity::Virtual10 0x184bd6, trigger event of the
// controls' mRadioButtons[0]/[1] (+0x80/+0x84)). ??_G 0x184d8a (0x2b bytes, three vptr stores: a user-declared empty
// dtor). ProcessEvent 0x184a41 looks up the form's controls 0x96d and 0x999 and calls the cdecl helper 0x184a90
// (bool: the clicked button's id +0x80 == 0x988, the 0x96d control, the 0x999 combo box; EESETSETGame.cpp, probably a
// file-static function), then SetCurrentSelection(0) on the combo; returns 0. Names are provisional. Requires
// lle/UIEvent.h.
#ifndef EE_EESEEGCPLACE_H
#define EE_EESEEGCPLACE_H

class EESEEGCPlace : public UIEvent {
public:
    virtual ~EESEEGCPlace() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x184a41
};

#endif
