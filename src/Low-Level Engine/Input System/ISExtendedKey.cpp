// C:\EEX\Low-Level Engine\Input System\ISExtendedKey.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10039a8d..0x10039b2a: the ctor and the dtor (everything else is header-inline, emitted by Input System.cpp).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/UWideString.h"
#include "lle/ISExtendedKey.h"

// FUNCTION: LLE 0x10039a8d
// Creates a combined key for a left/right pair (e.g. both Shift keys), named "<key1> and <key2>"
ISExtendedKey::ISExtendedKey(const ISKey* inKey1, const ISKey* inKey2, unsigned long inIndex)
    : mName(inKey1->GetName() + " and " + inKey2->GetName()), mKey1(inKey1), mKey2(inKey2), mIndex(inIndex)
{
}

// FUNCTION: LLE 0x10039b01
// Destroys the combined key
ISExtendedKey::~ISExtendedKey()
{
}
