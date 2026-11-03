// C:\EEX\Empire Earth\Scenario Editor\EEUIFSEUndo.h (RECONSTRUCTION; file name inferred). The editor's Undo button
// (RTTI EEUIFSEUndo > UIButton; vtable 0x445778, sizeof 0x108; ctor 0x1a4dbe) and its
// action (RTTI EEAUndo > EEAction; vtable 0x445768, sizeof 0xc; ctor inline). Requires lle/UIButton.h, ee/EEAction.h.
#ifndef EE_EEUIFSEUNDO_H
#define EE_EEUIFSEUNDO_H

class EEAUndo : public EEAction {
public:
    virtual void Virtual4();                // +0x04 (0x1a4e8e)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)
};

class EEUIFSEUndo : public UIButton {
public:
    EEUIFSEUndo(unsigned long inId, GEBitmap* inNormalBitmap, GEBitmap* inFocusedBitmap, GEBitmap* inPressedBitmap,
              const GE2DPoint& inPosition, unsigned long inWidth, unsigned long inHeight, UIForm* inForm,
              unsigned long inIndex);
    virtual ~EEUIFSEUndo() { this->mUndoAction->Dereference(); }  // +0x00 (inline: ??_G 0x1a4e5d expands it)
protected:
    virtual void ButtonUp();                // +0x8c (shared by the redo/undo buttons: 0x1a4d95)
private:
    EEAUndo* mUndoAction;                     // +0x104 (new in the ctor; dereferenced by the dtor)
};

#endif
