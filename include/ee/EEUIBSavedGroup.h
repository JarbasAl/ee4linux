// C:\EEX\Empire Earth\User Interface Thread\EEUIBSavedGroup.h (RECONSTRUCTION; file name inferred). The main game
// form's saved-group buttons: the controls with form index 0xd4 + group show the group's
// first unit's portrait (RTTI EEUIBSavedGroup > UIButton; vtable 0x449cb0; sizeof 0x118 from the members).
// EEUIBSavedGroup.cpp is 0x255c2a..0x25619b (ctor, ??_G, dtor, the overrides, then the members in alphabetical order).
// Member names are provisional. Requires lle/UIButton.h, lle/UIControlsTable.h.
#ifndef EE_EEUIBSAVEDGROUP_H
#define EE_EEUIBSAVEDGROUP_H

class EEComplexUnit;
class GETexture;
class UIControlsTable;

class EEUIBSavedGroup : public UIButton {
public:
    // 0x255c2a (the user-defined control factory 0x274b5d)
    EEUIBSavedGroup(UIControlsTable& inTable, const GERectangle& inRectangle, UIForm* inForm);
    virtual ~EEUIBSavedGroup();
    virtual long CopyData();
    virtual long DrawPolygons();
    virtual long MouseLButtonDoublePressed();
    // 0x25607c: shows the group's first unit again (EEUIFMainGame 0x26719d, 0x278cb0; provisional name)
    void Refresh();
    // 0x2560e1: shows inUnit's portrait (drops the old texture at +0x108; provisional name)
    void SetUnit(EEComplexUnit* inUnit);
protected:
    virtual long MouseLButtonUp();
    // ICF-folded with EEUIBColor's (0x2548ac)
    virtual long MouseRButtonDown() { return UIButton::MouseLButtonDown(); }
    virtual long MouseRButtonUp();

    GEBitmap* mPortrait;                    // +0x104 (new GEBitmap; the unit's portrait cell)
    GETexture* mTexture;                    // +0x108 (referenced) the portrait's texture
    float mPortraitX;                       // +0x10c the button object's +0x18/+0x1c as of the last SetUnit
    float mPortraitY;                       // +0x110
    bool mHasUnit;                          // +0x114
    bool mDrawPortrait;                     // +0x115 mHasUnit as of the last CopyData
};

#endif
