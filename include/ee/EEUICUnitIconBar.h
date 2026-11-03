// C:\EEX\Empire Earth\User Interface Thread\EEUICUnitIconBar.h (RECONSTRUCTION; file name inferred). The main game
// form's selected-units icon bar: one button icon per unit of the selected group (or per cargo unit of a lone
// transport), each with a black/red/green health bar; a click selects the unit, ctrl selects its
// type, shift collects a selection, a right click drops it (RTTI EEUICUnitIconBar > UIControl; vtable 0x44a0c8, 34
// slots). EEUICUnitIconBar.cpp is 0x259f7f..0x25b2ce (ctor, ??_G, dtor, then the members, the message ctors after their
// first users, vector<EEButtonObject*>::push_back last). The `return 0` slot (0x1ad67e) and the empty tab slots
// (0x1720c6) are ICF-folded inline overrides. Layout: the ctor's stores; provisional names. Requires lle/UIControl.h,
// lle/UIControlsTable.h, <vector>.
#ifndef EE_EEUICUNITICONBAR_H
#define EE_EEUICUNITICONBAR_H

class EEButtonObject;
class EEComplexUnit;
class GEBitmap;
class UIControlsTable;

class EEUICUnitIconBar : public UIControl {
public:
    // 0x259f7f (the user-defined control factory 0x274b82)
    EEUICUnitIconBar(UIControlsTable& inTable, const GERectangle& inRectangle, UIForm* inForm);
    virtual ~EEUICUnitIconBar();            // 0x25a194
    virtual long CopyData();                // +0x14 0x25a258
    virtual long DrawPolygons();            // +0x18 0x25a32f
    // 0x25a9e2: the icon under the mouse (100: none) (provisional name)
    long GetIconIndex();
    // 0x25a954: the unit shown by icon inIndex (0 for none) (provisional name)
    EEComplexUnit* GetUnit(unsigned long inIndex);
    // 0x25aa43: the ids of the shown units of type inType (provisional name)
    void GetUnitIDs(unsigned long inType, std::vector<long>& outIDs);
    // 0x25b010: refills the icons from the local player's selected group (EEUIFMainGame 0x26814f) (provisional name)
    void Update();
protected:
    virtual long MouseLButtonDown();        // +0x34 (ICF-folded with MouseRButtonDown 0x25ae3b)
    virtual long MouseLButtonUp();          // +0x38 0x25aa9b
    virtual long MouseMove(const GE2DPoint& inLocation);    // +0x40 0x25afa5
    virtual long MouseRButtonDown();        // +0x44 0x25ae3b
    virtual long MouseRButtonUp();          // +0x48 0x25ae44
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}
    virtual long UpdateKeyboard() { return 0; }

    std::vector<EEButtonObject*> mButtons;  // +0x0a0 the icons (Update)
    std::vector<EEButtonObject*> mCopiedButtons;    // +0x0b0 mButtons as of the last CopyData (referenced)
    std::vector<long> mSelectIDs;           // +0x0c0 the shift-collected unit ids
    std::vector<long> mSelectIcons;         // +0x0d0 their icon indices
    std::vector<long> mCopiedSelectIcons;   // +0x0e0
    std::vector<float> mHealth;             // +0x0f0 hit points / maximum per icon
    std::vector<float> mCopiedHealth;       // +0x100
    GERectangle mIconRectangle;             // +0x110 one icon
    GERectangle mCopiedIconRectangle;       // +0x120
    GEBitmap* mIcon;                        // +0x130 (new GEBitmap)
    GEBitmap* mBackground;                  // +0x134 the table's normal bitmap
    float mSpacing;                         // +0x138 the icon step as a fraction of an icon (at most 1)
    float mCopiedSpacing;                   // +0x13c
    bool mShowCargo;                        // +0x140 the icons are the lone selected transport's cargo
    bool mSelecting;                        // +0x141 shift is collecting a selection
};

#endif
