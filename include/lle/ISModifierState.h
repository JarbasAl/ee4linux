// lle/ISModifierState.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_ISMODIFIERSTATE_H
#define EE_ISMODIFIERSTATE_H

// ISKeyboard::mModifierState bits (ISKeyboard::ComputeModifierState)
enum ISModifierState {
    kModifierShift = 1,  // ComputeModifierState: DIK_LSHIFT/DIK_RSHIFT/0xe1
    kModifierControl = 2,  // ComputeModifierState: DIK_LCONTROL/DIK_RCONTROL/0xe0
    kModifierAlt = 4,    // ComputeModifierState: DIK_LMENU/DIK_RMENU/0xe2
};

#endif // EE_ISMODIFIERSTATE_H
