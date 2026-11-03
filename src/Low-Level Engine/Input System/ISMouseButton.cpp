// C:\EEX\Low-Level Engine\Input System\ISMouseButton.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1003bafc..0x1003bb89: the ctor, the dtor and CheckForDoublePress (the rest is header-inline, emitted by
// Input System.cpp).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/UWideString.h"
#include "lle/Input System.h"
#include "lle/ISMouseButton.h"

// FUNCTION: LLE 0x1003bafc
// Creates a mouse button with its owner mouse, button index and name, with no press recorded
ISMouseButton::ISMouseButton(ISMouse* inMouse, unsigned long inIndex, const UWideString& inName)
    : mName(inName), mMouse(inMouse), mIndex(inIndex), mLastPressTime(0), mDoublePressed(false)
{
}

// FUNCTION: LLE 0x1003bb33
// Destroys the mouse button (members only)
ISMouseButton::~ISMouseButton()
{
}

// FUNCTION: LLE 0x1003bb4c
// Marks a double press when the button is pressed again within the double-press time of its last press
void ISMouseButton::CheckForDoublePress()
{
    unsigned long theTime = TMGetTime();
    if (this->JustPressed()) {
        if (theTime - this->mLastPressTime < gDoublePressTimeDelta) {
            this->mLastPressTime = 0;
            this->mDoublePressed = true;
        } else {
            this->mDoublePressed = false;
            this->mLastPressTime = theTime;
        }
    } else
        this->mDoublePressed = false;
}
