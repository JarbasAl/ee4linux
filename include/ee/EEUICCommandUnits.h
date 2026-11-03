// C:\EEX\Empire Earth\User Interface Thread\EEUICCommandUnits.h (RECONSTRUCTION; file name inferred; first
// sketched by for EESEUICCommandUnits). The main game form's
// right-drag command control: a right drag on the map turns a flat arrow model ("Unit Direction Model", "textures\Unit
// Facing Arrow") to the facing the selected units take; the release hands target and facing to the form's command
// mouse click action (RTTI EEUICCommandUnits > UIControl; vtable 0x449e8c, 35 slots: UIControl's 34 + StartCommand; the scenario
// editor's EESEUICCommandUnits derives from it). EEUICCommandUnits.cpp is 0x257881..0x258464 (ctor, ??_G, dtor, then
// the members in alphabetical order). The `return 0` mouse/keyboard slots (0x1ad67e) and the empty tab slots
// (0x1720c6) are ICF-folded inline overrides. Layout: the ctor's stores; provisional names. Requires lle/UIControl.h,
// ee/EEAction.h, ee/EEAMouseClick.h.
#ifndef EE_EEUICCOMMANDUNITS_H
#define EE_EEUICCOMMANDUNITS_H

class EEAMouseClick;
class GE3DPoint;
class GEModel;
class GETexture;

class EEUICCommandUnits : public UIControl {
public:
    virtual bool GetTabable() const { return false; }   // +0x28 datacmp: slot 10 = 0x4ac9f3 (xor al, al)
    // 0x257881 (the user-defined control factory 0x274931; EESEUICCommandUnits 0x197274): UIControl(0, inId, a 1x1
    // rectangle at gViewport's view rectangle top-left, inForm, inIndex, 0); references inListener (+0xbc)
    EEUICCommandUnits(UIForm* inForm, unsigned long inId, unsigned long inIndex, EEAMouseClick* inListener);
    virtual ~EEUICCommandUnits();           // 0x257971
    float ComputeAngle(const GE3DPoint& inFrom, const GE3DPoint& inTo);   // 0x2579a8
    virtual long CopyData();                // +0x14 0x2579e5
    void CreateModel();                     // 0x257a03
    virtual long DrawPolygons();            // +0x18 0x257c4d
    void LoadTexture();                     // 0x257ece
    // +0x88 0x25829c: a right button press at inLocation (EEUIFMainGame; EESEUICCommandUnits 0x197383)
    virtual void StartCommand(const GE2DPoint& inLocation);
protected:
    virtual long MouseLButtonDown() { return 0; }
    virtual long MouseLButtonUp() { return 0; }
    virtual long MouseLButtonDoublePressed() { return 0; }
    virtual long MouseMove(const GE2DPoint& inLocation);    // +0x40 0x257fa9
    virtual long MouseRButtonDown() { return 0; }
    virtual long MouseRButtonUp();          // +0x48 0x258170
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}
    virtual long UpdateKeyboard() { return 0; }

    GE2DPoint mDownLocation;                // +0x0a0 the press location (rounded)
    GE3DPoint mPosition;                    // +0x0a8 the map point under the press
    float mFacing;                          // +0x0b4 (the editor subclass rotates by it)
    bool mCanFace;                          // +0x0b8 a selected unit is of one of the kinds that take a facing
    bool mHasFacing;                        // +0x0b9 the drag went past the threshold
    EEAMouseClick* mListener;               // +0x0bc (referenced; EEUIFMainGame +0x50c, the mouse click action)
    GEModel* mModel;                        // +0x0c0 (referenced)
    GETexture* mTexture;                    // +0x0c4 (referenced)
    float mDrawnFacing;                     // +0x0c8 mFacing as of the last CopyData
    bool mDrawnHasFacing;                   // +0x0cc
};

#endif
