// C:\EEX\Empire Earth\Scenario Editor\EEUIFSERedo.h (RECONSTRUCTION; file name inferred). The editor's Redo button
// (RTTI EEUIFSERedo > UIButton; vtable 0x4456d0, sizeof 0x108; ctor 0x1a4cbb) and its
// action (RTTI EEARedo > EEAction; vtable 0x4456c0, sizeof 0xc; ctor inline). Requires lle/UIButton.h, ee/EEAction.h.
#ifndef EE_EEUIFSEREDO_H
#define EE_EEUIFSEREDO_H

class EEARedo : public EEAction {
public:
    virtual void Virtual4();                // +0x04 (0x1a4db3)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
};

class EEUIFSERedo : public UIButton {
public:
    EEUIFSERedo(unsigned long inId, GEBitmap* inNormalBitmap, GEBitmap* inFocusedBitmap, GEBitmap* inPressedBitmap,
              const GE2DPoint& inPosition, unsigned long inWidth, unsigned long inHeight, UIForm* inForm,
              unsigned long inIndex);
    virtual ~EEUIFSERedo();               // +0x00 (??_G 0x1a4d5a calls the out-of-line dtor 0x1a4d76)
protected:
    virtual void ButtonUp();                // +0x8c (shared by the redo/undo buttons: 0x1a4d95)
private:
    EEARedo* mRedoAction;                     // +0x104 (new in the ctor; dereferenced by the dtor)
};

#endif
