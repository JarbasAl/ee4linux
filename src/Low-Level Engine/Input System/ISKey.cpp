// C:\EEX\Low-Level Engine\Input System\ISKey.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10039b2a..0x10039ca1: the two ctors, the dtor and CheckForDoublePress, then UResourceFile's header-inline block
// (the first unit that includes UResourceFile.h).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/UWideString.h"
#include "lle/Input System.h"
#include "lle/ISKey.h"
#include "lle/UResourceFile.h"

// FUNCTION: LLE 0x10039b2a
// Creates a keyboard key with its scan code and display name
ISKey::ISKey(ISKeyboard* inKeyboard, unsigned long inIndex, const UWideString& inName)
    : mName(inName), mKeyboard(inKeyboard), mIndex(inIndex), mLastPressTime(0), mDoublePressed(false)
{
}

// FUNCTION: LLE 0x10039b61
// Creates a copy of a key (name, keyboard, scan code and double-press state)
ISKey::ISKey(const ISKey* inKey)
    : mName(inKey->GetName())
{
    this->mKeyboard = inKey->mKeyboard;
    this->mIndex = inKey->mIndex;
    this->mLastPressTime = inKey->mLastPressTime;
    this->mDoublePressed = inKey->mDoublePressed;
}

// FUNCTION: LLE 0x10039ba4
// Destroys the key
ISKey::~ISKey()
{
}

// FUNCTION: LLE 0x10039bbd
// Updates the key's double-press flag: set when it is pressed again within the double-press delay + 200 ms
void ISKey::CheckForDoublePress()
{
    unsigned long theTime = TMGetTime();
    if (this->JustPressed()) {
        if (theTime - this->mLastPressTime < gDoublePressTimeDelta + 200) {
            this->mLastPressTime = 0;
            this->mDoublePressed = true;
        } else {
            this->mDoublePressed = false;
            this->mLastPressTime = theTime;
        }
    } else
        this->mDoublePressed = false;
}

// Header-inline members emitted here (markers at the end):
// SYNTHETIC: LLE 0x10039c02
// SYNTHETIC: LLE 0x10039c07
// SYNTHETIC: LLE 0x10039c2a
// SYNTHETIC: LLE 0x10039c47
