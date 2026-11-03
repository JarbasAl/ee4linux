// lle/UIButtonState.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_UIBUTTONSTATE_H
#define EE_UIBUTTONSTATE_H

// UIButton::mState: UIButton::GetCurrentBitmap picks the bitmap named here
enum UIButtonState {
    kButtonStateNormal = 0,  // GetCurrentBitmap -> mNormalBitmap (provisional name)
    kButtonStateFocused = 1,  // GetCurrentBitmap -> mFocusedBitmap (MouseMove/TabbedTo) (provisional name)
    kButtonStatePressed = 2,  // GetCurrentBitmap -> mPressedBitmap (ButtonDown) (provisional name)
};

#endif // EE_UIBUTTONSTATE_H
