// C:\EEX\Low-Level Engine\Thread System\TSCriticalSection.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>

#include "lle/LowLevelEngine.h"

// FUNCTION: LLE 0x10053ca1
// Creates an unowned critical section
TSCriticalSection::TSCriticalSection()
{
    this->mLockCount = 0;
    this->mOwnerThread = 0;
    InitializeCriticalSection(&this->mSection);
}

// FUNCTION: LLE 0x10053cb7
// Destroys the critical section
TSCriticalSection::~TSCriticalSection()
{
    DeleteCriticalSection(&this->mSection);
}

// FUNCTION: LLE 0x10053cbf
// Enters the critical section, recording the owning thread on the first entry
void TSCriticalSection::Enter()
{
    EnterCriticalSection(&this->mSection);
    this->mLockCount++;
    if (this->mLockCount == 1)
        this->mOwnerThread = GetCurrentThreadId();
}

// FUNCTION: LLE 0x10053cdd
// Leaves the critical section, clearing the owning thread on the last exit
void TSCriticalSection::Leave()
{
    this->mLockCount--;
    if (this->mLockCount == 0)
        this->mOwnerThread = 0;
    LeaveCriticalSection(&this->mSection);
}
