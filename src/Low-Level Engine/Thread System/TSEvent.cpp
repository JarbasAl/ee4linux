// C:\EEX\Low-Level Engine\Thread System\TSEvent.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>

#include "lle/LowLevelEngine.h"

// FUNCTION: LLE 0x10053cee
// Wraps an event handle (manual or automatic reset)
TSEvent::TSEvent(HANDLE inHandle, bool inManualReset)
    : mHandle(inHandle), mManualReset(inManualReset)
{
}

// FUNCTION: LLE 0x10053d00
// Destroys the event, closing its handle
TSEvent::~TSEvent()
{
    CloseHandle(this->mHandle);
}

// FUNCTION: LLE 0x10053d09
// Signals the event; records the Windows error on failure
long TSEvent::Set()
{
    long theResult;
    if (SetEvent(this->mHandle))
        theResult = kErrorOK;
    else {
        theResult = kErrorNativeError;
        ESRecordNativeError(GetLastError());
    }
    return theResult;
}

// FUNCTION: LLE 0x10053d2e
// Resets the event to unsignalled; records the Windows error on failure
long TSEvent::Clear()
{
    long theResult;
    if (ResetEvent(this->mHandle))
        theResult = kErrorOK;
    else {
        theResult = kErrorNativeError;
        ESRecordNativeError(GetLastError());
    }
    return theResult;
}

// FUNCTION: LLE 0x10053d53
// Waits for the event up to the timeout; kErrorTimeout on time-out, records the Windows error on failure
long TSEvent::Wait(unsigned long inTimeout)
{
    long theResult;
    unsigned long theWait = WaitForSingleObject(this->mHandle, inTimeout);
    if (theWait == WAIT_TIMEOUT)
        theResult = kErrorTimeout;
    else if (theWait != WAIT_FAILED)
        theResult = kErrorOK;
    else {
        theResult = kErrorNativeError;
        ESRecordNativeError(GetLastError());
    }
    return theResult;
}
