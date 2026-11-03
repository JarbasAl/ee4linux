// C:\EEX\Low-Level Engine\Time Manager\Time Manager.cpp (folder and file name inferred: sorts between
// Thread System.cpp and TMPoll.cpp)
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>
#include <mmsystem.h>

#include "lle/LowLevelEngine.h"

static void TMShutdown(bool inForced);

// Globals in address order (provisional names)
__int64 gCounterFrequency;                  // 0x12ded8 counter ticks per millisecond
float gMicrosecondFrequency;                // 0x12dee0 counter ticks per microsecond
bool gTimeManagerInitialized;               // 0x12dee4
bool gHasPerformanceCounter;                // 0x12dee5

// FUNCTION: LLE 0x10053701
// Returns the current time in milliseconds (performance counter if present, else timeGetTime)
unsigned long TMGetTime()
{
    if (gHasPerformanceCounter) {
        LARGE_INTEGER theCount;
        QueryPerformanceCounter(&theCount);
        return (unsigned long)(theCount.QuadPart / gCounterFrequency);
    }
    return timeGetTime();
}

// FUNCTION: LLE 0x1005373a
// Returns the current time in microseconds from the performance counter (milliseconds from timeGetTime without one)
__int64 TMGetMicroTime()
{
    if (gHasPerformanceCounter) {
        LARGE_INTEGER theCount;
        QueryPerformanceCounter(&theCount);
        return (__int64)(theCount.QuadPart / gMicrosecondFrequency);
    }
    return timeGetTime();
}

// FUNCTION: LLE 0x1005376c
// Fills a time-of-day record from the local clock (month counted from 0)
void TMGetTimeOfDay(TMTime& outTime)
{
    SYSTEMTIME theTime;
    GetLocalTime(&theTime);
    outTime.mSecond = theTime.wSecond;
    outTime.mMinute = theTime.wMinute;
    outTime.mHour = theTime.wHour;
    outTime.mDay = theTime.wDay;
    outTime.mMonth = theTime.wMonth - 1;
    outTime.mYear = theTime.wYear;
}

// FUNCTION: LLE 0x100537ab
// Starts the time manager: 1 ms timer resolution, performance-counter frequencies, and its shutdown registration
long TMInitialize()
{
    LARGE_INTEGER theFrequency;
    timeBeginPeriod(1);
    gTimeManagerInitialized = true;
    gHasPerformanceCounter = QueryPerformanceFrequency(&theFrequency) == TRUE;
    if (gHasPerformanceCounter) {
        gCounterFrequency = theFrequency.QuadPart / 1000;
        gMicrosecondFrequency = theFrequency.QuadPart * 0.000001f;
    }
    return URegisterShutdown(TMShutdown);
}

// FUNCTION: LLE 0x10053810
// Shuts the time manager down, restoring the timer resolution
static void TMShutdown(bool inForced)
{
    timeEndPeriod(1);
}
