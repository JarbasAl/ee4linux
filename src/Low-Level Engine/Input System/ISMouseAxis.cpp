// C:\EEX\Low-Level Engine\Input System\ISMouseAxis.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1003bab2..0x1003bafc: the ctor and the dtor (everything else is header-inline, emitted by Input System.cpp).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UWideString.h"
#include "lle/ISMouseAxis.h"

// FUNCTION: LLE 0x1003bab2
// Creates a mouse axis with a name, the device value it reads and the axis granularity
ISMouseAxis::ISMouseAxis(const UWideString& inName, long* inValue, long& inGranularity)
    : mName(inName), mValue(inValue)
{
    this->mGranularity = inGranularity;
}

// FUNCTION: LLE 0x1003bae3
// Destroys the mouse axis (members only)
ISMouseAxis::~ISMouseAxis()
{
}
