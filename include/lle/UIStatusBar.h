// C:\EEX\Low-Level Engine\User Interface\UIStatusBar.h (RECONSTRUCTION). A progress bar: a coloured rhombus whose
// width follows a value in [min, max], with an optional centred text (control type 9). UIStatusBar.cpp
// (0x1007db5b..0x1007e312) holds the ctor and the out-of-line members; the inline members (GetRange 0x1006745b,
// SetStatusBarOpacity, SetStatusBarColor, the implicit copy ctor/operator=, ??_E) are emitted first by
// UIControlsTable.cpp. Vtable 0x100998d4 (UIControl's 34 slots, no new virtuals). sizeof 0x114 (copy ctor).
// Requires lle/UIControl.h, lle/GERhombus.h, lle/GEText.h.
#ifndef LLE_UISTATUSBAR_H
#define LLE_UISTATUSBAR_H

#include "lle/UIControl.h"
#include "lle/GEText.h"

class LLE_API UIStatusBar : public UIControl {
public:
    UIStatusBar(unsigned long inId, const GERectangle& inRectangle, UIForm* inForm, unsigned long inIndex,
                unsigned long inBorderThickness);
    virtual ~UIStatusBar();
    virtual void ChangeFont(GEFont* inFont);
    virtual long CopyData();
    virtual long DrawPolygons();
    virtual long DrawTextA();
    long GetCurrentRangeValue() const { return this->mCurrentRangeValue; }  // ICF-folded (0x100633b1)
    GEText* GetGEText() { return this->mText; }                       // ICF-folded (0x1005a104)
    long GetMaxRange() const { return this->mMaxRange; }              // ICF-folded (0x100633db)
    long GetMinRange() const { return this->mMinRange; }              // ICF-folded (0x100660be)
    unsigned long GetRange() const { return this->mMaxRange - this->mMinRange + 1; }  // 0x1006745b
    virtual bool GetTabable() const { return false; }                 // ICF-folded (0x1005efa4)
    void SetCurrentRangeValue(long inValue);
    void SetFocusedTextColor(const GEColor& inColor) { this->mTextColor = inColor; }  // ICF-folded (0x1006040f)
    virtual void SetLocation(const GE2DPoint& inLocation);
    void SetRange(unsigned long inMin, unsigned long inMax);
    // emitted in this order (0x10067469, 0x1006747f)
    void SetStatusBarOpacity(float inOpacity) { this->mStatusRhombus.SetOpacity(inOpacity); }
    void SetStatusBarColor(const GEColor& inColor) { this->mStatusRhombus.SetColor(inColor); }
    void SetText(const UWideString& inText, GEFont* inFont);
    virtual long UpdateKeyboard() { return kErrorOK; }                  // ICF-folded (0x1005d3d1)
protected:
    virtual long MouseLButtonDown() { return kErrorOK; }
    virtual long MouseLButtonUp() { return kErrorOK; }
    virtual long MouseMove(const GE2DPoint& inLocation) { return kErrorOK; }   // ICF-folded (0x1005a023)
    virtual long MouseRButtonDown() { return kErrorOK; }
    virtual long MouseRButtonUp() { return kErrorOK; }
    virtual void TabbedTo() {}
    virtual void TabbedFrom() {}

    GERectangle mTextRectangle;             // +0xa0 mRectangle inset by the border thickness
    GERectangle mCopiedTextRectangle;       // +0xb0 the text rectangle as of the last CopyData (text centred)
    GERhombus mStatusRhombus;               // +0xc0 the bar
    GERhombus mCopiedStatusRhombus;         // +0xd8 the bar as of the last CopyData (inset by the border)
    GEColor mTextColor;                     // +0xf0 (SetFocusedTextColor; provisional name)
    GEText* mText;                          // +0xfc
    GEText* mCopiedText;                    // +0x100
    long mCurrentRangeValue;                // +0x104
    long mMinRange;                         // +0x108
    long mMaxRange;                         // +0x10c
    bool mHasCopiedText;                    // +0x110
};

#endif
