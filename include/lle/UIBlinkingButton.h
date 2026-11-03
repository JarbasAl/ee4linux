// C:\EEX\Low-Level Engine\User Interface\UIBlinkingButton.h (RECONSTRUCTION). A UIButton that pulses its
// background, border and bitmap opacities while blinking. UIBlinkingButton.cpp (0x1005db0f..0x1005e47c) emits
// UIButton's and its own inline members first. Vtable 0x10098d44. sizeof 0x148 (??_E stride, 0x1005e061; the TMPoll
// member is 8-aligned at +0x108). Requires lle/UIButton.h, lle/TMPoll.h.
#ifndef LLE_UIBLINKINGBUTTON_H
#define LLE_UIBLINKINGBUTTON_H

#include "lle/UIButton.h"

class LLE_API UIBlinkingButton : public UIButton {
public:
    UIBlinkingButton(unsigned long inId, GEBitmap* inNormalBitmap, GEBitmap* inFocusedBitmap,
                     GEBitmap* inPressedBitmap, const GERectangle& inRectangle, UIForm* inForm, unsigned long inIndex,
                     unsigned long inBorderThickness, unsigned long inBlinkRate);
    // implicit virtual dtor (0x1005e05c: a bare jmp to ~UIButton, no vptr store; exported)
    virtual long CopyData();
    virtual long DrawPolygons();            // ICF-folded with UIButton::DrawPolygons (0x1005eb30)
    virtual long DrawTextA();
    bool GetIsBlinking() { return this->mIsBlinking; }
    void SetIsBlinking(bool inBlinking);
protected:
    TMPoll mPoll;                           // +0x108 one blink phase
    unsigned long mBlinkPhase;              // +0x128 1: fading out, 0: fading in (provisional name)
    unsigned long mBlinkRate;               // +0x12c milliseconds per phase, 50..5000 (0: 250)
    float mPressedOpacity;                  // +0x130 mPressedBitmap's full opacity
    float mNormalOpacity;                   // +0x134 mNormalBitmap's full opacity
    float mBgOpacity;                       // +0x138 the background's full opacity
    float mFullBorderOpacity;               // +0x13c
    bool mSavedAlwaysDirty;                 // +0x140 mAlwaysDirty before blinking
    bool mNeedsOpacities;                   // +0x141 sample the full opacities at the next CopyData
    bool mIsBlinking;                       // +0x142
};

#endif
